#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <SDL.h>
#include "libretro.h"
#include "unzip.h"
#include "div_lr.h"

static retro_environment_t environ_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_log_printf_t log_cb;

static int booted;
static int64_t audio_usec;
static int geom_w;
static int geom_h;
static int run_count;
static char content_dir[1024];
static char content_exe[1024];

static unsigned pad_prev[2];
static bool pad_bitmasks;

/* RetroArch's Quick Menu > Controls remaps physical buttons onto these.
   The label is the DIV key the button sends. Port 0 is the usual
   arrows/space/ctrl layout. Port 1 is the usual second player (WASD). */
static const struct retro_input_descriptor input_descriptors[] = {
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "Left" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "Right" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "Up" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "Down" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,      "Space" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,      "Ctrl" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,      "Shift" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,      "Alt" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "1" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "2" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,     "3" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,     "4" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START,  "Enter" },
	{ 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT, "Escape" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,   "A" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT,  "D" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,     "W" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,   "S" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,      "2" },
	{ 1, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,      "4" },
	{ 0 },
};
static int game_frames;
static int ft_registered;
static int ft_fps;
static int saw_frame_time;
static int av_fps;

extern double freloj;
extern int dfps;
extern int max_saltos;
extern int get_reloj(void);

static void fallback_log(enum retro_log_level level, const char *fmt, ...) {
	va_list ap;
	(void)level;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}

static void lr_log(const char *msg) {
	if (log_cb)
		log_cb(RETRO_LOG_INFO, "%s\n", msg);
	else
		fprintf(stderr, "%s\n", msg);
}

void retro_init(void) {
	pad_bitmasks = false;
	pad_prev[0] = pad_prev[1] = 0;
	if (environ_cb && environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
		pad_bitmasks = true;
}

void retro_deinit(void) {
	pad_bitmasks = false;
	pad_prev[0] = pad_prev[1] = 0;
}

unsigned retro_api_version(void) { return RETRO_API_VERSION; }

void retro_set_controller_port_device(unsigned port, unsigned device) {
	(void)port;
	(void)device;
}

void retro_get_system_info(struct retro_system_info *info) {
	memset(info, 0, sizeof(*info));
	info->library_name = "DIV Games Studio";
	info->library_version = "2";
	info->valid_extensions = "exe|zip";
	info->need_fullpath = true;
	info->block_extract = true;
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
	info->geometry.base_width = geom_w ? geom_w : 320;
	info->geometry.base_height = geom_h ? geom_h : 200;
	info->geometry.max_width = DIV_LR_MAX_W;
	info->geometry.max_height = DIV_LR_MAX_H;
	info->geometry.aspect_ratio = 0.0f;
	info->timing.fps = (dfps >= 4 && dfps <= 999) ? (double)dfps : 24.0;
	info->timing.sample_rate = 44100.0;
}

void retro_set_environment(retro_environment_t cb) {
	struct retro_log_callback logging;
	environ_cb = cb;
	if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
		log_cb = logging.log;
	else
		log_cb = fallback_log;
	environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, (void *)input_descriptors);
}

void retro_set_audio_sample(retro_audio_sample_t cb) { audio_cb = cb; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }
void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }
void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }

void retro_reset(void) {}
size_t retro_serialize_size(void) { return 0; }
bool retro_serialize(void *data, size_t size) { (void)data; (void)size; return false; }
bool retro_unserialize(const void *data, size_t size) { (void)data; (void)size; return false; }
void retro_cheat_reset(void) {}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {
	(void)index; (void)enabled; (void)code;
}

unsigned retro_get_region(void) { return RETRO_REGION_NTSC; }

void *retro_get_memory_data(unsigned id) { (void)id; return NULL; }
size_t retro_get_memory_size(unsigned id) { (void)id; return 0; }

