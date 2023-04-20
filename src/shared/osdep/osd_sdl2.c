// OSDEP INCLUDE FILE
// CONFIGURED FOR SDL1.2

#include "osd_sdl2.h"

// key buffer
uint8_t OSDEP_key[2048];
SDL_Renderer* OSDEP_renderer=NULL;
SDL_Window* OSDEP_window=NULL;
SDL_Surface* OSDEP_buffer8=NULL;
SDL_Surface* OSDEP_buffer32=NULL;
SDL_Texture* OSDEP_texture=NULL;
char windowtitle[1024];

extern int vwidth;
extern int vheight;

void OSDEP_Init(void) {

// if(SDL_Init(SDL_INIT_VIDEO) < 0)
//     {
//         fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
//         // return 1;
//     }

    // if(SDL_Init(SDL_INIT_VIDEO) < 0)
    // {
    //     fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
    //     return;
    // }
	SDL_Init( SDL_INIT_EVERYTHING);
// on windows, get printf working.
#ifdef __WIN32
	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif

	//atexit(SDL_Quit);
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
	fprintf(stdout, "%s\n", __FUNCTION__);
	strcpy(windowtitle,title);
	SDL_WM_SetCaption((const char *)title, (const char *)icon);
}

OSDEP_VMode ** OSDEP_ListModes(void) {
fprintf(stdout, "%s\n", __FUNCTION__);
//	SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
	return NULL;
}

void OSDEP_WarpMouse(int x, int y) {
	SDL_WarpMouseInWindow(OSDEP_window, x, y);
}

int OSDEP_IsFullScreen(void) {
//	if (OSDEP_surface->flags & SDL_FULLSCREEN) return 1; // return true if surface is fullscreen
//    return 0;//OSDEP_window->fullscreen; // Return false if surface is windowed
    if(OSDEP_window != NULL) {
	    SDL_DestroyWindow(OSDEP_window);
    	OSDEP_window = NULL;
    }
    if(OSDEP_renderer !=NULL) {
    	SDL_DestroyRenderer(OSDEP_renderer);
    	OSDEP_renderer = NULL;
    }

    if(OSDEP_texture !=NULL) {
	    SDL_DestroyTexture(OSDEP_texture);
	    OSDEP_texture = NULL;
    }
    if(OSDEP_buffer32 !=NULL) {
    	SDL_FreeSurface(OSDEP_buffer32);
    	OSDEP_buffer32 = NULL;
    }
    if(OSDEP_buffer8 !=NULL) {
    	SDL_FreeSurface(OSDEP_buffer8);
    	OSDEP_buffer8 = NULL;
    }

    SDL_Quit();
    return 0;

}

void OSDEP_SetWindowSize(int w, int h) {
	SDL_SetWindowSize(OSDEP_window, w, h);
	SDL_RenderPresent(OSDEP_renderer);

	SDL_GetRendererOutputSize(OSDEP_renderer,
                       &vwidth, &vheight);

SDL_Log("VW: %d VH: %d W: %d H: %d\n",vwidth, vheight, w,h);

}

OSDEP_Surface * OSDEP_SetVideoModeX(int width, int height, int bpp, char fs) {
	fprintf(stdout, "Setting Video mode to %d %d\n", width, height);
// #include <SDL2/SDL.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_BPP 8

// int main(int argc, char* argv[])
// {
    SDL_Window* window = NULL;
    SDL_Surface* screen = NULL;
    SDL_Event event;
    int done = 0;

    srand(time(NULL));

    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Random Pixel Colors", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(window == NULL)
    {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        return 1;
    }

    screen = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_PIXELFORMAT_INDEX8);
    if(screen == NULL)
    {
        fprintf(stderr, "Unable to create surface: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Color palette[256];

    for(int i = 0; i < 256; i++)
    {
        palette[i].r = rand() % 256;
        palette[i].g = rand() % 256;
        palette[i].b = rand() % 256;
        palette[i].a = 255;
    }

    SDL_SetPaletteColors(screen->format->palette, palette, 0, 256);

    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                done = 1;
            }
        }

        Uint8* pixels = (Uint8*) screen->pixels;

        for(int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
        {
            pixels[i] = rand() % 256;
        }

        SDL_BlitSurface(screen, NULL, SDL_GetWindowSurface(window), NULL);
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(screen);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
// }

}


