
//════════════════════════════════════════════════════════════════════════════
// Functions for sprites, text and scroll
//════════════════════════════════════════════════════════════════════════════

#include "inter.h"

#ifdef DEBUG
int get_ticks(void);
void process_paint(int id, int n);
void function_exec(int id, int n);
#endif

int line_fx, color;

void line(int x0, int y0, int x1, int y1);
void draw_rectangle(int x, int y, int width, int height);
void filled_box(int x, int y, int width, int height);
void circle(int fill, int x0, int y0, int x1, int y1);
void line_pixel(int x, int y);


//════════════════════════════════════════════════════════════════════════════
// Headers
//════════════════════════════════════════════════════════════════════════════

void move_scroll(int plane, int x, int y);
void save_region(void);
void sp_scanc(byte *p, short n, short m, short o, byte *si, int width, int x0, int y0, int x1, int y1);
void sp_scancg(byte *p, short n, short m, short o, byte *si, int width, int x0, int y0, int x1, int y1);
void sp_scan(byte *p, short n, byte *si, int width, int x0, int y0, int x1, int y1);
void sp_scang(byte *p, short n, byte *si, int width, int x0, int y0, int x1, int y1);
void texc(byte *p, int x, int y, byte width, int height);
void texn(byte *p, int x, int y, byte width, int height);
void draw_mode7(int n, int camera_x, int camera_y, int camera_z, int angle);
void draw_sprite_m7(int n, int ide, int x, int y, int size, int ang);

//════════════════════════════════════════════════════════════════════════════
// Constant definitions
//════════════════════════════════════════════════════════════════════════════

#define angle_0 0
#define angle_30 320
#define angle_45 512
#define angle_90 1024
#define angle_180 2048
#define angle_270 3072
#define angle_360 4096

//════════════════════════════════════════════════════════════════════════════
// Module variables
//════════════════════════════════════════════════════════════════════════════

int sine[angle_360 + 1], cosine[angle_360 + 1]; //m7

//════════════════════════════════════════════════════════════════════════════
// Fast scroll parallax table
//════════════════════════════════════════════════════════════════════════════

tfast *fast;

//════════════════════════════════════════════════════════════════════════════
// Multiplies 2 numbers in 8.24 format
//════════════════════════════════════════════════════════════════════════════

static int mul_24(int a, int b) {
	uint64_t c = (uint64_t)a * (uint64_t)b;
	return (c >> 24);
}

//════════════════════════════════════════════════════════════════════════════
//  Muliply two 16:16 floating points
//════════════════════════════════════════════════════════════════════════════

static unsigned int mul_16(long a, long b) {
	uint64_t c = (uint64_t)a * (uint64_t)b;
	return c >> 16;
}

//════════════════════════════════════════════════════════════════════════════
// Function to draw 2 sprites of a scroll, sorted by Z
//════════════════════════════════════════════════════════════════════════════

void draw_sprites_scroll(void) {
#ifdef DEBUG
	int oclock;
#endif
	int old_ide = ide, max;

	if (post_process_scroll != NULL)
		post_process_scroll();

	for (ide = id_start; ide <= id_end; ide += iloc_len)
		mem[ide + _Painted] = 0;
	do {
#ifdef DEBUG
		oclock = get_ticks();
#endif
		ide = 0;
		max = 0x80000000;
		for (id = id_start; id <= id_end; id += iloc_len)
			if ((mem[id + _Status] == 2 || mem[id + _Status] == 4) && mem[id + _Ctype] == 1 &&
			    (mem[id + _Cnumber] == 0 || (mem[id + _Cnumber] & (1 << snum))) && !mem[id + _Painted] &&
			    mem[id + _Z] > max) {
				ide = id;
				max = mem[id + _Z];
			}
		if (ide) {
			draw_sprite();
			mem[ide + _Painted] = 1;
#ifdef DEBUG
			process_paint(ide, get_ticks() - oclock);
#endif
		}
	} while (ide);
	ide = old_ide;
}

//════════════════════════════════════════════════════════════════════════════
// Determines movement of scrolling window (automatic or manual)
//════════════════════════════════════════════════════════════════════════════