static void push_key(unsigned sym, int down) {
	SDL_Event e;
	memset(&e, 0, sizeof(e));
	e.type = down ? SDL_KEYDOWN : SDL_KEYUP;
	e.key.state = down ? SDL_PRESSED : SDL_RELEASED;
	e.key.keysym.sym = (SDLKey)sym;
	if (sym > 0 && sym < 128)
		e.key.keysym.unicode = sym;
	SDL_PushEvent(&e);
}

static void keyboard_cb(bool down, unsigned keycode, uint32_t character, uint16_t modifiers) {
	(void)character;
	(void)modifiers;
	if (!booted || keycode == RETROK_UNKNOWN)
		return;
	push_key(keycode, down ? 1 : 0);
}

static int ends_with(const char *s, const char *suf) {
	size_t ns, nf;
	if (!s || !suf)
		return 0;
	ns = strlen(s);
	nf = strlen(suf);
	if (ns < nf)
		return 0;
	return strcasecmp(s + ns - nf, suf) == 0;
}

static const char *base_name(const char *path) {
	const char *s = strrchr(path, '/');
	return s ? s + 1 : path;
}

static int write_all(const char *path, const void *data, size_t len) {
	FILE *f = fopen(path, "wb");
	if (!f)
		return 0;
	if (fwrite(data, 1, len, f) != len) {
		fclose(f);
		return 0;
	}
	fclose(f);
	return 1;
}

/* pack.c trailer: [runtime][game exe][data.div][exesize u32][datsize u32] "DX" */
static int unpack_dx(const char *exe_path, char *out_exe, size_t out_exe_sz) {
	FILE *f;
	unsigned char *buf;
	long len;
	uint32_t exesize, datsize;
	long exe_off, data_off;
	char game_dir[1100];
	char data_path[1200];

	f = fopen(exe_path, "rb");
	if (!f)
		return 0;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	if (len < 12) {
		fclose(f);
		return 0;
	}
	buf = malloc(len);
	if (!buf) {
		fclose(f);
		return 0;
	}
	if (fread(buf, 1, len, f) != (size_t)len) {
		free(buf);
		fclose(f);
		return 0;
	}
	fclose(f);
	if (buf[len - 2] != 'D' || buf[len - 1] != 'X') {
		free(buf);
		return 0;
	}
	memcpy(&exesize, buf + len - 10, 4);
	memcpy(&datsize, buf + len - 6, 4);
	data_off = len - 10 - (long)datsize;
	exe_off = data_off - (long)exesize;
	if (exe_off < 0 || data_off < exe_off || data_off > len - 10) {
		free(buf);
		return 0;
	}

	snprintf(game_dir, sizeof(game_dir), "%s/game", content_dir);
	mkdir(game_dir, 0755);
	snprintf(out_exe, out_exe_sz, "%s/EXEC.EXE", game_dir);
	snprintf(data_path, sizeof(data_path), "%s/data.div", game_dir);
	if (!write_all(out_exe, buf + exe_off, exesize) ||
	    !write_all(data_path, buf + data_off, datsize)) {
		free(buf);
		return 0;
	}
	fprintf(stderr, "div_lr: DX pack exe %u bytes, data.div %u bytes, ver '%c'\n",
		exesize, datsize, exesize > 2 ? buf[exe_off + 2] : '?');
	free(buf);
	return 1;
}

