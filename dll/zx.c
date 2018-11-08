// ZX SPECTRUM DLL
/**

gcc zx.c sdlgfx/SDL_rotozoom.c spectrum/colourspace.c spectrum/libscrplus.c spectrum/bits.c -o zx.so -fPIC -funsigned-char -shared -O3 `sdl-config --cflags` `sdl-config --libs`

**/
#include <math.h>
#include <stdio.h>
//#include <mem.h>

#include "spectrum/libscrplus.h"
#include "spectrum/bits.h"
#include "../src/shared/lib/sdlgfx/SDL_rotozoom.h"

#define GLOBALS
#include "div.h"

int w=256;
int h=192;
int bw=64;
int bh=40;

uint32_t rmask, gmask, bmask, amask;

	SDL_Surface *screen = NULL;
	SDL_Surface *image = NULL; // our temporary image
	SDL_Surface *image2 = NULL; // our temporary image
	SDL_Surface *image3 = NULL;
	SDL_Rect rc;

void set_video_mode(){
	
	rc.x=bw;
	rc.y=bh;
	rc.w=w*2;
	rc.h=h*2;
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif
  // Leaves unchanged the 320x200 mode, which is the default mode.
	
	screen = SDL_SetVideoMode(w*2+bw*2,h*2+bh*2,8,0);

	if(image!=NULL)
		SDL_FreeSurface(image);
	
	image = SDL_CreateRGBSurface(0,wide,height,32,rmask, gmask, bmask, amask);

//	image2 = SDL_CreateRGBSurface(0,w,h,32,rmask, gmask, bmask, amask);
	
}


inline void put_pixel32( SDL_Surface *surface, int x, int y, Uint32 pixel )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)surface->pixels;
    
    //Set the pixel
    pixels[ ( y * surface->w ) + x ] = pixel;
}

unsigned char render=0;//; // render|flags: 1=timex, 2=ulaplus, 4=bw

void buffer_to_video() {

//	int omode=0; // output mode: 0=scr, 1=bmp, 2=zxp
	int depress=0; // controls palette variation

	yuv_t palyuv[256]; 
	bool c_palyuv=true;
	memset(palyuv, 0, sizeof(palyuv));

	rgb_t pix[w][h]; bool c_pix=false;
	memset(pix, 0, sizeof(pix));

	yuv_t post[w][h]; bool c_post=false;
	memset(post, 0, sizeof(post));

	unsigned char opt[w/8][h][2][4]; bool c_opt=false;
	memset(opt, 0, sizeof(opt));

	unsigned char palopt[w/8][h][2][2]; bool c_palopt=false;
	memset(palopt, 0, sizeof(palopt));

	unsigned int distopt[256]; bool c_distopt=false;
	memset(distopt, 0, sizeof(distopt));

	unsigned char palent[64]; bool c_palent=false;
	memset(palent, 0, sizeof(palent));

	unsigned char pal[w/8][h][2]; bool c_pal=false;
	memset(pal, 0, sizeof(pal));

	bool npx[w][h]; bool c_npx=false;
	memset(npx, 0, sizeof(npx));

//	bool unsaved=false;

	bool dithflow=false;//true;

	sizer crop=SIZER_CROP;//SIZER_SCALE;
	
	unsigned char tweaks[3][2]; // h/s/v, shift/scale

	for(int i=0;i<3;i++) // set tweaks to defaults
	{
		tweaks[i][0]=tweaks[i][1]=128;
	}	
	if(screen==NULL)
		screen = SDL_SetVideoMode(256,192,32,0);

	int x=0;
	int y=0;
	byte c=0;
	
//	SDL_LockSurface(image);

	for(x=0;x<wide;x++) {
		for(y=0;y<height;y++) {
			c=buffer[x+wide*y];
			put_pixel32(image,x,y, SDL_MapRGB(image->format,palette[c*3]*4,palette[c*3+1]*4,palette[c*3+2]*4));
//32bitpal[c]);
			
		}
	}
	

//SDL_UnlockSurface(image);
	if(image2!=NULL)
		SDL_FreeSurface(image2);

// scale our image		
	image2=zoomSurface(image, (float)w/wide, (float)h/height,1);
	
// convert to spectrum

	if(genpalyuv(render))
		return;
		
	if(resample(image2, w, h, crop, pix))
		return;
	
	if(apply_tweak(w, h, render, pix, tweaks, post))
		return;

	if(pickattrs(w, h, render, post, pal))
		return;
		
	if(dither(w, h, render, post, pal, palent, dithflow, npx))
		return;

	SDL_LockSurface(image2);

	for(y=0;y<h;y++) {
		for(x=0;x<w;x++) {
			uint bx=floor(x/8), by=IS_RENDER_TIMEX(render)?y:floor(y/8);
			rgb_t rgb;

			int a=pal[bx][by][0];
			int i=a&7;
			int p=(a>>3)&7;
			int b=a>>6;
			int zx=(npx[x][y]?i:p)+(b<<3);
			zxtorgb(zx, rgb);

			put_pixel32(image2,x,y,SDL_MapRGB(image2->format,rgb[0],rgb[1],rgb[2]));

		}

	
	}

	SDL_UnlockSurface(image2);

	if(image3!=NULL)
		SDL_FreeSurface(image3);
	
	image3=zoomSurface(image2,2,2,0);
	
	if(SDL_MUSTLOCK(screen))
		SDL_LockSurface(screen);

	SDL_BlitSurface(image3,NULL,screen, &rc);

	

/*
  static int init=0,type_dump=0,f1_pressed=0;

	if (!init) { init=1; put_hboy(); }

  if (key[59]) {
      if (!f1_pressed) {
          f1_pressed=1;
          type_dump=(type_dump+1)%3;
      }
  } else f1_pressed=0;

  switch (type_dump) {
      case 0: dbuffer_to_video1(); break;
      case 1: dbuffer_to_video2(); break;
      case 2: dbuffer_to_video3(); break;
  }
*/
	if(SDL_MUSTLOCK(screen))
		SDL_UnlockSurface(screen);

SDL_Flip(screen);
}

void process_active_palette()
{
	printf("process active palette\n");
	return;
}

void post_process()
{
	return;
}

void process_palette()
{
	printf("process palette\n");
	return;
}

void __export divmain(COMMON_PARAMS) {
	AutoLoad();
	GLOBAL_IMPORT();
//	DIV_export("process_palette",process_palette);

	DIV_export("set_video_mode",set_video_mode);
	DIV_export("buffer_to_video",buffer_to_video);
//	DIV_export("post_process",post_process);
//	DIV_export("process_active_palette",process_active_palette);
	set_video_mode();
	
}

void __export divlibrary(LIBRARY_PARAMS){}
void __export divend(COMMON_PARAMS){
	screen=SDL_SetVideoMode(320,200,8,0);
}

