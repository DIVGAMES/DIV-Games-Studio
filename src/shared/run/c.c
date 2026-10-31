
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Code of the collision detection functions
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
#include "inter.h"
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Headers
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
int  check_collisions(int i,int block,int scroll);
void test_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags);
void sp_rotated_p(byte * si, int width, int height, int flags);
void test_normal(byte * p, int x, int y, int width, int height, int flags);
void test_clipped(byte * p, int x, int y, int width, int height, int flags);
void test_scaled(byte * old_si, int x, int y, int width, int height, int xg, int yg,int size, int flags);
void test_rotated(byte * si, int width, int height, int flags);
void test_scanc(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1);
void test_scan(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables defined at module level
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int collides;
int p[24];

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Out_region(id,region) - only for screen or scroll graphics
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void out_region(void) {

  int id,file,graph,reg,angle;
  int xg,yg,xg1,yg1,x,y,width,height;
  int * ptr,n,m;

  reg=opstack[sp--]; id=opstack[sp]; opstack[sp]=1; // By default it is not in the region

  if (mem[id+_Ctype]==2) { e(137); return; }

  file=mem[id+_File]; graph=mem[id+_Graph]; angle=mem[id+_Angle];

  if (reg<0 || reg>=max_region) { e(108); return; } // It already has region[reg].(x0..y1)

  if (file<0 || file>max_fpgs) { e(109); return; }

  if ((n=mem[id+_XGraph])>0) {
    m=mem[n]; if (m<1 || m>256) return;
    while (angle>=2*pi) angle-=2*pi;
    while (angle<0) angle+=2*pi;
    mem[id+_Flags]&=254;
    graph=((angle+(2*pi)/(m*2))*m)/(2*pi); angle=0;
    if (graph>=m) graph=0;
    if ((graph=mem[n+1+graph])<0) { graph=-graph; mem[id+_Flags]|=1; }
  }

  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  x=mem[id+_X]; y=mem[id+_Y];
  if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  width=ptr[13]; height=ptr[14];

  if (angle) {
    sp_size(&x,&y,&width,&height,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);
    width=width-x+1; height=height-y+1;
  } else if (mem[id+_Size]!=100) {
    sp_size_scaled(&x,&y,&width,&height,xg,yg,mem[id+_Size],mem[id+_Flags]);
    width=width-x+1; height=height-y+1;
  } else {
    if (mem[id+_Flags]&1) xg=width-1-xg; x-=xg;
    if (mem[id+_Flags]&2) yg=height-1-yg; y-=yg;
  }

  // It already has region[reg].(x0..y1) and sprite(x,y,width,height)

  if (mem[id+_Ctype]==1) {

    for (n=0;n<10;n++)if(iscroll[n].on&&(mem[id+_Cnumber]==0||(mem[id+_Cnumber]&(1<<n)))) {

      xg=x+iscroll[n].x-iscroll[n].map1_x; // Position of the sprite in the scroll
      yg=y+iscroll[n].y-iscroll[n].map1_y;

      xg1=xg+width; yg1=yg+height; // 1st Intersection of xg,yg,width,height and iscroll[n]

      if (iscroll[n].x>xg) xg=iscroll[n].x;
      if (iscroll[n].y>yg) yg=iscroll[n].y;
      if (iscroll[n].x+iscroll[n].w<xg1) xg1=iscroll[n].x+iscroll[n].w;
      if (iscroll[n].y+iscroll[n].h<yg1) yg1=iscroll[n].y+iscroll[n].h;

      if (xg>=xg1 || yg>=yg1) continue; // If there is no intersection, continue

      if (xg<region[reg].x1 && yg<region[reg].y1 && xg1>region[reg].x0 &&
        yg1>region[reg].y0) { opstack[sp]=0; return; } // If it is in the region

    }

  } else if (mem[id+_Ctype]==0) {

    if (x<region[reg].x1 && y<region[reg].y1 && x+width>region[reg].x0 &&
        y+height>region[reg].y0) opstack[sp]=0; // If it is in the region

  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Graphic_info(file,graph,info)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void graphic_info(void) {

  int info,graph,file;
  int xg,yg;
  int * ptr;

  info=opstack[sp--]; graph=opstack[sp--]; file=opstack[sp]; opstack[sp]=0;

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  switch(opstack[sp+2]) {
    case 0: opstack[sp]=ptr[13]; break;                    //g_wide (original)
    case 1: opstack[sp]=ptr[14]; break;                    //g_height (original)
    case 2: opstack[sp]=xg; break;                         //g_x_center
    case 3: opstack[sp]=yg; break;                         //g_y_center
    default: e(138);
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Main function of the language collision(process type)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// *** NOTE, Detects screen-screen, scroll-scroll and screen-scroll collisions
// But the graphics that are in a scroll detect collisions only in the first
// scroll they are in (even if they are in several)

void collision(void) {

  int i,block; // Go through processes from _IdScan onwards, if _IdScan=0 from start
  int file,graph,angle;
  int x,y,n,m;  // Coordinates of the current process
  int64_t xg,yg;  // Center of gravity of the current process's graphic
  int * ptr;

  block=opstack[sp]; opstack[sp]=0; // By default it does not collide

  if (mem[id+_Ctype]==2) { e(139); return; }

  if (mem[id+_IdScan]==0 || block!=mem[id+_BlScan]) {
    mem[id+_BlScan]=block; i=id_start;
  } else if (mem[id+_IdScan]>id_end) { opstack[sp]=0; return; }
         else i=mem[id+_IdScan];

  file=mem[id+_File]; graph=mem[id+_Graph]; angle=mem[id+_Angle];

  if (file<0 || file>max_fpgs) { e(109); return; }

  if ((n=mem[id+_XGraph])>0) {
    m=mem[n]; if (m<1 || m>256) return;
    while (angle>=2*pi) angle-=2*pi;
    while (angle<0) angle+=2*pi;
    mem[id+_Flags]&=254;
    graph=((angle+(2*pi)/(m*2))*m)/(2*pi); angle=0;
    if (graph>=m) graph=0;
    if ((graph=mem[n+1+graph])<0) { graph=-graph; mem[id+_Flags]|=1; }
  }

  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  x=mem[id+_X]; y=mem[id+_Y];
  if (mem[id+_Resolution]>0) { x/=mem[id+_Resolution]; y/=mem[id+_Resolution]; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  if (angle) {
    clipx0=x; clipy0=y; clipx1=ptr[13]; clipy1=ptr[14];
    sp_size(&clipx0,&clipy0,&clipx1,&clipy1,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);
  } else if (mem[id+_Size]!=100) {
    clipx0=x; clipy0=y; clipx1=ptr[13]; clipy1=ptr[14];
    sp_size_scaled(&clipx0,&clipy0,&clipx1,&clipy1,xg,yg,mem[id+_Size],mem[id+_Flags]);
  } else {
    if (mem[id+_Flags]&1) clipx0=x-(ptr[13]-1-xg); else clipx0=x-xg;
    if (mem[id+_Flags]&2) clipy0=y-(ptr[14]-1-yg); else clipy0=y-yg;
    clipx1=clipx0+ptr[13]-1; clipy1=clipy0+ptr[14]-1;
  }

  buffer_width=clipx1-clipx0+1; buffer_height=clipy1-clipy0+1;
  if ((buffer=(byte *)malloc(buffer_width*buffer_height))==NULL) { e(100); return; }
  memset(buffer,0,buffer_width*buffer_height);

  // Now the sprite has to be drawn into the buffer(clip...)

  put_collision(buffer,ptr,x,y,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);

  // It already has the sprite region clipx0..clipy1 (if it is on screen)

  if (mem[id+_Ctype]==1) {
    for(n=0;n<10;n++)if(iscroll[n].on&&(mem[id+_Cnumber]==0||(mem[id+_Cnumber]&(1<<n)))) {
      xg=iscroll[n].x-iscroll[n].map1_x;
      yg=iscroll[n].y-iscroll[n].map1_y;
      clipx0+=xg; clipx1+=xg; clipy0+=yg; clipy1+=yg;
      opstack[sp]=check_collisions(i,block,n); break;
    }
  } else opstack[sp]=check_collisions(i,block,-1);

  free(buffer);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Check collisions of sprite id (in buffer) with the rest (i..id_end)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int check_collisions(int i,int block,int scroll) {
  int file,graph,angle;
  int *ptr,n,m,j;
  int x,y,dist=0;
  int xx,yy,width,height;
  short xg,yg,xxg,yyg;

  if (block==0) { // collision(type mouse)
    if (mouse->x>=clipx0 && mouse->x<=clipx1 && mouse->y>=clipy0 && mouse->y<=clipy1) {
      if (*(buffer+buffer_width*(mouse->y-clipy0)+(mouse->x-clipx0))) return(id); else return(0);
    } else return(0);
  }

  for (;i<=id_end;i+=iloc_len) {

    if (i!=id && mem[i+_Block]==block && (mem[i+_Status]==2 ||
      	mem[i+_Status]==4) && mem[i+_Ctype]<2) {

      if (mem[i+_Ctype]==1) { // If it is a scroll one, determine which scroll (n)
        if (scroll<0) { // Screen-scroll collision - the first active one
      	  for(n=0;n<10;n++)
      	    if(iscroll[n].on&&(mem[i+_Cnumber]==0||(mem[i+_Cnumber]&(1<<n)))) break;
      	  if (n==10) continue;
        } else { // Scroll-scroll collision - as long as they coincide in the 1st
      	  if (mem[i+_Cnumber] && !(mem[i+_Cnumber]&(1<<scroll))) continue;
      	  else n=scroll;
      	}
      }

      file=mem[i+_File]; graph=mem[i+_Graph]; angle=mem[i+_Angle];

      if (file<0 || file>max_fpgs) continue;

      if ((j=mem[i+_XGraph])>0) {
        m=mem[j]; if (m<1 || m>256) continue;
        while (angle>=2*pi) angle-=2*pi;
        while (angle<0) angle+=2*pi;
        mem[i+_Flags]&=254;
        graph=((angle+(2*pi)/(m*2))*m)/(2*pi); angle=0;
        if (graph>=m) graph=0;
        if ((graph=mem[j+1+graph])<0) { graph=-graph; mem[i+_Flags]|=1; }
      }

      if (file) max_grf=1000; else max_grf=2000;
      if (graph<=0 || graph>=max_grf) continue;
      if (g[file].grf==NULL) continue;
      if ((ptr=g[file].grf[graph])==NULL) continue;

      x=mem[i+_X]; y=mem[i+_Y];
      if (mem[i+_Resolution]>0) { x/=mem[i+_Resolution]; y/=mem[i+_Resolution]; }

      if (mem[i+_Ctype]==1) {
      	x+=iscroll[n].x-iscroll[n].map1_x;
      	y+=iscroll[n].y-iscroll[n].map1_y;
      }

      if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
      } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

      collides=0;

      if (angle) {

        // Safety distance

        if (dist==0) dist=(int)sqrt((buffer_width*buffer_width+buffer_height*buffer_height)/4);
        width=((int)sqrt(ptr[13]*ptr[13]+ptr[14]*ptr[14])*mem[i+_Size])/100+dist;

        // Real distance

        xx=(clipx0+clipx1)/2-x; yy=(clipy0+clipy1)/2-y; height=(int)sqrt(xx*xx+yy*yy);

        if (height<=width) {
          xx=x; yy=y; width=ptr[13]; height=ptr[14];
          sp_size(&xx,&yy,&width,&height,xg,yg,angle,mem[i+_Size],mem[i+_Flags]);

          if (clipx1>=xx && clipx0<=width)
            if (clipy1>=yy && clipy0<height)
              collides=1;
        }

      } else if (mem[i+_Size]!=100) {

        xx=x; yy=y; width=ptr[13]; height=ptr[14];
        sp_size_scaled(&xx,&yy,&width,&height,xg,yg,mem[i+_Size],mem[i+_Flags]);

        if (clipx1>=xx && clipx0<=width)
          if (clipy1>=yy && clipy0<height) collides=1;

      } else {

        width=ptr[13]; height=ptr[14];
        if (mem[i+_Flags]&1) xxg=width-1-xg; else xxg=xg; xx=x-xxg;
        if (mem[i+_Flags]&2) yyg=height-1-yg; else yyg=yg; yy=y-yyg;
        if (clipx1>=xx && clipx0<xx+width)
          if (clipy1>=yy && clipy0<yy+height) collides=1;

      }

      if (collides==1) {
        collides=0;
        test_collision(buffer,ptr,x,y,xg,yg,angle,mem[i+_Size],mem[i+_Flags]);
        if (collides) {
      	  mem[id+_IdScan]=i+iloc_len; return(i);
        }
      }

    }
  }

  mem[id+_IdScan]=i; return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Return the box in which a rotated and scaled graphic lies
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_size( int *x, int *y, int *xx, int *yy, int xg, int yg,
              int ang, int size, int flags) {

  float d0,d1,d2,d3;
  float a0,a1,a2,a3;
  float a,s;
  int n;

  a=(float)(ang/radian); s=(float)(size/100.0f);

  xg=-xg; yg=-yg;

  d0=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a0=a; else a0=a+(float)atan2(-yg,xg);
  xg=xg+*xx-1;
  d1=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a1=a; else a1=a+(float)atan2(-yg,xg);
  yg=yg+*yy-1;
  d2=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a2=a; else a2=a+(float)atan2(-yg,xg);
  xg=xg-*xx+1;
  d3=(float)sqrt(xg*xg+yg*yg)*s; if (yg==0 && xg==0) a3=a; else a3=a+(float)atan2(-yg,xg);

  if (flags&1) {
    p[0]=*x-(int)((float)cos(a0)*d0); p[2]=*x-(int)((float)cos(a1)*d1);
    p[4]=*x-(int)((float)cos(a2)*d2); p[6]=*x-(int)((float)cos(a3)*d3);
  } else {
    p[0]=*x+(int)((float)cos(a0)*d0); p[2]=*x+(int)((float)cos(a1)*d1);
    p[4]=*x+(int)((float)cos(a2)*d2); p[6]=*x+(int)((float)cos(a3)*d3);
  }

  if (flags&2) {
    p[1]=*y-(int)(-(float)sin(a0)*d0); p[3]=*y-(int)(-(float)sin(a1)*d1);
    p[5]=*y-(int)(-(float)sin(a2)*d2); p[7]=*y-(int)(-(float)sin(a3)*d3);
  } else {
    p[1]=*y+(int)(-(float)sin(a0)*d0); p[3]=*y+(int)(-(float)sin(a1)*d1);
    p[5]=*y+(int)(-(float)sin(a2)*d2); p[7]=*y+(int)(-(float)sin(a3)*d3);
  }

  *x=0x7fffffff; *y=0x7fffffff; *xx=0x80000000; *yy=0x80000000;

  for (n=0;n<8;n++) {
    if (*x>p[n]) *x=p[n]; if (*xx<p[n]) *xx=p[n]; n++;
    if (*y>p[n]) *y=p[n]; if (*yy<p[n]) *yy=p[n];
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Return the box in which a rotated and/or scaled graphic lies
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_size_scaled( int *x, int *y, int *xx, int *yy, int xg, int yg,
                     int size, int flags) {

  int x0,y0,x1,y1; // Window occupied by the sprite in the copy plane

  if (flags&1) x0=*x-((*xx-1-xg)*size)/100;
  else x0=*x-(xg*size)/100;
  if (flags&2) y0=*y-((*yy-1-yg)*size)/100;
  else y0=*y-(yg*size)/100;

  x1=x0+(*xx*size)/100-1; y1=y0+(*yy*size)/100-1;

  if (x1<x0) x1=x0; if (y1<y0) y1=y0; // ***

  *x=x0; *y=y0; *xx=x1; *yy=y1;

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      External function to draw graphics (for collisions)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void put_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags) {

  byte * si;
  int width,height; // Information about the graphic relative to the screen
  byte * _copy;
  int _vga_width,_vga_height;
  int ix,iy;

  width=ptr[13]; height=ptr[14]; si=(byte*)ptr+64+ptr[15]*4;

  ix=clipx0; iy=clipy0;

  _copy=copy_buffer; copy_buffer=buffer;
  _vga_width=vga_width; vga_width=buffer_width;
  _vga_height=vga_height; vga_height=buffer_height;
  clipx0=0; clipx1-=ix-1;
  clipy0=0; clipy1-=iy-1;

  if (angle) {
    for (x=0;x<8;x+=2) { p[x]-=ix; p[x+1]-=iy; }
    sp_rotated_p(si,width,height,flags);
  } else if (size!=100) {
    x-=ix; y-=iy;
    sp_scaled(si,x,y,width,height,xg,yg,size,flags);
  } else {
    if (flags&1) xg=width-1-xg; x-=xg+ix;
    if (flags&2) yg=height-1-yg; y-=yg+iy;
    if (x>=clipx0 && x+width<=clipx1 && y>=clipy0 && y+height<=clipy1) // Draw sprite without clipping
      sp_normal(si,x,y,width,height,flags);
    else if (x<clipx1 && y<clipy1 && x+width>clipx0 && y+height>clipy0) // Draw clipped sprite
      sp_clipped(si,x,y,width,height,flags);
  }

  copy_buffer=_copy;
  vga_width=_vga_width;
  vga_height=_vga_height;
  clipx0=ix; clipx1+=ix-1;
  clipy0=iy; clipy1+=iy-1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sprite - rotated [scaled] [clipped] [mirrored] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_rotated_p(byte * si, int width, int height, int flags) {

  int h,hmin,hmax; // Minimum and maximum height
  int n,l0,l1;   // Side 0 and side 1 (indices of p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y,kk;
  byte * ptrcopy;

  memcpy(&p[8],&p[0],sizeof(p[0])*8); memcpy(&p[16],&p[0],sizeof(p[0])*8);

  hmin=32767; hmax=-32768; n=8; do {
    if (p[n+1]<hmin) { l0=n; hmin=p[n+1]; }
    if (p[n+1]>hmax) hmax=p[n+1];
    n+=2;
  } while (n<16);

  l1=l0; hmax0=hmin; hmax1=hmin; ptrcopy=copy_buffer+hmin*vga_width;

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

   if ((flags&3)==1 || (flags&3)==2) {
     kk=x0.l; x0.l=x1.l; x1.l=kk;
     kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
     kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    if (h<clipy1 && h>=clipy0 && x0.w[1]<clipx1 && x1.w[1]>=clipx0 && x1.w[1]>x0.w[1])
      sp_scan(ptrcopy+x0.w[1],x1.w[1]-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l);

    if ((flags&3)==1 || (flags&3)==2) {
      kk=x0.l; x0.l=x1.l; x1.l=kk;
      kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
      kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrcopy+=vga_width;

  } while (h++<hmax);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      External function to test graphics (for collisions)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags) {

  byte * si;
  int width,height; // Information about the graphic relative to the screen
  byte * _copy;
  int _vga_width,_vga_height;
  int ix,iy;

  width=ptr[13]; height=ptr[14]; si=(byte*)ptr+64+ptr[15]*4;

  ix=clipx0; iy=clipy0;

  _copy=copy_buffer; copy_buffer=buffer;
  _vga_width=vga_width; vga_width=buffer_width;
  _vga_height=vga_height; vga_height=buffer_height;
  clipx0=0; clipx1-=ix-1;
  clipy0=0; clipy1-=iy-1;

  if (angle) {
    for (x=0;x<8;x+=2) { p[x]-=ix; p[x+1]-=iy; }
    test_rotated(si,width,height,flags);
  } else if (size!=100) {
    x-=ix; y-=iy;
    test_scaled(si,x,y,width,height,xg,yg,size,flags);
  } else {
    if (flags&1) xg=width-1-xg; x-=xg+ix;
    if (flags&2) yg=height-1-yg; y-=yg+iy;
    if (x>=clipx0 && x+width<=clipx1 && y>=clipy0 && y+height<=clipy1) // Draw sprite without clipping
      test_normal(si,x,y,width,height,flags);
    else if (x<clipx1 && y<clipy1 && x+width>clipx0 && y+height>clipy0) // Draw clipped sprite
      test_clipped(si,x,y,width,height,flags);
  }

  copy_buffer=_copy;
  vga_width=_vga_width;
  vga_height=_vga_height;
  clipx0=ix; clipx1+=ix-1;
  clipy0=iy; clipy1+=iy-1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - [mirrored] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_normal(byte * p, int x, int y, int width, int height, int flags) {

  byte *q=copy_buffer+y*vga_width+x;
  int orig_width=width;

  switch (flags&3) {
    case 0: //--
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p++; q++;
        } while (--width);
        q+=vga_width-(width=orig_width);
      } while (--height); break;
    case 1: //h-
      p+=width-1;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p--; q++;
        } while (--width);
        q+=vga_width-(width=orig_width); p+=width*2;
      } while (--height); break;
    case 2: //-v
      p+=(height-1)*width;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p++; q++;
        } while (--width);
        q+=vga_width-(width=orig_width); p-=width*2;
      } while (--height); break;
    case 3: //hv
      p+=height*width-1;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p--; q++;
        } while (--width);
        q+=vga_width-(width=orig_width);
      } while (--height); break;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - clipped [mirrored] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_clipped(byte * p, int x, int y, int width, int height, int flags) {

  byte *q=copy_buffer+y*vga_width+x;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  if (x<clipx0) skip_x=clipx0-x; else skip_x=0;
  if (x+width>clipx1) rest_x=x+width-clipx1; else rest_x=0;
  long_x=width-skip_x-rest_x;

  if (y<clipy0) skip_y=clipy0-y; else skip_y=0;
  if (y+height>clipy1) rest_y=y+height-clipy1; else rest_y=0;
  long_y=height-skip_y-rest_y;

  switch (flags&3) {
    case 0: //--
      p+=width*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
      rest_x+=skip_x; width=long_x;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p++; q++;
        } while (--width);
        q+=vga_width-(width=long_x); p+=rest_x;
      } while (--long_y); break;
    case 1: //h-
      p+=width*skip_y+width-1-skip_x; q+=vga_width*skip_y+skip_x;
      rest_x+=skip_x; skip_x=long_x;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p--; q++;
        } while (--skip_x);
        q+=vga_width-(skip_x=long_x); p+=width+long_x;
      } while (--long_y); break;
    case 2: //-v
      p+=(height-1)*width-width*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
      rest_x+=skip_x; skip_x=long_x;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p++; q++;
        } while (--skip_x);
        q+=vga_width-(skip_x=long_x); p+=rest_x-width*2;
      } while (--long_y); break;
    case 3: //hv
      p+=height*width-1-width*skip_y-skip_x; q+=vga_width*skip_y+skip_x;
      rest_x+=skip_x; skip_x=long_x;
      do {
        do {
          if (*p && *q) { collides=1; return;}
          p--; q++;
        } while (--skip_x);
        q+=vga_width-(skip_x=long_x); p-=rest_x;
      } while (--long_y); break;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - scaled [clipped] [mirrored] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_scaled(byte * old_si, int x, int y, int width, int height, int xg, int yg,
                   int size, int flags) {

  int x0,y0,x1,y1; // Window occupied by the sprite in the copy plane
  int skip_x, long_x, rest_x; // Relative to the screen
  int skip_y, long_y, rest_y;
  int xr,ixr,yr,iyr,old_xr,old_width;
  byte *si,*di;

  if (flags&1) x0=x-((width-1-xg)*size)/100;
  else x0=x-(xg*size)/100;
  if (flags&2) y0=y-((height-1-yg)*size)/100;
  else y0=y-(yg*size)/100;
  x1=x0+(width*size)/100-1; y1=y0+(height*size)/100-1;

  if (x1<x0 || y1<y0) return;

  ixr=(float)(width*256)/(float)(x1-x0+1); iyr=(float)(height*256)/(float)(y1-y0+1);

  if (x1<clipx0 || y1<clipy0 || x0>=clipx1 || y0>=clipy1) return;

  di=copy_buffer+y0*vga_width+x0;

  if (x0<clipx0) skip_x=clipx0-x0; else skip_x=0;
  if (x1>=clipx1) rest_x=x1-clipx1+1; else rest_x=0;
  long_x=(width*size)/100-skip_x-rest_x;

  if (y0<clipy0) skip_y=clipy0-y0; else skip_y=0;
  if (y1>=clipy1) rest_y=y1-clipy1+1; else rest_y=0;
  long_y=(height*size)/100-skip_y-rest_y;

  if (flags&1) { xr=width*256-skip_x*ixr-1; ixr=-ixr; } else xr=skip_x*ixr;
  if (flags&2) { yr=height*256-skip_y*iyr-1; iyr=-iyr; } else yr=skip_y*iyr;

  old_xr=xr; old_width=width; di+=vga_width*skip_y+skip_x; width=long_x;

  do {
    si=old_si+(yr>>8)*old_width;
    do {
      if (*(si+(xr>>8)) && *di) { collides=1; return;}
      di++; xr+=ixr;
    } while (--width);
    yr+=iyr; xr=old_xr;
    di+=vga_width-(width=long_x);
  } while (--long_y);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - rotated [scaled] [clipped] [mirrored] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_rotated(byte * si, int width, int height, int flags) {

  int h,hmin,hmax; // Minimum and maximum height
  int n,l0,l1;     // Side 0 and side 1 (indices of p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y,kk;
  byte * ptrcopy;

  memcpy(&p[8],&p[0],sizeof(p[0])*8); memcpy(&p[16],&p[0],sizeof(p[0])*8);

  hmin=32767; hmax=-32768; n=8; do {
    if (p[n+1]<hmin) { l0=n; hmin=p[n+1]; }
    if (p[n+1]>hmax) hmax=p[n+1];
    n+=2;
  } while (n<16);

  l1=l0; hmax0=hmin; hmax1=hmin; ptrcopy=copy_buffer+hmin*vga_width;

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

   if ((flags&3)==1 || (flags&3)==2) {
     kk=x0.l; x0.l=x1.l; x1.l=kk;
     kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
     kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    if (h<clipy1 && h>=clipy0 && x0.w[1]<clipx1 && x1.w[1]>=clipx0 && x1.w[1]>x0.w[1])
    if (x0.w[1]<clipx0) if (x1.w[1]>=clipx1)
      test_scanc(ptrcopy+clipx0,x1.w[1]-x0.w[1],clipx1-clipx0-1,clipx0-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l);
    else test_scanc(ptrcopy+clipx0,x1.w[1]-x0.w[1],x1.w[1]-clipx0,clipx0-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l);
    else if (x1.w[1]>=clipx1)
      test_scanc(ptrcopy+x0.w[1],x1.w[1]-x0.w[1],clipx1-1-x0.w[1],0,si,width,g0x.l,g0y.l,g1x.l,g1y.l);
    else test_scan(ptrcopy+x0.w[1],x1.w[1]-x0.w[1],si,width,g0x.l,g0y.l,g1x.l,g1y.l);

    if ((flags&3)==1 || (flags&3)==2) {
      kk=x0.l; x0.l=x1.l; x1.l=kk;
      kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
      kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrcopy+=vga_width;

  } while (h++<hmax);

}

void test_scanc(byte * p,short n,short m,short o,byte * si,int width,int x0,int y0,int x1,int y1) {

  union { int l; short w[2]; } x,y;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  do {
    if (*p && *(si+x.w[1]+y.w[1]*width)) { collides=1; return;}
    p++; x.l+=x0; y.l+=y0;
  } while (m--);
}

void test_scan(byte * p,short n,byte * si,int width,int x0,int y0,int x1,int y1) {

  union { int l; short w[2]; } x,y;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  do {
    if (*p && *(si+x.w[1]+y.w[1]*width)) { collides=1; return;}
    p++; x.l+=x0; y.l+=y0;
  } while (n--);
}

