
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//      Pruebas con la interrupci¢n de teclado
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

#include "inter.h"
#include "osdep.h"


#ifdef GCW


// GCW DEFAULT BUTTON MAPPING


/*
#define GCW_L SDLK_TAB
#define GCW_R SDLK_BACKSPACE
#define GCW_UP SDLK_UP
#define GCW_DOWN SDLK_DOWN
#define GCW_LEFT SDLK_LEFT
#define GCW_RIGHT SDLK_RIGHT
#define GCW_A SDLK_LCTRL
#define GCW_B SDLK_LALT
#define GCW_X SDLK_LSHIFT
#define GCW_Y SDLK_SPACE
#define GCW_START SDLK_RETURN
#define GCW_SELECT SDLK_ESCAPE
#define GCW_LOCK SDLK_PAUSE
*/

#define GCW_L SDLK_TAB
#define GCW_R SDLK_BACKSPACE
#define GCW_UP SDLK_UP
#define GCW_DOWN SDLK_DOWN
#define GCW_LEFT SDLK_LEFT
#define GCW_RIGHT SDLK_RIGHT
#define GCW_A SDLK_LCTRL
#define GCW_B SDLK_RSHIFT
#define GCW_X SDLK_LSHIFT
#define GCW_Y SDLK_SPACE
#define GCW_START SDLK_RETURN
#define GCW_SELECT SDLK_ESCAPE
#define GCW_LOCK SDLK_PAUSE

#endif

#define JOY_DEADZONE 500
int joymx = 0, joymy=0;

#ifdef DEBUG
extern int mouse_b;//, mouse_y;
#endif


//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// IRQ Data
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

typedef void (__interrupt __far *TIRQHandler)(void);

TIRQHandler OldIrqHandler;
TIRQHandler OldIrq23;
TIRQHandler OldIrq1b;

byte * shift = (byte *) 0x417; // Shift status

byte buf[64*3]; // {ascii,scan_code,shift_status}
int ibuf=0; // Puntero al buffer, inicio de la cola
int fbuf=0; // Puntero al buffer, fin de la cola

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Interrupt handler related functions.
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

int ctrl_c=0,alt_x=0;
//word * kb_start = (void*) 0x41a;
//word * kb_end = (void*) 0x41c;

void __far __interrupt __loadds IrqHandler(void)
{
#ifdef DOS
  int n;
  int scancode;

  scancode=inp(0x60);

  if (((*shift)&4) && scancode==46) {
    kbdFLAGS[46]=1; ctrl_c=1;
    outp(0x20,0x20);
  } else {

    if (scancode<128) {
      kbdFLAGS[scancode]=1;
    } else if (scancode>129) {
      kbdFLAGS[scancode-128]=0;
    } else {
      n=128; do { kbdFLAGS[--n]=0; } while (n);
    }

//    if (kbhit()) getch();

    OldIrqHandler();

    if ((*kb_end-28)%32+30==*kb_start) *kb_start=(*kb_start-28)%32+30;

  }
#endif
}

void __far __interrupt __loadds Irq23(void){}
void __far __interrupt __loadds Irq1b(void){}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Obtains the address of an IRQ handler.
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