void advance_scroll(int back) {
	int i, x, y, ix, iy, r;

	if (i = (scroll + snum)->camera) { // Checks if scrolling is automatic
		if (i >= id_start && i <= id_end && mem[i] == i) {
			ix = mem[i + _X];
			iy = mem[i + _Y];
			if (mem[i + _Resolution]) {
				ix /= mem[i + _Resolution];
				iy /= mem[i + _Resolution];
			}
			x = ix + iscroll[snum].x - iscroll[snum].map1_x;
			y = iy + iscroll[snum].y - iscroll[snum].map1_y;
			r = (scroll + snum)->region1;
			if (r >= 0 && r < max_region) {
				ix = 0;
				iy = 0;
				if (x < region[r].x0)
					ix = x - region[r].x0;
				if (y < region[r].y0)
					iy = y - region[r].y0;
				if (x >= region[r].x1)
					ix = x - (region[r].x1 - 1);
				if (y >= region[r].y1)
					iy = y - (region[r].y1 - 1);
			} else { // No region1 of scroll lock, move to center
				ix -= iscroll[snum].w / 2;
				iy -= iscroll[snum].h / 2;
				ix -= iscroll[snum].map1_x;
				iy -= iscroll[snum].map1_y;
			}
			r = (scroll + snum)->region2;
			if (r >= 0 && r < max_region) {
				if (x < region[r].x0 || x >= region[r].x1) {
					if (x < region[r].x0)
						ix = x - region[r].x0;
					else
						ix = x - (region[r].x1 - 1);
				} else if ((scroll + snum)->speed > 0) {
					if (ix > (scroll + snum)->speed)
						ix = (scroll + snum)->speed;
					else if (ix < -(scroll + snum)->speed)
						ix = -(scroll + snum)->speed;
				}
				if (y < region[r].y0 || y >= region[r].y1) {
					if (y < region[r].y0)
						iy = y - region[r].y0;
					else
						iy = y - (region[r].y1 - 1);
				} else if ((scroll + snum)->speed > 0) {
					if (iy > (scroll + snum)->speed)
						iy = (scroll + snum)->speed;
					else if (iy < -(scroll + snum)->speed)
						iy = -(scroll + snum)->speed;
				}
			}

			x = iscroll[snum].map1_x;
			y = iscroll[snum].map1_y;
			move_scroll(0, ix, iy);

			if (back)
				if ((scroll + snum)->ratio) {
					ix = iscroll[snum].map1_x - x;
					iy = iscroll[snum].map1_y - y;
					move_scroll(1, ix * 100 / (scroll + snum)->ratio, iy * 100 / (scroll + snum)->ratio);
				}
		}
	} else { // Check whether the scroll is moved manually
		if ((scroll + snum)->x0 != iscroll[snum].map1_x || (scroll + snum)->y0 != iscroll[snum].map1_y) {
			move_scroll(0, (scroll + snum)->x0 - iscroll[snum].map1_x, (scroll + snum)->y0 - iscroll[snum].map1_y);
		}
		if (back)
			if ((scroll + snum)->x1 != iscroll[snum].map2_x || (scroll + snum)->y1 != iscroll[snum].map2_y) {
				move_scroll(1, (scroll + snum)->x1 - iscroll[snum].map2_x, (scroll + snum)->y1 - iscroll[snum].map2_y);
			}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Scroll simple
//════════════════════════════════════════════════════════════════════════════

void scroll_simple(void) {
	int n, b;
#ifdef DEBUG
	int oclock = get_ticks();
#endif
	byte *di, *si;

	advance_scroll(0);

	si = iscroll[snum].sscr1;
	b = iscroll[snum].block1;
	di = copy_buffer + iscroll[snum].x + iscroll[snum].y * vga_width;

	for (n = 0; n < iscroll[snum].h; n++) {
		memcpy(di, si, iscroll[snum].w);
		di += vga_width;
		if (--b == 0)
			si -= iscroll[snum].w * (iscroll[snum].h - 1);
		else
			si += iscroll[snum].w;
	}
#ifdef DEBUG
	function_exec(252, get_ticks() - oclock);
#endif
	draw_sprites_scroll();
}

//════════════════════════════════════════════════════════════════════════════
// Scroll parallax
//════════════════════════════════════════════════════════════════════════════

void scroll_parallax(void) {
	int n, width, b1, b2;
#ifdef DEBUG
	int oclock = get_ticks();
#endif
	int nt, scan;
	byte *di, *si1, *si2;

	advance_scroll(1); // Moves the 2 scroll planes (auto or manually)

	si1 = iscroll[snum].sscr1;
	si2 = iscroll[snum].sscr2;
	b1 = iscroll[snum].block1;
	b2 = iscroll[snum].block2;
	di = copy_buffer + iscroll[snum].x + iscroll[snum].y * vga_width;
	fast = iscroll[snum].fast;

	scan = iscroll[snum].iscan;

	for (n = 0; n < iscroll[snum].h; n++) {
		if (fast[scan].nt >= max_inc) {
			width = iscroll[snum].w;
			do {
				if (*si1)
					*di = *si1;
				else
					*di = *si2;
				di++;
				si1++;
				si2++;
			} while (--width);
		} else {
			nt = 0;
			do {
				if (fast[scan].inc[nt]) {
					if (nt & 1) {
						memcpy(di, si1, fast[scan].inc[nt]);
					} else {
						memcpy(di, si2, fast[scan].inc[nt]);
					}
					di += fast[scan].inc[nt];
					si1 += fast[scan].inc[nt];
					si2 += fast[scan].inc[nt];
				}
			} while (nt++ < fast[scan].nt);
		}
		if (++scan == iscroll[snum].h)
			scan = 0;
		di += vga_width - iscroll[snum].w;
		if (--b1 == 0)
			si1 -= iscroll[snum].w * iscroll[snum].h;
		if (--b2 == 0)
			si2 -= iscroll[snum].w * iscroll[snum].h;
	}
#ifdef DEBUG
	function_exec(252, get_ticks() - oclock);
#endif
	draw_sprites_scroll();
}

//════════════════════════════════════════════════════════════════════════════
//      Set_scroll(plane,x,y) - snum
//════════════════════════════════════════════════════════════════════════════

void set_scroll(int plane, int x, int y) {
	int width, height, iscan;
	int _nx, nx, ny;
	byte *di, *si, *_si;
	int kk, nt, isdata;

	if (snum < 0 || snum > 9)
		return;

	if (plane < 0 || plane >= iscroll[snum].on)
		return;

	if (plane == 0) {
		di = iscroll[snum].sscr1 = iscroll[snum]._sscr1;
		iscroll[snum].block1 = iscroll[snum].h;
		if (!(iscroll[snum].map_flags & 1)) {
			if (x < 0)
				x = 0;
			else if (x + iscroll[snum].w >= iscroll[snum].map1_width)
				x = iscroll[snum].map1_width - iscroll[snum].w;
		}
		if (!(iscroll[snum].map_flags & 2)) {
			if (y < 0)
				y = 0;
			else if (y + iscroll[snum].h >= iscroll[snum].map1_height)
				y = iscroll[snum].map1_height - iscroll[snum].h;
		}
		iscroll[snum].map1_x = x;
		iscroll[snum].map1_y = y;
		width = iscroll[snum].map1_width;
		height = iscroll[snum].map1_height;
		si = iscroll[snum].map1;
	} else {
		di = iscroll[snum].sscr2 = iscroll[snum]._sscr2;
		iscroll[snum].block2 = iscroll[snum].h;
		if (!(iscroll[snum].map_flags & 4)) {
			if (x < 0)
				x = 0;
			else if (x + iscroll[snum].w >= iscroll[snum].map2_width)
				x = iscroll[snum].map2_width - iscroll[snum].w;
		}
		if (!(iscroll[snum].map_flags & 8)) {
			if (y < 0)
				y = 0;
			else if (y + iscroll[snum].h >= iscroll[snum].map2_height)
				y = iscroll[snum].map2_height - iscroll[snum].h;
		}
		iscroll[snum].map2_x = x;
		iscroll[snum].map2_y = y;
		width = iscroll[snum].map2_width;
		height = iscroll[snum].map2_height;
		si = iscroll[snum].map2;
	}

	if (x < 0)
		x = (width - 1) - ((-x - 1) % width);
	else
		x = x % width;

	if (y < 0)
		y = (height - 1) - ((-y - 1) % height);
	else
		y = y % height;

	si += y * width + x;

	// Has (di,iscroll[snum].w/h) < (si,x,y,w,h)

	/* This optimization was removed
  if (x+iscroll[snum].width<=width && y+iscroll[snum].height<=height) {
    y=iscroll[snum].height; do {
      memcpy(di,si,iscroll[snum].width);
      di+=iscroll[snum].width; si+=width;
    } while (--y);
  } else {
*/

	if (iscroll[snum].on == 2 && plane == 0) {
		fast = iscroll[snum].fast;
		ny = height - y;
		_nx = width - x;
		y = iscroll[snum].h;
		iscan = 0;
		do {
			_si = si;
			nx = _nx;
			x = iscroll[snum].w;
			nt = 0; // number of stretches
			fast[iscan].inc[0] = 0;
			isdata = 0;
			do {
				if (kk = *si++) {
					if (isdata) {
						if (nt < max_inc)
							fast[iscan].inc[nt]++;
					} else {
						nt++;
						isdata = 1;
						if (nt < max_inc)
							fast[iscan].inc[nt] = 1;
					}
				} else {
					if (isdata) {
						nt++;
						isdata = 0;
						if (nt < max_inc)
							fast[iscan].inc[nt] = 1;
					} else {
						if (nt < max_inc)
							fast[iscan].inc[nt]++;
					}
				}
				*di++ = kk;
				if (--nx == 0) {
					si -= width;
					nx = width;
				}
			} while (--x);
			fast[iscan].nt = nt;
			iscan++;
			si = _si + width;
			if (--ny == 0) {
				si -= width * height;
				ny = height;
			}
		} while (--y);
		iscroll[snum].iscan = 0;
	} else {
		ny = height - y;
		_nx = width - x;
		y = iscroll[snum].h;
		do {
			_si = si;
			nx = _nx;
			x = iscroll[snum].w;
			do {
				kk = *si++;
				*di++ = kk;
				if (--nx == 0) {
					si -= width;
					nx = width;
				}
			} while (--x);
			si = _si + width;
			if (--ny == 0) {
				si -= width * height;
				ny = height;
			}
		} while (--y);
	}

	(scroll + snum)->x0 = iscroll[snum].map1_x;
	(scroll + snum)->y0 = iscroll[snum].map1_y;
	(scroll + snum)->x1 = iscroll[snum].map2_x;
	(scroll + snum)->y1 = iscroll[snum].map2_y;
}

//════════════════════════════════════════════════════════════════════════════
// Recalculates list of increments for a scan (0..vga_height-1)
//════════════════════════════════════════════════════════════════════════════

void process_scan(int n) { // Reprocesses a whole scan
	int in;                // index of fast[in].{nt,inc[]}
	byte *si;
	int minus, width;
	int nt = 0, isdata = 0;

	if ((in = iscroll[snum].iscan + n) >= iscroll[snum].h)
		in -= iscroll[snum].h;
	if (n >= iscroll[snum].block1)
		n -= iscroll[snum].h;
	si = iscroll[snum].sscr1 + n * iscroll[snum].w;
	fast = iscroll[snum].fast;

	width = iscroll[snum].w;
	minus = iscroll[snum].map1_width;
	fast[in].inc[0] = 0;
	do {
		if (*si++) {
			if (isdata) {
				if (nt < max_inc)
					fast[in].inc[nt]++;
			} else {
				nt++;
				isdata = 1;
				if (nt < max_inc)
					fast[in].inc[nt] = 1;
			}
		} else {
			if (isdata) {
				nt++;
				isdata = 0;
				if (nt < max_inc)
					fast[in].inc[nt] = 1;
			} else {
				if (nt < max_inc)
					fast[in].inc[nt]++;
			}
		}
		if (--minus == 0) {
			si -= iscroll[snum].map1_width;
			minus = iscroll[snum].map1_width;
		}
	} while (--width);
	fast[in].nt = nt;
}

//════════════════════════════════════════════════════════════════════════════
// One pixel is pushed into all lines from the right
//════════════════════════════════════════════════════════════════════════════

void process_scan_right(void) {
	int n, in, block;
	byte *si;

	block = iscroll[snum].block1;
	si = iscroll[snum].sscr1 + iscroll[snum].w - 1; // End of first line
	in = iscroll[snum].iscan;
	n = iscroll[snum].h;
	fast = iscroll[snum].fast;

	do {

		// 1st, remove pixel on the left

		if (fast[in].inc[0]) { // If it begins with background
			fast[in].inc[0]--;
		} else {
			if (fast[in].inc[1]) { // If it begins with foreground
				fast[in].inc[1]--;
			}
			if (!fast[in].inc[1]) { // A couple of stretches finished
				fast[in].nt -= 2;
				memmove(&fast[in].inc[0], &fast[in].inc[2], (max_inc - 2) * 2);

				if (fast[in].nt < max_inc && fast[in].nt + 2 >= max_inc) {
					process_scan(iscroll[snum].h - n);
					goto cont_loop;
				}
			}
		}

		// 2nd, add pixel on the right

		if (*si) {                 // If foreground incoming
			if (fast[in].nt & 1) { // If stretch ends in foreground
				if (fast[in].nt < max_inc)
					fast[in].inc[fast[in].nt]++;
			} else { // If it ends in background
				if (++fast[in].nt < max_inc)
					fast[in].inc[fast[in].nt] = 1;
			}
		} else {                   // If background incoming
			if (fast[in].nt & 1) { // If stretch ends in foreground
				if (++fast[in].nt < max_inc)
					fast[in].inc[fast[in].nt] = 1;
			} else { // If it ends in background
				if (fast[in].nt < max_inc)
					fast[in].inc[fast[in].nt]++;
			}
		}

	cont_loop:

		if (!--block)
			si -= iscroll[snum].w * (iscroll[snum].h - 1);
		else
			si += iscroll[snum].w;

		if (++in == iscroll[snum].h)
			in = 0;

	} while (--n);
}

//════════════════════════════════════════════════════════════════════════════
// Push a pixel into all lines from the left (leaving from the right)
//════════════════════════════════════════════════════════════════════════════

void process_scan_left(void) {
	int n, in, block;
	byte *si;

	block = iscroll[snum].block1;
	si = iscroll[snum].sscr1 + iscroll[snum].w - 1; // End of first line
	in = iscroll[snum].iscan;
	n = iscroll[snum].h;
	fast = iscroll[snum].fast;

	do {

		// 1st, remove pixel on the right

		if (fast[in].nt < max_inc) {
			if (!--fast[in].inc[fast[in].nt])
				fast[in].nt--;
		} else {                   // Complex case
			if (fast[in].nt & 1) { // If line ended in foreground ...
				if (!*si) {        // ... and now ends in background
					if (--fast[in].nt == max_inc - 1) {
						process_scan(iscroll[snum].h - n);
						goto cont_loop;
					}
				}
			} else {       // If line ended in background ...
				if (*si) { // ... and now ends in foreground
					if (--fast[in].nt == max_inc - 1) {
						process_scan(iscroll[snum].h - n);
						goto cont_loop;
					}
				}
			}
		}

		// 2nd, add pixel on the left

		if (*(si - iscroll[snum].w + 1)) { // If foreground incoming
			if (!fast[in].inc[0]) {         // If stretch begins in foreground
				fast[in].inc[1]++;
			} else { // If it begins in background
				fast[in].nt += 2;
				if (fast[in].nt < max_inc) {
					memmove(&fast[in].inc[2], &fast[in].inc[0], (max_inc - 2) * 2);
					fast[in].inc[0] = 0;
					fast[in].inc[1] = 1;
				}
			}
		} else { // If background incoming
			fast[in].inc[0]++;
		}

	cont_loop:

		if (!--block)
			si -= iscroll[snum].w * (iscroll[snum].h - 1);
		else
			si += iscroll[snum].w;

		if (++in == iscroll[snum].h)
			in = 0;

	} while (--n);
}

//════════════════════════════════════════════════════════════════════════════
//      Move_scroll(snum,plane,ix,iy)
//════════════════════════════════════════════════════════════════════════════

byte *scroll_right(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y);

byte *scroll_left(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y);

byte *scroll_down(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y);

byte *scroll_up(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y);

void move_scroll(int plane, int x, int y) {
	int ix, iy;

	if (snum < 0 || snum > 9)
		return;

	if (plane < 0 || plane >= iscroll[snum].on)
		return;

	if (plane == 0) {
		ix = iscroll[snum].map1_x;
		iy = iscroll[snum].map1_y;
		x += iscroll[snum].map1_x;
		y += iscroll[snum].map1_y;
		if (!(iscroll[snum].map_flags & 1)) {
			if (x < 0)
				x = 0;
			else if (x + iscroll[snum].w >= iscroll[snum].map1_width)
				x = iscroll[snum].map1_width - iscroll[snum].w;
		}
		if (!(iscroll[snum].map_flags & 2)) {
			if (y < 0)
				y = 0;
			else if (y + iscroll[snum].h >= iscroll[snum].map1_height)
				y = iscroll[snum].map1_height - iscroll[snum].h;
		}
	} else {
		ix = iscroll[snum].map2_x;
		iy = iscroll[snum].map2_y;
		x += iscroll[snum].map2_x;
		y += iscroll[snum].map2_y;
		if (!(iscroll[snum].map_flags & 4)) {
			if (x < 0)
				x = 0;
			else if (x + iscroll[snum].w >= iscroll[snum].map2_width)
				x = iscroll[snum].map2_width - iscroll[snum].w;
		}
		if (!(iscroll[snum].map_flags & 8)) {
			if (y < 0)
				y = 0;
			else if (y + iscroll[snum].h >= iscroll[snum].map2_height)
				y = iscroll[snum].map2_height - iscroll[snum].h;
		}
	}

	ix = x - ix;
	iy = y - iy;

	/////////////////////////////////////////////////////////////////////
	if (abs(ix) > iscroll[snum].w / 2 || abs(iy) > iscroll[snum].h / 2) {
		if (plane == 0) {
			ix += iscroll[snum].map1_x;
			iy += iscroll[snum].map1_y;
		} else {
			ix += iscroll[snum].map2_x;
			iy += iscroll[snum].map2_y;
		}
		set_scroll(plane, ix, iy);
		return;
	}
	/////////////////////////////////////////////////////////////////////

	if (plane == 0) {
		iscroll[snum].map1_x = x;
		iscroll[snum].map1_y = y;
	} else {
		iscroll[snum].map2_x = x;
		iscroll[snum].map2_y = y;
	}

	x -= ix;
	y -= iy;

	if (ix > 0)
		do {
			if (plane == 0) {
				iscroll[snum].sscr1 = scroll_right(iscroll[snum]._sscr1,
				                                   iscroll[snum].sscr1,
				                                   &iscroll[snum].block1,
				                                   iscroll[snum].map1,
				                                   iscroll[snum].map1_width,
				                                   iscroll[snum].map1_height,
				                                   x++,
				                                   y);
				if (iscroll[snum].on == 2) {
					process_scan_right(); // Push a scan from the right (exits from the left)
				}
			} else {
				iscroll[snum].sscr2 = scroll_right(iscroll[snum]._sscr2,
				                                   iscroll[snum].sscr2,
				                                   &iscroll[snum].block2,
				                                   iscroll[snum].map2,
				                                   iscroll[snum].map2_width,
				                                   iscroll[snum].map2_height,
				                                   x++,
				                                   y);
			}
		} while (--ix);

	if (ix < 0)
		do {
			if (plane == 0) {
				iscroll[snum].sscr1 = scroll_left(iscroll[snum]._sscr1,
				                                  iscroll[snum].sscr1,
				                                  &iscroll[snum].block1,
				                                  iscroll[snum].map1,
				                                  iscroll[snum].map1_width,
				                                  iscroll[snum].map1_height,
				                                  x--,
				                                  y);
				if (iscroll[snum].on == 2) {
					process_scan_left(); // Push a scan from the left (exits from the right)
				}
			} else {
				iscroll[snum].sscr2 = scroll_left(iscroll[snum]._sscr2,
				                                  iscroll[snum].sscr2,
				                                  &iscroll[snum].block2,
				                                  iscroll[snum].map2,
				                                  iscroll[snum].map2_width,
				                                  iscroll[snum].map2_height,
				                                  x--,
				                                  y);
			}
		} while (++ix);

	if (iy > 0)
		do {
			if (plane == 0) {
				iscroll[snum].sscr1 = scroll_down(iscroll[snum]._sscr1,
				                                  iscroll[snum].sscr1,
				                                  &iscroll[snum].block1,
				                                  iscroll[snum].map1,
				                                  iscroll[snum].map1_width,
				                                  iscroll[snum].map1_height,
				                                  x,
				                                  y++);
				if (iscroll[snum].on == 2) {
					if (++iscroll[snum].iscan == iscroll[snum].h)
						iscroll[snum].iscan = 0;
					process_scan(iscroll[snum].h - 1);
				}
			} else {
				iscroll[snum].sscr2 = scroll_down(iscroll[snum]._sscr2,
				                                  iscroll[snum].sscr2,
				                                  &iscroll[snum].block2,
				                                  iscroll[snum].map2,
				                                  iscroll[snum].map2_width,
				                                  iscroll[snum].map2_height,
				                                  x,
				                                  y++);
			}
		} while (--iy);

	if (iy < 0)
		do {
			if (plane == 0) {
				iscroll[snum].sscr1 = scroll_up(iscroll[snum]._sscr1,
				                                iscroll[snum].sscr1,
				                                &iscroll[snum].block1,
				                                iscroll[snum].map1,
				                                iscroll[snum].map1_width,
				                                iscroll[snum].map1_height,
				                                x,
				                                y--);
				if (iscroll[snum].on == 2) {
					if (iscroll[snum].iscan-- == 0)
						iscroll[snum].iscan = iscroll[snum].h - 1;
					process_scan(0);
				}
			} else {
				iscroll[snum].sscr2 = scroll_up(iscroll[snum]._sscr2,
				                                iscroll[snum].sscr2,
				                                &iscroll[snum].block2,
				                                iscroll[snum].map2,
				                                iscroll[snum].map2_width,
				                                iscroll[snum].map2_height,
				                                x,
				                                y--);
			}
		} while (++iy);

	if (plane) {
		(scroll + snum)->x1 = iscroll[snum].map2_x;
		(scroll + snum)->y1 = iscroll[snum].map2_y;
	} else {
		(scroll + snum)->x0 = iscroll[snum].map1_x;
		(scroll + snum)->y0 = iscroll[snum].map1_y;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Unidirectional scrolling functions
//════════════════════════════════════════════════════════════════════════════

byte *scroll_right(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y) {

	// Scrolling window (_buf,iscroll[snum].an,iscroll[snum].height(+1))
	// Number of scans in first block (*block)
	// Pointer to current scroll start (buf)
	// Map (map,an,al)
	// scroll start coordinates(x,y)

	int n, m, b;
	byte *ret;

	x += iscroll[snum].w;

	if (x < 0)
		x = (width - 1) - ((-x - 1) % width);
	else
		x = x % width;
	if (y < 0)
		y = (height - 1) - ((-y - 1) % height);
	else
		y = y % height;

	m = height - y;
	map += y * width + x; // 1st pixel in map

	buf++;
	if (buf == _buf + iscroll[snum].w * iscroll[snum].h) {
		memcpy(_buf, _buf + iscroll[snum].w * iscroll[snum].h, iscroll[snum].w);
		buf = _buf;
		*block = iscroll[snum].h;
	} else if (((int)(buf - _buf) % iscroll[snum].w) == 0) {
		memcpy(_buf, _buf + iscroll[snum].w * iscroll[snum].h, iscroll[snum].w);
		(*block)--;
	}

	ret = buf;

	buf += iscroll[snum].w - 1;
	b = *block;

	n = iscroll[snum].h;
	do {
		*buf = *map;
		if (--b == 0)
			buf -= iscroll[snum].w * (iscroll[snum].h - 1);
		else
			buf += iscroll[snum].w;
		if (--m == 0) {
			map -= width * (height - 1);
			m = height;
		} else
			map += width;
	} while (--n);

	return (ret);
}

byte *scroll_left(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y) {
	int n, m, b;
	byte *ret;

	x--;

	if (x < 0)
		x = (width - 1) - ((-x - 1) % width);
	else
		x = x % width;
	if (y < 0)
		y = (height - 1) - ((-y - 1) % height);
	else
		y = y % height;

	m = height - y;
	map += y * width + x; // 1st pixel in map

	if (buf == _buf) {
		memcpy(_buf + iscroll[snum].w * iscroll[snum].h, _buf, iscroll[snum].w);
		buf = _buf + iscroll[snum].w * iscroll[snum].h - 1;
		*block = 1;
	} else if (((int)(buf - _buf) % iscroll[snum].w) == 0) {
		buf--;
		memcpy(_buf + iscroll[snum].w * iscroll[snum].h, _buf, iscroll[snum].w);
		(*block)++;
	} else
		buf--;

	ret = buf;

	b = *block;

	n = iscroll[snum].h;
	do {
		*buf = *map;
		if (--b == 0)
			buf -= iscroll[snum].w * (iscroll[snum].h - 1);
		else
			buf += iscroll[snum].w;
		if (--m == 0) {
			map -= width * (height - 1);
			m = height;
		} else
			map += width;
	} while (--n);

	return (ret);
}

byte *scroll_down(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y) {
	int n, m;
	byte *ret;

	y += iscroll[snum].h;

	if (x < 0)
		x = (width - 1) - ((-x - 1) % width);
	else
		x = x % width;
	if (y < 0)
		y = (height - 1) - ((-y - 1) % height);
	else
		y = y % height;

	m = width - x;
	map += y * width + x; // 1st pixel in map

	buf += iscroll[snum].w;
	if (buf >= _buf + iscroll[snum].w * iscroll[snum].h) {
		buf -= iscroll[snum].w * iscroll[snum].h;
		*block = iscroll[snum].h;
	} else
		(*block)--;

	ret = buf;

	buf += (iscroll[snum].h - 1) * iscroll[snum].w;
	if (buf >= _buf + iscroll[snum].w * iscroll[snum].h)
		buf -= iscroll[snum].w * iscroll[snum].h;

	n = iscroll[snum].w;
	do {
		*buf++ = *map;
		if (--m == 0) {
			map -= width - 1;
			m = width;
		} else
			map++;
	} while (--n);

	return (ret);
}

byte *scroll_up(byte *_buf, byte *buf, int *block, byte *map, int width, int height, int x, int y) {
	int n, m;
	byte *ret;

	y--;

	if (x < 0)
		x = (width - 1) - ((-x - 1) % width);
	else
		x = x % width;
	if (y < 0)
		y = (height - 1) - ((-y - 1) % height);
	else
		y = y % height;

	m = width - x;
	map += y * width + x; // 1st pixel in map

	buf -= iscroll[snum].w;
	if (buf < _buf) {
		buf += iscroll[snum].w * iscroll[snum].h;
		*block = 1;
	} else
		(*block)++;

	ret = buf;

	n = iscroll[snum].w;
	do {
		*buf++ = *map;
		if (--m == 0) {
			map -= width - 1;
			m = width;
		} else
			map++;
	} while (--n);

	return (ret);
}

//════════════════════════════════════════════════════════════════════════════
//      External function to draw sprites (for the language put)
//════════════════════════════════════════════════════════════════════════════

void put_sprite(
    int file, int graph, int x, int y, int angle, int size, int flags, int reg, byte *cop, int cop_width, int cop_height) {

	int *ptr;
	byte *si;
	int width, height; // Info on graph
	int xg, yg;
	byte *_copy = copy_buffer;
	int _vga_width = vga_width, _vga_height = vga_height;

	if (file > max_fpgs || file < 0) {
		e(109);
		return;
	}
	if (file)
		max_grf = 1000;
	else
		max_grf = 2000;
	if (graph <= 0 || graph >= max_grf) {
		e(110);
		return;
	}
	if (g[file].grf == NULL) {
		e(111);
		return;
	}

	if (reg < 0 || reg >= max_region) {
		clipx0 = 0;
		clipx1 = cop_width;
		clipy0 = 0;
		clipy1 = cop_height;
	} else {
		clipx0 = region[reg].x0;
		clipx1 = region[reg].x1;
		clipy0 = region[reg].y0;
		clipy1 = region[reg].y1;
		if (clipx0 < 0)
			clipx0 = 0;
		if (clipx1 > cop_width)
			clipx1 = cop_width;
		if (clipx0 >= cop_width || clipx1 <= 0)
			return;
		if (clipy0 < 0)
			clipy0 = 0;
		if (clipy1 > cop_height)
			clipy1 = cop_height;
		if (clipy0 >= cop_height || clipy1 <= 0)
			return;
	}

	if ((ptr = g[file].grf[graph]) != NULL) {

		copy_buffer = cop;
		vga_width = cop_width;
		vga_height = cop_height;

		width = ptr[13];
		height = ptr[14];
		si = (byte *)ptr + 64 + ptr[15] * 4;

		if (ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] / 2;
		} else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		if (angle) {
			sp_rotated(si, x, y, width, height, xg, yg, angle, size, flags);
		} else if (size != 100) {
			sp_scaled(si, x, y, width, height, xg, yg, size, flags);
		} else {
			if (flags & 1)
				xg = width - 1 - xg;
			x -= xg;
			if (flags & 2)
				yg = height - 1 - yg;
			y -= yg;
			if (x >= clipx0 && x + width <= clipx1 && y >= clipy0 && y + height <= clipy1) // Draw sprite without clipping
				sp_normal(si, x, y, width, height, flags);
			else if (x < clipx1 && y < clipy1 && x + width > clipx0 && y + height > clipy0) // Draw clipped sprite
				sp_clipped(si, x, y, width, height, flags);
			x0s = x;
			x1s = x + width - 1;
			y0s = y;
			y1s = y + height - 1;
		}

		copy_buffer = _copy;
		vga_width = _vga_width;
		vga_height = _vga_height;

	} else
		e(121);
}

//════════════════════════════════════════════════════════════════════════════
//                Function that draws process sprites
//════════════════════════════════════════════════════════════════════════════

void draw_sprite(void) { // Draws a sprite (if visible), according to mem[ide+ ... ]

	int *ptr;
	byte *si;
	int x, y, width, height; // Info on graph
	int xg, yg;
	int n, m;

	if (mem[ide + _File] > max_fpgs || mem[ide + _File] < 0)
		return;
	if ((n = mem[ide + _XGraph]) > 0) {
		m = mem[n];
		if (m < 1 || m > 256)
			return;
		while (mem[ide + _Angle] >= 2 * pi)
			mem[ide + _Angle] -= 2 * pi;
		while (mem[ide + _Angle] < 0)
			mem[ide + _Angle] += 2 * pi;
		mem[ide + _Flags] &= 254;
		mem[ide + _Graph] = ((mem[ide + _Angle] + (2 * pi) / (m * 2)) * m) / (2 * pi);
		if (mem[ide + _Graph] >= m)
			mem[ide + _Graph] = 0;
		if ((mem[ide + _Graph] = mem[n + 1 + mem[ide + _Graph]]) < 0) {
			mem[ide + _Graph] = -mem[ide + _Graph];
			mem[ide + _Flags] |= 1;
		}
	}
	if (mem[ide + _File])
		max_grf = 1000;
	else
		max_grf = 2000;
	if (mem[ide + _Graph] <= 0 || mem[ide + _Graph] >= max_grf)
		return;
	if (g[mem[ide + _File]].grf == NULL)
		return;

	x = mem[ide + _Region];

	if (mem[ide + _Ctype] == 1 && iscroll[snum].on > 0) {
		clipx0 = iscroll[snum].x;
		clipx1 = iscroll[snum].x + iscroll[snum].w;
		clipy0 = iscroll[snum].y;
		clipy1 = iscroll[snum].y + iscroll[snum].h;
	} else if (x < 0 || x >= max_region) {
		clipx0 = 0;
		clipx1 = vga_width;
		clipy0 = 0;
		clipy1 = vga_height;
	} else {
		clipx0 = region[x].x0;
		clipx1 = region[x].x1;
		clipy0 = region[x].y0;
		clipy1 = region[x].y1;
	}

	if ((ptr = g[mem[ide + _File]].grf[mem[ide + _Graph]]) != NULL) {

		x = mem[ide + _X];
		y = mem[ide + _Y];
		if (mem[ide + _Resolution] > 0) {
			x /= mem[ide + _Resolution];
			y /= mem[ide + _Resolution];
		}

		if (mem[ide + _Ctype] == 1) {
			x += iscroll[snum].x - iscroll[snum].map1_x;
			y += iscroll[snum].y - iscroll[snum].map1_y;
		}

		width = ptr[13];
		height = ptr[14];
		si = (byte *)ptr + 64 + ptr[15] * 4;

		if (ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] / 2;
		} else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		if (putsprite != NULL) {
			putsprite(si, x, y, width, height, xg, yg, mem[ide + _Angle], mem[ide + _Size], mem[ide + _Flags]);
		} else if (mem[ide + _Angle] && mem[ide + _XGraph] <= 0) {
			sp_rotated(si, x, y, width, height, xg, yg, mem[ide + _Angle], mem[ide + _Size], mem[ide + _Flags]);
			save_region();
		} else if (mem[ide + _Size] != 100) {
			sp_scaled(si, x, y, width, height, xg, yg, mem[ide + _Size], mem[ide + _Flags]);
			save_region();
		} else {
			if (mem[ide + _Flags] & 1)
				xg = width - 1 - xg;
			x -= xg;
			if (mem[ide + _Flags] & 2)
				yg = height - 1 - yg;
			y -= yg;
			if (x >= clipx0 && x + width <= clipx1 && y >= clipy0 && y + height <= clipy1) // Draw sprite without clipping
				sp_normal(si, x, y, width, height, mem[ide + _Flags]);
			else if (x < clipx1 && y < clipy1 && x + width > clipx0 && y + height > clipy0) // Draw clipped sprite
				sp_clipped(si, x, y, width, height, mem[ide + _Flags]);
			x0s = x;
			x1s = x + width - 1;
			y0s = y;
			y1s = y + height - 1;
			save_region();
		}
	}
}

void save_region(void) {
	if (mem[ide + _Ctype] == 0) {
		if (clipx0 > x0s)
			x0s = clipx0;
		if (clipx1 < x1s)
			x1s = clipx1;
		if (clipy0 > y0s)
			y0s = clipy0;
		if (clipy1 < y1s)
			y1s = clipy1;
		if (x1s < x0s || y1s < y0s)
			return;
		mem[ide + _x0] = x0s;
		mem[ide + _y0] = y0s;
		mem[ide + _x1] = x1s;
		mem[ide + _y1] = y1s;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - [mirrored] [ghost]
//════════════════════════════════════════════════════════════════════════════

void sp_normal(byte *p, int x, int y, int width, int height, int flags) {

	byte *q = copy_buffer + y * vga_width + x;
	int orig_width = width;

	switch (flags & 7) {
		case 0: //--
			do {
				do {
					if (*p)
						*q = *p;
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
			} while (--height);
			break;
		case 1: //h-
			p += width - 1;
			do {
				do {
					if (*p)
						*q = *p;
					p--;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
				p += width * 2;
			} while (--height);
			break;
		case 2: //-v
			p += (height - 1) * width;
			do {
				do {
					if (*p)
						*q = *p;
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
				p -= width * 2;
			} while (--height);
			break;
		case 3: //hv
			p += height * width - 1;
			do {
				do {
					if (*p)
						*q = *p;
					p--;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
			} while (--height);
			break;
		case 4: //-- Ghost
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
			} while (--height);
			break;
		case 5: //h- Ghost
			p += width - 1;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p--;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
				p += width * 2;
			} while (--height);
			break;
		case 6: //-v Ghost
			p += (height - 1) * width;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
				p -= width * 2;
			} while (--height);
			break;
		case 7: //hv Ghost
			p += height * width - 1;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p--;
					q++;
				} while (--width);
				q += vga_width - (width = orig_width);
			} while (--height);
			break;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - clipped [mirrored] [ghost]
//════════════════════════════════════════════════════════════════════════════

void sp_clipped(byte *p, int x, int y, int width, int height, int flags) {

	byte *q = copy_buffer + y * vga_width + x;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (x < clipx0)
		skip_x = clipx0 - x;
	else
		skip_x = 0;
	if (x + width > clipx1)
		rest_x = x + width - clipx1;
	else
		rest_x = 0;
	long_x = width - skip_x - rest_x;

	if (y < clipy0)
		skip_y = clipy0 - y;
	else
		skip_y = 0;
	if (y + height > clipy1)
		rest_y = y + height - clipy1;
	else
		rest_y = 0;
	long_y = height - skip_y - rest_y;

	switch (flags & 7) {
		case 0: //--
			p += width * skip_y + skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			width = long_x;
			do {
				do {
					if (*p)
						*q = *p;
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = long_x);
				p += rest_x;
			} while (--long_y);
			break;
		case 1: //h-
			p += width * skip_y + width - 1 - skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					if (*p)
						*q = *p;
					p--;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p += width + long_x;
			} while (--long_y);
			break;
		case 2: //-v
			p += (height - 1) * width - width * skip_y + skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					if (*p)
						*q = *p;
					p++;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p += rest_x - width * 2;
			} while (--long_y);
			break;
		case 3: //hv
			p += height * width - 1 - width * skip_y - skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					if (*p)
						*q = *p;
					p--;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p -= rest_x;
			} while (--long_y);
			break;
		case 4: //-- Ghost
			p += width * skip_y + skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			width = long_x;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p++;
					q++;
				} while (--width);
				q += vga_width - (width = long_x);
				p += rest_x;
			} while (--long_y);
			break;
		case 5: //h- Ghost
			p += width * skip_y + width - 1 - skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p--;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p += width + long_x;
			} while (--long_y);
			break;
		case 6: //-v Ghost
			p += (height - 1) * width - width * skip_y + skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p++;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p += rest_x - width * 2;
			} while (--long_y);
			break;
		case 7: //hv Ghost
			p += height * width - 1 - width * skip_y - skip_x;
			q += vga_width * skip_y + skip_x;
			rest_x += skip_x;
			skip_x = long_x;
			do {
				do {
					*q = ghost[(*p << 8) + *q];
					p--;
					q++;
				} while (--skip_x);
				q += vga_width - (skip_x = long_x);
				p -= rest_x;
			} while (--long_y);
			break;
	}
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - scaled [clipped] [mirrored] [ghost]
//════════════════════════════════════════════════════════════════════════════

