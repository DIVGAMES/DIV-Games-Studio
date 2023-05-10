#ifndef _NEWFUNCS_H
#define _NEWFUNCS_H

#include "global.h"
#include "osdep.h"

#ifdef IMAGE
#include "SDL_image.h"
#endif

OSDEP_Surface * DIV_IMG_Load(char *file);
OSDEP_Surface *DIV_IMG_Load_Mem(void *ptr, byte len);



#endif
