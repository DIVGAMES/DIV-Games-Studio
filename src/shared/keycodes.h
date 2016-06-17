uint8_t sdl2key[2048];
//#define _s sdl2key

void sdlkeyinit(void) {
	printf("SDL KEY INIT\n");
//	exit(0);
#ifndef __EMSCRIPTEN__
	SDL_EnableUNICODE( SDL_ENABLE );  
#endif
	SDL_EnableKeyRepeat( 250,  SDL_DEFAULT_REPEAT_INTERVAL);
	
sdl2key[SDLK_ESCAPE]=1;
sdl2key[SDLK_F1]=59;
sdl2key[SDLK_F2]=60;
sdl2key[SDLK_F3]=61;
sdl2key[SDLK_F4]=62;
sdl2key[SDLK_F5]=63;
sdl2key[SDLK_F6]=64;
sdl2key[SDLK_F7]=65;
sdl2key[SDLK_F8]=66;
sdl2key[SDLK_F9]=67;
sdl2key[SDLK_F10]=68;
sdl2key[SDLK_F11]=87;
sdl2key[SDLK_F12]=88;
sdl2key[SDLK_PRINT]=55;
sdl2key[SDLK_SCROLLOCK]=70;

sdl2key[SDLK_BACKQUOTE]=41;
sdl2key[SDLK_UP]=72;
sdl2key[SDLK_DOWN]=80;
sdl2key[SDLK_LEFT]=75;
sdl2key[SDLK_RIGHT]=77;

sdl2key[SDLK_1]=2;
sdl2key[SDLK_2]=3;
sdl2key[SDLK_3]=4;
sdl2key[SDLK_4]=5;
sdl2key[SDLK_5]=6;
sdl2key[SDLK_6]=7;
sdl2key[SDLK_7]=8;
sdl2key[SDLK_8]=9;
sdl2key[SDLK_9]=10;
sdl2key[SDLK_0]=11;
sdl2key[SDLK_MINUS]=12;
sdl2key[SDLK_PLUS]=13;
sdl2key[SDLK_BACKSPACE]=14;


sdl2key[SDLK_TAB]=15;
sdl2key[SDLK_q]=16;
sdl2key[SDLK_w]=17;
sdl2key[SDLK_e]=18;
sdl2key[SDLK_r]=19;
sdl2key[SDLK_t]=20;
sdl2key[SDLK_y]=21;
sdl2key[SDLK_u]=22;
sdl2key[SDLK_i]=23;
sdl2key[SDLK_o]=24;
sdl2key[SDLK_p]=25;
sdl2key[SDLK_LEFTBRACKET]=26;
sdl2key[SDLK_RIGHTBRACKET]=27;
sdl2key[SDLK_RETURN]=28;

sdl2key[SDLK_CAPSLOCK]=58;
sdl2key[SDLK_a]=30;
sdl2key[SDLK_s]=31;
sdl2key[SDLK_d]=32;
sdl2key[SDLK_f]=33;
sdl2key[SDLK_g]=34;
sdl2key[SDLK_h]=35;
sdl2key[SDLK_j]=36;
sdl2key[SDLK_k]=37;
sdl2key[SDLK_l]=38;
sdl2key[SDLK_SEMICOLON]=39;
sdl2key[SDLK_QUOTE]=40;
sdl2key[SDLK_BACKSLASH]=43;

sdl2key[SDLK_LSHIFT]=42;
sdl2key[SDLK_z]=44;
sdl2key[SDLK_x]=45;
sdl2key[SDLK_c]=46;
sdl2key[SDLK_v]=47;
sdl2key[SDLK_b]=48;
sdl2key[SDLK_n]=49;
sdl2key[SDLK_m]=50;
sdl2key[SDLK_COMMA]=51;
sdl2key[SDLK_PERIOD]=51;
sdl2key[SDLK_SLASH]=51;
sdl2key[SDLK_RSHIFT]=54;

sdl2key[SDLK_LCTRL]=29;
sdl2key[SDLK_RCTRL]=29;
sdl2key[SDLK_LALT]=56;
sdl2key[SDLK_RALT]=56;
sdl2key[SDLK_SPACE]=57;
sdl2key[SDLK_INSERT]=82;
sdl2key[SDLK_HOME]=71;
sdl2key[SDLK_PAGEUP]=73;
sdl2key[SDLK_DELETE]=83;
sdl2key[SDLK_END]=79;
sdl2key[SDLK_PAGEDOWN]=81;

sdl2key[SDLK_NUMLOCK]=69;

sdl2key[SDLK_KP_DIVIDE]=53;
sdl2key[SDLK_KP_MULTIPLY]=53;
sdl2key[SDLK_KP_MINUS]=74;

sdl2key[SDLK_NUMLOCK]=69;
sdl2key[SDLK_BACKSLASH]=53;
sdl2key[SDLK_KP_ENTER]=28;
sdl2key[SDLK_KP_MULTIPLY]=55;
sdl2key[SDLK_KP_MINUS]=74;
sdl2key[SDLK_KP_PLUS]=78;
sdl2key[SDLK_KP_ENTER]=28;


//sdl2key[SDLK_LSHIFT]=43;

#ifdef NOTYET
//const _wave=41
const _c_center=76
#endif
}
