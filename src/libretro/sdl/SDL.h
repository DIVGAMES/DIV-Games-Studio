/* SDL 1.2 types and calls used by the DIV runtime, implemented for libretro.
   This header is not libSDL. The core includes it ahead of any system SDL. */
#ifndef _SDL_H
#define _SDL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t Uint8;
typedef int8_t Sint8;
typedef uint16_t Uint16;
typedef int16_t Sint16;
typedef uint32_t Uint32;
typedef int32_t Sint32;

#define SDL_MAJOR_VERSION 1
#define SDL_MINOR_VERSION 2
#define SDL_PATCHLEVEL 15

#define SDL_LIL_ENDIAN 1234
#define SDL_BIG_ENDIAN 4321
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SDL_BYTEORDER SDL_BIG_ENDIAN
#else
#define SDL_BYTEORDER SDL_LIL_ENDIAN
#endif

#define SDL_INIT_TIMER 0x00000001u
#define SDL_INIT_AUDIO 0x00000010u
#define SDL_INIT_VIDEO 0x00000020u
#define SDL_INIT_JOYSTICK 0x00000200u
#define SDL_INIT_EVERYTHING 0x0000FFFFu

#define SDL_SWSURFACE 0x00000000u
#define SDL_HWSURFACE 0x00000001u
#define SDL_ASYNCBLIT 0x00000004u
#define SDL_OPENGL 0x00000002u
#define SDL_FULLSCREEN 0x80000000u
#define SDL_RESIZABLE 0x00000010u
#define SDL_HWPALETTE 0x20000000u
#define SDL_DOUBLEBUF 0x40000000u
#define SDL_SRCCOLORKEY 0x00001000u
#define SDL_RLEACCEL 0x00004000u
#define SDL_SRCALPHA 0x00010000u
#define SDL_PREALLOC 0x01000000u

#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02
#define SDL_DISABLE 0
#define SDL_ENABLE 1
#define SDL_PRESSED 1
#define SDL_RELEASED 0
#define SDL_DEFAULT_REPEAT_INTERVAL 30

#define SDL_GRAB_QUERY -1
#define SDL_GRAB_OFF 0
#define SDL_GRAB_ON 1

#define SDL_HAT_CENTERED 0x00
#define SDL_HAT_UP 0x01
#define SDL_HAT_RIGHT 0x02
#define SDL_HAT_DOWN 0x04
#define SDL_HAT_LEFT 0x08

#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3

typedef int SDLKey;
typedef int SDLMod;

#define SDLK_UNKNOWN 0
#define SDLK_BACKSPACE 8
#define SDLK_TAB 9
#define SDLK_RETURN 13
#define SDLK_PAUSE 19
#define SDLK_ESCAPE 27
#define SDLK_SPACE 32
#define SDLK_QUOTE 39
#define SDLK_PLUS 43
#define SDLK_COMMA 44
#define SDLK_MINUS 45
#define SDLK_PERIOD 46
#define SDLK_SLASH 47
#define SDLK_0 48
#define SDLK_1 49
#define SDLK_2 50
#define SDLK_3 51
#define SDLK_4 52
#define SDLK_5 53
#define SDLK_6 54
#define SDLK_7 55
#define SDLK_8 56
#define SDLK_9 57
#define SDLK_SEMICOLON 59
#define SDLK_BACKSLASH 92
#define SDLK_BACKQUOTE 96
#define SDLK_a 97
#define SDLK_b 98
#define SDLK_c 99
#define SDLK_d 100
#define SDLK_e 101
#define SDLK_f 102
#define SDLK_g 103
#define SDLK_h 104
#define SDLK_i 105
#define SDLK_j 106
#define SDLK_k 107
#define SDLK_l 108
#define SDLK_m 109
#define SDLK_n 110
#define SDLK_o 111
#define SDLK_p 112
#define SDLK_q 113
#define SDLK_r 114
#define SDLK_s 115
#define SDLK_t 116
#define SDLK_u 117
#define SDLK_v 118
#define SDLK_w 119
#define SDLK_x 120
#define SDLK_y 121
#define SDLK_z 122
#define SDLK_DELETE 127
#define SDLK_KP_DIVIDE 267
#define SDLK_KP_MULTIPLY 268
#define SDLK_KP_MINUS 269
#define SDLK_KP_PLUS 270
#define SDLK_KP_ENTER 271
#define SDLK_UP 273
#define SDLK_DOWN 274
#define SDLK_RIGHT 275
#define SDLK_LEFT 276
#define SDLK_INSERT 277
#define SDLK_HOME 278
#define SDLK_END 279
#define SDLK_PAGEUP 280
#define SDLK_PAGEDOWN 281
#define SDLK_F1 282
#define SDLK_F2 283
#define SDLK_F3 284
#define SDLK_F4 285
#define SDLK_F5 286
#define SDLK_F6 287
#define SDLK_F7 288
#define SDLK_F8 289
#define SDLK_F9 290
#define SDLK_F10 291
#define SDLK_F11 292
#define SDLK_F12 293
#define SDLK_NUMLOCK 300
#define SDLK_CAPSLOCK 301
#define SDLK_SCROLLOCK 302
#define SDLK_RSHIFT 303
#define SDLK_LSHIFT 304
#define SDLK_RCTRL 305
#define SDLK_LCTRL 306
#define SDLK_RALT 307
#define SDLK_LALT 308
#define SDLK_PRINT 316
#define SDLK_LEFTBRACKET 91
#define SDLK_RIGHTBRACKET 93

