// *** OJO *** Que el usuario pueda determinar de alguna forma imem_max
//             (o bien el número de procesos máximo)

//═════════════════════════════════════════════════════════════════════════════
// Librerias utilizadas
//═════════════════════════════════════════════════════════════════════════════
#define DEFINIR_AQUI

#ifndef __APPLE__
#include <malloc.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "inter.h"
#include "divsound.h"
#include "cdrom.h"
#include "net.h"
#ifdef DIVDLL
#include "divdll.h"
#endif

// FILE PROTOTYPES
void interprete(void);
void crea_cuad(void);
void system_font(void);
void exec_process(void);
void finalizacion(void);
void elimina_proceso(int id);

// external prototypes
void InitHandler(int);
int  DetectBlaster(int *, int *, int *, int *, int *);
int  DetectGUS(int *, int *, int *, int *, int *);

extern int get_reloj();

void deb(void);

void readmouse(void);
void mouse_window(void);

int dll_loaded = 0;
int trace_program = 0;

int old_dump_type;
int old_restore_type;

int last_key_check, key_check, last_joy_check, joy_check; // Llamar al screensaver
int last_mou_check, mou_check;

//═════════════════════════════════════════════════════════════════════════════
// Critical error handler
//═════════════════════════════════════════════════════════════════════════════
unsigned c_e_0, c_e_1;
unsigned far *c_e_2;

int __far critical_error(unsigned deverr, unsigned errcode, unsigned far *devhdr) {
	c_e_0 = deverr;
	c_e_1 = errcode;
	c_e_2 = devhdr;

	return (_HARDERR_IGNORE);
}

//═════════════════════════════════════════════════════════════════════════════
// Exports
//═════════════════════════════════════════════════════════════════════════════
void CMP_export(char *name, void *dir, int nparms) {
#ifdef DIVDLL
	static int nExt = 0;

	name               = name;
	ExternDirs[nExt++] = dir;
	nparms             = nparms;
#endif
}

void CNT_export(char *name, void *dir, int nparms) {
	name   = name;
	dir    = dir;
	nparms = nparms;
}

//═════════════════════════════════════════════════════════════════════════════
// Programa principal
//═════════════════════════════════════════════════════════════════════════════
int splashtime = 5000;
int oldticks = 0;
int exesize;
int datsize;

void madewith(void);

void mainloop(void) {
	if(splashtime > 0) {
		if(OSDEP_GetTicks() < splashtime) {
			tecla();

			return;
		}
		else {
			splashtime = 0;

			svmode();
		}
	}

	frame_start();

#ifdef DEBUG
	if(kbdFLAGS[_F12] || trace_program) {
		trace_program = 0;
		call_to_debug = 1;
	}
#endif

	old_dump_type    = dump_type;
	old_restore_type = restore_type;

	do {
#ifdef DEBUG
		if(call_to_debug) {
			call_to_debug = 0;

			debug();
		}
#endif

		exec_process();
	}while(ide);

	frame_end();
}

char *jschar;

#ifdef ZLIB
extern int  datastartpos;
extern char exebin[255];
#endif

