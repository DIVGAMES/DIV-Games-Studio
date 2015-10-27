
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo que contiene las primitivas b sicas para ventanas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

void bwput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n);
void wtexc(byte*copia,int an_real_copia,int an_copia,int al_copia,
           byte*p,int x,int y,byte an,int al,byte c);

void wtexn(byte*copia,int an_real_copia,byte*p,int x,int y,byte an,int al,byte c) ;

           
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dibuja un boton en una ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void boton(int n,int x,int y,int centro,int color) {
  wwrite(v.ptr,v.an/big2,v.al/big2,x,y,centro,texto[100+n],color);
}

int ratonboton(int n,int x,int y,int centro) {
  int an,al;
  int mx=wmouse_x,my=wmouse_y;

  an=text_len(texto[100+n]+1); al=7;

  switch (centro) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }

  return(mx>=x-3 && mx<=x+an+3 && my>=y-3 && my<=y+al+3);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dibuja una caja en pantalla
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wbox(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  wbox_in_box(copia,an_copia,an_copia,al_copia,c,x,y,an,al);
}

void wbox_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  byte *p;

  if (big) {
    an_real_copia*=2; an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<0) {al+=y; y=0;}
  if (x<0) {an+=x; x=0;}
  if (y+al>al_copia) al=al_copia-y;
  if (x+an>an_copia) an=an_copia-x;

  if (an>0 && al>0) {
    p=copia+y*an_real_copia+x;
    do {
      memset(p,c,an);
      p+=an_real_copia;
    } while (--al);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Barra de tกtulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

char gradient[]={
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
  0,1,0,0,0,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,0,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,
  0,1,0,1,1,0,0,0,0,1,0,1,0,0,1,0,
  0,1,0,1,1,0,0,0,0,1,0,1,1,0,1,0,
  0,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,
  1,1,0,1,1,0,1,0,0,1,0,1,1,0,1,0,
  1,1,0,1,1,0,1,0,0,1,1,1,1,0,1,0,
  1,1,0,1,1,0,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,0,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,
  1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,
  1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

extern char cbs[65],cbn,cgs[65],cgn,crs[65],crn;

void wgra(byte*copia,int an_copia,int al_copia,byte color,int x,int y,int an,int al) {
  int xx,yy,c,m;
  char * cs, cn;

  if (big) {
    an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<0) {al+=y; y=0;}
  if (x<0) {an+=x; x=0;}
  if (y+al>al_copia) al=al_copia-y;
  if (x+an>an_copia) an=an_copia-x;

  if (color==c_r_low) {
    cn=crn; cs=crs;
  } else if (color==c1) {
    cn=cgn; cs=cgs;
  } else {
    cn=cbn; cs=cbs;
  }

  if (an>0 && al>0) {
    xx=0;
    do {
      c=(cn-1)*xx*16/an; m=c%16; c/=16;
      yy=0;
      do {
        if (gradient[m*16+(xx%4)+(yy%4)*4]) {
          copia[(y+yy)*an_copia+(x+xx)]=cs[c+1];
        } else {
          copia[(y+yy)*an_copia+(x+xx)]=cs[c];
        }
      } while (++yy<al);
    } while (++xx<an);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Resalta una caja, como un icono sobre el que se ponga el cursor
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wresalta_box(byte*copia,int an_copia,int al_copia,int x,int y,int an,int al) {
  byte *p;

  if (big) {
    an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<0) {al+=y; y=0;}
  if (x<0) {an+=x; x=0;}
  if (y+al>al_copia) al=al_copia-y;
  if (x+an>an_copia) an=an_copia-x;

  if (an>0 && al>0) {
    p=copia+y*an_copia+x; x=an;
    do {
      do {
        if (*p==c2) *p=c1; else if (*p==c3) *p=c2; p++;
      } while (--an);
      p+=an_copia-(an=x);
    } while (--al);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dibuja un rect ngulo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wrectangulo(byte*copia,int an_copia,int al_copia,byte c,int x,int y,int an,int al) {
  wbox(copia,an_copia,al_copia,c,x,y,an,1);
  wbox(copia,an_copia,al_copia,c,x,y+al-1,an,1);
  wbox(copia,an_copia,al_copia,c,x,y+1,1,al-2);
  wbox(copia,an_copia,al_copia,c,x+an-1,y+1,1,al-2);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Pone un gr fico
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wput(byte*copia,int an_copia,int al_copia,int x,int y,int n) {
  wput_in_box(copia,an_copia,an_copia,al_copia,x,y,n);
}

void wput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n) {

  int al,an;
  int block;
  byte *p,*q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (big) if ((n>=32 || n<0) && n!=233) { bwput_in_box(copia,an_real_copia,an_copia,al_copia,x,y,n); return; }
  else if (n==mouse_graf && mouse_shift) { x=mouse_shift_x; y=mouse_shift_y; }

  if (n<0) { n=-n; block=1; } else block=0;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  q=copia+y*an_real_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  if ((long_x=an-salta_x-resto_x)<=0) return;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  if ((long_y=al-salta_y-resto_y)<=0) return;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

void bwput_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,int x,int y,int n) {

  int al,an;
  int block;
  byte *p,*q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (n<0) { n=-n; block=1; } else block=0;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4)); 
  y-=*((word*)(graf[n]+6));

  if (an_copia>0) {
    an_real_copia*=2; an_copia*=2; al_copia*=2; x*=2; y*=2;
  } else {
    an_copia=-an_copia; if (an_real_copia<0) an_real_copia=-an_real_copia;
  }

  q=copia+y*an_real_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  if ((long_x=an-salta_x-resto_x)<=0) return;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  if ((long_y=al-salta_y-resto_y)<=0) return;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  if (block) do {
    do {
      switch(*p) {
        case 0: *q=c0; break;
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
  else do {
    do {
      switch(*p) {
        case 1: *q=c1; break;
        case 2: *q=c2; break;
        case 3: *q=c3; break;
        case 4: *q=c4; break;
      } p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado de una ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wvolcado(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta) {

  byte *q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  q=copia+y*an_copia+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia+salta; else resto_x=salta;
  long_x=an+salta-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_copia*salta_y+salta_x;
  resto_x+=salta_x+long_x; an=long_x;
  do {
    memcpy(q,p,an); q+=an_copia; p+=resto_x;
  } while (--long_y);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado de una ventana
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void wvolcado_oscuro(byte*copia,int an_copia,int al_copia,
              byte *p,int x,int y,int an,int al,int salta) {

  byte *q,*_ghost;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  q=copia+y*an_copia+x;
  _ghost=ghost+256*(int)c0;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia+salta; else resto_x=salta;
  long_x=an+salta-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  do {
    do {
      *q=*(_ghost+*p);
      p++; q++;
    } while (--an);
    q+=an_copia-(an=long_x); p+=resto_x;
  } while (--long_y);

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones de impresiขn de un texto
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int char_len(char c) {
  struct { byte an; word dir; } * car;
  car=(void*)(text_font+1); return(car[c].an);
}

int text_len(byte * ptr) {

  int an;

  struct {
    byte an;
    word dir;
  } * car;

  car=(void*)(text_font+1); an=0;
  while (*ptr) { an+=car[*ptr].an; ptr++; }

  if (big) an/=2;

  if (an) an--; return(an);

}

int text_len2(byte * ptr) {
  int an;
  struct {
    byte an;
    word dir;
  } * car;
  car=(void*)(text_font+1); an=0;
  while (*ptr) { an+=car[*ptr].an; ptr++; }
  if (big) an/=2; return(an-1);
}

void wwrite(byte*copia,int an_copia,int al_copia,
            int x,int y,int centro,byte * ptr,byte c) {
  wwrite_in_box(copia,an_copia,an_copia,al_copia,x,y,centro,ptr,c);
}

void wwrite_in_box(byte*copia,int an_real_copia,int an_copia,int al_copia,
            int x,int y,int centro,byte * ptr,byte c) {

  int an,al,boton,multi;

  struct {
    byte an;
    word dir;
  } * car;

  byte * font;

  if (centro>=10) { centro-=10; multi=1; } else multi=0;

  if (*ptr=='\xd') { boton=1; ptr++; } else boton=0;

  car=(void*)(text_font+1);

  if (big&&!multi) {
    an=text_len(ptr); al=7;
  } else {
    font=ptr; an=0; while (*font) { an+=car[*font].an; font++; }
    al=*text_font; if (an) an--;
  }

  font=text_font+1025;

  switch (centro) {
    case 0: break;
    case 1: x=x-(an>>1); break;
    case 2: x=x-an+1; break;
    case 3: y=y-(al>>1); break;
    case 4: x=x-(an>>1); y=y-(al>>1); break;
    case 5: x=x-an+1; y=y-(al>>1); break;
    case 6: y=y-al+1; break;
    case 7: x=x-(an>>1); y=y-al+1; break;
    case 8: x=x-an+1; y=y-al+1; break;
  }

  if (boton) if (c!=c0) {
    wbox(copia,an_real_copia,al_copia,c2,x-2,y-2,an+4,al+4);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-3,y-3,an+6,al+6);
    wrectangulo(copia,an_real_copia,al_copia,c3,x-2,y-2,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c3,x-2,y-2,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c4,x-2,y-2,1,1);
    wrectangulo(copia,an_real_copia,al_copia,c1,x-1,y+al+1,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c1,x+an+1,y-1,1,al+3);
    if (big) {
      *(copia+(2*y-3)*an_real_copia*2+2*x-4)=c3;
      *(copia+(2*y-4)*an_real_copia*2+2*x-3)=c3;
      *(copia+(2*y-4)*an_real_copia*2+2*(x+an)+2)=c3;
      *(copia+(2*y-3)*an_real_copia*2+2*(x+an)+3)=c1;
      *(copia+(2*(y+al)+2)*an_real_copia*2+2*x-4)=c3;
      *(copia+(2*(y+al)+3)*an_real_copia*2+2*x-3)=c1;
    }
  } else {
    wbox(copia,an_real_copia,al_copia,c1,x-2,y-2,an+4,al+4);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-3,y-3,an+6,al+6);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-2,y-2,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c0,x-2,y-2,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c2,x-1,y+al+1,an+3,1);
    wrectangulo(copia,an_real_copia,al_copia,c2,x+an+1,y-1,1,al+3);
    wrectangulo(copia,an_real_copia,al_copia,c3,x+an+1,y+al+1,1,1);
    if (big) {
      *(copia+(2*(y+al)+3)*an_real_copia*2+2*(x+an)+2)=c2;
      *(copia+(2*(y+al)+2)*an_real_copia*2+2*(x+an)+3)=c2;
      *(copia+(2*y-4)*an_real_copia*2+2*(x+an)+2)=c0;
      *(copia+(2*y-3)*an_real_copia*2+2*(x+an)+3)=c2;
      *(copia+(2*(y+al)+2)*an_real_copia*2+2*x-4)=c0;
      *(copia+(2*(y+al)+3)*an_real_copia*2+2*x-3)=c2;
    }
  }

  if (big&&!multi) {
    an_real_copia*=2; an_copia*=2; al_copia*=2;
    x*=2; y*=2; an*=2; al*=2;
  }

  if (y<al_copia && y+al>0) {
    if (y>=0 && y+al<=al_copia) { // El texto coge entero (coord. y)
      while (*ptr && x+car[*ptr].an<=0) { x=x+car[*ptr].an; ptr++; }
      if (*ptr && x<0) {
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
      while (*ptr && x+car[*ptr].an<=an_copia) {
        wtexn(copia,an_real_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
      if (*ptr && x<an_copia)
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
    } else {
      while (*ptr && x+car[*ptr].an<=0) { x=x+car[*ptr].an; ptr++; }
      while (*ptr && x<an_copia) {
        wtexc(copia,an_real_copia,an_copia,al_copia,font+car[*ptr].dir,x,y,car[*ptr].an,al,c);
        x=x+car[*ptr].an; ptr++; }
    }
  }
}

void wtexn(byte*copia,int an_real_copia,byte*p,int x,int y,byte an,int al,byte c) {

  byte *q=copia+y*an_real_copia+x;
  int ancho=an;

  do {
    do {
      if (*p) *q=c; p++; q++;
    } while (--an);
    q+=an_real_copia-(an=ancho);
  } while (--al);
}

void wtexc(byte*copia,int an_real_copia,int an_copia,int al_copia,
           byte*p,int x,int y,byte an,int al,byte c) {

  byte *q=copia+y*an_real_copia+x;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>an_copia) resto_x=x+an-an_copia; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>al_copia) resto_y=y+al-al_copia; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=an_real_copia*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      if (*p) *q=c; p++; q++;
    } while (--an);
    q+=an_real_copia-(an=long_x); p+=resto_x;
  } while (--long_y);
}