void sp_scaled(byte *old_si, int x, int y, int width, int height, int xg, int yg, int size, int flags) {

	int skip_x, long_x, rest_x; // Regarding screen
	int skip_y, long_y, rest_y;
	int xr, ixr, yr, iyr, old_xr, old_width;
	byte *si, *di, c;

	if (flags & 1)
		x0s = x - ((width - 1 - xg) * size) / 100;
	else
		x0s = x - (xg * size) / 100;
	if (flags & 2)
		y0s = y - ((height - 1 - yg) * size) / 100;
	else
		y0s = y - (yg * size) / 100;
	x1s = x0s + (width * size) / 100 - 1;
	y1s = y0s + (height * size) / 100 - 1;

	if (x1s < x0s || y1s < y0s)
		return;

	ixr = (float)(width * 256) / (float)(x1s - x0s + 1);
	iyr = (float)(height * 256) / (float)(y1s - y0s + 1);

	if (x1s < clipx0 || y1s < clipy0 || x0s >= clipx1 || y0s >= clipy1)
		return;

	di = copy_buffer + y0s * vga_width + x0s;

	if (x0s < clipx0)
		skip_x = clipx0 - x0s;
	else
		skip_x = 0;
	if (x1s >= clipx1)
		rest_x = x1s - clipx1 + 1;
	else
		rest_x = 0;
	long_x = (width * size) / 100 - skip_x - rest_x;

	if (y0s < clipy0)
		skip_y = clipy0 - y0s;
	else
		skip_y = 0;
	if (y1s >= clipy1)
		rest_y = y1s - clipy1 + 1;
	else
		rest_y = 0;
	long_y = (height * size) / 100 - skip_y - rest_y;

	if (flags & 1) {
		xr = width * 256 - skip_x * ixr - 1;
		ixr = -ixr;
	} else
		xr = skip_x * ixr;
	if (flags & 2) {
		yr = height * 256 - skip_y * iyr - 1;
		iyr = -iyr;
	} else
		yr = skip_y * iyr;

	old_xr = xr;
	old_width = width;
	di += vga_width * skip_y + skip_x;
	width = long_x;

	if (flags & 4)
		do {
			si = old_si + (yr >> 8) * old_width;
			do {
				c = *(si + (xr >> 8));
				*di = ghost[(c << 8) + *di];
				di++;
				xr += ixr;
			} while (--width);
			yr += iyr;
			xr = old_xr;
			di += vga_width - (width = long_x);
		} while (--long_y);
	else
		do {
			si = old_si + (yr >> 8) * old_width;
			do {
				if (c = *(si + (xr >> 8)))
					*di = c;
				di++;
				xr += ixr;
			} while (--width);
			yr += iyr;
			xr = old_xr;
			di += vga_width - (width = long_x);
		} while (--long_y);
}

