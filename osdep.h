// OSDEP includes
// should be good for most SDL ports

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#define cprintf printf

#define _MAX_PATH PATH_MAX
#define _MAX_DRIVE 26
#define _MAX_DIR 2048
#define _MAX_FNAME 256
#define _MAX_EXT 64

#include <SDL.h>


#define far /*far*/
#define __far /*__far*/
#define __interrupt /* __interrupt */
#define __loadds /* __loadds */
#define _HARDERR_FAIL 0

///////////////////////////////////////////////////
// DEFINED IN osdep.cpp
///////////////////////////////////////////////////

extern char * strupr(char *string);
extern char * strlwr(char *string);

extern void _dos_setdrive( unsigned __drivenum, unsigned *__drives );
char * itoa(long n, char *buf, int len);

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
