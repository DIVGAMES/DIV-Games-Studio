// OSDEP INCLUDE FILE
// CONFIGURED FOR SDL1.2

#include "osd_sdl2.h"

void OSDEP_Init(void) {

	SDL_Init( SDL_INIT_EVERYTHING);

}

void OSDEP_Quit(void) {
	SDL_Quit();
}

// Timer
uint32_t OSDEP_GetTicks(void) {
	return SDL_GetTicks();
}

// Display
void OSDEP_SetCaption(char *title, char *icon) {
	SDL_WM_SetCaption((const char *)title, (const char *)icon);
}

OSDEP_Surface * OSDEP_SetVideoMode(int width, int height, int bpp, Uint32 flags) {

	return SDL_SetVideoMode(width, height, bpp, flags);

}


int OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors) {
	return SDL_SetPaletteColors(surface->format->palette, colors, 0, 256);

}

// Joysticks
int32_t OSDEP_NumJoysticks(void) {
	return SDL_NumJoysticks();
}

int OSDEP_JoystickNumButtons(int n) {
	return SDL_JoystickNumButtons(n);
}

int OSDEP_JoystickNumHats(int n) {
	return SDL_JoystickNumHats(n);
}

int OSDEP_JoystickNumAxes(int n) {
	return SDL_JoystickNumAxes(n);
}

uint8_t OSDEP_JoystickGetButton(OSDEP_Joystick *joystick, int button) {
	return SDL_JoystickGetButton(joystick, button);
}

int16_t OSDEP_JoystickGetAxis(OSDEP_Joystick *joystick, int axis) {
	return SDL_JoystickGetAxis(joystick, axis);

}
OSDEP_Joystick * OSDEP_JoystickOpen(int n) {
	return SDL_JoystickOpen(n);
}

void OSDEP_JoystickClose(OSDEP_Joystick *joy) {
	SDL_JoystickClose(joy);
	return;
}

char * OSDEP_JoystickName(int n) {
	return SDL_JoystickName(n);
}

void OSDEP_keyInit(void) {


}