//════════════════════════════════════════════════════════════════════════════
// Sprite - rotated [scaled] [clipped] [mirrored] [ghost]
//════════════════════════════════════════════════════════════════════════════

void sp_rotated(byte *si, int x, int y, int width, int height, int xg, int yg, int ang, int size, int flags) {

	float d0, d1, d2, d3;
	float a0, a1, a2, a3;
	float a, s;
	int p[24];
	int h, hmin, hmax; // Min/max height
	int n, l0, l1;     // Side 0 and side 1 (indices into p[])

	int hmax0, hmax1;
	union {
		int32_t l;
		int16_t w[2];
	} x0, x1, g0x, g1x, g0y, g1y;
	int ix0, ix1, ig0x, ig1x, ig0y, ig1y, kk;
	byte *ptrcopy;

	a = (float)ang / radian;
	s = (float)size / 100;
	xg = -xg;
	yg = -yg;

	d0 = (float)sqrt(xg * xg + yg * yg) * s;
	if (yg == 0 && xg == 0)
		a0 = a;
	else
		a0 = a + (float)atan2(-yg, xg);
	xg = xg + width - 1;
	d1 = (float)sqrt(xg * xg + yg * yg) * s;
	if (yg == 0 && xg == 0)
		a1 = a;
	else
		a1 = a + (float)atan2(-yg, xg);
	yg = yg + height - 1;
	d2 = (float)sqrt(xg * xg + yg * yg) * s;
	if (yg == 0 && xg == 0)
		a2 = a;
	else
		a2 = a + (float)atan2(-yg, xg);
	xg = xg - width + 1;
	d3 = (float)sqrt(xg * xg + yg * yg) * s;
	if (yg == 0 && xg == 0)
		a3 = a;
	else
		a3 = a + (float)atan2(-yg, xg);

	if (flags & 1) {
		p[0] = x - (int)((float)cos(a0) * d0);
		p[2] = x - (int)((float)cos(a1) * d1);
		p[4] = x - (int)((float)cos(a2) * d2);
		p[6] = x - (int)((float)cos(a3) * d3);
	} else {
		p[0] = x + (int)((float)cos(a0) * d0);
		p[2] = x + (int)((float)cos(a1) * d1);
		p[4] = x + (int)((float)cos(a2) * d2);
		p[6] = x + (int)((float)cos(a3) * d3);
	}

	if (flags & 2) {
		p[1] = y - (int)(-(float)sin(a0) * d0);
		p[3] = y - (int)(-(float)sin(a1) * d1);
		p[5] = y - (int)(-(float)sin(a2) * d2);
		p[7] = y - (int)(-(float)sin(a3) * d3);
	} else {
		p[1] = y + (int)(-(float)sin(a0) * d0);
		p[3] = y + (int)(-(float)sin(a1) * d1);
		p[5] = y + (int)(-(float)sin(a2) * d2);
		p[7] = y + (int)(-(float)sin(a3) * d3);
	}

	memcpy(&p[8], &p[0], sizeof(p[0]) * 8);
	memcpy(&p[16], &p[0], sizeof(p[0]) * 8);

	x0s = hmin = 32767;
	x1s = hmax = -32768;
	n = 8;
	do {
		if (p[n] < x0s)
			x0s = p[n];
		if (p[n] > x1s)
			x1s = p[n];
		if (p[n + 1] < hmin) {
			l0 = n;
			hmin = p[n + 1];
		}
		if (p[n + 1] > hmax)
			hmax = p[n + 1];
		n += 2;
	} while (n < 16);
	y0s = hmin;
	y1s = hmax;

	l1 = l0;
	hmax0 = hmax1 = hmin;
	ptrcopy = copy_buffer + hmin * vga_width;

	h = hmin;
	do {

		if (h < hmax) {
			while (h == hmax0) {
				if ((hmax0 = p[l0 - 1]) != h) {
					x0.l = p[l0] << 16;
					ix0 = ((p[l0 - 2] << 16) - x0.l) / (hmax0 - h);
					if (ix0 < 0)
						x0.l += 65535;
					switch (l0 & 6) {
						case 0:
							g0y.l = 0;
							ig0y = (height << 16) / (hmax0 - h);
							g0x.l = 0;
							ig0x = 0;
							break;
						case 2:
							g0x.l = ((width - 1) << 16) + 65535;
							ig0x = (width << 16) / (h - hmax0);
							g0y.l = 0;
							ig0y = 0;
							break;
						case 4:
							g0y.l = ((height - 1) << 16) + 65535;
							ig0y = (height << 16) / (h - hmax0);
							g0x.l = ((width - 1) << 16) + 65535;
							ig0x = 0;
							break;
						case 6:
							g0x.l = 0;
							ig0x = (width << 16) / (hmax0 - h);
							g0y.l = ((height - 1) << 16) + 65535;
							ig0y = 0;
							break;
					}
				}
				l0 -= 2;
			}
			while (h == hmax1) {
				if ((hmax1 = p[l1 + 3]) != h) {
					x1.l = p[l1] << 16;
					ix1 = ((p[l1 + 2] << 16) - x1.l) / (hmax1 - h);
					if (ix1 < 0)
						x1.l += 65535;
					switch (l1 & 6) {
						case 0:
							g1x.l = 0;
							ig1x = (width << 16) / (hmax1 - h);
							g1y.l = 0;
							ig1y = 0;
							break;
						case 2:
							g1y.l = 0;
							ig1y = (height << 16) / (hmax1 - h);
							g1x.l = ((width - 1) << 16) + 65535;
							ig1x = 0;
							break;
						case 4:
							g1x.l = ((width - 1) << 16) + 65535;
							ig1x = (width << 16) / (h - hmax1);
							g1y.l = ((height - 1) << 16) + 65535;
							ig1y = 0;
							break;
						case 6:
							g1y.l = ((height - 1) << 16) + 65535;
							ig1y = (height << 16) / (h - hmax1);
							g1x.l = 0;
							ig1x = 0;
							break;
					}
				}
				l1 += 2;
			}
		} else {
			if (h == hmax0) {
				g0x.l = 0;
				g0y.l = 0;
				ig0x = 0;
				ig0y = 0;
				x0.l = p[l0] << 16;
				ix0 = 0;
				switch (l0 & 6) {
					case 2: g0x.l = ((width - 1) << 16) + 65535; break;
					case 4:
						g0y.l = ((height - 1) << 16) + 65535;
						g0x.l = ((width - 1) << 16) + 65535;
						break;
					case 6: g0y.l = ((height - 1) << 16) + 65535; break;
				}
			}
			if (h == hmax1) {
				g1x.l = 0;
				g1y.l = 0;
				ig1x = 0;
				ig1y = 0;
				x1.l = p[l1] << 16;
				ix1 = 0;
				switch (l1 & 6) {
					case 2: g1x.l = ((width - 1) << 16) + 65535; break;
					case 4:
						g1x.l = ((width - 1) << 16) + 65535;
						g1y.l = ((height - 1) << 16) + 65535;
						break;
					case 6: g1y.l = ((height - 1) << 16) + 65535; break;
				}
			}
		}

		if ((flags & 3) == 1 || (flags & 3) == 2) {
			kk = x0.l;
			x0.l = x1.l;
			x1.l = kk;
			kk = g0x.l;
			g0x.l = g1x.l;
			g1x.l = kk;
			kk = g0y.l;
			g0y.l = g1y.l;
			g1y.l = kk;
		}

		if (h < clipy1 && h >= clipy0 && x0.w[1] < clipx1 && x1.w[1] >= clipx0 && x1.w[1] > x0.w[1])
			if (x0.w[1] < clipx0)
				if (x1.w[1] >= clipx1)
					if (flags & 4)
						sp_scancg(ptrcopy + clipx0,
						          x1.w[1] - x0.w[1],
						          clipx1 - clipx0 - 1,
						          clipx0 - x0.w[1],
						          si,
						          width,
						          g0x.l,
						          g0y.l,
						          g1x.l,
						          g1y.l);
					else
						sp_scanc(ptrcopy + clipx0,
						         x1.w[1] - x0.w[1],
						         clipx1 - clipx0 - 1,
						         clipx0 - x0.w[1],
						         si,
						         width,
						         g0x.l,
						         g0y.l,
						         g1x.l,
						         g1y.l);
				else if (flags & 4)
					sp_scancg(ptrcopy + clipx0,
					          x1.w[1] - x0.w[1],
					          x1.w[1] - clipx0,
					          clipx0 - x0.w[1],
					          si,
					          width,
					          g0x.l,
					          g0y.l,
					          g1x.l,
					          g1y.l);
				else
					sp_scanc(ptrcopy + clipx0,
					         x1.w[1] - x0.w[1],
					         x1.w[1] - clipx0,
					         clipx0 - x0.w[1],
					         si,
					         width,
					         g0x.l,
					         g0y.l,
					         g1x.l,
					         g1y.l);
			else if (x1.w[1] >= clipx1)
				if (flags & 4)
					sp_scancg(ptrcopy + x0.w[1],
					          x1.w[1] - x0.w[1],
					          clipx1 - 1 - x0.w[1],
					          0,
					          si,
					          width,
					          g0x.l,
					          g0y.l,
					          g1x.l,
					          g1y.l);
				else
					sp_scanc(ptrcopy + x0.w[1],
					         x1.w[1] - x0.w[1],
					         clipx1 - 1 - x0.w[1],
					         0,
					         si,
					         width,
					         g0x.l,
					         g0y.l,
					         g1x.l,
					         g1y.l);
			else if (flags & 4)
				sp_scang(ptrcopy + x0.w[1], x1.w[1] - x0.w[1], si, width, g0x.l, g0y.l, g1x.l, g1y.l);
			else
				sp_scan(ptrcopy + x0.w[1], x1.w[1] - x0.w[1], si, width, g0x.l, g0y.l, g1x.l, g1y.l);

		if ((flags & 3) == 1 || (flags & 3) == 2) {
			kk = x0.l;
			x0.l = x1.l;
			x1.l = kk;
			kk = g0x.l;
			g0x.l = g1x.l;
			g1x.l = kk;
			kk = g0y.l;
			g0y.l = g1y.l;
			g1y.l = kk;
		}

		x0.l += ix0;
		x1.l += ix1;
		g0x.l += ig0x;
		g1x.l += ig1x;
		g0y.l += ig0y;
		g1y.l += ig1y;

		ptrcopy += vga_width;

	} while (h++ < hmax);
}

