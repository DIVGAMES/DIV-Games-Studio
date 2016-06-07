#include <math.h>
#include <stdio.h>

#define GLOBALS
#include "div.h"

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include "libscrplus.h"
#include "bits.h"
#include "bits.h"
#include "colourspace.h"

//#include "SDL/SDL.h"

//SDL_Surface *screen = NULL;

unsigned char tweaks[3][2]; // h/s/v, shift/scale
unsigned int w=256, h=192;

typedef unsigned char uchar;

unsigned char palent[64];


void init(void) {
	
	
	
	byte render =0;
	sizer crop=SIZER_ZOOM;
	rgb_t pix[w][h];
	yuv_t post[w][h];
	
	for(int i=0;i<3;i++) // set tweak defaults
	{
		tweaks[i][0]=128;
		tweaks[i][1]=128;
	}

	w=256;
	h=192;

	memset(palent, 0, sizeof(unsigned char[64]));

	genpalyuv(render); // 0 - default, 4=BW

//	resample(buffer,wide,height,crop,pix);
	
//	printf("%x %d %d\n",buffer,wide,height);

}

void post_process_buffer(void)
{
	// convert buffer to spectrum pixels, with border and colour clash
	
	// divide screen up into 8x8 squares
	// each with ink / paper + bright / flash (maybe)
//printf("post process buffer\n");

init();
}

void background_to_buffer(void)
{

}

void set_video_mode(void){
  // Leaves unchanged the 320x200 mode, which is the default mode.
//  if(screen==NULL)
//	screen = SDL_SetVideoMode(320,200,8,0);

}

void buffer_to_video(void) {
	
}


void __export divlibrary(LIBRARY_PARAMS)
{
	
}

void __export divmain(COMMON_PARAMS)
{
	AutoLoad();
	GLOBAL_IMPORT();
	DIV_export("post_process_buffer",post_process_buffer);
//	DIV_export("background_to_buffer",background_to_buffer);
//    DIV_export("buffer_to_video",buffer_to_video);
//	DIV_export("set_video_mode",set_video_mode);
//	screen=SDL_SetVideoMode(320,200,8,0);


}

void __export divend(COMMON_PARAMS) { }