static int extract_zip(const char *zip_path) {
	unzFile z;
	char dest[1200];

	mkdir(content_dir, 0755);
	z = unzOpen(zip_path);
	if (!z) {
		fprintf(stderr, "div_lr: cannot open zip %s\n", zip_path);
		return 0;
	}
	if (unzGoToFirstFile(z) != UNZ_OK) {
		unzClose(z);
		return 0;
	}
	do {
		unz_file_info info;
		char name[512];
		const char *base;
		FILE *out;
		if (unzGetCurrentFileInfo(z, &info, name, sizeof(name), NULL, 0, NULL, 0) != UNZ_OK)
			continue;
		base = base_name(name);
		if (!base[0] || base[strlen(base) - 1] == '/')
			continue;
		snprintf(dest, sizeof(dest), "%s/%s", content_dir, base);
		if (unzOpenCurrentFile(z) != UNZ_OK)
			continue;
		out = fopen(dest, "wb");
		if (out) {
			char chunk[8192];
			int n;
			while ((n = unzReadCurrentFile(z, chunk, sizeof(chunk))) > 0)
				fwrite(chunk, 1, n, out);
			fclose(out);
		}
		unzCloseCurrentFile(z);
	} while (unzGoToNextFile(z) == UNZ_OK);
	unzClose(z);
	return 1;
}

static int find_exe(char *out, size_t out_sz) {
	DIR *d;
	struct dirent *de;
	d = opendir(content_dir);
	if (!d)
		return 0;
	while ((de = readdir(d))) {
		if (de->d_name[0] == '.')
			continue;
		if (ends_with(de->d_name, ".exe") && strcasecmp(de->d_name, "setup.exe") != 0) {
			snprintf(out, out_sz, "%s/%s", content_dir, de->d_name);
			closedir(d);
			return 1;
		}
	}
	closedir(d);
	return 0;
}

static int prepare_content(const char *path) {
	char exe[1100];
	const char *slash;
	snprintf(content_dir, sizeof(content_dir), "/tmp/div-lr");
	mkdir(content_dir, 0755);

	if (ends_with(path, ".zip")) {
		char cmd_note[256];
		snprintf(cmd_note, sizeof(cmd_note), "div_lr: extracting %s", path);
		lr_log(cmd_note);
		if (!extract_zip(path))
			return 0;
		if (!find_exe(exe, sizeof(exe))) {
			lr_log("div_lr: zip has no exe");
			return 0;
		}
	} else {
		strncpy(exe, path, sizeof(exe) - 1);
		exe[sizeof(exe) - 1] = 0;
		slash = strrchr(path, '/');
		if (slash) {
			size_t n = (size_t)(slash - path);
			if (n >= sizeof(content_dir))
				n = sizeof(content_dir) - 1;
			memcpy(content_dir, path, n);
			content_dir[n] = 0;
		} else {
			strcpy(content_dir, ".");
		}
	}

	if (unpack_dx(exe, content_exe, sizeof(content_exe)))
		return 1;

	strncpy(content_exe, exe, sizeof(content_exe) - 1);
	content_exe[sizeof(content_exe) - 1] = 0;
	return 1;
}

static unsigned joypad_bits(unsigned port) {
	unsigned i, bits;
	if (!input_state_cb)
		return 0;
	if (pad_bitmasks)
		return (unsigned)input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
	bits = 0;
	for (i = 0; i <= RETRO_DEVICE_ID_JOYPAD_R3; i++) {
		if (input_state_cb(port, RETRO_DEVICE_JOYPAD, 0, i))
			bits |= 1u << i;
	}
	return bits;
}

static void poll_port(unsigned port, const unsigned *sym_for_id) {
	unsigned bits = joypad_bits(port);
	unsigned id;
	for (id = 0; id <= RETRO_DEVICE_ID_JOYPAD_R3; id++) {
		unsigned mask = 1u << id;
		int held, was;
		if (!sym_for_id[id])
			continue;
		held = (bits & mask) != 0;
		was = (pad_prev[port] & mask) != 0;
		if (held && !was)
			push_key(sym_for_id[id], 1);
		else if (!held && was)
			push_key(sym_for_id[id], 0);
	}
	pad_prev[port] = bits;
}