//════════════════════════════════════════════════════════════════════════════
// Draw a sprite rotated and scaled, with no clipping
//════════════════════════════════════════════════════════════════════════════

void sp_scanc(byte *p, short n, short m, short o, byte *si, int width, int x0, int y0, int x1, int y1) {

	union {
		int32_t l;
		int16_t w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0 = (x1 - x0) / n;
	y0 = (y1 - y0) / n;

	if (o) {
		x.l += x0 * o;
		y.l += y0 * o;
	}

	do {
		if (c = *(si + x.w[1] + y.w[1] * width))
			*p = c;
		p++;
		x.l += x0;
		y.l += y0;
	} while (m--);
}

void sp_scancg(byte *p, short n, short m, short o, byte *si, int width, int x0, int y0, int x1, int y1) {

	union {
		int32_t l;
		int16_t w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0 = (x1 - x0) / n;
	y0 = (y1 - y0) / n;

	if (o) {
		x.l += x0 * o;
		y.l += y0 * o;
	}

	do {
		c = *(si + x.w[1] + y.w[1] * width);
		*p = ghost[(c << 8) + *p];
		p++;
		x.l += x0;
		y.l += y0;
	} while (m--);
}

//════════════════════════════════════════════════════════════════════════════
// Draw a sprite rotated and scaled, with clipping
//════════════════════════════════════════════════════════════════════════════

void sp_scan(byte *p, short n, byte *si, int width, int x0, int y0, int x1, int y1) {

	union {
		int32_t l;
		int16_t w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0 = (x1 - x0) / n;
	y0 = (y1 - y0) / n;

	do {
		if (c = *(si + x.w[1] + y.w[1] * width))
			*p = c;
		p++;
		x.l += x0;
		y.l += y0;
	} while (n--);
}

void sp_scang(byte *p, short n, byte *si, int width, int x0, int y0, int x1, int y1) {

	union {
		int32_t l;
		int16_t w[2];
	} x, y;
	byte c;

	x.l = x0;
	y.l = y0;
	x0 = (x1 - x0) / n;
	y0 = (y1 - y0) / n;

	do {
		c = *(si + x.w[1] + y.w[1] * width);
		*p = ghost[(c << 8) + *p];
		p++;
		x.l += x0;
		y.l += y0;
	} while (n--);
}

//════════════════════════════════════════════════════════════════════════════
// Drawings
//════════════════════════════════════════════════════════════════════════════

void draw_drawings(void) {
	int x, y, width, height;
	int n = 0;

	do
		if (drawing[n].type) {

			if (drawing[n].x0 < drawing[n].x1) {
				x = drawing[n].x0;
				width = drawing[n].x1 - x + 1;
			} else {
				x = drawing[n].x1;
				width = drawing[n].x0 - x + 1;
			}
			if (drawing[n].y0 < drawing[n].y1) {
				y = drawing[n].y0;
				height = drawing[n].y1 - y + 1;
			} else {
				y = drawing[n].y1;
				height = drawing[n].y0 - y + 1;
			}

			clipx0 = region[drawing[n].region].x0;
			clipy0 = region[drawing[n].region].y0;
			clipx1 = region[drawing[n].region].x1;
			clipy1 = region[drawing[n].region].y1;

			if (x + width <= clipx0)
				continue;
			if (x >= clipx1)
				continue;
			if (y + height <= clipy0)
				continue;
			if (y >= clipy1)
				continue;

			line_fx = drawing[n].percentage;
			color = drawing[n].color;

			switch (drawing[n].type) {
				case 1: line(drawing[n].x0, drawing[n].y0, drawing[n].x1, drawing[n].y1); break;
				case 2: draw_rectangle(x, y, width, height); break;
				case 3: filled_box(x, y, width, height); break;
				case 4: circle(0, x, y, x + width - 1, y + height - 1); break;
				case 5: circle(1, x, y, x + width - 1, y + height - 1); break;
			}
		}
	while (++n < max_drawings);
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a box, according to rectangle_mode, line_fx and color
//═════════════════════════════════════════════════════════════════════════════

void draw_rectangle(int x, int y, int width, int height) {
	line(x, y, x + width - 1, y);
	line(x, y + height - 1, x + width - 1, y + height - 1);
	height -= 2;
	y++;
	if (height > 0)
		do {
			line_pixel(x, y);
			line_pixel(x + width - 1, y++);
		} while (--height);
}

void filled_box(int x, int y, int width, int height) {
	do {
		line(x, y, x + width - 1, y);
		y++;
	} while (--height);
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a circle (line_fx and color)
//═════════════════════════════════════════════════════════════════════════════

void circle(int fill, int x0, int y0, int x1, int y1) {

	int p[2048];   // Points in circumference
	double cx, rx; // Center and radius of circumference
	int width, height;    // Width and height
	double y, ymed, nsin;
	int n, xa, xb, ya, yb;

	width = x1 - x0 + 1;
	height = y1 - y0 + 1;
	if (height > 1024)
		return;

	rx = (double)(width - 1) / 2.0;
	cx = rx + x0 + 0.5;
	y = (double)y0 + 0.5;
	ymed = (double)(y1 + y0 + 1) / 2.0;

	n = 0;
	do {
		nsin = 1 - (y - y0) / (ymed - y0);
		xb = cx + cos(asin(nsin)) * rx;
		xa = x0 + (x1 - xb);
		ya = y;
		yb = y1 - (ya - y0);
		if (!(n && p[n - 2] == ya)) {
			p[n++] = xa;
			p[n++] = xb;
			p[n++] = ya;
			p[n++] = yb;
		}
	} while (y++ < ymed);

	if (n > 4)
		if (p[n - 1] == p[n - 6])
			n -= 4;
	if (height > 2 && width > 2 && p[0] == p[4] && p[0] + 1 < p[1]) {
		p[0] = p[4] + 1;
		p[1] = p[5] - 1;
	}
	if (height == 2) {
		p[0] = x0;
		p[1] = x1;
		p[4] = x0;
		p[5] = x1;
	}

	if (fill) {
		do {
			yb = p[--n];
			ya = p[--n];
			xb = p[--n];
			xa = p[--n];
			line(xa, ya, xb, ya);
			if (yb != ya)
				line(xa, yb, xb, yb);
		} while (n);
	} else {
		do {
			yb = p[--n];
			ya = p[--n];
			xb = p[--n];
			xa = p[--n];
			if (n) {
				if (p[n - 3] + 1 >= xb) {
					line_pixel(xa, ya);
					if (xb != xa)
						line_pixel(xb, ya);
					if (yb != ya) {
						line_pixel(xa, yb);
						if (xb != xa)
							line_pixel(xb, yb);
					}
				} else {
					line(xa, ya, p[n - 4], ya);
					line(p[n - 3] + 1, ya, xb, ya);
					if (yb != ya) {
						line(xa, yb, p[n - 4], yb);
						line(p[n - 3] + 1, yb, xb, yb);
					}
				}
			} else {
				line(xa, ya, xb, ya);
				if (yb != ya)
					line(xa, yb, xb, yb);
			}
		} while (n);
	}
}

//════════════════════════════════════════════════════════════════════════════
// Drawing primitive functions
//════════════════════════════════════════════════════════════════════════════

void line(int x0, int y0, int x1, int y1) {
	int dx, dy, a, b, d, x, y;

	if (x0 > x1) {
		x = x1;
		dx = x0 - x1;
	} else {
		x = x0;
		dx = x1 - x0;
	}
	if (y0 > y1) {
		y = y1;
		dy = y0 - y1;
	} else {
		y = y0;
		dy = y1 - y0;
	}

	if (!dx && !dy)
		line_pixel(x0, y0);
	else {
		line_pixel(x0, y0);
		if (dy <= dx) {
			if (x0 <= x1) {
				d = 2 * dy - dx;
				a = 2 * dy;
				b = 2 * (dy - dx);
				x = x0;
				y = y0;
				if (y0 <= y1)
					while (x < x1) {
						if (d <= 0) {
							d += a;
							x++;
						} else {
							d += b;
							x++;
							y++;
						}
						line_pixel(x, y);
					}
				else
					while (x < x1) {
						if (d <= 0) {
							d += a;
							x++;
						} else {
							d += b;
							x++;
							y--;
						}
						line_pixel(x, y);
					}
			} else {
				d = 2 * dy - dx;
				a = 2 * dy;
				b = 2 * (dy - dx);
				x = x0;
				y = y0;
				if (y0 <= y1)
					while (x > x1) {
						if (d <= 0) {
							d += a;
							x--;
						} else {
							d += b;
							x--;
							y++;
						}
						line_pixel(x, y);
					}
				else
					while (x > x1) {
						if (d <= 0) {
							d += a;
							x--;
						} else {
							d += b;
							x--;
							y--;
						}
						line_pixel(x, y);
					}
			}
		} else {
			if (y0 <= y1) {
				d = 2 * dx - dy;
				a = 2 * dx;
				b = 2 * (dx - dy);
				x = x0;
				y = y0;
				if (x0 <= x1)
					while (y < y1) {
						if (d <= 0) {
							d += a;
							y++;
						} else {
							d += b;
							y++;
							x++;
						}
						line_pixel(x, y);
					}
				else
					while (y < y1) {
						if (d <= 0) {
							d += a;
							y++;
						} else {
							d += b;
							y++;
							x--;
						}
						line_pixel(x, y);
					}
			} else {
				d = 2 * dx - dy;
				a = 2 * dx;
				b = 2 * (dx - dy);
				x = x0;
				y = y0;
				if (x0 <= x1)
					while (y > y1) {
						if (d <= 0) {
							d += a;
							y--;
						} else {
							d += b;
							y--;
							x++;
						}
						line_pixel(x, y);
					}
				else
					while (y > y1) {
						if (d <= 0) {
							d += a;
							y--;
						} else {
							d += b;
							y--;
							x--;
						}
						line_pixel(x, y);
					}
			}
		}
	}
}

//════════════════════════════════════════════════════════════════════════════
// Draws a pixel with opacity level set in line_fx (0..15)
//════════════════════════════════════════════════════════════════════════════

void line_pixel(int x, int y) {
	byte a, b, c, d;
	int n, color256;
	byte *p;

	if (x >= clipx0 && y >= clipy0 && x < clipx1 && y < clipy1) {

		p = copy_buffer + x + y * vga_width;

		switch (line_fx) {

			case 0:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + n + b);
				d = *(ghost + n + c);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 1:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + n + b);
				if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 2:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + n + b);
				d = *(ghost + b + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 3: // *** 25% ****
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 4:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + a + b * 256);
				d = *(ghost + b + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 5:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + a + b * 256);
				if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 6:
				n = (int)*p * 256;
				a = *(ghost + n + color);
				b = *(ghost + n + a);
				c = *(ghost + a + b * 256);
				d = *(ghost + a + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 7: // *** 50% ****
				n = (int)*p * 256;
				a = *(ghost + n + color);
				if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 8:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + a + b * 256);
				d = *(ghost + a + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 9:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + a + b * 256);
				if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 10:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + a + b * 256);
				d = *(ghost + b + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 11: // *** 75% ****
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 12:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + color256 + b);
				d = *(ghost + b + c * 256);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 13:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + color256 + b);
				if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 14:
				n = (int)*p * 256;
				color256 = color * 256;
				a = *(ghost + n + color);
				b = *(ghost + color256 + a);
				c = *(ghost + color256 + b);
				d = *(ghost + color256 + c);
				if (d != *p)
					*p = d;
				else if (c != *p)
					*p = c;
				else if (b != *p)
					*p = b;
				else if (a != *p)
					*p = a;
				else
					*p = color;
				break;

			case 15: // Put color
				*p = color;
				break;
		}
	}
}


