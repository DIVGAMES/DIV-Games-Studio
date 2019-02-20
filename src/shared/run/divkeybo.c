//════════════════════════════════════════════════════════════════════════════
// Pruebas con la interrupci¢n de teclado
//════════════════════════════════════════════════════════════════════════════
#include "inter.h"
#include "osdep.h"

#ifdef GCW
// GCW DEFAULT BUTTON MAPPING
/*
#define GCW_L      SDLK_TAB
#define GCW_R S    DLK_BACKSPACE
#define GCW_UP     SDLK_UP
#define GCW_DOWN   SDLK_DOWN
#define GCW_LEFT   SDLK_LEFT
#define GCW_RIGHT  SDLK_RIGHT
#define GCW_A      SDLK_LCTRL
#define GCW_B      SDLK_LALT
#define GCW_X      SDLK_LSHIFT
#define GCW_Y      SDLK_SPACE
#define GCW_START  SDLK_RETURN
#define GCW_SELECT SDLK_ESCAPE
#define GCW_LOCK   SDLK_PAUSE
*/
#define GCW_L      SDLK_TAB
#define GCW_R      SDLK_BACKSPACE
#define GCW_UP     SDLK_UP
#define GCW_DOWN   SDLK_DOWN
#define GCW_LEFT   SDLK_LEFT
#define GCW_RIGHT  SDLK_RIGHT
#define GCW_A      SDLK_LCTRL
#define GCW_B      SDLK_RSHIFT
#define GCW_X      SDLK_LSHIFT
#define GCW_Y      SDLK_SPACE
#define GCW_START  SDLK_RETURN
#define GCW_SELECT SDLK_ESCAPE
#define GCW_LOCK   SDLK_PAUSE
#endif

#define JOY_DEADZONE 500

int joymx = 0, joymy = 0;

#ifdef DEBUG
extern int mouse_b; //, mouse_y;
#endif


//════════════════════════════════════════════════════════════════════════════
// IRQ Data
//════════════════════════════════════════════════════════════════════════════
typedef void (__interrupt __far *TIRQHandler)(void);

TIRQHandler OldIrqHandler;
TIRQHandler OldIrq23;
TIRQHandler OldIrq1b;

byte *shift = (byte *)0x417; // Shift status

byte  buf[64 * 3]; // {ascii, scan_code, shift_status}
int  ibuf = 0;     // Puntero al buffer, inicio de la cola
int  fbuf = 0;     // Puntero al buffer, fin de la cola

//════════════════════════════════════════════════════════════════════════════
// Interrupt handler related functions.
//════════════════════════════════════════════════════════════════════════════
int ctrl_c = 0, alt_x = 0;

//word *kb_start = (void*) 0x41a;
//word *kb_end   = (void*) 0x41c;

void __far __interrupt __loadds IrqHandler(void) {
#ifdef DOS
	int n;
	int scancode;

	scancode = inp(0x60);
	if(((*shift) & 4) && scancode == 46) {
		kbdFLAGS[46]  =1;
		ctrl_c       = 1;

		outp(0x20, 0x20);
	}
	else {
		if(scancode < 128) {
			kbdFLAGS[scancode]=1;
		}
		else if(scancode > 129) {
			kbdFLAGS[scancode - 128] = 0;
		}
		else {
			n = 128;
			do {
				kbdFLAGS[--n] = 0;
			}while(n);
		}

//		if(kbhit()) {
//			getch();
//		}

		OldIrqHandler();

		if((*kb_end - 28) % 32 + 30 == *kb_start) {
			*kb_start = (*kb_start - 28) % 32 + 30;
		}
	}
#endif
}

void __far __interrupt __loadds Irq23(void) {}
void __far __interrupt __loadds Irq1b(void) {}