int main(int argc, char *argv[]) {
	FILE * f;
	char buf[255];
	uint32_t exesize = 0;
	uint32_t datsize = 0;
	uint32_t exestart = 0;
	uint32_t datstart = 0;
	int len=0;

	OSDEP_Init();

	if(OSDEP_NumJoysticks() > 0) {
		divjoy = OSDEP_JoystickOpen(0);

//printf("NUmhats: %d\nNumButtons: %d", SDL_JoystickNumHats(divjoy), SDL_JoystickNumButtons(divjoy));

		if(SDL_JoystickNumHats(divjoy) == 0) {
			SDL_JoystickClose(divjoy);
			divjoy = NULL;
		}
	}

#ifndef DEBUG
	if(1) {
printf("searching for magic\n");

		memset(buf,
		       255,
		       0);

		// search for magic..
		f = fopen(argv[0], "rb");
		if(f) {
			fseek(f,-2, SEEK_END);
			fread(buf, 1, 2, f);

			buf[2] = 0;

			if(!strcmp(buf, "DX")) {
				printf("Found exe\n");
				printf("data and exe packed\n");

				fseek(f, -10, SEEK_END);
				fread(&exesize, 4, 1, f);
				fread(&datsize, 4, 1, f);

				printf("exesize: %d\n",exesize);
				printf("datsize: %d\n",datsize);

				printf("[%c][%c]\n", buf[0], buf[1]);

				fclose(f);
			}
			else {
				printf("failed: %s\n", buf);
			}

			if(exesize == 0) {
				fclose(f);
			}
		}
	}

#ifndef __EMSCRIPTEN__
	if(argc < 2 && exesize == 0) {
		printf("DIV1RUN Run time library - version 1.0 - Freeware by DX Games\n");
		printf("Error: Needs a DIV1RUN executable to load.\n");

		exit(0);
	}
#endif
#else
	vga_an = argc; // Para quitar un warning
#endif

#ifdef DOS
	_harderr(critical_error);
#endif

	vga_an = 320;
	vga_al = 200;

	ireloj     = 100.0 / 24.0;
	max_saltos = 0;
	game_fps   = dfps = 24;

	if((mem = (int *)malloc(4 * imem_max)) != NULL) {
		memset(mem,
		       0,
		       4 * imem_max);

#ifdef __EMSCRIPTEN__
		max_saltos = 2;

//	jschar = emscripten_run_script_string("$('#exename').text()");
//emscripten_wget(jschar, "exe"); //, loadmarvin, errormarvin);

//while(!f) {
		f = fopen(HTML_EXE, "rb");
printf("FILE: %s %x exename: %s\n", HTML_EXE, f, jschar);
//}
#else
		if(exesize == 0) {
			if((f = fopen(argv[1], "rb")) == NULL) {
#ifndef DEBUG
				printf("Error: FILE NOT FOUND.");
#endif
				exit(0);
			}
		}
		else {
			f = fopen(argv[0], "rb");
			//fseek(f, exestart, SEEK_SET);
		}

		fseek(f, 0, SEEK_END);
		len = ftell(f);
		fseek(f, 0, SEEK_SET);

		if(exesize > 0) {
			exestart = len-exesize - datsize - 10;
			fseek(f, exestart, SEEK_SET);
		}

#ifdef ZLIB
		if(datsize > 0) {
			datastartpos = exestart + exesize;

			strcpy(exebin, argv[0]);
		}
#endif
#endif

//printf("exesize: %d datasize: %d exestart: %d datstart: %d filepos: %d\n ",
//       exesize,
//       datsize,
//       exestart,
//       datastartpos,
//       ftell(f));

		fseek(f, 8819, SEEK_CUR);
		fread(mem, 4, imem_max, f);
		fclose(f);

		if((mem[0] & 128) == 128) {
			trace_program = 1;

			mem[0] -= 128;
		}

		if(mem[0] != 0 && mem[0] != 1) {
#ifndef DEBUG
			printf("Error: %d %d Needs a DIV1RUN executable to load.", mem[0], mem[1]);
#endif

			exit(0);
		}

		kbdInit();
		if(mem[0] == 0) {
			DIVInitSound();
		}
		else {
#ifdef DOS
			InitHandler(0);
#endif
		}
#ifdef DOS
		Init_CD();
#endif

		interprete();
	}
	else {
		exer(1);
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Inicialización
//═════════════════════════════════════════════════════════════════════════════
int _mouse_x, _mouse_y;

void MAINSRV_Packet(WORD Usuario, WORD Comando, BYTE *Buffer, WORD Len);
void MAINNOD_Packet(WORD Usuario, WORD Comando, BYTE *Buffer, WORD Len);

extern int find_status;

void inicializacion(void) {
	FILE *f = NULL;
	int n;

	mouse  = (struct _mouse  *)&mem[long_header];
	scroll = (struct _scroll *)&mem[long_header + 12];
	m7     = (struct _m7     *)&mem[long_header + 12 + 10 * 10];
	joy    = (struct _joy    *)&mem[long_header + 12 + 10 * 10 + 10 * 7];
	setup  = (struct _setup  *)&mem[long_header + 12 + 10 * 10 + 10 * 7 + 8];

	if(mem[0] != 1) {
		f = fopen("sound.cfg", "rb");
	}
	if(f != NULL) {
		fread(setup, 4, 8, f);
		fclose(f);
	}
	else {
#ifdef DOS
		DetectBlaster(&setup->card,
		              &setup->port,
		              &setup->irq,
		              &setup->dma,
		              &setup->dma2);
		DetectGUS(&setup->card,
		          &setup->port,
		          &setup->irq,
		          &setup->dma,
		          &setup->dma2);
#endif

		setup->master   = 15;
		setup->sound_fx = 15;
		setup->cd_audio = 15;
	}

	iloc            = mem[2];          // Inicio de la imagen de las variables locales
	iloc_len        = mem[6] + mem[5]; // Longitud de las locales (públicas y privadas)
	if(iloc_len & 1) {
		iloc_len++;
	}
	iloc_pub_len    = mem[6];          // Longitud de las variables locales públicas
	inicio_privadas = iloc_pub_len;
	itxt            = mem[7];          // Inicio de la zona de textos
	imem            = itxt + mem[8];   // Final de código, locales y textos (longitud del cmp)
	if(!(imem & 1)) {
		imem++;
	}

	if((copia = (byte *)malloc(vga_an * vga_al)) == NULL) {
		exer(1);
	}
	memset(copia,
	       0,
	       vga_an * vga_al);

	if((copia2 = (byte *)malloc(vga_an * vga_al)) == NULL) {
		free(copia); // fix memory leak

		exer(1);
	}
	memset(copia2,
	       0,
	       vga_an * vga_al);

#ifdef DEBUG
	if((copia_debug = (char *)malloc(vga_an * vga_al)) == NULL) {
		free(copia);  // fix memory leak
		free(copia2); // fix memory leak

		exer(1);
	}
#endif

	if((ghost = (byte *)malloc(65536)) == NULL) {
		free(copia);       // fix memory leak
		free(copia2);      // fix memory leak
#ifdef DEBUG
		free(copia_debug); // fix memory leak
#endif

		exer(1);
	}

	crea_cuad();

/*
	if((texto = (struct t_texto *)malloc(sizeof(struct t_texto) * max_textos)) == NULL) {
		free(copia);       // fix memory leak
		free(copia2);      // fix memory leak
#ifdef DEBUG
		free(copia_debug); // fix memory leak
#endif
		free(ghost);       // fix memory leak

		exer(1);
	}
*/

	// Crea los dos primeros procesos, init y main
	procesos = 1;
	id_init  = imem;
	id_start = id_end = imem + iloc_len;

	imem += iloc_len;
	mem[iloc + _x1] = -1;

	// *** Init
	memcpy(&mem[id_init],
	       &mem[iloc],
	        iloc_pub_len << 2);
	mem[id_init + _Id    ] = id_init;
	mem[id_init + _IdScan] = id_init;
	mem[id_init + _Status] = 2;
	mem[id_init + _Son   ] = id_start;

	// *** Main
	memcpy(&mem[id_start],
	       &mem[iloc],
	        iloc_pub_len << 2);
	mem[id_start + _Id    ] = id_start;
	mem[id_start + _IdScan] = id_start;
	mem[id_start + _Status] = 2;
	mem[id_start + _Father] = id_init;
	mem[id_start + _IP    ] = mem[1];

	// Regions Init
	for(n = 0; n < max_region; n++) {
		region[n].x0 = 0;
		region[n].y0 = 0;
		region[n].x1 = vga_an;
		region[n].y1 = vga_al;
	}

	// FPGs and MAPs Init
	memset(g,
	       0,
	       sizeof(g));
	if((g[0].grf = (int **)malloc(sizeof(int32_t *) * 2000)) == NULL) {
		free(copia);       // fix memory leak
		free(copia2);      // fix memory leak
#ifdef DEBUG
		free(copia_debug); // fix memory leak
#endif
		free(ghost);       // fix memory leak

		exer(1);
	}
	memset(g[0].grf,
	       0,
	       sizeof(int32_t *) * 2000);

	next_map_code = 1000;

	// FNTs and Texts Init
	memset(fonts,
	       0,
	       sizeof(fonts));
	memset(texto,
	       0,
	       sizeof(texto));

	system_font();

#ifdef DOS
	_bios_timeofday(_TIME_GETCLOCK, (long *)&ip);
	init_divrnd(ip);
#endif

//	_setvideomode(_MRES256COLOR);

	// Internal Inits
	svmode();
	mouse_on();
	//cbd.mouse_action = 0;
	mouse_window();
	_mouse_x = mouse->x;
	_mouse_y = mouse->y;

	// Palette and Ghost tables
	memset(&paleta[0],
	        0,
	        768);
	memset(&dac[0],
	        0,
	        768);

	dacout_r       = 0;
	dacout_g       = 0;
	dacout_b       = 0;
	dacout_speed   = 8;
  now_dacout_r   = 64;
	now_dacout_g   = 64;
	now_dacout_b   = 64;
	paleta_cargada = 0;

	set_dac();
	//tabla_ghost();

	// Mouse Init
	mouse->z    = -512;
	mouse->x    = vga_an / 2;
	mouse->y    = vga_al / 2;
	mouse->size = 100;

	// Scroll and Mode7 Init (En un principio no hay sistema de scroll, ni de m7)
	for(n = 0; n < 10; n++) {
		(scroll + n)->z       = 512;
		(scroll + n)->ratio   = 200;
		(scroll + n)->region1 = -1;
		(scroll + n)->region2 = -1;

		(m7 + n)->z        = 256;
		(m7 + n)->height   = 32;
		(m7 + n)->distance = 64;
		(m7 + n)->focus    = 256;
	}

	get_reloj();
	ultimo_reloj = 0;
	freloj       = ireloj = 5.5;
	max_saltos   = 0;

#ifdef __EMSCRIPTEN__
	max_saltos = 2;
#endif

	game_fps    = 24;
	joy_timeout = 0;

#ifdef DEBUG
	debugger_step  = 0;
	call_to_debug  = 0;
	process_stoped = 0;
#endif

	saltar_volcado    = 0;
	volcados_saltados = 0;

	init_sin_cos(); // Tablas de seno y coseno para el modo7

#ifdef DEBUG
	init_debug();

	new_palette =0;
	new_mode   = 0;
#endif

	init_volcado();

#ifdef DIVDLL
	// DLL_3 Exportación de funciones y variables (para utilizarlas en las DLL)

	// Exportadas desde 'C'
	// files
	DIV_export("div_fopen"   ,(void *)fopen   );
	DIV_export("div_fclose"  ,(void *)fclose  );
	DIV_export("div_malloc"  ,(void *)malloc  );
	DIV_export("div_free"    ,(void *)free    );
	DIV_export("div_rand"    ,(void *)rand);
	DIV_export("div_text_out",(void *)text_out);

	// variables publicas
	DIV_export("mem"         , (void *)mem);
	DIV_export("stack"       , (void *)&pila);
	DIV_export("sp"          , (void *)&sp);
	DIV_export("process_size", (void *)&iloc_len);

	DIV_export("id_offset"      , (void *)&id);
	DIV_export("id_init_offset" , (void *)&id_init);
	DIV_export("id_start_offset", (void *)&id_start);
	DIV_export("id_end_offset"  , (void *)&id_end);

	DIV_export("wide"      , (void *)&vga_an);
	DIV_export("height"    , (void *)&vga_al);
	DIV_export("buffer"    , (void *)&copia);
	DIV_export("background", (void *)&copia2);

	DIV_export("ghost"         , (void *)&ghost);
	DIV_export("palette"       , (void *)paleta);
	DIV_export("active_palette", (void *)dac);
	DIV_export("set_palette"   , (void *)&activar_paleta);

	DIV_export("ss_time"  , (void *)&ss_time);
	DIV_export("ss_status", (void *)&ss_status);
	DIV_export("ss_exit"  , (void *)&ss_exit);

	DIV_export("key", (void *)kbdFLAGS);

	// Screensaver
	ss_time         = 3000;
	ss_time_counter = 0;
	ss_status       = 1;

	activar_paleta = 0;

	// Exports
	COM_export = CNT_export;
	LookForAutoLoadDlls();
	COM_export = CMP_export;
#endif

#ifdef NETLIB
	inicializacion_red = 0;

	SRV_Packet = MAINSRV_Packet;
	NOD_Packet = MAINNOD_Packet;
#endif

	find_status = 0;
}

//═════════════════════════════════════════════════════════════════════════════
//  Crea la tabla de cuadrados
//═════════════════════════════════════════════════════════════════════════════
void crea_cuad(void) {
	int a, b;

	if((cuad = (byte *)malloc(16384)) == NULL) {
		exer(1);
	}

	a = 0;
	do {
		b = 0;
		do {
			*(int *)(cuad + a * 4 * 64 + b * 4) = (a > b) ? (a - b) * (a - b) : (b - a) * (b - a);
		}while(++b < 64);
	}while(++a < 64);
}

//═════════════════════════════════════════════════════════════════════════════
//      Carga el font del systema
//═════════════════════════════════════════════════════════════════════════════
#include "06x08.h"

byte *sys06x08 = NULL;

void system_font(void) {
	int n, m;
	int *ptr;
	byte *si,*di, x;

	if((sys06x08 = (byte *)malloc(12288)) == NULL) {
		exer(1);
	}

	si = (byte *)_06x08;
	di = sys06x08;
	for(n = 0; n < 1536; n++) {
		x = *si++;
		for(m = 0; m < 8; m++) {
			if(x & 128) { *di++ = 1;         }
			else        { *di++ = 0; x *= 2; }
		}
	}

	n = 1356 + sizeof(t_fnt_table) * 256 + 12288;
	if((fonts[0] = (byte *)malloc(n)) == NULL) {
		free(sys06x08); // fix memory leak

		exer(1);
	}

	memset(fonts[0],
	       0,
	       n);
	memcpy(fonts[0] + n - 12288,
	       sys06x08,
	       12288);

	ptr = (int32_t *)(fonts[0] + 1356);
	for(n = 0; n < 256; n++) {
		*ptr++ = 6;
		*ptr++ = 8;
		*ptr++ = 0;
		*ptr++ = 1356 + sizeof(t_fnt_table) * 256 + n * 48;
	}

	last_c1 = 1;

	f_i[0].ancho     = 6;
	f_i[0].espacio   = 3;
	f_i[0].espaciado = 0;
	f_i[0].alto      = 8;
}

//═════════════════════════════════════════════════════════════════════════════
// Intérprete del código generado
//═════════════════════════════════════════════════════════════════════════════
int max ,max_reloj; // Para procesar según _Priority y pintar según _Z
extern int alt_x;

void interprete (void) {
	inicializacion();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(mainloop, 0, 0);
#else
	while(procesos && !(kbdFLAGS[_ESC] && kbdFLAGS[_L_CTRL]) && !(alt_x)) {
		mainloop();
	}
	finalizacion();
#endif
}

//═════════════════════════════════════════════════════════════════════════════
// Procesa el siguiente proceso
//═════════════════════════════════════════════════════════════════════════════
void exec_process(void) {
	ide = 0;
	max = 0x80000000;

#ifdef DEBUG
	if(process_stoped) {
		id = ide = process_stoped;

		process_stoped = 0;

		goto continue_process;
	}
#endif

	id = id_old;
	do {
		if(mem[id + _Status  ] == 2 &&
		  !mem[id + _Executed]      &&
		   mem[id + _Priority] > max) {
			ide = id;
			max = mem[id + _Priority];
		}

		if(id == id_end) {
			id = id_start;
		}
		else {
			id += iloc_len;
		}
	}while(id != id_old);

	if(ide) {
		if(mem[ide + _Frame] >= 100) {
			mem[ide + _Frame   ] -= 100;
			mem[ide + _Executed]  = 1;
		}
		else {
#ifdef NETLIB
			net_receive(); // Recibe los paquetes justo antes de ejecutar el proceso
#endif

			id       = ide;
			ip       = mem[id + _IP];
			sp       = 64;
			pila[64] = 0;

#ifdef DEBUG
continue_process:
#endif
			max_reloj = get_reloj() + max_process_time;
//printf("running bytecode: %d %d\n", ip + 1, mem[ip + 1]);
//printf("process entered at %d\n", ip);
			do {
//if((byte)mem[ip + 1] > 0) { printf("running bytecode: %d %d\n", ip, (byte)mem[ip]); }
//if(ip > 18819) { printf("fucking hell\n"); }
				switch(mem[ip++]) {
					case DIVBC_nop: {
						break;
					}
					case DIVBC_car: {
						pila[++sp] = mem[ip++];
						break;
					}
					case DIVBC_asi: {
//printf("LASI: %d %d %d %d\n", ip - 1, pila[sp], pila[sp - 1], mem[pila[sp - 1]]);
						pila[sp - 1] = mem[pila[sp - 1]] = pila[sp];
//printf("LASI: %d %d %d %d\n", ip - 1, pila[sp], pila[sp - 1], mem[pila[sp - 1]]);
						sp--;
						break;
					}
					case DIVBC_ori: {
						pila[sp - 1] |= pila[sp];
						sp--;
						break;
					}
					case DIVBC_xor: {
						pila[sp - 1] ^= pila[sp];
						sp--;
						break;
					}
					case DIVBC_and: {
						pila[sp - 1] &= pila[sp];
						sp--;
						break;
					}
					case DIVBC_igu: {
						pila[sp - 1] = pila[sp - 1] == pila[sp];
						sp--;
						break;
					}
					case DIVBC_dis: {
						pila[sp - 1] = pila[sp - 1] != pila[sp];
						sp--;
						break;
					}
					case DIVBC_may: {
						pila[sp - 1] = pila[sp - 1] > pila[sp];
						sp--;
						break;
					}
					case DIVBC_men: {
						pila[sp - 1] = pila[sp - 1] < pila[sp];
						sp--;
						break;
					}
					case DIVBC_mei: {
						pila[sp - 1] = pila[sp - 1] <= pila[sp];
						sp--;
						break;
					}
					case DIVBC_mai: {
						pila[sp - 1] = pila[sp - 1] >= pila[sp];
						sp--;
						break;
					}
					case DIVBC_add: {
						pila[sp - 1] += pila[sp];
						sp--;
						break;
					}
					case DIVBC_sub: {
						pila[sp - 1] -= pila[sp];
						sp--;
						break;
					}
					case DIVBC_mul: {
						pila[sp - 1] *= pila[sp];
						sp--;
						break;
					}
					case DIVBC_div: {
#ifdef DEBUG
						if(pila[sp] == 0) {
							pila[--sp] = 0;

							v_function = -2;

							e(e145);

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						pila[sp - 1] /= pila[sp];
						sp--;
						break;
					}
					case DIVBC_mod: {
#ifdef DEBUG
						if(pila[sp] == 0) {
							pila[--sp] = 0;

							v_function = -2;

							e(e145);

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						pila[sp - 1] %= pila[sp];
						sp--;
						break;
					}
					case DIVBC_neg: {
						pila[sp] = -pila[sp];
						break;
					}
					case DIVBC_ptr: {
						pila[sp] = mem[pila[sp]];
						break;
					}
					case DIVBC_not: {
						pila[sp] ^= -1;
						break;
					}
					case DIVBC_aid: {
						pila[sp] += id;
						break;
					}
					case DIVBC_cid: {
						pila[++sp] = id;
						break;
					}
					case DIVBC_rng: {
#ifdef DEBUG
						if(pila[sp] < 0 || pila[sp] > mem[ip]) {
							v_function = -2;

							e(e140);

							if(call_to_debug) {
								ip++;

								process_stoped = id;

								return;
							}
						}
#endif
						ip++;
						break;
					}
					case DIVBC_jmp: {
						ip = mem[ip];

#ifdef DEBUG
						if(get_reloj() > max_reloj) {
							v_function = -2;

							e(e142);

							max_reloj = max_process_time + get_reloj();

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						break;
					}
					case DIVBC_jpf: {
						if(pila[sp--] & 1) {
							ip++;
						}
						else {
							ip = mem[ip];
						}

#ifdef DEBUG
						if(get_reloj() > max_reloj) {
							v_function = -2;

							e(e142);

							max_reloj = max_process_time + get_reloj();

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						break;
					}
					case DIVBC_fun: {
						function();

#ifdef DEBUG
						if(call_to_debug) {
							process_stoped = id;

							return;
						}
#endif
						break;
					}
					case DIVBC_cal: {
//printf("Spawning new process %d\n", ip);
						pila[++sp] = id2 = id;
						pila[++sp] = ip + 1;

						if(sp > long_pila) {
							exer(3);
						}

						procesos++;

						ip = mem[ip];
						id = id_start;
						while(mem[id + _Status] && id <= id_end) {
							id += iloc_len;
						}
						if(id > id_end) {
							if(id > imem_max - iloc_len) {
								exer(2);
							}

							id_end = id;
						}

						memcpy(&mem[id],
						       &mem[iloc],
						        iloc_pub_len << 2);
						mem[id + _Id    ] = id;
						mem[id + _Status] = 2;
						if(mem[id + _BigBro] = mem[id2 + _Son]) {
							mem[mem[id + _BigBro] + _SmallBro] = id;
						}
						mem[id  + _Father] = id2;
						// Father
						mem[id2 + _Son   ] = id;
						break;
					}
					case DIVBC_ret: {
						elimina_proceso(id);

						bp        = sp;
						sp       -= mem[ip] + 1;
						ip        = pila[bp--];
						bp        = pila[bp];
						pila[sp]  = id;

						if(!ip) {
							id = bp;

							goto next_process1;
						}
						else {
							mem[id + _Executed] = 1;

							id = bp;
						}
						break;
					}
					case DIVBC_asp: {
						sp--;
						break;
					}
					case DIVBC_frm:{
						mem[id + _IdScan] = 0;
						mem[id + _BlScan] = 0;
						mem[id + _IP    ] = ip + 1;

						bp        = sp;
						sp       -= mem[ip] + 1;
						ip        = pila[bp--];
						bp        = pila[bp];
						pila[sp]  = id;

						if(!ip) {
							id = bp;

							goto next_process1;
						}
						else {
							mem[id + _Executed] = 1;

							id = bp;
						}
						break;
					}
					case DIVBC_cbp: {
						mem[id + _Param] = sp - mem[ip++] - 1;
						break;
					}
					case DIVBC_cpa: {
						mem[pila[sp--]] = pila[mem[id + _Param]++];
						break;
					}
					case DIVBC_typ: {
						mem[id + _Bloque] = mem[ip++];

						inicio_privadas = mem[6];
						break;
					}
					case DIVBC_pri: {
						memcpy(&mem[id + inicio_privadas],
						       &mem[ip + 1],
						       (mem[ip] - ip - 1) << 2);

						ip = mem[ip];
						break;
					}
					case DIVBC_cse: {
						if(pila[sp - 1] == pila[sp]) {
							ip++;
						}
						else {
							ip = mem[ip];
						}
						sp--;
						break;
					}
					case DIVBC_csr: {
						if(pila[sp - 2] >= pila[sp - 1] && pila[sp - 2] <= pila[sp]) {
							ip++;
						}
						else {
							ip = mem[ip];
						}
						sp -= 2;
						break;
					}
					case DIVBC_shr: {
						pila[sp - 1] >>= pila[sp];
						sp--;
						break;
					}
					case DIVBC_shl: {
						pila[sp - 1] <<= pila[sp];
						sp--;
						break;
					}
					case DIVBC_ipt: {
						pila[sp] = ++mem[pila[sp]];
						break;
					}
					case DIVBC_pti: {
						pila[sp] = mem[pila[sp]]++;
						break;
					}
					case DIVBC_dpt: {
						pila[sp] = --mem[pila[sp]];
						break;
					}
					case DIVBC_ptd: {
						pila[sp] = mem[pila[sp]]--;
						break;
					}
					case DIVBC_ada: {
						pila[sp - 1] = mem[pila[sp - 1]] += pila[sp];
						sp--;
						break;
					}
					case DIVBC_sua: {
						pila[sp - 1] = mem[pila[sp - 1]] -= pila[sp];
						sp--;
						break;
					}
					case DIVBC_mua: {
						pila[sp - 1] = mem[pila[sp - 1]] *= pila[sp];
						sp--;
						break;
					}
					case DIVBC_dia: {
#ifdef DEBUG
						if(pila[sp] == 0) {
							mem[pila[sp - 1]] = 0;

							pila[--sp] = 0;

							v_function = -2;

							e(e145);

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						pila[sp - 1] = mem[pila[sp - 1]] /= pila[sp];
						sp--;
						break;
					}
					case DIVBC_moa: {
#ifdef DEBUG
						if(pila[sp] == 0) {
							mem[pila[sp - 1]] = 0;

							pila[--sp] = 0;

							v_function = -2;

							e(e145);

							if(call_to_debug) {
								process_stoped = id;

								return;
							}
						}
#endif
						pila[sp - 1] = mem[pila[sp - 1]] %= pila[sp];
						sp--;
						break;
					}
					case DIVBC_ana: {
						pila[sp - 1] = mem[pila[sp - 1]] &= pila[sp];
						sp--;
						break;
					}
					case DIVBC_ora: {
						pila[sp - 1] = mem[pila[sp - 1]] |= pila[sp];
						sp--;
						break;
					}
					case DIVBC_xoa: {
						pila[sp - 1] = mem[pila[sp - 1]] ^= pila[sp];
						sp--;
						break;
					}
					case DIVBC_sra: {
						pila[sp - 1] = mem[pila[sp - 1]] >>= pila[sp];
						sp--;
						break;
					}
					case DIVBC_sla: {
						pila[sp - 1] = mem[pila[sp - 1]] <<= pila[sp];
						sp--;
						break;
					}
					case DIVBC_par: {
						inicio_privadas = mem[6] + mem[ip++];
						break;
					}
					case DIVBC_rtf: {
						elimina_proceso(id);

						bp  = --sp;
						sp -= mem[ip] + 1;
						ip  = pila[bp--];

						if(!ip) {
							id       = pila[bp];
							pila[sp] = pila[bp + 2];

							goto next_process1;
						}
						else {
							mem[id + _Executed] = 1;

							id       = pila[bp];
							pila[sp] = pila[bp + 2];
						}
						break;
					}
					case DIVBC_clo: {
						procesos++;

						id2 = id;
						id  = id_start;
						while(mem[id + _Status] && id <= id_end) {
							id += iloc_len;
						}
						if(id > id_end) {
							if(id > imem_max - iloc_len) {
								exer(2);
							}

							id_end = id;
						}

						memcpy(&mem[id],
						       &mem[id2],
						        iloc_len << 2);
						mem[id + _Id] = id;
						mem[id + _IP] = ip + 1;
						if(mem[id + _BigBro] = mem[id2 + _Son]) {
							mem[mem[id + _BigBro] + _SmallBro] = id;
						}
						mem[id  + _SmallBro] = 0;
						mem[id  + _Son     ] = 0;
						mem[id  + _Father  ] = id2;
						// Father
						mem[id2 + _Son     ] = id;

						id = id2;
						ip = mem[ip];
						break;
					}
					case DIVBC_frf: {
						mem[id + _IdScan]  = 0;
						mem[id + _BlScan]  = 0;
						mem[id + _Frame ] += pila[sp--];
						mem[id + _IP    ]  = ip + 1;

						bp        = sp;
						sp       -= mem[ip] + 1;
						ip        = pila[bp--];
						bp        = pila[bp];
						pila[sp]  = id;

						if(!ip) {
							id = bp;

							goto next_process2;
						}
						else {
							if(mem[id + _Frame] >= 100) {
								mem[id + _Frame   ] -= 100;
								mem[id + _Executed]  = 1;
							}

							id = bp;
						}
						break;
					}
					case DIVBC_imp: {
#ifdef DIVDLL
						if((pe[nDLL] = DIV_LoadDll((char *)&mem[itxt + mem[ip++]]) )== NULL) {
							exer(4);
						}
						else {
							nDLL++;
						}
#else
						ip++;
//printf("limp\n");
#endif
						break;
					}
					case DIVBC_ext: {
#ifdef DIVDLL
						call((voidReturnType)ExternDirs[mem[ip++]]);
#else
						ip++;
//printf("lext\n");
#endif
						break;
					}
					case DIVBC_chk: {
						if(pila[sp] < id_init || pila[sp] > id_end || pila[sp] != mem[pila[sp]]) {
							v_function = -2;

							e(e141);
#ifdef DEBUG
							if(call_to_debug) {
								process_stoped = id;

								return;
							}
#endif
						}
						break;
					}
					case DIVBC_dbg: {
#ifdef DEBUG
						for(ibreakpoint = 0; ibreakpoint < max_breakpoint; ibreakpoint++) {
							if(breakpoint[ibreakpoint].line > -1 && breakpoint[ibreakpoint].offset == ip - 1) {
								break;
							}
						}
						// Se llegó a un breakpoint
						if(ibreakpoint<max_breakpoint) {
							mem[--ip] = breakpoint[ibreakpoint].code;

							breakpoint[ibreakpoint].line = -1;

							call_to_debug  = 1;
							process_stoped = id;

							return;
						}

						deb();

						if(call_to_debug) {
							return;
						}
#endif
						break;
					}
//					#include "cases.h"
				}
			}while(1);

next_process1:
			mem[ide + _Executed] = 1;

next_process2:
			;
		}
	}

	id = ide;

	if(post_process != NULL) {
		post_process();
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Procesa la siguiente instruccion del siguiente proceso
//═════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG
void trace_process(void) {
	ide = 0;
	max = 0x80000000;

	if(process_stoped) {
		id = ide = process_stoped;

		process_stoped = 0;

		goto continue_process;
	}

	id = id_old;
	do {
		if(mem[id + _Status  ] == 2 &&
		  !mem[id + _Executed]      &&
		   mem[id + _Priority] > max) {
			ide = id;
			max = mem[id + _Priority];
		}

		if(id == id_end) {
			id = id_start;
		}
		else {
			id += iloc_len;
		}
	}while(id != id_old);

	if(ide)
		if(mem[ide + _Frame] >= 100) {
			mem[ide + _Frame   ] -= 100;
			mem[ide + _Executed]  = 1;
		}
		else {
#ifdef NETLIB
			net_receive(); // Recibe los paquetes justo antes de ejecutar el proceso
#endif

			id       = ide;
			ip       = mem[id + _IP];
			sp       = 64;
			pila[64] = 0;

continue_process:
			max_reloj = get_reloj() + max_process_time;

			switch(mem[ip++]) {
				case DIVBC_nop: {
					break;
				}
				case DIVBC_car: {
					pila[++sp] = mem[ip++];
					break;
				}
				case DIVBC_asi: {
					pila[sp - 1] = mem[pila[sp - 1]] = pila[sp];
					sp--;
					break;
				}
				case DIVBC_ori: {
					pila[sp - 1] |= pila[sp];
					sp--;
					break;
				}
				case DIVBC_xor: {
					pila[sp - 1] ^= pila[sp];
					sp--;
					break;
				}
				case DIVBC_and: {
					pila[sp - 1] &= pila[sp];
					sp--;
					break;
				}
				case DIVBC_igu: {
					pila[sp - 1] = pila[sp - 1] == pila[sp];
					sp--;
					break;
				}
				case DIVBC_dis: {
					pila[sp - 1] = pila[sp-1] != pila[sp];
					sp--;
					break;
				}
				case DIVBC_may: {
					pila[sp - 1] = pila[sp - 1] > pila[sp];
					sp--;
					break;
				}
				case DIVBC_men: {
					pila[sp - 1] = pila[sp - 1] < pila[sp];
					sp--;
					break;
				}
				case DIVBC_mei: {
					pila[sp - 1] = pila[sp - 1] <= pila[sp];
					sp--;
					break;
				}
				case DIVBC_mai: {
					pila[sp - 1] = pila[sp - 1] >= pila[sp];
					sp--;
					break;
				}
				case DIVBC_add: {
					pila[sp - 1] += pila[sp];
					sp--;
					break;
				}
				case DIVBC_sub: {
					pila[sp - 1] -= pila[sp];
					sp--;
					break;
				}
				case DIVBC_mul: {
					pila[sp - 1] *= pila[sp];
					sp--;
					break;
				}
				case DIVBC_div: {
					if(pila[sp] == 0) {
						pila[--sp] = 0;

						v_function = -2;

						e(e145);

						if(call_to_debug) {
							process_stoped = id;

							return;
						}
					}
					else {
						pila[sp - 1] /= pila[sp];
						sp--;
					}
					break;
				}
				case DIVBC_mod: {
					if(pila[sp] == 0) {
						pila[--sp] = 0;

						v_function = -2;

						e(e145);

						if(call_to_debug) {
							process_stoped = id;

							return;
						}
					}
					else {
						pila[sp - 1] %= pila[sp];
						sp--;
					}
					break;
				}
				case DIVBC_neg: {
					pila[sp] = -pila[sp];
					break;
				}
				case DIVBC_ptr: {
					pila[sp] = mem[pila[sp]];
					break;
				}
				case DIVBC_not: {
					pila[sp] ^= -1;
					break;
				}
				case DIVBC_aid: {
					pila[sp] += id;
					break;
				}
				case DIVBC_cid: {
					pila[++sp] = id;
					break;
				}
				case DIVBC_rng: {
					if(pila[sp] < 0 || pila[sp] > mem[ip]) {
						v_function = -2;

						e(e140);

						if(call_to_debug) {
							ip++;

							process_stoped = id;

							return;
						}
					}

					ip++;
					break;
				}
				case DIVBC_jmp: {
					ip = mem[ip];

					if(get_reloj() > max_reloj) {
						v_function = -2;

						e(e142);

						max_reloj = max_process_time + get_reloj();

						if(call_to_debug) {
							process_stoped = id;

							return;
						}
					}
					break;
				}
				case DIVBC_jpf: {
					if(pila[sp--] & 1) {
						ip++;
					}
					else {
						ip = mem[ip];
					}

					if(get_reloj() > max_reloj) {
						v_function = -2;

						e(e142);

						max_reloj = max_process_time + get_reloj();

						if(call_to_debug) {
							process_stoped = id;

							return;
						}
					}
					break;
				}
				case DIVBC_fun: {
					function();

					if(call_to_debug) {
						process_stoped = id;

						return;
					}
					break;
				}
				case DIVBC_cal: {
//printf("Spawning new process %d\n", ip);
					pila[++sp] = id2 = id;
					pila[++sp] = ip + 1;

					if(sp > long_pila) {
						exer(3);
					}

					procesos++;

					ip = mem[ip];
					id = id_start;
					while(mem[id + _Status] && id <= id_end) {
						id += iloc_len;
					}
					if(id > id_end) {
						if(id > imem_max - iloc_len) {
							exer(2);
						}

						id_end = id;
					}

					memcpy(&mem[id],
					        mem[iloc],
					        iloc_pub_len << 2);
					mem[id + _Id    ] = id;
					mem[id + _Status] = 2;
					if(mem[id + _BigBro] = mem[id2 + _Son]) {
						mem[mem[id + _BigBro] + _SmallBro] = id;
					}
					mem[id  + _Father] = id2;
					// Father
					mem[id2 + _Son   ] = id;
					break;
				}
				case DIVBC_ret: {
					elimina_proceso(id);

					bp        = sp;
					sp       -= mem[ip] + 1;
					ip        = pila[bp--];
					bp        = pila[bp];
					pila[sp]  = id;

					if (!ip) {
						id = bp;

						goto next_process1;
					}
					else {
						mem[id + _Executed] = 1;

						id = bp;
					}
					break;
				}
				case DIVBC_asp: {
					sp--;
					break;
				}
				case DIVBC_frm: {
					mem[id + _IdScan] = 0;
					mem[id + _BlScan] = 0;
					mem[id + _IP]     = ip + 1;

					bp        = sp;
					sp       -= mem[ip] + 1;
					ip        = pila[bp--];
					bp        = pila[bp];
					pila[sp]  = id;

					if(!ip) {
						id = bp;

						goto next_process1;
					}
					else {
						mem[id + _Executed] = 1;

						id = bp;
					}
					break;
				}
				case DIVBC_cbp: {
					mem[id + _Param] = sp - mem[ip++] - 1;
					break;
				}
				case DIVBC_cpa: {
					mem[pila[sp--]] = pila[mem[id + _Param]++];
					break;
				}
				case DIVBC_typ: {
					mem[id + _Bloque] = mem[ip++];

					inicio_privadas = mem[6];
					break;
				}
				case DIVBC_pri: {
					memcpy(&mem[id + inicio_privadas],
					       &mem[ip + 1],
					       (mem[ip] - ip - 1) << 2);

					ip = mem[ip];
					break;
				}
				case DIVBC_cse: {
					if(pila[sp - 1] == pila[sp]) {
						ip++;
					}
					else {
						ip = mem[ip];
					}
					sp--;
					break;
				}
				case DIVBC_csr: {
					if(pila[sp - 2] >= pila[sp - 1] && pila[sp - 2] <= pila[sp]) {
						ip++;
					}
					else {
						ip = mem[ip];
					}

					sp -= 2;
					break;
				}
				case DIVBC_shr: {
					pila[sp - 1] >>= pila[sp];
					sp--;
					break;
				}
				case DIVBC_shl: {
					pila[sp - 1] <<= pila[sp];
					sp--;
					break;
				}
				case DIVBC_ipt: {
					pila[sp] = ++mem[pila[sp]];
					break;
				}
				case DIVBC_pti: {
					pila[sp] = mem[pila[sp]]++;
					break;
				}
				case DIVBC_dpt: {
					pila[sp] = --mem[pila[sp]];
					break;
				}
				case DIVBC_ptd: {
					pila[sp] = mem[pila[sp]]--;
					break;
				}
				case DIVBC_ada: {
					pila[sp - 1] = mem[pila[sp - 1]] += pila[sp];
					sp--;
					break;
				}
				case DIVBC_sua: {
					pila[sp - 1] = mem[pila[sp - 1]] -= pila[sp];
					sp--;
					break;
				}
				case DIVBC_mua: {
					pila[sp - 1] = mem[pila[sp - 1]] *= pila[sp];
					sp--;
					break;
				}
				case DIVBC_dia: {
					pila[sp - 1] = mem[pila[sp - 1]] /= pila[sp];
					sp--;
					break;
				}
				case DIVBC_moa: {
					pila[sp - 1] = mem[pila[sp - 1]] %= pila[sp];
					sp--;
					break;
				}
				case DIVBC_ana: {
					pila[sp - 1] = mem[pila[sp - 1]] &= pila[sp];
					sp--;
					break;
				}
				case DIVBC_ora: {
					pila[sp - 1] = mem[pila[sp - 1]] |= pila[sp];
					sp--;
					break;
				}
				case DIVBC_xoa: {
					pila[sp - 1] = mem[pila[sp - 1]] ^= pila[sp];
					sp--;
					break;
				}
				case DIVBC_sra: {
					pila[sp - 1] = mem[pila[sp - 1]] >>= pila[sp];
					sp--;
					break;
				}
				case DIVBC_sla: {
					pila[sp - 1] = mem[pila[sp - 1]] <<= pila[sp];
					sp--;
					break;
				}
				case DIVBC_par: {
					inicio_privadas = mem[6] + mem[ip++];
					break;
				}
				case DIVBC_rtf: {
					elimina_proceso(id);

					bp  = --sp;
					sp -= mem[ip] + 1;
					ip  = pila[bp--];

					if(!ip) {
						id       = pila[bp];
						pila[sp] = pila[bp + 2];

						goto next_process1;
					}
					else {
						mem[id + _Executed] = 1;

						id       = pila[bp];
						pila[sp] = pila[bp + 2];
					}
					break;
				}
				case DIVBC_clo: {
					procesos++;

					id2 = id;
					id  = id_start;
					while(mem[id + _Status] && id <= id_end) {
						id += iloc_len;
					}
					if(id > id_end) {
						if(id > imem_max - iloc_len) {
							exer(2);
						}

						id_end = id;
					}

					memcpy(&mem[id],
					       &mem[id2],
					        iloc_len << 2);
					mem[id + _Id] = id;
					mem[id + _IP] = ip + 1;
					if(mem[id + _BigBro] = mem[id2 + _Son]) {
						mem[mem[id + _BigBro] + _SmallBro] = id;
					}
					mem[id  + _SmallBro] = 0;
					mem[id  + _Son     ] = 0;
					mem[id  + _Father  ] = id2;
					// Father
					mem[id2 + _Son     ] = id;

					id = id2;
					ip = mem[ip];
					break;
				}
				case DIVBC_frf: {
					mem[id + _IdScan]  = 0;
					mem[id + _BlScan]  = 0;
					mem[id + _Frame ] += pila[sp--];
					mem[id + _IP    ] = ip + 1;

					bp        = sp;
					sp       -= mem[ip] + 1;
					ip        = pila[bp--];
					bp        = pila[bp];
					pila[sp]  = id;

					if(!ip) {
						id = bp;

						goto next_process2;
					}
					else {
						if(mem[id + _Frame] >= 100) {
							mem[id + _Frame   ] -= 100;
							mem[id + _Executed]  = 1;
						}

						id = bp;
					}
					break;
				}
				case DIVBC_imp: {
#ifdef DIVDLL
					if((pe[nDLL] = DIV_LoadDll((byte *)&mem[itxt + mem[ip++]])) == NULL) {
						exer(4);
					}
					else {
						nDLL++;
					}
#else
					ip++;
//printf("limp\n");
#endif
					break;
				}
				case DIVBC_ext: {
#ifdef DIVDLL
					call((unsigned int)ExternDirs[mem[ip++]]);
#else
					ip++;
//printf("lext\n");
#endif
					break;
				}
				case DIVBC_chk: {
#ifdef DEBUG
					if(pila[sp] < id_init || pila[sp] > id_end || pila[sp] != mem[pila[sp]]) {
						v_function =- 2;

						e(e141);

						if(call_to_debug) {
							process_stoped = id;

							return;
						}
					}
#endif
					break;
				}
				case DIVBC_dbg: {
					for(ibreakpoint = 0; ibreakpoint < max_breakpoint; ibreakpoint++) {
						if(breakpoint[ibreakpoint].line > -1 && breakpoint[ibreakpoint].offset == ip - 1) {
							break;
						}
					}
					// Se llegó a un breakpoint
					if(ibreakpoint < max_breakpoint) {
						mem[--ip] = breakpoint[ibreakpoint].code;

						breakpoint[ibreakpoint].line = -1;
					}

					call_to_debug  = 1;
					process_stoped = id;
					return;
				}
			}

			process_stoped = id;

			goto end_trace;

next_process1:
			mem[ide + _Executed] = 1;

next_process2:
			;
		}
	}

	if(post_process != NULL) {
		post_process();
	}

end_trace:
	id = ide;
}
#endif

//═════════════════════════════════════════════════════════════════════════════
// Inicia un frame, prepara variables vuelca y espera timer
//═════════════════════════════════════════════════════════════════════════════
float ffps = 18.2;

void frame_start(void) {
	int n;

	// Control del screen saver
	if(ss_status && ss_frame != NULL) {
		if(get_reloj() > ss_time_counter) {
			if(ss_init != NULL) {
				ss_init();
			}

			ss_exit = 0;
			do {
				// Keyboard
				key_check = 0;
				for(n = 0; n < 128; n++) {
					if(key(n)) {
						key_check++;
					}
				}
				if(key_check != last_key_check) {
					ss_exit = 1;
				}

				// Mouse
				readmouse();

				mou_check = mouse->x     +
				            mouse->y     +
				            mouse->left  +
				            mouse->right +
				            mouse->middle;
				if(mou_check != last_mou_check) {
					ss_exit = 2; // Fix: before was 1
				}
//				if(cbd.mouse_action) {
//					ss_exit = 2;
//				}

				// Joystick
				if(joy_status) {
					read_joy();

					joy_check = joy->button1 +
					            joy->button2 +
					            joy->left    +
					            joy->right   +
					            joy->up      +
					            joy->down;
					if(joy_check != last_joy_check) {
						ss_exit = 3;
					}
				}

				ss_frame();

				volcado_completo = 1;

				volcado(copia);
			}while(!ss_exit);

			if(ss_end != NULL) {
				ss_end();
			}

			memcpy(copia,
			       copia2,
			       vga_an * vga_al);
			volcado_parcial(0,
			                0,
			                vga_an,
			                vga_al);

			ss_time_counter = get_reloj() + ss_time;
		}
	}

	// Elimina los procesos muertos
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		if(mem[ide + _Status] == 1) {
			elimina_proceso(ide);
		}
	}

	// Si se está haciendo un fade lo continúa
	if(now_dacout_r != dacout_r ||
	   now_dacout_g != dacout_g ||
	   now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();

		fading = 1;
	}
	else {
		if(activar_paleta) {
			set_paleta();
			set_dac();

			activar_paleta--;
		}
		fading = 0;
	}

	// Actualiza los timers
	for(max = 0; max < 10; max++) {
		timer(max) += get_reloj() - ultimo_reloj;
	}

	// Actualiza la variable "fps"
	if(get_reloj() > ultimo_reloj) {
		ffps = (ffps * 49.0 + 100.0 / (float)(get_reloj() - ultimo_reloj)) / 50.0;
		fps  = (int)ffps;
	}

	ultimo_reloj = get_reloj();

	// Acrualiza el audio
	//DIVLoopSound();

	// Actualiza el teclado
	ascii     = 0;
	scan_code = 0;

	tecla();

	// Permite comerse hasta un tercio del sgte frame
	if(get_reloj() > (freloj + ireloj / 3)) {
		if(volcados_saltados < max_saltos) {
			volcados_saltados += 1;
			saltar_volcado     = 1;
			freloj            += ireloj;
		}
		else {
			volcados_saltados  = 0;
			saltar_volcado     = 0;
			freloj             = (float)get_reloj() + ireloj;
		}
	}
	else {
		// Espera para no dar más de "n" fps
		do { ; }while(get_reloj() < (int)freloj);

		volcados_saltados  = 0;
		saltar_volcado     = 0;
		freloj            += ireloj;
	}

	// Marca todos los procesos como no ejecutados
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		mem[ide + _Executed] = 0;
	}

	// Fija la prioridad máxima, para ejecutar procesos según _Priority
	id_old = id_start; // El siguiente a procesar

	// Lee el mouse
	readmouse();

	mou_check = mouse->x     +
	            mouse->y     +
	            mouse->left  +
	            mouse->right +
	            mouse->middle;
	if(mou_check != last_mou_check) {
		last_mou_check = mou_check;

		ss_time_counter = get_reloj() + ss_time;
	}

/*
	// Posiciona las variables del mouse
	if(cbd.mouse_action) {
		readmouse();

		_mouse_x = mouse->x = cbd.mouse_cx >> 1;
		_mouse_y = mouse->y = cbd.mouse_dx;

		mouse->left   = (cbd.mouse_bx & 1);
		mouse->middle = (cbd.mouse_bx & 4) / 4;
		mouse->right  = (cbd.mouse_bx & 2) / 2;

		cbd.mouse_action = 0;
		ss_time_counter  = get_reloj() + ss_time;
	}
*/

	// Lee el joystick
	if(joy_status == 1 && joy_timeout >= 6) {
		joy->button1 = 0;
		joy->button2 = 0;
		joy->button3 = 0;
		joy->button4 = 0;
		joy->left    = 0;
		joy->right   = 0;
		joy->up      = 0;
		joy->down    = 0;

		joy_status = 0;
	}
	else if(joy_status) {
		read_joy();

		joy_check = joy->button1 +
		            joy->button2 +
		            joy->left    +
		            joy->right   +
		            joy->up      +
		            joy->down;
		if(joy_check != last_joy_check) {
			last_joy_check = joy_check;

			ss_time_counter = get_reloj() + ss_time;
		}
	}

	// Lee el teclado (keyboard)
	key_check = 0;
	for(n = 0; n < 128; n++) {
		if(key(n)) {
			key_check++;
		}
	}
	if(key_check != last_key_check) {
		last_key_check = key_check;

		ss_time_counter = get_reloj() + ss_time;
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Finaliza un frame e imprime los gráficos
//═════════════════════════════════════════════════════════════════════════════
void dprocess_active_palette(void);
void dset_video_mode(void);
void dbuffer_to_video(void);
void dprocess_sound(char *sound, int sound_lenght);
void dpost_process(void);
void dprocess_active_palette(void);
void dprocess_palette(void);

void frame_end(void) {
	int mouse_pintado = 0;
	int textos_pintados = 0;
	int mouse_x0, mouse_x1, mouse_y0, mouse_y1;
	int n, m7ide, scrollide, otheride;
	char buf[255];

	// DLL_0 Lee los puntos de ruptura (bien sea de autoload o de import)
#ifdef __EMSCRIPTEN__
	sprintf(buf,
	        "$('#fps').text(\"FPS: %d/%d (max frameskip: %d)\");",
	        fps,
	        dfps,
	        max_saltos);
	emscripten_run_script(buf);
#endif

	if(!dll_loaded) {
		dll_loaded = 1;

#ifdef DIVDLL
		// Los importa

		set_video_mode         = (void (*)())DIV_import("set_video_mode"); //ok
		process_palette        = (void (*)())DIV_import("process_palette"); //ok
		process_active_palette = (void (*)())DIV_import("process_active_palette"); //ok

		process_sound          = (void (*)(char *, int))DIV_import("process_sound"); //ok

		process_fpg            = (void (*)(char *, int))DIV_import("process_fpg"); //ok
		process_map            = (void (*)(char *, int))DIV_import("process_map"); //ok
		process_fnt            = (void (*)(char *, int))DIV_import("process_fnt"); //ok

		background_to_buffer   = (void (*)())DIV_import("background_to_buffer"); //ok
		buffer_to_video        = (void (*)())DIV_import("buffer_to_video"); //ok

		post_process_scroll    = (void (*)())DIV_import("post_process_scroll"); //ok
		post_process_m7        = (void (*)())DIV_import("post_process_m7"); //ok
		post_process_buffer    = (void (*)())DIV_import("post_process_buffer"); //ok
		post_process           = (void (*)())DIV_import("post_process"); //ok

		putsprite              = (void (*)(unsigned char *, int, int, int, int, int, int, int, int, int))DIV_import("put_sprite"); //ok

		ss_init                = (void (*)())DIV_import("ss_init"); //ok
		ss_frame               = (void (*)())DIV_import("ss_frame"); //ok
		ss_end                 = (void (*)())DIV_import("ss_end"); //ok
#endif

		ss_time_counter = get_reloj() + ss_time;

		// DLL_1 Aquí se llama a uno.
#ifdef DEBUG
		if(process_palette != NULL) {
			process_palette();
		}
#endif
	}

	// Si el usuario modificó mouse.x o mouse.y, posiciona el ratón debidamente
	if(_mouse_x != mouse->x || _mouse_y != mouse->y) {
		set_mouse(mouse->x, mouse->y);
	}

	if(!saltar_volcado) {
//printf("Restore type: %d DUmp type: %d\n", restore_type, dump_type);

		// *** OJO *** Restaura las zonas de copia fuera del scroll y del modo 7
		if(restore_type == 0 || restore_type == 1) {
			if(!iscroll[0].on || iscroll[0].x || iscroll[0].y || iscroll[0].an != vga_an || iscroll[0].al != vga_al) {
				if(background_to_buffer != NULL) {
					background_to_buffer();
				}
				else {
					if(old_restore_type == 0) {
						restore(copia, copia2);
					}
					else {
						memcpy(copia,
						       copia2,
						       vga_an * vga_al);
					}
				}
			}
		}

		// Pinta los sprites, por orden de _Z (a mayor z se pinta antes)
		for(ide = id_start; ide <= id_end; ide += iloc_len) {
			mem[ide + _Executed] =  0; // Sin ejecutar
			mem[ide + _x1]       = -1; // Sin región de volcado
		}

		for(n = 0; n < 10; n++) {
			im7[n].painted     = 0;
			iscroll[n].painted = 0;
		}

		do {
			ide       = 0;
			m7ide     = 0;
			scrollide = 0;
			otheride  = 0;
			max       = 0x80000000;

			// Screen Sprites (Processes)
			for(id = id_start; id <= id_end; id += iloc_len) {
				if((mem[id + _Status  ] == 2  ||
				    mem[id + _Status  ] == 4) &&
				    mem[id + _Ctype   ] == 0  &&
				   !mem[id + _Executed]       &&
				    mem[id + _Z       ] > max) {
					ide = id;
					max = mem[id + _Z];
				}
			}

			// Mode7
			for(n = 0; n < 10; n++) {
				if(im7[n].on && (m7 + n)->z >= max && !im7[n].painted) {
					m7ide = n + 1;
					max   = (m7 + n)->z;
				}
			}

			// Scroll
			for(n = 0; n < 10; n++) {
				if(iscroll[n].on && (scroll + n)->z >= max && !iscroll[n].painted) {
					scrollide = n + 1;
					max       = (scroll + n)->z;
				}
			}

			// Texts
			if(text_z >= max && !textos_pintados) {
				otheride = 1;
				max      = text_z;
			}

			// Mouse
			if(mouse->z >= max && !mouse_pintado) {
				otheride = 2;
				max      = mouse->z;
			}

			// Which ?
			if(otheride) {
				// Texts
				if(otheride == 1) {
					pinta_textos();

					textos_pintados = 1;
				}
				// Mouse
				else if(otheride == 2) {
					readmouse();

					// Para evitar retardos (en lo posible)
					/*if(cbd.mouse_action) {
						_mouse_x = mouse->x = cbd.mouse_cx >> 1;
						_mouse_y = mouse->y = cbd.mouse_dx;

						mouse->left   = (cbd.mouse_bx & 1);
						mouse->middle = (cbd.mouse_bx & 4) / 4;
						mouse->right  = (cbd.mouse_bx & 2) / 2;

						cbd.mouse_action = 0;
						ss_time_counter  = get_reloj() + ss_time;
					}*/

					x1s        = -1;
					v_function = -1; // No se producen errores

					put_sprite(mouse->file,
					           mouse->graph,
					           mouse->x,
					           mouse->y,
					           mouse->angle,
					           mouse->size,
					           mouse->flags,
					           mouse->region,
					           copia,
					           vga_an,
					           vga_al);

					mouse_x0 = x0s;
					mouse_x1 = x1s;
					mouse_y0 = y0s;
					mouse_y1 = y1s;

					mouse_pintado = 1;
				}
			}
			// Scroll
			else if(scrollide) {
				snum = scrollide - 1;

				if(iscroll[snum].on == 1)      { scroll_simple();   }
				else if(iscroll[snum].on == 2) { scroll_parallax(); }

				iscroll[snum].painted = 1;
			}
			// Mode7
			else if(m7ide) {
				pinta_m7(m7ide - 1);

				im7[m7ide - 1].painted = 1;
			}
			// Screen Sprite
			else if (ide) {
				pinta_sprite();

				mem[ide + _Executed] = 1;
			}
		}while(ide || m7ide || scrollide || otheride);

		if(post_process_buffer != NULL) {
			post_process_buffer();
		}

#ifdef DEBUG
		if(!debugger_step) {
#endif

		if(buffer_to_video != NULL) {
			buffer_to_video();
		}
		else {
			if(old_dump_type) {
				volcado_completo = 1;

				// Realiza un volcado completo
				volcado(copia);
			}
			else {
				volcado_completo = 0;

				// Añade los volcados de este frame a los restore del anterior

				// Screen Sprites (Processes)
				for(n = id_start; n <= id_end; n += iloc_len) {
					if(mem[n + _x1] != -1) {
						volcado_parcial(mem[n + _x0],
						                mem[n + _y0],
						                mem[n + _x1] - mem[n + _x0] + 1,
						                mem[n + _y1] - mem[n + _y0] + 1);
					}
				}

				// Mode7 and Scroll
				for(n = 0; n < 10; n++) {
					if(im7[n].on) {
						volcado_parcial(im7[n].x,
						                im7[n].y,
						                im7[n].an,
						                im7[n].al);
					}
					if(iscroll[n].on) {
						volcado_parcial(iscroll[n].x,
						                iscroll[n].y,
						                iscroll[n].an,
						                iscroll[n].al);
					}
				}

				// Mouse
				if(mouse_x1 != -1) {
					volcado_parcial(mouse_x0,
					                mouse_y0,
					                mouse_x1 - mouse_x0 + 1,
					                mouse_y1 - mouse_y0 + 1);
				}

				// Texts
				for(n = 0; n < max_textos; n++) {
					if(texto[n].font && texto[n].an) {
						volcado_parcial(texto[n].x0,
						                texto[n].y0,
						                texto[n].an,
						                texto[n].al);
					}
				}

				// Realiza un volcado parcial
				volcado(copia);
			}

			// Fija los restore para el siguiente frame
			if(dump_type == 0 || restore_type == 0) {
				// Screen Sprites (Processes)
				for(n = id_start; n <= id_end; n += iloc_len) {
					if(mem[n + _x1] != -1) {
						volcado_parcial(mem[n + _x0],
						                mem[n + _y0],
						                mem[n + _x1] - mem[n + _x0] + 1,
						                mem[n + _y1] - mem[n + _y0] + 1);
					}
				}

				// Mode7 and Scroll
				for(n = 0; n < 10; n++) {
					if(im7[n].on) {
						volcado_parcial(im7[n].x,
						                im7[n].y,
						                im7[n].an,
						                im7[n].al);
					}
					if(iscroll[n].on) {
						volcado_parcial(iscroll[n].x,
						                iscroll[n].y,
						                iscroll[n].an,
						                iscroll[n].al);
					}
				}

				// Mouse
				if(mouse_x1 != -1) {
					volcado_parcial(mouse_x0,
					                mouse_y0,
					                mouse_x1 - mouse_x0 + 1,
					                mouse_y1 - mouse_y0 + 1);
				}

				// Texts
				for(n = 0; n < max_textos; n++) {
					if(texto[n].font && texto[n].an) {
						volcado_parcial(texto[n].x0,
						                texto[n].y0,
						                texto[n].an,
						                texto[n].al);
					}
				}
			}
		}

#ifdef DEBUG
		}
#endif
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Elimina un proceso
//═════════════════════════════════════════════════════════════════════════════
void elimina_proceso(int id) {
	int id2;

	mem[id+_Status] = 0;

	procesos--;

	if(id2 = mem[id + _Father]) {
		if(mem[id2 + _Son] == id) {
			mem[id2 + _Son] = mem[id + _BigBro];
		}
	}
	if(id2 = mem[id + _BigBro]) {
		mem[id2 + _SmallBro] = mem[id + _SmallBro];
	}
	if(id2 = mem[id + _SmallBro]) {
		mem[id2 + _BigBro] = mem[id + _BigBro];
	}
	if(id2 = mem[id + _Son]) {
		do {
			mem[id2 + _Father] = id_init;

			if(mem[id2 + _BigBro] == 0) {
				mem[id2 + _BigBro] = mem[id_init + _Son];
				mem[mem[id_init + _Son] + _SmallBro] = id2;

				id2 = 0;
			}
			else {
				id2 = mem[id2 + _BigBro];
			}
		}while(id2);

		mem[id_init + _Son] = mem[id + _Son];
	}
}

//═════════════════════════════════════════════════════════════════════════════
// Finalización
//═════════════════════════════════════════════════════════════════════════════
void finalizacion(void) {
	dacout_r     = 64;
	dacout_g     = 64;
	dacout_b     = 64;
	dacout_speed = 4;
	while(now_dacout_r != dacout_r ||
	      now_dacout_g != dacout_g ||
	      now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();
	}

#ifdef DIVDLL
	while(nDLL--) {
		DIV_UnLoadDll(pe[nDLL]);
	}
#endif

#ifdef NETLIB
	if(inicializacion_red) {
		net_end();
	}
#endif
	rvmode();
	DIVEndSound();
	mouse_off();
	kbdReset();

//	printf("Ejecución correcta:\n\n\tnº actual de procesos = %u\n\tnº máximo de procesos = %u",
//	       procesos,
//	      (id_end - id_start) / iloc_len + 1);
}

//═════════════════════════════════════════════════════════════════════════════
// Error de ejecución
//═════════════════════════════════════════════════════════════════════════════
void exer(int e) {
#ifdef DEBUG
	FILE *f;

	if(e) {
		if((f = fopen("system/exec.err", "wb")) != NULL) {
			fwrite(&e, 4, 1, f);
			fclose(f);
		}
	}
#else
	if(e) {
		printf("Error: ");
		switch(e) {
			case 1:  { printf("Out of memory!");              break; }
			case 2:  { printf("Too many process!");           break; }
			case 3:  { printf("Stack overflow!");             break; }
			case 4:  { printf("DLL not found!");              break; }
			case 5:  { printf("System font file missed!");    break; }
			case 6:  { printf("System graphic file missed!"); break; }
			default: { printf("Internal error!");             break; }
		}
	}
#endif

//	printf("*** Error de ejecución:\n\n\tnº actual de procesos = %u\n\tnº máximo de procesos = %u",
//	       procesos,
//	      (id_end - id_start) / iloc_len + 1);

#ifdef NETLIB
	if(inicializacion_red) {
		net_end();
	}
#endif
	rvmode();
	DIVEndSound();
	mouse_off();
	kbdReset();

	exit(0);
}

//════════════════════════════════════════════════════════════════════════════
// Mensajes de error - Versión con debugger
//════════════════════════════════════════════════════════════════════════════
char *fname[] = {
	"signal",
	"key",
	"load_pal",
	"load_fpg",
	"start_scroll",
	"stop_scroll",
	"out_region" ,
	"graphic_info",
	"collision",
	"get_id",
	"get_distx",
	"get_disty",
	"get_angle",
	"get_dist",
	"fade",
	"load_fnt",
	"write",
	"write_int",
	"delete_text",
	"move_text",
	"unload_fpg",
	"rand",
	"define_region",
	"xput",
	"put",
	"put_screen",
	"map_xput",
	"map_put",
	"put_pixel",
	"get_pixel",
	"map_put_pixel",
	"map_get_pixel",
	"get_point",
	"clear_screen",
	"save",
	"load",
	"set_mode",
	"load_pcm",
	"unload_pcm",
	"sound",
	"stop_sound",
	"change_sound",
	"set_fps",
	"start_fli",
	"frame_fli",
	"end_fli",
	"reset_fli",
	"system",
	"refresh_scroll",
	"fget_dist",
	"fget_angle",
	"play_cd",
	"stop_cd",
	"is_playing_cd",
	"start_mode7",
	"stop_mode7",
	"advance",
	"abs",
	"fade_on",
	"fade_off",
	"rand_seed",
	"sqrt",
	"pow",
	"map_block_copy",
	"move_scroll",
	"near_angle",
	"let_me_alone",
	"exit",
	"roll_palette",
	"get_real_point",
	"get_joy_button",
	"get_joy_position",
	"convert_palette",
	"load_map",
	"reset_sound",
	"unload_map",
	"unload_fnt",
	"set_volume"
};

#ifndef DEBUG
#ifdef __cplusplus
void e(int t) {
	printf("error %d\n", t);

	e("DIV ERROR\n");
}
#endif

void e(char *texto) {
	if(v_function == -1) {
		return;
	}

	if(v_function >= 0) {
		printf("DIV Execution error: %s (%s)", texto, fname[v_function]);
	}
	else {
		printf("DIV Execution error: %s", texto);
	}

#ifdef NETLIB
	if(inicializacion_red) {
		net_end();
	}
#endif

	rvmode();
	DIVEndSound();
	mouse_off();
//	if(end_extern != NULL) {
//		end_extern();
//	}
	kbdReset();

	exit(0);
}

#endif
