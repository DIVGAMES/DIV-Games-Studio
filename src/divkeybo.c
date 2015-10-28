
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Pruebas con la interrupciขn de teclado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// IRQ Data
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Interrupt handler related functions.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ


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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Obtains the address of an IRQ handler.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
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
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sets the address of an IRQ handler.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Function to initialize the handler.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void kbdInit(void)
{
//	printf("TODO - kbdInit\n");
	 SDL_EnableUNICODE( SDL_ENABLE );  
#ifdef NOTYET
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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Function to uninitialize the handler.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void kbdReset(void)
{
printf("TODO - kbdReset\n");
#ifdef NOTYET
    if (GetIRQVector(9) == IrqHandler) {   // If it was installed.
      SetIRQVector(9,OldIrqHandler);       // Uninstall it.
      SetIRQVector(0x1b,OldIrq1b);         // Uninstall it.
      SetIRQVector(0x23,OldIrq23);         // Uninstall it.
    }
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Espera una tecla, actualiza variables (ascii,scan_code y shift_status)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void tecla_bios(void) {
printf("tecla_bios\n");
#ifdef NOTYET
  int as,sc,ss;
  union REGS r;
  struct SREGS s;

  if (_bios_keybrd(_KEYBRD_READY)) {
    s.ds=s.es=s.fs=s.gs=FP_SEG(&s);
    do {
      r.h.ah=0; int386x(0x16,&r,&r,&s);
      as=r.h.al; sc=r.h.ah;
      r.h.ah=2; int386x(0x16,&r,&r,&s); ss=r.h.al;
      if (as>='0' && as<='9') {
        kbdFLAGS[0x4B]=0;
        kbdFLAGS[0x4D]=0;
        kbdFLAGS[0x48]=0;
        kbdFLAGS[0x50]=0;
        sc=0;
      }
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
//extern float m_x=0.0,m_y;
void tecla(void) {
#ifdef NOTYET
    ascii=0; scan_code=0;
//printf("Reading keyboard\n");

SDL_Event event;
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
				scan_code = event.key.keysym.scancode;
			}
			  if (event.type == SDL_MOUSEMOTION)
            {
				m_x+=event.motion.xrel;
				m_y+=event.motion.yrel;
			}
            /* If a button on the mouse is pressed. */
            if (event.type == SDL_MOUSEBUTTONDOWN)
            {
				printf("click\n");
				m_b = 1;
			}
			
			if (event.type == SDL_MOUSEBUTTONUP)
            {
				m_b = 0;
			}
			
        }


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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//     Empty the keyboard buffer (real and internal)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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

