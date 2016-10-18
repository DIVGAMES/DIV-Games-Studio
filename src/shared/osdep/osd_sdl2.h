#ifndef __OSD_SDL2__
#define __OSD_SDL2__

#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef MIXER
#include <SDL2/SDL_mixer.h>
#endif

#ifdef IMAGE
#include <SDL2/SDL_image.h>
#endif

#include "osd_sdl.h"
typedef SDL_Keymod OSDEPMod;
#endif

void OSDEP_SetWindowSize(int w, int h);