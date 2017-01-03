//#include <3ds.h>
#ifndef SDL
typedef unsigned int OSDEP_Joystick;
typedef struct _OSDEP_Buffer {
	int id;
	uint8_t *pixels;
	int pitch;
} OSDEP_Buffer;

#define OSDEP_Surface OSDEP_Buffer

typedef struct _OSDEP_Color {
	int r;
	int g;
	int b;
	int a;
} OSDEP_Color;


void OSDEP_Quit(void);

#endif