//════════════════════════════════════════════════════════════════════════════
// Texts
//════════════════════════════════════════════════════════════════════════════

void text_out(char *ptr, int x, int y) {
	int width, height;
	byte *ptr2;

	ptr2 = (byte *)ptr;
	width = 0;
	while (*ptr2) {
		if (fnt[*ptr2].width == 0) {
			width += f_i[0].space;
			ptr2++;
		} else {
			width += fnt[*ptr2++].width;
		}
	}

	fnt = (FNTTABLE *)((byte *)fonts[0] + 1356);

	height = f_i[0].height;
	while (*ptr && x + fnt[*ptr].width <= 0)
		if (fnt[*ptr].width == 0) {
			x += f_i[0].space;
			ptr++;
		} else {
			x = x + fnt[*ptr].width;
			ptr++;
		}
	while (*ptr && x < vga_width)
		if (fnt[*ptr].width == 0) {
			x += f_i[0].space;
			ptr++;
		} else {
			texc(fonts[0] + fnt[*ptr].offset, x, y + fnt[*ptr].incY, fnt[*ptr].width, fnt[*ptr].height);
			x = x + fnt[*ptr].width;
			ptr++;
		}
}

void checkpal_font(int ifonts);

void draw_texts(int n) { // In: text[]

	int x, y, width, height;
	int font;

	byte *ptr, *ptr2;
	byte number[32];

	do
		if (texts[n].font) {

			if (n == max_texts) {
				if (clock_ticks & 64)
					break;
				else
					ptr = text[91];
			} else
				switch (texts[n].type) {
					case 0: ptr = (byte *)&mem[texts[n].ptr]; break;
					case 1:
						if (mem[texts[n].ptr] < 0) {
							number[0] = '-';
							ltoa(-mem[texts[n].ptr], (char *)number + 1, 10);
						} else
							ltoa(mem[texts[n].ptr], (char *)number, 10);
						ptr = number;
						break;
				}

			texts[n].w = 0; // Initializes region to flip for this text

			x = texts[n].x; // X first character
			y = texts[n].y; // Y

			font = 0;
			while (font < max_fonts) {
				if (texts[n].font == fonts[font])
					break;
				font++;
			}
			if (font == max_fonts)
				continue;

			checkpal_font(font);

			fnt = (FNTTABLE *)((byte *)texts[n].font + 1356);

			height = f_i[font].height;

			ptr2 = ptr;
			width = 0;
			while (*ptr2) {
				if (fnt[*ptr2].width == 0) {
					width += f_i[font].space;
					ptr2++;
				} else
					width += fnt[*ptr2++].width;
			}

			switch (texts[n].center) {
				case 0: break;
				case 1: x = x - (width >> 1); break;
				case 2: x = x - width; break;
				case 3: y = y - (height >> 1); break;
				case 4:
					x = x - (width >> 1);
					y = y - (height >> 1);
					break;
				case 5:
					x = x - width;
					y = y - (height >> 1);
					break;
				case 6: y = y - height; break;
				case 7:
					x = x - (width >> 1);
					y = y - height;
					break;
				case 8:
					x = x - width;
					y = y - height;
					break;
			}

			if (y < vga_height && y + height > 0) {

				texts[n].x0 = x;
				texts[n].y0 = y;
				texts[n].w = width;
				texts[n].h = height;

				if (y >= 0 && y + height <= vga_height) { // The text fits entirely (y coord.)

					while (*ptr && x + fnt[*ptr].width <= 0) {
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else {
							x = x + fnt[*ptr].width;
							ptr++;
						}
					}

					if (*ptr && x < 0)
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else {
							texc(texts[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].width,
							     fnt[*ptr].height);
							x = x + fnt[*ptr].width;
							ptr++;
						}

					while (*ptr && x + fnt[*ptr].width <= vga_width)
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else {
							texn(texts[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].width,
							     fnt[*ptr].height);
							x = x + fnt[*ptr].width;
							ptr++;
						}

					if (*ptr && x < vga_width)
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else
							texc(texts[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].width,
							     fnt[*ptr].height);

				} else {

					while (*ptr && x + fnt[*ptr].width <= 0)
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else {
							x = x + fnt[*ptr].width;
							ptr++;
						}

					while (*ptr && x < vga_width)
						if (fnt[*ptr].width == 0) {
							x += f_i[font].space;
							ptr++;
						} else {
							texc(texts[n].font + fnt[*ptr].offset,
							     x,
							     y + fnt[*ptr].incY,
							     fnt[*ptr].width,
							     fnt[*ptr].height);
							x = x + fnt[*ptr].width;
							ptr++;
						}
				}
			}
		}
	while (++n < max_texts);
}

