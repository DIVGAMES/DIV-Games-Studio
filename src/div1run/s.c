//════════════════════════════════════════════════════════════════════════════
// Funciones de sprites, textos y scroll
//════════════════════════════════════════════════════════════════════════════
#include "inter.h"

//════════════════════════════════════════════════════════════════════════════
// Definicion de constantes
//════════════════════════════════════════════════════════════════════════════
#define grados_0    0
#define grados_30   320
#define grados_45   512
#define grados_90   1024
#define grados_180  2048
#define grados_270  3072
#define grados_360  4096
#define grados_giro 16


// file prototypes
void move_scroll(int plano, int x, int y);
void save_region(void);
void sp_scanc(byte *p, short n, short m, short o, byte *si, int an, int x0, int y0, int x1, int y1);
void sp_scancg(byte *p, short n, short m, short o, byte *si, int an, int x0, int y0, int x1, int y1);
void sp_scang(byte *p, short n, byte *si, int an, int x0, int y0, int x1, int y1);
void texc(byte *p, int x, int y, byte an, int al);
void texn(byte *p, int x, int y, byte an, int al);
void pinta_modo7(int n, int camara_x, int camara_y, int camara_z, int angulo);
void pinta_sprite_m7(int n, int ide, int x, int y, int size, int ang);


//════════════════════════════════════════════════════════════════════════════
// Variables del módulo
//════════════════════════════════════════════════════════════════════════════
int seno[grados_360 + 1], coseno[grados_360 + 1]; //m7

static unsigned int mul_16(int a, int b);
static int          mul_24(int a, int b);

//════════════════════════════════════════════════════════════════════════════
// Tabla para fast scroll parallax
//════════════════════════════════════════════════════════════════════════════
t_fast *fast;

//════════════════════════════════════════════════════════════════════════════
// Función para pintar los sprites de un scroll ordenados por Z
//════════════════════════════════════════════════════════════════════════════
void pinta_sprites_scroll(void) {
	int old_ide = ide, max;

	if(post_process_scroll != NULL) {
		post_process_scroll();
	}

	// Marca todos los procesos como no pintados
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		mem[ide + _Painted] = 0;
	}

	do {
		ide = 0;
		max = 0x80000000;
		for(id = id_start; id <= id_end; id += iloc_len) {
			if((mem[id + _Status ] == 2            ||
			    mem[id + _Status ] == 4)           &&
			    mem[id + _Ctype  ] == 1            &&
			   (mem[id + _Cnumber] == 0            ||
			   (mem[id + _Cnumber] & (1 << snum))) &&
			   !mem[id + _Painted] &&
			    mem[id + _Z] > max) {
				ide = id;
				max = mem[id + _Z];
			}
		}
		if(ide) {
			pinta_sprite();

			mem[ide + _Painted] = 1;
		}
	}while(ide);

	ide = old_ide;
}

