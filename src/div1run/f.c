//════════════════════════════════════════════════════════════════════════════
// Código de las funciones internas
//════════════════════════════════════════════════════════════════════════════
#include "inter.h"
#include "divsound.h"
#include "cdrom.h"
//#include "net.h"

// file prototypes
void DIV_signal(void);
void DIV_key(void);
void DIV_load_pal(void);
void DIV_load_fpg(void);
void DIV_stop_scroll(void);
void DIV_start_scroll(void);
void DIV_get_id(void);
void DIV_get_distx(void);
void DIV_get_disty(void);
void DIV_get_dist(void);
void DIV_get_angle(void);
void DIV_load_fnt(void);
void DIV_load_fpg(void);
void DIV_fade(void);
void DIV_write(void);
void DIV_write_int(void);
void DIV_delete_text(void);
void DIV_unload_fpg(void);
void DIV_move_text(void);
void DIV_define_region(void);
void DIV_rand(void);
void DIV_xput(void);
void DIV_put(void); 
void DIV_put_screen(void);
void DIV_map_xput(void);
void DIV_map_put(void);
void DIV_put_pixel(void);
void DIV_get_pixel(void);
void DIV_map_put_pixel(void);
void DIV_map_get_pixel(void);
void DIV_get_point(void);
void DIV_clear_screen(void);
void DIV_save(void);
void DIV_load(void);
void DIV_set_mode(void);
void DIV_load_pcm(void);
void DIV_unload_pcm(void);
void DIV_sound(void);
void DIV_stop_sound(void);
void DIV_change_sound(void);
void DIV_set_fps(void);
void DIV_start_fli(void);
void DIV_frame_end(void);
void DIV_frame_fli(void);
void DIV_end_fli(void);
void DIV_reset_fli(void);
void DIV_system(void);
void DIV_fget_dist(void);
void DIV_get_dist(void);
void DIV_fget_angle(void);
void DIV_get_angle(void);
void DIV_refresh_scroll(void);
void DIV_play_cd(void);
void DIV_stop_cd(void);
void DIV_is_playing_cd(void);
void DIV_start_mode7(void);
void DIV_stop_mode7(void);
void DIV_advance(void);
void DIV_abs(void);
void DIV_fade_off(void);
void DIV_fade_on(void);
void DIV_rand_seed(void);
void DIV_sqrt(void);
void DIV_pow(void);
void DIV_map_block_copy(void);
void DIV_move_scroll(void);
void DIV_near_angle(void);
void DIV_let_me_alone(void);
void DIV_exit(void);
void DIV_roll_palette(void);
void DIV_get_real_point(void);
void DIV_get_joy_button(void);
void DIV_get_joy_position(void);
void DIV_convert_palette(void);
void DIV_load_map(void);
void DIV_reset_sound(void);
void DIV_unload_map(void);
void DIV_unload_fnt(void);
void DIV_set_volume(void);


void signal_tree(int p, int s);
void kill_invisible(void);
int  joy_position(int eje);

// sound prototypes
int DIVLoadSound(char *ptr, long Len, int Loop);
int DIVPlaySound(int n, int m, int o);
int DIVUnloadSound(int n);
int DIVStopSound(int n);
int DIVChangeSound(int n, int m, int o);

void read_mouse(void);
//void path_find(void);
//void path_line(void);
//void path_free(void);

//════════════════════════════════════════════════════════════════════════════
// Switch principal
//════════════════════════════════════════════════════════════════════════════
void function(void) {
//	printf("calling func %s at: %d %d\n", fname[mem[ip]], ip - 1, mem[ip]);
	switch(v_function = mem[ip++]) {
		case  0: { DIV_signal();           break; }
		case  1: { DIV_key();              break; }
		case  2: { DIV_load_pal();         break; }
		case  3: { DIV_load_fpg();         break; }
		case  4: { DIV_start_scroll();     break; }
		case  5: { DIV_stop_scroll();      break; }
		case  6: { DIV_out_region();       break; }
		case  7: { DIV_graphic_info();     break; }
		case  8: { DIV_collision();        break; }
		case  9: { DIV_get_id();           break; }
		case 10: { DIV_get_distx();        break; }
		case 11: { DIV_get_disty();        break; }
		case 12: { DIV_get_angle();        break; }
		case 13: { DIV_get_dist();         break; }
		case 14: { DIV_fade();             break; }
		case 15: { DIV_load_fnt();         break; }
		case 16: { DIV_write();            break; }
		case 17: { DIV_write_int();        break; }
		case 18: { DIV_delete_text();      break; }
		case 19: { DIV_move_text();        break; }
		case 20: { DIV_unload_fpg();       break; }
		case 21: { DIV_rand();             break; }
		case 22: { DIV_define_region();    break; }
		case 23: { DIV_xput();             break; }
		case 24: { DIV_put();              break; }
		case 25: { DIV_put_screen();       break; }
		case 26: { DIV_map_xput();         break; }
		case 27: { DIV_map_put();          break; }
		case 28: { DIV_put_pixel();        break; }
		case 29: { DIV_get_pixel();        break; }
		case 30: { DIV_map_put_pixel();    break; }
		case 31: { DIV_map_get_pixel();    break; }
		case 32: { DIV_get_point();        break; }
		case 33: { DIV_clear_screen();     break; }
		case 34: { DIV_save();             break; }
		case 35: { DIV_load();             break; }
		case 36: { DIV_set_mode();         break; }
		case 37: { DIV_load_pcm();         break; }
		case 38: { DIV_unload_pcm();       break; }
		case 39: { DIV_sound();            break; }
		case 40: { DIV_stop_sound();       break; }
		case 41: { DIV_change_sound();     break; }
		case 42: { DIV_set_fps();          break; }
		case 43: { DIV_start_fli();        break; }
		case 44: { DIV_frame_fli();        break; }
		case 45: { DIV_end_fli();          break; }
		case 46: { DIV_reset_fli();        break; }
		case 47: { DIV_system();           break; }
		case 48: { DIV_refresh_scroll();   break; }
		case 49: { DIV_fget_dist();        break; }
		case 50: { DIV_fget_angle();       break; }
		case 51: { DIV_play_cd();          break; }
		case 52: { DIV_stop_cd();          break; }
		case 53: { DIV_is_playing_cd();    break; }
		case 54: { DIV_start_mode7();      break; }
		case 55: { DIV_stop_mode7();       break; }
		case 56: { DIV_advance();          break; }
		case 57: { DIV_abs();              break; }
		case 58: { DIV_fade_on();          break; }
		case 59: { DIV_fade_off();         break; }
		case 60: { DIV_rand_seed();        break; }
		case 61: { DIV_sqrt();             break; }
		case 62: { DIV_pow();              break; }
		case 63: { DIV_map_block_copy();   break; }
		case 64: { DIV_move_scroll();      break; }
		case 65: { DIV_near_angle();       break; }
		case 66: { DIV_let_me_alone();     break; }
		case 67: { DIV_exit();             break; }
		case 68: { DIV_roll_palette();     break; }
		case 69: { DIV_get_real_point();   break; }
		case 70: { DIV_get_joy_button();   break; }
		case 71: { DIV_get_joy_position(); break; }
		case 72: { DIV_convert_palette();  break; }
		case 73: { DIV_load_map();         break; }
		case 74: { DIV_reset_sound();      break; }
		case 75: { DIV_unload_map();       break; }
		case 76: { DIV_unload_fnt();       break; }
		case 77: { DIV_set_volume();       break; }
/*#ifdef NOTYET
    case 78: { DIV_set_color();        break; }
#ifdef NETLIB // -> DIV1 has no networking support
    case 79: { DIV_net_init_ipx();     break; }
    case 80: { DIV_net_init_modem();   break; }
    case 81: { DIV_net_init_serial();  break; }
    case 82: { DIV_net_send();         break; }
    case 83: { DIV_net_asign();        break; }
#endif
    case 84: { DIV_path_find();        break; }
    case 85: { DIV_path_line();        break; }
    case 86: { DIV_path_free();        break; }
    case 87: { DIV_new_map();          break; }
#endif*/
		/* OJO al añadir una funcion, hacerlo también en la lista fname de i.cpp, ltobj, add.prg y help */
	}
}

static int n_reloj = 0, o_reloj = 0;

