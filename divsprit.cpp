
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo de impresiขn de sprites
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Graficos sin rotar ni escalar
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_normal( byte * p, int x, int y, int an, int al,
                int xg, int yg, int flags) {

  byte * q;

  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (mask_on) { sp_normal_mask(p,x,y,an,al,xg,yg,flags); return; }

  x-=xg; y-=yg;

  q=map+y*map_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>map_an) resto_x=x+an-map_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>map_al) resto_y=y+al-map_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=map_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  switch(flags) {

  case 0: // None
    do {
      do {
        if (*p) *q=*p;
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 1: // Ghost
    do {
      do {
        if (*p) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 2: // Block
    do {
      do {
        *q=*p;
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 3: // Ghost + Block
    do {
      do {
        *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  }
}

void sp_normal_mask( byte * p, int x, int y, int an, int al,
                     int xg, int yg, int flags) {

  byte * q;

  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  x-=xg; y-=yg;

  q=map+y*map_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>map_an) resto_x=x+an-map_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>map_al) resto_y=y+al-map_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=map_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  switch(flags) {

  case 0: // None
    do {
      do {
        if (*p && !mask[*q]) *q=*p;
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 1: // Ghost
    do {
      do {
        if (*p && !mask[*q]) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 2: // Block
    do {
      do {
        if (!mask[*q]) *q=*p;
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  case 3: // Ghost + Block
    do {
      do {
        if (!mask[*q]) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--an);
      q+=map_an-(an=long_x); p+=resto_x;
    } while (--long_y);
    break;

  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Pone un gr fico rotado y escalado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte * ptrmap;

void sp_rotado( byte * si, int x, int y, int an, int al, int xg, int yg,
                int flags, float a, float s) {

  float d0,d1,d2,d3;
  float a0,a1,a2,a3;
  int p[24];
  int h,hmin,hmax; // Altura minima y maxima
  int n,l0,l1;   // Lado 0 y lado 1 (indices p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y;

  xg=-xg; yg=-yg;

  d0=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a0=a; else a0=a+(float)atan2(-yg,xg);
  xg=xg+an-1;
  d1=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a1=a; else a1=a+(float)atan2(-yg,xg);
  yg=yg+al-1;
  d2=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a2=a; else a2=a+(float)atan2(-yg,xg);
  xg=xg-an+1;
  d3=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a3=a; else a3=a+(float)atan2(-yg,xg);

  p[0]=x+(int)((float)cos(a0)*d0); p[1]=y+(int)(-(float)sin(a0)*d0);
  p[2]=x+(int)((float)cos(a1)*d1); p[3]=y+(int)(-(float)sin(a1)*d1);
  p[4]=x+(int)((float)cos(a2)*d2); p[5]=y+(int)(-(float)sin(a2)*d2);
  p[6]=x+(int)((float)cos(a3)*d3); p[7]=y+(int)(-(float)sin(a3)*d3);

  memcpy(&p[8],&p[0],sizeof(p[0])*8); memcpy(&p[16],&p[0],sizeof(p[0])*8);

  hmin=32767; hmax=-32768; n=8; do {
    if (p[n+1]<hmin) { l0=n; hmin=p[n+1]; }
    if (p[n+1]>hmax) hmax=p[n+1];
    n+=2;
  } while (n<16);

  l1=l0; hmax0=hmin; hmax1=hmin; ptrmap=map+hmin*map_an;

  h=hmin; do {

    if (h<hmax) {
      while (h==hmax0) {
        if ((hmax0=p[l0-1])!=h) {
          x0.l=p[l0]<<16;
          ix0=((p[l0-2]<<16)-x0.l)/(hmax0-h);
          if (ix0<0) x0.l+=65535;
          switch(l0&6) {
            case 0: g0y.l=0; ig0y=(al<<16)/(hmax0-h);
                    g0x.l=0; ig0x=0; break;
            case 2: g0x.l=((an-1)<<16)+65535; ig0x=(an<<16)/(h-hmax0);
                    g0y.l=0; ig0y=0; break;
            case 4: g0y.l=((al-1)<<16)+65535; ig0y=(al<<16)/(h-hmax0);
                    g0x.l=((an-1)<<16)+65535; ig0x=0; break;
            case 6: g0x.l=0; ig0x=(an<<16)/(hmax0-h);
                    g0y.l=((al-1)<<16)+65535; ig0y=0; break;
          }
        } l0-=2;
      }
      while (h==hmax1) {
        if ((hmax1=p[l1+3])!=h) {
          x1.l=p[l1]<<16;
          ix1=((p[l1+2]<<16)-x1.l)/(hmax1-h);
          if (ix1<0) x1.l+=65535;
          switch(l1&6) {
            case 0: g1x.l=0; ig1x=(an<<16)/(hmax1-h);
                    g1y.l=0; ig1y=0; break;
            case 2: g1y.l=0; ig1y=(al<<16)/(hmax1-h);
                    g1x.l=((an-1)<<16)+65535; ig1x=0; break;
            case 4: g1x.l=((an-1)<<16)+65535; ig1x=(an<<16)/(h-hmax1);
                    g1y.l=((al-1)<<16)+65535; ig1y=0; break;
            case 6: g1y.l=((al-1)<<16)+65535; ig1y=(al<<16)/(h-hmax1);
                    g1x.l=0; ig1x=0; break;
          }
        } l1+=2;
      }
    } else {
      if (h==hmax0) {
        g0x.l=0; g0y.l=0; ig0x=0; ig0y=0;
        x0.l=p[l0]<<16; ix0=0;
        switch(l0&6) {
          case 2: g0x.l=((an-1)<<16)+65535; break;
          case 4: g0y.l=((al-1)<<16)+65535; g0x.l=((an-1)<<16)+65535; break;
          case 6: g0y.l=((al-1)<<16)+65535; break;
        }
      }
      if (h==hmax1) {
        g1x.l=0; g1y.l=0; ig1x=0; ig1y=0;
        x1.l=p[l1]<<16; ix1=0;
        switch(l1&6) {
          case 2: g1x.l=((an-1)<<16)+65535; break;
          case 4: g1x.l=((an-1)<<16)+65535; g1y.l=((al-1)<<16)+65535; break;
          case 6: g1y.l=((al-1)<<16)+65535; break;
        }
      }
    }

    if (h<map_al && h>=0 && x0.w[1]<map_an && x1.w[1]>=0 && x1.w[1]>x0.w[1])
    if (!mask_on) {
      if (x0.w[1]<0) if (x1.w[1]>=map_an)
        sp_scanc(ptrmap,x1.w[1]-x0.w[1],map_an-1,-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scanc(ptrmap,x1.w[1]-x0.w[1],x1.w[1],-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else if (x1.w[1]>=map_an)
        sp_scanc(ptrmap+x0.w[1],x1.w[1]-x0.w[1],map_an-1-x0.w[1],0,si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scan(ptrmap+x0.w[1],x1.w[1]-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
    } else {
      if (x0.w[1]<0) if (x1.w[1]>=map_an)
        sp_scanc_mask(ptrmap,x1.w[1]-x0.w[1],map_an-1,-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scanc_mask(ptrmap,x1.w[1]-x0.w[1],x1.w[1],-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else if (x1.w[1]>=map_an)
        sp_scanc_mask(ptrmap+x0.w[1],x1.w[1]-x0.w[1],map_an-1-x0.w[1],0,si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scan_mask(ptrmap+x0.w[1],x1.w[1]-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l,flags);
    }

    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrmap+=map_an;

  } while (h++<hmax);

}

void sp_scanc(byte * p,short n,short m,short o,byte * si,int an,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  switch(flags) {
  case 0: // None
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 1: // Ghost
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  }
}

void sp_scan(byte * p,short n,byte * si,int an,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  switch(flags) {
  case 0: // None
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 1: // Ghost
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  }
}

void sp_scanc_mask(byte * p,short n,short m,short o,byte * si,int an,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  switch(flags) {
  case 0: // None
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 1: // Ghost
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  }
}

void sp_scan_mask(byte * p,short n,byte * si,int an,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  switch(flags) {
  case 0: // None
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 1: // Ghost
    do {
      if (c=*(si+x.w[1]+y.w[1]*an)) if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*an); if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Invierte horizontalmente un gr fico
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void invierte_hor(byte * p, int an, int al) {
  int x,y;
  byte * q;

  for (y=0;y<al;y++) {
    q=p+an-1;
    for (x=0;x<an/2;x++) {
      *p^=*q; *q^=*p; *p^=*q;
      p++; q--;
    } p+=an-an/2;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Invierte verticalmente un gr fico
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void invierte_ver(byte * sp, int an, int al) {
  int x,y;
  byte * q, * p;

  for (x=0;x<an;x++) {
    p=sp++; q=p+(al-1)*an;
    for (y=0;y<al/2;y++) {
      *p^=*q; *q^=*p; *p^=*q;
      p+=an; q-=an;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Devuelve el ancho y alto de un gr fico rotado y/o escalado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_size( int *x, int *y, int *an, int *al, int xg, int yg,
              float a, float s) {

  float d0,d1,d2,d3;
  float a0,a1,a2,a3;

  int p[8],x0,x1,y0,y1,n;

  xg=-xg; yg=-yg;

  d0=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a0=a; else a0=a+(float)atan2(-yg,xg);
  xg=xg+*an-1;
  d1=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a1=a; else a1=a+(float)atan2(-yg,xg);
  yg=yg+*al-1;
  d2=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a2=a; else a2=a+(float)atan2(-yg,xg);
  xg=xg-*an+1;
  d3=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a3=a; else a3=a+(float)atan2(-yg,xg);

  p[0]=*x+(int)((float)cos(a0)*d0); p[1]=*y+(int)(-(float)sin(a0)*d0);
  p[2]=*x+(int)((float)cos(a1)*d1); p[3]=*y+(int)(-(float)sin(a1)*d1);
  p[4]=*x+(int)((float)cos(a2)*d2); p[5]=*y+(int)(-(float)sin(a2)*d2);
  p[6]=*x+(int)((float)cos(a3)*d3); p[7]=*y+(int)(-(float)sin(a3)*d3);

  x0=map_an; y0=map_al; x1=-map_an; y1=-map_al;

  for (n=0;n<8;n++) {
    if (x0>p[n]) x0=p[n]; if (x1<p[n]) x1=p[n]; n++;
    if (y0>p[n]) y0=p[n]; if (y1<p[n]) y1=p[n];
  }

  *x=x0; *y=y0; *an=x1-x0+1; *al=y1-y0+1;

}