//════════════════════════════════════════════════════════════════════════════
// Determina el movimiento de la ventana de scroll (automático o manual)
//════════════════════════════════════════════════════════════════════════════
void mover_scroll(int back) {
	int i, x, y, ix, iy, r;

	// Comprueba si el scroll es autómático
	if(i = (scroll + snum)->camera) {
		if(i >= id_start && i <= id_end && mem[i] == i) {
			ix = mem[i + _X];
			iy = mem[i + _Y];
			if(mem[i + _Resolution]) {
				ix /= mem[i + _Resolution];
				iy /= mem[i + _Resolution];
			}

			x = ix + iscroll[snum].x-iscroll[snum].map1_x;
			y = iy + iscroll[snum].y-iscroll[snum].map1_y;

			r = (scroll + snum)->region1;
			if(r >= 0 && r < max_region) {
				ix = 0;
				iy = 0;
				if(x <  region[r].x0) { ix = x -  region[r].x0;      }
				if(y <  region[r].y0) { iy = y -  region[r].y0;      }
				if(x >= region[r].x1) { ix = x - (region[r].x1 - 1); }
				if(y >= region[r].y1) { iy = y - (region[r].y1 - 1); }
			}
			// No hay region1 de bloqueo de scroll, se tiende al centro
			else {
				ix -= iscroll[snum].an / 2;
				iy -= iscroll[snum].al / 2;
				ix -= iscroll[snum].map1_x;
				iy -= iscroll[snum].map1_y;
			}

			r = (scroll + snum)->region2;
			if(r >= 0 && r < max_region) {
				if(x < region[r].x0 || x >= region[r].x1) {
					if(x < region[r].x0) { ix = x -  region[r].x0;      }
					else                 { ix = x - (region[r].x1 - 1); }
				}
				else if((scroll + snum)->speed > 0) {
					if     (ix >  (scroll +snum)->speed) { ix =  (scroll + snum)->speed; }
					else if(ix < -(scroll +snum)->speed) { ix = -(scroll + snum)->speed; }
				}
				if(y < region[r].y0 || y >= region[r].y1) {
					if(y < region[r].y0) { iy = y -  region[r].y0;      }
					else                 { iy = y - (region[r].y1 - 1); }
				}
				else if((scroll + snum)->speed > 0) {
					if     (iy >  (scroll + snum)->speed) { iy =  (scroll + snum)->speed; }
					else if(iy < -(scroll + snum)->speed) { iy = -(scroll + snum)->speed; }
				}
			}

			x = iscroll[snum].map1_x;
			y = iscroll[snum].map1_y;
			move_scroll(0,
			            ix,
			            iy);

			if(back) {
				if((scroll + snum)->ratio) {
					ix = iscroll[snum].map1_x - x;
					iy = iscroll[snum].map1_y - y;
					move_scroll(1,
					            ix * 100 / (scroll + snum)->ratio,
					            iy * 100 / (scroll + snum)->ratio);
				}
			}
		}
	}
	// Comprueba si se mueve el scroll manualmente
	else {
		if((scroll + snum)->x0 != iscroll[snum].map1_x ||
		   (scroll + snum)->y0 != iscroll[snum].map1_y) {
				move_scroll(0,
				           (scroll + snum)->x0 - iscroll[snum].map1_x,
				           (scroll + snum)->y0 - iscroll[snum].map1_y);
		}
		if(back) {
			if((scroll + snum)->x1 != iscroll[snum].map2_x ||
			   (scroll + snum)->y1 != iscroll[snum].map2_y) {
				move_scroll(1,
				           (scroll + snum)->x1 - iscroll[snum].map2_x,
				           (scroll + snum)->y1 - iscroll[snum].map2_y);
			}
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Scroll simple
//════════════════════════════════════════════════════════════════════════════
void scroll_simple(void) {
	int n, b;
	byte *di, *si;

	mover_scroll(0);

	b  = iscroll[snum].block1;
	si = iscroll[snum].scr1;
	di = copia + iscroll[snum].x + iscroll[snum].y * vga_an;

	for(n = 0; n < iscroll[snum].al; n++) {
		memcpy(di,
		       si,
		       iscroll[snum].an);
		di += vga_an;

		if(--b == 0) {
			si -= iscroll[snum].an * (iscroll[snum].al - 1);
		}
		else {
			si += iscroll[snum].an;
		}
	}

	pinta_sprites_scroll();
}

//════════════════════════════════════════════════════════════════════════════
// Scroll parallax
//════════════════════════════════════════════════════════════════════════════
void scroll_parallax(void) {
	int n, an, b1, b2;
	int nt, scan;
	byte *di, *si1, *si2;

	mover_scroll(1); // Mueve los dos planos de scroll (auto o manualmente)

	b1   = iscroll[snum].block1;
	b2   = iscroll[snum].block2;
	fast = iscroll[snum].fast;
	scan = iscroll[snum].iscan;
	si1  = iscroll[snum].scr1;
	si2  = iscroll[snum].scr2;
	di   = copia + iscroll[snum].x + iscroll[snum].y * vga_an;

	for(n = 0; n < iscroll[snum].al; n++) {
		if(fast[scan].nt >= max_inc) {
			an = iscroll[snum].an;
			do {
				if(*si1) { *di = *si1; }
				else     { *di = *si2; }
				si1++;
				si2++;
				di++;
			}while(--an);
		}
		else {
			nt = 0;
			do {
				if(fast[scan].inc[nt]) {
					if(nt & 1) { memcpy(di, si1, fast[scan].inc[nt]); }
					else       { memcpy(di, si2, fast[scan].inc[nt]); }
					si1 += fast[scan].inc[nt];
					si2 += fast[scan].inc[nt];
					di  += fast[scan].inc[nt];
				}
			}while(nt++ < fast[scan].nt);
		}

		if(++scan == iscroll[snum].al) {
			scan=0;
		}

		if(--b1 == 0) { si1 -= iscroll[snum].an * iscroll[snum].al; }
		if(--b2 == 0) { si2 -= iscroll[snum].an * iscroll[snum].al; }
		di += vga_an - iscroll[snum].an;
	}

	pinta_sprites_scroll();
}

//════════════════════════════════════════════════════════════════════════════
// set_scroll(plano, x, y) - snum
//════════════════════════════════════════════════════════════════════════════
void set_scroll(int plano, int x, int y) {
	int an, al, iscan;
	int _nx, nx, ny;
	byte *di, *si, *_si;
	int kk, nt, esdato;

	if(snum < 0 || snum > 9) {
		return;
	}

	if(plano < 0 || plano >= iscroll[snum].on) {
		return;
	}

	if(plano == 0) {
		if(!(iscroll[snum].map_flags & 1)) {
			if(x < 0) {
				x = 0;
			}
			else if(x + iscroll[snum].an >= iscroll[snum].map1_an) {
				x = iscroll[snum].map1_an - iscroll[snum].an;
			}
		}
		if(!(iscroll[snum].map_flags & 2)) {
			if(y < 0) {
				y = 0;
			}
			else if(y + iscroll[snum].al >= iscroll[snum].map1_al) {
				y = iscroll[snum].map1_al - iscroll[snum].al;
			}
		}

		iscroll[snum].map1_x = x;
		iscroll[snum].map1_y = y;
		iscroll[snum].block1 = iscroll[snum].al;

		an = iscroll[snum].map1_an;
		al = iscroll[snum].map1_al;
		si = iscroll[snum].map1;
		di = iscroll[snum].scr1 = iscroll[snum]._scr1;
	}
	else {
		if(!(iscroll[snum].map_flags & 4)) {
			if(x < 0) {
				x = 0;
			}
			else if(x + iscroll[snum].an >= iscroll[snum].map2_an) {
				x = iscroll[snum].map2_an - iscroll[snum].an;
			}
		}
		if(!(iscroll[snum].map_flags & 8)) {
			if(y < 0) {
				y = 0;
			}
			else if(y + iscroll[snum].al >= iscroll[snum].map2_al) {
				y = iscroll[snum].map2_al - iscroll[snum].al;
			}
		}

		iscroll[snum].map2_x = x;
		iscroll[snum].map2_y = y;
		iscroll[snum].block2 = iscroll[snum].al;

		an = iscroll[snum].map2_an;
		al = iscroll[snum].map2_al;
		si = iscroll[snum].map2;
		di = iscroll[snum].scr2 = iscroll[snum]._scr2;
	}

	if(x < 0) { x = (an - 1) - ((-x - 1) % an); } else { x = x % an; }
	if(y < 0) { y = (al - 1) - ((-y - 1) % al); } else { y = y % al; }

	si += y * an + x;

	// Dispone de (di,iscroll[snum].an/al) < (si,x,y,an,al)

// Se ha quitado esta optimización
//  if (x+iscroll[snum].an<=an && y+iscroll[snum].al<=al) {
//    y=iscroll[snum].al; do {
//      memcpy(di,si,iscroll[snum].an);
//      di+=iscroll[snum].an; si+=an;
//    } while (--y);
//  } else {

	if(iscroll[snum].on == 2 && plano == 0) {
		iscan = 0;
		fast  = iscroll[snum].fast;
		_nx   = an - x;
		ny    = al - y;
		y     = iscroll[snum].al;
		do {
			esdato             = 0;

			_si                =  si;
			nx                 = _nx;
			nt                 = 0; // número de tramos
			fast[iscan].inc[0] = 0;
			x                  = iscroll[snum].an;
			do {
				if(kk = *si++) {
					if(esdato) {
						if(nt < max_inc) { fast[iscan].inc[nt]++;   }
					}
					else {
						esdato = 1;

						nt++;
						if(nt < max_inc) { fast[iscan].inc[nt] = 1; }
					}
				}
				else {
					if(esdato) {
						esdato = 0;

						nt++;
						if(nt < max_inc) { fast[iscan].inc[nt] = 1; }
					}
					else {
						if(nt < max_inc) { fast[iscan].inc[nt]++;   }
					}
				}

				if(--nx == 0) {
					si -= an;
					nx  = an;
				}
				*di++ = kk;
			}while(--x);

			iscan++;
			fast[iscan].nt = nt;
			si             = _si + an;
			if(--ny == 0) {
				si -= an * al;
				ny  = al;
			}
		}while(--y);

		iscroll[snum].iscan = 0;
	}
	else {
		_nx = an - x;
		ny  = al - y;
		y   = iscroll[snum].al;
		do {
			_si = si;
			nx  = _nx;
			x   = iscroll[snum].an;
			do {
				kk = *si++;
				if(--nx == 0) {
					si -= an;
					nx  = an;
				}
				*di++ = kk;
			}while(--x);

			si = _si + an;
			if(--ny == 0) {
				si -= an * al;
				ny  = al;
			}
		}while(--y);
	}

	(scroll + snum)->x0 = iscroll[snum].map1_x;
	(scroll + snum)->y0 = iscroll[snum].map1_y;
	(scroll + snum)->x1 = iscroll[snum].map2_x;
	(scroll + snum)->y1 = iscroll[snum].map2_y;
}

//════════════════════════════════════════════════════════════════════════════
// Recalcula la ristra de incrementos para un scan (0..vga_al-1)
//════════════════════════════════════════════════════════════════════════════
void process_scan(int n) { // Reprocesa un scan por completo
	int in; // indice de fast[in].{nt,inc[]}
	byte *si;
	int resta, an;
	int nt = 0, esdato = 0;

	if((in = iscroll[snum].iscan + n) >= iscroll[snum].al) {
		in -= iscroll[snum].al;
	}
	if(n >= iscroll[snum].block1) {
		n -= iscroll[snum].al;
	}

	fast[in].inc[0] = 0;
	fast            = iscroll[snum].fast;
	resta           = iscroll[snum].map1_an;
	si              = iscroll[snum].scr1 + n * iscroll[snum].an;
	an              = iscroll[snum].an;
	do {
		if(*si++) {
			if(esdato) {
				if(nt < max_inc) { fast[in].inc[nt]++;   }
			}
			else {
				esdato = 1;

				nt++;
				if(nt < max_inc) { fast[in].inc[nt] = 1; }
			}
		}
		else {
			if(esdato) {
				esdato = 0;

				nt++;
				if(nt < max_inc) { fast[in].inc[nt] = 1; }
			}
			else {
				if(nt < max_inc) { fast[in].inc[nt]++;   }
			}
		}

		if(--resta == 0) {
			resta  = iscroll[snum].map1_an;
			si    -= iscroll[snum].map1_an;
		}
	}while(--an);

	fast[in].nt = nt;
}

//════════════════════════════════════════════════════════════════════════════
// Entra un pixel en todas las líneas por la derecha (sale por la izda)
//════════════════════════════════════════════════════════════════════════════
void process_scan_right(void) {
	int n, in, block;
	byte *si;

	fast  = iscroll[snum].fast;
	block = iscroll[snum].block1;
	in    = iscroll[snum].iscan;
	si    = iscroll[snum].scr1 + iscroll[snum].an - 1; // Final de la primera línea
	n     = iscroll[snum].al;
	do {
		// 1º quita el pixel por la izquierda ...

		// Si empieza con fondo
		if(fast[in].inc[0]) {
			fast[in].inc[0]--;
		}
		else {
			// Si empieza con primer plano
			if (fast[in].inc[1]) {
				fast[in].inc[1]--;
			}
			// Se acabaron un par de tramos
			if (!fast[in].inc[1]) {
				fast[in].nt -= 2;

				memmove(&fast[in].inc[0],
				        &fast[in].inc[2],
				        (max_inc - 2) * 2);

				if(fast[in].nt < max_inc && fast[in].nt + 2 >= max_inc) {
					process_scan(iscroll[snum].al-n);

					goto cont_loop;
				}
			}
		}

		// 2º añade el pixel por la derecha ...

		// Si viene primer plano
		if(*si) {
			// Si el tramo acaba en primer plano
			if(fast[in].nt & 1) {
				if(fast[in].nt < max_inc) {
					fast[in].inc[fast[in].nt]++;
				}
			}
			// Si acaba en fondo
			else {
				if(++fast[in].nt < max_inc) {
					fast[in].inc[fast[in].nt] = 1;
				}
			}
		}
		// Si viene fondo
		else {
			// Si el tramo acaba en primer plano
			if (fast[in].nt&1) {
				if(++fast[in].nt < max_inc) {
					fast[in].inc[fast[in].nt] = 1;
				}
			}
			// Si acaba en fondo
			else {
				if(fast[in].nt < max_inc) {
					fast[in].inc[fast[in].nt]++;
				}
			}
		}

cont_loop:
		if(!--block) {
			si -= iscroll[snum].an * (iscroll[snum].al - 1);
		}
		else {
			si += iscroll[snum].an;
		}

		if(++in == iscroll[snum].al) {
			in = 0;
		}
	}while(--n);
}

//════════════════════════════════════════════════════════════════════════════
// Entra un pixel en todas las líneas por la izquierda (sale por la dcha)
//════════════════════════════════════════════════════════════════════════════
void process_scan_left(void) {
	int n, in, block;
	byte *si;

	block = iscroll[snum].block1;
	in    = iscroll[snum].iscan;
	fast  = iscroll[snum].fast;
	si    = iscroll[snum].scr1 + iscroll[snum].an - 1; // Final de la primera línea
	n     = iscroll[snum].al;
	do {
		// 1º quita el pixel por la derecha ...

		if(fast[in].nt < max_inc) {
			if(!--fast[in].inc[fast[in].nt]) {
				fast[in].nt--;
			}
		}
		// Caso complicado
		else {
			// Si la línea acababa en primer plano ...
			if(fast[in].nt & 1) {
				// ... y ahora acaba en fondo
				if(!*si) {
					if (--fast[in].nt == max_inc - 1) {
						process_scan(iscroll[snum].al - n);

						goto cont_loop;
					}
				}
			}
			// Si la línea acababa en fondo ...
			else {
				// ... y ahora acaba en primer plano
				if(*si) {
					if (--fast[in].nt == max_inc - 1) {
						process_scan(iscroll[snum].al - n);

						goto cont_loop;
					}
				}
			}
		}

		// 2º añade el pixel por la izquierda ...

		// Si viene primer plano
		if(*(si - iscroll[snum].an + 1)) {
			// Si el tramo comienza en primer plano
			if(!fast[in].inc[0]) {
				fast[in].inc[1]++;
			}
			// Si comienza con fondo
			else {
				fast[in].nt += 2;
				if(fast[in].nt < max_inc) {
					memmove(&fast[in].inc[2],
					        &fast[in].inc[0],
					        (max_inc - 2) * 2);

					fast[in].inc[0] = 0;
					fast[in].inc[1] = 1;
				}
			}
		}
		// Si viene fondo
		else {
			fast[in].inc[0]++;
		}

cont_loop:
		if(!--block) {
			si -= iscroll[snum].an * (iscroll[snum].al - 1);
		}
		else {
			si += iscroll[snum].an;
		}

		if(++in == iscroll[snum].al) {
			in = 0;
		}
	}while(--n);
}

//════════════════════════════════════════════════════════════════════════════
// move_scroll(snum, plano, ix, iy)
//════════════════════════════════════════════════════════════════════════════
byte *scroll_right(byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y);
byte *scroll_left (byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y);
byte *scroll_down (byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y);
byte *scroll_up   (byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y);

void move_scroll(int plano, int x, int y) {
	int ix, iy;

	if(snum < 0 || snum > 9) {
		return;
	}

	if(plano < 0 || plano >= iscroll[snum].on) {
		return;
	}

	if(plano == 0) {
		ix  = iscroll[snum].map1_x;
		iy  = iscroll[snum].map1_y;
		x  += iscroll[snum].map1_x;
		y  += iscroll[snum].map1_y;
		if(!(iscroll[snum].map_flags & 1)) {
			if(x < 0) {
				x = 0;
			}
			else if(x + iscroll[snum].an >= iscroll[snum].map1_an) {
				x = iscroll[snum].map1_an - iscroll[snum].an;
			}
		}
		if(!(iscroll[snum].map_flags & 2)) {
			if(y < 0) {
				y = 0;
			}
			else if(y + iscroll[snum].al >= iscroll[snum].map1_al) {
				y = iscroll[snum].map1_al - iscroll[snum].al;
			}
		}
	}
	else {
		ix  = iscroll[snum].map2_x;
		iy  = iscroll[snum].map2_y;
		x  += iscroll[snum].map2_x;
		y  += iscroll[snum].map2_y;
		if(!(iscroll[snum].map_flags & 4)) {
			if(x < 0) {
				x = 0;
			}
			else if(x + iscroll[snum].an >= iscroll[snum].map2_an) {
				x = iscroll[snum].map2_an-iscroll[snum].an;
			}
		}
		if(!(iscroll[snum].map_flags & 8)) {
			if(y < 0) {
				y = 0;
			}
			else if(y + iscroll[snum].al >= iscroll[snum].map2_al) {
				y = iscroll[snum].map2_al - iscroll[snum].al;
			}
		}
	}

	ix = x - ix;
	iy = y - iy;

	/////////////////////////////////////////////////////////////////////
	if(abs(ix) > iscroll[snum].an / 2 ||
	   abs(iy) > iscroll[snum].al / 2) {
		if(plano == 0) {
			ix += iscroll[snum].map1_x;
			iy += iscroll[snum].map1_y;
		}
		else {
			ix += iscroll[snum].map2_x;
			iy += iscroll[snum].map2_y;
		}

		set_scroll(plano, ix, iy);

		return;
	}
	/////////////////////////////////////////////////////////////////////

	if(plano == 0) {
		iscroll[snum].map1_x = x;
		iscroll[snum].map1_y = y;
	}
	else {
		iscroll[snum].map2_x = x;
		iscroll[snum].map2_y = y;
	}

	x -= ix;
	y -= iy;

	if(ix > 0) {
		do {
			if(plano == 0) {
				iscroll[snum].scr1 = scroll_right(iscroll[snum]._scr1,
				                                  iscroll[snum].scr1,
				                                 &iscroll[snum].block1,
				                                  iscroll[snum].map1,
				                                  iscroll[snum].map1_an,
				                                  iscroll[snum].map1_al,
				                                  x++,
				                                  y);

				if(iscroll[snum].on == 2) {
					process_scan_right(); // Entra un scan por la derecha (sale por la izda)
				}
			}
			else {
				iscroll[snum].scr2 = scroll_right(iscroll[snum]._scr2,
				                                  iscroll[snum].scr2,
				                                 &iscroll[snum].block2,
				                                  iscroll[snum].map2,
				                                  iscroll[snum].map2_an,
				                                  iscroll[snum].map2_al,
				                                  x++,
				                                  y);
			}
		}while(--ix);
	}
	if(ix < 0) {
		do {
			if(plano == 0) {
				iscroll[snum].scr1 = scroll_left(iscroll[snum]._scr1,
				                                 iscroll[snum].scr1,
				                                &iscroll[snum].block1,
				                                 iscroll[snum].map1,
				                                 iscroll[snum].map1_an,
				                                 iscroll[snum].map1_al,
				                                 x--,
				                                 y);

				if(iscroll[snum].on == 2) {
					process_scan_left(); // Entra un scan por la izda (sale por la dcha)
				}
			}
			else {
				iscroll[snum].scr2 = scroll_left(iscroll[snum]._scr2,
				                                 iscroll[snum].scr2,
				                                &iscroll[snum].block2,
				                                 iscroll[snum].map2,
				                                 iscroll[snum].map2_an,
				                                 iscroll[snum].map2_al,
				                                 x--,
				                                 y);
			}
		}while(++ix);
	}

	if(iy > 0) {
		do {
			if(plano == 0) {
				iscroll[snum].scr1 = scroll_down(iscroll[snum]._scr1,
				                                 iscroll[snum].scr1,
				                                &iscroll[snum].block1,
				                                 iscroll[snum].map1,
				                                 iscroll[snum].map1_an,
				                                 iscroll[snum].map1_al,
				                                 x,
				                                 y++);

				if(iscroll[snum].on == 2) {
					if(++iscroll[snum].iscan == iscroll[snum].al) {
						iscroll[snum].iscan = 0;
					}

					process_scan(iscroll[snum].al-1);
				}
			}
			else {
				iscroll[snum].scr2 = scroll_down(iscroll[snum]._scr2,
				                                 iscroll[snum].scr2,
				                                &iscroll[snum].block2,
				                                 iscroll[snum].map2,
				                                 iscroll[snum].map2_an,
				                                 iscroll[snum].map2_al,
				                                 x,
				                                 y++);
			}
		}while(--iy);
	}
	if(iy < 0) {
		do {
			if(plano == 0) {
				iscroll[snum].scr1 = scroll_up(iscroll[snum]._scr1,
				                               iscroll[snum].scr1,
				                              &iscroll[snum].block1,
				                               iscroll[snum].map1,
				                               iscroll[snum].map1_an,
				                               iscroll[snum].map1_al,
				                               x,
				                               y--);

				if(iscroll[snum].on == 2) {
					if(iscroll[snum].iscan-- == 0) {
						iscroll[snum].iscan = iscroll[snum].al - 1;
					}

					process_scan(0);
				}
			}
			else {
				iscroll[snum].scr2 = scroll_up(iscroll[snum]._scr2,
				                               iscroll[snum].scr2,
				                              &iscroll[snum].block2,
				                               iscroll[snum].map2,
				                               iscroll[snum].map2_an,
				                               iscroll[snum].map2_al,
				                               x,
				                               y--);
			}
		}while(++iy);
	}

	if(plano) {
		(scroll + snum)->x1 = iscroll[snum].map2_x;
		(scroll + snum)->y1 = iscroll[snum].map2_y;
	}
	else {
		(scroll + snum)->x0 = iscroll[snum].map1_x;
		(scroll + snum)->y0 = iscroll[snum].map1_y;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Funciones de scroll unidireccional
//════════════════════════════════════════════════════════════════════════════
byte *scroll_right(byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y) {
	// Ventana de scroll (_buf,iscroll[snum].an,iscroll[snum].al(+1))
	// Número de scanes del primer bloque (*block)
	// Puntero al inicio actual del scroll (buf)
	// Mapa en (map,an,al) coordenadas inicio scroll (x,y)

	int n, m, b;
	byte *ret;

	x += iscroll[snum].an;
	if(x < 0) { x = (an - 1) - ((-x - 1) % an); } else { x = x % an; }
	if(y < 0) { y = (al - 1) - ((-y - 1) % al); } else { y = y % al; }

	buf += 1;
	if(buf == _buf + iscroll[snum].an * iscroll[snum].al) {
		memcpy(_buf,
		       _buf + iscroll[snum].an * iscroll[snum].al,
		       iscroll[snum].an);
		buf = _buf;

		block = iscroll[snum].al;
	}
	else if(((int)(buf - _buf) % iscroll[snum].an) == 0) {
		memcpy(_buf,
		       _buf + iscroll[snum].an * iscroll[snum].al,
		       iscroll[snum].an);

		(*block)--;
	}

	ret = buf;

	map += y * an + x; // 1er. pixel en el mapa
	buf += iscroll[snum].an  -1;
	b    = *block;
	m    = al - y;
	n    = iscroll[snum].al;
	do {
		*buf = *map;

		if(--b == 0) {
			buf -= iscroll[snum].an * (iscroll[snum].al - 1);
		}
		else {
			buf += iscroll[snum].an;
		}
		if(--m == 0) {
			map -= an * (al - 1);
			m    = al;
		}
		else {
			map += an;
		}
	}while(--n);

	return (ret);
}

byte *scroll_left(byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y) {
	int n, m, b;
	byte *ret;

	x--;
	if(x < 0) { x = (an - 1) - ((-x - 1) % an); } else { x = x % an; }
	if(y < 0) { y = (al - 1) - ((-y - 1) % al); } else { y = y % al; }

	if(buf == _buf) {
		memcpy(_buf + iscroll[snum].an * iscroll[snum].al,
		       _buf,
		       iscroll[snum].an);
		buf = _buf + iscroll[snum].an * iscroll[snum].al - 1;

		*block = 1;
	}
	else if(((int)(buf - _buf) % iscroll[snum].an) == 0) {
		buf--;
		memcpy(_buf + iscroll[snum].an * iscroll[snum].al,
		       _buf,
		       iscroll[snum].an);

		(*block)++;
	}
	else {
		buf--;
	}

	ret = buf;

	map += y * an + x; // 1er. pixel en el mapa
	b    = *block;
	m    = al - y;
	n    = iscroll[snum].al;
	do {
		*buf = *map;

		if(--b == 0) {
			buf -= iscroll[snum].an * (iscroll[snum].al - 1); 
		}
		else {
			buf += iscroll[snum].an;
		}
		if(--m == 0) {
			map -= an * (al - 1);
			m    = al;
		}
		else {
			map += an;
		}
	}while(--n);

	return (ret);
}

byte *scroll_down(byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y) {
	int n, m;
	byte *ret;

	y += iscroll[snum].al;
	if(x < 0) { x = (an - 1) - ((-x - 1) % an); } else { x = x % an; }
	if(y < 0) { y = (al - 1) - ((-y - 1) % al); } else { y = y % al; }

	buf += iscroll[snum].an;
	if(buf >= _buf + iscroll[snum].an * iscroll[snum].al) {
		buf -= iscroll[snum].an * iscroll[snum].al;

		*block = iscroll[snum].al;
	}
	else {
		(*block)--;
	}

	ret = buf;

	map += y * an + x; // 1er. pixel en el mapa
	buf += (iscroll[snum].al - 1) * iscroll[snum].an;
	if(buf >= _buf + iscroll[snum].an * iscroll[snum].al) {
		buf -= iscroll[snum].an * iscroll[snum].al;
	}
	m   = an - x;
	n   = iscroll[snum].an;
	do {
		*buf++ = *map;

		if(--m == 0) {
			map -= an - 1;
			m    = an;
		}
		else {
			map++;
		}
	}while(--n);

	return (ret);
}

byte *scroll_up(byte *_buf, byte *buf, int *block, byte *map, int an, int al, int x, int y) {
	int n, m;
	byte *ret;

	y--;
	if(x < 0) { x = (an - 1) - ((-x - 1) % an); } else { x = x % an; }
	if(y < 0) { y = (al - 1) - ((-y - 1) % al); } else { y = y % al; }

	buf -= iscroll[snum].an;
	if(buf < _buf) {
		buf += iscroll[snum].an * iscroll[snum].al;

		*block = 1;
	}
	else {
		(*block)++;
	}

	ret = buf;

	map += y * an + x; // 1er. pixel en el mapa
	m    = an - x; 
	n    = iscroll[snum].an;
	do {
		*buf++ = *map;

		if(--m == 0) {
			map -= an - 1;
			m    = an;
		}
		else {
			map++;
		}
	}while(--n);

	return (ret);
}

//════════════════════════════════════════════════════════════════════════════
// Funcion externa para pintar gráficos (para el put del lenguaje)
//════════════════════════════════════════════════════════════════════════════
void put_sprite(int file, int graph, int x, int y, int angle, int size, int flags, int reg, byte *cop, int copan, int copal) {
	int *ptr;
	byte *si;
	int an, al; // Información respecto a pantalla del grafico
	int xg, yg;
	byte *_copia = copia;
	int _vga_an = vga_an, _vga_al = vga_al;

	if(file > max_fpgs || file < 0) {
		e(e109);
		return;
	}
	if(file) { max_grf = 1000; } else { max_grf = 2000; }
	if(graph <= 0 || graph >= max_grf) {
		e(e110);
		return;
	}
	if(g[file].grf == NULL) {
		e(e111);
		return;
	}

	if(reg < 0 || reg >= max_region) {
		clipx0 = 0;
		clipx1 = copan;
		clipy0 = 0;
		clipy1 = copal;
	}
	else {
		clipx0 = region[reg].x0;
		clipx1 = region[reg].x1;
		clipy0 = region[reg].y0;
		clipy1 = region[reg].y1;

		if(clipx0 <     0) { clipx0 = 0;     }
		if(clipx1 > copan) { clipx1 = copan; }
		if(clipx0 >= copan || clipx1 <= 0) {
			return;
		}

		if(clipy0 <     0) { clipy0 = 0;     }
		if(clipy1 > copal) { clipy1 = copal; }
		if(clipy0 >= copal || clipy1 <= 0) {
			return;
		}
	}

	if((ptr = g[file].grf[graph]) != NULL) {
		copia  = cop;
		vga_an = copan;
		vga_al = copal;

		an = ptr[13];
		al = ptr[14];
		si = (byte *)ptr + 64 + ptr[15] * 4;

		if(ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] / 2;
		}
		else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		if(angle) {
			sp_rotado(si,
			          x,
			          y,
			          an,
			          al,
			          xg,
			          yg,
			          angle,
			          size,
			          flags);
		}
		else if(size != 100) {
			sp_escalado(si,
			            x,
			            y,
			            an,
			            al,
			            xg,
			            yg,
			            size,
			            flags);
		}
		else {
			if(flags & 1) { xg  = an - 1 - xg; }
			if(flags & 2) { yg  = al - 1 - yg; }

			x -= xg;
			y -= yg;

			// Pinta sprite sin cortar
			if(x >= clipx0 && x + an <= clipx1 && y >= clipy0 && y + al <= clipy1) {
				sp_normal(si,
				          x,
				          y,
				          an,
				          al,
				          flags);
			}
			// Pinta sprite cortado
			else if(x < clipx1 && y < clipy1 && x + an > clipx0 && y + al > clipy0) {
				sp_cortado(si,
				           x,
				           y,
				           an,
				           al,
				           flags);
			}

			x0s = x;
			x1s = x + an - 1;
			y0s = y;
			y1s = y + al - 1;
		}

		copia  = _copia;
		vga_an = _vga_an;
		vga_al = _vga_al;
	}
	else {
		e(e121);
	}
}

//════════════════════════════════════════════════════════════════════════════
// La función que pinta los sprites de los procesos
//════════════════════════════════════════════════════════════════════════════
void pinta_sprite(void) { // Pinta un sprite (si se ve), según mem[ide + ... ]
	int *ptr;
	byte *si;
	int x, y, an, al; // Información respecto a pantalla del grafico
	int xg, yg;
	int n, m;

	if(mem[ide + _File] > max_fpgs || mem[ide + _File] < 0) {
		return;
	}

	if((n = mem[ide + _XGraph]) > 0) {
		m = mem[n];
		if(m < 1 || m > 256) {
			return;
		}

		while(mem[ide + _Angle] >= 2 * pi) { mem[ide + _Angle] -= 2 * pi; }
		while(mem[ide + _Angle] <       0) { mem[ide + _Angle] += 2 * pi; }
		mem[ide + _Flags] &= 254;
		mem[ide + _Graph] = ((mem[ide + _Angle] + (2 * pi) / (m * 2)) * m) / (2 * pi);
		if(mem[ide + _Graph] >= m) {
			mem[ide + _Graph] = 0;
		}
		if((mem[ide + _Graph] = mem[n + 1 + mem[ide + _Graph]]) < 0) {
			mem[ide + _Graph] = -mem[ide + _Graph];
			mem[ide + _Flags] |= 1;
		}
	}
	if(mem[ide + _File]) { max_grf = 1000; } else { max_grf = 2000; }
	if(mem[ide + _Graph] <= 0 || mem[ide + _Graph] >= max_grf) {
		return;
	}
	if(g[mem[ide + _File]].grf == NULL) {
		return;
	}

	x = mem[ide + _Region];

	if(mem[ide + _Ctype] == 1 && iscroll[snum].on > 0) {
		clipx0 = iscroll[snum].x;
		clipx1 = iscroll[snum].x + iscroll[snum].an;
		clipy0 = iscroll[snum].y;
		clipy1 = iscroll[snum].y + iscroll[snum].al;
	}
	else if(x < 0 || x >= max_region) {
		clipx0 = 0;
		clipx1 = vga_an;
		clipy0 = 0;
		clipy1 = vga_al;
	}
	else {
		clipx0 = region[x].x0;
		clipx1 = region[x].x1;
		clipy0 = region[x].y0;
		clipy1 = region[x].y1;
	}

	if((ptr = g[mem[ide + _File]].grf[mem[ide + _Graph]]) != NULL) {
		x = mem[ide + _X];
		y = mem[ide + _Y];
		if(mem[ide + _Resolution] > 0) {
			x /= mem[ide + _Resolution];
			y /= mem[ide + _Resolution];
		}

		if(mem[ide + _Ctype] == 1) {
			x += iscroll[snum].x - iscroll[snum].map1_x;
			y += iscroll[snum].y - iscroll[snum].map1_y;
		}

		an = ptr[13];
		al = ptr[14];
		si = (byte *)ptr + 64 + ptr[15] * 4;

		if(ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] / 2;
		}
		else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		if(putsprite != NULL) {
			putsprite(si,
			          x,
			          y,
			          an,
			          al,
			          xg,
			          yg,
			          mem[ide + _Angle],
			          mem[ide + _Size],
			          mem[ide + _Flags]);
		}
		else if(mem[ide + _Angle] && mem[ide + _XGraph] <= 0) {
			sp_rotado(si,
			          x,
			          y,
			          an,
			          al,
			          xg,
			          yg,
			          mem[ide + _Angle],
			          mem[ide + _Size],
			          mem[ide + _Flags]);

			save_region();
		}
		else if(mem[ide + _Size] != 100) {
			sp_escalado(si,
			            x,
			            y,
			            an,
			            al,
			            xg,
			            yg,
			            mem[ide + _Size],
			            mem[ide + _Flags]);

			save_region();
		}
		else {
			if(mem[ide + _Flags] & 1) { xg  = an - 1 - xg; }
			if(mem[ide + _Flags] & 2) { yg  = al - 1 - yg; }

			x -= xg;
			y -= yg;

			// Pinta sprite sin cortar
			if(x >= clipx0 && x + an <= clipx1 && y >= clipy0 && y + al <= clipy1) {
				sp_normal(si,
				          x,
				          y,
				          an,
				          al,
				          mem[ide + _Flags]);
			}
			// Pinta sprite cortado
			else if(x < clipx1 && y < clipy1 && x + an > clipx0 && y + al > clipy0) {
				sp_cortado(si,
				           x,
				           y,
				           an,
				           al,
				           mem[ide + _Flags]);
			}

			x0s = x;
			x1s = x + an - 1;
			y0s = y;
			y1s = y + al - 1;

			save_region();
		}
	}
}

void save_region(void) {
	if(mem[ide + _Ctype] == 0) {
		if(clipx0 > x0s) { x0s = clipx0; }
		if(clipx1 < x1s) { x1s = clipx1; }
		if(clipy0 > y0s) { y0s = clipy0; }
		if(clipy1 < y1s) { y1s = clipy1; }
		if(x1s < x0s || y1s < y0s) {
			return;
		}

		mem[ide + _x0] = x0s;
		mem[ide + _y0] = y0s;
		mem[ide + _x1] = x1s;
		mem[ide + _y1] = y1s;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - [espejado] [ghost]
//════════════════════════════════════════════════════════════════════════════
void sp_normal(byte *p, int x, int y, int an, int al, int flags) {
	byte *q = copia + y * vga_an + x;
	int ancho = an;

	switch(flags & 7) {
		// --
		case 0: {
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
			break;
		}
		// h-
		case 1: {
			p += an - 1;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p--;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
				p += an * 2;
			}while(--al);
			break;
		}
		// -v
		case 2: {
			p+=(al-1)*an;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p++;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
				p -= an * 2;
			}while(--al);
			break;
		}
		// hv
		case 3: {
			p += al * an -1;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p--;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
			}while(--al);
			break;
		}
		//-- Ghost
		case 4: {
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p++;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
			}while(--al);
			break;
		}
		// h- Ghost
		case 5: {
			p += an - 1;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p--;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
				p += an * 2;
			}while(--al);
			break;
		}
		//-v Ghost
		case 6: {
			p += (al - 1) * an;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p++;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
				p -= an * 2;
			}while(--al);
			break;
		}
		// hv Ghost
		case 7: {
			p += al * an - 1;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p--;
					q++;
				}while(--an);

				q += vga_an - (an = ancho);
			}while(--al);
			break;
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - cortado [espejado] [ghost]
//════════════════════════════════════════════════════════════════════════════
void sp_cortado(byte *p, int x, int y, int an, int al, int flags) {
	byte *q = copia + y * vga_an + x;
	int salta_x, long_x, resto_x;
	int salta_y, long_y, resto_y;

	if(x      < clipx0) { salta_x = clipx0 - x;     } else { salta_x = 0; }
	if(x + an > clipx1) { resto_x = x + an -clipx1; } else { resto_x = 0; }
	long_x = an - salta_x - resto_x;

	if(y      < clipy0) { salta_y = clipy0 - y;      } else { salta_y = 0; }
	if(y + al > clipy1) { resto_y = y + al - clipy1; } else { resto_y = 0; }
	long_y = al - salta_y - resto_y;

	switch(flags & 7) {
		// --
		case 0: {
			p       += an * salta_y + salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			an       = long_x;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p++;
					q++;
				}while(--an);

				p += resto_x;
				q += vga_an - (an = long_x);
			}while(--long_y);
			break;
		}
		// h-
		case 1: {
			p       += an * salta_y + an - 1 - salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p--;
					q++;
				}while(--salta_x);

				p += an + long_x;
				q += vga_an - (salta_x = long_x);
			}while(--long_y);
			break;
		}
		// -v
		case 2: {
			p       += (al - 1) * an - an * salta_y + salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p++;
					q++;
				}while(--salta_x);

				p += resto_x - an * 2;
				q += vga_an - (salta_x = long_x);
			}while(--long_y);
			break;
		}
		// hv
		case 3: {
			p       += al * an - 1 - an * salta_y - salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					if(*p) {
						*q = *p;
					}
					p--;
					q++;
				}while(--salta_x);

				p -= resto_x;
				q += vga_an - (salta_x = long_x);
			} while (--long_y);
			break;
		}
		// -- Ghost
		case 4: {
			p       += an * salta_y + salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			an       = long_x;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p++;
					q++;
				}while(--an);

				p += resto_x;
				q += vga_an - (an = long_x);
			}while(--long_y);
			break;
		}
		// h- Ghost
		case 5: {
			p       += an * salta_y + an - 1 - salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p--;
					q++;
				}while(--salta_x);

				p += an + long_x;
				q += vga_an - (salta_x = long_x);
			}while(--long_y);
			break;
		}
		// -v Ghost
		case 6: {
			p       += (al - 1) * an - an * salta_y + salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p++;
					q++;
				}while(--salta_x);

				p += resto_x - an * 2;
				q += vga_an - (salta_x = long_x);
			}while(--long_y);
			break;
		}
		// hv Ghost
		case 7: {
			p       += al * an - 1 - an * salta_y - salta_x;
			q       += vga_an * salta_y + salta_x;
			resto_x += salta_x;
			salta_x  = long_x;
			do {
				do {
					*q = ghost[(*q << 8) + *p];
					p--;
					q++;
				}while(--salta_x);

				p -= resto_x;
				q += vga_an - (salta_x = long_x);
			}while(--long_y);
			break;
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - escalado [cortado] [espejado] [ghost]
//════════════════════════════════════════════════════════════════════════════
void sp_escalado(byte *old_si, int x, int y, int an, int al, int xg, int yg, int size, int flags) {
	int salta_x, long_x, resto_x; // Referidas a pantalla
	int salta_y, long_y, resto_y;
	int xr, ixr, yr, iyr, old_xr, old_an;
  byte *si,*di, c;

	if(flags & 1) { x0s = x - ((an - 1 - xg) * size) / 100; }
	else          { x0s = x - (xg * size) / 100;            }
	if(flags & 2) { y0s = y - ((al - 1 - yg) * size) / 100; }
	else          { y0s = y - (yg * size) / 100;            }
	x1s = x0s + (an * size) / 100 - 1;
	y1s = y0s + (al * size) / 100 - 1;
	if(x1s < x0s || y1s < y0s) {
		return;
	}

	ixr = (float)(an * 256) / (float)(x1s - x0s + 1);
	iyr = (float)(al * 256) / (float)(y1s - y0s + 1);
	if(x1s <  clipx0 || y1s <  clipy0 ||
	   x0s >= clipx1 || y0s >= clipy1) {
		return;
	}

	if(x0s <  clipx0) { salta_x = clipx0 - x0s;     } else { salta_x = 0; }
	if(x1s >= clipx1) { resto_x = x1s - clipx1 + 1; } else { resto_x = 0; }
	long_x = (an * size) / 100 - salta_x - resto_x;

	if(y0s <  clipy0) { salta_y = clipy0 - y0s;     } else { salta_y = 0; }
	if(y1s >= clipy1) { resto_y = y1s - clipy1 + 1; } else { resto_y = 0; }
	long_y = (al * size) / 100 - salta_y - resto_y;

	if(flags & 1) {
		xr  = an * 256 - salta_x * ixr - 1;
		ixr = -ixr;
	}
	else {
		xr  = salta_x * ixr;
	}
	if(flags & 2) {
		yr  = al * 256 - salta_y * iyr - 1;
		iyr = -iyr;
	}
	else {
		yr  = salta_y * iyr;
	}

	di      = copia + y0s * vga_an + x0s;
	di     += vga_an * salta_y + salta_x;
	old_xr  = xr;
	old_an  = an;
	an      = long_x;

	if(flags & 4) {
		do {
			si = old_si + (yr >> 8) * old_an;
			do {
				c = * (si + (xr >> 8));
				*di = ghost[(*di << 8) + c];

				di++;
				xr += ixr;
			}while(--an);

			yr += iyr;
			xr  = old_xr;
			di += vga_an-(an = long_x);
		}while(--long_y);
	}
	else {
		do {
			si = old_si + (yr >> 8) * old_an;
			do {
				if(c = *(si + (xr >> 8))) {
					*di = c;
				}
				di++;
				xr += ixr;
			}while(--an);

			yr += iyr;
			xr  = old_xr;
			di += vga_an - (an = long_x);
		}while(--long_y);
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - rotado [escalado] [cortado] [espejado] [ghost]
//════════════════════════════════════════════════════════════════════════════
void sp_rotado(byte *si, int x, int y, int an, int al, int xg, int yg, int ang, int size, int flags) {
	float d0, d1, d2, d3;
	float a0, a1, a2, a3;
	float a, s;
	int p[24];
	int h, hmin, hmax; // Altura minima y maxima
	int n, l0, l1;     // Lado 0 y lado 1 (indices p[])

	int hmax0, hmax1;
	union {
		int   l;
		short w[2];
	} x0, x1, g0x, g1x, g0y, g1y;
	int ix0, ix1, ig0x, ig1x, ig0y, ig1y, kk;
	byte *ptrcopia;

	a  = (float)ang / radian;
	s  = (float)size / 100;
	xg = -xg;
	yg = -yg;

	d0 = (float)sqrt(xg * xg + yg * yg) * s;
	if(yg == 0 && xg == 0) { a0 = a; } else { a0 = a + (float)atan2(-yg, xg); }
	xg = xg + an - 1;

	d1 = (float)sqrt(xg * xg + yg * yg) * s;
	if(yg == 0 && xg == 0) { a1 = a; } else { a1 = a + (float)atan2(-yg, xg); }
	yg = yg + al - 1;

	d2 = (float)sqrt(xg * xg + yg * yg) * s;
	if(yg == 0 && xg == 0) { a2 = a; } else { a2 = a + (float)atan2(-yg, xg); }
	xg = xg - an + 1;

	d3 = (float)sqrt(xg * xg + yg * yg) * s;
	if(yg == 0 && xg == 0) { a3 = a; } else { a3 = a + (float)atan2(-yg, xg); }

	if(flags & 1) {
		p[0] = x - (int)((float)cos(a0) * d0);
		p[2] = x - (int)((float)cos(a1) * d1);
		p[4] = x - (int)((float)cos(a2) * d2);
		p[6] = x - (int)((float)cos(a3) * d3);
	}
	else {
		p[0] = x + (int)((float)cos(a0) * d0);
		p[2] = x + (int)((float)cos(a1) * d1);
		p[4] = x + (int)((float)cos(a2) * d2);
		p[6] = x + (int)((float)cos(a3) * d3);
	}

	if(flags & 2) {
		p[1] = y - (int)(-(float)sin(a0) * d0);
		p[3] = y - (int)(-(float)sin(a1) * d1);
		p[5] = y - (int)(-(float)sin(a2) * d2);
		p[7] = y - (int)(-(float)sin(a3) * d3);
	}
	else {
		p[1] = y + (int)(-(float)sin(a0) * d0);
		p[3] = y + (int)(-(float)sin(a1) * d1);
		p[5] = y + (int)(-(float)sin(a2) * d2);
		p[7] = y + (int)(-(float)sin(a3) * d3);
	}

	memcpy(&p[8],
	       &p[0],
	        sizeof(p[0]) * 8);
	memcpy(&p[16],
	       &p[0],
	        sizeof(p[0]) * 8);

	x0s = hmin =  32767;
	x1s = hmax = -32768;
	n   = 8;
	do {
		if(p[n]     < x0s ) { x0s = p[n];              }
		if(p[n]     > x1s ) { x1s = p[n];              }
		if(p[n + 1] < hmin) { hmin = p[n + 1]; l0 = n; }
		if(p[n + 1] > hmax) { hmax = p[n + 1];         }

		n += 2;
	}while(n < 16);

	y0s      = hmin;
	y1s      = hmax;

	ptrcopia = copia + hmin * vga_an;
	l1       = l0;
	hmax0    = hmax1 = hmin;
	h        = hmin;
	do {
		if(h < hmax) {
			while(h == hmax0) {
				if((hmax0 = p[l0 - 1]) != h) {
					x0.l = p[l0] << 16;
					ix0  = ((p[l0 - 2] << 16) - x0.l) / (hmax0 - h);
					if(ix0 < 0) {
						x0.l += 65535;
					}
					switch(l0 & 6) {
						case 0: {
							g0x.l = 0;
							g0y.l = 0;
							ig0x  = 0;
							ig0y  = (al << 16) / (hmax0 - h);
							break;
						}
						case 2: {
							g0x.l = ((an - 1) << 16) + 65535;
							g0y.l = 0;
							ig0x  = (an << 16) / (h - hmax0);
							ig0y  = 0;
							break;
						}
						case 4: {
							g0x.l = ((an - 1) << 16) + 65535;
							g0y.l = ((al - 1) << 16) + 65535;
							ig0x  = 0;
							ig0y  = (al << 16) / (h - hmax0);
							break;
						}
						case 6: {
							g0x.l = 0;
							g0y.l = ((al - 1) << 16) + 65535;
							ig0x  = (an << 16) / (hmax0 - h);
							ig0y  = 0;
							break;
						}
					}
				}

				l0 -= 2;
			}

			while(h == hmax1) {
				if((hmax1 = p[l1 + 3]) != h) {
					x1.l = p[l1] << 16;
					ix1  = ((p[l1 + 2] << 16) - x1.l) / (hmax1 - h);
					if(ix1 < 0) {
						x1.l += 65535;
					}
					switch(l1 & 6) {
						case 0: {
							g1x.l = 0;
							g1y.l = 0;
							ig1x  = (an << 16) / (hmax1 - h);
							ig1y  = 0;
							break;
						}
						case 2: {
							g1x.l = ((an - 1) << 16) + 65535;
							g1y.l = 0;
							ig1x  = 0;
							ig1y  = (al << 16) / (hmax1 - h);
							break;
						}
						case 4: {
							g1x.l = ((an - 1) << 16) + 65535;
							g1y.l = ((al - 1) << 16) + 65535;
							ig1x  = (an << 16) / (h - hmax1);
							ig1y  = 0;
							break;
						}
						case 6: {
							g1x.l = 0;
							g1y.l = ((al - 1) << 16) + 65535;
							ig1x  = 0;
							ig1y  = (al << 16) / (h - hmax1);
							break;
						}
					}
				}

				l1 += 2;
			}
		}
		else {
			if(h == hmax0) {
				g0x.l = 0;
				g0y.l = 0;
				ig0x  = 0;
				ig0y  = 0;
				x0.l  = p[l0] << 16;
				ix0   = 0;
				switch(l0 & 6) {
					case 2: {
						g0x.l = ((an - 1) << 16) + 65535;
						break;
					}
					case 4: {
						g0x.l = ((an - 1) << 16) + 65535;
						g0y.l = ((al - 1) << 16) + 65535;
						break;
					}
					case 6: {
						g0y.l = ((al - 1) << 16) + 65535;
						break;
					}
				}
			}

			if(h == hmax1) {
				g1x.l = 0;
				g1y.l = 0;
				ig1x  = 0;
				ig1y  = 0;
				x1.l  = p[l1] << 16;
				ix1   = 0;
				switch(l1 & 6) {
					case 2: {
						g1x.l = ((an - 1) << 16) + 65535;
						break;
					}
					case 4: {
						g1x.l = ((an - 1) << 16) + 65535;
						g1y.l = ((al - 1) << 16) + 65535;
						break;
					}
					case 6: {
						g1y.l = ((al - 1) << 16) + 65535;
						break;
					}
				}
			}
		}

		if((flags & 3) == 1 ||
		   (flags & 3) == 2) {
			kk    = x0.l;
			x0.l  = x1.l;
			x1.l  = kk;

			kk    = g0x.l;
			g0x.l = g1x.l;
			g1x.l = kk;

			kk    = g0y.l;
			g0y.l = g1y.l;
			g1y.l = kk;
		}

		if(h       < clipy1 && h       >= clipy0 &&
		   x0.w[1] < clipx1 && x1.w[1] >= clipx0 &&
		   x1.w[1] > x0.w[1]) {
			if(x0.w[1] < clipx0) {
				if(x1.w[1] >= clipx1) {
					if(flags & 4) {
						sp_scancg(ptrcopia + clipx0,
						          x1.w[1] - x0.w[1],
						          clipx1 - clipx0 - 1,
						          clipx0 - x0.w[1],
						          si,
						          an,
						          g0x.l,
						          g0y.l,
						          g1x.l,
						          g1y.l);
					}
					else {
						sp_scanc(ptrcopia + clipx0,
						         x1.w[1] - x0.w[1],
						         clipx1 - clipx0 - 1,
						         clipx0 - x0.w[1],
						         si,
						         an,
						         g0x.l,
						         g0y.l,
						         g1x.l,
						         g1y.l);
					}
				}
				else {
					if(flags & 4) {
						sp_scancg(ptrcopia + clipx0,
						          x1.w[1] - x0.w[1],
						          x1.w[1] - clipx0,
						          clipx0 - x0.w[1],
						          si,
						          an,
						          g0x.l,
						          g0y.l,
						          g1x.l,
						          g1y.l);
					}
					else {
						sp_scanc(ptrcopia + clipx0,
						         x1.w[1] - x0.w[1],
						         x1.w[1] - clipx0,
						         clipx0 - x0.w[1],
						         si,
						         an,
						         g0x.l,
						         g0y.l,
						         g1x.l,
						         g1y.l);
					}
				}
			}
			else if(x1.w[1] >= clipx1) {
				if(flags & 4) {
					sp_scancg(ptrcopia + x0.w[1],
					          x1.w[1] - x0.w[1],
					          clipx1 - 1 - x0.w[1],
					          0,
					          si,
					          an,
					          g0x.l,
					          g0y.l,
					          g1x.l,
					          g1y.l);
				}
				else {
					sp_scanc(ptrcopia + x0.w[1],
					         x1.w[1] - x0.w[1],
					         clipx1 - 1 - x0.w[1],
					         0,
					         si,
					         an,
					         g0x.l,
					         g0y.l,
					         g1x.l,
					         g1y.l);
				}
			}
			else {
				if(flags & 4) {
					sp_scang(ptrcopia + x0.w[1],
					         x1.w[1] - x0.w[1],
					         si,
					         an,
					         g0x.l,
					         g0y.l,
					         g1x.l,
					         g1y.l);
				}
				else {
					sp_scan(ptrcopia + x0.w[1],
					        x1.w[1] - x0.w[1],
					        si,
					        an,
					        g0x.l,
					        g0y.l,
					        g1x.l,
					        g1y.l);
				}
			}
		}

		if((flags & 3) == 1 ||
		   (flags & 3) == 2) {
			kk    = x0.l;
			x0.l  = x1.l;
			x1.l  = kk;

			kk    = g0x.l;
			g0x.l = g1x.l;
			g1x.l = kk;

			kk    = g0y.l;
			g0y.l = g1y.l;
			g1y.l = kk;
		}

		x0.l     += ix0;
		x1.l     += ix1;
		g0x.l    += ig0x;
		g1x.l    += ig1x;
		g0y.l    += ig0y;
		g1y.l    += ig1y;

		ptrcopia += vga_an;
	}while(h++ < hmax);
}

//════════════════════════════════════════════════════════════════════════════
// Impresión de un sprite rotado y escalado, sin cortar
//════════════════════════════════════════════════════════════════════════════
void sp_scanc(byte *p, short n, short m, short o, byte *si, int an, int x0, int y0, int x1, int y1) {
	union {
		int   l;
		short w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0  = (x1 - x0) / n;
	y0  = (y1 - y0) / n;

	if(o) {
		x.l += x0 * o;
		y.l += y0 * o;
	}

	do {
		if(c = *(si + x.w[1] + y.w[1] * an)) {
			*p = c;
		}
		p++;

		x.l += x0;
		y.l += y0;
	}while(m--);
}

void sp_scancg(byte *p, short n, short m, short o, byte *si, int an, int x0, int y0, int x1, int y1) {
	union {
		int   l;
		short w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0  = (x1 - x0) / n;
	y0  = (y1 - y0) / n;

	if(o) {
		x.l += x0 * o;
		y.l += y0 * o;
	}

	do {
		c = *(si + x.w[1] + y.w[1] * an);
		*p = ghost[(c << 8) + *p];
		p++;

		x.l += x0;
		y.l += y0;
	}while(m--);
}

//════════════════════════════════════════════════════════════════════════════
// Impresión de un sprite rotado y escalado, cortando
//════════════════════════════════════════════════════════════════════════════
void sp_scan(byte *p, short n, byte *si, int an, int x0, int y0, int x1, int y1) {
	union {
		int   l;
		short w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0  = (x1 - x0) / n;
	y0  = (y1 - y0) / n;

	do {
		if(c = *(si + x.w[1] + y.w[1] * an)) {
			*p = c;
		}
		p++;

		x.l += x0;
		y.l += y0;
	}while(n--);
}

void sp_scang(byte *p, short n, byte *si, int an, int x0, int y0, int x1, int y1) {
	union {
		int   l;
		short w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0  = (x1 - x0) / n;
	y0  = (y1 - y0) / n;

	do {
		c = *(si + x.w[1] + y.w[1] * an);
		*p = ghost[(c << 8) + *p];
		p++;

		x.l += x0;
		y.l += y0;
	}while(n--);
}

//════════════════════════════════════════════════════════════════════════════
// Textos
//════════════════════════════════════════════════════════════════════════════
void text_out(char *ptr, int x, int y) {
	int an, al;
	byte *ptr2;

	an   = 0;
	ptr2 = (byte *)ptr;
	while(*ptr2) {
		if(fnt[*ptr2].ancho == 0) {
			an += f_i[0].espacio;
			ptr2++;
		}
		else {
			an += fnt[*ptr2].ancho;
			ptr2++;
		}
	}

	fnt = (t_fnt_table *)((byte *)fonts[0] + 1356);

	al = f_i[0].alto;
	while(*ptr && x + fnt[*ptr].ancho <= 0) {
		if(fnt[*ptr].ancho == 0) {
			x += f_i[0].espacio;
			ptr++;
		}
		else {
			x = x + fnt[*ptr].ancho;
			ptr++;
		}
	}

	while(*ptr && x<vga_an) {
		if(fnt[*ptr].ancho == 0) {
			x += f_i[0].espacio;
			ptr++;
		}
		else {
			texc(fonts[0] + fnt[*ptr].offset,
			     x,
			     y + fnt[*ptr].incY,
			     fnt[*ptr].ancho,
			     fnt[*ptr].alto);

			x = x + fnt[*ptr].ancho;
			ptr++;
		}
	}
}

void pinta_textos(void) { // E: texto[]
	int n = 0, x, y, an ,al;
	int fuente;

	byte *ptr, *ptr2;
	byte numero[32];

	do {
		if(texto[n].font) {
			texto[n].an = 0; // Inicializa la region a volcar para este texto

			switch (texto[n].tipo) {
				case 0: {
					ptr = (byte *)&mem[texto[n].ptr];
					break;
				}
				case 1: {
					if(mem[texto[n].ptr] < 0) {
						numero[0] = '-';
						ltoa(-mem[texto[n].ptr], (char *)numero + 1, 10);
					}
					else {
						ltoa(mem[texto[n].ptr], (char *)numero, 10);
					}

					ptr = numero;
					break;
				}
			}

			fuente = 0;
			while(fuente < max_fonts) {
				if(texto[n].font == fonts[fuente]) {
					break;
				}

				fuente++;
			}
			if(fuente == max_fonts) {
				continue;
			}

			fnt  = (t_fnt_table *)((byte *)texto[n].font + 1356);
			x    = texto[n].x; // X primer carácter
			y    = texto[n].y; // Y
			al   = f_i[fuente].alto;
			an   = 0;
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

			switch(texto[n].centro) {
				case 0: {                                       break; }
				case 1: { x = x - (an >> 1);                    break; }
				case 2: { x = x - an;                           break; }
				case 3: { y = y - (al >> 1);                    break; }
				case 4: { x = x - (an >> 1); y = y - (al >> 1); break; }
				case 5: { x = x - an;        y = y - (al >> 1); break; }
				case 6: { y = y - al;                           break; }
				case 7: { x = x - (an >> 1); y = y - al;        break; }
				case 8: { x = x - an;        y = y - al;        break; }
			}

			if(y < vga_al && y + al > 0) {
				texto[n].x0 = x;
				texto[n].y0 = y;
				texto[n].an = an;
				texto[n].al = al;

				// El texto coge entero (coord. y)
				if(y >= 0 && y + al <= vga_al) {
					while(*ptr && x + fnt[*ptr].ancho <= 0) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							x = x + fnt[*ptr].ancho;
							ptr++;
						}
					}

					if(*ptr && x < 0) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							texc(texto[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].ancho,
							     fnt[*ptr].alto);

							x = x + fnt[*ptr].ancho;
							ptr++;
						}
					}

					while(*ptr && x + fnt[*ptr].ancho <= vga_an) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							texn(texto[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].ancho,
							     fnt[*ptr].alto);

							x = x + fnt[*ptr].ancho;
							ptr++;
						}
					}

					if(*ptr && x < vga_an) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							texc(texto[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].ancho,
							     fnt[*ptr].alto);
						}
					}
				}
				else {
					while(*ptr && x + fnt[*ptr].ancho <= 0) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							x = x + fnt[*ptr].ancho;
							ptr++;
						}
					}

					while(*ptr && x < vga_an) {
						if(fnt[*ptr].ancho == 0) {
							x += f_i[fuente].espacio;
							ptr++;
						}
						else {
							texc(texto[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].ancho,
							     fnt[*ptr].alto);

							x = x + fnt[*ptr].ancho;
							ptr++;
						}
					}
				}
			}
		}
	}while(++n < max_textos);
}