#define KMOD_NONE 0x0000
#define KMOD_LSHIFT 0x0001
#define KMOD_RSHIFT 0x0002
#define KMOD_LCTRL 0x0040
#define KMOD_RCTRL 0x0080
#define KMOD_LALT 0x0100
#define KMOD_RALT 0x0200
#define KMOD_NUM 0x1000
#define KMOD_CAPS 0x2000
#define KMOD_CTRL (KMOD_LCTRL | KMOD_RCTRL)
#define KMOD_SHIFT (KMOD_LSHIFT | KMOD_RSHIFT)
#define KMOD_ALT (KMOD_LALT | KMOD_RALT)

typedef struct SDL_Rect {
	Sint16 x, y;
	Uint16 w, h;
} SDL_Rect;

typedef struct SDL_Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 unused;
} SDL_Color;

typedef struct SDL_Palette {
	int ncolors;
	SDL_Color *colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
	SDL_Palette *palette;
	Uint8 BitsPerPixel;
	Uint8 BytesPerPixel;
	Uint8 Rloss, Gloss, Bloss, Aloss;
	Uint8 Rshift, Gshift, Bshift, Ashift;
	Uint32 Rmask, Gmask, Bmask, Amask;
	Uint32 colorkey;
	Uint8 alpha;
} SDL_PixelFormat;

typedef struct SDL_Surface {
	Uint32 flags;
	SDL_PixelFormat *format;
	int w, h;
	Uint16 pitch;
	void *pixels;
	SDL_Rect clip_rect;
	int refcount;
} SDL_Surface;

typedef struct SDL_keysym {
	Uint8 scancode;
	SDLKey sym;
	SDLMod mod;
	Uint16 unicode;
} SDL_keysym;

typedef struct SDL_KeyboardEvent {
	Uint8 type;
	Uint8 which;
	Uint8 state;
	SDL_keysym keysym;
} SDL_KeyboardEvent;

typedef struct SDL_ResizeEvent {
	Uint8 type;
	int w, h;
} SDL_ResizeEvent;

typedef struct SDL_MouseMotionEvent {
	Uint8 type;
	Uint8 which;
	Uint8 state;
	Uint16 x, y;
	Sint16 xrel, yrel;
} SDL_MouseMotionEvent;

typedef struct SDL_MouseButtonEvent {
	Uint8 type;
	Uint8 which;
	Uint8 button;
	Uint8 state;
	Uint16 x, y;
} SDL_MouseButtonEvent;

