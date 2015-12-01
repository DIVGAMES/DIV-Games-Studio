
#include "inter.h"

// file prototypes
void readmouse(void);



//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mouse
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

/*

callback_data cbd={0};

int lock_region (void *address, unsigned length) {
    union REGS regs;
    unsigned linear;

    linear = (unsigned) address;
    regs.w.ax = 0x600;			// DPMI Lock Linear Region
    regs.w.bx = (unsigned short) (linear >> 16); // Linear address in BX:CX
    regs.w.cx = (unsigned short) (linear & 0xFFFF);
    regs.w.si = (unsigned short) (length >> 16); // Length in SI:DI
    regs.w.di = (unsigned short) (length & 0xFFFF);
    int386 (0x31, &regs, &regs);
    return (! regs.w.cflag);		// Return 0 if can't lock
}

#pragma off (check_stack)

void _loadds far click_handler (int mbx,int mcx,int mdx) {
#pragma aux click_handler parm [EBX] [ECX] [EDX]
    cbd.mouse_action = 1;
    cbd.mouse_bx = (unsigned short) mbx;
    cbd.mouse_cx = (unsigned short) mcx;
    cbd.mouse_dx = (unsigned short) mdx;
} void cbc_end (void) { }

*/

void mouse_on (void) {
#ifdef DOS
  struct SREGS sregs;
  union REGS inregs, outregs;
  void (far *function_ptr)();

  segread(&sregs);
  inregs.w.ax = 0;
  int386 (0x33, &inregs, &outregs);

  if (outregs.w.ax == 0xffff) {
    if ((lock_region (&cbd, sizeof(cbd))) &&
      (lock_region ((void near *) click_handler,
         (char *) cbc_end - (char near *) click_handler))) {
      inregs.w.ax = 0xC;
      inregs.w.cx = 0x1f; //0x0002 + 0x0008;
      function_ptr = ( void(*)(void) ) click_handler;
      inregs.x.edx = FP_OFF (function_ptr);
      sregs.es = FP_SEG (function_ptr);
      int386x (0x33, &inregs, &outregs, &sregs);
    }
  }
#endif
}

void mouse_off (void) {
#ifdef DOS
  union REGS inregs, outregs;
  inregs.w.ax = 0;
  int386 (0x33, &inregs, &outregs);
#endif
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_mouse(x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_mouse(int x,int y) {
#ifdef DOS
  union REGS inregs, outregs;
  struct SREGS sregs;

  if (y<0) y=0; if (x<0) x=0;
  if (y>vga_al) y=vga_al; if (x>vga_an) x=vga_an;

  memset(&inregs,0,sizeof(inregs));
  memset(&outregs,0,sizeof(outregs));
  memset(&sregs,0,sizeof(sregs));

  inregs.w.ax = 4;
  inregs.w.cx = x*2; inregs.w.dx = y;
  int386x (0x33, &inregs, &outregs, &sregs);
#endif
  readmouse();

/*
  inregs.w.ax = 3;
  int386x (0x33, &inregs, &outregs, &sregs);

  _mouse_x=mouse->x==outregs.w.cx>>1;
  _mouse_y=mouse->y==outregs.w.dx;
  mouse->left=(outregs.w.bx&1);
  mouse->middle=(outregs.w.bx&4);
  mouse->right=(outregs.w.bx&2);

  cbd.mouse_action=0;
*/

}

void readmouse(void) {
//tecla();
#ifdef DOS
  union REGS regs;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=3;
  int386(0x033,&regs,&regs);
  mouse->left=(regs.w.bx&1);
  mouse->middle=(regs.w.bx&4)/4;
  mouse->right=(regs.w.bx&2)/2;
  _mouse_x=mouse->x=regs.w.cx/2;
  _mouse_y=mouse->y=regs.w.dx;
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Define la regiขn de movimiento del ratขn (pantalla completa)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void mouse_window(void) {
#ifdef DOS
  union REGS inregs, outregs;
  inregs.w.ax = 7;
  inregs.w.cx = 0; inregs.w.dx = (vga_an*2)-1;
  int386 (0x33, &inregs, &outregs);
  inregs.w.ax = 8;
  inregs.w.cx = 0; inregs.w.dx = vga_al-1;
  int386 (0x33, &inregs, &outregs);
#endif
}

