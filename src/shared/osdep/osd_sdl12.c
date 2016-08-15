// OSDEP INCLUDE FILE
// CONFIGURED FOR SDL1.2

#include "osd_sdl12.h"


// key buffer
uint8_t OSDEP_key[2048];
OSDEP_Surface *OSDEP_surface;

// startup / shutdown

void OSDEP_Init(void) {

#if !defined (GP2X) && !defined (PS2) && !defined (PSP) 
  SDL_putenv("SDL_VIDEO_WINDOW_POS=center"); 
#endif

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

OSDEP_VMode ** OSDEP_ListModes(void) {
	SDL_Rect **modes;
	int i;
	OSDEP_VMode *smodes[1024];
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	

	if(modes == (SDL_Rect **)0) {
		return NULL;
	}

	if(modes == (SDL_Rect **)-1) {
		return (OSDEP_VMode **)-1;
	}

	for(i=0;modes[i];++i) {
		if(modes[i]) {
			smodes[i]=(OSDEP_VMode*)malloc(sizeof(OSDEP_VMode));
			smodes[i]->w = modes[i]->w;
			smodes[i]->h = modes[i]->h;
		} else {
			smodes[i]=NULL;
		}
	}

	return smodes;
}

void OSDEP_WarpMouse(int x, int y) {
	SDL_WarpMouse(x, y);
}

int OSDEP_IsFullScreen(void) {
	if (OSDEP_surface->flags & SDL_FULLSCREEN) return 1; // return true if surface is fullscreen
    return 0; // Return false if surface is windowed

}

OSDEP_Surface * OSDEP_SetVideoMode(int width, int height, int bpp, char fs) {
	uint32_t flags;
	if(fs) {
		flags = SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF;
	} else {
		flags = SDL_SWSURFACE | SDL_RESIZABLE;
	}
	OSDEP_surface = SDL_SetVideoMode(width, height, bpp, flags);
	return OSDEP_surface;
}


void OSDEP_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	SDL_UpdateRect(screen, x, y, w, h);
}

void OSDEP_Flip(OSDEP_Surface *s) {
	SDL_Flip(s);
}

int OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors) {
	return SDL_SetPalette(surface, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256);
}

// Joysticks
#ifdef JOYSTICK
int32_t OSDEP_NumJoysticks(void) {
	return SDL_NumJoysticks();
}

int OSDEP_JoystickNumButtons(int n) {
	return SDL_JoystickNumButtons((SDL_Joystick *)n);
}

int OSDEP_JoystickNumHats(int n) {
	return SDL_JoystickNumHats((SDL_Joystick *)n);
}