void texn(byte *p, int x, int y, byte width, int height) {

	byte *q = copy_buffer + y * vga_width + x;
	int orig_width = width;

	do {
		do {
			if (*p)
				*q = *p;
			p++;
			q++;
		} while (--width);
		q += vga_width - (width = orig_width);
	} while (--height);
}

void texc(byte *p, int x, int y, byte width, int height) {

	byte *q = copy_buffer + y * vga_width + x;
	int skip_x, long_x, rest_x;
	int skip_y, long_y, rest_y;

	if (x < 0)
		skip_x = -x;
	else
		skip_x = 0;
	if (x + width > vga_width)
		rest_x = x + width - vga_width;
	else
		rest_x = 0;
	long_x = width - skip_x - rest_x;

	if (long_x <= 0)
		return;

	if (y < 0)
		skip_y = -y;
	else
		skip_y = 0;
	if (y + height > vga_height)
		rest_y = y + height - vga_height;
	else
		rest_y = 0;
	long_y = height - skip_y - rest_y;

	if (long_y <= 0)
		return;

	p += width * skip_y + skip_x;
	q += vga_width * skip_y + skip_x;
	rest_x += skip_x;
	width = long_x;
	do {
		do {
			if (*p)
				*q = *p;
			p++;
			q++;
		} while (--width);
		q += vga_width - (width = long_x);
		p += rest_x;
	} while (--long_y);
}

//════════════════════════════════════════════════════════════════════════════
// Draws mode 7 window
//════════════════════════════════════════════════════════════════════════════

void draw_sprites_m7(int n, int cx, int cy, float ang);

void draw_m7(int n) {
	int x, y;
#ifdef DEBUG
	int oclock = get_ticks();
#endif
	int id = (m7 + n)->camera;
	int height = (m7 + n)->height;
	int distance = (m7 + n)->distance;
	int angle = mem[id + _Angle];

	if (!im7[n].on || !id || id != mem[id])
		return;

	while (mem[id + _Angle] > 2 * pi)
		mem[id + _Angle] -= 2 * pi;
	while (mem[id + _Angle] < 0)
		mem[id + _Angle] += 2 * pi;

	angle = (mem[id + _Angle] * 4096) / (2 * pi);

	if (mem[id + _Resolution] > 0) {
		x = (((mem[id + _X] * 256) / mem[id + _Resolution]) * 256 - get_distx(mem[id + _Angle], distance * 65536));
		y = -(((mem[id + _Y] * 256) / mem[id + _Resolution]) * 256 - get_disty(mem[id + _Angle], distance * 65536));
	} else {
		x = (mem[id + _X] * 65536 - get_distx(mem[id + _Angle], distance * 65536));
		y = -(mem[id + _Y] * 65536 - get_disty(mem[id + _Angle], distance * 65536));
	}

	draw_mode7(n, x, height * 16384, y, angle); //<<14

	if (post_process_m7 != NULL)
		post_process_m7();

#ifdef DEBUG
	function_exec(251, get_ticks() - oclock);
#endif

	draw_sprites_m7(n, x, -y, (float)((float)mem[id + _Angle] / radian));
}

