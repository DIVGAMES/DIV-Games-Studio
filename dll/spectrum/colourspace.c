/*
	SCRplus - (c) Edward Cree 2010-12
	Licensed under the GNU GPL v3+
*/

#include "colourspace.h"
#include <stdlib.h>

void rgbtoyuv(const rgb_t rgb, yuv_t yuv)
{
	yuv[0]=(0.257 * rgb[0]) + (0.504 * rgb[1]) + (0.098 * rgb[2]) + 16.0;
	yuv[1]=-(0.148 * rgb[0]) - (0.291 * rgb[1]) + (0.439 * rgb[2]) + 128.0;
	yuv[2]=(0.439 * rgb[0]) - (0.368 * rgb[1]) - (0.071 * rgb[2]) + 128.0;
}

void yuvtorgb(const yuv_t yuv, rgb_t rgb)
{
	rgb[2]=min(max(1.164*(yuv[0]-16.0)+2.018*(yuv[1]-128.0), 0), 255);
	rgb[1]=min(max(1.164*(yuv[0]-16.0)-0.813*(yuv[2]-128.0)-0.391*(yuv[1]-128.0), 0), 255);
	rgb[0]=min(max(1.164*(yuv[0]-16.0)+1.596*(yuv[2]-128.0), 0), 255);
}

double yuvdist(const yuv_t pal, const yuv_t dat)
{
	return(fabs(pal[0]-dat[0])+fabs(pal[1]-dat[1])+fabs(pal[2]-dat[2]));
}

void rgbtohsv(const rgb_t rgb, hsv_t hsv)
{
	// based on https://en.wikipedia.org/wiki/HSL_and_HSV
	unsigned char M=0, m=255;
	for(unsigned int i=0;i<3;i++)
	{
		M=max(M, rgb[i]);
		m=min(m, rgb[i]);
	}
	double hp; // H'
	unsigned char C=M-m; // Chroma
	if(!C)
		hp=0; // really undefined
	else if(M==rgb[0])
		hp=fmod((rgb[1]-rgb[2])/(double)C, 6);
	else if(M==rgb[1])
		hp=(rgb[2]-rgb[0])/(double)C+2;
	else
		hp=(rgb[0]-rgb[1])/(double)C+4;
	hsv[2]=floor(hp*255/6.0); // Hue
	hsv[0]=M; // Value
	if(M)
		hsv[1]=(C*255)/M; // Saturation
	else
		hsv[1]=0;
}

unsigned char clamp(int i)
{
	if(i<0) return(0);
	if(i>255) return(255);
	return(i);
}

void hsvtorgb(const hsv_t hsv, rgb_t rgb)
{
	double hp=hsv[2]*6/255.0; // H'
	double C=hsv[1]*hsv[0]/255;
	double x=C*(1-fabs(fmod(hp, 2)-1));
	int m=hsv[0]-C;
	int fh=floor(hp);
	switch(fh%6)
	{
		case 0:
			rgb[0]=clamp(C+m);
			rgb[1]=clamp(x+m);
			rgb[2]=clamp(m);
		break;
		case 1:
			rgb[0]=clamp(x+m);
			rgb[1]=clamp(C+m);
			rgb[2]=clamp(m);
		break;
		case 2:
			rgb[0]=clamp(m);
			rgb[1]=clamp(C+m);
			rgb[2]=clamp(x+m);
		break;
		case 3:
			rgb[0]=clamp(m);
			rgb[1]=clamp(x+m);
			rgb[2]=clamp(C+m);
		break;
		case 4:
			rgb[0]=clamp(x+m);
			rgb[1]=clamp(m);
			rgb[2]=clamp(C+m);
		break;
		case 5:
			rgb[0]=clamp(C+m);
			rgb[1]=clamp(m);
			rgb[2]=clamp(x+m);
		break;
	}
}
