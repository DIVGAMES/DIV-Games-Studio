/*
	SCRplus - (c) Edward Cree 2010-12
	Licensed under the GNU GPL v3+
*/

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include "colourspace.h"

#define RENDER_TIMEX	1 // Timex 8x1 attribute mode
#define RENDER_ULAPLUS	2 // ULA+ mode
#define RENDER_BW		4 // black & white mode, don't flip attribs

#define IS_RENDER_TIMEX(render)		((render) & RENDER_TIMEX)
#define IS_RENDER_ULAPLUS(render)	((render) & RENDER_ULAPLUS)
#define IS_RENDER_BW(render)		((render) & RENDER_BW)

typedef enum
{
	SIZER_SCALE,
	SIZER_CROP,
	SIZER_ZOOM,
}
sizer;

typedef unsigned char palette[64];

extern yuv_t palyuv[256];

// Palette manipulation
int import_palette(FILE *f, palette palent); // reads a palette from a file
int genpalyuv(unsigned char render); // creates the yuv triples for each palette entry for the selected render mode

// Main image pipeline
int resample(const SDL_Surface *image, unsigned int w, unsigned int h, sizer crop, rgb_t pix[w][h]); // resamples an image to the specified width & height
int apply_tweak(unsigned int w, unsigned int h, unsigned char render, rgb_t pix[w][h], unsigned char tweaks[3][2], yuv_t post[w][h]); // Applies hsv-tweaks to image & converts to yuv
int pickattrs(unsigned int w, unsigned int h, unsigned char render, yuv_t yuv[w][h], unsigned char pal[w/8][h][2]); // picks attributes for normal ULA display
int dither(unsigned int w, unsigned int h, unsigned char render, yuv_t yuv[w][h], unsigned char pal[w/8][h][2], const palette palent, bool dithflow, bool npx[w][h]); // dithers image down, using Floyd-Steinberg

// ULA+ specific pipeline functions (see file 'algorithm' for details)
int optima(unsigned int w, unsigned int h, unsigned char render, yuv_t yuv[w][h], unsigned char opt[w/8][h][2][4]); // computes per-block yuv optima
int optipal(unsigned int w, unsigned int h, unsigned char render, unsigned char opt[w/8][h][2][4], unsigned char palopt[w/8][h][2][2]); // finds nearest palent for each per-block optimum
int countopt(unsigned int w, unsigned int h, unsigned char render, unsigned char palopt[w/8][h][2][2], unsigned int distopt[256]); // count the optima over the palette
int pickpal(const unsigned int distopt[256], palette palent, int depress, unsigned char clutmask, unsigned char usemask); // pick a palette (heuristic magic)
int ula_pickattrs(unsigned int w, unsigned int h, unsigned char render, unsigned char opt[w/8][h][2][4], const palette palent, unsigned char pal[w/8][h][2], unsigned char usemask); // picks attributes for ULA+ display subject to the given palette

// Output drivers
int writebmp(FILE *fp, unsigned int w, unsigned int h, unsigned char render, unsigned char pal[w/8][h][2], const palette palent, bool npx[w][h]); // write the image out as a bitmap
int writescr(FILE *fp, unsigned int w, unsigned int h, unsigned char render, unsigned char pal[w/8][h][2], const palette palent, bool npx[w][h]); // write the image out as an extended SCR
int writezxp(FILE *fp, unsigned int w, unsigned int h, unsigned char render, unsigned char pal[w/8][h][2], const palette palent, bool npx[w][h]); // write the image out as an extended ZXP

// RGB output mappings
void zxtorgb(unsigned char zx, rgb_t rgb); // convert a Spectrum (4-bit) colour to RGB
void ulatorgb(unsigned char ula, rgb_t rgb); // convert a ULA+ (8-bit) colour to RGB
