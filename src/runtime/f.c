//════════════════════════════════════════════════════════════════════════════
// Internal functions code
//════════════════════════════════════════════════════════════════════════════
#include <errno.h>

#include "inter.h"

#include "cdrom.h"
#include "divmixer.hpp"
#include "divsound.h"
#ifdef NETPLAY
#include "net.h"
#include "netlib.h"
#endif

extern int max_reloj;

extern int omitidos[128];
extern int nomitidos;

// Prototipes
#ifdef __EMSCRIPTEN__
extern void es_fps(byte);
#endif

FILE *div_open_file(char *file);
void readmouse(void);
int  get_ticks(void);
void function_exec(int, int);

int  es_PCX(byte *buffer);
void adaptar(byte *ptr, int len, byte *pal, byte *xlat);
void set_env_color(void);

void texn2(byte *copia, int vga_an, byte *p, int x, int y, byte an, int al);

void kill_invisible(void);

int  joy_position(int eje);

void signal_tree(int p, int s);

void get_token(void);
void expres0(void);
void expres1(void);
void expres2(void);
void expres3(void);
void expres4(void);
void expres5(void);

//void _encriptar(int encode, char *fichero, char *clave);
//void _comprimir(int encode, char *fichero);

void m8_object_avance(int ide, int angulo, int velocidad);

// DIV prototypes
void DIV_fade_on(void);
void DIV_fade_off(void);

void DIV_load_pal(void);

void DIV_put_screen(void);

void DIV_path_find(void);
void DIV_path_line(void);
void DIV_path_free(void);

void DIV_stop_scroll(void);
void DIV_stop_mode7(void);

void DIV_set_sector_height(void);
void DIV_get_sector_height(void);
void DIV_go_to_flag(void);
void DIV_set_point_m8(void);
void DIV_get_point_m8(void);
void DIV_start_mode8(void);
void DIV_stop_mode8(void);
void DIV_load_wld(void);
void DIV_set_fog(void);
void DIV_set_sector_texture(void);
void DIV_get_sector_texture(void);
void DIV_set_wall_texture(void);
void DIV_get_wall_texture(void);


// dummy function for missing mode8
#if !defined(MODE8) && !defined(NEWMODE8)
void m8_object_avance(int ide, int angulo, int velocidad) {
	mem[id + _X] += get_distx(mem[id + _Angle], pila[sp]);
	mem[id + _Y] += get_disty(mem[id + _Angle], pila[sp]);

//printf("DUMMY - object advance\n");
}
#endif

//════════════════════════════════════════════════════════════════════════════
//  Para arreglar el puto bug de /oneatx /fp5 en i.cpp
//════════════════════════════════════════════════════════════════════════════
static int n_reloj = 0, o_reloj = 0;

int get_reloj(void) {
//	reloj = SDL_GetTicks() / 10;

//	return reloj;

	n_reloj  = OSDEP_GetTicks();
	reloj   += (n_reloj - o_reloj);
	o_reloj  = n_reloj;

//reloj = SDL_GetTicks() / 10;

	return reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV signal(INT process, INT signal)
//════════════════════════════════════════════════════════════════════════════
void signal_tree(int p, int s) {
	do {
		mem[p + _Status] = s;
		if(mem[p + _Son]) {
			signal_tree(mem[p + _Son], s);
		}

		p = mem[p + _BigBro];
	}while(p);
}

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
			pila[--sp] = 0; // Returns 0 if the process was dead
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
						signal_tree(mem[i + _Son], pila[sp] - 99);
					}
				}
			}
		}

		pila[--sp] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV key(INT key_code)
