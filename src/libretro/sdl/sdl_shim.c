#include "SDL.h"
#include "SDL_mixer.h"
#include "div_lr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIX_RATE 44100
#define MIX_CH 32
#define MIX_CHUNK 512

static char sdl_err[128];
static SDL_Event evq[64];
static int ev_head, ev_tail, ev_count;

static void set_err(const char *s) {
	snprintf(sdl_err, sizeof(sdl_err), "%s", s ? s : "");
}

char *SDL_GetError(void) { return sdl_err; }

int SDL_Init(Uint32 flags) {
	(void)flags;
	return 0;
}

void SDL_Quit(void) {}

void SDL_Delay(Uint32 ms) { (void)ms; }

int SDL_putenv(const char *variable) {
	(void)variable;
	return 0;
}

int SDL_ShowCursor(int toggle) {
	(void)toggle;
	return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
	(void)title;
	(void)icon;
}

void SDL_WarpMouse(Uint16 x, Uint16 y) {
	(void)x;
	(void)y;
}

int SDL_WM_GrabInput(int mode) {
	(void)mode;
	return SDL_GRAB_OFF;
}

int SDL_EnableUNICODE(int enable) {
	(void)enable;
	return 0;
}

int SDL_EnableKeyRepeat(int delay, int interval) {
	(void)delay;
	(void)interval;
	return 0;
}

extern uint32_t OSDEP_GetTicks(void);

Uint32 SDL_GetTicks(void) { return OSDEP_GetTicks(); }

static void mask_shift(Uint32 mask, Uint8 *shift, Uint8 *loss) {
	Uint8 s = 0, bits = 0;
	if (!mask) {
		*shift = 0;
		*loss = 8;
		return;
	}
	while ((mask & 1u) == 0) {
		mask >>= 1;
		s++;
	}
	*shift = s;
	while (mask & 1u) {
		mask >>= 1;
		bits++;
	}
	*loss = (Uint8)(8 - bits);
}

SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth,
	Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask) {
	SDL_Surface *s;
	int bpp;
	if (width < 1)
		width = 1;
	if (height < 1)
		height = 1;
	if (width > 4096)
		width = 4096;
	if (height > 4096)
		height = 4096;
	bpp = depth / 8;
	if (bpp < 1)
		bpp = 1;
	if (bpp > 4)
		bpp = 4;
	if (bpp == 4 && !Rmask && !Gmask && !Bmask) {
		Rmask = 0x00ff0000u;
		Gmask = 0x0000ff00u;
		Bmask = 0x000000ffu;
		Amask = 0xff000000u;
	}
	s = (SDL_Surface *)calloc(1, sizeof(*s));
	if (!s)
		return NULL;
	s->format = (SDL_PixelFormat *)calloc(1, sizeof(*s->format));
	if (!s->format) {
		free(s);
		return NULL;
	}
	s->flags = flags & ~SDL_HWSURFACE;
	s->w = width;
	s->h = height;
	s->pitch = (Uint16)(width * bpp);
	s->pixels = calloc((size_t)height, s->pitch ? s->pitch : 1);
	s->refcount = 1;
	s->format->BitsPerPixel = (Uint8)(bpp * 8);
	s->format->BytesPerPixel = (Uint8)bpp;
	s->format->Rmask = Rmask;
	s->format->Gmask = Gmask;
	s->format->Bmask = Bmask;
	s->format->Amask = Amask;
	s->format->alpha = 255;
	mask_shift(Rmask, &s->format->Rshift, &s->format->Rloss);
	mask_shift(Gmask, &s->format->Gshift, &s->format->Gloss);
	mask_shift(Bmask, &s->format->Bshift, &s->format->Bloss);
	mask_shift(Amask, &s->format->Ashift, &s->format->Aloss);
	if (bpp == 1) {
		s->format->palette = (SDL_Palette *)calloc(1, sizeof(SDL_Palette));
		s->format->palette->ncolors = 256;
		s->format->palette->colors = (SDL_Color *)calloc(256, sizeof(SDL_Color));
	}
	if (!s->pixels || (bpp == 1 && (!s->format->palette || !s->format->palette->colors))) {
		SDL_FreeSurface(s);
		return NULL;
	}
	return s;
}

