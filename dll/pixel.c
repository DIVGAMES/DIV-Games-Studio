/**
 * Pixelate DLL
 * for DIV GAMES STUDIO
 * (c) 2015 mikeDX
 * Created for MarioKart remix
 * 
 * http://js.mikedx.co.uk/mkremix.html (Javascript)
 * http://js.mikedx.co.uk/mkremix.opk  (GCW Zero)
 * http://js.mikedx.co.uk/mkremix.apk  (Android)
 * http://js.mikedx.co.uk/mkremix.pnd  (Pandora Console)
 *
 * gcc pixel.c -o pixel.dll -fPIC -funsigned-char -shared -O3
 *
 */

#include <math.h>
#include <stdio.h>

#define GLOBALS
#include "div.h"

int ps = 0;
int pb = 0;

void Pixelate_Background()
{
	pb = getparm();
	retval(pb);
}

void Pixelate_Screen()
{
	ps = getparm();
	retval(ps);
}

void post_process_buffer(void)
{

	int x=0;
	byte p=0;
	int y=0;
	int offs=0;
	int pps=0;

	if(ps<2)
		return;

	for(x=0; x<wide; x+=ps) {
		for(y=0; y<height; y+=ps) {

			p = buffer[x+y*wide];
			pps=ps;

			if(x+ps>wide) {
				pps=wide-x;
			}

			offs=0;

			while(y+offs<height && offs<ps) {
				memset(&buffer[x+(y+offs)*wide],p,pps);
				offs++;
			}
		}
	}

}

void background_to_buffer(void)
{


	int x=0,y=0;
	byte p=0;
	int ppb;
	int offs=0;

	if(pb<2) {
		memcpy(buffer,background,wide*height);
		return;
	}

	for(x=0; x<wide; x+=pb) {
		for(y=0; y<height; y+=pb) {

			p = background[x+y*wide];
			ppb=pb;

			if(x+pb>wide) {
				ppb=wide-x;
			}

			offs=0;

			while(y+offs<height && offs<pb) {
				memset(&buffer[x+(y+offs)*wide],p,ppb);
				offs++;
			}
		}
	}

}

void __export divlibrary(LIBRARY_PARAMS)
{

	COM_export("Pixelate_Screen",Pixelate_Screen,1);
	COM_export("Pixelate_Background",Pixelate_Background,1);

}

void __export divmain(COMMON_PARAMS)
{
//  AutoLoad();
	GLOBAL_IMPORT();
	DIV_export("post_process_buffer",post_process_buffer);
	DIV_export("background_to_buffer",background_to_buffer);

}

void __export divend(COMMON_PARAMS) { }