void texn(byte *p, int x, int y, byte an, int al) {
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

void texc(byte *p, int x, int y, byte an, int al) {
	byte *q = copia + y * vga_an + x;
	int salta_x, long_x, resto_x;
	int salta_y, long_y, resto_y;

	if(x      < 0)      { salta_x = -x;              } else { salta_x = 0; }
	if(x + an > vga_an) { resto_x = x + an - vga_an; } else { resto_x = 0; }
	long_x = an - salta_x - resto_x;
	if(long_x <= 0) {
		return;
	}

	if(y      < 0)      { salta_y = -y;              } else { salta_y = 0; }
	if(y + al > vga_al) { resto_y = y + al - vga_al; } else { resto_y = 0; }
	long_y = al - salta_y - resto_y;
	if(long_y <= 0) {
		return;
	}

	p       += an * salta_y + salta_x;
	q       += vga_an * salta_y + salta_x;
	resto_x += salta_x;
	an       = long_x;
	do {
		do {
			if(*p) {
				*q = *p;
			}
			p++;
			q++;
		}while(--an);

		p += resto_x;
		q += vga_an - (an = long_x);
	}while(--long_y);
}

//════════════════════════════════════════════════════════════════════════════
// Pinta la ventana de modo 7
//════════════════════════════════════════════════════════════════════════════
void pinta_sprites_m7(int n, int cx, int cy, float ang);

void pinta_m7(int n) {
	int x, y;
	int id       = (m7 + n)->camera;
	int height   = (m7 + n)->height;
	int distance = (m7 + n)->distance;
	int angle    = mem[id + _Angle];

	if(!im7[n].on || !id || id != mem[id]) {
		return;
	}

	while(mem[id + _Angle] > 2 * pi) { mem[id + _Angle] -= 2 * pi; }
	while(mem[id + _Angle] < 0)      { mem[id + _Angle] += 2 * pi; }

	angle = (mem[id + _Angle] * 4096) / (2 * pi);

	if(mem[id + _Resolution] > 0) {
		x =  (((mem[id + _X] * 256) / mem[id + _Resolution]) * 256 - get_distx(mem[id + _Angle], distance * 65536));
		y = -(((mem[id + _Y] * 256) / mem[id + _Resolution]) * 256 - get_disty(mem[id + _Angle], distance * 65536));
	}
	else {
		x =  (mem[id + _X] * 65536 - get_distx(mem[id + _Angle], distance * 65536));
		y = -(mem[id + _Y] * 65536 - get_disty(mem[id + _Angle], distance * 65536));
	}

	pinta_modo7(n,
	            x,
	            height * 16384,
	            y,
	            angle); //<<14

	if(post_process_m7 != NULL) {
		post_process_m7();
	}

	pinta_sprites_m7(n,
	                 x,
	                -y,
	                 (float)((float)mem[id + _Angle] / radian));
}

//════════════════════════════════════════════════════════════════════════════
// Pinta los sprites del modo 7 (scroll snum)
//════════════════════════════════════════════════════════════════════════════
void pinta_sprites_m7(int n, int cx, int cy, float ang) { // Le pasamos la posición de la cámara
	int factor;
	int cx2 = cx / 4096, cy2 = cy / 4096, dx, dy;
	int ide, id, max, distmax, old_z;
	int altura, h, anchura, porcen;
	float a;

	factor = (((m7 + n)->focus * im7[n].an * 16) / 320);

	// Pre-calcula _Dist1 y _Dist2 para cada objeto del modo-7
	for(id = id_start; id <= id_end; id += iloc_len) {
		if((mem[id + _Status ] == 2  ||
		    mem[id + _Status ] == 4) &&
		    mem[id + _Ctype  ] == 2  &&
		   (mem[id + _Cnumber] == 0  ||
		   (mem[id + _Cnumber] & (1 << n)))) {
			if(mem[id + _Resolution] > 0) {
				dx = (mem[id + _X] * 16) / mem[id + _Resolution] - cx2;
				dy = cy2 - (mem[id + _Y] * 16) / mem[id + _Resolution];
			}
			else {
				dx = mem[id + _X] * 16 - cx2;
				dy = cy2 - mem[id + _Y] * 16;
			}

			if((!dx && !dy) || abs(dx) + abs(dy) > 2800 * 16) {
				mem[id + _Dist1] = 0;
			}
			else {
				a = ang - atan2(dy, dx);

				mem[id + _Dist1] = sqrt(dx * dx + dy * dy);
				mem[id + _Dist2] = (float)mem[id + _Dist1] * sin(a);
				mem[id + _Dist1] = (float)mem[id + _Dist1] * cos(a);
			}
		}
		else {
			mem[id + _Dist1] = 0;
		}
	}

	// Marca todos objetos mode7 como no pintados
	for(ide = id_start; ide <= id_end; ide += iloc_len) {
		mem[ide + _Painted] = 0;
	}

	// Pinta los objetos visibles dentro del modo-7
	do {
		ide = 0;
		max = 0x80000000;

		for(id = id_start; id <= id_end; id += iloc_len) {
			if(mem[id + _Dist1] && !mem[id + _Painted]) {
				if(mem[id + _Dist1] >  max ||
					(mem[id + _Dist1] == max && mem[id + _Z] > old_z)) {
					ide     = id;
					max     = mem[id + _Dist1];
					distmax = mem[id + _Dist2];
					old_z   = mem[id + _Z];
				}
			}
		}

		if(ide) {
			// No pinta el objeto si está en las coordenadas de la cámara (±2)
			if(max >= 32) {
				h       = (m7 + n)->height - mem[ide + _Height];
				altura  = (h * (max - factor)) / (max);                // en pix / 4
				altura  = im7[n].y + im7[n].al / 2 + (h - altura) / 4; // en pix
				altura += (m7 + n)->horizon - im7[n].al / 2;

				h      += 1000;
				porcen  = (h * (max - factor)) / (max);                // en pix / 4
				porcen  = im7[n].y + im7[n].al / 2 + (h - porcen) / 4; // en pix
				porcen += (m7 + n)->horizon - im7[n].al / 2 - altura;

				porcen  = mem[ide + _Size] * porcen / 1000;

				anchura = vga_an / 2 + (factor * distmax) / (max * 16);
				a       = ((float)mem[ide + _Angle] / radian) - ang;
				h       = (int)((a * 4096.0) / 6.2831853) + 1024;
				while(h <     0) { h += 4096; }
				while(h >= 4096) { h -= 4096; }

				pinta_sprite_m7(n,
				                ide,
				                anchura,
				                altura,
				                porcen,
				                h);

				// *** Pixel blanco en la base del objeto ***
				//if(altura <  vga_al && altura  >= 0 &&
				//   anchura < vga_an && anchura >= 0) {
				//	*(copia + altura * vga_an + anchura) = 127;
				//}
			}

			mem[ide + _Painted] = 1;
		}
	}while(ide);
}

//════════════════════════════════════════════════════════════════════════════
// Pinta un sprite (mem[ide]) en el modo 7 (si está delante de la cámara)
//════════════════════════════════════════════════════════════════════════════
void pinta_sprite_m7(int n, int ide, int x, int y, int size, int ang) {
	int *ptr;
	byte *si;
	int an ,al; // Información respecto a pantalla del grafico
	int xg, yg;
	int m, p;

	if(mem[ide + _File] > max_fpgs || mem[ide + _File] < 0) {
		return;
	}

	if((p = mem[ide + _XGraph]) > 0) {
		m = mem[p];
		if(m < 1 || m > 256) {
			return;
		}

		// ang es el ángulo de visión del gráfico (0..4096), m el número de particiones
		mem[ide + _Flags] &= 254;
		mem[ide + _Graph] = ((ang + 4096 / (m * 2)) * m) / 4096;
		if(mem[ide + _Graph] >= m) {
			mem[ide + _Graph] = 0;
		}
		if((mem[ide + _Graph] = mem[p + 1 + mem[ide + _Graph]]) < 0) {
			mem[ide + _Graph]  =-mem[ide + _Graph];
			mem[ide + _Flags] |= 1;
		}
	}
	if(mem[ide + _File]) { max_grf = 1000; } else { max_grf = 2000; }
	if(mem[ide + _Graph] <= 0 || mem[ide + _Graph] >= max_grf) {
		return;
	}
	if(g[mem[ide + _File]].grf == NULL) {
		return;
	}

	clipx0 = im7[n].x;
	clipx1 = im7[n].x + im7[n].an;
	clipy0 = im7[n].y;
	clipy1 = im7[n].y + im7[n].al;

	if((ptr = g[mem[ide + _File]].grf[mem[ide + _Graph]]) != NULL) {
		an = ptr[13];
		al = ptr[14];                        // Ancho y Alto del gráfico
		si = (byte *)ptr + 64 + ptr[15] * 4; // Inicio del gráfico

		if(ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] - 1;
		}
		else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		sp_escalado(si,
		            x,
		            y,
		            an,
		            al,
		            xg,
		            yg,
		            size,
		            mem[ide + _Flags]);
	}
}

