#ifndef __OSD_SDL2__
#define __OSD_SDL2__

#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef MIXER
#include <SDL2/SDL_mixer.h>
#endif

#ifdef IMAGE
#include <SDL2/SDL_image.h>
#endif

//#include "lib/sdlgfx/SDL_rotozoom.h"

typedef SDL_Joystick OSDEP_Joystick;
typedef SDL_Color OSDEP_Color;
typedef SDL_Surface OSDEP_Surface;

#include "osd_funcs.h"


#endif