void SDL_FreeSurface(SDL_Surface *surface) {
	if (!surface)
		return;
	if (surface->format) {
		if (surface->format->palette) {
			free(surface->format->palette->colors);
			free(surface->format->palette);
		}
		free(surface->format);
	}
	free(surface->pixels);
	free(surface);
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags) {
	SDL_Surface *s = SDL_CreateRGBSurface(flags, width, height, bpp ? bpp : 8, 0, 0, 0, 0);
	if (s)
		s->flags |= flags & ~SDL_HWSURFACE;
	return s;
}

SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags) {
	(void)format;
	(void)flags;
	return (SDL_Rect **)(intptr_t)-1;
}

int SDL_LockSurface(SDL_Surface *surface) {
	(void)surface;
	return 0;
}

void SDL_UnlockSurface(SDL_Surface *surface) { (void)surface; }

int SDL_Flip(SDL_Surface *screen) {
	(void)screen;
	return 0;
}

void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	(void)screen;
	(void)x;
	(void)y;
	(void)w;
	(void)h;
}

static void clip_box(SDL_Surface *s, int *x, int *y, int *w, int *h) {
	if (*x < 0) {
		*w += *x;
		*x = 0;
	}
	if (*y < 0) {
		*h += *y;
		*y = 0;
	}
	if (*x + *w > s->w)
		*w = s->w - *x;
	if (*y + *h > s->h)
		*h = s->h - *y;
	if (*w < 0)
		*w = 0;
	if (*h < 0)
		*h = 0;
}

int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color) {
	int x, y, w, h, bpp, row;
	if (!dst || !dst->pixels || !dst->format)
		return -1;
	x = dstrect ? dstrect->x : 0;
	y = dstrect ? dstrect->y : 0;
	w = dstrect ? dstrect->w : dst->w;
	h = dstrect ? dstrect->h : dst->h;
	clip_box(dst, &x, &y, &w, &h);
	bpp = dst->format->BytesPerPixel;
	for (row = 0; row < h; row++) {
		Uint8 *p = (Uint8 *)dst->pixels + (y + row) * dst->pitch + x * bpp;
		int col;
		if (bpp == 1) {
			memset(p, (int)(color & 255), (size_t)w);
			continue;
		}
		for (col = 0; col < w; col++) {
			if (bpp == 4)
				memcpy(p, &color, 4);
			else if (bpp == 2) {
				Uint16 c = (Uint16)color;
				memcpy(p, &c, 2);
			} else {
				p[0] = (Uint8)color;
				p[1] = (Uint8)(color >> 8);
				p[2] = (Uint8)(color >> 16);
			}
			p += bpp;
		}
	}
	return 0;
}

Uint32 SDL_MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b) {
	int i, best, bestd;
	if (!format)
		return 0;
	if (format->BitsPerPixel == 8 && format->palette && format->palette->colors) {
		best = 0;
		bestd = 1 << 30;
		for (i = 0; i < format->palette->ncolors; i++) {
			int dr = (int)format->palette->colors[i].r - r;
			int dg = (int)format->palette->colors[i].g - g;
			int db = (int)format->palette->colors[i].b - b;
			int d = dr * dr + dg * dg + db * db;
			if (d < bestd) {
				bestd = d;
				best = i;
			}
		}
		return (Uint32)best;
	}
	return ((Uint32)(r >> format->Rloss) << format->Rshift) |
		((Uint32)(g >> format->Gloss) << format->Gshift) |
		((Uint32)(b >> format->Bloss) << format->Bshift);
}

void SDL_GetRGB(Uint32 pixel, const SDL_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b) {
	if (!fmt) {
		*r = *g = *b = 0;
		return;
	}
	if (fmt->BitsPerPixel == 8 && fmt->palette && pixel < (Uint32)fmt->palette->ncolors) {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
		return;
	}
	*r = (Uint8)(((pixel & fmt->Rmask) >> fmt->Rshift) << fmt->Rloss);
	*g = (Uint8)(((pixel & fmt->Gmask) >> fmt->Gshift) << fmt->Gloss);
	*b = (Uint8)(((pixel & fmt->Bmask) >> fmt->Bshift) << fmt->Bloss);
}

int SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors) {
	int i;
	(void)flags;
	if (!surface || !surface->format || !surface->format->palette || !colors)
		return 0;
	if (firstcolor < 0)
		firstcolor = 0;
	for (i = 0; i < ncolors; i++) {
		int n = firstcolor + i;
		if (n < 0 || n >= surface->format->palette->ncolors)
			break;
		surface->format->palette->colors[n] = colors[i];
	}
	return 1;
}

int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors) {
	return SDL_SetPalette(surface, SDL_LOGPAL | SDL_PHYSPAL, colors, firstcolor, ncolors);
}

int SDL_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key) {
	if (!surface || !surface->format)
		return -1;
	surface->format->colorkey = key;
	if (flag)
		surface->flags |= SDL_SRCCOLORKEY;
	else
		surface->flags &= ~SDL_SRCCOLORKEY;
	return 0;
}

int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha) {
	if (!surface || !surface->format)
		return -1;
	surface->format->alpha = alpha;
	if (flag)
		surface->flags |= SDL_SRCALPHA;
	else
		surface->flags &= ~SDL_SRCALPHA;
	return 0;
}

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect) {
	int sx, sy, dx, dy, w, h, row, col;
	int sbpp, dbpp;
	if (!src || !dst || !src->pixels || !dst->pixels || !src->format || !dst->format)
		return -1;
	sx = srcrect ? srcrect->x : 0;
	sy = srcrect ? srcrect->y : 0;
	w = srcrect ? srcrect->w : src->w;
	h = srcrect ? srcrect->h : src->h;
	dx = dstrect ? dstrect->x : 0;
	dy = dstrect ? dstrect->y : 0;
	if (sx < 0) {
		w += sx;
		dx -= sx;
		sx = 0;
	}
	if (sy < 0) {
		h += sy;
		dy -= sy;
		sy = 0;
	}
	if (dx < 0) {
		w += dx;
		sx -= dx;
		dx = 0;
	}
	if (dy < 0) {
		h += dy;
		sy -= dy;
		dy = 0;
	}
	if (sx + w > src->w)
		w = src->w - sx;
	if (sy + h > src->h)
		h = src->h - sy;
	if (dx + w > dst->w)
		w = dst->w - dx;
	if (dy + h > dst->h)
		h = dst->h - dy;
	if (w <= 0 || h <= 0)
		return 0;
	sbpp = src->format->BytesPerPixel;
	dbpp = dst->format->BytesPerPixel;
	for (row = 0; row < h; row++) {
		Uint8 *sp = (Uint8 *)src->pixels + (sy + row) * src->pitch + sx * sbpp;
		Uint8 *dp = (Uint8 *)dst->pixels + (dy + row) * dst->pitch + dx * dbpp;
		if (sbpp == dbpp) {
			memcpy(dp, sp, (size_t)w * sbpp);
			continue;
		}
		for (col = 0; col < w; col++) {
			Uint8 r, g, b;
			Uint32 pix = 0, out;
			if (sbpp == 1)
				pix = sp[0];
			else if (sbpp == 4)
				memcpy(&pix, sp, 4);
			else if (sbpp == 2)
				pix = sp[0] | ((Uint32)sp[1] << 8);
			SDL_GetRGB(pix, src->format, &r, &g, &b);
			out = SDL_MapRGB(dst->format, r, g, b);
			if (dbpp == 1)
				dp[0] = (Uint8)out;
			else if (dbpp == 4)
				memcpy(dp, &out, 4);
			else if (dbpp == 2) {
				Uint16 c = (Uint16)out;
				memcpy(dp, &c, 2);
			}
			sp += sbpp;
			dp += dbpp;
		}
	}
	return 0;
}

int SDL_SaveBMP(SDL_Surface *surface, const char *file) {
	(void)surface;
	(void)file;
	set_err("SaveBMP is not implemented");
	return -1;
}

SDL_Surface *SDL_LoadBMP_RW(SDL_RWops *src, int freesrc) {
	if (freesrc)
		SDL_FreeRW(src);
	set_err("LoadBMP is not implemented");
	return NULL;
}

static int rw_close(SDL_RWops *context) {
	SDL_FreeRW(context);
	return 0;
}

