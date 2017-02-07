
//═════════════════════════════════════════════════════════════════════════════
//      Pruebas con la interrupción de teclado
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"
#include <time.h>

//═════════════════════════════════════════════════════════════════════════════
// IRQ Data
//═════════════════════════════════════════════════════════════════════════════
/*
typedef void (__interrupt __far *TIRQHandler)(void);

TIRQHandler OldIrqHandler;
TIRQHandler OldIrq23;
TIRQHandler OldIrq1b;
*/
byte * shift = (byte *) 0x417; // Shift status

byte buf[64*3]; // {ascii,scan_code,shift_status}
int ibuf=0; // Puntero al buffer, inicio de la cola
int fbuf=0; // Puntero al buffer, fin de la cola

//═════════════════════════════════════════════════════════════════════════════
// Interrupt handler related functions.
//═════════════════════════════════════════════════════════════════════════════

int ctrl_c=0;

#ifdef NOTYET
word * kb_start = (void*) 0x41a;
word * kb_end = (void*) 0x41c;
#endif

void __far __interrupt __loadds IrqHandler(void)
{
#ifdef NOTYET
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

//═════════════════════════════════════════════════════════════════════════════
// Obtains the address of an IRQ handler.
//═════════════════════════════════════════════════════════════════════════════
#ifdef NOTYET
TIRQHandler GetIRQVector(int n)
{
    struct SREGS sregs;
    union REGS inregs, outregs;

    inregs.x.eax = 0x3500 + n;   // DOS4GW redefines the DOS get vector call.
    sregs.ds = sregs.es = 0;
    int386x (0x21, &inregs, &outregs, &sregs);
    return (TIRQHandler)(MK_FP((word)sregs.es, outregs.x.ebx));
}
#endif
//═════════════════════════════════════════════════════════════════════════════
// Sets the address of an IRQ handler.
//═════════════════════════════════════════════════════════════════════════════

#ifdef NOTYET
void SetIRQVector(int n, TIRQHandler vec)
{
    struct SREGS sregs;
    union REGS inregs, outregs;

    inregs.x.eax = 0x2500 + n;   // DOS set vector call.
    inregs.x.edx = FP_OFF (vec);
    sregs.ds     = FP_SEG (vec);     // Handler pointer.
    sregs.es     = 0;
    int386x (0x21, &inregs, &outregs, &sregs);
}
#endif
//═════════════════════════════════════════════════════════════════════════════
// Function to initialize the handler.
//═════════════════════════════════════════════════════════════════════════════

void kbdInit(void)
{
	// call osdep keyboard init
	OSDEP_keyInit();
}

//═════════════════════════════════════════════════════════════════════════════
// Function to uninitialize the handler.
//═════════════════════════════════════════════════════════════════════════════

void kbdReset(void)
{
	// nothing to do
}

//═════════════════════════════════════════════════════════════════════════════
//      Espera una tecla, actualiza variables (ascii,scan_code y shift_status)
//═════════════════════════════════════════════════════════════════════════════

void tecla_bios(void) {
	// nothing to do
}

extern int reloj;

//extern float m_x=0.0,m_y;
void tecla(void) {
mclock = SDL_GetTicks()/100;//(int)clock()/20000;
reloj = SDL_GetTicks()/100;//(int)clock();

//printf("%d %d\n",mclock,reloj);

//mclock++;
#ifdef NOTYET
//printf("Reading keyboard\n");
    ascii=0; scan_code=0;

SDL_Event event;
#ifdef GCW

#else
while(SDL_PollEvent(&event))
        {
			

			
            /* If a quit event has been sent */
            if (event.type == SDL_QUIT)
            {
                /* Quit the application */
                salir_del_entorno=1;
            }
            
            if (event.type == SDL_KEYDOWN)
            {
				printf("key pressed\n");
#ifndef DROID
				scan_code = osdep_key[event.key.keysym.unicode];
#endif
				kbgFlags[scan_code]=1;
			}
			if(event.type == SDL_KEYUP) 
			{
				scan_code = osdep_key[event.key.keysym.sym];
				//scan_code = event.key.keysym.scancode;
				kbdFLAGS[scan_code]=0;
				scan_code=0;
			}
			  if (event.type == SDL_MOUSEMOTION)
            {
				m_x+=event.motion.xrel;
				m_y+=event.motion.yrel;
			}
            /* If a button on the mouse is pressed. */
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
	//			m_b = 1;
			}
			
			if (event.type == SDL_MOUSEBUTTONUP)
            {
//				m_b = 0;
			}
			
        }

#endif

  union REGS r;
  struct SREGS s;

  tecla_bios();

  if (ibuf!=fbuf) {
    ascii=buf[ibuf]; scan_code=buf[ibuf+1]; shift_status=buf[ibuf+2];
    if ((ibuf+=3)==64*3) ibuf=0;
  } else {
    ascii=0; scan_code=0;
    s.ds=s.es=s.fs=s.gs=FP_SEG(&s);
    r.h.ah=2; int386x(0x16,&r,&r,&s); shift_status=r.h.al;
  }

#endif

}

//═════════════════════════════════════════════════════════════════════════════
//     Empty the keyboard buffer (real and internal)
//═════════════════════════════════════════════════════════════════════════════

void vacia_buffer(void) {
//printf("vacia_buffer\n");
#ifdef NOTYET
  union REGS r;
  struct SREGS s;

  s.ds=s.es=s.fs=s.gs=FP_SEG(&s);

  while(_bios_keybrd(_KEYBRD_READY)) { r.h.ah=0; int386x(0x16,&r,&r,&s); }
  ascii=0; scan_code=0;
  r.h.ah=2; int386x(0x16,&r,&r,&s); shift_status=r.h.al;
#endif
  ibuf=fbuf=0; // Empty the internal buffer
}