//════════════════════════════════════════════════════════════════════════════
// Obtains the address of an IRQ handler.
//════════════════════════════════════════════════════════════════════════════
TIRQHandler GetIRQVector(int n) {
#ifdef DOS
	struct SREGS sregs;
	union REGS inregs, outregs;

	// DOS4GW redefines the DOS get vector call.
	inregs.x.eax = 0x3500 + n;
	sregs.ds     = 0;
	sregs.es     = 0;
	int386x(0x21,
	       &inregs,
	       &outregs,
	       &sregs);

	return(TIRQHandler)(MK_FP((word)sregs.es, outregs.x.ebx));
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Sets the address of an IRQ handler.
//════════════════════════════════════════════════════════════════════════════
void SetIRQVector(int n, TIRQHandler vec) {
#ifdef DOS
	struct SREGS sregs;
	union REGS inregs, outregs;

	// DOS set vector call.
	inregs.x.eax = 0x2500 + n;
	inregs.x.edx = FP_OFF (vec);
	// Handler pointer.
	sregs.ds     = FP_SEG (vec);
	sregs.es     = 0;
	int386x(0x21,
	       &inregs,
	       &outregs,
	       &sregs);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Function to initialize the handler.
//════════════════════════════════════════════════════════════════════════════
void kbdInit(void) {
	OSDEP_keyInit();

#ifdef DOS
	// If not already installed.
	if(GetIRQVector(9) != IrqHandler) {
			// Get old handler.
			OldIrqHandler = GetIRQVector(9);
			// Set our handler.
			SetIRQVector(9, IrqHandler);

			// Get old handler.
			OldIrq1b = GetIRQVector(0x1b);
			// Set our handler.
			SetIRQVector(0x1b, Irq1b);

			// Get old handler.
			OldIrq23 = GetIRQVector(0x23);
			// Set our handler.
			SetIRQVector(0x23, Irq23);
		}

		signal(SIGBREAK, SIG_IGN);
		signal(SIGINT  , SIG_IGN);
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Function to uninitialize the handler.
//════════════════════════════════════════════════════════════════════════════
void kbdReset(void) {
#ifdef DOS
		// If it was installed.
		if(GetIRQVector(9) == IrqHandler) {
			// Uninstall.
			SetIRQVector(9,    OldIrqHandler);
			SetIRQVector(0x1b, OldIrq1b);
			SetIRQVector(0x23, OldIrq23);
		}
#endif
}

//════════════════════════════════════════════════════════════════════════════
// Espera una tecla, actualiza variables (ascii, scan_code y shift_status)
//════════════════════════════════════════════════════════════════════════════
void tecla_bios(void) {
#ifdef DOS
	int as, sc, ss;
	union REGS r;
	struct SREGS s;

	if(_bios_keybrd(_KEYBRD_READY)) {
		s.ds = s.es = s.fs = s.gs = FP_SEG(&s);
		do {
			r.h.ah = 0;
			int386x(0x16,
			       &r,
			       &r,
			       &s);

			as     = r.h.al;
			sc     = r.h.ah;
			r.h.ah = 2;
			int386x(0x16,
			       &r,
			       &r,
			       &s);
			ss     = r.h.al;

			buf[fbuf    ] = as;
			buf[fbuf + 1] = sc;
			buf[fbuf + 2] = ss;
			if((fbuf += 3) == 64 * 3) {
				fbuf = 0;
			}
		}while(_bios_keybrd(_KEYBRD_READY));
	}

	if(ctrl_c==1) {
		s.ds   = s.es = s.fs = s.gs = FP_SEG(&s);
		r.h.ah = 2;
		int386x(0x16,
		       &r,
		       &r,
		       &s);
		ss     = r.h.al;

		buf[fbuf    ] = 0;
		buf[fbuf + 1] = 46;
		buf[fbuf + 2] = ss;
		if((fbuf += 3) == 64 * 3) {
			fbuf = 0;
		}
		ctrl_c = 0;
	}
#endif
}

void checkmod(OSDEPMod mod) {
//	shift_status = 0;
	if(mod == KMOD_NONE){
// printf( "None\n" );
		return;
	}
/*
	if(mod & KMOD_NUM) {
printf( "NUMLOCK " );
	}

	if(mod & KMOD_CAPS) {
printf( "CAPSLOCK " );
	}
*/

	if(mod & KMOD_LCTRL ) { shift_status |= 4;  }
	if(mod & KMOD_RCTRL ) { shift_status |= 4;  }
	if(mod & KMOD_RSHIFT) { shift_status |= 1;  }
	if(mod & KMOD_LSHIFT) { shift_status |= 2;  }
	if(mod & KMOD_RALT  ) { shift_status |= 8;  }
	if(mod & KMOD_LALT  ) { shift_status |= 8;  }
	if(mod & KMOD_CTRL  ) { shift_status |= 4;  }
	if(mod & KMOD_ALT   ) { shift_status |= 8;  }
	if(mod & KMOD_CAPS  ) { shift_status |= 64; }
	if(mod & KMOD_NUM   ) { shift_status |= 32; }
/*
	if(mod & KMOD_NUM   ) { printf("NUMLOCK ");  }
	if(mod & KMOD_CAPS  ) { printf("CAPSLOCK "); }
	if(mod & KMOD_LCTRL ) { printf("LCTRL ");    }
	if(mod & KMOD_RCTRL ) { printf("RCTRL ");    }
	if(mod & KMOD_RSHIFT) { printf("RSHIFT ");   }
	if(mod & KMOD_LSHIFT) { printf("LSHIFT ");   }
	if(mod & KMOD_RALT  ) { printf("RALT ");     }
	if(mod & KMOD_LALT  ) { printf("LALT ");     }
	if(mod & KMOD_CTRL  ) { printf("CTRL ");     }
//	if(mod & KMOD_SHIFT ) { printf( "SHIFT ");   }
	if(mod & KMOD_ALT   ) { printf("ALT ");      }
	printf("\n");
*/
}

#ifdef SDL2
#include <SDL2/SDL_events.h>

void PrintEvent(const SDL_Event *event) {
	if(event->type == SDL_WINDOWEVENT) {
		switch(event->window.event) {
			case SDL_WINDOWEVENT_SHOWN: {
				SDL_Log("Window %d shown",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_HIDDEN: {
				SDL_Log("Window %d hidden",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_EXPOSED: {
				SDL_Log("Window %d exposed",
				        event->window.windowID);
				break;
			case SDL_WINDOWEVENT_MOVED: {
				SDL_Log("Window %d moved to %d,%d",
				        event->window.windowID,
				        event->window.data1,
				        event->window.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED: {
				SDL_Log("Window %d resized to %dx%d",
				        event->window.windowID,
				        event->window.data1,
				        event->window.data2);
				//vga_an  = event->window.data1;
				//vga_al  = event->window.data2;
				vwidth  = event->window.data1;
				vheight = event->window.data2;
				//DIVEndSound();
				//soundstopped = 1;
				//vwidth  = event->window.data1;
				//vheight = event->window.data2;
				break;
			}
			case SDL_WINDOWEVENT_SIZE_CHANGED: {
				SDL_Log("Window %d size changed to %dx%d",
				        event->window.windowID,
				        event->window.data1,
				        event->window.data2);
				break;
			}
			case SDL_WINDOWEVENT_MINIMIZED: {
				SDL_Log("Window %d minimized",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_MAXIMIZED: {
				SDL_Log("Window %d maximized",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_RESTORED: {
				SDL_Log("Window %d restored",
				        event->window.windowID);
				break;
			case SDL_WINDOWEVENT_ENTER: {
				SDL_Log("Mouse entered window %d",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_LEAVE: {
				SDL_Log("Mouse left window %d",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_FOCUS_GAINED: {
				SDL_Log("Window %d gained keyboard focus",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_FOCUS_LOST: {
				SDL_Log("Window %d lost keyboard focus",
				        event->window.windowID);
				break;
			}
			case SDL_WINDOWEVENT_CLOSE: {
				SDL_Log("Window %d closed",
				        event->window.windowID);
				break;
			}
			default: {
				SDL_Log("Window %d got unknown event %d",
				        event->window.windowID,
				        event->window.event);
				break;
			}
		}
	}
}
#endif


int8_t hx = 0, hy = 0; // hat xy positions
byte oldhatval;

void tecla(void) {
//printf("tecla\n");

//	ascii     = 0;
//	scan_code = 0;

	SDL_Event event;

	if(vwidth == 0 && vheight == 0) {
		vwidth  = vga_an;
		vheight = vga_al;
	}

#ifdef GCW
	if(divjoy) {
		byte hatval;

		// reset hat positions (D-PAD)
		// get new positions (D-PAD)
		hatval = SDL_JoystickGetHat(divjoy, 0);
		if(hatval != oldhatval) {
			key(OSDEP_key[GCW_LEFT ]) = 0;
			key(OSDEP_key[GCW_RIGHT]) = 0;
			key(OSDEP_key[GCW_DOWN ]) = 0;
			key(OSDEP_key[GCW_UP   ]) = 0;

			if(hatval & SDL_HAT_RIGHT) { key(OSDEP_key[GCW_RIGHT]) = 1; }
			if(hatval & SDL_HAT_LEFT ) { key(OSDEP_key[GCW_LEFT ]) = 1; }
			if(hatval & SDL_HAT_UP   ) { key(OSDEP_key[GCW_UP   ]) = 1; }
			if(hatval & SDL_HAT_DOWN ) { key(OSDEP_key[GCW_DOWN ]) = 1; }
		}

		oldhatval = hatval;
	}
#endif

	while(SDL_PollEvent(&event)) {
/*
printf("event: %d\n",
       event.type);
*/

		// check keys
#ifdef SDL2
		PrintEvent(&event);
#endif

#ifdef SDL
		if(event.type == SDL_VIDEORESIZE) {
//printf("RESIZING\n");
			// Fix: bug causing black screen and erroneous size when program window is maximized and switch back to normal
			//if(event.resize.w != 320 && event.resize.h != 200) {
				vwidth  = event.resize.w;
				vheight = event.resize.h;
			//}
			//else {
			//	svmode();
			//}

/*
			DIVEndSound();
			soundstopped = 1;

			volcado_parcial(0,
			                0,
			                vga_an - 1,
			                vga_al - 1);

				SDL_PauseAudio(0);
*/
		}

#endif

#ifdef GCW
		if(event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP) {
			if(event.type == SDL_JOYBUTTONDOWN) {
				event.type = SDL_KEYDOWN;
			}
			if(event.type == SDL_JOYBUTTONUP) {
				event.type = SDL_KEYUP;
			}

/*
	ELEMENT_B,
	ELEMENT_A,
	ELEMENT_Y,
	ELEMENT_X,
	ELEMENT_SELECT,
	ELEMENT_START,
	ELEMENT_L,
	ELEMENT_R,
*/

			switch(event.jbutton.button) {
				// these buttons probably arent correcct.
				// B
				case 0: {
					event.key.keysym.sym = SDLK_LALT;
					break;
				}
				// A
				case 3: {
					event.key.keysym.sym = SDLK_LCTRL;
					break;
				}
				// Y
				case 2: {
					event.key.keysym.sym = SDLK_SPACE;
					break;
				}
				// X
				case 1: {
					event.key.keysym.sym = SDLK_LSHIFT;
					break;
				}
				// SELECT
				case 4: {
					event.key.keysym.sym = SDLK_ESCAPE;
					break;
				}
				// START
				case 5: {
					event.key.keysym.sym = SDLK_RETURN;
					break;
				}
				// LEFT
				case 6: {
					event.key.keysym.sym = SDLK_TAB;
					break;
				}
				// RIGHT
				case 7: {
					event.key.keysym.sym = SDLK_BACKSPACE;
					break;
				}
			}
		}

		// Analog joystick mapped to mouse movement
		if(event.type == SDL_JOYAXISMOTION) {
			switch(event.jaxis.axis) {
				// axis 0 (left-right)
				case 0: {
					joymx = 0;
					if(event.jaxis.value < -JOY_DEADZONE) {
						// left movement
						joymx = -2;
					}
					else if(event.jaxis.value > JOY_DEADZONE) {
						// right movement
						joymx = 2;
					}
					break;
				}
				// axis 1 (up-down)
				case 1: {
					joymy = 0;
					if(event.jaxis.value < -JOY_DEADZONE) {
						// up movement
						joymy = -2;
					}
					else if(event.jaxis.value > JOY_DEADZONE) {
						// down movement
						joymy = 2;
					}
					break;
				}
				default: {
					break;
				}
			}
		}
#endif

		/* If a quit event has been sent */
		if(event.type == SDL_QUIT) {
			alt_x = 1;
		}

		if(event.type == SDL_KEYDOWN) {
/*
printf("KEYDOWN %d\n",
       event.key.keysym.sym);
*/
			switch(event.key.keysym.sym) {
#ifdef GCW
				// D-PAD LEFT
				case SDLK_LEFT: {
					event.key.keysym.sym = GCW_LEFT;
					//key(OSDEP_key[GCW_LEFT]) = 1;
					break;
				}
				// D-PAD RIGHT
				case SDLK_RIGHT {
					event.key.keysym.sym = GCW_RIGHT;
					//key(OSDEP_key[GCW_RIGHT]) = 1;
					break;
				}
				// D-PAD UP
				case SDLK_UP: {
					event.key.keysym.sym = GCW_UP;
					//key(OSDEP_key[GCW_UP]) = 1;
					break;
				}
				// D-PAD DOWN
				case SDLK_DOWN: {
					event.key.keysym.sym = GCW_DOWN;
					//key(OSDEP_key[GCW_DOWN]) = 1;
					break;
				}
				// A
				case SDLK_LCTRL: {
					event.key.keysym.sym=GCW_A;
					//key(OSDEP_key[GCW_A]) = 1;
					mouse->left = 1;
					break;
				}
			// B
			case SDLK_LALT: {
				event.key.keysym.sym = GCW_B;
				//key(OSDEP_key[GCW_B]) = 1;
				mouse->right = 1;
				break;
			}
			// X
			case SDLK_LSHIFT: {
				event.key.keysym.sym = GCW_X;
				//key(OSDEP_key[GCW_X]) = 1;
				break;
			}
			// Y
			case SDLK_SPACE: {
				event.key.keysym.sym = GCW_Y;
				//key(OSDEP_key[GCW_Y]) = 1;
				break;
			}
			// Left shoulder
			case SDLK_TAB:
				event.key.keysym.sym = GCW_L;
				//key(OSDEP_key[GCW_L]) = 1;
				break;
			}
			// Right shoulder
			case SDLK_BACKSPACE: {
				event.key.keysym.sym = GCW_R;
				//key(OSDEP_key[GCW_R]) = 1;
				break;
			}
			// Start
			case SDLK_RETURN: {
				event.key.keysym.sym = GCW_START;
				//key(OSDEP_key[GCW_START]) = 1;
				//scan_code = OSDEP_key[GCW_START];
				break;
			}
			// Select
			case SDLK_ESCAPE:
				event.key.keysym.sym = GCW_SELECT;
				//key(OSDEP_key[GCW_SELECT]) = 1;
				if(key(OSDEP_key[GCW_X]) == 1) {
					alt_x = 1;
				}
				break;
			}
			// Lock
			case SDLK_PAUSE: {
				event.key.keysym.sym = GCW_LOCK;
				//key(OSDEP_key[GCW_LOCK]) = 1;
				break;
			}
			default: {
				break;
			}
#else
			// handle special keys
			case SDLK_LSHIFT: {
				shift_status |= 2;
				break;
			}
			case SDLK_RSHIFT: {
				shift_status |= 1;
				break;
			}
			case SDLK_LCTRL: {
				shift_status |= 4;
				break;
			}
			case SDLK_RCTRL: {
				shift_status |= 4;
				break;
			}
			case SDLK_LALT: {
				shift_status |= 8;
				break;
			}
			case SDLK_RALT: {
				shift_status |= 8;
				break;
			}
			case SDLK_INSERT: {
				shift_status |= 128;
				break;
			}
			default: {
				break;
			}
#endif
		}

		//if(scan_code == 0) {
			scan_code = OSDEP_key[event.key.keysym.sym < 2048 ? event.key.keysym.sym : event.key.keysym.sym - 0x3FFFFD1A];
		//}

		ascii = event.key.keysym.scancode;

		checkmod((OSDEPMod)event.key.keysym.mod);

		// unicode not working on android
#ifndef DROID
#ifndef SDL2
		if(event.key.keysym.unicode < 0x80) {
			ascii = event.key.keysym.unicode;
/*
printf("ascii val: %d\n",
       ascii);
*/
		}
#endif
#endif
//#ifndef GCW
		kbdFLAGS[scan_code] = 1;
/*
printf("kbdflags: %d %d\n",
       kbdFLAGS[scan_code],
       key(scan_code));
*/
//#endif
	}

		if(event.type == SDL_KEYUP) {
/*
printf("KEYUP %d\n",
       event.key.keysym.sym);
*/
#ifdef GCW
			switch(event.key.keysym.sym) {
				// D-PAD LEFT
				case SDLK_LEFT: {
					event.key.keysym.sym = GCW_LEFT;
					//key(OSDEP_key[GCW_LEFT]) = 0;
					break;
				}
				// D-PAD RIGHT
				case SDLK_RIGHT: {
					event.key.keysym.sym = GCW_RIGHT;
					//key(OSDEP_key[GCW_RIGHT]) = 0;
					break;
				}
				// D-PAD UP
				case SDLK_UP: {
					event.key.keysym.sym = GCW_UP;
					//key(OSDEP_key[GCW_UP]) = 0;
					break;
				}
				// D-PAD DOWN
				case SDLK_DOWN: {
					event.key.keysym.sym = GCW_DOWN;
					//key(OSDEP_key[GCW_DOWN]) = 0;
					break;
				}
				// A
				case SDLK_LCTRL: {
					event.key.keysym.sym = GCW_A;
					//key(OSDEP_key[GCW_A]) = 0;
					mouse->left = 0;
					break;
				}
				// B
				case SDLK_LALT: {
					event.key.keysym.sym = GCW_B;
					//key(OSDEP_key[GCW_B]) = 0;
					mouse->right = 0;
					break;
				}
				// X
				case SDLK_LSHIFT: {
					event.key.keysym.sym = GCW_X;
					//key(OSDEP_key[GCW_X]) = 0;
					break;
				}
				// Y
				case SDLK_SPACE: {
					event.key.keysym.sym = GCW_Y;
					//key(OSDEP_key[GCW_Y]) = 0;
					break;
				}
				// Left shoulder
				case SDLK_TAB: {
					event.key.keysym.sym = GCW_L;
					//key(OSDEP_key[GCW_L]) = 0;
					break;
				}
				// Right shoulder
				case SDLK_BACKSPACE: {
					event.key.keysym.sym = GCW_R;
					//key(OSDEP_key[GCW_R]) = 0;
					break;
				}
				// Start
				case SDLK_RETURN: {
					event.key.keysym.sym = GCW_START;
					//key(OSDEP_key[GCW_START]) = 0;
					break;
				}
				// Select
				case SDLK_ESCAPE: {
					event.key.keysym.sym = GCW_SELECT;
					//key(OSDEP_key[GCW_SELECT]) = 0;
					break;
				}
				// Lock
				case SDLK_PAUSE: {
					event.key.keysym.sym = GCW_LOCK;
					//key(OSDEP_key[GCW_LOCK]) = 0;
					break;
				}
				default: {
					break;
				}
			}
//#else
#endif

			shift_status = 0;

			checkmod((OSDEPMod)event.key.keysym.mod);

			//acan_code = OSDEP_key[event.key.keysym.sym];
			//scan_code = event.key.keysym.scancode;
			kbdFLAGS[OSDEP_key[event.key.keysym.sym < 2048 ? event.key.keysym.sym:event.key.keysym.sym - 0x3FFFFD1A]] = 0;
		}

		if(event.type == SDL_MOUSEMOTION) {
			mouse->x = event.motion.x;
			mouse->y = event.motion.y;

			if(vga_an != vwidth || vga_al != vheight) {
				mouse->x = (int)(event.motion.x * (float)((float)vga_an / (float)vwidth));  // / (float)vga_an);
				mouse->y = (int)(event.motion.y * (float)((float)vga_al / (float)vheight)); // / (float)vga_al);

#ifdef SDL2
				SDL_Log("Mouse: VX: %d VY: %d x: %d y: %d\n",
				        mouse->x,
				        mouse->y,
				        event.motion.x,
				        event.motion.y);
#endif
			}

			//m_x += event.motion.xrel;
			//m_y += event.motion.yrel;
		}

		/* If a button on the mouse is pressed. */
		if(event.type == SDL_MOUSEBUTTONDOWN) {
			if(event.button.button == SDL_BUTTON_LEFT) {
				mouse->	left = 1;
#ifdef DEBUG
				mouse_b |= 1;
#endif
			}
			if(event.button.button == SDL_BUTTON_RIGHT) {
				mouse->right = 1;
#ifdef DEBUG
				mouse_b |= 2;
#endif
			}
			if(event.button.button == SDL_BUTTON_MIDDLE) {
				mouse->middle = 1;
#ifdef DEBUG
				mouse_b |= 4;
#endif
			}
		}

		/* If a button on the mouse is released. */
		if(event.type == SDL_MOUSEBUTTONUP) {
			if(event.button.button == SDL_BUTTON_LEFT) {
				mouse->left = 0;
#ifdef DEBUG
				mouse_b ^= 1;
#endif
			}
			if(event.button.button == SDL_BUTTON_RIGHT) {
				mouse->right = 0;
#ifdef DEBUG
				mouse_b ^= 2;
#endif
			}
			if(event.button.button == SDL_BUTTON_MIDDLE) {
				mouse->middle = 0;
#ifdef DEBUG
				mouse_b ^= 4;
#endif
			}
		}

		if((shift_status & 8) && scan_code == _x) {
			alt_x = 1;
		}
	} // end while


#ifdef DOS
	union REGS r;
	struct SREGS s;

	tecla_bios();

	if(ibuf != fbuf) {
		ascii        = buf[ibuf];
		scan_code    = buf[ibuf + 1];
		shift_status = buf[ibuf + 2];
		if((shift_status & 8) && scan_code == _x) {
			alt_x = 1;
		}
		else {
			alt_x = 0;
		}

		if((ibuf += 3) == buf_max * 3) {
			ibuf = 0;
		}
	}
	else {
		ascii     = 0;
		scan_code = 0;

		s.ds   = s.es = s.fs = s.gs = FP_SEG(&s);
		r.h.ah = 2;
		int386x(0x16,
		       &r,
		       &r,
		       &s);

		shift_status = r.h.al;
	}
#endif

#ifdef GCW
	mouse->x+=joymx;
	mouse->y+=joymy;
#endif

/*
printf("ascii: %d scan_code: %d\n",
       ascii,
       scan_code);
*/
}

//════════════════════════════════════════════════════════════════════════════
// Vacia el buffer de teclado (real e interno)
//════════════════════════════════════════════════════════════════════════════
void vacia_buffer(void) {
#ifdef DOS
	union REGS r;
	struct SREGS s;

	s.ds = s.es = s.fs = s.gs = FP_SEG(&s);

	while(_bios_keybrd(_KEYBRD_READY)) {
		r.h.ah = 0;
		int386x(0x16,
		       &r,
		       &r,
		       &s);
	}

	ascii        = 0;
	scan_code    = 0;

	r.h.ah = 2;
	int386x(0x16,
	       &r,
	       &r,
	       &s);

	shift_status = r.h.al;
#endif

	ibuf = fbuf = 0; // Vacia el buffer interno
}
