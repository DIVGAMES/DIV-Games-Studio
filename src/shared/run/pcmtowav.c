//════════════════════════════════════════════════════════════════════════════
// Convierte de PCM a WAV
//════════════════════════════════════════════════════════════════════════════

#ifdef DOS
#include <dos.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "osdep.h"

#define byte unsigned char
#define word unsigned short

int DIVSaveWAV (byte *byte_ptr, int length, FILE *f);
int DIVIsWAV   (FILE *FileName);

//════════════════════════════════════════════════════════════════════════════
// Programa principal
//════════════════════════════════════════════════════════════════════════════
int pcm2wav(FILE *in, long inlen, FILE *out, long outlen) {
	byte *ptr;
	int c;

	if(DIVIsWAV(in)) {
		return 1;
	}

	if ((ptr=(byte *)malloc(inlen)) == NULL) {
printf("\n\nError: Memoria insuficiente.\n");

	return 4;
}

	fseek(in, 0, SEEK_SET);

	if((c = fread(ptr, 1, inlen, in)) != inlen) {
printf("\n\nError: %d %d No se pudo leer el archivo.\n",
       c,
       inlen);

	free(ptr);

	return 3;
	}

	if(!DIVSaveWAV(ptr, inlen, out)) {
printf("...Error\n");

		free(ptr);

		return 2;
	}

//printf("...Ok\n");

	free(ptr);

	return 0; // success
}

typedef struct _t_HeadDC {
	unsigned int   dwUnknow;
	unsigned short wFormatTag;
	unsigned short wChannels;
	unsigned int   dwSamplePerSec;
	unsigned int   dwAvgBytesPerSec;
	unsigned short wBlockAlign;
	unsigned short wBits;
} t_HeadDC;

int DIVSaveWAV(byte *byte_ptr, int length, FILE *dstfile) {
	t_HeadDC MyHeadDC;
//	int    con;
//	float  paso, pos;

	fputc('R', dstfile);
	fputc('I', dstfile);
	fputc('F', dstfile);
	fputc('F', dstfile);

	length += 36;
	if(fwrite(&length, 1, 4, dstfile) != 4) {
		return (0);
	}

	fputc('W', dstfile);
	fputc('A', dstfile);
	fputc('V', dstfile);
	fputc('E', dstfile);
	fputc('f', dstfile);
	fputc('m', dstfile);
	fputc('t', dstfile);
	fputc(' ', dstfile);

	MyHeadDC.dwUnknow         = 16;
	MyHeadDC.wFormatTag       = 1;
	MyHeadDC.wChannels        = 1;
	MyHeadDC.dwSamplePerSec   = 11025;
	MyHeadDC.dwAvgBytesPerSec = 11025;
	MyHeadDC.wBlockAlign      = 1;
	MyHeadDC.wBits            = 8;
	if(fwrite(&MyHeadDC, 1, sizeof(t_HeadDC), dstfile) != sizeof(t_HeadDC)) {
		return (0);
	}

	fputc('d', dstfile);
	fputc('a', dstfile);
	fputc('t', dstfile);
	fputc('a', dstfile);

	length -= 36;
	if(fwrite(&length, 1, 4, dstfile)!=4) {
		return (0);
	}

	if(fwrite(byte_ptr, 1, length, dstfile) != length) {
		return (0);
	}

	return (1);
}

int DIVIsWAV(FILE *f) {
//	FILE *f;
	int ok = 1;

/*
	if((f = fopen(FileName, "rb")) == NULL) {
		return (0);
	}
*/

	if(f == NULL) {
		return (0);
	}

	if(fgetc(f) != 'R') { ok = 0; }
	if(fgetc(f) != 'I') { ok = 0; }
	if(fgetc(f) != 'F') { ok = 0; }
	if(fgetc(f) != 'F') { ok = 0; }

	fseek(f, 4, SEEK_CUR);

	if(fgetc(f) != 'W') { ok = 0; }
	if(fgetc(f) != 'A') { ok = 0; }
	if(fgetc(f) != 'V') { ok = 0; }
	if(fgetc(f) != 'E') { ok = 0; }

	fseek(f, 0, SEEK_SET);

	return (ok);
}