//════════════════════════════════════════════════════════════════════════════
// Draws mode 7 sprites (scroll snum)
//════════════════════════════════════════════════════════════════════════════

void draw_sprites_m7(int n, int cx, int cy, float ang) { // We pass in camera position
	int factor;
#ifdef DEBUG
	int oclock;
#endif
	int cx2 = cx / 4096, cy2 = cy / 4096, dx, dy;
	int ide, id, max, distmax, old_z;
	int height, h, width, percent;
	float a;

	factor = (((m7 + n)->focus * im7[n].w * 16) / 320);

	// Pre-calculate _Dist1 and _Dist2 for each mode-7 object

	for (id = id_start; id <= id_end; id += iloc_len) {
		if ((mem[id + _Status] == 2 || mem[id + _Status] == 4) && mem[id + _Ctype] == 2 &&
		    (mem[id + _Cnumber] == 0 || (mem[id + _Cnumber] & (1 << n)))) {
			if (mem[id + _Resolution] > 0) {
				dx = (mem[id + _X] * 16) / mem[id + _Resolution] - cx2;
				dy = cy2 - (mem[id + _Y] * 16) / mem[id + _Resolution];
			} else {
				dx = mem[id + _X] * 16 - cx2;
				dy = cy2 - mem[id + _Y] * 16;
			}
			if ((!dx && !dy) || abs(dx) + abs(dy) > 2800 * 16)
				mem[id + _Dist1] = 0;
			else {
				mem[id + _Dist1] = sqrt(dx * dx + dy * dy);
				a = ang - atan2(dy, dx);
				mem[id + _Dist2] = (float)mem[id + _Dist1] * sin(a);
				mem[id + _Dist1] = (float)mem[id + _Dist1] * cos(a);
			}
		} else
			mem[id + _Dist1] = 0;
	}

	// Draw visible objects inside mode-7

	for (ide = id_start; ide <= id_end; ide += iloc_len)
		mem[ide + _Painted] = 0;

	do {
#ifdef DEBUG
		oclock = get_ticks();
#endif
		ide = 0;
		max = 0x80000000;
		for (id = id_start; id <= id_end; id += iloc_len) {
			if (mem[id + _Ctype] == 2 && mem[id + _Dist1] && !mem[id + _Painted]) {
				if (mem[id + _Dist1] > max || (mem[id + _Dist1] == max && mem[id + _Z] > old_z)) {
					ide = id;
					max = mem[id + _Dist1];
					distmax = mem[id + _Dist2];
					old_z = mem[id + _Z];
				}
			}
		}

		if (ide) {
			if (max >= 32) { // Don't draw object if it's at camera coordinates (±2)

				h = (m7 + n)->height - mem[ide + _Height];
				height = (h * (max - factor)) / (max);                // in pix/4
				height = im7[n].y + im7[n].h / 2 + (h - height) / 4; // in pix
				height += (m7 + n)->horizon - im7[n].h / 2;

				h += 1000;
				percent = (h * (max - factor)) / (max);                // in pix/4
				percent = im7[n].y + im7[n].h / 2 + (h - percent) / 4; // in pix
				percent += (m7 + n)->horizon - im7[n].h / 2 - height;

				percent = mem[ide + _Size] * percent / 1000;

				width = vga_width / 2 + (factor * distmax) / (max * 16);

				a = ((float)mem[ide + _Angle] / radian) - ang;
				h = (a * 4096.0) / 6.2831853;
				h += 1024;
				while (h < 0)
					h += 4096;
				while (h >= 4096)
					h -= 4096;

				draw_sprite_m7(n, ide, width, height, percent, h);
#ifdef DEBUG
				process_paint(ide, get_ticks() - oclock);
#endif

				// *** White pixel at object base ***
				// if (height<vga_height && height>=0 && width<vga_width && width>=0) {
				//   *(copia+altura*vga_width+anchura)=127;
				// }
			}
			mem[ide + _Painted] = 1;
		}

	} while (ide);
}

//════════════════════════════════════════════════════════════════════════════
// Draw a sprite (mem[ide]) in mode 7 (if in front of camera)
//════════════════════════════════════════════════════════════════════════════

void draw_sprite_m7(int n, int ide, int x, int y, int size, int ang) {

	int *ptr;
	byte *si;
	int width, height; // Info regarding graph
	int xg, yg;
	int m, p;

	if (mem[ide + _File] > max_fpgs || mem[ide + _File] < 0)
		return;
	if ((p = mem[ide + _XGraph]) > 0) {
		m = mem[p];
		if (m < 1 || m > 256)
			return;
		// ang is the graph FoV (0..4096), m is the partition count
		mem[ide + _Flags] &= 254;
		mem[ide + _Graph] = ((ang + 4096 / (m * 2)) * m) / 4096;
		if (mem[ide + _Graph] >= m)
			mem[ide + _Graph] = 0;
		if ((mem[ide + _Graph] = mem[p + 1 + mem[ide + _Graph]]) < 0) {
			mem[ide + _Graph] = -mem[ide + _Graph];
			mem[ide + _Flags] |= 1;
		}
	}
	if (mem[ide + _File])
		max_grf = 1000;
	else
		max_grf = 2000;
	if (mem[ide + _Graph] <= 0 || mem[ide + _Graph] >= max_grf)
		return;
	if (g[mem[ide + _File]].grf == NULL)
		return;

	clipx0 = im7[n].x;
	clipx1 = im7[n].x + im7[n].w;
	clipy0 = im7[n].y;
	clipy1 = im7[n].y + im7[n].h;

	if ((ptr = g[mem[ide + _File]].grf[mem[ide + _Graph]]) != NULL) {

		width = ptr[13];
		height = ptr[14];                        // Graph width and height
		si = (byte *)ptr + 64 + ptr[15] * 4; // Graph start

		if (ptr[15] == 0 || *((word *)ptr + 32) == 65535) {
			xg = ptr[13] / 2;
			yg = ptr[14] - 1;
		} else {
			xg = *((word *)ptr + 32);
			yg = *((word *)ptr + 33);
		}

		sp_scaled(si, x, y, width, height, xg, yg, size, mem[ide + _Flags]);
	}
}

//════════════════════════════════════════════════════════════════════════════
// Get_distx/y(angle, dist)
//════════════════════════════════════════════════════════════════════════════

int get_distx(int a, int d) {
	angle = (float)a / radian;
	return ((int)((float)cos(angle) * d));
}

int get_disty(int a, int d) {
	angle = (float)a / radian;
	return (-(int)((float)sin(angle) * d));
}

//════════════════════════════════════════════════════════════════════════════
// Draws map in perspective
//════════════════════════════════════════════════════════════════════════════

void draw_mode7(int n, int camera_x, int camera_y, int camera_z, int angle) {

	int y, u, du, vv, dv, pos_x, pos_y;
	int width_m, height_m, width_e, height_e;
	int projection_y, sint, cost;
	int midx_mode7 = im7[n].w >> 1;
	int midx_mode7_16;
	int distance = ((im7[n].w * (m7 + n)->focus) / 320) << 16;
	int divisor;
	byte *di = copy_buffer + im7[n].y * vga_width + im7[n].x, *di_end, color = (m7 + n)->color;

	midx_mode7_16 = midx_mode7 << 16;

	width_m = im7[n].map_width - 1;
	height_m = im7[n].map_height - 1;
	width_e = im7[n].ext_width - 1;
	height_e = im7[n].ext_height - 1;

	if (!camera_y)
		return;

	for (y = 0; y < im7[n].h; y++, di += vga_width - im7[n].w) {
		divisor = (m7 + n)->horizon - y - 1;
		if (camera_y > 0) {
			if (divisor >= 0) {
				di += im7[n].w;
				continue;
			}
		} else {
			if (divisor <= 0) {
				di += im7[n].w;
				continue;
			}
		}
		projection_y = -camera_y / divisor;

		sint = mul_24(sine[angle], projection_y);
		cost = mul_24(cosine[angle], projection_y);

		u = camera_x + mul_16(distance, cost) + mul_16(sint, -midx_mode7_16);
		du = sint;

		vv = camera_z + mul_16(distance, sint) - mul_16(cost, -midx_mode7_16);
		dv = -cost;
		vv = -vv;
		dv = -dv;

		di_end = di + im7[n].w;

		if (im7[n].ext != NULL)
			do {
				pos_x = u >> 16;
				pos_y = vv >> 16;
				if (pos_x > width_m || pos_x < 0 || pos_y > height_m || pos_y < 0) {
					*di = *(im7[n].ext + (pos_y & height_e) * im7[n].ext_width + (pos_x & width_e));
				} else {
					*di = *(im7[n].map + pos_y * im7[n].map_width + pos_x);
				}
				u += du;
				vv += dv;
			} while (++di < di_end);
		else
			do {
				pos_x = u >> 16;
				pos_y = vv >> 16;
				if (pos_x > width_m || pos_x < 0 || pos_y > height_m || pos_y < 0) {
					*di = color;
				} else {
					*di = *(im7[n].map + pos_y * im7[n].map_width + pos_x);
				}
				u += du;
				vv += dv;
			} while (++di < di_end);
	}
}


//════════════════════════════════════════════════════════════════════════════
// Initializes sine and cosine tables
//════════════════════════════════════════════════════════════════════════════

void init_sin_cos() {
	int i;

	for (i = 0; i < angle_90; i++) { // Generate sine and cosine tables
		sine[i] = 256 * sin((float)i * 1131 / 737280) * 65536;
		sine[angle_180 - i] = sine[i];
		sine[angle_180 + i] = -sine[i];
		sine[angle_360 - i] = -sine[i];
		cosine[angle_90 - i] = sine[i];
		cosine[angle_90 + i] = -sine[i];
		cosine[angle_270 - i] = -sine[i];
		cosine[angle_270 + i] = sine[i];
	}
	sine[angle_0] = 0;
	sine[angle_90] = 256 * 65536;
	sine[angle_180] = 0;
	sine[angle_270] = -256 * 65536;
	sine[angle_360] = 0;
	cosine[angle_0] = 256 * 65536;
	cosine[angle_90] = 0;
	cosine[angle_180] = -256 * 65536;
	cosine[angle_270] = 0;
	cosine[angle_360] = 256 * 65536;
}

//════════════════════════════════════════════════════════════════════════════