OSDEP_Surface * OSDEP_SetVideoMode(int width, int height, int bpp, char fs) {
	// width = 640;
	// height = 480;
	fprintf(stdout, "%s\n", __FUNCTION__);
	Uint32 rmask, gmask, bmask, amask;

	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		fs=fs;
	}


	SDL_Log("Setting Videomode to %d x %d\n", width, height);
    /* SDL interprets each pixel as a 32-bit number, so our masks must depend
       on the endianness (byte order) of the machine */

	   SDL_RendererInfo info = {};

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    if(OSDEP_window != NULL) {
//	    SDL_DestroyRenderer(OSDEP_renderer);
//    	SDL_DestroyWindow(OSDEP_window);
    	// if window smaller that vibile allowed, resize
    	// SDL_SetWindowSize(OSDEP_window,
     //                   width, height);

    } else {
		OSDEP_window= SDL_CreateWindow("Random Pixel Colors", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
		if(OSDEP_window == NULL)
    {
        fprintf(stderr, "Unable to create window: %s\n", SDL_GetError());
        return 1;
    }
		//SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED,
        //                                  SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

		// // Create a renderer
    	// OSDEP_renderer = SDL_CreateRenderer(OSDEP_window, -1, 0);

    	// SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &OSDEP_window, &OSDEP_renderer);
    	// hide mouse cursor
		// SDL_ShowCursor(SDL_DISABLE);

	}

	// SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
//  	SDL_UpdateWindowSurface(OSDEP_window);

//	if(!OSDEP_renderer) {
//		printf("Failed to create renderer\n");
//	}

	if(!OSDEP_window) {
		printf("Failed to create window\n");
	}

	// if(SDL_GetRenderDriverInfo(0, &info) == 0) {
	// 	int supported = 0;
	// 	for(int i = 0; i < info.num_texture_formats; ++i) {
	// 		if(info.texture_formats[i] == SDL_PIXELFORMAT_INDEX8) {
	// 			supported = 1;
	// 			break;
	// 		}
	// 	}
	// 	printf("Supported: %s\n", supported ? "YES" : "NO");
	// } else {
	// 	printf("Failed: %s\n", SDL_GetError());
	// }

    // if(OSDEP_texture !=NULL) {
	//     SDL_DestroyTexture(OSDEP_texture);
	//     OSDEP_texture = NULL;
    // }
    // if(OSDEP_buffer32 !=NULL) {
    // 	SDL_FreeSurface(OSDEP_buffer32);
    // 	OSDEP_buffer32 = NULL;
    // }

    if(OSDEP_buffer8 !=NULL) {
    	SDL_FreeSurface(OSDEP_buffer8);
    	OSDEP_buffer8 = NULL;
    }


    OSDEP_buffer8 = SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_INDEX8);

	SDL_Color palette[256];

    for(int i = 0; i < 256; i++)
    {
        palette[i].r = rand() % 256;
        palette[i].g = rand() % 256;
        palette[i].b = rand() % 256;
        palette[i].a = 255;
    }

    SDL_SetPaletteColors(OSDEP_buffer8->format->palette, palette, 0, 256);

//	OSDEP_buffer8 = SDL_CreateRGBSurfaceWithFormat(0, width, height, 8, SDL_PIXELFORMAT_INDEX8);

	// SDL_CreateRGBSurface(0, width, height, 8,
		// 0,0,0,0);
	
	if(!OSDEP_buffer8) {
		printf("Failed to create 8bit buffer\n");
	}
//	OSDEP_buffer32 = SDL_GetWindowSurface(OSDEP_window);
	// OSDEP_buffer32 = SDL_CreateRGBSurface(0, width, height, 32,
		// 0,0,0,0);
		// rmask,
		// gmask,
		// bmask,
		// amask);

	if(!OSDEP_buffer32) {
		printf("Failed to create 32bit buffer\n");
	}