int get_reloj(void) {
/*
	n_reloj  = SDL_GetTicks() / 10;
	reloj   += (n_reloj - o_reloj);
	o_reloj  = n_reloj;
*/

	reloj = SDL_GetTicks() / 10;

	return reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV signal(INT process, INT signal_type)
//════════════════════════════════════════════════════════════════════════════
void DIV_signal(void) {
	int i;

	bp = pila[sp - 1];
	if((bp & 1) && bp >= id_init && bp <= id_end && bp == mem[bp]) {
		if(mem[bp + _Status]) {
			if(pila[sp] < 100) {
				mem[bp + _Status] = pila[sp--] + 1;
			}
			else {
				mem[bp + _Status] = pila[sp--] - 99;

				if(mem[bp + _Son]) {
					signal_tree(mem[bp + _Son], pila[sp + 1] - 99);
				}
			}
		}
		else {
			pila[--sp] = 0; // Devuelve el proceso o 0 si estaba muerto
		}
	}
	else {
		for(i = id_start; i <= id_end; i += iloc_len) {
			if(mem[i + _Status] && mem[i + _Bloque] == bp) {
				if(pila[sp] < 100) {
					mem[i + _Status] = pila[sp] + 1;
				}
				else {
					mem[i + _Status] = pila[sp] - 99;

					if(mem[i + _Son]) {
						signal_tree(mem[i + _Son],
						            pila[sp] - 99);
					}
				}
			}
		}

		pila[--sp] = 0;
	}
}

void signal_tree(int p, int s) {
	do {
		mem[p + _Status] = s;

		if(mem[p + _Son]) {
			signal_tree(mem[p + _Son], s);
		}

		p = mem[p + _BigBro];
	}while (p);
}

//════════════════════════════════════════════════════════════════════════════
// DIV key(INT key_code)
//════════════════════════════════════════════════════════════════════════════
void DIV_key(void) {
	if(pila[sp] <= 0 || pila[sp] >= 128) {
		e(e101);
		return;
	}

//	printf("Looking for key: %d %d\n", pila[sp], key(pila[sp]));
	pila[sp] = kbdFLAGS[pila[sp]];
}

//════════════════════════════════════════════════════════════════════════════
//  Funcion para localizar y abrir un fichero (pal,fpg,fnt,...)
//  Esta función debe seguir el mismo algoritmo en F.CPP y DIVC.CPP
//════════════════════════════════════════════════════════════════════════════
char full[_MAX_PATH + 1];

FILE *div_open_file(byte *file) {
	FILE *f;

	char  drive[_MAX_DRIVE + 1];
	char  dir  [_MAX_DIR   + 1];
	char  fname[_MAX_FNAME + 1];
	char  ext  [_MAX_EXT   + 1];
#ifndef DOS
//printf("trying to load [%s]\n", file);

	if(strlen((char *)file )== 0) {
		return NULL;
	}

	char *ff = (char *)file;

	while(*ff != 0) {
		if(*ff =='\\') { *ff='/'; }
		ff++;
	}
#endif

//printf("%s\n",full);
	strcpy(full, (char*)file);
//printf("trying to load %s\n", full);
	if((f = fopen(full, "rb")) == NULL) {                     // "paz\fixero.est"
		if(_fullpath(full, (char*)file, _MAX_PATH) == NULL) {
			return(NULL);
		}

		_splitpath(full, drive,dir, fname,ext);

		if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
		else                         { strcpy(full, strchr(ext, '.') + 1); }

		if(strlen(full) && file[0] != '/') {
			strcat(full, "/");
		}
		strcat(full, (char* )file);
//printf("Trying: %s\n", full);
		if((f = fopen(full, "rb")) == NULL) {                   // "est\paz\fixero.est"
			strupr(full);
//printf("Trying: %s\n", full);
			if((f = fopen(full, "rb")) == NULL) {                   // "est\paz\fixero.est"
				strcpy(full, fname);
				strcat(full, ext);
//printf("Trying: %s\n", full);
				if((f = fopen(full, "rb")) == NULL) {                 // "fixero.est"
					strupr(full);
//printf("Trying: %s\n", full);
					if((f = fopen(full, "rb")) == NULL) {                 // "fixero.est"
						strlwr(full);
						if((f = fopen(full, "rb")) == NULL) {                 // "fixero.est"
							if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
							else                         { strcpy(full, strchr(ext, '.') + 1); }
							if(strlen(full)) {
								strcat(full, "/");
							}
							strcat(full, fname);
							strcat(full, ext);
//printf("Trying: %s\n", full);
							if((f = fopen(full, "rb")) == NULL) {               // "est\fixero.est"
								strlwr(full);
								if((f = fopen(full, "rb")) == NULL) {               // "est\fixero.est"
//#ifndef PSP
									if(f = memz_open_file(file)) {
										return (f);
									}
//#endif
//printf("failed %s\n", file);

									strcpy(full, "");

									return (NULL);
								}
								else {
									return (f);
								}
							}
							else {
								return (f);
							}
						}
						else {
							return (f);
						}
					}
					else {
						return (f);
					}
				}
				else {
					return(f);
				}
			}
			else {
				return (f);
			}
		}
		else {
			return (f);
		}
	}
	else {
		return (f);
	}
}

FILE *_open_file(char *file) {
	FILE *f;

	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];

	strcpy(full, file);
	if((f = fopen(full, "rb")) == NULL) {                     // "paz\fixero.est"
		if(_fullpath(full, file, _MAX_PATH) == NULL) {
			return (NULL);
		}

		_splitpath(full, drive, dir, fname, ext);

		if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
		else                         { strcpy(full, strchr(ext, '.') + 1); }
		if(strlen(full) && file[0] != '/') {
			strcat(full, "/");
		}
		strcat(full, file);
		if((f = fopen(full, "rb")) == NULL) {                   // "est\paz\fixero.est"
			strcpy(full, fname);
			strcat(full, ext);
			if((f = fopen(full, "rb")) == NULL) {                 // "fixero.est"
				if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
				else                         { strcpy(full, strchr(ext, '.') + 1); }
				if(strlen(full)) {
					strcat(full, "/");
				}
				strcat(full, fname);
				strcat(full, ext);
				if((f = fopen(full, "rb")) == NULL) {               // "est\fixero.est"
					strcpy(full, "");

					return (NULL);
				}
				else {
					return (f);
				}
			}
			else {
				return (f);
			}
		}
		else {
			return (f);
		}
	}
	else {
		return (f);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_pal(STRING file_name)
//════════════════════════════════════════════════════════════════════════════
int hacer_fade = 0;

void DIV_load_pal(void) {
	byte pal[1352];
	int m, offs = 8;

	if((es = div_open_file((byte*)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e102);
	}
	else {
		fread(pal, 1, 1352, es);
		fclose(es);

		if(strcmp((char *)pal, "pal\x1a\x0d\x0a")) {
			if(strcmp((char *)pal, "fpg\x1a\x0d\x0a")) {
				if(strcmp((char *)pal, "fnt\x1a\x0d\x0a")) {
					if(strcmp((char *)pal, "map\x1a\x0d\x0a")) {
						e(e103);
						return;
					}
					else {
						offs = 48;
					}
				}
			}
		}

		for(m = 0; m < 768; m++) {
			if(pal[m + offs] != paleta[m]) {
				break;
			}
		}
		if(m < 768) {
			dr = dacout_r;
			dg = dacout_g;
			db = dacout_b;
			if(dr < 63 || dg < 63 || db < 63) {
				hacer_fade=1;

				DIV_fade_off();

				sp--;
			}

			memcpy(paleta,
			       pal + offs,
			       768);

			nueva_paleta();
		}

		paleta_cargada = 1;

		pila[sp] = 1;
	}
}

void nueva_paleta(void) {
	byte *p, c0, c1;
	int n;

	if(process_palette != NULL) {
		process_palette();
	}

	memcpy(dac,
	       paleta,
	       68);

	init_ghost();
	crear_ghost();

	find_color( 0,  0,  0); c0 = find_col;
	find_color(63, 63, 63); c1 = find_col;

	p = fonts[0] + 1356 + sizeof(t_fnt_table) * 256;

	for(n = 0; n < 12288; n++) {
		if(*p == last_c1) {
			*p++ = c1;
			if((n & 7) != 7 && *p != last_c1) {
				*p = c0;
			}
		}
		else {
			p++;
		}
	}

	last_c1 = c1;

	set_paleta();

#ifdef DEBUG
	init_colors();

	new_palette = 1;
#endif

	if(hacer_fade) {
		hacer_fade = 0;

		dacout_r = dr;
		dacout_g = dg;
		dacout_b = db;

		DIV_fade_on();

		sp--;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV unload_map(INT code)
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_map(void) {
	if(pila[sp] < 1000 || pila[sp] > 1999) {
		return;
	}

	if(g[0].grf[pila[sp]] != 0) {
		free((byte *)(g[0].grf[pila[sp]]) - 1394 + 64);

		g[0].grf[pila[sp]] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_map(STRING file_name)
// - Devuelve el código del gráfico (1000..1999)
//════════════════════════════════════════════════════════════════════════════
void DIV_load_map(void) {
	int ancho, alto, npuntos, m;
	byte *ptr;
	int old_reloj = get_reloj();

	if((es = div_open_file((byte *)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e143);
	}
	else {
		fseek(es, 0, SEEK_END);
		file_len = ftell(es);

		if((ptr = (byte *)malloc(file_len)) != NULL) {
			fseek(es, 0, SEEK_SET);
			fread(ptr, 1, file_len, es);
			fclose(es);

			if(strcmp((char *)ptr, "map\x1a\x0d\x0a")) {
				e(e144);
				free(ptr);
				return;
			}

			if(process_map != NULL) {
				process_map((char *)ptr, file_len);
			}

			if(!paleta_cargada) {
				for(m = 0; m < 768; m++) {
					if(ptr[m + 48] != paleta[m]) {
						break;
					}
				}
				if(m < 768) {
					dr = dacout_r;
					dg = dacout_g;
					db = dacout_b;
					if(dr < 63 || dg < 63 || db < 63) {
						hacer_fade = 1;

						DIV_fade_off();

						sp--;
					}

					memcpy(paleta,
					       ptr + 48,
					       68);

					nueva_paleta();
				}

				paleta_cargada = 1;
			}

			ancho   = *(word *)(ptr + 8);
			alto    = *(word *)(ptr + 10);
			npuntos = *(word *)(ptr + 1392);

			ptr = ptr + 1394 - 64;
			*((int *)ptr + 13) = ancho;
			*((int *)ptr + 14) = alto;
			*((int *)ptr + 15) = npuntos;

			g[0].grf[next_map_code] = (int *)ptr;

			pila[sp] = next_map_code++;
		}
		else {
			fclose(es);

			pila[sp] = 0;

			e(e100);
		}
	}

	reloj = old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV new_map(INT width, INT height, INT center_x, INT center_y, INT color)
// - Devuelve el código
// - Se descarga igualmente con unload_map(INT code)
//════════════════════════════════════════════════════════════════════════════
void DIV_new_map(void) {
	int ancho, alto, cx, cy, color;
  byte *ptr;

	color = pila[sp--];
	cy    = pila[sp--];
	cx    = pila[sp--];
	alto  = pila[sp--];
	ancho = pila[sp];

	pila[sp] = 0;

	// Comprueba límites an/al/color ...
	if(ancho < 1     || alto < 1 ||
	   ancho > 32768 || alto > 32768) {
		e(e153);
		return;
	}
	if(color < 0 || color > 255) {
		e(e154);
		return;
	}
	if(cx < 0 || cy < 0 || cx >= ancho || cy >= alto) {
		e(e155);
		return;
	}

	if((ptr = (byte *)malloc(64 + 4 + ancho * alto)) != NULL) {
		*((int *)ptr + 13) = ancho;
		*((int *)ptr + 14) = alto;
		*((int *)ptr + 15) = 1; // Se define un punto de control (el centro)

		*((word *)ptr + 32) = cx;
		*((word *)ptr + 33) = cy;
		memset(ptr + 4 + 64,
		       color,
		       ancho * alto);

		g[0].grf[next_map_code] = (int *)ptr;

		pila[sp] = next_map_code++;
	}
	else {
		e(e100);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_fpg(STRING file_name)
// - Devuelve el código del fichero 0..max_fpgs
//════════════════════════════════════════════════════════════════════════════
void DIV_load_fpg(void) {
	int n = 0, m;
	int **lst;
	byte *ptr;
	int *iptr;
	int old_reloj = get_reloj();

	while(n < max_fpgs) {
		if(g[n].fpg == 0) {
			break;
		}

		n++;
	}
	if(n == max_fpgs) {
		pila[sp] = 0;

		e(e104);
		return;
	}

	if(n) {
		if((lst =(int **)malloc(sizeof(int *) * 1000)) == NULL) {
			pila[sp] = 0;

			e(e100);
			return;
		}
	}
	else {
		lst = g[0].grf;
	}
	memset(lst,
	       0,
	       sizeof(int *) * 1000);

	if((es = div_open_file((byte *)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e105);
	}
	else {
		fseek(es, 0, SEEK_END);
#ifdef __EMSCRIPTEN__
		file_len = 1352; // just palette
#else
		file_len = ftell(es);
#endif
//printf("File len: %d\n", file_len);
		if((ptr = (byte *)malloc(file_len)) != NULL) {
			g[n].fpg = (int **)ptr;

			fseek(es, 0, SEEK_SET);
			fread(ptr, 1, file_len, es);
			fclose(es);

#ifdef STDOUTLOG
//printf("fpg pointer is %x\n", (int **)ptr);
#endif
		}
		else {
			fclose(es);

			pila[sp] = 0;

			e(e100);
			return;
		}
	}

	if(strcmp((char *)ptr, "fpg\x1a\x0d\x0a")) {
		free(ptr); // avoid potential memory leak

		e(e106);
		return;
	}

	if(process_fpg != NULL) {
		process_fpg((char *)ptr, file_len);
	}

	if(!paleta_cargada) {
		for(m = 0; m < 768; m++) {
			if(ptr[m + 8] != paleta[m]) {
				break;
			}
			if(m < 768) {
				dr = dacout_r;
				dg = dacout_g;
				db = dacout_b;
				if(dr < 63 || dg < 63 || db < 63) {
					hacer_fade = 1;

					DIV_fade_off();

					sp--;
				}

				memcpy(paleta,
				       ptr + 8,
				       768);

				nueva_paleta();
			}

			paleta_cargada=1;
		}
	}

	g[n].grf = lst;

#ifdef __EMSCRIPTEN__
	// do something different
	int len_ = 1;
	int num_ = 1;

	es = div_open_file((byte *)&mem[itxt + pila[sp]]); // TODO check open error
//printf("File pointer: %x\n", es);
	fseek(es, 0, SEEK_END);
	file_len = ftell(es);
//printf("File len: %d\n", file_len);
	fseek(es, 1352, SEEK_SET);

	while(ftell(es) < file_len && len_ > 0 && num_ > 0) {
		int pos = ftell(es);
		byte *mptr = &ptr[pos];

		fread(&num_, 4, 1, es);
		fread(&len_, 4, 1, es);
//	printf("%d %d %d\n", len_, num_, ftell(es));
		fseek(es, -8, SEEK_CUR);

		mptr = (byte *)malloc(len_);
		fread(mptr, 1, len_, es);
		lst[num_] = iptr = (int *)mptr;
//	printf("mem ptr is %x\n", iptr);
//	if(m != palcrc) {
//		adaptar(ptr + 64 + iptr[15] * 4,
//		        iptr[13] * iptr[14],
//		        (byte *)(g[num].fpg) + 8,
//		       &xlat[0]);
//	}
	}
	fclose(es);
#else
	ptr += 1352; // Longitud cabecera fpg
	while(ptr < (byte *)g[n].fpg + file_len &&
	     (*(int *)ptr < 1000 && *(int *)ptr > 0)) {
		lst[*(int *)ptr] = (int *)ptr;

		ptr += *(int *)(ptr + 4);
	}
#endif

	pila[sp] = n;

	reloj = old_reloj;
}

//══════════════════════════════════════════════════════════════════════════════════════════
// DIV start_scroll(INT scroll_num, INT file, INT graph1, INT graph2, INT region, INT flags)
//══════════════════════════════════════════════════════════════════════════════════════════
void set_scroll(int plano, int x, int y);

void DIV_start_scroll(void) {
	int file, graf1, graf2, reg, s;
	int *ptr1, *ptr2, mf;

	mf    = pila[sp--];
	reg   = pila[sp--];
	graf2 = pila[sp--];
	graf1 = pila[sp--];
	file  = pila[sp--];
	snum  = pila[sp];

	pila[sp] = 0;

//printf("Start scroll: %d %d %d %d %d %d\n", snum, file, graf1, graf2, reg, mf);

	if(snum < 0 || snum > 9) {
		e(e107);
		return;
	}

	iscroll[snum].map_flags = mf;

	if(iscroll[snum].on) {
		pila[sp] = snum;

		DIV_stop_scroll();
	}

	if(reg >= 0 && reg < max_region) {
		iscroll[snum].x  = region[reg].x0;
		iscroll[snum].y  = region[reg].y0;
		iscroll[snum].an = region[reg].x1 - region[reg].x0;
		iscroll[snum].al = region[reg].y1 - region[reg].y0;
	}
	else {
		e(e108);
		return;
	}
	if(iscroll[snum].an == 0 || iscroll[snum].al == 0) {
		e(e146);
		return;
	}
	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 < 0 || graf1 >= max_grf) {
		e(e110);
		return;
	}
	if(graf2 < 0 || graf2 >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	ptr1 = g[file].grf[graf1];
	ptr2 = g[file].grf[graf2];
	if(ptr1 == NULL && ptr2 == NULL) {
		e(e112);
		return;
	}

	if(ptr1 == NULL) {
		ptr1 = ptr2;
		ptr2 = NULL;
	}
	// Tipo de scroll, normal(1) o parallax(2)
	if(ptr2 == NULL) { s = 1; } else { s = 2; }

	iscroll[snum].map1_an = ptr1[13];
	iscroll[snum].map1_al = ptr1[14];
	iscroll[snum].map1    = (byte *)ptr1 + 64 + ptr1[15] * 4;
	if(iscroll[snum].an > iscroll[snum].map1_an) {
		iscroll[snum].map_flags |= 1;
	}
	if(iscroll[snum].al > iscroll[snum].map1_al) {
		iscroll[snum].map_flags |= 2;
	}
	if(ptr1[15] == 0) {
		iscroll[snum].map1_x = 0;
		iscroll[snum].map1_y = 0;
	}
	else {
		iscroll[snum].map1_x = *((word *)ptr1 + 32);
		iscroll[snum].map1_y = *((word *)ptr1 + 33);
	}

	if((iscroll[snum]._scr_1 = (byte *)malloc(iscroll[snum].an * (iscroll[snum].al + 1))) == NULL) {
		e(e100);
		return;
	}
	if((iscroll[snum].fast = (t_fast *)malloc(iscroll[snum].al * sizeof(t_fast))) == NULL) {
		free(iscroll[snum]._scr_1); // memory leak fix

		e(e100);
		return;
	}
	iscroll[snum].scr_1   = iscroll[snum]._scr_1;
	iscroll[snum].block1 = iscroll[snum].al;
	iscroll[snum].on     = s;

	set_scroll(0, iscroll[snum].map1_x, iscroll[snum].map1_y);

	iscroll[snum].on = 0; // Si hay algún error (malloc), no habrá scroll

	// we have parallax scroll
	if(s == 2) {
		iscroll[snum].map2_an = ptr2[13];
		iscroll[snum].map2_al = ptr2[14];
		iscroll[snum].map2    = (byte *)ptr2 + 64 + ptr2[15] * 4;
		if(iscroll[snum].an > iscroll[snum].map2_an) {
			iscroll[snum].map_flags |= 4;
		}
		if(iscroll[snum].al > iscroll[snum].map2_al) {
			iscroll[snum].map_flags |= 8;
		}

		if(ptr2[15] == 0) {
			iscroll[snum].map2_x = 0;
			iscroll[snum].map2_y = 0;
		}
		else {
			iscroll[snum].map2_x = *((word *)ptr2 + 32);
			iscroll[snum].map2_y = *((word *)ptr2 + 33);
		}

		if((iscroll[snum]._scr_2 = (byte *)malloc(iscroll[snum].an *(iscroll[snum].al + 1))) == NULL) {
			free(iscroll[snum]._scr_1);
			free(iscroll[snum].fast);

			e(e100);
			return;
		}

		iscroll[snum].scr_2   = iscroll[snum]._scr_2;
		iscroll[snum].block2 = iscroll[snum].al;
		iscroll[snum].on     = 2;

		set_scroll(1, iscroll[snum].map2_x, iscroll[snum].map2_y);
	}

	iscroll[snum].on = s; // Al final si no ha habido errores, fija la variable scroll

	(scroll + snum)->x0 = iscroll[snum].map1_x;
	(scroll + snum)->y0 = iscroll[snum].map1_y;
	(scroll + snum)->x1 = iscroll[snum].map2_x;
	(scroll + snum)->y1 = iscroll[snum].map2_y;
}

//════════════════════════════════════════════════════════════════════════════
// DIV refresh_scroll(INT scroll_num)
//════════════════════════════════════════════════════════════════════════════
void DIV_refresh_scroll(void) {
	snum = pila[sp];

	set_scroll(0, iscroll[snum].map1_x, iscroll[snum].map1_y);
	set_scroll(1, iscroll[snum].map2_x, iscroll[snum].map2_y);
}

//════════════════════════════════════════════════════════════════════════════
// DIV move_scroll(INT scroll_num)
// - Mueve automática o manualmente el scroll
//════════════════════════════════════════════════════════════════════════════
void mover_scroll(int);

void DIV_move_scroll(void) {
	snum = pila[sp];

	if(snum < 0 || snum > 9) {
		e(e107);
		return;
	}

	if(iscroll[snum].on == 1)      { mover_scroll(0); }
	else if(iscroll[snum].on == 2) { mover_scroll(1); }
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_scroll(INT scroll_num)
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_scroll(void) {
	snum = pila[sp];

	if(snum < 0 || snum > 9) {
		e(e107);
		return;
	}

	if(iscroll[snum].on) {
		free(iscroll[snum].fast); // memory leak fix
		free(iscroll[snum]._scr_1);
	}
	if(iscroll[snum].on == 2) {
		free(iscroll[snum]._scr_2);
	}

	iscroll[snum].on = 0;

	kill_invisible();

	(scroll + snum)->x0 = 0;
	(scroll + snum)->y0 = 0;
	(scroll + snum)->x1 = 0;
	(scroll + snum)->y1 = 0;
}

//════════════════════════════════════════════════════════════════════════════
// Funcion que mata los procesos de scroll o modo-7 que ya no se ven
//════════════════════════════════════════════════════════════════════════════
void elimina_proceso(int);

void kill_invisible(void) {
	int i, n;

	for(i = id_start; i <= id_end; i += iloc_len) {
		if(mem[i + _Status]) {
			n = 0;

			if(mem[i + _Ctype]== 1 ) {
				for(n = 0;n < 10; n++) {
					if(iscroll[n].on && (!mem[i + _Cnumber] || (mem[i + _Cnumber] & (1 << n)))) {
						break;
					}
				}
			}
			if(mem[i + _Ctype] == 2) {
				for(n = 0; n < 10; n++) {
					if(im7[n].on && (!mem[i + _Cnumber] || (mem[i + _Cnumber] & (1 << n)))) {
						break;
					}
				}
			}
			if(n == 10) {
				elimina_proceso(i);
			}
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_id(INT type)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_id(void) {
	int i, bloque;

	bloque = pila[sp];

	if(mem[id + _IdScan] == 0 || bloque !=-mem[id + _BlScan]) {
		mem[id + _BlScan] = -bloque;

		i = id_init;
	}
	else if (mem[id + _IdScan] > id_end) {
		pila[sp] = 0;

		return;
	}
	else {
		i = mem[id + _IdScan];
	}

	do {
		if(i != id && mem[i + _Bloque] == bloque && (mem[i + _Status] == 2 || mem[i + _Status] == 4)) {
			mem[id + _IdScan] = i + iloc_len;

			pila[sp] = i;

			return;
		}

		i += iloc_len;
	}while(i <= id_end);

	mem[id + _IdScan] = i;

	pila[sp] = 0;

	return;
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_distx(INT angle, INT dist)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_distx(void) {
	angulo = (float)pila[sp - 1] / radian;

	pila[sp - 1] = (int)((float)cos(angulo) * pila[sp]);

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_disty(INT angle, INT dist)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_disty(void) {
	angulo = (float)pila[sp - 1] / radian;

	pila[sp - 1] = -(int)((float)sin(angulo) * pila[sp]);

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_angle(INT id)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_angle(void) {
	bp = pila[sp];

	x = mem[bp + _X] - mem[id + _X];
	y = mem[id + _Y] - mem[bp + _Y];
	if(!x && !y) {
		pila[sp] = 0;
	}
	else {
		pila[sp] = (float)atan2(y, x) * radian;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_dist(INT id)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_dist(void) {
	int n = 1;

	bp = pila[sp];

	x = mem[bp + _X] - mem[id + _X];
	y = mem[id + _Y] - mem[bp + _Y];

	while(abs(x) + abs(y) >= 46000) {
		n *= 2;
		x /= 2;
		y /= 2;
	}

	pila[sp] = sqrt(x * x + y * y) * n;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fade(INT %r, INT %g, INT %b, INT speed)
//════════════════════════════════════════════════════════════════════════════
void DIV_fade(void) {
	int r, g, b;

	r            = pila[sp - 3];
	g            = pila[sp - 2];
	b            = pila[sp - 1];
	dacout_speed = pila[sp];

	if(r < 0) { r = 0; } else if (r > 200) { r = 200; }
	if(g < 0) { g = 0; } else if (g > 200) { g = 200; }
	if(b < 0) { b = 0; } else if (b > 200) { b = 200; }

	dacout_r = 64 -r * 64 / 100;
	dacout_g = 64 -g * 64 / 100;
	dacout_b = 64 -b * 64 / 100;
	if(now_dacout_r != dacout_r ||
	   now_dacout_g != dacout_g ||
	   now_dacout_b != dacout_b) {
		fading = 1;
	}

	sp -= 3;
	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV unload_fnt(INT fnt_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_fnt(void) {
	if(pila[sp] < 1 || pila[sp] >= max_fonts) {
		return;
	}

	if(fonts[pila[sp]] != NULL) {
		free(fonts[pila[sp]]);

		fonts[pila[sp]] = NULL;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_fnt(STRING file_name)
// - Devuelve codigo_font (fnt_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_load_fnt(void) {
	byte *ptr;
	int n, an, al, nan, ifonts;
	int old_reloj = get_reloj();

//printf("load font %s\n", (byte *)&mem[itxt + pila[sp]]);
	for(ifonts = 1; ifonts < max_fonts; ifonts++) {
		if(!fonts[ifonts]) {
			break;
		}
	}
	if(ifonts == max_fonts) {
		pila[sp] = 0;

		e(e113);
		return;
	}

	if((es = div_open_file((byte *)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e114);
	}
	else {
		fseek(es, 0, SEEK_END);
		file_len = ftell(es);

		if((ptr = (byte *)malloc(file_len)) != NULL) {
			fonts[ifonts] = ptr;

			fseek(es, 0, SEEK_SET);
			fread(ptr, 1, file_len,es);
			fclose(es);

			if(strcmp((char *)ptr, "fnt\x1a\x0d\x0a")) {
				fonts[ifonts] = 0;

				free(ptr); // moved up to avoid possible memory leaks

				e(e115);
				return;
			}

			if(process_fnt != NULL) {
				process_fnt((char *)ptr, file_len);
			}

			an  = 0;
			al  = 0;
			nan = 0;
			fnt = (t_fnt_table *)((byte *)ptr + 1356);

			for(n = 0; n < 256; n++) {
				if(fnt[n].ancho) {
					an += fnt[n].ancho;
					nan++;
				}
				if(fnt[n].alto + fnt[n].incY > al) {
					al = fnt[n].alto + fnt[n].incY;
				}
			}

			f_i[ifonts].ancho     =  an / nan;
			f_i[ifonts].espacio   = (an / nan) / 2;
			f_i[ifonts].espaciado = 0;
			f_i[ifonts].alto      = al;

			pila[sp] = ifonts;
		}
		else {
			fclose(es);

			pila[sp] = 0;

			e(e118);
		}
	}

	reloj = old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV write(INT fnt_id, INT x, INT y, INT center, INT ptr)
//════════════════════════════════════════════════════════════════════════════
void DIV_write(void) {
	int f;

	f = pila[sp - 4];

	if(f < 0 || f >= max_fonts) {
		e(e116);

		f = 0;
	}
	if(fonts[f] == 0) {
		e(e116);

		f = 0;
	}

	x = 1;
	while(texto[x].font && x < max_textos) {
		x++;
	}
	if(x < max_textos) {
		texto[x].ptr    = itxt + pila[sp--];
		texto[x].centro = pila[sp--];
		texto[x].y      = pila[sp--];
		texto[x].x      = pila[sp--];

		if(texto[x].centro < 0 || texto[x].centro > 8) {
			pila[sp] = 0;

			e(e117);
		}

		texto[x].tipo   = 0;
		texto[x].font   = (byte *)fonts[f];

		pila[sp] = x;
	}
	else {
		sp -= 4;
		pila[sp] = 0;

		e(e118);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV write_int(INT fnt_id, INT x, INT y, INT center, INT *num)
//════════════════════════════════════════════════════════════════════════════
void DIV_write_int(void) {
	int f;

	f = pila[sp - 4];

	if(f < 0 || f >= max_fonts) {
		e(e116);

		f = 0;
	}
	if(fonts[f] == 0) {
		e(e116);

		f = 0;
	}

	x = 1;
	while(texto[x].font && x < max_textos) {
		x++;
	}
	if(x < max_textos) {
		texto[x].ptr    = pila[sp--];
		texto[x].centro = pila[sp--];
		texto[x].y      = pila[sp--];
		texto[x].x      = pila[sp--];

		if(texto[x].centro < 0 || texto[x].centro > 8) {
			pila[sp] = 0;

			e(e117);
		}

		texto[x].tipo   = 1;
		texto[x].font   = (byte *)fonts[f];

		pila[sp] = x;
	}
	else {
		sp -= 4;
		pila[sp] = 0;

		e(e118);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV delete_text(INT text_id)
// - Text_id puede ser tambien "all_text"
//════════════════════════════════════════════════════════════════════════════
void DIV_delete_text(void) {
	x = pila[sp];

	if(x < max_textos && x > 0) {
		texto[x].font = 0;
	}
	else if (x == 0) {
		x = 1;
		do {
			texto[x++].font = 0;
		}while(x < max_textos);
	}
	else {
		e(e119);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV move_text(INT text_id, INT x, INT y)
//════════════════════════════════════════════════════════════════════════════
void DIV_move_text(void) {
	x = pila[sp - 2];

	if(x < max_textos && x > 0) {
		texto[x].x = pila[sp - 1];
		texto[x].y = pila[sp];
	}
	else {
		e(e119);

		sp -= 2;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV unload_fpg(INT fpg_id)
// - Devuelve el código del fichero 0..max_fpgs
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_fpg(void) {
	int c;

	c = pila[sp];

	pila[sp] = 0;

	if(c < max_fpgs && c >= 0) {
		if(g[c].fpg != 0) {
			free(g[c].fpg);
			g[c].fpg = 0;
		}
		else {
			e(e109);
		}

		if(g[c].grf != 0) {
			if(c) {
				free(g[c].grf);
				g[c].grf = 0;
			}
			else {
				memset(g[c].grf,
				       0,
				       sizeof(int *) * 1000);
			}
		}
	}
	else {
		e(e109);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV rand(INT min, INT max)
//════════════════════════════════════════════════════════════════════════════
union {
	byte b[128];
	int  d[32];
} seed; // Seed aleatorio (127 bytes + PTR)

byte divrnd(void) {
	byte ptr;

	ptr = seed.b[127];
	if((seed.b[127] = (ptr + seed.b[ptr]) & 127) == 127) {
		seed.b[127] = 0;
	}

	return (seed.b[seed.b[127]] += ++ptr);
}

int divrandom(int min, int max) {
	int r;

	if(max < min) {
		swap(max, min);
	}

	r = (((divrnd() & 127) * 256 + divrnd()) * 256 + divrnd()) * 256 + divrnd();

	return ((r % (max - min + 1)) + min);
}

void DIV_rand(void) {
	int min, max;

	max = pila[sp--];
	min = pila[sp];

	pila[sp] = divrandom(min, max);
}

//════════════════════════════════════════════════════════════════════════════
// DIV rand_seed(INT seed)
//════════════════════════════════════════════════════════════════════════════
void init_divrnd(int n){
	int a;

	for(a = 0; a < 32; a++) {
		seed.d[a] = n;
	}
	for(a = 0; a < 2048; a++) {
		divrnd();
	}
}

void DIV_rand_seed(void) {
	init_divrnd(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV define_region(INT region, INT x, INT y, INT width, INT height)
//════════════════════════════════════════════════════════════════════════════
void DIV_define_region(void) {
	int n, x, y, an, al;

	al = pila[sp--];
	an = pila[sp--];
	y  = pila[sp--];
	x  = pila[sp--];
	n  = pila[sp];

	if(x < 0)           { an += x; x = 0;  }
	if(y < 0)           { al += y; y = 0;  }
	if(x + an > vga_an) { an = vga_an - x; }
	if(y + al > vga_al) { al = vga_al - y; }
	if(an < 0 || al < 0) {
		e(e120);
		return;
	}

	if(n >= 0 && n < max_region) {
		region[n].x0 = x;
		region[n].y0 = y;
		region[n].x1 = x + an;
		region[n].y1 = y + al;

		pila[sp] = 1;
	}
	else {
		pila[sp] = 0;

		e(e108);
	}
}

//════════════════════════════════════════════════════════════════════════════════════════
// DIV xput(INT file, INT graph, INT x, INT y, INT angle, INT size, INT flags, INT region)
//════════════════════════════════════════════════════════════════════════════════════════
void DIV_xput(void) {
	int file, graf, x, y, angle, size, flags, reg;

	reg   = pila[sp--];
	flags = pila[sp--];
	size  = pila[sp--];
	angle = pila[sp--];
	y     = pila[sp--];
	x     = pila[sp--];
	graf  = pila[sp--];
	file  = pila[sp];

	put_sprite(file,
	           graf,
	           x,
	           y,
	           angle,
	           size,
	           flags,
	           reg,
	           copia2,
	           vga_an,
	           vga_al);
}

//════════════════════════════════════════════════════════════════════════════
// DIV put(INT file, INT graph, INT x, INT y)
//════════════════════════════════════════════════════════════════════════════
void DIV_put(void) {
	int file, graf, x, y;

	y    = pila[sp--];
	x    = pila[sp--];
	graf = pila[sp--];
	file = pila[sp];

	put_sprite(file,
	           graf,
	           x,
	           y,
	           0,
	           100,
	           0,
	           0,
	           copia2,
	           vga_an,
	           vga_al);
}

//═════════════════════════════════════════════════════════════════════════════════════════
// DIV map_xput(INT file, INT graph1, graph2, INT x, INT y, INT angle, INT size, INT flags)
//═════════════════════════════════════════════════════════════════════════════════════════
void DIV_map_xput(void) {
	int file, graf1, graf2, x, y, angle, size, flags;
	int *ptr;

	flags = pila[sp--];
	size  = pila[sp--];
	angle = pila[sp--];
	y     = pila[sp--];
	x     = pila[sp--];
	graf2 = pila[sp--];
	graf1 = pila[sp--];
	file  = pila[sp];

	if(file > max_fpgs || file < 0) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 <= 0 || graf1 >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	if((ptr = g[file].grf[graf1]) != NULL) {
		put_sprite(file,
		           graf2,
		           x,
		           y,
		           angle,
		           size,
		           flags,
		          -1,
		           (byte *)ptr + 64 + ptr[15] * 4,
		           ptr[13],
		           ptr[14]);
	}
	else {
		e(e121);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV map_put(INT file, INT graph1, INT graph2, INT x, INT y)
//════════════════════════════════════════════════════════════════════════════
void DIV_map_put(void) {
	int file, graf1, graf2, x, y;
	int *ptr;

	y     = pila[sp--];
	x     = pila[sp--];
	graf2 = pila[sp--];
	graf1 = pila[sp--];
	file  = pila[sp];

	if(file > max_fpgs || file < 0) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 <= 0 || graf1 >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	if((ptr = g[file].grf[graf1]) != NULL) {
		put_sprite(file,
		           graf2,
		           x,
		           y,
		           0,
		           100,
		           0,
		          -1,
		           (byte *)ptr + 64 + ptr[15] * 4,
		           ptr[13],
		           ptr[14]);
	}
	else {
		e(e121);
	}
}

//══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
// DIV map_block_copy(INT file, INT graph_dst, INT dst_x, INT dst_x, INT graph_src, INT src_x, INT src_y, INT width, INT height)
//══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
void DIV_map_block_copy(void) {
	int file, grafd, xd, yd;
	int graf, x, y, an, al;
	int *ptrd, *ptr;
	byte *_copia = copia, *si;
	int _vga_an = vga_an, _vga_al = vga_al;

	al    = pila[sp--];
	an    = pila[sp--];
	y     = pila[sp--];
	x     = pila[sp--];
	graf  = pila[sp--];
	yd    = pila[sp--];
	xd    = pila[sp--];
	grafd = pila[sp--];
	file  = pila[sp];

//printf("map block_copy file: %d\n," file);
	if(file > max_fpgs || file < 0) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(grafd <= 0 || grafd >= max_grf) {
		e(e110);
		return;
	}
	if(graf  <= 0 || graf  >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	if((ptrd = g[file].grf[grafd]) != NULL) {
		if((ptr = g[file].grf[graf]) != NULL) {
			vga_an = ptrd[13];
			vga_al = ptrd[14];
			copia  = (byte *)ptrd + 64 + ptrd[15] * 4;

			if(xd > 0)           { clipx0 = xd;      } else { clipx0 = 0;      }
			if(yd > 0)           { clipy0 = yd;      } else { clipy0 = 0;      }
			if(xd + an < vga_an) { clipx1 = xd + an; } else { clipx1 = vga_an; }
			if(yd + al < vga_al) { clipy1 = yd + al; } else { clipy1 = vga_al; }

			if(clipx0 >= vga_an || clipx1 <= 0)      { goto no; }
			if(clipy0 >= vga_al || clipy1 <= 0)      { goto no; }
			if(clipx0 >= clipx1 || clipy0 >= clipy1) { goto no; }

			an = ptr[13];
			al = ptr[14];
			si = (byte *)ptr + 64 + ptr[15] * 4;
			x  = xd - x;
			y  = yd - y;

			// Pinta sprite sin cortar
			if(x >= clipx0 && x + an <= clipx1 &&
			   y >= clipy0 && y + al <= clipy1) {
				sp_normal(si,
				          x,
				          y,
				          an,
				          al,
				          0);
			}
			// Pinta sprite cortado
			else if(x      < clipx1 && y      < clipy1 &&
			        x + an > clipx0 && y + al > clipy0) {
				sp_cortado(si,
				           x,
				           y,
				           an,
				           al,
				           0);
			}

no:
			vga_an = _vga_an;
			vga_al = _vga_al;
			copia  = _copia;
		}
		// Error
		else {
			e(e121);
		}
	}
	else {
		e(e121);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV put_screen(INT file, INT graph)
//════════════════════════════════════════════════════════════════════════════
void DIV_put_screen(void) {
	int file, graf;
	short xg, yg;
	int *ptr;

	graf = pila[sp--];
	file = pila[sp];

	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(e121);
		return;
	}

	if(ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
		xg = ptr[13] / 2;
		yg = ptr[14] / 2;
	}
	else {
		xg = *((word *)ptr + 32);
		yg = *((word *)ptr + 33);
	}

	memset(copia2,
	       0,
	       vga_an * vga_al);

	put_sprite(file,
	           graf,
	           xg,
	           yg,
	           0,
	           100,
	           0,
	           0,
	           copia2,
	           vga_an,
	           vga_al);
}

//════════════════════════════════════════════════════════════════════════════
// DIV put_pixel(INT x, INT y, INT color)
//════════════════════════════════════════════════════════════════════════════
void DIV_put_pixel(void) {
	int x, y, color;

	color = pila[sp--];
	y     = pila[sp--];
	x     = pila[sp];

	if(x >= 0      && y >= 0 &&
	   x <  vga_an && y <  vga_al) {
		*(copia2 + x + y * vga_an) = color;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_pixel(INT x, INT y)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_pixel(void) {
	int x, y;

	y = pila[sp--];
	x = pila[sp];

	if(x >= 0      && y >= 0 &&
	   x <  vga_an && y <  vga_al) {
		pila[sp] = (int)(*(copia2 + x + y * vga_an));
	}
	else {
		pila[sp] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV map_put_pixel(INT file, INT graph, INT x, INT y, INT color)
//════════════════════════════════════════════════════════════════════════════
void DIV_map_put_pixel(void) {
	int file, graf, x, y, color;
	int *ptr;
	byte *si;

	color = pila[sp--];
	y     = pila[sp--];
	x     = pila[sp--];
	graf  = pila[sp--];
	file  = pila[sp]; 

	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(e121);
		return;
	}

	if(x >= 0       && y >= 0 &&
	   x <  ptr[13] && y <  ptr[14]) {
		si = (byte *)ptr + 64 + ptr[15] * 4;
		*(si + x + y * ptr[13]) = color;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV map_get_pixel(INT file, INT graph, INT x, INT y)
//════════════════════════════════════════════════════════════════════════════
void DIV_map_get_pixel(void) {
	int file, graf, x, y;
	int *ptr;
	byte *si;

	y    = pila[sp--];
	x    = pila[sp--];
	graf = pila[sp--];
	file = pila[sp];

	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(e121);
		return;
	}

	if(x >= 0       && y >= 0 &&
	   x <  ptr[13] && y <  ptr[14]) {
		si = (byte *)ptr + 64 + ptr[15] * 4;
		pila[sp] = (int)(*(si + x + y * ptr[13]));
	}
	else {
		pila[sp] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_point(INT file, INT graph, INT npoint, INT *x, INT *y)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_point(void) {
	int file, graf, n, dx, dy;
	int *ptr;
	short *p;

	dy   = pila[sp--];
	dx   = pila[sp--];
	n    = pila[sp--];
	graf = pila[sp--];
	file = pila[sp];

	pila[sp]=0;

	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(e121);
		return;
	}

	if(n >= 0 || n < ptr[15]) {
		p = (short *)&ptr[16];

		mem[dx] = p[n * 2];
		mem[dy] = p[n * 2 + 1];
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV clear_screen()
//════════════════════════════════════════════════════════════════════════════
void DIV_clear_screen(void) {
	memset(copia2,
	       0,
	       vga_an * vga_al);

	sp++;
}

//════════════════════════════════════════════════════════════════════════════
// DIV save(STRING file_name, INT *offset, INT size)
//════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG // Versión con debugger.
FILE *open_save_file(char *file) {
	FILE *f;
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];

	strcpy(full, file);
	if((f = fopen(full, "wb")) == NULL) { // "paz\fixero.est"
		if(_fullpath(full, file, _MAX_PATH) == NULL) {
			return (NULL);
		}

		_splitpath(full, drive, dir, fname, ext);

		if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
		else                         { strcpy(full, strchr(ext, '.') + 1); }
		if(strlen(full) && file[0] != '/') {
			strcat(full, "/");
		}
		strcat(full, file);
		if((f = fopen(full, "wb")) == NULL) { // "est\paz\fixero.est"
			strcpy(full, fname);
			strcat(full, ext);
			if((f = fopen(full, "rb")) != NULL) { // Si está ya en el raiz de div ...
				fclose(f);
				if((f = fopen(full, "wb")) == NULL) { // "fixero.est"
					strcpy(full, "");

					return (NULL);
				}
				else {
					return (f);
				}
			}
			else {
				if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
				else                         { strcpy(full, strchr(ext, '.') + 1); }
				if(strlen(full)) {
					strcat(full, "/");
				}
				strcat(full, fname);
				strcat(full, ext);
				if((f = fopen(full, "wb")) == NULL) { // "est\fixero.est"
					if(strchr(ext, '.') == NULL) { mkdir(ext);                 }
					else                         { mkdir(strchr(ext,'.') + 1); }
					if((f = fopen(full, "wb")) == NULL) {  // "est\fixero.est"
						strcpy(full, fname);
						strcat(full, ext);
						if((f = fopen(full, "wb")) == NULL) { // "fixero.est"
							strcpy(full, "");

							return (NULL);
						}
						else {
							return (f);
						}
					}
					else {
						return (f);
					}
				}
				else {
					return (f);
				}
			}
		}
		else {
			return (f);
		}
	}
	else {
		return (f);
	}
}

void DIV_save(void) {
	int offset, lon;
	int old_reloj = get_reloj();

	lon    = pila[sp--];
	offset = pila[sp--];

	if(offset < long_header || offset + lon > imem_max) {
		pila[sp] = 0;

		e(e122);

		return;
	}

	es = open_save_file((byte *)&mem[itxt +pila[sp]]);
	if(es == NULL) {
		pila[sp] = 0;

		e(e123);

		return;
	}

	if(fwrite(&mem[offset], 4, lon, es) != lon) {
		e(e124);
	}

	fclose(es);

	reloj = old_reloj;
}

#else         // Versión instalaciones.

FILE *open_save_file(char *file) {
	FILE *f;
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	// fix the path
	char *ff = (char *)file;

//printf("incoming file: %s\n", file);

	while(*ff != 0) {
		if(*ff =='\\') { *ff ='/'; }
		ff++;
	}

//printf("Looking for save file: %s\n", file);
	strcpy(full, file);
	if(_fullpath(full, file, _MAX_PATH) == NULL) {
		return (NULL);
	}

	_splitpath(full, drive, dir, fname, ext);

	if(strchr(ext, '.') == NULL) { strcpy(full, ext);                  }
	else                         { strcpy(full, strchr(ext, '.') + 1); }
	if(strlen(full)) {
		strcat(full, "/");
	}
	strcat(full, fname);
	strcat(full, ext);
//printf("Looking for save file: %s\n", full);
	if((f=fopen(full, "wb")) == NULL) { // "est\fixero.est"
		strcpy(full, fname);
		strcat(full, ext);
//printf("Looking for save file: %s\n", full);
		if((f = fopen(full, "wb")) == NULL) { // "fixero.est"
			strcpy(full, "");

			return (NULL);
		}
		else {
			return (f);
		}
	}
	else {
		return (f);
	}
}

void DIV_save(void) {
	int offset, lon;
	int old_reloj = get_reloj();

	lon    = pila[sp--];
	offset = pila[sp--];

	if(offset < long_header || offset + lon > imem_max) {
		pila[sp] = 0;

		e(e122);

		return;
	}

	es = open_save_file((char *)&mem[itxt +pila[sp]]);
	if(es == NULL) {
		pila[sp] = 0;

//		e(e123); // OJO! Para que no de error en los CD-ROM

		return;
	}

//if(fwrite(&mem[offset], 4, lon, es) != lon) {
//	e(e124);
//}

	fwrite(&mem[offset], 4, lon, es);
	fclose(es);

	reloj = old_reloj;
}

#endif

//════════════════════════════════════════════════════════════════════════════
// DIV load(STRING file_name, INT *offset)
//════════════════════════════════════════════════════════════════════════════
void DIV_load(void) {
	int offset, lon;
	int old_reloj = get_reloj();

	offset = pila[sp--];

	if(offset < long_header) {
		pila[sp] = 0;

		e(e125);
		return;
	}

	if((es = div_open_file((byte *)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e126);
		return;
	}

	fseek(es, 0, SEEK_END);
	lon = ftell(es) / 4;
	fseek(es, 0, SEEK_SET);

	if(offset + lon > imem_max) {
		pila[sp] = 0;

		e(e125);
		return;
	}

	if(fread(&mem[offset], 4, lon, es) != lon) {
		e(e127);
	}

	fclose(es);

	reloj = old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_mode(INT mode)
//════════════════════════════════════════════════════════════════════════════
void DIV_set_mode(void) {
	int n;

#ifdef DEBUG
	if(v.tipo) {
		new_mode = 1; // Avisa al debugger de un cambio de modo de vídeo
	}
#endif

	vga_an = pila[sp] / 1000;
	vga_al = pila[sp] % 1000;

	dacout_r     = 64;
	dacout_g     = 64;
	dacout_b     = 64;
	dacout_speed = 8;

	while(now_dacout_r != dacout_r ||
	      now_dacout_g != dacout_g ||
	      now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();

		//DIVLoopSound();
	}

	free(copia);
	free(copia2);
#ifdef DEBUG
	free(copia_debug);
#endif

	if((copia = (byte *)malloc(vga_an * vga_al)) == NULL) {
		exer(1);
	}
	memset(copia,
	       0,
	       vga_an * vga_al);

	if((copia2 = (byte *)malloc(vga_an * vga_al)) == NULL) {
		free(copia); // memory leak fix

		exer(1);
	}
	memset(copia2,
	       0,
	       vga_an * vga_al);

#ifdef DEBUG
	if((copia_debug = (char *)malloc(vga_an * vga_al)) == NULL) {
		free(copia);  // memory leak fix
		free(copia2); // memory leak fix

		exer(1);
	}
	memset(copia_debug,
	       0,
	       vga_an * vga_al);
#endif

	if(set_video_mode != NULL) {
		set_video_mode();
	}
	else {
		svmode();
	}

	mouse_window();

	dacout_speed = 0;
	set_dac();

	for(n = 0; n < 10; n++) {
		if(iscroll[n].on) {
			pila[sp] = n;

			DIV_stop_scroll();
			iscroll[n].on = 0;
		}

		if(im7[n].on) {
			pila[sp] = n;

			DIV_stop_mode7();
			im7[n].on = 0; // fix: before iscroll[n].on = 0;
		}
	}

	region[0].x0 = 0;
	region[0].y0 = 0;
	region[0].x1 = vga_an;
	region[0].y1 = vga_al;

	pila[sp] = 0;

#ifdef DEBUG
	init_big();
#endif

	DIV_fade_on();

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_pcm(STRING file_name, INT loop)
//════════════════════════════════════════════════════════════════════════════
void DIV_load_pcm(void) {
	FILE *f;
	char *ptr;
	int loop;
	int old_reloj = get_reloj();

	loop = pila[sp--];

	if((f = div_open_file((byte *)&mem[itxt + pila[sp]])) != NULL) {
		fseek(f, 0, SEEK_END);
		file_len = ftell(f);

		if((ptr = (char *)malloc(file_len)) != NULL) {
			fseek(f, 0, SEEK_SET);
			fread(ptr, 1, file_len, f);
			fclose(f);

			pila[sp] = DIVLoadSound(ptr, file_len, loop);

			free(ptr);
		}
		else {
			fclose(f);

			pila[sp] = 0;

			e(e128);
			return;
		}
	}
	else {
		e(e128);
	}

	reloj = old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV unload_pcm(INT sound_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_pcm(void) {
	DIVUnloadSound(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV sound(INT sound_id, INT volume, INT speed)
//════════════════════════════════════════════════════════════════════════════
void DIV_sound(void) {
	int vol, fre;

	fre = pila[sp--];
	vol = pila[sp--];

	if(fre) {
		pila[sp] = DIVPlaySound(pila[sp], vol, fre);
		//if(pila[sp] == -1) {
		//	e(e129);
		//}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_sound(INT channel_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_sound(void) {
	DIVStopSound(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV change_sound(INT channel_id, INT volume, INT speed)
//════════════════════════════════════════════════════════════════════════════
void DIV_change_sound(void) {
	int vol, fre;

	fre  =pila[sp--];
	vol = pila[sp--];

	if(fre < 16) { fre = 16; }
	if(vol <  0) { vol = 0;  }

	DIVChangeSound(pila[sp], vol, fre);
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_fps(INT fps, INT fps_max, INT skips)
//════════════════════════════════════════════════════════════════════════════
void DIV_set_fps(void) {
#ifdef __EMSCRIPTEN__
	max_saltos = 2;
#else
	max_saltos = pila[sp--];
#endif
	if(max_saltos <  0) { max_saltos = 0;  }
	if(max_saltos > 10) { max_saltos = 10; }

	if(pila[sp] <   4) { pila[sp] = 4;   }
	if(pila[sp] > 100) { pila[sp] = 100; }

	//printf("setting fps(%d,%d)\n", pila[sp], max_saltos);
	dfps = pila[sp];

	ireloj = 100.0 / (double)pila[sp];
}

//════════════════════════════════════════════════════════════════════════════
// DIV start_fli(STRING file_name, INT x, INT y)
// - Devuelve nº frames
//════════════════════════════════════════════════════════════════════════════
void DIV_start_fli(void) {
	int x, y;

	y = pila[sp--];
	x = pila[sp--];

#ifdef USE_FLI
	if((es = div_open_file((byte *)&mem[itxt + pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(e147);
	}
	else {
		fclose(es);

		pila[sp] = DIVStartFLI(full, (char *)copia2, vga_an, vga_al, x, y);
		if(pila[sp] == 0) {
			e(e130);
		}
	}
//pila[sp]=StartFLI((byte*)&mem[itxt+pila[sp]],copia2,vga_an,vga_al,x,y);
//if (pila[sp]==0) e(e130);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV frame_fli()
// - Devuelve 0-fin fli, 1-continúa
//════════════════════════════════════════════════════════════════════════════
void DIV_frame_fli(void) {
	pila[++sp] = DIVNextframeFLI();
}

//════════════════════════════════════════════════════════════════════════════
// DIV end_fli()
//════════════════════════════════════════════════════════════════════════════
void DIV_end_fli(void) {
	DIVEndFLI();

	sp++;
}

//════════════════════════════════════════════════════════════════════════════
// DIV reset_fli()
//════════════════════════════════════════════════════════════════════════════
void DIV_reset_fli(void) {
	DIVResetFLI();

	sp++;
}

//════════════════════════════════════════════════════════════════════════════
// DIV system(STRING command)
//════════════════════════════════════════════════════════════════════════════
#ifndef DEBUG
void read_mouse(void) {
#ifdef DOS
	union REGS regs;

	memset(&regs,
	        0,
	        sizeof(regs));
	regs.w.ax = 3;
	int386(0x033,
	      &regs,
	      &regs);

	mouse->x = regs.w.cx >> 1;
	mouse->y = regs.w.dx;
#else
// read sdl mouse
#endif

}

#endif

void DIV_system(void) { // TODO: current don't work
	char cwork[256];

#ifdef STDOUTLOG
printf("system call not implemented yet\n");
#endif

	return;

	if(system(NULL)) {
		if(!strcmp(strupr((char *)&mem[itxt + pila[sp]]), "COMMAND.COM")) {
#ifdef DOS
			_setvideomode(_TEXTC80);
#endif
			getcwd(cwork, 256);
			system("command.com");
			chdir(cwork);

			svmode();
			set_dac();
			mouse_window();
			set_mouse(mouse->x, mouse->y);
			read_mouse();

			volcado_completo = 1;
		}
		else {
			system((char *)&mem[itxt + pila[sp]]);
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV fget_dist(INT x0, INT y0, INT x1, INT y1);
//════════════════════════════════════════════════════════════════════════════
void DIV_fget_dist(void) {
	int x0, y0, x1, y1, n = 1;

	y1 = pila[sp--];
	x1 = pila[sp--];
	y0 = pila[sp--];
	x0 = pila[sp];

	x0 = abs(x1 - x0);
	y0 = abs(y1 - y0);
	while(x0 + y0 >= 46000) {
		n  *= 2;
		x0 /= 2;
		y0 /= 2;
	}

	pila[sp] = sqrt(x0 * x0 + y0 * y0) * n;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fget_angle(INT x0, INT y0, INT x1, INT y1);
//════════════════════════════════════════════════════════════════════════════
void DIV_fget_angle(void) {
	int x0, y0, x1, y1;

	y1 = pila[sp--];
	x1 = pila[sp--];
	y0 = pila[sp--];
	x0 = pila[sp];

	x0 = x1 - x0;
	y0 = y0 - y1;

	if(!x0 && !y0) {
		pila[sp] = 0;
	}
	else {
		pila[sp] = (float)atan2(y0, x0) * radian;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV play_cd(INT track, INT mode);
//════════════════════════════════════════════════════════════════════════════
void DIV_play_cd(void) {
#ifdef DOS
	int p, m;

	m = pila[sp--];
	p = pila[sp];

	Stop_CD();

	if(p < 1) {
		return;
	}

	Play_CD(p, m);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_cd();
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_cd(void) {
#ifdef DOS
	Stop_CD();
#endif

	sp++;
}

//════════════════════════════════════════════════════════════════════════════
// DIV is_playing_cd();
//════════════════════════════════════════════════════════════════════════════
unsigned int get_cd_error(void);

void DIV_is_playing_cd(void) {
#ifdef DOS
	if(get_cd_error() & 0x200) {
		pila[++sp] = 1;
	}
	else {
		pila[++sp] = 0;
	}
#else
	pila[++sp] = 0;
#endif
}

//═══════════════════════════════════════════════════════════════════════════════════════
// DIV start_mode7(INT nmode7, INT file, INT graph1, INT graph2, INT region, INT horizon)
//═══════════════════════════════════════════════════════════════════════════════════════
void DIV_start_mode7(void) {
	int n, m, file, graf1, graf2, reg;
	int *ptr1,*ptr2;

	m     = pila[sp--];
	reg   = pila[sp--];
	graf2 = pila[sp--];
	graf1 = pila[sp--];
	file  = pila[sp--];
	n     = pila[sp];

	pila[sp] = 0;

	if(n < 0 || n > 9) {
		e(e131);
		return;
	}

	(m7 + n)->horizon = m;

	im7[n].map = NULL;
	im7[n].ext = NULL;

	if(im7[n].on) {
		pila[sp] = n;

		DIV_stop_mode7();
	}

	if(reg >= 0 && reg < max_region) {
		im7[n].x  = region[reg].x0;
		im7[n].y  = region[reg].y0;
		im7[n].an = region[reg].x1 - region[reg].x0;
		im7[n].al = region[reg].y1 - region[reg].y0;
	}
	else {
		e(e108);
		return;
	}

	if(im7[n].an == 0 || im7[n].al == 0) {
		e(e146);
		return;
	}

	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 < 0 || graf1 >= max_grf) {
		e(e110);
		return;
	}
	if(graf2 < 0 || graf2 >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	ptr1 = g[file].grf[graf1];
	ptr2 = g[file].grf[graf2];
	if(ptr1 == NULL && ptr2 == NULL) {
		e(e132);
		return;
	}
	if(ptr1 == NULL) {
		ptr1 = ptr2;
		ptr2 = NULL;
	}

	im7[n].map_an = ptr1[13];
	im7[n].map_al = ptr1[14];
	im7[n].map    = (byte *)ptr1 + 64 + ptr1[15] * 4;

	if(ptr2 != NULL) {
		im7[n].ext_an = ptr2[13];
		im7[n].ext_al = ptr2[14];
	}
	else {
		im7[n].ext_an = 0;
		im7[n].ext_al = 0; // added
	}

	switch(im7[n].ext_an) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
		case 2048:
		case 4096:
		case 8192:{
			break;
		}
		default: {
			im7[n].ext_an = 0;
		}
	}

	switch(im7[n].ext_al) {
		case 1:
		case 2:
		case 4:
		case 8:
		case 16:
		case 32:
		case 64:
		case 128:
		case 256:
		case 512:
		case 1024:
		case 2048:
		case 4096:
		case 8192: {
			break;
		}
		default: {
			im7[n].ext_al = 0;
		}
	}

	if(im7[n].ext_an && im7[n].ext_al) {
		im7[n].ext = (byte *)ptr2 + 64 + ptr2[15] * 4;
	}
	else {
		im7[n].ext = NULL;
	}

	im7[n].on = 1; // Al final si no ha habido errores, fija la variable m7
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_mode7(INT nmode7)
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_mode7(void) {
	int n;

	n = pila[sp];

	if(n < 0 || n > 9) {
		e(e131);
		return;
	}

	im7[n].on  = 0;
	im7[n].map = NULL;
	im7[n].ext = NULL;

	kill_invisible();
}

//════════════════════════════════════════════════════════════════════════════
// DIV advance(INT dist)
//════════════════════════════════════════════════════════════════════════════
void DIV_advance(void) {
	mem[id + _X] += get_distx(mem[id + _Angle], pila[sp]);
	mem[id + _Y] += get_disty(mem[id + _Angle], pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV abs(INT value)
//════════════════════════════════════════════════════════════════════════════
void DIV_abs(void) {
	pila[sp] = abs(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV fade_on()
//════════════════════════════════════════════════════════════════════════════
void DIV_fade_on(void) {
	dacout_r     = 0;
	dacout_g     = 0;
	dacout_b     = 0;
	dacout_speed = 8;
	if(now_dacout_r != dacout_r ||
	   now_dacout_g != dacout_g ||
	   now_dacout_b != dacout_b) {
		fading = 1;
	}

	sp++;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fade_off()
//════════════════════════════════════════════════════════════════════════════
void DIV_fade_off(void) {
	int old_reloj = get_reloj();

	dacout_r     = 64;
	dacout_g     = 64;
	dacout_b     = 64;
	dacout_speed = 8;
	while(now_dacout_r != dacout_r ||
	      now_dacout_g != dacout_g ||
	      now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();

		//DIVLoopSound();
	}
	fading=0;

	sp++;

	reloj = old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV sqrt(INT x)
//════════════════════════════════════════════════════════════════════════════
void DIV_sqrt(void) {
	int x;

	x = abs(pila[sp]);

	if(x < 2147483648) {
		pila[sp] = sqrt(x);
	}
	else {
		pila[sp] = 999999999;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV pow(INT n, INT m)
//════════════════════════════════════════════════════════════════════════════
void DIV_pow(void) {
	int n, m;

	m = pila[sp--];
	n = pila[sp];

	if(m > 1) {
		m--;
		do {
			pila[sp] *= n;
		}while(--m);
	}
	else if(m == 0) {
		pila[sp] = 1;
	}
	else if (m < 0) {
		pila[sp] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV near_angle(INT angle1, INT angle2, INT angle_inc)
// - Devuelve "new_angle1"
//════════════════════════════════════════════════════════════════════════════
void DIV_near_angle(void) {
	int a1, a2, i;

	i  = abs(pila[sp--]);
	a2 = pila[sp--];
	a1 = pila[sp];

	while(a1 < a2 - pi) { a1 += 2 * pi; }
	while(a1 > a2 + pi) { a1 -= 2 * pi; }

	if(a1 < a2) { a1 += i; if(a1 > a2) { a1 = a2; } }
	else        { a1 -= i; if(a1 < a2) { a1 = a2; } }

	pila[sp] = a1;
}

//════════════════════════════════════════════════════════════════════════════
// DIV let_me_alone()
//════════════════════════════════════════════════════════════════════════════
void DIV_let_me_alone(void) {
	int i;

	for(i = id_start; i <= id_end; i += iloc_len) {
		if(i != id && mem[i + _Status]) {
			mem[i + _Status] = 1;
		}
	}

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV exit(STRING message, INT error_level)
//════════════════════════════════════════════════════════════════════════════
void DIV_exit(void) {
#ifdef DEBUG
	FILE *f;
#endif

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

#ifdef DEBUG
	if((f = fopen("system/exec.err", "wb")) != NULL) {
		fwrite("\x0\x0\x0\x0", 4, 1, f);
		fwrite(&pila[sp], 4, 1, f);
		fwrite(&mem[itxt + pila[sp - 1]], 1, strlen((char *)(&mem[itxt + pila[sp - 1]])) + 1, f);
		fclose(f);
	}
#else
	//printf("%s",&mem[itxt+pila[sp-1]]);
#endif

	exit(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV roll_palette(INT start_color, INT num_colors, INT inc)
//════════════════════════════════════════════════════════════════════════════
void DIV_roll_palette(void) {
	int c, n, i, x, color;
	char pal[768];

	i = pila[sp--];
	n = abs(pila[sp--]);
	c = abs(pila[sp]) % 256;

	if(n + c > 256) {
		n = 256 - c;
	}
	for(x = c; x < c + n; x++) {
		color = x + i - c;
		while(color <  0) { color += n; }
		while(color >= n) { color -= n; }
		color += c;

		memcpy(&pal[color * 3],
		       &paleta[x * 3],
		        3);
	}
	memcpy(&paleta[c * 3],
	       &pal[c * 3],
	        n * 3);
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_real_point(INT npoint, INT *x, INT *y)
//════════════════════════════════════════════════════════════════════════════
void DIV_get_real_point(void) {
	int x, y, an, al, xg, yg;
	int n, dx, dy, px, py;
	int *ptr;
	float ang, dis;
	short *p;

	dy = pila[sp--];
	dx = pila[sp--];
	n  = pila[sp];

	if(mem[id + _File] > max_fpgs || mem[id + _File] < 0) {
		e(e109);
		return;
	}
	if(mem[id + _File]) { max_grf = 1000; } else { max_grf = 2000; }
	if(mem[id + _Graph] <= 0 || mem[id + _Graph] >= max_grf) {
		e(e110);
		return;
	}
	if(g[mem[id + _File]].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[mem[id + _File]].grf[mem[id + _Graph]]) == NULL) {
		e(e121);
		return;
	}

	if(n >= 0 || n < ptr[15]) {
		p  = (short *)&ptr[16];
		px = p[n * 2];
		py = p[n * 2 + 1];

		x = mem[id + _X];
		y = mem[id + _Y];
		if(mem[id + _Resolution] > 0) {
			x /= mem[id + _Resolution];
			y /= mem[id + _Resolution];
		}

		an = ptr[13];
		al = ptr[14];

		if(ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] / 2;
		}
		else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		if(mem[id + _Angle] != 0) {
			px -= xg;
			py -= yg;
			if(!px && !py) {
				px = x;
				py = y;
			}
			else {
				if(!px) {
					if(py > 0) {
						ang = -1.5708;
					}
					else {
						ang = 1.5708;
					}
				}
				else {
					ang = atan2(-py, px);
				}
				ang += ((float)mem[id + _Angle]) / radian;
				dis  = sqrt(px * px + py * py) * mem[id + _Size] / 100;

				if(mem[id + _Flags] & 1) {
					px = x - cos(ang) * dis;
				}
				else {
					px = x + cos(ang) * dis;
				}
				if(mem[id + _Flags] & 2) {
					py = y + sin(ang) * dis;
				}
				else {
					py = y - sin(ang) * dis;
				}
			}
		}
		else if(mem[id + _Size] != 100) {
			if(mem[id + _Flags] & 1) {
				px = x + (xg - px) * mem[id + _Size] / 100;
			}
			else {
				px = x + (px - xg) * mem[id + _Size] / 100;
			}
			if(mem[id + _Flags] & 2) {
				py = y + (yg - py) * mem[id + _Size] / 100;
			}
			else {
				py = y + (py - yg) * mem[id + _Size] / 100;
			}
		}
		else {
			if(mem[id + _Flags] & 1) {
				px = x + xg - px;
			}
			else {
				px += x - xg;
			}
			if(mem[id + _Flags] & 2) {
				py = y + yg - py;
			}
			else {
				py += y - yg;
			}
		}

		if(mem[id + _Resolution] > 0) {
			px *= mem[id + _Resolution];
			py *= mem[id + _Resolution];
		}

		mem[dx] = px;
		mem[dy] = py;
	}
	else {
		e(e133);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_joy_button(INT button)
// - Button = 0..3
//════════════════════════════════════════════════════════════════════════════
#define GAME_PORT  0x201
#define TIMER_PORT 0x40
#define TIME_OUT   2000

void DIV_get_joy_button(void) { // TODO: support function for modern platforms
	if(pila[sp] < 0 || pila[sp] > 3) {
		pila[sp] = 0;

		e(e134);
		return;
	}

#ifdef DOS
	if(inp(GAME_PORT) & (1 << (4 + pila[sp]))) {
		pila[sp] = 0;
	}
	else {
		pila[sp] = 1;
	}
#else
	pila[sp] = 1;
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_joy_position(INT axis)
// - Axis = 0..3
//════════════════════════════════════════════════════════════════════════════
int ej[4] = { -1, -1, -1, -1};

int joy_position(int eje) {
#ifdef DOS
	unsigned start, finish, result;
	nt i, mask = 1 << eje;

	_disable();

	outp(TIMER_PORT + 3,0);
	start  = inp(TIMER_PORT);
	start += inp(TIMER_PORT) << 8;
	outp(GAME_PORT, 0);

	for(i = 0; i < TIME_OUT; i++) {
		if((inp(GAME_PORT) & mask) == 0) {
			break;
		}
	}
	if(i == TIME_OUT) {
		_enable();

		joy_timeout++;

		return (0);
	}
	else {
		joy_timeout = 0;
	}

	outp(TIMER_PORT + 3, 0);
	finish  = inp(TIMER_PORT);
	finish += inp(TIMER_PORT) << 8;

	_enable();

	if(start >= finish) {
		result = start-finish;
	}
	else {
		result = 0xffff - finish + start;
	}

	for(i = 0; i < TIME_OUT / 2; i++) {
		if((inp(GAME_PORT) & 0x0f) == 0) {
			break;
		}
	}

	result = ((result & 0x1ff0) >> 4) * 100;

	if(ej[eje] == -1) {
		ej[eje] = result;
	}
	else {
		if(result > 0 && result < 25600) {
			if(joy_filter < 0)         { joy_filter = 0;   }
			else if (joy_filter > 100) { joy_filter = 100; }

			ej[eje] = (result * (100 - joy_filter) + ej[eje] * joy_filter) / 100;
		}

		result = ej[eje];
	}

	return (result / 100);
#endif
//printf("get joy pos\n");

	return (0);
}

void DIV_get_joy_position(void) {
	if(pila[sp] < 0 || pila[sp] > 3) {
		pila[sp] = 0;

		e(e134);
		return;
	}
	else {
		pila[sp] = joy_position(pila[sp]);
	}
}

//════════════════════════════════════════════════════════════════════════════
// read_joy()
// - Lectura del joystick con auto-calibración - INTERNA
//════════════════════════════════════════════════════════════════════════════
int joy_cx = 0, joy_cy = 0, joy_x0, joy_x1, joy_y0, joy_y1, init_joy = 0;

void read_joy(void) {
#ifdef DOS
	int n, x, y;

	n = inp(GAME_PORT);
	if(n &  16) { joy->button1 = 0; } else { joy->button1 = 1; }
	if(n &  32) { joy->button2 = 0; } else { joy->button2 = 1; }
	if(n &  64) { joy->button3 = 0; } else { joy->button3 = 1; }
	if(n & 128) { joy->button4 = 0; } else { joy->button4 = 1; }

	x = joy_position(0);
	y = joy_position(1);

	if(init_joy <= 10) {
		// Los timeout no los tiene en cuenta
		if(x == 0 || y == 0) {
			return;
		}

		if(init_joy < 10) {
			joy_cx += x;
			joy_cy += y;
		}
		else {
			joy_cx /= 10;
			joy_cy /= 10;
			joy_x0 = joy_cx - 16;
			joy_x1 = joy_cx + 16;
			joy_y0 = joy_cy - 16;
			joy_y1 = joy_cy + 16;
		}

		init_joy++;

		joy->left  = 0;
		joy->right = 0;

		return;
	}
	else {
		if(x < joy_x0) {
			joy_x0 = x;
			if(joy_x1 < joy_cx + ((joy_cx - x) * 8) / 10) {
				joy_x1 = joy_cx + ((joy_cx - x) * 8) / 10;
			}
		}
		else if (x > joy_x1) {
			joy_x1 = x;
			if(joy_x0 > joy_cx + ((joy_cx - x) * 8) / 10) {
				joy_x0 = joy_cx + ((joy_cx - x) * 8) / 10;
			}
		}

		if(y < joy_y0) {
			joy_y0 = y;
			if(joy_y1 < joy_cy + ((joy_cy - y) * 8) / 10) {
				joy_y1 = joy_cy + ((joy_cy - y) * 8) / 10;
			}
		}
		else if(y > joy_y1) {
			joy_y1 = y;
			if(joy_y0 > joy_cy + ((joy_cy - y) * 8) / 10) {
				joy_y0 = joy_cy + ((joy_cy - y) * 8) / 10;
			}
		}
	}

	if(x > (joy_x0 * 2 + joy_x1 * 3) / 5) {
		joy->left  = 0;
		joy->right = 1;
	}
	else if(x < (joy_x0 * 3 + joy_x1 * 2) / 5) {
		joy->left  = 1;
		joy->right = 0;
	}
	else {
		joy->left  = 0;
		joy->right = 0;
	}

	if(y > (joy_y0 * 2 + joy_y1 * 3) / 5) {
		joy->up   = 0;
		joy->down = 1;
	}
	else if(y < (joy_y0 * 3 + joy_y1 * 2) / 5) {
		joy->up   = 1;
		joy->down = 0;
	}
	else {
		joy->up   = 0;
		joy->down = 0;
	}
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV convert_palette(INT file, INT graph, INT *new_palette)
//════════════════════════════════════════════════════════════════════════════
void DIV_convert_palette(void) {
	int file, graf, pal_ofs;
	int *ptr, n;
	byte *si;

	pal_ofs = pila[sp--];
	graf    = pila[sp--];
	file    = pila[sp];

	if(pal_ofs < long_header || pal_ofs + 256 >= imem_max) {
		e(e136);
		return;
	}
	if(file < 0 || file > max_fpgs) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(e121);
		return;
	}

	n  = ptr[13] * ptr[14];
	si = (byte *)ptr + 64 + ptr[15] * 4;
	do {
		*si = (byte)mem[pal_ofs + *si];
		 si++;
	}while(--n);
}

//════════════════════════════════════════════════════════════════════════════
// DIV reset_sound()
//════════════════════════════════════════════════════════════════════════════
void DIVEndSound2(void);
int  DIVInitSound2(int Tarj, int Port, int Irq, int Dma, int Dma2, int master, int sound_fx, int cd_audio);

void DIV_reset_sound(void) {
#ifdef DOS
	DIVEndSound2();
	DIVInitSound2(setup->card,
	              setup->port,
	              setup->irq,
	              setup->dma,
	              setup->dma2,
	              setup->master,
	              setup->sound_fx,
	              setup->cd_audio);
#endif

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_volume()
//════════════════════════════════════════════════════════════════════════════
void DIVSetMasterVolume(word);
void DIVSetVocVolume(word);
void DIVSetCDVolume(word);

void DIV_set_volume(void) {
	if(setup->master        <  0) { setup->master   = 0;  }
	else if(setup->master   > 15) { setup->master   = 15; }
	if(setup->sound_fx      <  0) { setup->sound_fx = 0;  }
	else if(setup->sound_fx > 15) { setup->sound_fx = 15; }
	if(setup->cd_audio      <  0) { setup->cd_audio = 0;  }
	else if(setup->cd_audio > 15) { setup->cd_audio = 15; }

#ifdef DOS
	DIVSetMasterVolume(setup->master);
	DIVSetVocVolume(setup->sound_fx);
	DIVSetCDVolume(setup->cd_audio);
#endif

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_color(INT color, INT r, INT g, INT b)
//════════════════════════════════════════════════════════════════════════════
void DIV_set_color(void) {
	int color, r, g, b;

	b     = abs(pila[sp--]) % 64;
	g     = abs(pila[sp--]) % 64;
	r     = abs(pila[sp--]) % 64;
	color = abs(pila[sp]  ) % 256;

	paleta[color * 3    ] = r;
	paleta[color * 3 + 1] = g;
	paleta[color * 3 + 2] = b;

	if(!activar_paleta) {
		activar_paleta = 1;
	}
}