TIRQHandler GetIRQVector(int n)
{
#ifdef DOS
    struct SREGS sregs;
    union REGS inregs, outregs;

    inregs.x.eax = 0x3500 + n;   // DOS4GW redefines the DOS get vector call.
    sregs.ds = sregs.es = 0;
    int386x (0x21, &inregs, &outregs, &sregs);
    return (TIRQHandler)(MK_FP((word)sregs.es, outregs.x.ebx));
#endif
}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Sets the address of an IRQ handler.
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void SetIRQVector(int n, TIRQHandler vec)
{
#ifdef DOS
    struct SREGS sregs;
    union REGS inregs, outregs;

    inregs.x.eax = 0x2500 + n;   // DOS set vector call.
    inregs.x.edx = FP_OFF (vec);
    sregs.ds     = FP_SEG (vec);     // Handler pointer.
    sregs.es     = 0;
    int386x (0x21, &inregs, &outregs, &sregs);
#endif
}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Function to initialize the handler.
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void kbdInit(void)
{
	OSDEP_keyInit();
	
#ifdef DOS
    if (GetIRQVector(9) != IrqHandler) {   // If not already installed.
      OldIrqHandler=GetIRQVector(9);       // Get old handler.
      SetIRQVector(9,IrqHandler);          // Set our handler.
      OldIrq1b=GetIRQVector(0x1b);         // Get old handler.
      SetIRQVector(0x1b,Irq1b);            // Set our handler.
      OldIrq23=GetIRQVector(0x23);         // Get old handler.
      SetIRQVector(0x23,Irq23);            // Set our handler.
    }

    signal(SIGBREAK,SIG_IGN);
    signal(SIGINT,SIG_IGN);
#endif
}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
// Function to uninitialize the handler.
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void kbdReset(void)
{
#ifdef DOS
    if (GetIRQVector(9) == IrqHandler) {   // If it was installed.
      SetIRQVector(9,OldIrqHandler);       // Uninstall it.
      SetIRQVector(0x1b,OldIrq1b);         // Uninstall it.
      SetIRQVector(0x23,OldIrq23);         // Uninstall it.
    }
#endif
}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//      Espera una tecla, actualiza variables (ascii,scan_code y shift_status)
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void tecla_bios(void) {
#ifdef DOS
  int as,sc,ss;
  union REGS r;
  struct SREGS s;

  if (_bios_keybrd(_KEYBRD_READY)) {
    s.ds=s.es=s.fs=s.gs=FP_SEG(&s);
    do {
      r.h.ah=0; int386x(0x16,&r,&r,&s);
      as=r.h.al; sc=r.h.ah;
      r.h.ah=2; int386x(0x16,&r,&r,&s); ss=r.h.al;
      buf[fbuf]=as; buf[fbuf+1]=sc; buf[fbuf+2]=ss;
      if ((fbuf+=3)==64*3) fbuf=0;

    } while (_bios_keybrd(_KEYBRD_READY));
  }

  if (ctrl_c==1) {
    s.ds=s.es=s.fs=s.gs=FP_SEG(&s);
    r.h.ah=2; int386x(0x16,&r,&r,&s); ss=r.h.al;
    buf[fbuf]=0; buf[fbuf+1]=46; buf[fbuf+2]=ss;
    if ((fbuf+=3)==64*3) fbuf=0; ctrl_c=0;
  }
#endif
}

#ifdef SDL
void checkmod(OSDEPMod mod) {
	
//	shift_status = 0;
	if( mod == KMOD_NONE ){
//            printf( "None\n" );
            return;
        }
	// if( mod & KMOD_NUM ) printf( "NUMLOCK " );
    //    if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) shift_status |=4; 
       if( mod & KMOD_RCTRL ) shift_status |=4;
        if( mod & KMOD_RSHIFT ) shift_status |=1;
        if( mod & KMOD_LSHIFT ) shift_status |=2;
        if( mod & KMOD_RALT ) shift_status |=8;
        if( mod & KMOD_LALT ) shift_status |=8;
        if( mod & KMOD_CTRL ) shift_status |=4;
        if( mod & KMOD_ALT ) shift_status |=8;
        
        if (mod & KMOD_CAPS) shift_status |=64;
        if (mod & KMOD_NUM) shift_status |=32;
        /*
         if( mod & KMOD_NUM ) printf( "NUMLOCK " );
        if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) printf( "LCTRL " );
        if( mod & KMOD_RCTRL ) printf( "RCTRL " );
        if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
        if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
        if( mod & KMOD_RALT ) printf( "RALT " );
        if( mod & KMOD_LALT ) printf( "LALT " );
        if( mod & KMOD_CTRL ) printf( "CTRL " );
//        if( mod & KMOD_SHIFT ) printf( "SHIFT " );
        if( mod & KMOD_ALT ) printf( "ALT " );
        
        printf("\n");
        * */
}

