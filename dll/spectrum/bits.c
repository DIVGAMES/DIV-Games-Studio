/*
	SCRplus - (c) Edward Cree 2010-11
	Licensed under the GNU GPL v3+
*/

#include "bits.h"

void fputw(unsigned short int a, FILE *stream)
{
	/* LITTLE ENDIAN */
	fputc(a%256, stream);
	fputc(a>>8, stream);
}

void fputq(unsigned long int a, FILE *stream)
{
	/* LITTLE ENDIAN */
	fputc(a%256, stream);
	fputc((a>>8)%256, stream);
	fputc((a>>16)%256, stream);
	fputc((a>>24)%256, stream);
}

int vtweak(int value, int shift, int scale)
{
	return((int)min(max((value+(shift<<1)-255-127.5)*scale/(double)(256-scale)+127.5, 0), 255));
}

int ctweak(int value, int shift, int scale)
{
	return((int)min(max((value+(shift<<1)-255)*scale/(double)(256-scale), 0), 255));
}

int htweak(int value, int shift)
{
	return((value+shift+128)%256);
}

int ncluts(unsigned char clm)
{
	return((clm&1)+((clm>>1)&1)+((clm>>2)&1)+((clm>>3)&1));
}
