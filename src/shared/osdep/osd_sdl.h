#ifndef __OSD_SDL_H__
#define __OSD_SDL_H__

typedef SDL_Joystick OSDEP_Joystick;
typedef SDL_Color    OSDEP_Color;
typedef SDL_Surface  OSDEP_Surface;

typedef struct _OSDEP_VMode {
	int w;
	int h;
} OSDEP_VMode;

extern uint8_t OSDEP_key[2048];

// the following functions all need to be implemented in order for a port to function correctly
void             OSDEP_Init(void);
void             OSDEP_Quit(void);

void             OSDEP_KeyInit(void);
void             OSDEP_WarpMouse(int x, int y);

void             OSDEP_Flip(OSDEP_Surface *s);
void             OSDEP_UpdateRect(OSDEP_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);

uint32_t         OSDEP_GetTicks(void);

void             OSDEP_SetCaption(char *title, char *icon);

int              OSDEP_IsFullScreen(void);
OSDEP_VMode    **OSDEP_ListModes(void);
OSDEP_Surface   *OSDEP_SetVideoMode(int width, int height, int bpp, char fs);

int              OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors);

int32_t          OSDEP_NumJoysticks(void);
int              OSDEP_JoystickNumButtons(int n);
int              OSDEP_JoystickNumHats(int n);
int              OSDEP_JoystickNumAxes(int n);
OSDEP_Joystick  *OSDEP_JoystickOpen(int n);
void             OSDEP_JoystickClose(OSDEP_Joystick *joy);
uint8_t          OSDEP_JoystickGetButton(OSDEP_Joystick *joystick, int button);
int16_t          OSDEP_JoystickGetAxis(OSDEP_Joystick *joystick, int axis);
char            *OSDEP_JoystickName(int n);

#endif