//	OSDEP_texture = SDL_CreateTextureFromSurface(OSDEP_renderer, OSDEP_buffer32);

//	if(!OSDEP_texture) {
//		printf("Failed to create texture\n");
//	}

/*
int done = 0;

OSDEP_Flip(OSDEP_buffer8);
    while(!done)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                done = 1;
            }
        }

        Uint8* pixels = (Uint8*) OSDEP_buffer8->pixels;

        for(int i = 0; i < width * height; i++)
        {
            pixels[i] = rand() % 256;
        }

        SDL_BlitSurface(OSDEP_buffer8, NULL, SDL_GetWindowSurface(OSDEP_window), NULL);
        SDL_UpdateWindowSurface(OSDEP_window);
    }

    // SDL_FreeSurface(screen);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
*/
	return OSDEP_buffer8;
}

extern int vga_an;
extern int vga_al;

void OSDEP_Flip(OSDEP_Surface *s) {
//	fprintf(stdout, "%s\n", __FUNCTION__);

// SDL_Color palette[256];

//     for(int i = 0; i < 256; i++)
//     {
//         palette[i].r = rand() % 256;
//         palette[i].g = rand() % 256;
//         palette[i].b = rand() % 256;
//         palette[i].a = 255;
//     }

//     SDL_SetPaletteColors(s->format->palette, palette, 0, 256);

// 	Uint8* pixels = (Uint8*) s->pixels;

//         for(int i = 0; i < vga_an * vga_al; i++)
//         {
//             pixels[i] = rand() % 256;
//         }

	SDL_BlitSurface(OSDEP_buffer8, NULL, SDL_GetWindowSurface(OSDEP_window), NULL);
	SDL_UpdateWindowSurface(OSDEP_window);
    // SDL_Log("Updateing Texture\n");

	return;

}

void blah(void) {
	void *pixels; 
  int pitch; 

  /* blit the 320x200x8 Surface to a 32bit helper                */ 
  /* Surface for pixel format conversion (8bit palettized to     */ 
  /* 32bit). SDL2 Textures don't work with 8bit pixels formats.  */ 

 /* move it to the texture */ 
  SDL_BlitSurface(OSDEP_buffer8, NULL, OSDEP_buffer32, NULL);

  // SDL_LockTexture(OSDEP_texture, NULL, &pixels, &pitch); 
  // SDL_ConvertPixels(OSDEP_buffer32->w, OSDEP_buffer32->h, 
  //              OSDEP_buffer32->format->format, 
  //              OSDEP_buffer32->pixels, OSDEP_buffer32->pitch, 
  //              SDL_PIXELFORMAT_ARGB8888, 
  //              pixels, pitch); 
  // SDL_UnlockTexture(OSDEP_texture); 

	// int w=0;
	// int h=0;

	// for(w=0;w<OSDEP_buffer8->width;w++) {
	// 	for(w=0;w<OSDEP_buffer8->width;w++) {
			
	// 	}

	// }

    /* Modify the texture's pixels */
	SDL_UpdateTexture(OSDEP_texture, NULL, OSDEP_buffer32->pixels, OSDEP_buffer32->pitch);
    
    /* Make the modified texture visible by rendering it */
    SDL_RenderCopy(OSDEP_renderer, OSDEP_texture, NULL, NULL);

	SDL_RenderPresent(OSDEP_renderer);
}

void OSDEP_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
	// fprintf(stdout, "%s\n", __FUNCTION__);
	OSDEP_Flip(screen);
//	SDL_RenderPresent(OSDEP_renderer);	
}

int OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors) {
	fprintf(stdout, "%s\n", __FUNCTION__);
//	return 0;

	SDL_SetPaletteColors(surface->format->palette, colors, 0, 256);
	return 1;
}

// Joysticks
int32_t OSDEP_NumJoysticks(void) {
	fprintf(stdout, "%s\n", __FUNCTION__);
	return SDL_NumJoysticks();
}

int OSDEP_JoystickNumButtons(OSDEP_Joystick *n) {
	return SDL_JoystickNumButtons(n);
}

