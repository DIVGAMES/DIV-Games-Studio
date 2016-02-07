#ifndef __OSDEP_H
#define __OSDEP_H

// OSDEP includes
// should be good for most SDL ports

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef ZLIB
#include "unzip.h"
#endif

#ifdef TTF
#include "ttf.h"
#endif


#ifndef PS2
#include <stdint.h>
#else
#include <time.h>
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef long int64_t;
typedef unsigned short uint16_t;
typedef int int32_t;

#endif

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>
#define cprintf printf

#ifndef PATH_MAX
#define PATH_MAX 143
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 144
#endif

#ifndef _MAX_DRIVE
#define _MAX_DRIVE 26
#endif

#ifndef _MAX_DIR
#define _MAX_DIR 66
#endif

#ifndef _MAX_FNAME
#define _MAX_FNAME 256+100
#endif

#ifndef _MAX_EXT
#define _MAX_EXT 64
#endif



#include <SDL/SDL.h>

#ifdef TTF
#include "SDL/SDL_ttf.h"
#endif

#ifdef MIXER
#include <SDL/SDL_mixer.h>
#endif

#ifdef IMAGE
#include <SDL/SDL_image.h>
#endif

#if __WORDSIZE == 32
#define memptrsize int
#else
#define memptrsize long
#endif


#define far /*far*/
#define __far /*__far*/
#define __near /*__near*/
#define __interrupt /* __interrupt */
#define __loadds /* __loadds */
#define _HARDERR_FAIL 0
#define _HARDERR_IGNORE 1

#ifndef __stdcall 
#define __stdcall /* __stdcall */
#endif

///////////////////////////////////////////////////
// DEFINED IN osdep.cpp
///////////////////////////////////////////////////
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <string.h>
#else
#ifndef PS2
#ifndef PSP
extern char * strupr(char *string);
extern char * strlwr(char *string);
#endif
#endif

#endif

extern void _dos_setdrive( unsigned __drivenum, unsigned *__drives );
#ifndef __WIN32__
char * itoa(long n, char *buf, int len);
#endif
void sdlkeyinit(void);
extern uint8_t sdl2key[2048];

#define ltoa itoa 
#define stdprn stdout
#define stricmp strcasecmp


extern void _splitpath (
   const char *path,  // Path Input
   char *drive,       // Drive     : Output
   char *dir,         // Directory : Output
   char *fname,       // Filename  : Output
   char *ext          // Extension : Output
);

extern char *_fullpath(char *_FullPath,const char *_Path,size_t _SizeInBytes);


// dos directory stuff

#include <dirent.h>

struct find_t {
  char reserved[21];
  unsigned char attrib;
  unsigned short wr_time;
  unsigned short wr_date;
  unsigned long size;
  char name[256];
};
#ifndef _A_NORMAL
#define _A_NORMAL (0x00)
//Normal file (no read/write restrictions)
#endif

#ifndef _A_RDONLY
#define _A_RDONLY (0x01)
//Read only file
#endif

#ifndef _A_HIDDEN
#define _A_HIDDEN (0x02)
//Hidden file
#endif

#ifndef _A_SYSTEM
#define _A_SYSTEM (0x04)
//System file
#endif

#define _A_VOLID (0x08)
//Volume ID file

#ifndef _A_SUBDIR
#define _A_SUBDIR (0x10)
//Subdirectory
#endif

#ifndef _A_ARCH
#define _A_ARCH (0x20)
//Archive file
#endif

unsigned int _dos_findfirst(char *name, unsigned int attr, 
                            struct find_t *result);
unsigned int _dos_findnext(struct find_t *result);

unsigned int _dos_setfileattr(const char *filename, unsigned int attr);

#ifndef __WIN32__
void mkdir(char *dir);
#endif

void _dos_getdrive( unsigned int *drive);
void _makepath(
   char *path,
   const char *drive,
   const char *dir,
   const char *fname,
   const char *ext 
);

extern void compilar(void);
extern int _chdir(const char* Directory);

//#define SoundInfo SDL_AudioSpec

#define TXTRESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7


void textcolor(int attr, int fg, int bg);

#if __WORDSIZE == 64
#define ptr8 16
#define ptr4 8
#else
#define ptr8 8
#define ptr4 4
#endif

#endif // __OSDEP_H