static void poll_pad(void) {
	static const unsigned port0[RETRO_DEVICE_ID_JOYPAD_R3 + 1] = {
		[RETRO_DEVICE_ID_JOYPAD_B]      = SDLK_SPACE,
		[RETRO_DEVICE_ID_JOYPAD_Y]      = SDLK_LSHIFT,
		[RETRO_DEVICE_ID_JOYPAD_SELECT] = SDLK_ESCAPE,
		[RETRO_DEVICE_ID_JOYPAD_START]  = SDLK_RETURN,
		[RETRO_DEVICE_ID_JOYPAD_UP]     = SDLK_UP,
		[RETRO_DEVICE_ID_JOYPAD_DOWN]   = SDLK_DOWN,
		[RETRO_DEVICE_ID_JOYPAD_LEFT]   = SDLK_LEFT,
		[RETRO_DEVICE_ID_JOYPAD_RIGHT]  = SDLK_RIGHT,
		[RETRO_DEVICE_ID_JOYPAD_A]      = SDLK_LCTRL,
		[RETRO_DEVICE_ID_JOYPAD_X]      = SDLK_LALT,
		[RETRO_DEVICE_ID_JOYPAD_L]      = SDLK_1,
		[RETRO_DEVICE_ID_JOYPAD_R]      = SDLK_2,
		[RETRO_DEVICE_ID_JOYPAD_L2]     = SDLK_3,
		[RETRO_DEVICE_ID_JOYPAD_R2]     = SDLK_4,
	};
	static const unsigned port1[RETRO_DEVICE_ID_JOYPAD_R3 + 1] = {
		[RETRO_DEVICE_ID_JOYPAD_UP]    = SDLK_w,
		[RETRO_DEVICE_ID_JOYPAD_DOWN]  = SDLK_s,
		[RETRO_DEVICE_ID_JOYPAD_LEFT]  = SDLK_a,
		[RETRO_DEVICE_ID_JOYPAD_RIGHT] = SDLK_d,
		[RETRO_DEVICE_ID_JOYPAD_L]     = SDLK_2,
		[RETRO_DEVICE_ID_JOYPAD_R]     = SDLK_4,
	};
	poll_port(0, port0);
	poll_port(1, port1);
}

static void frame_time_cb(retro_usec_t usec) {
	if (usec < 0)
		usec = 0;
	/* A stall (unfocus, load) must not replay a long stretch of game. */
	if (usec > 250000)
		usec = 250000;
	div_lr_add_usec((int64_t)usec);
	audio_usec += (int64_t)usec;
	saw_frame_time = 1;
}

static void install_frame_time(int fps) {
	struct retro_frame_time_callback ft;
	if (fps < 4)
		fps = 24;
	if (fps == ft_fps && ft_registered)
		return;
	memset(&ft, 0, sizeof(ft));
	ft.callback = frame_time_cb;
	ft.reference = (retro_usec_t)(1000000 / fps);
	ft_registered = environ_cb(RETRO_ENVIRONMENT_SET_FRAME_TIME_CALLBACK, &ft) ? 1 : 0;
	ft_fps = fps;
}

static void publish_fps(int fps) {
	struct retro_system_av_info info;
	if (fps < 4)
		fps = 24;
	/* retro_get_system_av_info already reported 24 before the first run. */
	if (av_fps == 0)
		av_fps = 24;
	if (fps == av_fps)
		return;
	retro_get_system_av_info(&info);
	info.timing.fps = (double)fps;
	av_fps = fps;
	if (environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &info))
		fprintf(stderr, "div_lr: set_fps %d frameskip %d\n", fps, max_saltos);
}

static void run_due_frames(void) {
	int budget = max_saltos + 1;
	int ran = 0;
	if (budget < 1)
		budget = 1;
	if (budget > 11)
		budget = 11;
	while (ran < budget && get_reloj() >= (int)freloj) {
		div_lr_frame();
		game_frames++;
		ran++;
	}
}