SDL_RWops *SDL_RWFromMem(void *mem, int size) {
	SDL_RWops *rw;
	if (!mem || size < 0)
		return NULL;
	rw = (SDL_RWops *)calloc(1, sizeof(*rw));
	if (!rw)
		return NULL;
	rw->hidden.base = (Uint8 *)mem;
	rw->hidden.here = (Uint8 *)mem;
	rw->hidden.stop = (Uint8 *)mem + size;
	rw->close = rw_close;
	return rw;
}

void SDL_FreeRW(SDL_RWops *area) { free(area); }

int SDL_PushEvent(SDL_Event *event) {
	if (!event || ev_count >= (int)(sizeof(evq) / sizeof(evq[0])))
		return -1;
	evq[ev_tail] = *event;
	ev_tail = (ev_tail + 1) % (int)(sizeof(evq) / sizeof(evq[0]));
	ev_count++;
	return 0;
}

int SDL_PollEvent(SDL_Event *event) {
	if (!event || ev_count <= 0)
		return 0;
	*event = evq[ev_head];
	ev_head = (ev_head + 1) % (int)(sizeof(evq) / sizeof(evq[0]));
	ev_count--;
	return 1;
}

int SDL_NumJoysticks(void) { return 0; }
SDL_Joystick *SDL_JoystickOpen(int index) {
	(void)index;
	return NULL;
}
void SDL_JoystickClose(SDL_Joystick *joystick) { (void)joystick; }
const char *SDL_JoystickName(int index) {
	(void)index;
	return "";
}
int SDL_JoystickNumButtons(SDL_Joystick *joystick) {
	(void)joystick;
	return 0;
}
int SDL_JoystickNumAxes(SDL_Joystick *joystick) {
	(void)joystick;
	return 0;
}
int SDL_JoystickNumHats(SDL_Joystick *joystick) {
	(void)joystick;
	return 0;
}
Uint8 SDL_JoystickGetButton(SDL_Joystick *joystick, int button) {
	(void)joystick;
	(void)button;
	return 0;
}
Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis) {
	(void)joystick;
	(void)axis;
	return 0;
}
Uint8 SDL_JoystickGetHat(SDL_Joystick *joystick, int hat) {
	(void)joystick;
	(void)hat;
	return SDL_HAT_CENTERED;
}

/* Mixer. WAV and raw PCM become 16-bit stereo at MIX_RATE. freqEffect in
   divsound.c then pitches that buffer, the same way SDL_mixer did. */

static const char *mix_err = "";
static int audio_open;
static Mix_Channel_Finished_t finished_cb;

typedef struct {
	Mix_Chunk *chunk;
	int playing;
	int volume;
	int left, right;
	Mix_EffectFunc_t effect;
	Mix_EffectDone_t done;
	void *udata;
} MixChan;

static MixChan mix_ch[MIX_CH];

const char *Mix_GetError(void) { return mix_err; }

int Mix_Init(int flags) { return flags; }

static const SDL_version mix_ver = {1, 2, 12};
const SDL_version *Mix_Linked_Version(void) { return &mix_ver; }

int Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize) {
	(void)frequency;
	(void)format;
	(void)channels;
	(void)chunksize;
	audio_open = 1;
	return 0;
}

void Mix_CloseAudio(void) { audio_open = 0; }
void Mix_Quit(void) {}

int Mix_VolumeMusic(int volume) {
	(void)volume;
	return 0;
}

int Mix_AllocateChannels(int numchans) {
	(void)numchans;
	return MIX_CH;
}