int OSDEP_JoystickNumHats(OSDEP_Joystick *n) {
	return SDL_JoystickNumHats(n);
}

int OSDEP_JoystickNumAxes(OSDEP_Joystick *n) {
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
	
	OSDEP_key[SDLK_ESCAPE]=1;
	OSDEP_key[SDLK_F1-0x3FFFFD1A]=59;
	OSDEP_key[SDLK_F2-0x3FFFFD1A]=60;
	OSDEP_key[SDLK_F3-0x3FFFFD1A]=61;
	OSDEP_key[SDLK_F4-0x3FFFFD1A]=62;
	OSDEP_key[SDLK_F5-0x3FFFFD1A]=63;
	OSDEP_key[SDLK_F6-0x3FFFFD1A]=64;
	OSDEP_key[SDLK_F7-0x3FFFFD1A]=65;
	OSDEP_key[SDLK_F8-0x3FFFFD1A]=66;
	OSDEP_key[SDLK_F9-0x3FFFFD1A]=67;
	OSDEP_key[SDLK_F10-0x3FFFFD1A]=68;
	OSDEP_key[SDLK_F11-0x3FFFFD1A]=87;
	OSDEP_key[SDLK_F12-0x3FFFFD1A]=88;
	OSDEP_key[SDLK_PRINTSCREEN-0x3FFFFD1A]=55;
	OSDEP_key[SDLK_SCROLLLOCK-0x3FFFFD1A]=70;

	OSDEP_key[SDLK_BACKQUOTE]=41;
	OSDEP_key[SDLK_UP-0x3FFFFD1A]=72;
	OSDEP_key[SDLK_DOWN-0x3FFFFD1A]=80;
	OSDEP_key[SDLK_LEFT-0x3FFFFD1A]=75;
	OSDEP_key[SDLK_RIGHT-0x3FFFFD1A]=77;

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

	OSDEP_key[SDLK_CAPSLOCK-0x3FFFFD1A]=58;
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

	OSDEP_key[SDLK_LSHIFT-0x3FFFFD1A]=42;
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
	OSDEP_key[SDLK_RSHIFT-0x3FFFFD1A]=54;

	OSDEP_key[SDLK_LCTRL-0x3FFFFD1A]=29;
	OSDEP_key[SDLK_RCTRL-0x3FFFFD1A]=29;
	OSDEP_key[SDLK_LALT-0x3FFFFD1A]=56;
	OSDEP_key[SDLK_RALT-0x3FFFFD1A]=56;
	OSDEP_key[SDLK_SPACE]=57;
	OSDEP_key[SDLK_INSERT-0x3FFFFD1A]=82;
	OSDEP_key[SDLK_HOME-0x3FFFFD1A]=71;
	OSDEP_key[SDLK_PAGEUP-0x3FFFFD1A]=73;
	OSDEP_key[SDLK_DELETE]=83;
	OSDEP_key[SDLK_END-0x3FFFFD1A]=79;
	OSDEP_key[SDLK_PAGEDOWN-0x3FFFFD1A]=81;

	OSDEP_key[SDLK_NUMLOCKCLEAR-0x3FFFFD1A]=69;

	OSDEP_key[SDLK_KP_DIVIDE-0x3FFFFD1A]=53;
	OSDEP_key[SDLK_KP_MULTIPLY-0x3FFFFD1A]=53;
	OSDEP_key[SDLK_KP_MINUS-0x3FFFFD1A]=74;

	OSDEP_key[SDLK_BACKSLASH]=53;
	OSDEP_key[SDLK_KP_ENTER-0x3FFFFD1A]=28;
	OSDEP_key[SDLK_KP_MULTIPLY-0x3FFFFD1A]=55;
	OSDEP_key[SDLK_KP_MINUS-0x3FFFFD1A]=74;
	OSDEP_key[SDLK_KP_PLUS-0x3FFFFD1A]=78;
	OSDEP_key[SDLK_KP_ENTER-0x3FFFFD1A]=28;


//OSDEP_key[SDLK_LSHIFT-0x3FFFFD1A]=43;

#ifdef NOTYET
//const _wave=41
	const _c_center=76
#endif


}
