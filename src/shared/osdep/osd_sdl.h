#ifndef __OSD_SDL_H__
#define __OSD_SDL_H__

typedef SDL_Joystick OSDEP_Joystick;
typedef SDL_Color OSDEP_Color;
typedef SDL_Surface OSDEP_Surface;

typedef struct _OSDEP_VMode {
	int w;
	int h;
} OSDEP_VMode;

extern uint8_t OSDEP_key[2048];

// the following functions all need to be implemented in order for a port to
// function correctly

typedef SDL_Rect OSDEP_Rect;


void OSDEP_Init(void);
void OSDEP_Quit(void);
void OSDEP_WarpMouse(int x, int y);
void OSDEP_Flip(OSDEP_Surface *s);
uint32_t OSDEP_GetTicks(void);
void OSDEP_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
int OSDEP_IsFullScreen(void);

OSDEP_Surface * OSDEP_SetVideoMode(int width, int height, int bpp, char fs);

int OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors);

void OSDEP_JoystickClose(OSDEP_Joystick *joy);

// int OSDEP_JoystickNumButtons(OSDEP_Joystick *n);
// int OSDEP_JoystickNumHats(OSDEP_Joystick *n);
// int OSDEP_JoystickNumAxes(OSDEP_Joystick *n);

uint8_t OSDEP_JoystickGetButton(OSDEP_Joystick *joystick, int button);
int16_t OSDEP_JoystickGetAxis(OSDEP_Joystick *joystick, int axis);

char * OSDEP_JoystickName(int n);


#endif
