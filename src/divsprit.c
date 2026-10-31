
//═════════════════════════════════════════════════════════════════════════════
//      Sprites printing module
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"


void sp_normal_mask( byte * p, int x, int y, int width, int height,
                     int xg, int yg, int flags);

void sp_scanc(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1,int flags);

void sp_rotated( byte * si, int x, int y, int width, int height, int xg, int yg,
                int flags, float a, float s);

void sp_scan(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1,int flags);

void sp_scanc_mask(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1,int flags);

void sp_scan_mask(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1,int flags);


                
//═════════════════════════════════════════════════════════════════════════════
//      Graphics without rotation or scaling
//═════════════════════════════════════════════════════════════════════════════

void sp_normal( byte * p, int x, int y, int width, int height,
                int xg, int yg, int flags) {

  byte * q;

  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  if (mask_on) { sp_normal_mask(p,x,y,width,height,xg,yg,flags); return; }

  x-=xg; y-=yg;

  q=map+y*map_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+width>map_width) rest_x=x+width-map_width; else rest_x=0;
  long_x=width-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+height>map_height) rest_y=y+height-map_height; else rest_y=0;
  long_y=height-skip_y-rest_y;

  p+=width*skip_y+skip_x; q+=map_width*skip_y+skip_x;
  rest_x+=skip_x; width=long_x;

  switch(flags) {

  case 0: // None
    do {
      do {
        if (*p) *q=*p;
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 1: // Ghost
    do {
      do {
        if (*p) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 2: // Block
    do {
      do {
        *q=*p;
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 3: // Ghost + Block
    do {
      do {
        *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  }
}

void sp_normal_mask( byte * p, int x, int y, int width, int height,
                     int xg, int yg, int flags) {

  byte * q;

  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  x-=xg; y-=yg;

  q=map+y*map_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+width>map_width) rest_x=x+width-map_width; else rest_x=0;
  long_x=width-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+height>map_height) rest_y=y+height-map_height; else rest_y=0;
  long_y=height-skip_y-rest_y;

  p+=width*skip_y+skip_x; q+=map_width*skip_y+skip_x;
  rest_x+=skip_x; width=long_x;

  switch(flags) {

  case 0: // None
    do {
      do {
        if (*p && !mask[*q]) *q=*p;
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 1: // Ghost
    do {
      do {
        if (*p && !mask[*q]) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 2: // Block
    do {
      do {
        if (!mask[*q]) *q=*p;
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  case 3: // Ghost + Block
    do {
      do {
        if (!mask[*q]) *q=*(ghost+*q+*p*256);
        p++; q++;
      } while (--width);
      q+=map_width-(width=long_x); p+=rest_x;
    } while (--long_y);
    break;

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Put a rotated and scaled graphic
//═════════════════════════════════════════════════════════════════════════════

byte * ptrmap;

void sp_rotated( byte * si, int x, int y, int width, int height, int xg, int yg,
                int flags, float a, float s) {

  float d0,d1,d2,d3;
  float a0,a1,a2,a3;
  int p[24];
  int h,hmin,hmax; // Minimum and maximum height
  int n,l0,l1;   // Side 0 and side 1 (indices of p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y;

  xg=-xg; yg=-yg;

  d0=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a0=a; else a0=a+(float)atan2(-yg,xg);
  xg=xg+width-1;
  d1=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a1=a; else a1=a+(float)atan2(-yg,xg);
  yg=yg+height-1;
  d2=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a2=a; else a2=a+(float)atan2(-yg,xg);
  xg=xg-width+1;
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

  l1=l0; hmax0=hmin; hmax1=hmin; ptrmap=map+hmin*map_width;

  h=hmin; do {

    if (h<hmax) {
      while (h==hmax0) {
        if ((hmax0=p[l0-1])!=h) {
          x0.l=p[l0]<<16;
          ix0=((p[l0-2]<<16)-x0.l)/(hmax0-h);
          if (ix0<0) x0.l+=65535;
          switch(l0&6) {
            case 0: g0y.l=0; ig0y=(height<<16)/(hmax0-h);
                    g0x.l=0; ig0x=0; break;
            case 2: g0x.l=((width-1)<<16)+65535; ig0x=(width<<16)/(h-hmax0);
                    g0y.l=0; ig0y=0; break;
            case 4: g0y.l=((height-1)<<16)+65535; ig0y=(height<<16)/(h-hmax0);
                    g0x.l=((width-1)<<16)+65535; ig0x=0; break;
            case 6: g0x.l=0; ig0x=(width<<16)/(hmax0-h);
                    g0y.l=((height-1)<<16)+65535; ig0y=0; break;
          }
        } l0-=2;
      }
      while (h==hmax1) {
        if ((hmax1=p[l1+3])!=h) {
          x1.l=p[l1]<<16;
          ix1=((p[l1+2]<<16)-x1.l)/(hmax1-h);
          if (ix1<0) x1.l+=65535;
          switch(l1&6) {
            case 0: g1x.l=0; ig1x=(width<<16)/(hmax1-h);
                    g1y.l=0; ig1y=0; break;
            case 2: g1y.l=0; ig1y=(height<<16)/(hmax1-h);
                    g1x.l=((width-1)<<16)+65535; ig1x=0; break;
            case 4: g1x.l=((width-1)<<16)+65535; ig1x=(width<<16)/(h-hmax1);
                    g1y.l=((height-1)<<16)+65535; ig1y=0; break;
            case 6: g1y.l=((height-1)<<16)+65535; ig1y=(height<<16)/(h-hmax1);
                    g1x.l=0; ig1x=0; break;
          }
        } l1+=2;
      }
    } else {
      if (h==hmax0) {
        g0x.l=0; g0y.l=0; ig0x=0; ig0y=0;
        x0.l=p[l0]<<16; ix0=0;
        switch(l0&6) {
          case 2: g0x.l=((width-1)<<16)+65535; break;
          case 4: g0y.l=((height-1)<<16)+65535; g0x.l=((width-1)<<16)+65535; break;
          case 6: g0y.l=((height-1)<<16)+65535; break;
        }
      }
      if (h==hmax1) {
        g1x.l=0; g1y.l=0; ig1x=0; ig1y=0;
        x1.l=p[l1]<<16; ix1=0;
        switch(l1&6) {
          case 2: g1x.l=((width-1)<<16)+65535; break;
          case 4: g1x.l=((width-1)<<16)+65535; g1y.l=((height-1)<<16)+65535; break;
          case 6: g1y.l=((height-1)<<16)+65535; break;
        }
      }
    }

    if (h<map_height && h>=0 && x0.w[1]<map_width && x1.w[1]>=0 && x1.w[1]>x0.w[1]) {
    
    if (!mask_on) {
      if (x0.w[1]<0) if (x1.w[1]>=map_width)
        sp_scanc(ptrmap,x1.w[1]-x0.w[1],map_width-1,-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scanc(ptrmap,x1.w[1]-x0.w[1],x1.w[1],-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else if (x1.w[1]>=map_width)
        sp_scanc(ptrmap+x0.w[1],x1.w[1]-x0.w[1],map_width-1-x0.w[1],0,si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scan(ptrmap+x0.w[1],x1.w[1]-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
    } else {
      if (x0.w[1]<0) if (x1.w[1]>=map_width)
        sp_scanc_mask(ptrmap,x1.w[1]-x0.w[1],map_width-1,-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scanc_mask(ptrmap,x1.w[1]-x0.w[1],x1.w[1],-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else if (x1.w[1]>=map_width)
        sp_scanc_mask(ptrmap+x0.w[1],x1.w[1]-x0.w[1],map_width-1-x0.w[1],0,si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
      else
        sp_scan_mask(ptrmap+x0.w[1],x1.w[1]-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l,flags);
    }

	}
	
    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrmap+=map_width;

  } while (h++<hmax);

}

void sp_scanc(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  switch(flags) {
  case 0: // None
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 1: // Ghost
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) 
      	*p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  }
}

void sp_scan(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  switch(flags) {
  case 0: // None
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 1: // Ghost
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  }
}

void sp_scanc_mask(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  switch(flags) {
  case 0: // None
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 1: // Ghost
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (m--); break;

  }
}

void sp_scan_mask(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1,int flags) {

  union { int l; short w[2]; } x,y;
  byte c;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  switch(flags) {
  case 0: // None
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 1: // Ghost
    do {
      if ((c=*(si+x.w[1]+y.w[1]*width))) if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 2: // Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); if (!mask[*p]) *p=c; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  case 3: // Ghost + Block
    do {
      c=*(si+x.w[1]+y.w[1]*width); if (!mask[*p]) *p=ghost[(c<<8)+*p]; p++;
      x.l+=x0; y.l+=y0;
    } while (n--); break;

  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Flip a graphic horizontally
//═════════════════════════════════════════════════════════════════════════════

void invert_horizontal(byte * p, int width, int height) {
  int x,y;
  byte * q;

  for (y=0;y<height;y++) {
    q=p+width-1;
    for (x=0;x<width/2;x++) {
      *p^=*q; *q^=*p; *p^=*q;
      p++; q--;
    } p+=width-width/2;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Flip a graphic vertically
//═════════════════════════════════════════════════════════════════════════════

void invert_vertical(byte * sp, int width, int height) {
  int x,y;
  byte * q, * p;

  for (x=0;x<width;x++) {
    p=sp++; q=p+(height-1)*width;
    for (y=0;y<height/2;y++) {
      *p^=*q; *q^=*p; *p^=*q;
      p+=width; q-=width;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Return the width and height of a rotated and/or scaled graphic
//═════════════════════════════════════════════════════════════════════════════

void sp_size( int *x, int *y, int *width, int *height, int xg, int yg,
              float a, float s) {

  float d0,d1,d2,d3;
  float a0,a1,a2,a3;

  int p[8],x0,x1,y0,y1,n;

  xg=-xg; yg=-yg;

  d0=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a0=a; else a0=a+(float)atan2(-yg,xg);
  xg=xg+*width-1;
  d1=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a1=a; else a1=a+(float)atan2(-yg,xg);
  yg=yg+*height-1;
  d2=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a2=a; else a2=a+(float)atan2(-yg,xg);
  xg=xg-*width+1;
  d3=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a3=a; else a3=a+(float)atan2(-yg,xg);

  p[0]=*x+(int)((float)cos(a0)*d0); p[1]=*y+(int)(-(float)sin(a0)*d0);
  p[2]=*x+(int)((float)cos(a1)*d1); p[3]=*y+(int)(-(float)sin(a1)*d1);
  p[4]=*x+(int)((float)cos(a2)*d2); p[5]=*y+(int)(-(float)sin(a2)*d2);
  p[6]=*x+(int)((float)cos(a3)*d3); p[7]=*y+(int)(-(float)sin(a3)*d3);

  x0=map_width; y0=map_height; x1=-map_width; y1=-map_height;

  for (n=0;n<8;n++) {
    if (x0>p[n]) x0=p[n]; if (x1<p[n]) x1=p[n]; n++;
    if (y0>p[n]) y0=p[n]; if (y1<p[n]) y1=p[n];
  }

  *x=x0; *y=y0; *width=x1-x0+1; *height=y1-y0+1;

}
