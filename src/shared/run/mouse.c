
#include "inter.h"

float m_x=0.0,m_y=0.0;

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set_mouse(x,y)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

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



void check_mouse(void) {

#ifdef DOS
  struct SREGS sregs;
  union REGS inregs, outregs;

  segread(&sregs);
  inregs.w.ax = 0;
  int386 (0x33, &inregs, &outregs);
  if (outregs.w.ax!=0xffff) mouse->cursor=1; else mouse->cursor=0;
#endif
}

void set_mouse(int x,int y) {
  m_x=(float)x;
  m_y=(float)y;
  SDL_WarpMouse(x,y);
}

float vx=0.0,vy=0.0,vmax;
#ifdef DEBUG
extern int mouse_x,mouse_y;
#endif

void readmouse(void) {
  
  short ix,iy;
  int keymouse=0,n=0;

  tecla();
#ifdef DOS
  union REGS regs;

//  tecla();

  mouse->left=0;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=3;
  int386(0x33,&regs,&regs);

  mouse->left=(regs.w.bx&1);
  mouse->middle=(regs.w.bx&4)/4;
  mouse->right=(regs.w.bx&2)/2;

  memset(&regs,0,sizeof(regs));
  regs.w.ax=0xb;
  int386(0x33,&regs,&regs);

  ix=regs.w.cx;
  iy=regs.w.dx;

  if (mouse->speed<0) mouse->speed=0;
  if (mouse->speed>9) mouse->speed=9;

  m_x+=(float)ix/(1.0+((float)mouse->speed/3.0));
  m_y+=(float)iy/(1.0+((float)mouse->speed/3.0));

  // Control del ratขn con el teclado

  if (mouse->cursor&1) {

    if(key(_ENTER)) mouse->left=1;

    vmax=1.0-(float)mouse->speed/15.0;

    if (key(_LEFT)&&!key(_RIGHT)) { vx-=vmax; keymouse|=1; }
    if (key(_RIGHT)&&!key(_LEFT)) { vx+=vmax; keymouse|=1; }
    if (key(_UP)&&!key(_DOWN)) { vy-=vmax; keymouse|=2; }
    if (key(_DOWN)&&!key(_UP)) { vy+=vmax; keymouse|=2; }

    if (vx || vy) { //keymouse) {
      if (vx<-vmax*10.0) vx=-vmax*10.0; if (vx>vmax*10.0) vx=vmax*10.0;
      if (vy<-vmax*10.0) vy=-vmax*10.0; if (vy>vmax*10.0) vy=vmax*10.0;
      m_x+=vx;
      m_y+=vy;
    }

    if (!(keymouse&1)) {
      if (vx>1.0) vx-=2.0;
      else if (vx<-1.0) vx+=2.0;
      else vx=0.0;
    }

    if (!(keymouse&2)) {
      if (vy>1.0) vy-=2.0;
      else if (vy<-1.0) vy+=2.0;
      else vy=0.0;
    }

  }
#endif
//if(mouse->left) mouse_b=1; else mouse_b=0;
  _mouse_x=(int)mouse->x;
  _mouse_y=(int)mouse->y;

  if (_mouse_x<0) { _mouse_x=0; n++; }
  else if (_mouse_x>=vga_an) { _mouse_x=vga_an-1; n++; }
  if (_mouse_y<0) { _mouse_y=0; n++; }
  else if (_mouse_y>=vga_al) { _mouse_y=vga_al-1; n++; }

  if (n) set_mouse(_mouse_x,_mouse_y);

  mouse->x=_mouse_x;
  mouse->y=_mouse_y;
#ifdef DEBUG
			mouse_x = _mouse_x;
			mouse_y = _mouse_y;
#endif

}

