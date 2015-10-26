// OSDEP includes
// should be good for most SDL ports

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#define cprintf printf

#define _MAX_PATH PATH_MAX

#include <SDL.h>


#define far /*far*/
#define __far /*__far*/
#define _HARDERR_FAIL 0

///////////////////////////////////////////////////
// DEFINED IN osdep.cpp
///////////////////////////////////////////////////

extern void strupr(char *string);
extern void strlwr(char *string);

extern void _dos_setdrive( unsigned __drivenum, unsigned *__drives );
void itoa(long n, char *buf, int len);

#define ltoa itoa 

