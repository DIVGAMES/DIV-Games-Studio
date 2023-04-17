#ifndef __OSD_SDL2__
#define __OSD_SDL2__

#include <SDL.h>
#include <stdio.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef MIXER
#include <SDL_mixer.h>
#endif

#ifdef IMAGE
#include <SDL_image.h>
#endif

#include "osd_sdl.h"
typedef SDL_Keymod OSDEPMod;
#endif

void OSDEP_SetWindowSize(int w, int h);

int OSDEP_JoystickNumButtons(OSDEP_Joystick *n);
int OSDEP_JoystickNumHats(OSDEP_Joystick *n);
int OSDEP_JoystickNumAxes(OSDEP_Joystick *n);