typedef struct SDL_JoyAxisEvent {
	Uint8 type;
	Uint8 which;
	Uint8 axis;
	Sint16 value;
} SDL_JoyAxisEvent;

typedef struct SDL_JoyButtonEvent {
	Uint8 type;
	Uint8 which;
	Uint8 button;
	Uint8 state;
} SDL_JoyButtonEvent;

typedef union SDL_Event {
	Uint8 type;
	SDL_KeyboardEvent key;
	SDL_ResizeEvent resize;
	SDL_MouseMotionEvent motion;
	SDL_MouseButtonEvent button;
	SDL_JoyAxisEvent jaxis;
	SDL_JoyButtonEvent jbutton;
} SDL_Event;

#define SDL_NOEVENT 0
#define SDL_KEYDOWN 2
#define SDL_KEYUP 3
#define SDL_MOUSEMOTION 4
#define SDL_MOUSEBUTTONDOWN 5
#define SDL_MOUSEBUTTONUP 6
#define SDL_JOYAXISMOTION 7
#define SDL_JOYBUTTONDOWN 10
#define SDL_JOYBUTTONUP 11
#define SDL_QUIT 12
#define SDL_VIDEORESIZE 16

typedef struct SDL_version {
	Uint8 major;
	Uint8 minor;
	Uint8 patch;
} SDL_version;

struct SDL_RWops;
typedef struct SDL_RWops SDL_RWops;
struct SDL_RWops {
	int (*seek)(struct SDL_RWops *context, int offset, int whence);
	int (*read)(struct SDL_RWops *context, void *ptr, int size, int maxnum);
	int (*write)(struct SDL_RWops *context, const void *ptr, int size, int num);
	int (*close)(struct SDL_RWops *context);
	Uint32 type;
	struct {
		Uint8 *base;
		Uint8 *here;
		Uint8 *stop;
	} hidden;
};

typedef struct _SDL_Joystick SDL_Joystick;

#define SDL_MUSTLOCK(s) ((s) && ((s)->flags & (SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_RLEACCEL)))

int SDL_Init(Uint32 flags);
void SDL_Quit(void);
char *SDL_GetError(void);
Uint32 SDL_GetTicks(void);
void SDL_Delay(Uint32 ms);
int SDL_putenv(const char *variable);
int SDL_ShowCursor(int toggle);
void SDL_WM_SetCaption(const char *title, const char *icon);
void SDL_WarpMouse(Uint16 x, Uint16 y);
int SDL_WM_GrabInput(int mode);
int SDL_EnableUNICODE(int enable);
int SDL_EnableKeyRepeat(int delay, int interval);

SDL_Surface *SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth,
	Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
void SDL_FreeSurface(SDL_Surface *surface);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
SDL_Rect **SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);
int SDL_Flip(SDL_Surface *screen);
void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h);
int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
int SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors);
int SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
Uint32 SDL_MapRGB(const SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b);
void SDL_GetRGB(Uint32 pixel, const SDL_PixelFormat *fmt, Uint8 *r, Uint8 *g, Uint8 *b);
int SDL_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);
int SDL_SaveBMP(SDL_Surface *surface, const char *file);
SDL_Surface *SDL_LoadBMP_RW(SDL_RWops *src, int freesrc);

SDL_RWops *SDL_RWFromMem(void *mem, int size);
void SDL_FreeRW(SDL_RWops *area);

int SDL_PollEvent(SDL_Event *event);
int SDL_PushEvent(SDL_Event *event);

int SDL_NumJoysticks(void);
SDL_Joystick *SDL_JoystickOpen(int index);
void SDL_JoystickClose(SDL_Joystick *joystick);
const char *SDL_JoystickName(int index);
int SDL_JoystickNumButtons(SDL_Joystick *joystick);
int SDL_JoystickNumAxes(SDL_Joystick *joystick);
int SDL_JoystickNumHats(SDL_Joystick *joystick);
Uint8 SDL_JoystickGetButton(SDL_Joystick *joystick, int button);
Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);
Uint8 SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);

#ifdef __cplusplus
}
#endif

#endif