//════════════════════════════════════════════════════════════════════════════
void DIV_key(void) {
	if(pila[sp] <= 0 || pila[sp] >= 128) {
		e(101);
		return;
	}

	pila[sp] = key(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
//  Funcion para localizar y abrir un fichero (pal,fpg,fnt,...)
//  Esta funci�n debe seguir el mismo algoritmo en F.CPP y DIVC.CPP
//════════════════════════════════════════════════════════════════════════════
char full[_MAX_PATH + 1];

#ifdef DEBUG
FILE *__fpopen(byte *file, char *mode) {
	char fprgpath[_MAX_PATH * 2];
	FILE *f;

	strcpy(fprgpath, prgpath);
	strcat(fprgpath, "/");
	strcat(fprgpath, full);

//fprintf(stdout, "Trying to open %s\n", fprgpath);

	if((f = fopen(fprgpath, mode))) { // prgpath/file
//printf("Found %s in prg dir [%s]\n", file, prgpath);
		strcpy(full, fprgpath);

		return f;
	}

	return NULL;
}

FILE *fpopen(byte *file, char *mode) {
	return __fpopen(file, mode);
}
#endif

FILE *open_multi(char *file, char *mode) {
	FILE *f;
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];

	char remote[255];

	char *ff = (char *)file;

	while(*ff != 0) {
		if(*ff =='\\') {
			*ff = '/';
		}

		ff++;
	}

	strcpy(full, (char*)file); // full filename
//fprintf(stdout,"Trying to open %s\n", file);
#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

//fprintf(stdout,"Trying to open %s\n", full);

	// "paz\fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

	if(_fullpath(full, (char*)file, _MAX_PATH) == NULL) {
		return (NULL);
	}

	_splitpath(full, drive, dir, fname, ext);

	if(strchr(ext, '.') == NULL) {
		strcpy(full, ext);
	}
	else {
		strcpy(full, strchr(ext, '.') + 1);
	}
	if(strlen(full) && file[0] != '/') {
		strcat(full, "/");
	}
	strcat(full, (char*)file);

//fprintf(stdout, "Trying to open %s\n", full);

	// "est\paz\fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	strupr(full);

//fprintf(stdout, "Trying to open %s\n", full);

	// "est\paz\fixero.est"
	if((f = fopen(full,mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	strcpy(full, fname);
	strcat(full, ext);

//fprintf(stdout, "Trying to open %s\n", full);

	// "fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	strupr(full);

//fprintf(stdout, "Trying to open %s\n", full);

	// "fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	strlwr(full);

//fprintf(stdout,"Trying to open %s\n",full);

	// "fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	if(strchr(ext, '.') == NULL) {
		strcpy(full, ext);
	}
	else {
		strcpy(full, strchr(ext, '.') + 1);
	}
	if(strlen(full)) {
		strcat(full, "/");
	}
	strcat(full, fname);
	strcat(full, ext);

	// "est\fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

	strlwr(full);

	// "est\fixero.est"
	if((f = fopen(full, mode))) {
		return f;
	}

#ifdef DEBUG
	if(f = fpopen(full, mode)) {
		return f;
	}
#endif

#ifdef ZLIB
//fprintf(stdout, "Trying to open from zip %s\n", file);

	if(mode[0] != 'w') {
		if(f = memz_open_file(file)) {
			return f;
		}
	}
#endif

	return NULL;
}

FILE *div_open_file(char *file) {
	FILE *f, *fe;
	char *ff = (char *)file;

#ifdef DEBUG
printf("opening file: [%s]\n", file);
#endif

	if(strlen((const char *)file) < 1) {
		return NULL;
	}

	if(strlen((char *)file) == 0) {
		return NULL;
	}

	f = open_multi(file, "rb");
	if(f == NULL) {
		strcpy(full, "");
	}

	return (f);
}

//════════════════════════════════════════════════════════════════════════════
//  Al guardar un archivo (save*()), lo quita del packfile (si est�)
//════════════════════════════════════════════════════════════════════════════
void packfile_del(char *file) {
	FILE *f;
	char full [_MAX_PATH  + 1];
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	int n;

	if(_fullpath(full, (char*)file ,_MAX_PATH) == NULL) {
		return;
	}

	_splitpath(full, drive, dir, fname, ext);

	strcpy(full,fname);
	strcat(full,ext);
	strupr(full);

	for(n = 0; n < npackfiles; n++) {
		if(!strcmp(full, packdir[n].filename)) {
			break;
		}
	}

	// Si el archivo est� en el packfile ...
	if(n < npackfiles) {
		if((f = fopen(packfile, "rb+")) != NULL) {
			strcpy(packdir[n].filename, "");

			fseek(f, 24, SEEK_SET);
			fwrite(packdir, sizeof(t_packdir), npackfiles, f);
			fseek(f, 0, SEEK_END);
			fclose(f);
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Lee un archivo del packfile (en byte * packptr)
// devuelve: -1 Not found, -2 Not enough memory, N Longitud fichero
//════════════════════════════════════════════════════════════════════════════
int read_packfile(byte *file) {
	FILE *f;
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	char *ptr;
	int n;
	unsigned long len_desc;

//fprintf(stdout, "trying to read %s from %d files\n", file, npackfiles);

	if(_fullpath(full, (char *)file, _MAX_PATH) == NULL) {
		return (-1);
	}

	char *ff = (char *)file;

	while(*ff != 0) {
		if(*ff =='\\') {
			*ff = '/';
		}

		ff++;
	}

	strcpy(full, (char*)file);

	_splitpath(full, drive, dir, fname, ext);

	strcpy(full, fname);
	strcat(full, ext);
	strupr(full);

	for(n = 0; n < npackfiles; n++) {
//printf("looking for %s against %s\n", full, packdir[n].filename);

		if(!strcmp(full, packdir[n].filename)) {
			break;
		}
	}
	if(n < npackfiles) {
		len_desc = packdir[n].len_desc;

		if((packptr = (byte *)malloc(len_desc)) != NULL) {
			if((ptr = (char *)malloc(packdir[n].len)) != NULL) {
				if((f = fopen(packfile, "rb")) == NULL) {
#ifndef DEBUG
#ifdef ZLIB
					f = memz_open_file(packfile);
#endif
#endif
				}
				if(f != NULL) {
					fseek(f, packdir[n].offset, SEEK_SET);
					fread(ptr, 1, packdir[n].len, f);
					fclose(f);

#ifdef ZLIB
					if(!uncompress(packptr, &len_desc, (byte *)ptr, packdir[n].len))
#else
					if(false)
#endif
					{
						free(ptr);

						return (packdir[n].len_desc);
					}
					else {
						free(ptr);
						free(packptr);

						return (-2);
					}
				}
				else {
					free(ptr);
					free(packptr);

					return (-1);
				}
			}
			else {
				free(packptr);

				return (-2);
			}
		}
		else {
			return (-2);
		}
	}
	else {
		return (-1);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV foad_pal(STRING file_name)
//════════════════════════════════════════════════════════════════════════════
int hacer_fade = 0;

void DIV_force_pal(void) {
	adaptar_paleta = 0;

	if(pila[sp]) {
		DIV_load_pal();

		if(pila[sp]) {
			adaptar_paleta = 1;
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_pal(STRING file_name)
//════════════════════════════════════════════════════════════════════════════
void nueva_paleta(void) {
	byte *p, c0, c1;
	int n;

	if(process_palette != NULL) {
		process_palette();
	}

	palcrc = 0;
	for(n = 0; n < 768; n++) {
		palcrc += (int)paleta[n];
		palcrc <<= 1;
	}

	memcpy(dac,
	       paleta,
	       768);

	init_ghost();
	crear_ghost();

	find_color(0,   0,  0); c0 = find_col;
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

void DIV_load_pal(void) {
	byte pal[1352];
	int m, offs = 8;

	if(adaptar_paleta) {
		pila[sp] = 0;

		e(183);
		return;
	}

	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto palfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <=  0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		memcpy(pal,
		       packptr,
		       352);

		free(packptr);
	}
	else {
palfuera:
		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = 0;

			e(102);
			return;
		}
		else {
			fread(pal, 1, 1352, es);
			fclose(es);
		}
	}

	// not a PAL file
	if(strcmp((char *)pal, "pal\x1a\x0d\x0a")) {
		// not a FPG file
		if(strcmp((char *)pal, "fpg\x1a\x0d\x0a")) {
			// not a FNT file
			if(strcmp((char *)pal, "fnt\x1a\x0d\x0a")) {
				// not a MAP file
				if(strcmp((char *)pal, "map\x1a\x0d\x0a")) {
					// Take the PCX palette
					if(es_PCX((byte*)pal)) {
						if(npackfiles) {
							m = read_packfile((byte *)&mem[pila[sp]]);

							if(m == -1) {
								goto palfuera2;
							}
							if(m == -2) {
								pila[sp] = 0;

								e(100);
								return;
							}
							if(m <=  0) {
								pila[sp] = 0;

								e(200);
								return;
							}

							memcpy(pal,
							       packptr + m - 768,
							       768);

							free(packptr);
						}
						else {
palfuera2:
							if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
								pila[sp] = 0;

								e(102);
								return;
							}
							else {
								fseek(es, -768, SEEK_END);
								fread(pal, 1, 768, es);
								fclose(es);
							}
						}

						for(m = 0; m < 768; m++) {
							pal[m] /= 4;
						}

						offs = 0;
					}
					else {
						pila[sp] = 0;

						e(103);
						return;
					}
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
			hacer_fade = 1;

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

//════════════════════════════════════════════════════════════════════════════
// DIV unload_map(INT code)
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_map(void) {
	if(pila[sp] < 1000 || pila[sp] > 1999) {
		return;
	}

	if(g[0].grf[pila[sp]] != 0) {
//printf("Freeing %lx\n ", (g[0].grf[pila[sp]]));//-1330);

		free((byte *)(g[0].grf[pila[sp]]) - 1330);
		g[0].grf[pila[sp]] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_map(STIRNG file_name)
// - Devuelve el c�digo del gr�fico (1000..1999)
//════════════════════════════════════════════════════════════════════════════
typedef struct _t_pcx_header {
	char  manufacturer;
	char  version;
	char  encoding;
	char  bits_per_pixel;
	short xmin, ymin;
	short xmax, ymax;
	short hres;
	short vres;
	char  palette16[48];
	char  reserved;
	char  color_planes;
	short bytes_per_line;
	short palette_type;
	short Hresol;
	short Vresol;
	char  filler[54];
} t_pcx_header;

typedef struct _t_pcx_struct {
	t_pcx_header       header;
	unsigned char far *cimage;
	unsigned char      palette[3 * 256];
	unsigned char far *image;
	int                clength;
} t_pcx_struct;

int es_PCX(byte *buffer) {
	int loes = 0;

	if(buffer[2] == 1 && buffer[3] == 8 && buffer[65] == 1) {
		loes = 1;
	}

	return (loes);
}

byte *pcxdac;

void descomprime_PCX(byte *buffer, byte *mapa) {
	unsigned int con;
	unsigned int pixel = 0, pixel_line = 0;
	unsigned int last_byte, bytes_line;
	char ch, rep;
	t_pcx_header header;
	byte *pDest;
	int map_an, map_al;

	memcpy((byte *)&header,
	       buffer,
	       sizeof(t_pcx_header));

	memset(mapa,
	       0,
	       map_an * map_al);

	last_byte   = header.bytes_per_line * header.color_planes * map_al;
	bytes_line  = header.bytes_per_line * header.color_planes;
	map_an      = header.xmax - header.xmin + 1;
	map_al      = header.ymax - header.ymin + 1;
	buffer     += 128; // Comienzo de la imagen
	pDest       = mapa;

	do {
		// Copia uno por defecto.
		ch = *buffer++;
		// Si RLE entonces
		if((ch & 192) == 192) {
			// rep = n� de veces a copiar.
			rep = (ch & 63);
			ch  = *buffer++;
		}
		else {
			rep = 1;
		}

		// Controla que no nos salgamos.
		pixel_line += rep;
		pixel      += rep;
		if(pixel > last_byte) {
			rep -= pixel - last_byte;
			for(con = 0; con < rep; con++) {
				*pDest++ = ch;
			}
			break;
		}
		if(pixel_line == bytes_line) {
			pixel_line  = 0;
			rep        -= bytes_line - map_an;
		}

		for(con = 0; con < rep; con++) {
			*pDest++ = ch;
		}
	}while(1);

	for(con = 0; con < 768; con++) {
		buffer[con] /= 4;
	}
	pcxdac = buffer;

	if(!paleta_cargada) {
		for(con = 0; con < 768; con++) {
			if(buffer[con] != paleta[con]) {
				break;
			}
		}
		if(con < 768) {
			dr = dacout_r;
			dg = dacout_g;
			db = dacout_b;

			if(dr < 63 || dg < 63 || db < 63) {
				hacer_fade = 1;

				DIV_fade_off();

				sp--;
			}

			memcpy(paleta,
			       buffer,
			       768);

			nueva_paleta();
		}

		paleta_cargada = 1;
	}
}

void DIV_load_map(void) {
	int ancho, alto, npuntos, m;
	byte *ptr, *buffer;
	t_pcx_header header;

	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto mapfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <=  0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		ptr      = packptr;
		file_len = m;
	}
	else {
mapfuera:
		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = 0;

			e(143);
			return;
		}
		else {
			fseek(es, 0, SEEK_END);
			file_len = ftell(es);
			if((ptr = (byte *)malloc(file_len)) != NULL) {
				fseek(es, 0, SEEK_SET);
				fread(ptr, 1, file_len, es);
				fclose(es);
			}
			else {
				fclose(es);

				pila[sp] = 0;

				e(100);
				return;
			}
		}
	}

	// MAP
	if(!strcmp((char *)ptr, "map\x1a\x0d\x0a")) {
		if(process_map != NULL) {
			process_map((char*)ptr, file_len);
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
				       768);

				nueva_paleta();
			}

			paleta_cargada = 1;
		}

		ancho   = *(word *)(ptr + 8);
		alto    = *(word *)(ptr + 10);
		npuntos = *(word *)(ptr + 1392);

		adaptar(ptr + 1394 + npuntos * 4,
		        ancho * alto,
		        ptr + 48,
		        NULL);

		ptr = ptr + 1394 - 64;
		*((int*)ptr + 13) = ancho;
		*((int*)ptr + 14) = alto;
		*((int*)ptr + 15) = npuntos;

		while(g[0].grf[next_map_code]) {
			if(next_map_code++ == 1999) {
				next_map_code = 1000;
			}
		}
		g[0].grf[next_map_code] = (int *)ptr;

		pila[sp] = next_map_code;
	}
	// PCX
	else if(es_PCX(ptr)) {
		memcpy((byte *)&header,
		       ptr,
		       sizeof(t_pcx_header));

		ancho   = header.xmax - header.xmin + 1;
		alto    = header.ymax - header.ymin + 1;
		npuntos = 0;

		if((!ancho && !alto) || ancho < 0 || alto < 0) {
			free(ptr); // Fix: memory leak

			e(144);
			return;
		}

		buffer = (byte *)malloc(1394 + ancho * alto);
		if(buffer == NULL) { // Fix: insufficient memory check
			free(ptr); // Fix: memory leak

			e(100);
			return;
		}

		descomprime_PCX(ptr, &buffer[1394]);

		adaptar(buffer + 1394,
		        ancho * alto,
		        pcxdac,
		        NULL);

		free(ptr);

		buffer = buffer + 1394 - 64;
		*((int *)buffer + 13) = ancho;
		*((int *)buffer + 14) = alto;
		*((int *)buffer + 15) = npuntos;

		while(g[0].grf[next_map_code]) {
			if(next_map_code++ == 1999) {
				next_map_code = 1000;
			}
		}
		g[0].grf[next_map_code] = (int *)buffer;

		pila[sp] = next_map_code;
	}
	// Error
	else {
		free(ptr); // Fix: memory leak

		e(144);
		return;
	}

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV new_map(INT width, INT height, INT center_x, INT center_x, INT color)
// - Devuelve el c�digo
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

	// Comprueba l�mites an/al/color ...
	if(ancho < 1 || alto < 1 || ancho > 32768 || alto > 32768) {
		e(153);
		return;
	}
	if(color < 0 || color > 255) {
		e(154);
		return;
	}
	if(cx < 0 || cy < 0 || cx >= ancho || cy >= alto) {
		e(155);
		return;
	}

	if((ptr = (byte *)malloc(1330 + 64 + 4 + ancho * alto)) != NULL) {
		ptr += 1330; // fix load_map/unload_map
		*((int  *)ptr + 13) = ancho;
		*((int  *)ptr + 14) = alto;
		*((int  *)ptr + 15) = 1; // Se define un punto de control (el centro)
		*((word *)ptr + 32) = cx;
		*((word *)ptr + 33) = cy;
		memset(ptr + 4 + 64,
		       color,
		       ancho * alto);

		while(g[0].grf[next_map_code]) {
			if(next_map_code++ == 1999) {
				next_map_code = 1000;
			}
		}
		g[0].grf[next_map_code] = (int *)ptr;

		pila[sp] = next_map_code;
	}
	else {
		e(100);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_fpg(STRING file_name)
// - Devuelve el c�digo del fichero 0..max_fpgs
//════════════════════════════════════════════════════════════════════════════
#define STDOUTLOG

void DIV_load_fpg(void) {
	int num = 0, n = 0, m = 0;
	int **lst = NULL;
	byte *ptr = NULL , *ptr2 = NULL, *ptr3 = NULL;
	int *iptr = NULL;
	byte xlat[256];
	int frompak = 0;

	memset(xlat,
	       0,
	       256);

	while(num<max_fpgs) {
		if(g[num].fpg == 0) {
			break;
		}

		num++;
	}
	if(num == max_fpgs) {
		pila[sp] = 0;

		e(104);
		return;
	}
//printf("num is %d\n", num);

	if(num) {
		if((lst = (int **)malloc(sizeof(int *) * 1000)) == NULL) {
			pila[sp] = 0;

			e(100);
			return;
		}
	}
	else {
		lst = g[0].grf;
	}
	memset(lst,
	       0,
	       sizeof(int *) * 1000);

	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto fpgfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <= 0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		ptr        = packptr;
		file_len   = m;
		g[num].fpg = (int**)ptr;
		frompak    = 1;
	}
	else {
fpgfuera:
#ifdef STDOUTLOG
printf("fpg wanted is [%s]\n", &mem[pila[sp]]);
#endif

		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = 0;

			e(105);
			return;
		}
		else {
			fseek(es, 0, SEEK_END);
#ifndef __EMSCRIPTEN__ 
			file_len = ftell(es);
#else
			file_len = 1352;
#endif
//printf("file_len is %d\n",file_len);

			if((ptr = (byte *)malloc(file_len + 8)) != NULL) {
				memset(ptr,
				       0,
				       file_len + 8);

				g[num].fpg = (int **)ptr;

				fseek(es, 0, SEEK_SET);
				n = fread(ptr, 1, file_len, es);

#ifdef STDOUTLOG
printf("ptr is %x\n",
       ptr);
printf("read %d bytes of %d\n",
       n,
       file_len);
printf("fpg pointer is %x\n",
       (int **)ptr);
#endif

#ifndef __EMSCRIPTEN__ 
				fclose(es);
#endif
			}
			else {
				fclose(es);

				pila[sp] = 0;

				e(100);
				return;
			}
		}
	}

	if(strcmp((char *)ptr, "fpg\x1a\x0d\x0a")) {
		free(lst); // Fix: memory leak
		free(ptr); // Fix: before to "e" call to avoid memory leak

		e(106);
		return;
	}

	if(process_fpg != NULL) {
		process_fpg((char *)ptr, file_len);
	}

//#ifdef STDOUTLOG
//printf("fpg found\n");
//#endif

	if(!paleta_cargada) {
		for(m = 0; m < 768; m++) {
			if(ptr[m + 8] != paleta[m]) {
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
			       ptr + 8,
			       768);

			nueva_paleta();
		}

		paleta_cargada = 1;
	}

	for(m = 0, n = 0; n < 768; n++) {
		m += (int)ptr[n + 8];
		m <<= 1;
	}
	if(m != palcrc) {
		xlat[0] = 0;
		for(n = 1; n < 256; n++) {
			find_color(ptr[n * 3 + 8], ptr[n * 3 + 9], ptr[n * 3 + 10]);

			xlat[n] = find_col;
		}
	}

	g[num].grf = lst;

#ifdef STDOUTLOG
printf("num: %d ptr: %x\n", num, ptr);
#endif

#ifdef __EMSCRIPTEN__
// do something different
	int len_ = 1;
	int num_ = 1;

	if(frompak) {
		es = fmemopen(ptr, file_len, "rb");
	}
	fseek(es, 0, SEEK_END);
	file_len = ftell(es);
	fseek(es, 1352, SEEK_SET);
	while(ftell(es) < file_len && len_ > 0 && num_ > 0) {
		int pos    = ftell(es);
		byte *mptr = &ptr[pos];

		fread(&num_, 4, 1, es);
		fread(&len_, 4, 1, es);
//printf("len: %d num: %d len: %d\n", len_, num_, ftell(es));
		fseek(es, -8, SEEK_CUR);

		mptr = (byte *)malloc(len_);
		if(mptr == NULL) { // Fix: insufficient memory check
			pila[sp] = 0;

			e(100);
			return;
		}

		fread(mptr, 1, len_, es);

		lst[num_] = iptr = (int *)mptr;
//printf("mem ptr is %x\n", iptr);
		if(m != palcrc) {
			adaptar(ptr + 64 + iptr[15] * 4,
			        iptr[13] * iptr[14],
			       (byte *)(g[num].fpg) + 8,
			       &xlat[0]);
		}
	}
	fclose(es);
#else
	ptr  += 1352; // Longitud cabecera fpg
	ptr2  = ptr;
	ptr3  = ptr;
	while(ptr <= (ptr2 + file_len) && *(int *)ptr3 < 1000 && *(int *)ptr3 > 0) {
		int *ptr_4 = (int *)ptr3;
		int *ptr_8 = (int *)ptr3;
		int num    = *ptr_4;
		int len    = *(ptr_8 + 1);

		lst[num] = iptr = ptr_4;

		if(m != palcrc) {
			adaptar(ptr + 64 + iptr[15] * 4,
			        iptr[13] * iptr[14],
			       (byte *)(g[num].fpg) + 8,
			       &xlat[0]);
		}

		ptr  = (byte *)&ptr2[len];//(int *)(ptr[4]);
		ptr3 = ptr;
		ptr2 = ptr;

		//if(num <= 0 || num > 1000) break;
	}
#endif
#ifdef STDOUTLOG
printf("fpg search ended, %x: ptr: %x\n",
       (byte *)g[num].fpg + file_len,
       ptr);
#endif

	pila[sp] = num;

	max_reloj += get_reloj() - old_reloj;
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

/*
printf("Start scroll: %d %d %d %d %d %d\n",
       snum,
       file,
       graf1,
       graf2,
       reg,
       mf);
*/

	if(snum < 0 || snum > 9) {
		e(107);
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
		e(108);
		return;
	}
	if(iscroll[snum].an == 0 || iscroll[snum].al == 0) {
		e(146);
		return;
	}
	if(file < 0 || file > max_fpgs) {
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 < 0 || graf1 >= max_grf) {
		e(110);
		return;
	}
	if(graf2 < 0 || graf2 >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}

	ptr1 = g[file].grf[graf1];
	ptr2 = g[file].grf[graf2];
	if(ptr1 == NULL && ptr2 == NULL) {
		e(112);
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

	if((iscroll[snum]._scr1 = (byte *)malloc(iscroll[snum].an * (iscroll[snum].al + 1))) == NULL) {
		e(100);
		return;
	}
	if((iscroll[snum].fast = (t_fast *)malloc(iscroll[snum].al * sizeof(t_fast))) == NULL) {
		e(100);
		return;
	}
	iscroll[snum].scr1   = iscroll[snum]._scr1;
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

		if((iscroll[snum]._scr2 = (byte *)malloc(iscroll[snum].an *(iscroll[snum].al + 1))) == NULL) {
			e(100);
			return;
		}

		iscroll[snum].scr2   = iscroll[snum]._scr2;
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
		e(107);
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
		e(107);
		return;
	}

	if(iscroll[snum].on) {
		free(iscroll[snum].fast); // Fix: memory leak (before on if(iscroll[snum].on == 2), this cause memory leak when we stop a scroll with only the first plane)
		free(iscroll[snum]._scr1);
	}
	if(iscroll[snum].on == 2) {
		free(iscroll[snum]._scr2);
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
	int n, an, al, nan, ifonts, m;
	int old_reloj = get_reloj();

//printf("load font %s\n", (byte *)&mem[itxt + pila[sp]]);
	for(ifonts = 1; ifonts < max_fonts; ifonts++) {
		if(!fonts[ifonts]) {
			break;
		}
	}
	if(ifonts == max_fonts) {
		pila[sp] = 0;

		e(113);
		return;
	}

	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto fntfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <=  0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		ptr      = packptr;
		file_len = m;

		fonts[ifonts] = ptr;
	}
	else {
fntfuera:
		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = 0;

			e(114);
			return;
		}
		else {
			fseek(es, 0, SEEK_END);
			file_len = ftell(es);
			if((ptr = (byte *)malloc(file_len)) != NULL) {
				fseek(es, 0, SEEK_SET);
				fread(ptr, 1, file_len,es);
				fclose(es);

				fonts[ifonts] = ptr;
			}
			else {
				fclose(es);

				pila[sp] = 0;

				e(118);
				return;
			}
		}
	}

	if(strcmp((char *)ptr, "fnt\x1a\x0d\x0a")) {
		fonts[ifonts] = 0;

		free(ptr); // Fix: before to "e" call to avoid memory leak

		e(115);
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
		if(fnt[n].alto) {
			if(fnt[n].alto + fnt[n].incY > al) {
				al = fnt[n].alto + fnt[n].incY;
			}
		}
	}

	ptr += 8;
	m    = 0;
	for(n = 0; n < 768; n++) {
		m += (int)ptr[n];
		m <<= 1;
	}

	if(strlen((char *)&mem[pila[sp]]) < 80) {
		strcpy(f_i[ifonts].name, (char*)&mem[pila[sp]]);
	}
	else {
		strcpy(f_i[ifonts].name, "");
	}
	f_i[ifonts].len       = file_len;
	f_i[ifonts].fonpal    = m;
	f_i[ifonts].syspal    = m;
	f_i[ifonts].ancho     =  an / nan;
	f_i[ifonts].espacio   = (an / nan) / 2;
	f_i[ifonts].espaciado = 0;
	f_i[ifonts].alto      = al;

	pila[sp] = ifonts;

	if(adaptar_paleta) {
		adaptar(fonts[ifonts] + 1356 + sizeof(t_fnt_table) * 256,
		        f_i[ifonts].len - 1356 - sizeof(t_fnt_table) * 256,
		        fonts[ifonts] + 8,
		        NULL);

		f_i[ifonts].syspal = palcrc;
	}

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// Adapta un font a la paleta del systema (tiene "palcrc" calculado)
//════════════════════════════════════════════════════════════════════════════
void checkpal_font(int ifonts) {
	if(ifonts <= 0 || ifonts >= max_fonts) {
		return;
	}
	if(!fonts[ifonts]) {
		return;
	}

	if(f_i[ifonts].syspal != palcrc) {
		// Debe "recargarlo"
		if(f_i[ifonts].syspal != f_i[ifonts].fonpal) {
			if(npackfiles) {
				file_len = read_packfile((byte *)&mem[pila[sp]]);

				if(file_len == -1) {
					goto fntfuera;
				}
				if(file_len == -2) {
					return;
				}
				if(file_len <=  0) {
					return;
				}
				if(file_len != f_i[ifonts].len) {
					return;
				}

				memcpy(fonts[ifonts],
				       packptr,
				       file_len);

				free(packptr);
			}
			else {
fntfuera:
				if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
					return;
				}
				else {
					fseek(es, 0, SEEK_END);
					file_len = ftell(es);
					if(file_len != f_i[ifonts].len) {
						return;
					}
					fseek(es, 0, SEEK_SET);
					fread(fonts[ifonts], 1, file_len, es);
					fclose(es);
				}
			}

			if(process_fnt != NULL) {
				process_fnt((char *)fonts[ifonts], file_len);
			}
		}

		if(f_i[ifonts].fonpal != palcrc) {
			adaptar(fonts[ifonts] + 1356 + sizeof(t_fnt_table) * 256,
			        f_i[ifonts].len - 1356 - sizeof(t_fnt_table) * 256,
			        fonts[ifonts] + 8,
			        NULL);
		}

		f_i[ifonts].syspal = palcrc;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Adapta (ptr, len) siendo pal[] su paleta
//════════════════════════════════════════════════════════════════════════════
void adaptar(byte *ptr, int len, byte *pal, byte *xlat) {
	int n, m;
	byte _xlat[256];
	byte *endptr;

	if(adaptar_paleta) {
		if(xlat == NULL) {
			xlat = &_xlat[0];

			for(m = 0, n = 0; n < 768; n++) {
				m += (int)pal[n];
				m <<= 1;
			}
			if(m == palcrc) {
				return;
			}

			xlat[0] = 0;
			for(n = 1; n < 256; n++) {
				find_color(pal[n * 3 + 0],
				           pal[n * 3 + 1],
				           pal[n * 3 + 2]);

				xlat[n] = find_col;
			}
		}

		endptr = ptr + len;
		do {
			*ptr = xlat[*ptr];
		}while(++ptr < endptr);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV write(INT fnt_id, INT x, INT y, INT center, INT ptr)
//════════════════════════════════════════════════════════════════════════════
void DIV_write(void) {
	int f;

	f = pila[sp - 4];

	if(f < 0 || f >= max_fonts) {
		e(116);

		f = 0;
	}
	if(fonts[f] == 0) {
		e(116);

		f = 0;
	}

	x = 1;
	while(texto[x].font != NULL) {
		x++;
		if(x == max_textos) {
			break;
		}

		if(pila[sp - 1] == texto[x].centro &&
		   pila[sp - 2] == texto[x].y      &&
		   pila[sp - 3] == texto[x].x) {
			break;
		}
	}
	if(x < max_textos) {
		texto[x].ptr    = pila[sp--];
		texto[x].centro = pila[sp--];
		texto[x].y      = pila[sp--];
		texto[x].x      = pila[sp--];

		if(texto[x].centro < 0 || texto[x].centro > 8) {
			pila[sp] = 0;

			e(117);
		}

		texto[x].tipo   = 0;
		texto[x].font   = (byte *)fonts[f];

		pila[sp] = x;
	}
	else {
		sp -= 4;
		pila[sp] = 0;

		e(118);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV write_int(INT fnt_id, INT x, INT y, INT center, INT *num)
//════════════════════════════════════════════════════════════════════════════
void DIV_write_int(void) {
	int f;

	f = pila[sp - 4];

	if(f < 0 || f >= max_fonts) {
		e(116);

		f = 0;
	}
	if(fonts[f] == 0) {
		e(116);

		f = 0;
	}

	x = 1;
	while(texto[x].font) {
		x++;

		if(x == max_textos) {
			break;
		}

		if(pila[sp - 1] == texto[x].centro &&
		   pila[sp - 2] == texto[x].y      &&
		   pila[sp - 3] == texto[x].x) {
			break;
		}
	}
	if(x < max_textos) {
		texto[x].ptr    = pila[sp--];
		texto[x].centro = pila[sp--];
		texto[x].y      = pila[sp--];
		texto[x].x      = pila[sp--];

		if(texto[x].centro < 0 || texto[x].centro > 8) {
			pila[sp] = 0;

			e(117);
		}

		texto[x].tipo   = 1;
		texto[x].font   = (byte *)fonts[f];

		pila[sp] = x;
	}
	else {
		sp -= 4;
		pila[sp] = 0;

		e(118);
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
	else if(x == 0) {
		x = 1;
		do {
			texto[x++].font = 0;
		}while(x < max_textos);
	}
	else {
		e(119);
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
		e(119);

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
			e(109);
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
		e(109);
	}
}


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

void init_divrandom(int n){
	int a;

	for(a = 0; a < 32; a++) {
		seed.d[a] = n;
	}
	for(a = 0; a < 2048; a++) {
		divrnd();
	}
}

int divrandom(int min, int max) {
	int r;

	if(max < min) {
		swap(max, min);
	}

	r = (((divrnd() & 127) * 256 + divrnd()) * 256 + divrnd()) * 256 + divrnd();

	if(min < -2147483640 && max > 2147483640) {
		return (r);
	}

	return ((r % (max - min + 1)) + min);
}

//════════════════════════════════════════════════════════════════════════════
// DIV rand(INT min, INT max)
//════════════════════════════════════════════════════════════════════════════
void DIV_rand(void) {
	int min, max;

	max = pila[sp--];
	min = pila[sp];

	pila[sp] = divrandom(min, max);
}

//════════════════════════════════════════════════════════════════════════════
// DIV rand_seed(INT seed)
//════════════════════════════════════════════════════════════════════════════
void DIV_rand_seed(void) {
	init_divrandom(pila[sp]);
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
		e(120);
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

		e(108);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 <= 0 || graf1 >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
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
		e(121);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 <= 0 || graf1 >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
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
		e(121);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(grafd <= 0 || grafd >= max_grf) {
		e(110);
		return;
	}
	if(graf  <= 0 || graf  >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
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
			e(121);
		}
	}
	else {
		e(121);
	}
}

//══════════════════════════════════════════════════════════════════════════════════════
// DIV screen_copy(INT region, INT file, INT graph, INT x, INT y, INT width, INT height)
// - (des-volcado escalado desde la una region de copia a un gr�fico)
//══════════════════════════════════════════════════════════════════════════════════════
void DIV_screen_copy(void) {
	int reg, file, graf;
	int an, al, divand, ald;
	int *ptr;
	int xr, ixr, yr, iyr;
	byte *old_si, *si, *di;

	ald    = pila[sp--];
	divand = pila[sp--];
	yr     = pila[sp--];
	xr     = pila[sp--];
	graf   = pila[sp--];
	file   = pila[sp--];
	reg    = pila[sp];

	if(reg >= 0 && reg < max_region) {
		an = region[reg].x1 - region[reg].x0;
		al = region[reg].y1 - region[reg].y0;
	}
	else {
		e(108);
		return;
	}

	if(file > max_fpgs || file < 0) {
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}

	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
		return;
	}

	if(xr < 0)              { xr = 0;               }
	if(yr < 0)              { yr = 0;               }
	if(xr + divand>ptr[13]) { divand= ptr[13] - xr; }
	if(yr + ald>ptr[14])    { ald   = ptr[14] - yr; }
	if(divand <= 0 || ald <= 0 || an <= 0 || al <= 0) {
		return;
	}

	di     = (byte *)ptr + 64 + ptr[15] * 4 + xr + yr * ptr[13];
	old_si = copia + region[reg].x0 + region[reg].y0 * vga_an;
	ixr    = (float)(an * 256) / (float)divand;
	iyr    = (float)(al * 256) / (float)ald;
	an     = divand;
	yr     = 0;
  do {
		si = old_si + (yr >> 8) * vga_an;
		xr = 0;
		do {
			*di = *(si + (xr >> 8));

			di++;
			xr += ixr;
		}while(--an);

		di += ptr[13] - (an = divand);
		yr += iyr;
	}while(--ald);
}

//════════════════════════════════════════════════════════════════════════════════════
// DIV load_screen(STRING file_name)
//════════════════════════════════════════════════════════════════════════════════════
void DIV_load_screen(void) {
	DIV_load_map(); // filename

	pila[sp + 1] = pila[sp];
	pila[sp++  ] = 0;

	DIV_put_screen(); // file, graf

	sp++;

	DIV_unload_map(); // graf

	pila[--sp] = 0;
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
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
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
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

	pila[++sp ] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV save(STRING file_name, INT *offset, INT size)
//════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG // Versi�n con debugger.
FILE *open_save_file(byte *file) {
	FILE *f;
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];

	f = open_multi(file, "wb");

	return f;
}

#else // Versi�n instalaciones.
FILE *open_save_file(byte *file) {
	FILE *f;
	char drive[_MAX_DRIVE  + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];

	char *ff = (char *)file;

	packfile_del((char *)file);

	while(*ff != 0) {
		if(*ff =='\\') {
			*ff = '/';
		}

		ff++;
	}

//printf("Looking for save file: %s\n",file);

	f = open_multi(file, "wb");

	return f;
}
#endif

void DIV_save(void) {
	int offset, lon;
	int llon;

	if(unit_size < 1) {
		unit_size = 1;
	}

	lon    = pila[sp--];
	offset = pila[sp--];

	lon = lon * unit_size;

	if(!capar(offset) || !capar(offset + lon)) {
		pila[sp] = 0;

		e(122);
		return;
	}

	es = open_save_file((byte *)&mem[pila[sp]]);
	if(es == NULL) {
		pila[sp] = 0;

		e(123);
		return;
	}

//fprintf(stdout, "File saved: %s\n", full);

	llon = (int)fwrite(&mem[offset], 1, lon, es);
	fclose(es);

	if(lon != llon) { // * unit_size) {
		e(124);
	}

	max_reloj += get_reloj() - old_reloj;
}

/*
void _save(void) {
  int offset,lon;
  int llon;

  if (unit_size<1) unit_size=1;
  lon=pila[sp--]; offset=pila[sp--];
  if (offset<long_header || offset+lon>imem_max) { pila[sp]=0; e(122); return; }
  es=open_save_file((byte*)&mem[pila[sp]]);
  if (es==NULL) {
    pila[sp]=0;
//    e(123); // OJO! Para que no de error en los CD-ROM
    return;
  }
//  if (fwrite(&mem[offset],unit_size,lon,es)!=lon) e(124);
  llon = fwrite(&mem[offset],unit_size,lon,es);
  fclose(es);

  if(llon !=lon)
    e(124);
  max_reloj+=get_reloj()-old_reloj;
}
*/

//════════════════════════════════════════════════════════════════════════════
// DIV load(STRING file_name, INT *offset)
//════════════════════════════════════════════════════════════════════════════
void DIV_load(void) {
	int offset = 0, lon = 0;
	int fbytes = 0;

	if(unit_size < 1) {
		unit_size = 1;
	}

	offset = pila[sp--];

	if(!capar(offset)) {
		pila[sp] = 0;

		e(125);
		return;
	}

//fprintf(stdout, "loading data from: %s\n", (byte*)&mem[pila[sp]]);

	if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
		// if not found, check pak
		// this way files override paks
		lon = read_packfile((byte *)&mem[pila[sp]]);
		if(lon > 0) {
			if(!capar(offset + lon)) {
				pila[sp] = 0;

				e(125);
				return;
			}

			memcpy(&mem[offset],
			        packptr,
			        lon);

			max_reloj += get_reloj() - old_reloj;
			return;
		}

		pila[sp] = 0;

#ifdef DEBUG
		e(126);
#endif
		return;
	}

//fprintf(stdout, "File loaded: %s\n", full);

	fseek(es, 0, SEEK_END);
	lon = ftell(es); /// 4; 
//printf("File len: %d\n", ftell(es));
	fseek(es, 0, SEEK_SET);

	if(!capar(offset + lon)) {
		pila[sp] = 0;

		e(125);
		return;
	}

	lon    = lon / unit_size;
	fbytes = fread(&mem[offset], unit_size, lon, es);
	if(fbytes != lon) {
/*
fprintf(stdout,
        "Bytes read: %d bytes wanted: %d len: %d unit_size: %d\n",
        fbytes,
        lon * unit_size,
        lon,
        unit_size);
/*/

		e(127);
	}
	fclose(es);

	max_reloj += get_reloj() - old_reloj;
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

//printf("Tring to set mode %dx%d\n", vga_an, vga_al);

	// nonsense ?
	for(n = 0; n <num_video_modes; n++) {
		if(pila[sp] == video_modes[n].modo) {
			vga_an = video_modes[n].ancho;
			vga_al = video_modes[n].alto;
			break;
		}
	}

	vvga_an = vga_an;
	vvga_al = vga_al;

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
		exer(1);
	}
	memset(copia2,
	       0,
	       vga_an * vga_al);

#ifdef DEBUG
	if((copia_debug = (char *)malloc(vga_an * vga_al)) == NULL) {
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

#ifdef SDL2
	OSDEP_SetWindowSize(vga_an, vga_al);
#endif

	//mouse_window(); mouse ?

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
			im7[n].on = 0; // Fix: before iscroll[n].on = 0;
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

	if(vwidth < vga_an || vheight < vga_al) {
		vwidth  = vga_an;
		vheight = vga_al;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_pcm(STRING file_name, INT loop)
//════════════════════════════════════════════════════════════════════════════
void DIV_load_pcm(void) {
	char *ptr;
	int loop, m;

	loop = pila[sp--];

	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto pcmfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <=  0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		ptr      = (char *)packptr;
		file_len = m;
	}
	else {
pcmfuera:
		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = -1;

			e(128);
			return;
		}
		else {
			fseek(es, 0, SEEK_END);
			file_len = ftell(es);
			if((ptr = (char *)malloc(file_len)) != NULL) {
				fseek(es, 0, SEEK_SET);
				fread(ptr, 1, file_len, es);
				fclose(es);
			}
			else {
				fclose(es);

				pila[sp] = 0;

				e(100);
				return;
			}
		}
	}

	pila[sp] = DIVLoadSound(ptr, file_len, loop);

	free(ptr);

	max_reloj += get_reloj() - old_reloj;
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

	if(vol < 0)        { vol = 0;   }
	else if(vol > 511) { vol = 511; }
	if(fre < 8)        { fre = 8;   }

	if(fre) {
#ifdef MIXER
		pila[sp] = DIVPlaySound(pila[sp], vol, fre) + 1;
		//if(pila[sp] == -1) {
		//	e(129);
		//}
//printf("New sound on channel %d\n", pila[sp]);
#else
		pila[sp] = 0;
#endif
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_sound(INT channel_id)
//════════════════════════════════════════════════════════════════════════════
extern int MusicChannels;

void DIV_stop_sound(void) {
#ifdef MIXER
	int x;
/*
  int InitChannel=16;

  if(MusicChannels>InitChannel) InitChannel=MusicChannels;
  if(InitChannel>=32) {
    pila[sp]=0;
    return;
  }
  */
//  if(pila[sp]==-1) {
//    for(x=0; x<CHANNELS; x++) topSound(x);
//  } else {

		DIVStopSound(pila[sp] - 1);
//}
#endif

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV change_sound(INT channel_id, INT volume, INT speed)
//════════════════════════════════════════════════════════════════════════════
void DIV_change_sound(void) {
	int vol, fre;

	fre = pila[sp--];
	vol = pila[sp--];

	if(vol < 0)        { vol = 0;   }
	else if(vol > 511) { vol = 511; }
	if(fre < 8)        { fre = 8;   }

	DIVChangeSound(pila[sp] - 1, vol, fre);
}

//════════════════════════════════════════════════════════════════════════════
// DIV change_channel(INT channel_id, INT volume, INT panning)
//════════════════════════════════════════════════════════════════════════════
void DIV_change_channel(void) {
	int vol, pan;

	pan = pila[sp--];
	vol = pila[sp--];

	if(vol      < 0)    { vol = 0;   }
	else if(vol > 511)  { vol = 511; }
	if(pan < 0)         { pan = 0;   }
	else if (pan > 255) { pan = 255; }

	DIVChangeChannel(pila[sp] - 1, vol, pan);
}

//════════════════════════════════════════════════════════════════════════════
// DIV load_song(STRING file_name, INT loop)
//════════════════════════════════════════════════════════════════════════════
void DIV_load_song(void) {
	int loop, m;
	char *ptr;

	loop = pila[sp--];

//printf("Requesting song: %s\n", (char *)&mem[pila[sp]]);
	if(npackfiles) {
		m = read_packfile((byte *)&mem[pila[sp]]);

		if(m == -1) {
			goto songfuera;
		}
		if(m == -2) {
			pila[sp] = 0;

			e(100);
			return;
		}
		if(m <=  0) {
			pila[sp] = 0;

			e(200);
			return;
		}

		ptr      = (char *)packptr;
		file_len = m;
	}
	else {
songfuera:
		if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
			pila[sp] = -1;

			e(167);
			return;
		}
		else {
			fseek(es, 0, SEEK_END);
			file_len = ftell(es);
			if((ptr = (char *)malloc(file_len)) != NULL) {
				fseek(es, 0, SEEK_SET);
				fread(ptr, 1, file_len, es);
				fclose(es);
			}
			else {
				fclose(es);

				pila[sp] = 0;

				e(100);
				return;
			}
		}
	}

//printf("Loading Song\n");
	pila[sp] = DIVLoadSong(ptr, file_len, loop);

	free(ptr);

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV unload_song(INT song_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_unload_song(void) {
	DIVUnloadSong(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV song(song_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_song(void) {
	DIVPlaySong(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_song()
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_song(void) {
	DIVStopSong();

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_song_pos(INT pattern)
//════════════════════════════════════════════════════════════════════════════
void DIV_set_song_pos(void) {
	DIVSetSongPos(pila[sp]);
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_song_pos()
//════════════════════════════════════════════════════════════════════════════
void DIV_get_song_pos(void) {
	pila[++sp] = DIVGetSongPos();
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_song_line()
//════════════════════════════════════════════════════════════════════════════
void DIV_get_song_line(void) {
	pila[++sp] = DIVGetSongLine();
}

//════════════════════════════════════════════════════════════════════════════
// DIV is_playing_sound(INT channel_id)
//════════════════════════════════════════════════════════════════════════════
void DIV_is_playing_sound(void) {
	pila[sp] = DIVIsPlayingSound(pila[sp] - 1);
}

//════════════════════════════════════════════════════════════════════════════
// DIV is_playing_song()
//════════════════════════════════════════════════════════════════════════════
void DIV_is_playing_song(void) {
	pila[++sp] = DIVIsPlayingSong();
}

//════════════════════════════════════════════════════════════════════════════
// DIV set_fps(INT fps, INT fps_max, INT skips)
//════════════════════════════════════════════════════════════════════════════
//void mainloop(void);

void DIV_set_fps(void) {
	max_saltos = pila[sp--];

	if(max_saltos <  0) { max_saltos = 0;  }
	if(max_saltos > 10) { max_saltos = 10; }

	if(pila[sp] <   4) { pila[sp] = 4;   }
	if(pila[sp] > 999) { pila[sp] = 999; }

#ifdef __EMSCRIPTEN__
//if(max_saltos < 2) {
//	max_saltos = 2;
//}
//	emscripten_cancel_main_loop();
//	emscripten_set_main_loop(mainloop, dfps, 0);
#endif
	ireloj = 1000.0 / (double)pila[sp];
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
	if((es = div_open_file((byte *)&mem[pila[sp]])) == NULL) {
		pila[sp] = 0;

		e(147);
	}
	else {
		fclose(es);

		pila[sp] = DIVStartFLI(full, (char *)copia2, vga_an, vga_al, x, y);
		if(pila[sp] == 0) {
			e(130);
		}
	}
#endif

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV frame_fli()
// - Devuelve 0-fin fli, 1-continúa
//════════════════════════════════════════════════════════════════════════════
void DIV_frame_fli(void) {
#ifdef USE_FLI // Fix: avoid crash if FRAME_FLI() function is called without having FLIC support built
	pila[++sp] = DIVNextframeFLI();
#else
	pila[++sp] = 0;
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV end_fli()
//════════════════════════════════════════════════════════════════════════════
void DIV_end_fli(void) {
#ifdef USE_FLI
	DIVEndFLI();
#endif

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV reset_fli()
//════════════════════════════════════════════════════════════════════════════
void DIV_reset_fli(void) {
#ifdef USE_FLI
	DIVResetFLI();
#endif

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV system(STRING command)
//════════════════════════════════════════════════════════════════════════════
void DIV_system(void) { // TODO
	char cwork[256];
	unsigned n;
#ifdef STDOUTLOG
printf("system call not implemented yet\n");
#endif

	return;

	if(system(NULL)) {
		if(!strcmp(strupr((char *)&mem[pila[sp]]), "COMMAND.COM")) {
			getcwd(cwork,256);

			DIVEndSound();

#ifdef DOS
			_setvideomode(_TEXTC80);

			flushall();
			_heapmin();
			_heapshrink();

			system("command.com");
#endif

			DIVInitSound();
			set_mixer();

			_dos_setdrive((int)toupper(*cwork) - 'A' + 1, &n);
			chdir(cwork);

			svmode();

			set_dac();

			set_mouse(mouse->x, mouse->y);
			readmouse();

			volcado_completo = 1;
		}
		else {
#ifdef DOS
			flushall();
			_heapmin();
			_heapshrink();
#endif
			system((char *)&mem[pila[sp]]);
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
	int p, m;

	m = pila[sp--];
	p = pila[sp];

	if(p < 1) { // Fix: check before to non stop the currently CD playing if param value is wrong
		return;
	}

	DIVStopCD();
	DIVPlayCD(p, m);
}

//════════════════════════════════════════════════════════════════════════════
// DIV stop_cd();
//════════════════════════════════════════════════════════════════════════════
void DIV_stop_cd(void) {
	DIVStopCD();

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV is_playing_cd();
//════════════════════════════════════════════════════════════════════════════
unsigned int get_cd_error(void);

void DIV_is_playing_cd(void) { // TODO
//CACA
#ifdef DOS
	if(get_cd_error() & 0x200) {
		pila[++sp] = 1;
	}
	else {
		pila[++sp] = 0;
	}
#endif

	pila[++sp] = 0;
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
		e(131);
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
		e(108);
		return;
	}

	if(im7[n].an == 0 || im7[n].al == 0) {
		e(146);
		return;
	}

	if(file < 0 || file > max_fpgs) {
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf1 < 0 || graf1 >= max_grf) {
		e(110);
		return;
	}
	if(graf2 < 0 || graf2 >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}

	ptr1 = g[file].grf[graf1];
	ptr2 = g[file].grf[graf2];
	if(ptr1 == NULL && ptr2 == NULL) {
		e(132);
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
		e(131);
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
	if(mem[id + _Ctype] == 3) {
		m8_object_avance(id, mem[id + _Angle], pila[sp]);
	}
	else {
		mem[id + _X] += get_distx(mem[id + _Angle], pila[sp]);
		mem[id + _Y] += get_disty(mem[id + _Angle], pila[sp]);
	}

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV xadvance(INT angle, INT dist)
//════════════════════════════════════════════════════════════════════════════
void DIV_xadvance(void) {
	int distancia = pila[sp--];

	if(mem[id + _Ctype] == 3) {
		m8_object_avance(id, pila[sp], distancia);
	}
	else {
		mem[id + _X] += get_distx(pila[sp], distancia);
		mem[id + _Y] += get_disty(pila[sp], distancia);
	}

	pila[sp] = 0;
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

	pila[++sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fade_off()
//════════════════════════════════════════════════════════════════════════════
void DIV_fade_off(void) {
	//int old_reloj = get_reloj();

	dacout_r     = 64;
	dacout_g     = 64;
	dacout_b     = 64;
	dacout_speed = 8;
	/*while(now_dacout_r != dacout_r ||
	      now_dacout_g != dacout_g ||
	      now_dacout_b != dacout_b) {
		set_paleta();
		set_dac();

		//DIVLoopSound();
	}
	fading = 0;*/

	if(now_dacout_r != dacout_r ||
	   now_dacout_g != dacout_g ||
	   now_dacout_b != dacout_b) {
		fading = 1;
	}

	pila[++sp] = 0;

	//max_reloj += get_reloj() - old_reloj;
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

#ifdef NET
	if(inicializacion_red) {
		net_end();
	}
#endif

	rvmode();
	kbdReset();

//	DIVEndSound();

#ifdef DEBUG
	if((f = fopen("system/exec.err", "wb")) != NULL) {
		fwrite("\x0\x0\x0\x0", 4, 1, f);
		fwrite(&pila[sp], 4, 1, f);
		fwrite(&mem[pila[sp - 1]], 1, strlen((char *)(&mem[pila[sp - 1]])) + 1, f);
		fclose(f);

		DIV_exit_called = 1; // Fix: bug #18: https://github.com/DIVGAMES/DIV-Games-Studio/issues/18
	}
#else
printf("%s\n",
       (char *)&mem[pila[sp - 1]]);
#endif

	_dos_setdrive((int)toupper(*divpath) - 'A' + 1, &divnum);
	chdir(divpath);

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

	if(!activar_paleta) {
		activar_paleta = 1;
	}
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
		e(109);
		return;
	}
	if(mem[id + _File]) { max_grf = 1000; } else { max_grf = 2000; }
	if(mem[id + _Graph] <= 0 || mem[id + _Graph] >= max_grf) {
		e(110);
		return;
	}
	if(g[mem[id + _File]].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[mem[id + _File]].grf[mem[id + _Graph]]) == NULL) {
		e(121);
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
		e(133);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_joy_button(INT button)
// - Button = 0..3
//════════════════════════════════════════════════════════════════════════════
#define GAME_PORT  0x201
#define TIMER_PORT 0x40
#define TIME_OUT   2000

void DIV_get_joy_button(void) {
	if(pila[sp] < 0 || pila[sp] > 3) {
		pila[sp] = 0;

		e(134);
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
	// SDL joypad
	if(divjoy && joy_status) {
		pila[sp] = OSDEP_JoystickGetButton(divjoy, pila[sp]);
	}
	else {
		pila[sp] = 0;
	}
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

#else

	return OSDEP_JoystickGetAxis(divjoy, pila[sp]) / 100;
#endif
}

void DIV_get_joy_position(void) {
	if(pila[sp] < 0 || pila[sp] > 3) {
		pila[sp] = 0;

		e(134);
		return;
	}


	pila[sp] = joy_position(pila[sp]);
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
#else
// do SDL joystick stuff
//	if(joy_status) {
//		joy->button1 = 1;
//	}
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
		e(136);
		return;
	}
	if(file < 0 || file > max_fpgs) {
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graf <= 0 || graf >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graf]) == NULL) {
		e(121);
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
void DIV_reset_sound(void) {
	DIVResetSound();

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

//════════════════════════════════════════════════════════════════════════════
// DIV find_color(INT r, INT g, INT b)
// - Devuelve un color de la paleta (1..255)
//════════════════════════════════════════════════════════════════════════════
void DIV_find_color(void) {
	int r, g, b;

	b = abs(pila[sp--]) % 64;
	g = abs(pila[sp--]) % 64;
	r = abs(pila[sp]  ) % 64;

	find_color(r, g, b);

	pila[sp] = find_col;
}

//════════════════════════════════════════════════════════════════════════════
// Funciones de cadenas
//════════════════════════════════════════════════════════════════════════════

//════════════════════════════════════════════════════════════════════════════
// DIV char(STRING literal)
//════════════════════════════════════════════════════════════════════════════
void DIV_char(void) { // char("0") -> 48
	if((unsigned)pila[sp] > 255) {
		pila[sp] = (int)memb[pila[sp] * 4];
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV strcpy(STRING dst, STRING src)
//════════════════════════════════════════════════════════════════════════════
void DIV_strcpy(void) {
	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		e(164);
		return;
	}
	if((unsigned)pila[sp] > 255 && (mem[pila[sp - 1] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[sp]])) {
		sp--;

		e(140);
		return;
	}

	if((unsigned)pila[sp] > 255) {
		strcpy((char*)&mem[pila[sp - 1]],
		       (char*)&mem[pila[sp]]);
	}
	else {
		mem[pila[sp - 1]] = pila[sp];
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV strcat(STRING dst, STRING src)
//════════════════════════════════════════════════════════════════════════════
void DIV_strcat(void) {
	int n;

	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		e(164);
		return;
	}

	if((unsigned)pila[sp] > 255) {
		n = strlen((char *)&mem[pila[sp]]);
	}
	else {
		n = 1;
	}
	if((mem[pila[sp - 1] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[sp - 1]]) + n) {
		sp--;

		e(140);
		return;
	}

	if((unsigned)pila[sp] > 255) {
		strcat((char *)&mem[pila[sp - 1]],
		       (char *)&mem[pila[sp]]);
	}
	else {
		sprintf((char *)&mem[pila[sp - 1]],
		        "%s%c%c",
		        (char*)&mem[pila[sp - 1]],
		        pila[sp],
		        '\0');
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV strlen(STRING str)
//════════════════════════════════════════════════════════════════════════════
void DIV_strlen(void) {
	if((unsigned)pila[sp] > 255) {
		pila[sp] = strlen((char *)&mem[pila[sp]]);
	}
	else {
		pila[sp] = 1;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV strcmp(STRING str1, STRING str1)
//════════════════════════════════════════════════════════════════════════════
void DIV_strcmp(void) {
	if((unsigned)pila[sp - 1] > 255) {
		if((unsigned)pila[sp] > 255) {
			pila[sp - 1] = strcmp((char *)&mem[pila[sp - 1]],
			                      (char *)&mem[pila[sp]]);
		}
		else {
			pila[sp - 1] = strcmp((char *)&mem[pila[sp - 1]],
			                      (char *)&pila[sp]);
		}
	}
	else {
		if((unsigned)pila[sp] > 255) {
			pila[sp - 1] = strcmp((char *)&pila[sp - 1],
			                      (char *)&mem[pila[sp]]);
		}
		else {
			pila[sp - 1] = strcmp((char *)&pila[sp - 1],
			                      (char *)&pila[sp]);
		}
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV strchr(STRING str, STRING chars)
// - ej: strchr(cadena,"aeiou") -> -1 No se encontro, N Posici�n
//════════════════════════════════════════════════════════════════════════════
void DIV_strchr(void) {
	char *p;

	if((unsigned)pila[sp] > 255) {
		p = strpbrk((char *)&mem[pila[sp - 1]],
		            (char *)&mem[pila[sp]]);
	}
	else {
		p = strchr((char *)&mem[pila[sp - 1]],
		           (char)pila[sp]);
	}
	if(p != NULL) {
		pila[sp - 1] = (int)(p - (char *)&mem[pila[sp - 1]]);
	}
	else {
		pila[sp - 1] =-1;
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV strstr(STRING str, STRING substr)
//════════════════════════════════════════════════════════════════════════════
void DIV_strstr(void) {
	char *p;

	if((unsigned)pila[sp] > 255) {
		p = strstr((char *)&mem[pila[sp - 1]],
		           (char *)&mem[pila[sp]]);
	}
	else {
		p = strchr((char *)&mem[pila[sp - 1]],
		           (char)pila[sp]);
	}
	if(p != NULL) {
		pila[sp - 1] = (int)(p - (char *)&mem[pila[sp - 1]]);
	}
	else {
		pila[sp - 1] = -1;
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV strset(STRING str, STRING char)
//════════════════════════════════════════════════════════════════════════════
void DIV_strset(void) {
	int n;

	if((mem[pila[sp - 1] - 1] & 0xFFF00000) != 0xDAD00000) {
		sp--;

		e(164);
		return;
	}

	n = (mem[pila[sp - 1] - 1] & 0xFFFFF) + 1;
	if((unsigned)pila[sp] > 255) {
		memset((char *)&mem[pila[sp - 1]],
		       (char)mem[pila[sp]], n);
	}
	else {
		memset((char *)&mem[pila[sp - 1]],
		       (char)pila[sp], n);
	}

	sp--;
}

//════════════════════════════════════════════════════════════════════════════
// DIV upper(STRING str)
//════════════════════════════════════════════════════════════════════════════
byte strupper[270] =
	"                                                                "
	" ABCDEFGHIJKLMNOPQRSTUVWXYZ      ABCDEFGHIJKLMNOPQRSTUVWXYZ     "
	"   A AA EEEIII A   O OUUY       AIOU                            "
	"                                                                ";

void DIV_upper(void) {
	int n;

	if((unsigned)pila[sp] > 255) {
		n = strlen((char *)&mem[pila[sp]]);
		while(n--) {
			if(strupper[memb[pila[sp] * 4 + n]] != ' ') {
				memb[pila[sp] * 4 + n] = strupper[memb[pila[sp] * 4 + n]];
			}
		}
	}
	else {
		if(strupper[(char)pila[sp]] != ' ') {
			pila[sp] = (int)strupper[(char)pila[sp]];
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV lower(STRING str)
//════════════════════════════════════════════════════════════════════════════
byte strlower[260] =
	"                                                                "
	" abcdefghijklmnopqrstuvwxyz      abcdefghijklmnopqrstuvwxyz     "
	"                                                                "
	"                                                                ";

void DIV_lower(void) {
	int n;

	if((unsigned)pila[sp] > 255) {
		n = strlen((char *)&mem[pila[sp]]);
		while(n--) {
			if(strlower[memb[pila[sp] * 4 + n]] != ' ') {
				memb[pila[sp] * 4 + n] = strlower[memb[pila[sp] * 4 + n]];
			}
		}
	}
	else {
		if(strlower[(char)pila[sp]] != ' ') {
			pila[sp] = (int)strlower[(char)pila[sp]];
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV strdel(STRING str, INT start, INT end)
// - (cadena, n, m) borra <n> char del inicio y <m> del final
//════════════════════════════════════════════════════════════════════════════
void strdelbeg(char *s, int n) {
	int len = strlen(s);

	if(n>0) {
		if(n >= len) {
			*s = 0;
		}
		else {
			memmove(s,
			        s + n,
			        len + 1 - n);
		}
	}
	else if(n < 0) {
		memmove(s - n,
		        s,
		        len + 1);
		memset(s,
		       ' ',
		      -n);
	}
}

void strdelend(char *s, int n) {
	int len = strlen(s);

	if(n > 0) {
		if(n >= len) {
			*s = 0;
		}
		else {
			s[len - n] = 0;
		}
	}
	else if(n < 0) {
		n = len - n;
		for(; len < n; len++) {
			s[len] = ' ';
		}
		s[len] = 0;
	}
}

void DIV_strdel(void) {
	int m = pila[sp--];
	int n = pila[sp--];

	if((mem[pila[sp] - 1] & 0xFFF00000) != 0xDAD00000) {
		e(164);
		return;
	}

	if((mem[pila[sp] - 1] & 0xFFFFF) + 1 < strlen((char *)&mem[pila[sp]]) - n - m) {
		e(140);
		return;
	}

	// Borra primero del inicio
	if(n > m) {
		strdelbeg((char *)&mem[pila[sp]], n);
		strdelend((char *)&mem[pila[sp]], m);
	}
	// Borra primero del final
	else {
		strdelend((char *)&mem[pila[sp]], m);
		strdelbeg((char *)&mem[pila[sp]], n);
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV qsort(INT struct_name, INT field_name, INT mode)
// - Funci�n de ordenaci�n / desordenaci�n de estructuras (nombre, campo, modo)
//════════════════════════════════════════════════════════════════════════════
byte xlat_rnd[256];
int offset_clave;

int sort0(const void *a, const void *b) {
	return ((*((int *)a + offset_clave)) - (*((int *)b + offset_clave)));
}

int sort1(const void*a, const void *b) {
	return ((*((int *)b + offset_clave)) - (*((int *)a + offset_clave)));
}

int sort2(const void *a, const void *b) {
	return (strcmp((char *)a + offset_clave * 4, (char *)b + offset_clave * 4));
}

int sort3(const void *a, const void *b) {
	return (strcmp((char *)a + offset_clave * 4, (char *)b + offset_clave * 4));
}

int sort4(const void *a, const void *b) {
	return (strcmp((char *)&mem[*((char *)a + offset_clave)], (char *)&mem[*((char *)b + offset_clave)]));
}

int sort5(const void *a, const void *b) {
	return (-strcmp((char *)&mem[*((char *)a + offset_clave)], (char *)&mem[*((char *)b + offset_clave)]));
}

int unsort00(byte *a, byte *b) {
	return ((int)(xlat_rnd[*(a + offset_clave * 4)] ^ (xlat_rnd[*(a + 1 + offset_clave * 4)] / 2) ^ (xlat_rnd[*(a + 2 + offset_clave * 4)] / 4) ^ (xlat_rnd[*(a + 3 + offset_clave * 4)] / 8))
         -(int)(xlat_rnd[*(b + offset_clave * 4)] ^ (xlat_rnd[*(b + 1 + offset_clave * 4)] / 2) ^ (xlat_rnd[*(b + 2 + offset_clave * 4)] / 4) ^ (xlat_rnd[*(b + 3 + offset_clave * 4)] / 8)));
}

int unsort0(const void *a, const void *b) {
	return unsort00((byte *)a, (byte *)b);
}

int strcmpsort(const void *a, const void *b) {
	return strcmp((char *)a, (char *)b);
}

void DIV_qsort(void) {
	int tipo_clave;
	int offset, size, numreg,modo;

	modo         = pila[sp--];
	tipo_clave   = pila[sp--];
	offset_clave = pila[sp--];
	numreg       = pila[sp--];
	size         = pila[sp--];
	offset       = pila[sp];

	if(modo < 0 || modo > 1) {
		for(modo = 0; modo < 256; modo++) {
			xlat_rnd[modo] = divrnd();
		}

		qsort(&mem[offset], numreg,size * 4, unsort0);
	}
	else {
		switch(tipo_clave) {
			case 0: {
				if(modo) { qsort(&mem[offset], numreg, size * 4, sort1); }
				else     { qsort(&mem[offset], numreg, size * 4, sort0); }
				break;
			}
			case 1: {
				if(modo) { qsort(&mem[offset], numreg, size * 4, sort3); }
				else     { qsort(&mem[offset], numreg, size * 4, sort2); }
				break;
			}
			case 2: {
				if(modo) { qsort(&mem[offset], numreg, size * 4, sort5); }
				else     { qsort(&mem[offset], numreg, size * 4, sort4); }
				break;
			}
		}
	}

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// Funciones de ficheros (adem�s de load/save)
//════════════════════════════════════════════════════════════════════════════

/*
function 122 int fopen(0,0)          // Casi como en C (filename,"rwa+") (siempre modo binario)
function 123 int fclose(0)           // Casi como en C (handle) pero si handle es 0 hace un fcloseall
function 124 int fread(0,0,0)        // Casi como en C (&buffer,longitud(en ints),handle) pero con s�lo una longitud de ints
function 125 int fwrite(0,0,0)       // Inversa a fread (&buffer,longitud,handle)
function 126 int fseek(0,0,0)        // Id�ntica a C (handle,posicion,modo) (modo seek_set/cur/end)
function 127 int ftell(0)            // Id�ntica a C (handle)
function 128 int filelength(0)       // Longitud de un fichero abierto (handle)
function 129 int flush()             // Vacia buffers de escritura y devuelve n� de ficheros abiertos

function 130 int    get_dirinfo(0,0) // Lee un directorio ("dir\*.pr?",_hidden+_system+_subdir) en la estructura dirinfo(files,name[]) y devuelve "files" (n�mero)
function 131 string get_fileinfo(0)  // Rellena la estructura fileinfo (fullpath (o cwd), drive, dir, name, ext, size, date, time, attrib) y devuelve puntero a "fullpath" o "cwd"

function 132 int getdrive()          // Devuelve la unidad actual (1-A, 2-B, ...)
function 133 int setdrive(0)         // Fija la unidad actual (idem)
function 134 int chdir(0)            // Cambia el directorio actual ("..")
function 135 int mkdir(0)            // Crea un directorio nuevo (a partir del cwd)
function 136 int remove(0)           // Borra archivos o directorios (se aceptan comodines) (auto rmdir)
function 137 int disk_free(0)        // Espacio disponible en una unidad (en Kbs)
function 138 int memory_free()       // Memoria disponible (en Kbs)
function 139 int ignore_error(0)     // Ignorar un error de ejecuci�n (n�mero)
*/

//════════════════════════════════════════════════════════════════════════════
// DIV fopen(STRING fname, STRING mode)
// - fopen((filename, "rwa+") (siempre modo binario)
//════════════════════════════════════════════════════════════════════════════
void DIV_fopen(void) {
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	char modo[128];
	int n, x;
	FILE *f = NULL;

	strcpy(modo, (char *)&mem[pila[sp--]]);
	strcpy(full, (char *)&mem[pila[sp]]);

	for(n = 0; n < strlen(modo); n++) {
		if(modo[n] != 'r' && modo[n] != 'w' && modo[n] != 'a' && modo[n] != '+') {
			break;
		}
	}
	if(n < strlen(modo)) {
		pila[sp] = 0;

		e(166);
	}

	strcat(modo, "b");

// Busca el archivo, ya que puede haber sido incluido en la instalaci�n

	packfile_del(full);

#ifdef DEBUG
	// check for file in prg dir
	f = __fpopen(full, modo);
#endif

	if(f == NULL) {
		// "paz\fixero.est"
		if((f = fopen(full, modo)) == NULL) {
			if(_fullpath(full, (char *)&mem[pila[sp]], _MAX_PATH) == NULL) {

				pila[sp] = 0;
				return;
			}

			_splitpath(full, drive, dir, fname, ext);

			if(strchr(ext, '.') == NULL) {
				strcpy(full, ext);
			}
			else {
				strcpy(full, strchr(ext, '.') + 1);
			}
			if(strlen(full) && memb[pila[sp] * 4] != '/') {
				strcat(full, "/");
			}
			strcat(full, (char *)&mem[pila[sp]]);

			// "est\paz\fixero.est"
			if((f = fopen(full, modo)) == NULL) {
				strcpy(full, fname);
				strcat(full, ext);

				// "fixero.est"
				if((f = fopen(full, modo)) == NULL) {
					if(strchr(ext, '.') == NULL) {
						strcpy(full, ext);
					}
					else {
						strcpy(full, strchr(ext, '.') + 1);
					}
				}
				if(strlen(full)) {
					strcat(full, "/");
				}
				strcat(full, fname);
				strcat(full, ext);

				// "est\fixero.est"
				f = fopen(full, modo);
			}
		}
	}

	if(f) {
		for(x = 0; x < 32; x++) {
			if(tabfiles[x] == 0) {
				break;
			}
		}
		if(x == 32) {
			fclose(f);

			pila[sp] = 0;

			e(169);
		}
		else {
			tabfiles[x] = f;

			pila[sp] = x * 2 + 1;
		}
	}
	else {
		pila[sp] = 0;

		if(errno == EMFILE) {
			e(169);
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV fclose(handle)
// - handle == 0 -> fcloseall
//════════════════════════════════════════════════════════════════════════════
void DIV_fclose(void) {
	int n;

	if(pila[sp] == 0) {
		pila[sp] = 0; // fcloseall(); // TODO

		if(pila[sp] == EOF) {
			pila[sp] = 0;
		}

		memset(tabfiles,
		       0,
		       32 * 4);
	}
	else {
		if(!(pila[sp] & 1) || pila[sp] < 1 || pila[sp] > 63) {
#ifdef DEBUG
			e(170);
#else
			pila[sp] = 0;
#endif
			return;
		}
	}

	n = pila[sp] / 2;
	if(tabfiles[n] == 0) {
		e(170);
		return;
	}

	pila[sp] = fclose((FILE *)(tabfiles[n]));

	tabfiles[n] = 0;

	if(!pila[sp]) {
		pila[sp] = 1;
	}
	else {
		pila[sp] = 0;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV fread(VOID *data, INT length, INT handle)
// - fread(&buffer, longitud(en unit_size), handle)
//════════════════════════════════════════════════════════════════════════════
void DIV_fread(void) {
	int offset, lon, handle, n;
	FILE *f;

	if(unit_size < 1) {
		unit_size = 1;
	}

	handle = pila[sp--];
	lon    = pila[sp--];
	offset = pila[sp];

	if(!(handle & 1) || handle < 1 || handle > 63) {
		e(170);
		return;
	}
	if(tabfiles[handle / 2] == 0) {
		e(170);
		return;
	}
	if(!capar(offset) || !capar(offset + (lon * unit_size) / 4)) {
		pila[sp] = 0;

		e(125);
		return;
	}

	f = (FILE *)tabfiles[handle / 2];
	n = fread(&mem[offset], 1, unit_size * lon, f); // Bytes leidos
	if((n + unit_size - 1) / unit_size < lon) {
		pila[sp] = 0;

		e(127);
	}
	else {
		if(n / unit_size < lon) {
			memset(&memb[offset * 4 + n],
			        0,
			        lon * unit_size - n);
		}

		pila[sp] = 1;
	}

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fwrite(VOID *data, INT length, INT handle)
// - fwrite(&buffer, longitud(en unit_size), handle)
//════════════════════════════════════════════════════════════════════════════
void DIV_fwrite(void) {
	int offset, lon, handle;
	FILE *f;

	if(unit_size < 1) {
		unit_size = 1;
	}

	handle = pila[sp--];
	lon    = pila[sp--];
	offset = pila[sp];

	if(!(handle & 1) || handle < 1 || handle > 63) {
		e(170);
		return;
	}
	if(tabfiles[handle / 2] == 0) {
		e(170);
		return;
	}

	if (!capar(offset) || !capar(offset + (lon * unit_size) / 4)) {
		pila[sp] = 0;

		e(122);
		return;
	}

	f = (FILE *)tabfiles[handle / 2];
	if(fwrite(&mem[offset], unit_size, lon, f) != lon) {
		pila[sp] = 0;

		e(124);
	}
	else {
		pila[sp] = 1;
	}

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV fseek(INT handle, INT position, INT mode)
//════════════════════════════════════════════════════════════════════════════
void DIV_fseek(void) {
	int handle, offset, mode;
	FILE *f;

	if(unit_size < 1) {
		unit_size  = 1;
	}

	mode   = pila[sp--];
	offset = pila[sp--] * unit_size;
	handle = pila[sp];

	if(!(handle & 1) || handle < 1 || handle > 63) {
		e(170);
		return;
	}
	if(tabfiles[handle / 2] == 0) {
		e(170);
		return;
	}

	f = (FILE *)tabfiles[handle / 2];
	fseek(f, offset, mode);

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV ftell(INT handle)
//════════════════════════════════════════════════════════════════════════════
void DIV_ftell(void) {
	int handle;

	if(unit_size < 1) {
		unit_size = 1;
	}

	handle = pila[sp];

	if(!(handle & 1) || handle < 1 || handle > 63) {
		e(170);
		return;
	}
	if(tabfiles[handle / 2] == 0) {
		e(170);
		return;
	}

	pila[sp] = (int)(ftell((FILE *)tabfiles[handle / 2]) + unit_size - 1) / unit_size;
}

//════════════════════════════════════════════════════════════════════════════
// DIV filelength(INT handle)
//════════════════════════════════════════════════════════════════════════════
void DIV_filelength(void) {
	int handle;
	int pos,len;

	if(unit_size < 1) {
		unit_size = 1;
	}

	handle = pila[sp];

	if(!(handle & 1) || handle < 1 || handle > 63) {
		e(170);
		return;
	}
	if(tabfiles[handle / 2] == 0) {
		e(170);
		return;
	}

	pos = ftell((FILE *)tabfiles[handle / 2]);
	fseek((FILE *)tabfiles[handle / 2], 0, SEEK_END);
	len = (ftell((FILE *)tabfiles[handle / 2]) + unit_size - 1) / unit_size;
	fseek((FILE *)tabfiles[handle / 2], pos, SEEK_SET);

	pila[sp] = len;
}

//════════════════════════════════════════════════════════════════════════════
// DIV flush()
//════════════════════════════════════════════════════════════════════════════
void DIV_flush(void) {
	pila[++sp] = 0;//flushall()-numfiles; // TODO
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_dirinfo(STRING dirmask, INT flags)
//════════════════════════════════════════════════════════════════════════════
// function 130 int get_dirinfo(0, 0)
// Lee un directorio ("dir\*.pr?" ,_hidden + _system + _subdir)
// en la estructura dirinfo(files, name[]) y devuelve "files" (n�mero)
void DIV_get_dirinfo(void) {
	//char *filenames (en mem[imem_max + 258 * 5])
	//dirinfo->files
	//dirinfo->name[]
	unsigned rc;
	struct find_t ft;
	int x = 0;
	int flags;

	flags = pila[sp--];

	memset(dirinfo->name,
	       0,
	       1025 * 4);

	rc = _dos_findfirst((char *)&mem[pila[sp]], flags, &ft);
	while(!rc) {
		strcpy(&filenames[x * 16], ft.name);
		dirinfo->name[x] = imem_max + 258 * 5 + x * 4;

		rc = _dos_findnext(&ft);

		x++;
	}

	qsort(filenames, x, 16, strcmpsort);

	dirinfo->files = pila[sp] = x;
}

//════════════════════════════════════════════════════════════════════════════
// DIV get_fileinfo(STRING filename)
//════════════════════════════════════════════════════════════════════════════
#define YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define MONTH(t)  ((t & 0x01E0) >> 5)
#define DAY(t)    (t & 0x001F)
#define HOUR(t)   ((t & 0xF800) >> 11)
#define MINUTE(t) ((t & 0x07E0) >> 5)
#define SECOND(t) ((t & 0x001F) << 1)

void DIV_get_fileinfo(void) {
	unsigned rc;
	struct find_t ft;

	char drive   [_MAX_DRIVE + 1];
	char dir     [_MAX_DIR   + 1];
	char fname   [_MAX_FNAME + 1];
	char ext     [_MAX_EXT   + 1];
	char filename[_MAX_PATH  + 1];

	if(unit_size < 1) {
		unit_size = 1;
	}

	strcpy(filename, (char *)&mem[pila[sp]]);

	pila[sp] = 0;

	rc = _dos_findfirst(filename, _A_NORMAL | _A_SYSTEM | _A_HIDDEN | _A_SUBDIR, &ft);
	if(rc) {
		return;
	}

	if(_fullpath(full, filename, _MAX_PATH) == NULL) {
		return;
	}

	strupr(full);
	_splitpath(full, drive, dir, fname, ext);

	strcpy(fileinfo->fullpath, full);
	fileinfo->drive  = (int)drive[0] - 64;
	strcpy(fileinfo->dir, dir);
	strcpy(fileinfo->name, fname);
	strcpy(fileinfo->ext, ext);
	fileinfo->size   = (ft.size + unit_size - 1) / unit_size;
	fileinfo->day    = DAY(ft.wr_date);
	fileinfo->month  = MONTH(ft.wr_date);
	fileinfo->year   = YEAR(ft.wr_date);
	fileinfo->hour   = HOUR(ft.wr_time);
	fileinfo->min    = MINUTE(ft.wr_time);
	fileinfo->sec    = SECOND(ft.wr_time);
	fileinfo->attrib = ft.attrib;

	pila[sp] = 1;
}

//════════════════════════════════════════════════════════════════════════════
// DIV getdrive()
// - Devuelve: n�mero de unidad (1-A, 2-B, 3-C, ...)
//════════════════════════════════════════════════════════════════════════════
void DIV_getdrive(void) {
#ifdef DOS
	unsigned int drive;

	_dos_getdrive(&drive);

	pila[++sp] = drive;
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV setdrive(INT drive)
//════════════════════════════════════════════════════════════════════════════
void DIV_setdrive(void) {
	unsigned int total;

	_dos_setdrive(pila[sp], &total);

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV chdir(STRING name)
// - Devuelve: 1 - exito, 0 - error
//════════════════════════════════════════════════════════════════════════════
void DIV_chdir(void) {
	if(chdir((char *)&mem[pila[sp]])) {
		pila[sp] = 0;
	}
	else {
		pila[sp] = 1;
	}
}

//════════════════════════════════════════════════════════════════════════════
// DIV mkdir(STRING name)
//════════════════════════════════════════════════════════════════════════════
void DIV_mkdir(void) {
	char *buffer;
	char cwork[256];
	int x;

	buffer = (char *)&mem[pila[sp]];

	if(strlen(buffer)) {
		if(buffer[strlen(buffer) - 1] == '/') {
			buffer[strlen(buffer) - 1] = 0;
		}
	}

	for(x = 0; x < strlen(buffer); x++) {
		if(x > 0 && buffer[x - 1] == ':') {
			continue;
		}
		if(buffer[x] == '/') {
			strcpy(cwork, buffer);
			cwork[x] = 0;
			__mkdir(cwork);
		}
	}
	__mkdir(buffer);

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV remove(STRING name)
//════════════════════════════════════════════════════════════════════════════
void DIV_remove(void) {
	unsigned rc;
	struct find_t ft;
	int x;
	char cwork1[_MAX_PATH + 1];
	char cwork2[_MAX_PATH + 1];
	char cwork3[_MAX_PATH + 1];

	strcpy(cwork2, (char *)&mem[pila[sp]]);

	pila[sp] = 0;

	for(x = strlen(cwork2) - 1; ; x--) {
		if(x == -1) {
			cwork2[0] = 0;
			break;
		}
		if(cwork2[x] == '/') {
			cwork2[x + 1] = 0;
			break;
		}
	}

	rc = _dos_findfirst((char *)&mem[pila[sp]], _A_NORMAL | _A_SYSTEM | _A_HIDDEN,&ft);
	while(!rc) {
		strcpy(cwork3, cwork2);
		strcat(cwork3, ft.name);
		if(_fullpath(cwork1, cwork3, _MAX_PATH) == NULL) {
			strcpy(cwork1, ft.name);
		}

		_dos_setfileattr(cwork1, _A_NORMAL);

		remove(cwork1);

		rc = _dos_findnext(&ft);
	}

	rc = _dos_findfirst((char *)&mem[pila[sp]], _A_SUBDIR, &ft);
	while(!rc) {
		strcpy(cwork3, cwork2);
		strcat(cwork3, ft.name);
		if(_fullpath(cwork1, cwork3, _MAX_PATH) == NULL) {
			strcpy(cwork1, ft.name);
		}

		_dos_setfileattr(cwork1, _A_SUBDIR);

		rmdir(cwork1);

		rc = _dos_findnext(&ft);
	}
}

//════════════════════════════════════════════════════════════════════════════
// Funciones de memoria o sistema
//════════════════════════════════════════════════════════════════════════════
/*
function 137 int disk_free(0)                      // Espacio disponible en una (unidad), en bytes
function 138 int memory_free()                     // Memoria disponible (en Kbs)
function 139 int ignore_error(0)                   // Ignorar un error de ejecuci�n (n�mero)
*/
typedef struct _t_meminfo {
	unsigned Bloque_mas_grande_disponible;
	unsigned Maximo_de_paginas_desbloqueadas;
	unsigned Pagina_bloqueable_mas_grande;
	unsigned Espacio_de_direccionamiento_lineal;
	unsigned Numero_de_paginas_libres_disponibles;
	unsigned Numero_de_paginas_fisicas_libres;
	unsigned Total_de_paginas_fisicas;
	unsigned Espacio_de_direccionamiento_lineal_libre;
	unsigned Tamano_del_fichero_de_paginas;
	unsigned reservado[3];
} t_meminfo;

int Mem_GetHeapFree() {
#ifdef DOS
	struct _heapinfo miheap;
	int status = 0,total = 0;

	miheap._pentry = NULL;
	_heapmin();

	for(;;) {
		statu s =_heapwalk(&miheap);
		if(status != _HEAPOK) {
			break;
		}

		if(miheap._useflag == _FREEENTRY) {
			total += miheap._size;
		}
	}
//	if(status != _HEAPEND) {
//		return -1;
//	}

	return total;
#endif

	return 65535;
}

void GetFreeMem(t_meminfo *Meminfo) {
#ifdef DOS
	union REGS regs;
	struct SREGS sregs;

	regs.x.eax=0x0500;
	memset(&sregs,
	        0,
	        sizeof(sregs));

	sregs.es   = FP_SEG(Meminfo);
	regs.x.edi = FP_OFF(Meminfo);

	int386x(0x031, &regs, &regs, &sregs);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV disk_free(INT drive)
//════════════════════════════════════════════════════════════════════════════
void DIV_disk_free(void) {
#ifdef DOS
	long MBfree;
	union REGS regs;
	struct diskfree_t structdiskfree;

	regs.w.ax =0x4409;
	regs.w.bx = pila[sp];
	// informaci�n de dispositivo
	int386(0x21, &regs, &regs);

	// si no hubo error
	if(!(regs.w.cflag & INTR_CF)) {
		// i/o no permitida
		if(!(regs.w.dx & (1 << 9))) {
			structdiskfree.avail_clusters=0;

			_dos_getdiskfree(pila[sp], &structdiskfree);

			MBfree  = structdiskfree.sectors_per_cluster * structdiskfree.bytes_per_sector;
			MBfree *= structdiskfree.avail_clusters;

			pila[sp] = MBfree / 1024;
		}
		else {
			pila[sp] = 0;
		}
	}
	else {
		pila[sp] = 0;
	}
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV memory_free()
//════════════════════════════════════════════════════════════════════════════
void DIV_memory_free(void) {
#ifdef DOS
	meminfo Mi_meminfo;
	int mem;

	_heapshrink();

	GetFreeMem(&Mi_meminfo);
	mem = Mem_GetHeapFree();

	pila[++sp] = (Mi_meminfo.Bloque_mas_grande_disponible + mem) / 1024;
#else
	pila[++sp] = 0;
#endif
}

//════════════════════════════════════════════════════════════════════════════
// DIV ignore_error(INT number)
//════════════════════════════════════════════════════════════════════════════
void DIV_ignore_error(void) {
	int n;

	n = 0;
	while(n < nomitidos) {
		if(omitidos[n] == pila[sp]) {
			break;
		}

		n++;
	}
	if(n >= nomitidos && nomitidos < 127) {
		omitidos[nomitidos++] = pila[sp];
	}
	else if(nomitidos == 127) {
		e(168);
	}

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// Funciones matematicas
//════════════════════════════════════════════════════════════════════════════
#define pi     180000
#define radian 57295.77951

//════════════════════════════════════════════════════════════════════════════
// DIV sin(INT angle)
// - Devuelve el seno de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_sin(void) {
	float angle = (float)pila[sp] / radian;

	pila[sp] = (int)((float)sin(angle) * 1000);
}

//════════════════════════════════════════════════════════════════════════════
// DIV asin(INT angle)
// - Devuelve el arco-seno de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_asin(void) {
	float seno = (float)pila[sp] / 1000.0;

	if(pila[sp] < -1000 || pila[sp] > 1000) {
		pila[sp] = 0;

		e(171);
		return;
	}

	pila[sp] = (int)((float)asin(seno) * radian);
}

//════════════════════════════════════════════════════════════════════════════
// DIV cos(INT angle)
// - Devuelve el coseno de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_cos(void) {
	float angle = (float)pila[sp] / radian;

	pila[sp] = (int)((float)cos(angle) * 1000);
}

//════════════════════════════════════════════════════════════════════════════
// DIV acos(INT angle)
// - Devuelve el arco-coseno de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_acos(void) {
	float coseno = (float)pila[sp] / 1000.0;

	if(pila[sp] < -1000 || pila[sp] > 1000) {
		pila[sp] = 0;

		e(171);
		return;
	}

	pila[sp] = (int)((float)acos(coseno) * radian);
}

//════════════════════════════════════════════════════════════════════════════
// DIV tan(INT angle)
// - Devuelve la tangente de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_tan(void) {
	float angle;

	while(pila[sp] >=  180000) { pila[sp] -= 360000; }
	while(pila[sp] <= -180000) { pila[sp] += 360000; }

	if(pila[sp] ==  90000) { pila[sp] = 0x7FFFFFFF; return; }
	if(pila[sp] == -90000) { pila[sp] = 0x80000000; return; }

	angle = (float)pila[sp] / radian;

	pila[sp] = (int)((float)tan(angle) * 1000);
}

//════════════════════════════════════════════════════════════════════════════
// DIV atan(INT angle)
// - Devuelve la arco-tangente de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_atan(void) {
	float tangente;

	tangente = (float)pila[sp] / 1000.0;

	pila[sp] = (int)((float)atan(tangente) * radian);
}

//════════════════════════════════════════════════════════════════════════════
// DIV atan2(INT angle)
// - Devuelve la arco-tangente de un angulo dado en milesimas
//════════════════════════════════════════════════════════════════════════════
void DIV_atan2(void) {
	float x, y;

	x = (float)pila[sp--];
	y = (float)pila[sp];

	pila[sp] = (int)((float)atan2(y, x) * radian);
}

//═══════════════════════════════════════════════════════════════════════════════════════
// DIV draw(INT type, INT color, INT alpha, INT region, INT x0, INT y0, INT x1, INT y1)
//═══════════════════════════════════════════════════════════════════════════════════════
#define tipo_mayor 5 // 1-Linea 2-Rect�ngulo 3-Rec.Relleno 4-Circulo 5-Cir.Rell.

void DIV_draw(void) {
	int x;

	x = 0;
	while(drawing[x].tipo && x < max_drawings) {
		x++;
	}
	if(x == max_drawings) {
		sp -= 7;
		pila[sp] = 0;

		e(172);
		return;
	}

	drawing[x].y1         = pila[sp--];
	drawing[x].x1         = pila[sp--];
	drawing[x].y0         = pila[sp--];
	drawing[x].x0         = pila[sp--];
	drawing[x].region     = pila[sp--];
	drawing[x].porcentaje = pila[sp--];
	drawing[x].color      = pila[sp--];
	drawing[x].tipo       = pila[sp];

	if(drawing[x].tipo < 1 || drawing[x].tipo > tipo_mayor) {
		drawing[x].tipo = 0;

		e(173);
	}
	if(drawing[x].color < 0 || drawing[x].color > 255) {
		drawing[x].tipo = 0;

		e(154);
	}
	if(drawing[x].porcentaje < 0 || drawing[x].porcentaje > 15) {
		drawing[x].tipo = 0;

		e(174);
	}
	if (drawing[x].region < 0 || drawing[x].region >= max_region) {
		drawing[x].tipo = 0;

		e(108);
	}

	if(drawing[x].tipo) {
		pila[sp] = x * 2 + 1;
	}
	else {
		pila[sp] = 0;
	}
}

//═══════════════════════════════════════════════════════════════════════════════════════
// DIV delete_draw(INT draw_id)
//═══════════════════════════════════════════════════════════════════════════════════════
void DIV_delete_draw(void) {
	int draw_id;

	if((draw_id = pila[sp]) == - 1) {
		for(draw_id = 0; draw_id < max_drawings; draw_id++) {
			drawing[draw_id].tipo = 0;
		}
	}
	else {
		if((draw_id & 1) == 0 || draw_id < 1 || draw_id > max_drawings * 2 - 1) {
			pila[sp] = 0;

			e(175);
			return;
		}

		draw_id /= 2;

		if(drawing[draw_id].tipo == 0) {
			pila[sp] = 0;

			e(175);
			return;
		}

		drawing[draw_id].tipo = 0;
	}
}

//═══════════════════════════════════════════════════════════════════════════════════════
// DIV move_draw(INT draw_id, DWORD color, INT alpha, INT x0, INT y0, INT x1, INT y1)
//═══════════════════════════════════════════════════════════════════════════════════════
void DIV_move_draw(void) {
	int draw_id;

	draw_id = pila[sp - 6];

	if((draw_id & 1) == 0 || draw_id < 1 || draw_id > max_drawings * 2 - 1) {
		sp -= 6;
		pila[sp] = 0;

		e(175);
		return;
	}

	draw_id /= 2;

	if(drawing[draw_id].tipo == 0) {
		pila[sp] = 0;

		e(175);
		return;
	}

	drawing[draw_id].y1         = pila[sp--];
	drawing[draw_id].x1         = pila[sp--];
	drawing[draw_id].y0         = pila[sp--];
	drawing[draw_id].x0         = pila[sp--];
	drawing[draw_id].porcentaje = pila[sp--];
	drawing[draw_id].color      = pila[sp--];

	if(drawing[draw_id].color < 0 || drawing[draw_id].color > 255) {
		drawing[draw_id].tipo = 0;

		e(154);
	}
	if(drawing[draw_id].porcentaje < 0 || drawing[draw_id].porcentaje > 15) {
		drawing[draw_id].tipo = 0;

		e(174);
	}

	pila[sp] = 0;
}

//════════════════════════════════════════════════════════════════════════════
// DIV save_map(INT fpg_id, INT graph, STRING fname)
// DIV save_pcx(INT fpg_id, INT graph, STRING fname)
// Devuelve: 1-Exito 0-Error
//════════════════════════════════════════════════════════════════════════════
int graba_PCX(byte *mapa, int an, int al, FILE *f);
int graba_MAP(byte *mapa, int an, int al, FILE *f);

void DIV_save_mapcx(int tipo) {
	int file, graph;
	int *ptr;
	int an, al;
	byte *buffer;
	char cwork[256];
	FILE *f;

	strcpy(cwork, (char*)&mem[pila[sp--]]);
	graph = pila[sp--];
	file  = pila[sp];

	pila[sp] = 0;

	if(file > max_fpgs || file < 0) {
		e(109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graph <= 0 || graph >= max_grf) {
		e(110);
		return;
	}
	if(g[file].grf == NULL) {
		e(111);
		return;
	}
	if((ptr = g[file].grf[graph]) == NULL) {
		e(121);
		return;
	}

	an     = ptr[13];
	al     = ptr[14];
  buffer = (byte*)ptr + 64 + ptr[15] * 4;

	if((f = open_save_file((byte *)cwork)) == NULL) {
		e(123);
		return;
	}
	if(tipo) {
		if(graba_PCX(buffer, an, al, f)) {
			fclose(f);

			e(100);
			return;
		}
	}
	else {
		if(graba_MAP(buffer, an, al, f)) {
			fclose(f);

			e(100);
			return;
		}
	}
	fclose(f);

	pila[sp] = 1;

	max_reloj += get_reloj() - old_reloj;
}

//════════════════════════════════════════════════════════════════════════════
// DIV write_in_map(INT fnt_id, VOID text, INT anchor)
// - Devuelve el c�digo del mapa
// - Se descarga igualmente con unload_map(INT code)
//════════════════════════════════════════════════════════════════════════════
void DIV_write_in_map(void) {
	int centro, texto;
	int cx, cy, an, al;
	int fuente;

	byte *ptr, *ptr2;

	centro = pila[sp--];
	texto  = pila[sp--];
	fuente = pila[sp];

	if(fuente < 0 || fuente >= max_fonts) {
		e(116);
		return;
	}
	if(fonts[fuente] == 0) {
		e(116);
		return;
	}
	if(centro < 0 || centro > 8) {
		e(117);
		return;
	}

	checkpal_font(fuente);

	fnt  = (t_fnt_table *)((byte *)fonts[fuente] + 1356);
	an   = 0;
	al   = f_i[fuente].alto;
	ptr  = (byte *)&mem[texto];
	ptr2 = ptr;
	while(*ptr2) {
		if(fnt[*ptr2].ancho == 0) {
			an += f_i[fuente].espacio;
			ptr2++;
		}
		else {
			an += fnt[*ptr2].ancho;
			ptr2++;
		}
	}

	cx = 0;
	cy = 0;
	switch(centro) {
		case 0: {                                 break; }
		case 1: { cx = (an >> 1);                 break; }
		case 2: { cx = an - 1;                    break; }
		case 3: { cy = (al >> 1);                 break; }
		case 4: { cx = (an >> 1); cy = (al >> 1); break; }
		case 5: { cx = an - 1;    cy = (al >> 1); break; }
		case 6: { cy = al - 1;                    break; }
		case 7: { cx = (an >> 1); cy = al - 1;    break; }
		case 8: { cx = an - 1;    cy = al - 1;    break; }
	}

	ptr2 = ptr;

	if((ptr = (byte *)malloc(1330 + 64 + 4 + an * al)) != NULL) {
		ptr += 1330; // fix load_map/unload_map
		*((int  *)ptr + 13) = an;
		*((int  *)ptr + 14) = al;
		*((int  *)ptr + 15) = 1; // Se define un punto de control (el centro)
		*((word *)ptr + 32) = cx;
		*((word *)ptr + 33) = cy;
		memset(ptr + 4 + 64,
		       0,
		       an * al);

		while(g[0].grf[next_map_code]) {
			if(next_map_code++ == 1999) {
				next_map_code = 1000;
			}
		}
		g[0].grf[next_map_code] = (int *)ptr;

		pila[sp] = next_map_code;
	}
	else {
		e(100);
	}

	cx = 0; // Pintar el texto (ptr2) en ptr + 68 (an * al)

	while(*ptr2 && cx + fnt[*ptr2].ancho <= an) {
		if(fnt[*ptr2].ancho == 0) {
			cx += f_i[fuente].espacio;
			ptr2++;
		}
		else {
			texn2(ptr + 68,
			      an,
			      fonts[fuente] + fnt[*ptr2].offset,
			      cx,
			      fnt[*ptr2].incY,
			      fnt[*ptr2].ancho,
			      fnt[*ptr2].alto);

			cx = cx + fnt[*ptr2].ancho;
			ptr2++;
		}
	}
}

void texn2(byte *copia, int vga_an, byte *p, int x, int y, byte an, int al) {
	byte *q = copia + y * vga_an + x;
	int ancho = an;

	do {
		do {
			if(*p) {
				*q = *p;
			}

			p++;
			q++;
		}while(--an);

		q += vga_an - (an = ancho);
	}while(--al);
}


//══════════════════════════════════════════════════════════════════════════════
// Evaluador de expresiones (se le pasa el puntero en `expresion')
// Si todo fue bien, devuelve token=p_num y tnumero=n
//══════════════════════════════════════════════════════════════════════════════
enum tokens {
	p_inicio,
	p_ultimo,
	p_error,
	p_num,
	p_abrir,
	p_cerrar,
	p_add,
	p_sub,
	p_mul,
	p_div,
	p_mod,
	p_shl,
	p_shr,
	p_xor,
	p_or,
	p_and,
	p_not,
	p_sqrt,
	p_neg
};

struct {           // Para contener la expresion analizada
	int    token;
	double numero;
} expres[64];
int iexpres;       // N�mero de elementos introducidos en expres[]

int     token;     // Del tipo enumerado anterior
double  tnumero;   // Cuando token==p_num
char   *expresion; // Puntero a la expresi�n asciiz

double evaluar(void) {
	double pila[64];
	int sp = 0, n = 0;

	do {
		switch(expres[n].token) {
			case p_num: {
				pila[++sp] = expres[n].numero;
				break;
			}
			case p_or: {
				pila[sp - 1] = (double)((int)pila[sp - 1]|(int)pila[sp]);
				sp--;
				break;
			}
			case p_xor: {
				pila[sp - 1] = (double)((int)pila[sp - 1] ^ (int)pila[sp]);
				sp--;
				break;
			}
			case p_and: {
				pila[sp - 1] = (double)((int)pila[sp - 1 ] &(int)pila[sp]);
				sp--;
				break;
			}
			case p_add: {
				pila[sp - 1] += pila[sp];
				sp--;
				break;
			}
			case p_sub: {
				pila[sp - 1] -= pila[sp];
				sp--;
				break;
			}
			case p_mul: {
				pila[sp - 1] *= pila[sp];
				sp--;
				break;
			}
			case p_div: {
				if(pila[sp] == 0.0) {
					token = p_error;
					n     = iexpres;
				}
				else {
					pila[sp - 1] /= pila[sp];
					sp--;
				}
				break;
			}
			case p_mod: {
				if((int)pila[sp] == 0) {
					token = p_error;
					n     = iexpres;
				}
				else {
					pila[sp - 1] = (double)((int)pila[sp - 1] % (int)pila[sp]);
					sp--;
				}
				break;
			}
			case p_neg: {
				pila[sp] = -pila[sp];
				break;
			}
			case p_not: {
				pila[sp] = (double)((int)pila[sp] ^ -1);
				break;
			}
			case p_shr: {
				pila[sp - 1] = (double)((int)pila[sp - 1] >> (int)pila[sp]);
				sp--;
				break;
			}
			case p_shl: {
				pila[sp - 1] = (double)((int)pila[sp - 1] << (int)pila[sp]);
				sp--;
				break;
			}
			case p_sqrt: {
				if(pila[sp] < 0) {
					token = p_error;
					n     = iexpres;
				}
				else {
					if(pila[sp] < 2147483648) {
						pila[sp] = sqrt(pila[sp]);
					}
					else {
						token = p_error;
						n     = iexpres;
					}
				}
				break;
			}
			default: {
				token=p_error;
				n=iexpres;
				break;
			}
		}
	}while (++n<iexpres);

	if(sp != 1) {
		token = p_error;
	}

	return (pila[sp]);
}

void expres0() { // xor or and
	int p;

	expres1();
	while((p = token) >= p_xor && p <= p_and) {
		get_token();
		expres1();

		expres[iexpres].token = p;
		iexpres++;
	}
}

void expres1() { // << >>
	int p;

	expres2();
	while((p = token) >= p_shl && p <= p_shr) {
		get_token();
		expres2();

		expres[iexpres].token = p;
		iexpres++;
	}
}

void expres2() { // + -
	int p;

	expres3();
	while((p = token) >= p_add && p <= p_sub) {
		get_token();
		expres3();

		expres[iexpres].token = p;
		iexpres++;
	}
}

void expres3() { // * / %
	int p;

	expres4();
	while((p = token) >= p_mul && p <= p_mod) {
		get_token();
		expres4();

		expres[iexpres].token = p;
		iexpres++;
	}
}

void expres4() { // signo !
	int p;

	while((p = token) == p_add) {
		get_token();
	}
	if(p == p_not || p == p_sub) {
		if(p == p_sub) {
			p = p_neg;
		}

		get_token();
		expres4();

		expres[iexpres].token = p;
		iexpres++;
	}
	else {
		expres5();
	}
}

void expres5() {
	if(token == p_abrir) {
		get_token();
		expres0();

		if(token != p_cerrar) {
			token = p_error;
			return;
		}
		else {
			get_token();
		}
	}
	else if(token == p_sqrt) {
		get_token();
		expres5();

		expres[iexpres].token = p_sqrt;
		iexpres++;
	}
	else if(token == p_num) {
		expres[iexpres].token  = p_num;
		expres[iexpres].numero = tnumero;
		iexpres++;

		get_token();
	}
	else {
		token = p_error;
	}
}

double get_num(void) { // Lee el n�mero que hay en *expresion (double en hex o dec)
	double x = 0;
	double dec = 10;

	if(*expresion == '0' && tolower(*(expresion + 1)) == 'x') { // Numeros en hex
		expresion += 2;

		if((*expresion          >= '0' && *expresion          <= '9') ||
		   (tolower(*expresion) >= 'a' && tolower(*expresion) <= 'f')) {
			do {
				if(*expresion >= '0' && *expresion <= '9') {
					x = x * 16 + *expresion++ - 0x30;
				}
				else {
					x = x * 16 + tolower(*expresion++) - 'a' + 10;
				}
			}while((*expresion          >= '0' && *expresion          <= '9') ||
			       (tolower(*expresion) >= 'a' && tolower(*expresion) <= 'f'));
		}
		else {
			token = p_error;
		}
	}
	else {
		while(*expresion >= '0' && *expresion <= '9') {
			x *= 10;
			x += (*expresion - '0');

			expresion++;
		}
		if(*expresion == '.') {
			expresion++;

			if(*expresion < '0' || *expresion > '9') {
				token = p_error;
			}
			else {
				do {
				x   += (double)(*expresion-'0') / dec;
				dec *= 10;

				expresion++;
				}while(*expresion >= '0' && *expresion <= '9');
			}
		}
	}

	return (x);
}

void get_token(void) {
	char cwork[32];
	int n;

	if(token != p_error) {
reget_token:
		switch(*expresion++) {
			case ' ': {
				goto reget_token;
			}
			case 0: {
				token = p_ultimo;
				break;
			}
			case '0': case '1':
			case '2': case '3':
			case '4': case '5':
			case '6': case '7':
			case '8': case '9':
			case '.': {
				expresion--;
				token = p_num;
				tnumero = get_num();
				break;
			}
			case '(': {
				token = p_abrir;
				break;
			}
			case ')': {
				token = p_cerrar;
				break;
			}
			case '+': {
				token = p_add;
				break;
			}
			case '-': {
				token = p_sub;
				break;
			}
			case '*': {
				token = p_mul;
				break;
			}
			case '/': {
				token = p_div;
				break;
			}
			case '%': {
				token = p_mod;
				break;
			}
			case '<': {
				if(*expresion++ == '<') {
					token = p_shl;
				}
				else {
					token = p_error;
				}
				break;
			}
			case '>': {
				if (*expresion++ == '>') {
					token = p_shr;
				}
				else {
					token = p_error;
				}
				break;
			}
			case '^': {
				if(*expresion == '^') {
					expresion++;
				}
				token = p_xor;
				break;
			}
			case '|': {
				if(*expresion == '|') {
					expresion++;
				}
				token = p_or;
				break;
			}
			case '&': {
				if(*expresion == '&') {
					expresion++;
				}
				token = p_and;
				break;
			}
			case '!': {
				token = p_not;
				break;
			}
			default: {
				if(tolower(*(expresion - 1)) >= 'a' &&
				   tolower(*(expresion - 1)) <= 'z') {
					n       = 1;
					cwork[0] = tolower(*(expresion -1));
					while(n < 31                     &&
					      tolower(*expresion) >= 'a' &&
					      tolower(*expresion) <= 'z') {
						cwork[n++] = tolower(*expresion++);
					}
					cwork[n] = 0;
					if(!strcmp(cwork, "mod")) {
						token = p_mod;
					}
					else if(!strcmp(cwork, "not")) {
						token = p_not;
					}
					else if(!strcmp(cwork, "xor")) {
						token = p_xor;
					}
					else if(!strcmp(cwork, "or")) {
						token = p_or;
					}
					else if(!strcmp(cwork, "and")) {
						token = p_and;
					}
					else if(!strcmp(cwork,"sqrt")) {
						token = p_sqrt;
					}
					else if(!strcmp(cwork, "pi")) {
						token = p_num;
						tnumero = 3.14159265359;
					}
					else {
						token = p_error;
					}
				}
				else {
					token = p_error;
				}
				break;
			}
		}
	}
}

void calcular(void) {
	double evaluacion;

	token   =p_inicio; // No hay ningun token inicialmente
	iexpres = 0;       // Inicializa el contador de expresiones

	get_token();       // Obtiene el primer token
	expres0();         // Se analiza la expresion

		// Se analiz� con �xito la expresion
	if(token == p_ultimo) {
		evaluacion = evaluar();

		// Se evalu� con �xito
		if(token != p_error) {
			token   = p_num;
			tnumero = evaluacion;
		}
	}
	else {
		token = p_error;
	}
}


//══════════════════════════════════════════════════════════════════════════════
// DIV calculate(STRING expression)
// - Si error devuelve 0, si no el resultado
//══════════════════════════════════════════════════════════════════════════════
void DIV_calculate(void) {
	expresion = (char *)&mem[pila[sp]];

	calcular();

	if(token == p_num) {
		pila[sp] = (int)tnumero;
	}
	else {
		pila[sp] = 0;
	}
}

//══════════════════════════════════════════════════════════════════════════════
// DIV itoa(INT number)
// - Devuelve la cadena
//══════════════════════════════════════════════════════════════════════════════
extern int nullstring[4];
extern int nstring;

void DIV_itoa(void) {
	itoa(pila[sp], (char *)&mem[nullstring[nstring]], 10);

	pila[sp] = nullstring[nstring];

	nstring = ((nstring + 1) & 3);
}

//══════════════════════════════════════════════════════════════════════════════
// DIV malloc(INT size)
// - Retorna: 0 o �ndice de mem (impar)
//══════════════════════════════════════════════════════════════════════════════
// OJO El �nico problema es que los STRING dentro de un malloc no tendr�n la marca de cadena 0xDAD... !!!
void DIV_malloc(void) {
	byte *p;
	int con = 0;

	while(con < 256 && divmalloc[con].ptr) {
		con++;
	}
	if(con == 256) {
		pila[sp] = 0;

		e(179);
		return;
	}

	if(pila[sp] < 1) {
		pila[sp] = 0;

		e(181);
		return;
	}

	divmalloc[con].ptr = (byte *)malloc(pila[sp] * 4 + 4 + 3);
	if(!divmalloc[con].ptr) {
		pila[sp] = 0;

		e(100);
		return;
	}

	memset(divmalloc[con].ptr,
	       0,
	       pila[sp] * 4 + 4 + 3);

	p = (byte *)((((memptrsize)divmalloc[con].ptr + 3) / 4) * 4);

	divmalloc[con].imem1 = ((memptrsize)p - (memptrsize)mem) / 4;
	if(!(divmalloc[con].imem1 & 1)) {
		divmalloc[con].imem1++;
	}

	divmalloc[con].imem2 = divmalloc[con].imem1 + pila[sp];

	pila[sp] = divmalloc[con].imem1;
}

//══════════════════════════════════════════════════════════════════════════════
// DIV free(INT ptr)
//══════════════════════════════════════════════════════════════════════════════
void DIV_free(void) {
	int con = 0;

	while(con < 256 && divmalloc[con].imem1 != pila[sp]) {
		con++;
	}
	if(con == 256 || pila[sp] == 0) {
		pila[sp] = 0;

		e(180);
		return;
	}

	free(divmalloc[con].ptr);

	divmalloc[con].ptr   = NULL;
	divmalloc[con].imem1 = 0;
	divmalloc[con].imem2 = 0;

	pila[sp] = 1;
}

//══════════════════════════════════════════════════════════════════════════════
// DIV encode(VOID *data, INT length, STRING key)
// - Devuelve 0 � 1
//══════════════════════════════════════════════════════════════════════════════
void init_rnd_coder(int n, char *clave);
byte rndb(void);

void DIV_encode(void) {
	int offset, size, clave;
	int n;
	byte *ptr;

	clave  = pila[sp--];
	size   = pila[sp--];
	offset = pila[sp];

	if(!capar(offset) || !capar(offset + size)) {
		pila[sp] = 0;

		e(182);
		return;
	}

	pila[sp] = 1;

	init_rnd_coder(size + 33, (char *)&mem[clave]);

	ptr = (byte *)&mem[offset];
	for(n = 0; n < size * 4; n++) {
		ptr[n] ^= rndb();
	}
}

//══════════════════════════════════════════════════════════════════════════════
// DIV encode_file(STRING fname, STRING key)
// - Devuelve 0 � 1
//══════════════════════════════════════════════════════════════════════════════
void _encriptar(int encode, char *fichero, char *clave) {
	char full [_MAX_PATH  + 1];
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	int n, size;
	byte *ptr, *p;
	FILE *f;

	if((f = fopen(fichero, "rb")) != NULL) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		if((ptr = (byte *)malloc(size)) != NULL) {
			fseek(f, 0, SEEK_SET);
			if(fread(ptr, 1, size, f) == size) {
				fclose(f);
			}
			else {
				fclose(f);

				free(ptr);

				pila[sp] = 0;

				e(127);
				return;
			}
		}
		else {
			fclose(f);

			pila[sp] = 0;

			e(100);
			return;
		}
	}
	else {
		pila[sp] = 0;

		e(105);
		return;
	}

	if(encode) {
		if(!strcmp((char *)ptr, "dj!\x1a\x0d\x0a\xff")) {
			return;
		}
		else {
			p = ptr;
		}
	}
	else {
		if(strcmp((char *)ptr, "dj!\x1a\x0d\x0a\xff")) {
			return;
		}
		else {
			p     = ptr + 8;
			size -= 8;
		}
	}

	init_rnd_coder(size + 1133, clave);

	for(n = 0; n < size; n++) {
		p[n] ^= rndb();
	}

	_splitpath(fichero, drive, dir, fname, ext);

	strcpy(ext, "dj!");
	_makepath(full, drive, dir, fname, ext);

	if(rename(fichero, full)) {
		free(ptr);

		pila[sp] = 0;

		e(105);
		return;
	}

	if((f = open_save_file((byte *)fichero)) == NULL) {
		rename(full, fichero);

		free(ptr);

		pila[sp] = 0;

		e(105);
		return;
	}

	if(encode) {
		if(fwrite("dj!\x1a\x0d\x0a\xff", 1, 8, f) != 8) {
			fclose(f);

			remove(fichero);
			rename(full, fichero);

			free(ptr);

			pila[sp] = 0;

			e(105);
			return;
		}
	}

	if(fwrite(p, 1, size, f) != size) {
		fclose(f);

		remove(fichero);
		rename(full, fichero);

		free(ptr);

		pila[sp] = 0;

		e(105);
		return;
	}

	// Si todo ha ido bien ...

	fclose(f);
	remove(full);

	free(ptr);
}

void DIV_encode_file(int encode) {
	unsigned rc;
	struct find_t ft;
	int x;
	char cwork1[_MAX_PATH + 1];
	char cwork2[_MAX_PATH + 1];
	char cwork3[_MAX_PATH + 1];
	byte *name, *clave;

	clave = (byte *)&mem[pila[sp--]];
	name  = (byte *)&mem[pila[sp]];

	pila[sp] = 1;

	strcpy(cwork2, (char *)name);
	for(x = strlen(cwork2) - 1; ; x--) {
		if(x == -1) {
			cwork2[0] = 0;
			break;
		}
		if(cwork2[x] == '/') {
			cwork2[x + 1] = 0;
			break;
		}
	}

	rc = _dos_findfirst((char *)name, _A_NORMAL, &ft);
	while(!rc) {
		strcpy(cwork3, cwork2);
		strcat(cwork3, ft.name);
		if(_fullpath(cwork1, cwork3, _MAX_PATH) == NULL) {
			strcpy(cwork1, ft.name);
		}

		_dos_setfileattr(cwork1, _A_NORMAL);

		_encriptar(encode, cwork1, (char *)clave);

		rc = _dos_findnext(&ft);
	}

	max_reloj += get_reloj() - old_reloj;
}

//══════════════════════════════════════════════════════════════════════════════
// DIV compress(STRING fname)
// Devuelve 0 � 1
//══════════════════════════════════════════════════════════════════════════════
int divcompress(unsigned char *dest, unsigned long *destLen, unsigned char *source, unsigned long sourceLen);

void _comprimir(int encode, char *fichero) {
	char full [_MAX_PATH  + 1];
	char drive[_MAX_DRIVE + 1];
	char dir  [_MAX_DIR   + 1];
	char fname[_MAX_FNAME + 1];
	char ext  [_MAX_EXT   + 1];
	unsigned long size,size2;
	byte *ptr,*ptr_dest;
	FILE *f;

	if((f = fopen(fichero, "rb")) != NULL) {
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		if((ptr = (byte *)malloc(size)) != NULL) {
			fseek(f, 0, SEEK_SET);
			if(fread(ptr, 1, size, f) == size) {
				fclose(f);
			}
			else {
				fclose(f);

				free(ptr);

				pila[sp] = 0;

				e(127);
				return;
			}
		}
		else {
			fclose(f);

			pila[sp] = 0;

			e(100);
			return;
		}
	}
	else {
		pila[sp] = 0;

		e(105);
		return;
	}

	if(encode) {
		if(!strcmp((char *)ptr, "zx!\x1a\x0d\x0a\xff")) {
			return;
		}

		size2 = size + size / 100 + 256;
		if((ptr_dest = (byte *)malloc(size2)) == NULL) {
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#ifdef ZLIB
		if(compress(ptr_dest, &size2, ptr, size)) {
			free(ptr_dest);
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#else
		if(false) {
			free(ptr_dest);
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#endif

		// Si no se gana espacio, se deja el fichero sin comprimir
		if(size2 >= size - 12) {
			free(ptr_dest);
			free(ptr);

			return;
		}
	}
	else {
		if(strcmp((char *)ptr, "zx!\x1a\x0d\x0a\xff")) {
			return;
		}

		size2 = *(int *)(ptr + 8);
		if((ptr_dest = (byte *)malloc(size2)) == NULL) {
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#ifdef ZLIB
		if(uncompress(ptr_dest, &size2, ptr + 12, size - 12)) {
			free(ptr_dest);
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#else
		if(true) {
			free(ptr_dest);
			free(ptr);

			pila[sp] = 0;

			e(100);
			return;
		}
#endif

		size2 = *(int *)(ptr + 8);
	}

	free(ptr);

	_splitpath(fichero, drive, dir, fname, ext);

	strcpy(ext, "ZX!");
	_makepath(full, drive, dir, fname, ext);

	if(rename(fichero, full)) {
		free(ptr_dest);

		pila[sp] = 0;

		e(105);
		return;
	}

	if((f = open_save_file((byte *)fichero)) == NULL) {
		rename(full, fichero);

		free(ptr_dest);

		pila[sp] = 0;

		e(105);
		return;
	}

	if(encode) {
		if(fwrite("zx!\x1a\x0d\x0a\xff", 1, 8, f) != 8) {
			fclose(f);
			remove(fichero);
			rename(full, fichero);

			free(ptr_dest);

			pila[sp] = 0;

			e(105);
			return;
		}
		if(fwrite(&size, 1, 4, f) != 4) {
			fclose(f);
			remove(fichero);
			rename(full,fichero);

			free(ptr_dest);

			pila[sp] = 0;

			e(105);
			return;
		}
	}

	if(fwrite(ptr_dest, 1, size2, f) != size2) {
		fclose(f);
		remove(fichero);
		rename(full,fichero);

		free(ptr_dest);

		pila[sp] = 0;

		e(105);
		return;
	}

	// Si todo ha ido bien ...
	fclose(f);
	remove(full);

	free(ptr_dest);
}

void DIV_compress(int encode) {
	unsigned rc;
	struct find_t ft;
	int x;
	char cwork1[_MAX_PATH + 1];
	char cwork2[_MAX_PATH + 1];
	char cwork3[_MAX_PATH + 1];
	byte *name;

	name = (byte *)&mem[pila[sp]];

	pila[sp] = 1;

	strcpy(cwork2, (char *)name);
	for(x = strlen(cwork2) - 1; ; x--) {
		if(x == -1) {
			cwork2[0] = 0;
			break;
		}
		if(cwork2[x] == '/') {
			cwork2[x + 1] = 0;
			break;
		}
	}

	rc = _dos_findfirst((char *)name , _A_NORMAL ,&ft);
	while(!rc) {
		strcpy(cwork3, cwork2);
		strcat(cwork3, ft.name);
		if(_fullpath(cwork1, cwork3, _MAX_PATH) == NULL) {
			strcpy(cwork1, ft.name);
		}
		_dos_setfileattr(cwork1, _A_NORMAL);

		_comprimir(encode, cwork1);

		rc = _dos_findnext(&ft);
	}

	max_reloj += get_reloj() - old_reloj;
}


//══════════════════════════════════════════════════════════════════════════════
// write ... printf, fprintf, itoatoi, calculate, ...
// funciones de modo texto ... ???

//══════════════════════════════════════════════════════════════════════════════
// Funcion para evitar los page fault (��� OJO con incluir los malloc !!!)
//══════════════════════════════════════════════════════════════════════════════
int capar(int dir) {
	int n;

	if(dir > 0 && dir <= imem_max) {
		return (dir);
	}
	else {
		for(n = 0; n < 256; n++) {
			if(dir >= divmalloc[n].imem1 && dir <= divmalloc[n].imem2) {
				return (dir);
			}
		}
	}

	return (0);
}

//══════════════════════════════════════════════════════════════════════════════
// Switch principal
//══════════════════════════════════════════════════════════════════════════════
#ifdef DEBUG
extern int f_time[256]; // Tiempo consumido por las diferentes funciones
#endif

void function(void) {
#ifdef DEBUG
	int oticks = get_ticks();
#endif

	old_reloj = get_reloj();

//printf("func: %s\n", fname[mem[ip + 1]]);
	switch(v_function = mem[ip++]) {
		case   0: { DIV_signal();             break; }
		case   1: { DIV_key();                break; }
		case   2: { DIV_load_pal();           break; }
		case   3: { DIV_load_fpg();           break; }
		case   4: { DIV_start_scroll();       break; }
		case   5: { DIV_stop_scroll();        break; }
		case   6: { DIV_out_region();         break; }
		case   7: { DIV_graphic_info();       break; }
		case   8: { DIV_collision();          break; }
		case   9: { DIV_get_id();             break; }
		case  10: { DIV_get_distx();          break; }
		case  11: { DIV_get_disty();          break; }
		case  12: { DIV_get_angle();          break; }
		case  13: { DIV_get_dist();           break; }
		case  14: { DIV_fade();               break; }
		case  15: { DIV_load_fnt();           break; }
		case  16: { DIV_write();              break; }
		case  17: { DIV_write_int();          break; }
		case  18: { DIV_delete_text();        break; }
		case  19: { DIV_move_text();          break; }
		case  20: { DIV_unload_fpg();         break; }
		case  21: { DIV_rand();               break; }
		case  22: { DIV_define_region();      break; }
		case  23: { DIV_xput();               break; }
		case  24: { DIV_put();                break; }
		case  25: { DIV_put_screen();         break; }
		case  26: { DIV_map_xput();           break; }
		case  27: { DIV_map_put();            break; }
		case  28: { DIV_put_pixel();          break; }
		case  29: { DIV_get_pixel();          break; }
		case  30: { DIV_map_put_pixel();      break; }
		case  31: { DIV_map_get_pixel();      break; }
		case  32: { DIV_get_point();          break; }
		case  33: { DIV_clear_screen();       break; }
		case  34: { DIV_save();               break; }
		case  35: { DIV_load();               break; }
		case  36: { DIV_set_mode();           break; }
		case  37: { DIV_load_pcm();           break; }
		case  38: { DIV_unload_pcm();         break; }
		case  39: { DIV_sound();              break; }
		case  40: { DIV_stop_sound();         break; }
		case  41: { DIV_change_sound();       break; }
		case  42: { DIV_set_fps();            break; }
		case  43: { DIV_start_fli();          break; }
		case  44: { DIV_frame_fli();          break; }
		case  45: { DIV_end_fli();            break; }
		case  46: { DIV_reset_fli();          break; }
		case  47: { DIV_system();             break; }
		case  48: { DIV_refresh_scroll();     break; }
		case  49: { DIV_fget_dist();          break; }
		case  50: { DIV_fget_angle();         break; }
		case  51: { DIV_play_cd();            break; }
		case  52: { DIV_stop_cd();            break; }
		case  53: { DIV_is_playing_cd();      break; }
		case  54: { DIV_start_mode7();        break; }
		case  55: { DIV_stop_mode7();         break; }
		case  56: { DIV_advance();            break; }
		case  57: { DIV_abs();                break; }
		case  58: { DIV_fade_on();            break; }
		case  59: { DIV_fade_off();           break; }
		case  60: { DIV_rand_seed();          break; }
		case  61: { DIV_sqrt();               break; }
		case  62: { DIV_pow();                break; }
		case  63: { DIV_map_block_copy();     break; }
		case  64: { DIV_move_scroll();        break; }
		case  65: { DIV_near_angle();         break; }
		case  66: { DIV_let_me_alone();       break; }
		case  67: { DIV_exit();               break; }
		case  68: { DIV_roll_palette();       break; }
		case  69: { DIV_get_real_point();     break; }
		case  70: { DIV_get_joy_button();     break; }
		case  71: { DIV_get_joy_position();   break; }
		case  72: { DIV_convert_palette();    break; }
		case  73: { DIV_load_map();           break; }
		case  74: { DIV_reset_sound();        break; }
		case  75: { DIV_unload_map();         break; }
		case  76: { DIV_unload_fnt();         break; }
		case  77: { DIV_set_volume();         break; }
		case  78: { DIV_set_color();          break; }
#ifdef NETPLAY
		case  79: { DIV_net_join_game();      break; }
		case  80: { DIV_net_get_games();      break; }
#endif
#if defined(MODE8) || defined(NEWMODE8)
		case  81: { DIV_stop_mode8();         break; }
#endif
		case  82: { DIV_xadvance();           break; }
		case  83: { DIV_char();               break; }
		case  84: { DIV_path_find();          break; }
		case  85: { DIV_path_line();          break; }
		case  86: { DIV_path_free();          break; }
		case  87: { DIV_new_map();            break; }
#if defined(MODE8) || defined(NEWMODE8)
		case  88: { DIV_load_wld();           break; }
		case  89: { DIV_start_mode8();        break; }
		case  90: { DIV_go_to_flag();         break; }
		case  91: { DIV_set_sector_height();  break; }
		case  92: { DIV_get_sector_height();  break; }
		case  93: { DIV_set_point_m8();       break; }
		case  94: { DIV_get_point_m8();       break; }
		case  95: { DIV_set_fog();            break; }
		case  96: { DIV_set_sector_texture(); break; }
		case  97: { DIV_get_sector_texture(); break; }
		case  98: { DIV_set_wall_texture();   break; }
		case  99: { DIV_get_wall_texture();   break; }
		case 100: { DIV_set_env_color();      break; }
#endif
		case 101: { DIV_strcpy();             break; }
		case 102: { DIV_strcat();             break; }
		case 103: { DIV_strlen();             break; }
		case 104: { DIV_strcmp();             break; }
		case 105: { DIV_strchr();             break; }
		case 106: { DIV_strstr();             break; }
		case 107: { DIV_strset();             break; }
		case 108: { DIV_upper();              break; }
		case 109: { DIV_lower();              break; }
		case 110: { DIV_strdel();             break; }
		case 111: { DIV_screen_copy();        break; }
		case 112: { DIV_qsort();              break; }
		case 113: { DIV_load_song();          break; }
		case 114: { DIV_unload_song();        break; }
		case 115: { DIV_song();               break; }
		case 116: { DIV_stop_song();          break; }
		case 117: { DIV_set_song_pos();       break; }
		case 118: { DIV_get_song_pos();       break; }
		case 119: { DIV_get_song_line();      break; }
		case 120: { DIV_is_playing_sound();   break; }
		case 121: { DIV_is_playing_song();    break; }
		case 122: { DIV_fopen();              break; }
		case 123: { DIV_fclose();             break; }
		case 124: { DIV_fread();              break; }
		case 125: { DIV_fwrite();             break; }
		case 126: { DIV_fseek();              break; }
		case 127: { DIV_ftell();              break; }
		case 128: { DIV_filelength();         break; }
		case 129: { DIV_flush();              break; }
		case 130: { DIV_get_dirinfo();        break; }
		case 131: { DIV_get_fileinfo();       break; }
		case 132: { DIV_getdrive();           break; }
		case 133: { DIV_setdrive();           break; }
		case 134: { DIV_chdir();              break; }
		case 135: { DIV_mkdir();              break; }
		case 136: { DIV_remove();             break; }
		case 137: { DIV_disk_free();          break; }
		case 138: { DIV_memory_free();        break; }
		case 139: { DIV_ignore_error();       break; }
		case 140: { DIV_save_mapcx(1);        break; }
		case 141: { DIV_sin();                break; }
		case 142: { DIV_cos();                break; }
		case 143: { DIV_tan();                break; }
		case 144: { DIV_asin();               break; }
		case 145: { DIV_acos();               break; }
		case 146: { DIV_atan();               break; }
		case 147: { DIV_atan2();              break; }
		case 148: { DIV_draw();               break; }
		case 149: { DIV_delete_draw();        break; }
		case 150: { DIV_move_draw();          break; }
		case 151: { DIV_save_mapcx(0);        break; }
		case 152: { DIV_write_in_map();       break; }
		case 153: { DIV_calculate();          break; }
		case 154: { DIV_itoa();               break; }
		case 155: { DIV_change_channel();     break; }
		case 156: { DIV_malloc();             break; }
		case 157: { DIV_free();               break; }
		case 158: { DIV_encode();             break; }
		case 159: { DIV_encode_file(1);       break; }
		case 160: { DIV_encode_file(0);       break; }
		case 161: { DIV_compress(1);          break; }
		case 162: { DIV_compress(0);          break; }
		case 163: { DIV_find_color();         break; }
		case 164: { DIV_load_screen();        break; }
		case 165: { DIV_force_pal();          break; }
	}

#ifdef DEBUG
	function_exec(v_function, get_ticks() - oticks);
#endif
}

// A�adir una funci�n:
// En ltobj.def, en el case anterior y fname (y doc: add.prg y help)
char *fname[] = {
	"signal",
	"key",
	"load_pal",
	"load_fpg",
	"start_scroll",
	"stop_scroll",
	"out_region",
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
	"load_pcm/wav",
	"unload_pcm/wav",
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
	"load_map/pcx",
	"reset_sound",
	"unload_map/pcx",
	"unload_fnt",
	"set_volume",

	// Nuevas funciones a�adidas para DIV 2.0
	"set_color",
	"net_join_game",
	"net_get_games",
	"stop_mode8",
	"x_advance",
	"char",
	"path_find",
	"path_line",
	"path_free",
	"new_map",
	"load_wld",
	"start_mode8",
	"go_to_flag",
	"set_sector_height",
	"get_sector_height",
	"set_point_m8",
	"get_point_m8",
	"set_fog",
	"set_sector_texture",
	"get_sector_texture",
	"set_wall_texture",
	"get_wall_texture",
	"set_env_color",
	"strcpy",
	"strcat",
	"strlen",
	"strcmp",
	"strchr",
	"strstr",
	"strset",
	"strupr",
	"strlwr",
	"strdel",
	"screen_copy",
	"sort",
	"load_song",
	"unload_song",
	"song",
	"stop_song",
	"set_song_pos",
	"get_song_pos",
	"get_song_line",
	"is_playing_sound",
	"is_playing_song",
	"fopen",
	"fclose",
	"fread",
	"fwrite",
	"fseek",
	"ftell",
	"filelength",
	"flush",
	"get_dirinfo",
	"get_fileinfo",
	"getdrive",
	"setdrive",
	"chdir",
	"mkdir",
	"remove",
	"disk_free",
	"memory_free",
	"ignore_error",
	"save_pcx",
	"sin",
	"cos",
	"tan",
	"asin",
	"acos",
	"atan",
	"atan2",
	"draw",
	"delete_draw",
	"move_draw",
	"save_map",
	"write_in_map",
	"calculate",
	"itoa",
	"change_channel",
	"malloc",
	"free",
	"encode",
	"encode_file",
	"decode_file",
	"compress_file",
	"uncompress_file",
	"find_color",
	"load_screen",
	"force_pal"
};