//════════════════════════════════════════════════════════════════════════════
// Get_distx/y(angulo, dist)
//════════════════════════════════════════════════════════════════════════════
int get_distx(int a, int d) {
	angulo = (float)a / radian;

	return ((int)((float)cos(angulo) * d));
}

int get_disty(int a, int d) {
	angulo = (float)a / radian;

	return (-(int)((float)sin(angulo) * d));
}

//════════════════════════════════════════════════════════════════════════════
// Dibuja el mapa en perspectiva
//════════════════════════════════════════════════════════════════════════════
void pinta_modo7(int n, int camara_x, int camara_y, int camara_z, int angulo) {
	int y, u, du, vv, dv, pos_x, pos_y;
	int ancho_m, alto_m, ancho_e, alto_e;
	int proyeccion_y, sint, cost;
	int mediox_modo7 = im7[n].an >> 1;
	int mediox_modo7_16;
	int distancia = ((im7[n].an * (m7 + n)->focus) / 320) << 16;
	int divisor;
	byte *di = copia + im7[n].y * vga_an + im7[n].x,*di_end, color = (m7 + n)->color;

	mediox_modo7_16 = mediox_modo7 << 16;

	ancho_m = im7[n].map_an - 1;
	alto_m  = im7[n].map_al - 1;
	ancho_e = im7[n].ext_an - 1;
	alto_e  = im7[n].ext_al - 1;

	if(!camara_y) {
		return;
	}

	for(y = 0; y < im7[n].al; y++, di += vga_an - im7[n].an) {
		divisor = (m7 + n)->horizon - y - 1;
		if(camara_y > 0) {
			if(divisor >= 0) { di += im7[n].an; continue; }
		}
		else {
			if(divisor <= 0) { di += im7[n].an; continue; }
		}
		proyeccion_y = -camara_y / divisor;

		sint = mul_24(seno[angulo],   proyeccion_y);
		cost = mul_24(coseno[angulo], proyeccion_y);

		u  =  camara_x + mul_16(distancia, cost) + mul_16(sint, -mediox_modo7_16);
		du =  sint;

		vv =  camara_z + mul_16(distancia, sint) - mul_16(cost, -mediox_modo7_16);
		dv = -cost;

		vv = -vv;
		dv = -dv;

		di_end = di + im7[n].an;

		if(im7[n].ext != NULL) {
			do {
				pos_x = u  >> 16;
				pos_y = vv >> 16;
				if(pos_x > ancho_m || pos_x < 0 ||
				   pos_y > alto_m  || pos_y < 0) {
					*di = *(im7[n].ext + (pos_y & alto_e) * im7[n].ext_an + (pos_x & ancho_e));
				}
				else {
					*di = *(im7[n].map + pos_y * im7[n].map_an + pos_x);
				}

				u  += du;
				vv += dv;
			}while(++di < di_end);
		}
		else {
			do {
				pos_x = u  >> 16;
				pos_y = vv >> 16;
				if(pos_x > ancho_m || pos_x < 0 ||
				   pos_y > alto_m  || pos_y < 0) {
					*di = color;
				}
				else {
					*di = *(im7[n].map + pos_y * im7[n].map_an + pos_x);
				}

				u  += du;
				vv += dv;
			}while(++di < di_end);
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Multiplica dos numeros de 8.24
//════════════════════════════════════════════════════════════════════════════
#ifdef DOS
extern int mul_24(int, int);

#pragma aux mul_24 = \
        "imul edx" \
        "shrd eax,edx,24" \
        parm caller [EDX] [EAX] value [EAX]
#else
int mul_24(int a, int b) {
	uint64_t c = (uint64_t)a * (uint64_t)b;

	return (c >> 24);
}
#endif

//════════════════════════════════════════════════════════════════════════════
//  Multiplica dos numeros de 16.16
//════════════════════════════════════════════════════════════════════════════
#ifdef DOS
extern int mul_16(int, int);

#pragma aux mul_16 = \
        "imul edx"   \
        "shrd eax,edx,16" \
        parm caller [EDX] [EAX] value [EAX]
#else
unsigned int mul_16(int a, int b) {
	uint64_t c = (uint64_t)a * (uint64_t)b;

	return c >> 16;
}
#endif

//════════════════════════════════════════════════════════════════════════════
// Inicializa la tabla de senos y cosenos
//════════════════════════════════════════════════════════════════════════════
void init_sin_cos() {
	int i;

	// Generamos la tabla del seno y el coseno
	for(i = 0; i < grados_90; i++) {
		seno[i] = 256 *sin((float)i * 1131 / 737280) * 65536;

		seno[grados_180 - i] =  seno[i];
		seno[grados_180 + i] = -seno[i];
		seno[grados_360 - i] = -seno[i];

		coseno[grados_90  - i] =  seno[i];
		coseno[grados_90  + i] = -seno[i];
		coseno[grados_270 - i] = -seno[i];
		coseno[grados_270 + i] =  seno[i];
	}
	seno[grados_0  ] =  0;
	seno[grados_90 ] =  256 * 65536;
	seno[grados_180] =  0;
	seno[grados_270] = -256 * 65536;
	seno[grados_360] =  0;

	coseno[grados_0  ] =  256 * 65536;
	coseno[grados_90 ] =  0;
	coseno[grados_180] = -256 * 65536;
	coseno[grados_270] =  0;
	coseno[grados_360] =  256 * 65536;
}