bool retro_load_game(const struct retro_game_info *game) {
	struct retro_keyboard_callback keys;
	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
	char dir[1024];
	const char *slash;

	if (!game || !game->path)
		return false;
	if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
		lr_log("div_lr: frontend rejected XRGB8888");
	if (!prepare_content(game->path))
		return false;

	strncpy(dir, content_exe, sizeof(dir) - 1);
	dir[sizeof(dir) - 1] = 0;
	slash = strrchr(dir, '/');
	if (slash)
		dir[slash - dir] = 0;
	if (chdir(dir) != 0) {
		fprintf(stderr, "div_lr: chdir %s failed\n", dir);
		return false;
	}
	fprintf(stderr, "div_lr: boot %s in %s\n", content_exe, dir);
	if (div_lr_boot(content_exe) != 0)
		return false;
	booted = 1;
	geom_w = div_lr_width ? div_lr_width : 320;
	geom_h = div_lr_height ? div_lr_height : 200;

	memset(&keys, 0, sizeof(keys));
	keys.callback = keyboard_cb;
	environ_cb(RETRO_ENVIRONMENT_SET_KEYBOARD_CALLBACK, &keys);
	return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num) {
	(void)type; (void)info; (void)num;
	return false;
}

void retro_unload_game(void) {
	fprintf(stderr, "div_lr: ran %d host frames, %d game frames, clock %dms, fps %d skip %d, last %dx%d\n",
		run_count, game_frames, booted ? get_reloj() : 0,
		dfps, max_saltos, div_lr_width, div_lr_height);
	booted = 0;
	ft_registered = 0;
	ft_fps = 0;
	av_fps = 0;
	saw_frame_time = 0;
	game_frames = 0;
}

void retro_run(void) {
	int fps;
	if (input_poll_cb)
		input_poll_cb();
	if (booted) {
		poll_pad();
		fps = (dfps >= 4 && dfps <= 999) ? dfps : 24;
		install_frame_time(fps);
		publish_fps(fps);
		if (!saw_frame_time) {
			if (!ft_registered) {
				audio_usec += (int64_t)(1000000 / fps);
				div_lr_advance_time();
				div_lr_frame();
				game_frames++;
			} else {
				float hz = 60.0f;
				int64_t usec;
				if (!environ_cb(RETRO_ENVIRONMENT_GET_TARGET_REFRESH_RATE, &hz) || hz < 1.0f)
					hz = 60.0f;
				usec = (int64_t)(1000000.0 / hz);
				audio_usec += usec;
				div_lr_add_usec(usec);
				run_due_frames();
			}
		} else {
			run_due_frames();
		}
		saw_frame_time = 0;
		run_count++;
	}
	if (div_lr_width > 0 && div_lr_height > 0) {
		if (div_lr_width != geom_w || div_lr_height != geom_h) {
			struct retro_game_geometry geom;
			geom.base_width = div_lr_width;
			geom.base_height = div_lr_height;
			geom.max_width = DIV_LR_MAX_W;
			geom.max_height = DIV_LR_MAX_H;
			geom.aspect_ratio = 0.0f;
			environ_cb(RETRO_ENVIRONMENT_SET_GEOMETRY, &geom);
			geom_w = div_lr_width;
			geom_h = div_lr_height;
		}
		video_cb(div_lr_pixels, div_lr_width, div_lr_height, div_lr_width * 4);
	} else {
		video_cb(NULL, 0, 0, 0);
	}
	if (audio_batch_cb) {
		enum { MAXF = 11025 };
		static int16_t buf[MAXF * 2];
		int frames;
		if (audio_usec <= 0)
			audio_usec = 1000000 / 60;
		frames = (int)(audio_usec * 44100 / 1000000);
		if (frames < 1)
			frames = 1;
		if (frames > MAXF)
			frames = MAXF;
		audio_usec -= (int64_t)frames * 1000000 / 44100;
		if (audio_usec < 0)
			audio_usec = 0;
		div_lr_mix_audio(buf, frames);
		audio_batch_cb(buf, frames);
	}
	(void)audio_cb;
}