#endif

#ifdef SDL2
#include <SDL2/SDL_events.h>

void PrintEvent(const SDL_Event * event)
{
    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
        case SDL_WINDOWEVENT_SHOWN:
            SDL_Log("Window %d shown", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            SDL_Log("Window %d hidden", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_Log("Window %d exposed", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MOVED:
            SDL_Log("Window %d moved to %d,%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_Log("Window %d resized to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
//            vga_an = event->window.data1;
//			vga_al = event->window.data2;
			vwidth = event->window.data1;
			vheight = event->window.data2;
			// EndSound();
			// soundstopped = 1;
		    //vwidth = event->window.data1;
            //vheight = event->window.data2;
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            SDL_Log("Window %d size changed to %dx%d",
                    event->window.windowID, event->window.data1,
                    event->window.data2);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            SDL_Log("Window %d minimized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            SDL_Log("Window %d maximized", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            SDL_Log("Window %d restored", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_ENTER:
            SDL_Log("Mouse entered window %d",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            SDL_Log("Mouse left window %d", event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_Log("Window %d gained keyboard focus",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_Log("Window %d lost keyboard focus",
                    event->window.windowID);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_Log("Window %d closed", event->window.windowID);
            break;
        default:
            SDL_Log("Window %d got unknown event %d",
                    event->window.windowID, event->window.event);
            break;
        }
    }
}
#endif


int8_t hx=0,hy=0; // hat xy positions

byte oldhatval;
void tecla(void) {
//printf("tecla\n");
//ascii=0; scan_code=0;

#ifdef SDL
SDL_Event event;
#endif

if(vwidth == 0 && vheight == 0) {
	vwidth = vga_an;
	vheight = vga_al;
}
#ifdef GCW
if(divjoy) {
	byte hatval;

// reset hat positions (D-PAD)

// get new positions (D-PAD)
	hatval = SDL_JoystickGetHat(divjoy,0);
if(hatval!=oldhatval) {
		key(OSDEP_key[GCW_LEFT])=0;
		key(OSDEP_key[GCW_RIGHT])=0;
		key(OSDEP_key[GCW_DOWN])=0;
		key(OSDEP_key[GCW_UP])=0;

	if(hatval & SDL_HAT_RIGHT) 
		key(OSDEP_key[GCW_RIGHT])=1;

	if(hatval & SDL_HAT_LEFT)
		key(OSDEP_key[GCW_LEFT])=1;
	
	if(hatval & SDL_HAT_UP)
		key(OSDEP_key[GCW_UP])=1;

	if(hatval & SDL_HAT_DOWN)
		key(OSDEP_key[GCW_DOWN])=1;

}
oldhatval = hatval;

}
#endif
	
#ifdef SDL
	while(SDL_PollEvent(&event)) {	
//		printf("event: %d\n",event.type);
		// check keys
#ifdef SDL2
	PrintEvent(&event);
#endif

#ifdef SDL
		if (event.type == SDL_VIDEORESIZE) {
//				printf("RESIZING\n");
			if(event.resize.w!=320 && event.resize.h!=200) {
				vwidth = event.resize.w;
				vheight = event.resize.h;
			} else {
				svmode();
			}
//			EndSound();
//			soundstopped=1;
//				volcado_parcial(0,0,vga_an-1,vga_al-1);
//				SDL_PauseAudio(0);
			
        }

#endif

#ifdef GCW
		if(event.type == SDL_JOYBUTTONDOWN || event.type == SDL_JOYBUTTONUP)  {
			
			if(event.type == SDL_JOYBUTTONDOWN)
				event.type = SDL_KEYDOWN;
				
			if(event.type == SDL_JOYBUTTONUP)
				event.type = SDL_KEYUP;
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

				case 0: // B
					event.key.keysym.sym=SDLK_LALT;
					break;
				case 3: // A
					event.key.keysym.sym=SDLK_LCTRL;
					break;
				case 2: // Y
					event.key.keysym.sym=SDLK_SPACE;
					break;
				case 1: // X
					event.key.keysym.sym=SDLK_LSHIFT;
					break;
				case 4: // SELECT
					event.key.keysym.sym=SDLK_ESCAPE;
					break;
				case 5: // START
					event.key.keysym.sym=SDLK_RETURN;
					break;	
				case 6: // LEFT
					event.key.keysym.sym=SDLK_TAB;
					break;	
				case 7: // RIGHT
					event.key.keysym.sym=SDLK_BACKSPACE;
					break;	

			}
		}
			
		// Analog joystick mapped to mouse movement
		if(event.type == SDL_JOYAXISMOTION) {			
				
			switch(event.jaxis.axis)
			{
				case 0:		// axis 0 (left-right)
					joymx=0;
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						joymx=-2;
						// left movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						joymx=2;
						// right movement
					}
				break;
				case 1:		// axis 1 (up-down)
					joymy=0;
					if(event.jaxis.value < -JOY_DEADZONE)
					{
						joymy=-2;
						// up movement
					}
					else if(event.jaxis.value > JOY_DEADZONE)
					{
						joymy=2;
						// down movement
					}
				break;

				default:
				break;
			}
		}
#endif

            /* If a quit event has been sent */
		if (event.type == SDL_QUIT)
			alt_x=1;
            
		if (event.type == SDL_KEYDOWN) {
//			printf("KEYDOWN %d\n",event.key.keysym.sym);
			switch(event.key.keysym.sym) {
#ifdef GCW
			case SDLK_LEFT:		// D-PAD LEFT
			event.key.keysym.sym=GCW_LEFT;
//				key(OSDEP_key[GCW_LEFT])=1;
			break;
			
			case SDLK_RIGHT:	// D-PAD RIGHT
				event.key.keysym.sym=GCW_RIGHT;
//				key(OSDEP_key[GCW_RIGHT])=1;

			break;
			case SDLK_UP:		// D-PAD UP
			event.key.keysym.sym=GCW_UP;
//				key(OSDEP_key[GCW_UP])=1;
			break;
			
			case SDLK_DOWN:		// D-PAD DOWN
			event.key.keysym.sym=GCW_DOWN;
//					key(OSDEP_key[GCW_DOWN])=1;
			break;
			
			case SDLK_LCTRL:	// A
			event.key.keysym.sym=GCW_A;
//				key(OSDEP_key[GCW_A])=1;
				mouse->left=1;
			break;
			
			case SDLK_LALT:		// B
			event.key.keysym.sym=GCW_B;
//				key(OSDEP_key[GCW_B])=1;
				mouse->right=1;
			break;
			
			case SDLK_LSHIFT:	// X
			event.key.keysym.sym=GCW_X;
//				key(OSDEP_key[GCW_X])=1;
			break;
			
			case SDLK_SPACE:	// Y
			event.key.keysym.sym=GCW_Y;
//				key(OSDEP_key[GCW_Y])=1;
			break;

			case SDLK_TAB:		// Left shoulder
			event.key.keysym.sym=GCW_L;
//				key(OSDEP_key[GCW_L])=1;
			break;

			case SDLK_BACKSPACE:	// Right shoulder
			event.key.keysym.sym=GCW_R;
//				key(OSDEP_key[GCW_R])=1;
			break;
			
			case SDLK_RETURN:	// Start
//				scan_code = OSDEP_key[GCW_START];
				event.key.keysym.sym=GCW_START;
//				key(OSDEP_key[GCW_START])=1;
			break;
			
			case SDLK_ESCAPE:	// Select
				event.key.keysym.sym=GCW_SELECT;
//				key(OSDEP_key[GCW_SELECT])=1;			
				if(key(OSDEP_key[GCW_X])==1)
					alt_x=1;
			break;
			
			case SDLK_PAUSE:	// Lock
			event.key.keysym.sym=GCW_LOCK;
//				key(OSDEP_key[GCW_LOCK])=1;
			break;

			default:
			break;
#else
// handle special keys
			case SDLK_LSHIFT:
				shift_status|=2;
				break;
			case SDLK_RSHIFT:
				shift_status|=1;
				break;
			case SDLK_LCTRL:
				shift_status|=4;
				break;
			case SDLK_RCTRL:
				shift_status|=4;
				break;
			case SDLK_LALT:
				shift_status|=8;
				break;
			case SDLK_RALT:
				shift_status|=8;
				break;
			case SDLK_INSERT:
				shift_status|=128;
#endif
		}

//		if(scan_code==0)
			scan_code = OSDEP_key[event.key.keysym.sym<2048?event.key.keysym.sym:event.key.keysym.sym-0x3FFFFD1A];
		
		ascii = event.key.keysym.scancode;
		checkmod((OSDEPMod) event.key.keysym.mod);

// unicode not working on android
#ifndef DROID
#ifndef SDL2
		if(event.key.keysym.unicode<0x80) {
			ascii = event.key.keysym.unicode;
			//printf("ascii val: %d\n",ascii);
		}					
#endif
#endif
//#ifndef GCW				
		kbdFLAGS[scan_code]=1;	
		//printf("kbdflags: %d %d\n",kbdFLAGS[scan_code],key(scan_code));		
//#endif
	}
	
	
	if(event.type == SDL_KEYUP) {
//		printf("KEYUP %d\n",event.key.keysym.sym);
#ifdef GCW
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:		// D-PAD LEFT
				event.key.keysym.sym=GCW_LEFT;
//				key(OSDEP_key[GCW_LEFT])=0;
				break;
			
			case SDLK_RIGHT:	// D-PAD RIGHT
				event.key.keysym.sym=GCW_RIGHT;
//				key(OSDEP_key[GCW_RIGHT])=0;
				break;
			
			case SDLK_UP:		// D-PAD UP
				event.key.keysym.sym=GCW_UP;
//				key(OSDEP_key[GCW_UP])=0;
				break;
			
			case SDLK_DOWN:		// D-PAD DOWN
				event.key.keysym.sym=GCW_DOWN;
	//			key(OSDEP_key[GCW_DOWN])=0;
				break;
			
			case SDLK_LCTRL:	// A
				event.key.keysym.sym=GCW_A;
				//key(OSDEP_key[GCW_A])=0;
				mouse->left=0;
				break;
			
			case SDLK_LALT:		// B
				event.key.keysym.sym=GCW_B;
				//key(OSDEP_key[GCW_B])=0;
				mouse->right=0;
				break;
			
			case SDLK_LSHIFT:	// X
				event.key.keysym.sym=GCW_X;
				//key(OSDEP_key[GCW_X])=0;
				break;
			
			case SDLK_SPACE:	// Y
				event.key.keysym.sym=GCW_Y;
//				key(OSDEP_key[GCW_Y])=0;
				break;
	
			case SDLK_TAB:		// Left shoulder
				event.key.keysym.sym=GCW_L;
//				key(OSDEP_key[GCW_L])=0;
				break;

			case SDLK_BACKSPACE:	// Right shoulder
				event.key.keysym.sym=GCW_R;
				//key(OSDEP_key[GCW_R])=0;
				break;
			
			case SDLK_RETURN:	// Start
				event.key.keysym.sym=GCW_START;
//				key(OSDEP_key[GCW_START])=0;
				break;
			
			case SDLK_ESCAPE:	// Select
				event.key.keysym.sym=GCW_SELECT;
//				key(OSDEP_key[GCW_SELECT])=0;			
				break;
			
			case SDLK_PAUSE:	// Lock
				event.key.keysym.sym=GCW_LOCK;
//				key(OSDEP_key[GCW_LOCK])=0;
				break;
			
			default:
				break;
		}
//#else
#endif

		shift_status=0;
		checkmod((OSDEPMod) event.key.keysym.mod);
		
//		scan_code = OSDEP_key[event.key.keysym.sym];
		//scan_code = event.key.keysym.scancode;
		kbdFLAGS[OSDEP_key[event.key.keysym.sym<2048?event.key.keysym.sym:event.key.keysym.sym-0x3FFFFD1A]]=0;
	}
	
	if (event.type == SDL_MOUSEMOTION) {
		mouse->x = event.motion.x;
		mouse->y = event.motion.y;

		if(vga_an != vwidth || vga_al != vheight) {
			mouse->x = (int)(event.motion.x*(float)((float)vga_an / (float)vwidth));// / (float)vga_an);
			mouse->y = (int)(event.motion.y*(float)((float)vga_al / (float)vheight));// / (float)vga_al);
#ifdef SDL2
			SDL_Log("Mouse: VX: %d VY: %d x: %d y: %d\n",mouse->x, mouse->y, event.motion.x,event.motion.y);
#endif
		}

//				m_x+=event.motion.xrel;
//				m_y+=event.motion.yrel;
	}
		/* If a button on the mouse is pressed. */
	if (event.type == SDL_MOUSEBUTTONDOWN) {
		if(event.button.button == SDL_BUTTON_LEFT) {
			mouse->	left = 1;
#ifdef DEBUG
			mouse_b|=1;
#endif
		}
		if(event.button.button == SDL_BUTTON_RIGHT) {
			mouse->right = 1;
#ifdef DEBUG
			mouse_b|=2;
#endif
		}
		if(event.button.button == SDL_BUTTON_MIDDLE) {
			mouse->middle = 1;
#ifdef DEBUG
			mouse_b|=4;
#endif
		}
	}
		
	if (event.type == SDL_MOUSEBUTTONUP) {

		if(event.button.button == SDL_BUTTON_LEFT) {
			mouse->left = 0;
#ifdef DEBUG
			mouse_b ^=1;
#endif
		}
	
		if(event.button.button == SDL_BUTTON_RIGHT) {
			mouse->right = 0;
#ifdef DEBUG
			mouse_b ^=2;
#endif
		}
	
		if(event.button.button == SDL_BUTTON_MIDDLE) {
			mouse->middle = 0;
#ifdef DEBUG
			mouse_b ^=4;
#endif
		}	
	}

	if ((shift_status&8) && scan_code==_x) 
		alt_x=1; 

	} // end while

#endif // HAS SDL


#ifdef DOS
  union REGS r;
  struct SREGS s;

  tecla_bios();

  if (ibuf!=fbuf) {
    ascii=buf[ibuf]; scan_code=buf[ibuf+1]; shift_status=buf[ibuf+2];
    if ((shift_status&8) && scan_code==_x) alt_x=1; else alt_x=0;
    if ((ibuf+=3)==buf_max*3) ibuf=0;
  } else {
    ascii=0; scan_code=0;
    s.ds=s.es=s.fs=s.gs=FP_SEG(&s);
    r.h.ah=2; int386x(0x16,&r,&r,&s); shift_status=r.h.al;
  }
#endif

#ifdef GCW
	mouse->x+=joymx;
	mouse->y+=joymy;
#endif

//printf("ascii: %d scan_code: %d\n",ascii,scan_code);

}

//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ
//      Vacia el buffer de teclado (real e interno)
//ÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍ

void vacia_buffer(void) {
#ifdef DOS
  union REGS r;
  struct SREGS s;

  s.ds=s.es=s.fs=s.gs=FP_SEG(&s);

  while(_bios_keybrd(_KEYBRD_READY)) { r.h.ah=0; int386x(0x16,&r,&r,&s); }
  ascii=0; scan_code=0;
  r.h.ah=2; int386x(0x16,&r,&r,&s); shift_status=r.h.al;
#endif
  ibuf=fbuf=0; // Vacia el buffer interno
}
