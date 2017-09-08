#ifndef __OSD_SDL12__
#define __OSD_SDL12__

#include <stdlib.h>
#include <string.h>

#include <SDL/SDL.h>
#ifdef MIXER
#include <SDL/SDL_mixer.h>
#endif
#ifdef IMAGE
#include <SDL/SDL_image.h>
#endif
#ifdef NETPLAY
#include <SDL/SDL_net.h> // Fix: correct SDL_net path (before "SDL_net.h")
#endif
#include "lib/sdlgfx/SDL_rotozoom.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "osd_sdl.h"

typedef SDLMod OSDEPMod;

#endif