static int rd16(const Uint8 *p) { return p[0] | (p[1] << 8); }
static int rd32(const Uint8 *p) {
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static Mix_Chunk *wav_to_chunk(const Uint8 *p, int len) {
	const Uint8 *fmt = NULL, *data = NULL;
	int fmt_sz = 0, data_sz = 0, pos;
	int format, channels, rate, bits, src_frames, out_frames, i, ch;
	int16_t *src, *out;
	Mix_Chunk *chunk;
	double step;
	if (!p || len < 44)
		return NULL;
	if (memcmp(p, "RIFF", 4) != 0 || memcmp(p + 8, "WAVE", 4) != 0)
		return NULL;
	pos = 12;
	while (pos + 8 <= len) {
		int sz = rd32(p + pos + 4);
		const Uint8 *id = p + pos;
		if (sz < 0)
			break;
		pos += 8;
		if (pos + sz > len)
			sz = len - pos;
		if (memcmp(id, "fmt ", 4) == 0) {
			fmt = p + pos;
			fmt_sz = sz;
		} else if (memcmp(id, "data", 4) == 0) {
			data = p + pos;
			data_sz = sz;
		}
		pos += sz + (sz & 1);
	}
	if (!fmt || fmt_sz < 16 || !data || data_sz <= 0)
		return NULL;
	format = rd16(fmt);
	channels = rd16(fmt + 2);
	rate = rd32(fmt + 4);
	bits = rd16(fmt + 14);
	if (format != 1 || channels < 1 || channels > 2 || (bits != 8 && bits != 16) || rate < 1000)
		return NULL;
	src_frames = data_sz / (channels * (bits / 8));
	if (src_frames < 1)
		return NULL;
	src = (int16_t *)malloc((size_t)src_frames * channels * sizeof(int16_t));
	if (!src)
		return NULL;
	if (bits == 8) {
		for (i = 0; i < src_frames * channels; i++)
			src[i] = (int16_t)(((int)data[i] - 128) << 8);
	} else {
		for (i = 0; i < src_frames * channels; i++)
			src[i] = (int16_t)rd16(data + i * 2);
	}
	out_frames = (int)((int64_t)src_frames * MIX_RATE / rate);
	if (out_frames < 1)
		out_frames = 1;
	out = (int16_t *)malloc((size_t)out_frames * 2 * sizeof(int16_t));
	if (!out) {
		free(src);
		return NULL;
	}
	step = (double)rate / (double)MIX_RATE;
	for (i = 0; i < out_frames; i++) {
		double posd = i * step;
		int i0 = (int)posd;
		int i1 = i0 + 1;
		double frac = posd - i0;
		if (i0 >= src_frames)
			i0 = src_frames - 1;
		if (i1 >= src_frames)
			i1 = src_frames - 1;
		for (ch = 0; ch < 2; ch++) {
			int sc = ch < channels ? ch : 0;
			int s0 = src[i0 * channels + sc];
			int s1 = src[i1 * channels + sc];
			out[i * 2 + ch] = (int16_t)(s0 + (int)((s1 - s0) * frac));
		}
	}
	free(src);
	chunk = (Mix_Chunk *)calloc(1, sizeof(*chunk));
	if (!chunk) {
		free(out);
		return NULL;
	}
	chunk->allocated = 1;
	chunk->abuf = (Uint8 *)out;
	chunk->alen = (Uint32)out_frames * 4;
	chunk->volume = 128;
	return chunk;
}

Mix_Chunk *Mix_LoadWAV_RW(SDL_RWops *src, int freesrc) {
	Mix_Chunk *chunk = NULL;
	if (src && src->hidden.base && src->hidden.stop >= src->hidden.base)
		chunk = wav_to_chunk(src->hidden.base, (int)(src->hidden.stop - src->hidden.base));
	if (freesrc)
		SDL_FreeRW(src);
	if (!chunk)
		mix_err = "not a PCM wav";
	return chunk;
}

void Mix_FreeChunk(Mix_Chunk *chunk) {
	if (!chunk)
		return;
	free(chunk->abuf);
	free(chunk);
}

static void halt_one(int i) {
	Mix_EffectDone_t done;
	void *udata;
	if (i < 0 || i >= MIX_CH || !mix_ch[i].playing)
		return;
	mix_ch[i].playing = 0;
	done = mix_ch[i].done;
	udata = mix_ch[i].udata;
	mix_ch[i].effect = NULL;
	mix_ch[i].done = NULL;
	mix_ch[i].chunk = NULL;
	if (done)
		done(i, udata);
	if (finished_cb)
		finished_cb(i);
}

int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops) {
	int i;
	(void)loops;
	if (!chunk)
		return -1;
	if (channel < 0) {
		for (i = 0; i < MIX_CH; i++) {
			if (!mix_ch[i].playing) {
				channel = i;
				break;
			}
		}
	}
	if (channel < 0 || channel >= MIX_CH)
		return -1;
	mix_ch[channel].chunk = chunk;
	mix_ch[channel].playing = 1;
	mix_ch[channel].volume = 128;
	mix_ch[channel].left = 255;
	mix_ch[channel].right = 255;
	mix_ch[channel].effect = NULL;
	mix_ch[channel].done = NULL;
	mix_ch[channel].udata = NULL;
	return channel;
}