int OSDEP_JoystickNumAxes(int n) {
	return SDL_JoystickNumAxes((SDL_Joystick *)n);
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

char * OSDEP_JoystickName(OSDEP_Joystick *n) {
	return SDL_JoystickName(n);
}

#endif

void OSDEP_keyInit(void) {

	printf("SDL KEY INIT\n");
//	exit(0);
#ifndef __EMSCRIPTEN__
	SDL_EnableUNICODE( SDL_ENABLE );  
#endif
	SDL_EnableKeyRepeat( 250,  SDL_DEFAULT_REPEAT_INTERVAL);
	
	OSDEP_key[SDLK_ESCAPE]=1;
	OSDEP_key[SDLK_F1]=59;
	OSDEP_key[SDLK_F2]=60;
	OSDEP_key[SDLK_F3]=61;
	OSDEP_key[SDLK_F4]=62;
	OSDEP_key[SDLK_F5]=63;
	OSDEP_key[SDLK_F6]=64;
	OSDEP_key[SDLK_F7]=65;
	OSDEP_key[SDLK_F8]=66;
	OSDEP_key[SDLK_F9]=67;
	OSDEP_key[SDLK_F10]=68;
	OSDEP_key[SDLK_F11]=87;
	OSDEP_key[SDLK_F12]=88;
	OSDEP_key[SDLK_PRINT]=55;
	OSDEP_key[SDLK_SCROLLOCK]=70;

	OSDEP_key[SDLK_BACKQUOTE]=41;
	OSDEP_key[SDLK_UP]=72;
	OSDEP_key[SDLK_DOWN]=80;
	OSDEP_key[SDLK_LEFT]=75;
	OSDEP_key[SDLK_RIGHT]=77;

	OSDEP_key[SDLK_1]=2;
	OSDEP_key[SDLK_2]=3;
	OSDEP_key[SDLK_3]=4;
	OSDEP_key[SDLK_4]=5;
	OSDEP_key[SDLK_5]=6;
	OSDEP_key[SDLK_6]=7;
	OSDEP_key[SDLK_7]=8;
	OSDEP_key[SDLK_8]=9;
	OSDEP_key[SDLK_9]=10;
	OSDEP_key[SDLK_0]=11;
	OSDEP_key[SDLK_MINUS]=12;
	OSDEP_key[SDLK_PLUS]=13;
	OSDEP_key[SDLK_BACKSPACE]=14;


	OSDEP_key[SDLK_TAB]=15;
	OSDEP_key[SDLK_q]=16;
	OSDEP_key[SDLK_w]=17;
	OSDEP_key[SDLK_e]=18;
	OSDEP_key[SDLK_r]=19;
	OSDEP_key[SDLK_t]=20;
	OSDEP_key[SDLK_y]=21;
	OSDEP_key[SDLK_u]=22;
	OSDEP_key[SDLK_i]=23;
	OSDEP_key[SDLK_o]=24;
	OSDEP_key[SDLK_p]=25;
	OSDEP_key[SDLK_LEFTBRACKET]=26;
	OSDEP_key[SDLK_RIGHTBRACKET]=27;
	OSDEP_key[SDLK_RETURN]=28;

	OSDEP_key[SDLK_CAPSLOCK]=58;
	OSDEP_key[SDLK_a]=30;
	OSDEP_key[SDLK_s]=31;
	OSDEP_key[SDLK_d]=32;
	OSDEP_key[SDLK_f]=33;
	OSDEP_key[SDLK_g]=34;
	OSDEP_key[SDLK_h]=35;
	OSDEP_key[SDLK_j]=36;
	OSDEP_key[SDLK_k]=37;
	OSDEP_key[SDLK_l]=38;
	OSDEP_key[SDLK_SEMICOLON]=39;
	OSDEP_key[SDLK_QUOTE]=40;
	OSDEP_key[SDLK_BACKSLASH]=43;

	OSDEP_key[SDLK_LSHIFT]=42;
	OSDEP_key[SDLK_z]=44;
	OSDEP_key[SDLK_x]=45;
	OSDEP_key[SDLK_c]=46;
	OSDEP_key[SDLK_v]=47;
	OSDEP_key[SDLK_b]=48;
	OSDEP_key[SDLK_n]=49;
	OSDEP_key[SDLK_m]=50;
	OSDEP_key[SDLK_COMMA]=51;
	OSDEP_key[SDLK_PERIOD]=51;
	OSDEP_key[SDLK_SLASH]=51;
	OSDEP_key[SDLK_RSHIFT]=54;

	OSDEP_key[SDLK_LCTRL]=29;
	OSDEP_key[SDLK_RCTRL]=29;
	OSDEP_key[SDLK_LALT]=56;
	OSDEP_key[SDLK_RALT]=56;
	OSDEP_key[SDLK_SPACE]=57;
	OSDEP_key[SDLK_INSERT]=82;
	OSDEP_key[SDLK_HOME]=71;
	OSDEP_key[SDLK_PAGEUP]=73;
	OSDEP_key[SDLK_DELETE]=83;
	OSDEP_key[SDLK_END]=79;
	OSDEP_key[SDLK_PAGEDOWN]=81;

	OSDEP_key[SDLK_NUMLOCK]=69;

	OSDEP_key[SDLK_KP_DIVIDE]=53;
	OSDEP_key[SDLK_KP_MULTIPLY]=53;
	OSDEP_key[SDLK_KP_MINUS]=74;

	OSDEP_key[SDLK_NUMLOCK]=69;
	OSDEP_key[SDLK_BACKSLASH]=53;
	OSDEP_key[SDLK_KP_ENTER]=28;
	OSDEP_key[SDLK_KP_MULTIPLY]=55;
	OSDEP_key[SDLK_KP_MINUS]=74;
	OSDEP_key[SDLK_KP_PLUS]=78;
	OSDEP_key[SDLK_KP_ENTER]=28;


//OSDEP_key[SDLK_LSHIFT]=43;

#ifdef NOTYET
//const _wave=41
const _c_center=76
#endif

}