int Mix_Playing(int channel) {
	int i, n;
	if (channel >= 0 && channel < MIX_CH)
		return mix_ch[channel].playing ? 1 : 0;
	n = 0;
	for (i = 0; i < MIX_CH; i++)
		n += mix_ch[i].playing ? 1 : 0;
	return n;
}

int Mix_HaltChannel(int channel) {
	int i;
	if (channel < 0) {
		for (i = 0; i < MIX_CH; i++)
			halt_one(i);
		return 0;
	}
	halt_one(channel);
	return 0;
}

int Mix_Volume(int channel, int volume) {
	int prev, i;
	if (volume > 128)
		volume = 128;
	if (channel < 0) {
		prev = mix_ch[0].volume;
		if (volume >= 0) {
			for (i = 0; i < MIX_CH; i++)
				mix_ch[i].volume = volume;
		}
		return prev;
	}
	if (channel >= MIX_CH)
		return 0;
	prev = mix_ch[channel].volume;
	if (volume >= 0)
		mix_ch[channel].volume = volume;
	return prev;
}

int Mix_SetPanning(int channel, Uint8 left, Uint8 right) {
	if (channel < 0 || channel >= MIX_CH)
		return 0;
	mix_ch[channel].left = left;
	mix_ch[channel].right = right;
	return 1;
}

int Mix_RegisterEffect(int chan, Mix_EffectFunc_t f, Mix_EffectDone_t d, void *arg) {
	if (chan < 0 || chan >= MIX_CH)
		return 0;
	mix_ch[chan].effect = f;
	mix_ch[chan].done = d;
	mix_ch[chan].udata = arg;
	return 1;
}

int Mix_UnregisterAllEffects(int channel) {
	if (channel < 0 || channel >= MIX_CH)
		return 0;
	mix_ch[channel].effect = NULL;
	mix_ch[channel].done = NULL;
	mix_ch[channel].udata = NULL;
	return 1;
}

void Mix_ChannelFinished(Mix_Channel_Finished_t channel_finished) {
	finished_cb = channel_finished;
}

Mix_Music *Mix_LoadMUS_RW(SDL_RWops *src) {
	(void)src;
	mix_err = "module music is not decoded";
	return NULL;
}

void Mix_FreeMusic(Mix_Music *music) { (void)music; }

int Mix_PlayMusic(Mix_Music *music, int loops) {
	(void)music;
	(void)loops;
	return -1;
}

int Mix_HaltMusic(void) { return 0; }
int Mix_PlayingMusic(void) { return 0; }

static void mix_n(int16_t *dst, int frames) {
	int32_t acc[MIX_CHUNK * 2];
	int16_t tmp[MIX_CHUNK * 2];
	int i, s, nch;
	nch = frames * 2;
	memset(acc, 0, (size_t)nch * sizeof(acc[0]));
	for (i = 0; i < MIX_CH; i++) {
		int vol, left, right;
		if (!mix_ch[i].playing || !mix_ch[i].effect)
			continue;
		memset(tmp, 0, (size_t)nch * sizeof(tmp[0]));
		mix_ch[i].effect(i, tmp, nch * (int)sizeof(int16_t), mix_ch[i].udata);
		vol = mix_ch[i].volume;
		left = mix_ch[i].left;
		right = mix_ch[i].right;
		for (s = 0; s < nch; s++) {
			int pan = (s & 1) ? right : left;
			acc[s] += (int32_t)tmp[s] * vol * pan / (128 * 255);
		}
	}
	for (s = 0; s < nch; s++) {
		int32_t v = acc[s];
		if (v > 32767)
			v = 32767;
		if (v < -32768)
			v = -32768;
		dst[s] = (int16_t)v;
	}
}

void div_lr_mix_audio(int16_t *dst, int frames) {
	if (!dst || frames <= 0)
		return;
	memset(dst, 0, (size_t)frames * 2 * sizeof(int16_t));
	if (!audio_open)
		return;
	while (frames > 0) {
		int n = frames > MIX_CHUNK ? MIX_CHUNK : frames;
		mix_n(dst, n);
		dst += n * 2;
		frames -= n;
	}
}
