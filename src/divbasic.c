
///////////////////////////////////////////////////////////////////////////////
//      Module that contains the basic (graphics) primitives
///////////////////////////////////////////////////////////////////////////////

#include "global.h"

void drawing_help(int); // Drawing support


void draw_selection_box(int _x0,int _y0,int _x1,int _y1);
void draw_selection_mab(void);
void fill_inter(int w,int h);
void blit_inter(int w);
void fill_scan(word x,word y);
void fill_draw(void);
void copy_block(byte *d,byte *s,int w,int h);
void xchg_block(byte *d,byte *s,int w,int h);


///////////////////////////////////////////////////////////////////////////////
//      Global module variables
///////////////////////////////////////////////////////////////////////////////

word * m0, * m1;
word original_width;

extern int num_point;

///////////////////////////////////////////////////////////////////////////////
//      Functions to print text
///////////////////////////////////////////////////////////////////////////////

void writetxt(int x,int y,int center,byte * ptr) {
  wwrite(copy_buffer,vga_width/big2,vga_height,x,y,center,ptr,text_color);
}

///////////////////////////////////////////////////////////////////////////////
//      Draw a box 
///////////////////////////////////////////////////////////////////////////////

void box(byte c,int x0,int y0,int x1,int y1) {
  wbox(copy_buffer,vga_width,vga_height,c,x0,y0,x1-x0+1,y1-y0+1);
}

///////////////////////////////////////////////////////////////////////////////
//      Draw a rectangle
///////////////////////////////////////////////////////////////////////////////

void rectangle(byte c,int x,int y,int w,int h) {
  wrectangle(copy_buffer,vga_width,vga_height,c,x,y,w,h);
}

///////////////////////////////////////////////////////////////////////////////
//      Save ( flag = 0 ) or retrieve ( flag = 1) of a virtual box
///////////////////////////////////////////////////////////////////////////////

void save_restore(byte * p, int x, int y, int n, int flag) {
  byte *q;
  int w,h;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]);

  x-=*((word*)(graph[n]+4));
  y-=*((word*)(graph[n]+6));


	if(x>vga_width | y>vga_height)
		return;

  blit_partial(x,y,w,h);

  q=copy_buffer+y*vga_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>vga_width) rest_x=x+w-vga_width; else rest_x=0;
  long_x=w-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>vga_height) rest_y=y+h-vga_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=w*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;
  do {
    do {
      if (flag) *q=*p; else *p=*q; p++; q++;
    } while (--w);
    q+=vga_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

///////////////////////////////////////////////////////////////////////////////
//      Put a graphic
///////////////////////////////////////////////////////////////////////////////

void put(int x,int y,int n) {
  wput_in_box(copy_buffer,vga_width,vga_width,vga_height,x,y,n);
}

void put_bw(int x,int y,int n) { // Puts a contrasting graphic (mouse edition )
  int h,w;
  byte *p,*q;
  int skip_x, long_x, rest_x;
  int skip_y, long_y, rest_y;

  p=graph[n]+8;

  h=*((word*)(graph[n]+2));
  w=*((word*)graph[n]);

  x-=*((word*)(graph[n]+4));
  y-=*((word*)(graph[n]+6));

  if (x>=vga_width || y>=vga_height || x+w<=0 || y+h<=0) return;

  q=copy_buffer+y*vga_width+x;

  if (x<0) skip_x=-x; else skip_x=0;
  if (x+w>vga_width) rest_x=x+w-vga_width; else rest_x=0;
  long_x=w-skip_x-rest_x;

  if (y<0) skip_y=-y; else skip_y=0;
  if (y+h>vga_height) rest_y=y+h-vga_height; else rest_y=0;
  long_y=h-skip_y-rest_y;

  p+=w*skip_y+skip_x; q+=vga_width*skip_y+skip_x;
  rest_x+=skip_x; w=long_x;

  h=(int)c0*3; skip_x=dac[h]+dac[h+1]+dac[h+2];
  h=(int)c4*3; skip_x+=dac[h]+dac[h+1]+dac[h+2]; skip_x/=2;

  do {
    do {
      if (*p) {
        h=(int)(*q)*3;
        if (dac[h]+dac[h+1]+dac[h+2]>skip_x) *q=c0; else *q=c4;
      } p++; q++;
    } while (--w);
    q+=vga_width-(w=long_x); p+=rest_x;
  } while (--long_y);
}

///////////////////////////////////////////////////////////////////////////////
//      Swap two memory blocks ( without requiring a third party)
///////////////////////////////////////////////////////////////////////////////

void memxchg(byte *d, byte *s, int n) {
  int m,x;

  if (m=n/4) do {
    x=*(int*)s; *(int*)s=*(int*)d; *(int*)d=x;
    d+=4; s+=4;
  } while (--m);

  if (n&=3) do {
    x=*s; *s=*d; *d=x;
    d++; s++;
  } while (--n);
}

///////////////////////////////////////////////////////////////////////////////
//      MEGAROUTINE to restore the background of a box being edited
///////////////////////////////////////////////////////////////////////////////

static int zoom_portion=0;
int zoom_width,zoom_height;
byte *zoom_p,*zoom_q;

void edit_background(int x,int y,int w,int h) {

  int _x0,_y0,_x1,_y1;
  int _zoom_x,_zoom_y;
  int _zx,_zy,_zwidth,_zheight;
  int _big=big;

  if (x<0) { w+=x; x=0; }
  if (y<0) { h+=y; y=0; }
  if (x+w>vga_width) w=vga_width-x;
  if (y+h>vga_height) h=vga_height-y;
  if (w<=0 || h<=0) return;

  blit_partial(x,y,w,h);

  // Restore the background of the window system, NOTE *** improve update_box();

  if (zx || zy) if (x<zx || y<zy || x+w>zx+zwidth || y+h>zy+zheight) update_box(x,y,w,h);

  if (_big) { big=0; big2=1; }

  if (zy) {
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c2,zx-2-x,zy-2-y,zwidth+4,1);
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c0,zx-1-x,zy-1-y,zwidth+2,1);
  }
  if (zy+zheight<vga_height) {
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c2,zx-2-x,zy+zheight+1-y,zwidth+4,1);
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c0,zx-1-x,zy+zheight-y,zwidth+2,1);
  }

  if (zx) {
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c2,zx-2-x,zy-1-y,1,zheight+2);
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c0,zx-1-x,zy-y,1,zheight);
  }
  if (zx+zwidth<vga_width) {
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c2,zx+zwidth+1-x,zy-1-y,1,zheight+2);
    wbox_in_box(copy_buffer+y*vga_width+x,vga_width,w,h,c0,zx+zwidth-x,zy-y,1,zheight);
  }

  if (_big) { big=1; big2=2; }

  // Call zoom_map so it updates the corresponding box

  // 1 - Intersection between zoom and window in the copy

  _x0=(x>zx)?x:zx;
  _y0=(y>zy)?y:zy;
  _x1=(x+w<zx+zwidth)?x+w-1:zx+zwidth-1;
  _y1=(y+h<zy+zheight)?y+h-1:zy+zheight-1;

  if (_x1<_x0 || _y1<_y0) return;

  // 2 - Compute the first pixel and last pixel of the map

  _x0=((_x0-zx)>>zoom)+zoom_x;
  _y0=((_y0-zy)>>zoom)+zoom_y;
  _x1=((_x1-zx)>>zoom)+zoom_x;
  _y1=((_y1-zy)>>zoom)+zoom_y;

  zoom_p=map+_y0*map_width+_x0;
  zoom_q=copy_buffer+(zy+((_y0-zoom_y)<<zoom))*vga_width+zx+((_x0-zoom_x)<<zoom);
  zoom_width=_x1-_x0+1;
  zoom_height=_y1-_y0+1;

  _zoom_x=zoom_x; _zoom_y=zoom_y; _zx=zx; _zy=zy; _zwidth=zwidth; _zheight=zheight;
  zx=zx+((_x0-zoom_x)<<zoom); zy=zy+((_y0-zoom_y)<<zoom);
  zoom_x=_x0; zoom_y=_y0;
  zwidth=(_x1-_x0+1)<<zoom; zheight=(_y1-_y0+1)<<zoom;
  blit_partial(zx,zy,zwidth,zheight);
  zoom_portion=1; zoom_map(); zoom_portion=0;
  zoom_x=_zoom_x; zoom_y=_zoom_y; zx=_zx; zy=_zy; zwidth=_zwidth; zheight=_zheight;

}

//═════════════════════════════════════════════════════════════════════════════
//      Zoom, dump of map(map_width*map_height) to copy according to zoom,zoom_x/y
//═════════════════════════════════════════════════════════════════════════════

int w,h;

void zoom_map(void) {

  int32_t n,m,c;
  byte *p,*q;
  int _big=big;

  if (!zoom_portion) {

    p=map+zoom_y*map_width+zoom_x;
    q=copy_buffer;

    if ((map_width<<zoom)<vga_width) { // Copy of this chunk in select_zoom()
      zwidth=map_width<<zoom; zx=(vga_width-zwidth)/2; q+=zx; w=map_width;
    } else {
      zx=0; w=vga_width>>zoom; zwidth=w<<zoom;
    }

    if ((map_height<<zoom)<vga_height) {
      zheight=map_height<<zoom; zy=(vga_height-zheight)/2; q+=zy*vga_width; h=map_height;
    } else {
      zy=0; h=vga_height>>zoom; zheight=h<<zoom;
    }

    if (!zoom_background) {
      if (zx || zy) {
        update_background();
        blit_full=1;
      } else {
        if (_big) { big=0; big2=1; }
        if (zy+zheight<vga_height) {
          wbox(copy_buffer,vga_width,vga_height,c2,zx-2,zy+zheight+1,zwidth+4,1);
          wbox(copy_buffer,vga_width,vga_height,c0,zx-1,zy+zheight,zwidth+2,1);
          blit_partial(0,zy+zheight,vga_width,2); }
        if (zx+zwidth<vga_width) {
          wbox(copy_buffer,vga_width,vga_height,c2,zx-2,zy-2,zwidth+4,1);
          wbox(copy_buffer,vga_width,vga_height,c0,zx-1,zy-1,zwidth+2,1);
          blit_partial(zy+zwidth,0,2,vga_height); }
        if (_big) { big=1; big2=2; }
      }
      zoom_background=1;
    }

    if (zx || zy) blit_partial(zx,zy,zwidth,zheight); else blit_full=1;

  } else {
    p=zoom_p; q=zoom_q; w=zoom_width; h=zoom_height;
  }

  switch (zoom) {
  case 0:
    m=h; do {
      memcpy(q,p,w);
      p+=map_width;
      q+=vga_width;
    } while (--m);
    break;

  case 1:
    m=h; do {
      n=w; do {
        c=*p; c+=c*256;
        *(word*)q=c;
        *(word*)(q+vga_width)=c;
        p++; q+=2;
      } while (--n);
      p+=map_width-w;
      q+=vga_width*2-w*2;
    } while (--m);
    break;

  case 2:
    m=h; do {
      n=w; do {
        c=*p; c+=c*256; c+=c*65536;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=vga_width*3-4;
        p++;
      } while (--n);
      p+=map_width-w;
      q+=vga_width*4-w*4;
    } while (--m);
    break;

  case 3:
    m=h; do {
      n=w; do {
        c=*p; c+=c*256; c+=c*65536;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_width;
        *(int*)q=c; q-=4;
        *(int*)q=c; q-=vga_width*7-8;
        p++;
      } while (--n);
      p+=map_width-w;
      q+=vga_width*8-w*8;
    } while (--m);
    break;
  }

  if (!zoom_portion) cclock=(*system_clock)>>1;

  if (sel_status) switch(selection_mode) {
    case -1: for (n=0;n<512;n+=2) if (v.map->points[n]!=-1) {
      draw_selection_box(v.map->points[n],v.map->points[n+1],v.map->points[n],v.map->points[n+1]);
      if (num_point==n/2) {
        draw_selection_box(v.map->points[n]-1,v.map->points[n+1]-1,v.map->points[n]+1,v.map->points[n+1]+1);
      }
    } break;
    case 0: draw_selection_box(sel_x0,sel_y0,sel_x1,sel_y1); break;
    case 1: draw_selection_mab(); break;
    case 2: draw_selection_mab(); break;
    case 3: draw_selection_mab();
      if (sel_x1>=0) draw_selection_box(sel_x0,sel_y0,sel_x1,sel_y1);
      break;
    case 4: draw_selection_box(sel_x0,sel_y0,sel_x1,sel_y1); break;
    case 5: draw_selection_mab(); break;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Interpolative fill sub-bar
//═════════════════════════════════════════════════════════════════════════════

void inter_mode(void) {

  int w,h;

  w=mab_x1-mab_x0+1+4; h=mab_y1-mab_y0+1+4;

  if ((m0=(word*)malloc(w*h*2))!=NULL)
  if ((m1=(word*)malloc(w*h*2))!=NULL) {

    make_near_rule();

    bar[0]=101+zoom; bar[1]=120; bar[2]=154; bar[3]=0;
    draw_bar(0); put_bar(10,2,118); do_zoom=1;

    memset(m0,0,w*h*2);
    memset(m1,0,w*h*2);

    do {
      drawing_help(1295);
      read_mouse(); select_zoom(); test_mouse();

      if (((mouse_b&1) && selected_icon==2)) {
        fill_inter(w,h); do_zoom=1;
      } else if ((mouse_b&1) && selected_icon==1) {
        fill_inter(w,h); do_zoom=1;
        do { read_mouse(); } while (mouse_b&1);
      }

      blit_inter(w);

      blit_edit();
    } while (!(mouse_b&2) && !key(_ESC) && mode<100 &&
      !(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)));

    blit_inter(w);

    if (key(_ESC)||(mouse_b && mouse_in(bar_x,bar_y+10,bar_x+9,bar_y+18)))
      { put_bar(2,10,45); blit_bars(0);
        put(mouse_x,mouse_y,mouse_graph); blit_screen(copy_buffer); }

    free(m1); free(m0);
  } else free(m0);

  if (m0==NULL || m1==NULL) {
    v_text=(char *)texts[45]; dialog(err0);
  }

  mode+=100;
}

//═════════════════════════════════════════════════════════════════════════════
//      Interpolation
//═════════════════════════════════════════════════════════════════════════════

void fill_inter(int w,int h) { // Fill an area with averages

  int x,y;
  word *si,*di,n0,n1,n2,n3;
  int n;

  // 1 - Shift m0 one pixel right and down

  for (y=h-1;y>0;y--) for (x=w-1;x>0;x--) *(m0+y*w+x)=*(m0+(y-1)*w+x-1);
  for (x=0;x<w;x++) *(m0+x)=0; for (y=0;y<h;y++) *(m0+y*w)=0;

  // 2 - Refresh in m0 the wire of points surrounding the area

  for (y=mab_y0-1;y<=mab_y1+1;y++) if (y>=0 && y<map_height)
    for (x=mab_x0-1;x<=mab_x1+1;x++) if (x>=0 && x<map_width)
      if (is_near_mab(x,y)) {
        n=*(original+(y-mab_y0+1)*(original_width)+(x-mab_x0+1));
        n=(dac[n*3]+dac[n*3+1]+dac[n*3+2])*256+256;
        *(m0+(y-mab_y0+2)*w+(x-mab_x0+2))=n;
      } else if (!is_mab(x,y)) {
        *(m0+(y-mab_y0+2)*w+(x-mab_x0+2))=0;
      }

  // Interpolate from m0 to m1

  si=m0; di=m1;

  for (y=0;y<h-1;y++) {
    for (x=0;x<w-1;x++) {

        if (n0=*si) // Put into *(di) the average of the four *(si+?)
             if (n1=*(si+1))
                  if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n0+n1+n2+n3)>>2;
                       else *di=(n0+n1+n2)/3;
                  else if (n3=*(si+w+1))
                            *di=(n0+n1+n3)/3;
                       else *di=(n0+n1)>>1;
             else if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n0+n2+n3)/3;
                       else *di=(n0+n2)>>1;
                  else if (n3=*(si+w+1))
                            *di=(n0+n3)>>1;
                       else *di=n0;
        else if (n1=*(si+1))
                  if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n1+n2+n3)/3;
                       else *di=(n1+n2)>>1;
                  else if (n3=*(si+w+1))
                            *di=(n1+n3)>>1;
                       else *di=n1;
             else if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n2+n3)>>1;
                       else *di=n2;
                  else if (n3=*(si+w+1))
                            *di=n3;
                       else *di=0;
      si++; di++;
    } si++; di++;
  }

  // And now interpolate from m1 to m0

  si=m1; di=m0;

  for (y=0;y<h-1;y++) {
    for (x=0;x<w-1;x++) {

        if (n0=*si) // Put into *(di) the average of the four *(si+?)
             if (n1=*(si+1))
                  if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n0+n1+n2+n3)>>2;
                       else *di=(n0+n1+n2)/3;
                  else if (n3=*(si+w+1))
                            *di=(n0+n1+n3)/3;
                       else *di=(n0+n1)>>1;
             else if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n0+n2+n3)/3;
                       else *di=(n0+n2)>>1;
                  else if (n3=*(si+w+1))
                            *di=(n0+n3)>>1;
                       else *di=n0;
        else if (n1=*(si+1))
                  if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n1+n2+n3)/3;
                       else *di=(n1+n2)>>1;
                  else if (n3=*(si+w+1))
                            *di=(n1+n3)>>1;
                       else *di=n1;
             else if (n2=*(si+w))
                       if (n3=*(si+w+1))
                            *di=(n2+n3)>>1;
                       else *di=n2;
                  else if (n3=*(si+w+1))
                            *di=n3;
                       else *di=0;

      si++; di++;
    } si++; di++;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Select a block via a fill (x,y,fill_dac)
//═════════════════════════════════════════════════════════════════════════════

void fill_select(word x,word y) {

  int _fill_mode;

  _fill_mode=fill_mode; fill_mode=1;
  if ((fss=(word*)malloc(60000))!=NULL) {
    fsp=fss; fsp_max=fss+30000; fill_scan(x,y); free(fss);
  } else {
    v_text=(char *)texts[45]; dialog(err0);
  }
  fill_mode=_fill_mode;
}

//═════════════════════════════════════════════════════════════════════════════
//      Fill an area (containing fill_dac colors) with a color
//═════════════════════════════════════════════════════════════════════════════

void fill(word x,word y) {

  int a,b,c,d;

  if ((fss=(word*)malloc(60000))!=NULL) {

    fsp=fss; fsp_max=fss+30000;

    memset(mab,0,((map_width*map_height+31)/32)*4);

    if (fill_mode==2) {
      memset(fill_dac,1,256); // Fill up to the selected color
      fill_dac[color]=0;
    } else {
      memset(fill_dac,0,256); // Fill the color at map(x,y)
      fill_dac[*(map+y*map_width+x)]=1;
    }

    mab_x0=map_width; mab_y0=map_height; mab_x1=0; mab_y1=0;

    fill_scan(x,y);

    if (fill_mode==3) {
      if (mab_x0) a=mab_x0-1; else a=mab_x0;
      if (mab_y0) b=mab_y0-1; else b=mab_y0;
      if (mab_x1<map_width-1) c=mab_x1+1; else c=mab_x1;
      if (mab_y1<map_height-1) d=mab_y1+1; else d=mab_y1;
      c=c-a+1; d=d-b+1; original_width=c;
      if ((original=(byte*)save_undo(a,b,c,d))!=NULL) {
        if (!mab_x0) original--;
        if (!mab_y0) original-=original_width;
        inter_mode();
      }
    } else {
      if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1))
        fill_draw();
    }

    free(fss);
  } else {
    v_text=(char *)texts[45]; dialog(err0);
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Fill algorithm (by scans)
//═════════════════════════════════════════════════════════════════════════════

void fill_scan(word x,word y) {
  int x0,x1;

  new_scan:

  x0=x; do x0--; while (x0>=0 && fill_dac[*(map+y*map_width+x0)]); x0++;
  x1=x; do x1++; while (x1<map_width && fill_dac[*(map+y*map_width+x1)]); x1--;

  if (x0<mab_x0) mab_x0=x0; if (x1>mab_x1) mab_x1=x1;
  if (y<mab_y0) mab_y0=y; if (y>mab_y1) mab_y1=y;

  for (x=x0;x<=x1;x++) set_mab(x,y);

  if (fill_mode==1) { if (x0>0) x0--; if (x1<map_width-1) x1++; }

  x=x0; continue_scan_0:

  if (y>0) while (x<=x1) {
    if (!is_mab(x,y-1)) // If it is not painted
      if (fill_dac[*(map+(y-1)*map_width+x)]) // If it has to be painted
        if (fsp<fsp_max) { // If it can be painted
          *fsp=x0; fsp++; *fsp=x1; fsp++;
          *fsp=x+2; fsp++; *fsp=y; fsp++;
          *fsp=0; fsp++; y--;
          goto new_scan;
        }
    x++;
  }

  x=x0; continue_scan_1:

  if (y<map_height-1) while (x<=x1) {
    if (!is_mab(x,y+1)) // If it is not painted
      if (fill_dac[*(map+(y+1)*map_width+x)]) // If it has to be painted
        if (fsp<fsp_max) { // If it can be painted
          *fsp=x0; fsp++; *fsp=x1; fsp++;
          *fsp=x+2; fsp++; *fsp=y; fsp++;
          *fsp=1; fsp++; y++;
          goto new_scan;
        }
    x++;
  }

  if (fsp>fss) { // While there are unfinished scans
    fsp-=2; y=*fsp; fsp--; x=*fsp;
    fsp--; x1=*fsp; fsp--; x0=*fsp;
    if (*(fsp+4)) goto continue_scan_1; else goto continue_scan_0;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Fill of the fill
//═════════════════════════════════════════════════════════════════════════════

extern byte * texture_color;
byte get_color(int x,int y);

void fill_draw(void) {
  int x,y;

  if (texture_color) {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+y*map_width+x)=get_color(x,y);
  } else {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+y*map_width+x)=color;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Dump of the interpolated area
//═════════════════════════════════════════════════════════════════════════════

void blit_inter(int w) { // Dump the interpolated area to the screen

  int x,y,n;

      for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) {
          n=*(m0+(y-mab_y0+1)*w+(x-mab_x0+1)); if (n>256) n-=256; else n=0;
          *(map+y*map_width+x)=near_rule[n/256]; }
}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a bitmap selection (according to zoom,zoom_x/y,mab_*)
//═════════════════════════════════════════════════════════════════════════════

void draw_selection_mab(void) {

  int x0,y0,x1,y1; // Intersection of the selected area and what is seen in zoom
  int x,y,c;
  byte *p,g0=c0,g4=c4;
  word g04,g40;
  int inc32,*_mab;

  x0=zoom_x; if (mab_x0>x0) x0=mab_x0;
  y0=zoom_y; if (mab_y0>y0) y0=mab_y0;
  x1=zoom_x+(zwidth>>zoom)-1; if (mab_x1<x1) x1=mab_x1;
  y1=zoom_y+(zheight>>zoom)-1; if (mab_y1<y1) y1=mab_y1;

  // NOTE !!!
  // p=copy_buffer+(zy+((y0-zoom_y)<<zoom))*vga_width+zx+((x0-zoom_x)<<zoom);

  if (x1>=x0 && y1>=y0) switch(zoom) {

    case 0: for (y=y0;y<=y1;y++) {
      inc32=y*map_width+x0; _mab=mab+(inc32>>5); inc32&=31;
      p=copy_buffer+(zy+y-zoom_y)*vga_width+(zx+x0-zoom_x); c=(cclock^y^x0)&1;
      for (x=x0;x<=x1;x++,p++,c^=1) {
        if (*_mab) if ((*_mab)&(1<<inc32))
            if (y==0 || x==0 || y==map_height-1 || x==map_width-1)  {
              if (c) *p=g0; else *p=g4;
            } else if (!is_mab(x,y-1) || !is_mab(x-1,y) || !is_mab(x,y+1) || !is_mab(x+1,y)) {
              if (c) *p=g0; else *p=g4;
            }
        if (!(inc32=(inc32+1)&31)) _mab++;
      }
    } break;

    case 1: if (cclock&1) { g0=c4; g4=c0; }
    g04=g0+g4*256; g40=g4+g0*256;
    for (y=y0;y<=y1;y++) {
      inc32=y*map_width+x0; _mab=mab+(inc32>>5); inc32&=31;
      p=copy_buffer+(zy+(y-zoom_y)*2)*vga_width+zx+(x0-zoom_x)*2;
      for (x=x0;x<=x1;x++,p+=2) {
        if (*_mab) if ((*_mab)&(1<<inc32)) {
          if (y==0 || (y!=0 && !is_mab(x,y-1))) {
            *(word*)p=g04;
            if (y==map_height-1 || (y<map_height-1 && !is_mab(x,y+1))) {
              *(word*)(p+vga_width)=g40;
            } else {
              if (x==0 || (x!=0 && !is_mab(x-1,y))) {
                *(p+vga_width)=g4;
              }
              if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
                *(p+vga_width+1)=g0;
              }
            }
          } else if (x==0 || (x!=0 && !is_mab(x-1,y))) {
            *p=g0;
            *(p+vga_width)=g4;
            if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
              *(p+1)=g4;
              *(p+vga_width+1)=g0;
            } else if (y==map_height-1 || (y<map_height-1 && !is_mab(x,y+1))) {
              *(p+vga_width+1)=g0;
            }
          } else if (y==map_height-1 || (y<map_height-1 && !is_mab(x,y+1))) {
            *(word*)(p+vga_width)=g40;
            if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
              *(p+1)=g4;
            }
          } else if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
             *(p+1)=g4;
             *(p+vga_width+1)=g0;
          }
        }
        if (!(inc32=(inc32+1)&31)) _mab++;
      }
    } break;

    case 2: if (cclock&1) { g0=c4; g4=c0; }
    for (y=y0;y<=y1;y++) {
      p=copy_buffer+(zy+(y-zoom_y)*4)*vga_width+zx+(x0-zoom_x)*4;
      for (x=x0;x<=x1;x++,p+=4) if (is_mab(x,y)) {
        if (y==0 || (y!=0 && !is_mab(x,y-1))) {
          *p=g0; *(p+1)=g4;
          *(p+2)=g0; *(p+3)=g4; }
        if (x==0 || (x!=0 && !is_mab(x-1,y))) {
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p-=vga_width*3; }
        if (y==map_height-1 || (y<map_height-1 && !is_mab(x,y+1))) {
          p+=vga_width*3;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p-=vga_width*3+3; }
        if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
          p+=3;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p-=vga_width*3+3; }
      }
    } break;

    case 3: if (cclock&1) { g0=c4; g4=c0; }
    for (y=y0;y<=y1;y++) {
      p=copy_buffer+(zy+(y-zoom_y)*8)*vga_width+zx+(x0-zoom_x)*8;
      for (x=x0;x<=x1;x++,p+=8) if (is_mab(x,y)) {
        if (y==0 || (y!=0 && !is_mab(x,y-1))) {
          *p=g0; *(p+1)=g4;
          *(p+2)=g0; *(p+3)=g4;
          *(p+4)=g0; *(p+5)=g4;
          *(p+6)=g0; *(p+7)=g4; }
        if (x==0 || (x!=0 && !is_mab(x-1,y))) {
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p-=vga_width*7; }
        if (y==map_height-1 || (y<map_height-1 && !is_mab(x,y+1))) {
          p+=vga_width*7;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p-=vga_width*7+7; }
        if (x==map_width-1 || (x<map_width-1 && !is_mab(x+1,y))) {
          p+=7;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p+=vga_width;
          *p=g4; p+=vga_width;
          *p=g0; p-=vga_width*7+7; }
      }
    } break;
  }

}

//═════════════════════════════════════════════════════════════════════════════
//      Draw a selection box (according to zoom,zoom_x/y)
//═════════════════════════════════════════════════════════════════════════════

void draw_selection_box(int _x0,int _y0,int _x1,int _y1) {

  int x0,y0,x1,y1,n;
  byte *p;

  if (_x0>_x1) swap(_x0,_x1); if (_y0>_y1) swap(_y0,_y1);

  x0=zx+(_x0<<zoom)-(zoom_x<<zoom);
  y0=zy+(_y0<<zoom)-(zoom_y<<zoom);
  x1=zx+(_x1<<zoom)-(zoom_x<<zoom)+(1<<zoom)-1;
  y1=zy+(_y1<<zoom)-(zoom_y<<zoom)+(1<<zoom)-1;

  if (x1>=zx && x0<zx+zwidth && y1>=zy && y0<zy+zheight) {

    if (y0>=zy) {
      if (x0<zx) x0=zx; if (x1>=zx+zwidth) x1=zx+zwidth-1;
      n=x1-x0+1;
      p=copy_buffer+vga_width*y0+x0;
      if ((y0^cclock)&1) do {
        if ((memptrsize)p&1) *p=c0; else *p=c4; p++;
      } while (--n);
      else do {
        if ((memptrsize)p&1) *p=c4; else *p=c0; p++;
      } while (--n);
    }

    if (y1<zy+zheight) {
      if (x0<zx) x0=zx; if (x1>=zx+zwidth) x1=zx+zwidth-1;
      n=x1-x0+1;
      p=copy_buffer+vga_width*y1+x0;
      if ((y1^cclock)&1) do {
        if ((memptrsize)p&1) *p=c0; else *p=c4; p++;
      } while (--n);
      else do {
        if ((memptrsize)p&1) *p=c4; else *p=c0; p++;
      } while (--n);
    }

    x0=zx+(_x0<<zoom)-(zoom_x<<zoom);
    x1=zx+(_x1<<zoom)-(zoom_x<<zoom)+(1<<zoom)-1;

    if (x0>=zx) {
      if (y0<zy) y0=zy; if (y1>=zy+zheight) y1=zy+zheight-1;
      n=y1-y0+1;
      p=copy_buffer+vga_width*y0+x0;
      if ((x0^cclock)&1) do {
        if (y0&1) *p=c0; else *p=c4; p+=vga_width; y0++;
      } while (--n);
      else do {
        if (y0&1) *p=c4; else *p=c0; p+=vga_width; y0++;
      } while (--n);
      y0=zy+(_y0<<zoom)-(zoom_y<<zoom);
    }

    if (x1<zx+zwidth) {
      if (y0<zy) y0=zy; if (y1>=zy+zheight) y1=zy+zheight-1;
      n=y1-y0+1;
      p=copy_buffer+vga_width*y0+x1;
      if ((x1^cclock)&1) do {
        if (y0&1) *p=c0; else *p=c4; p+=vga_width; y0++;
      } while (--n);
      else do {
        if (y0&1) *p=c4; else *p=c0; p+=vga_width; y0++;
      } while (--n);
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Bitmap functions, set a pixel
//═════════════════════════════════════════════════════════════════════════════

void set_mab(int x,int y) { // Set a pixel in the bitmap

  x+=y*map_width; // Bit number in the buffer
  y=x>>5; // Int number in the buffer
  x&=31; // Bit number in the int
  *(mab+y)|=1<<x;
}

//═════════════════════════════════════════════════════════════════════════════
//      Bitmap functions, query a pixel
//═════════════════════════════════════════════════════════════════════════════

int is_mab(int x,int y) { // Query a pixel in the bitmap

  x+=y*map_width; // Bit number in the buffer
  y=x>>5; // Int number in the buffer
  x&=31; // Bit number in the int
  return(*(mab+y)&(1<<x));
}

int is_near_mab(int x,int y) {

  int c;

  if (x) c=is_mab(x-1,y); else c=0;
  if (x<map_width-1) c|=is_mab(x+1,y);
  if (y) c|=is_mab(x,y-1);
  if (y<map_height-1) c|=is_mab(x,y+1);

  return(c&& !is_mab(x,y));
}

//═════════════════════════════════════════════════════════════════════════════
//      UNDO - Save the area occupied by an action, before performing it of course
//═════════════════════════════════════════════════════════════════════════════

byte * save_undo(int x, int y, int w, int h) {

  int a,start,end; // Start and end of the saved block (in undo[])
  byte *ret=0;

  if (x<0) {w+=x; x=0; } if (y<0) {h+=y; y=0; }
  if (x+w>map_width) w=map_width-x; if (y+h>map_height) h=map_height-y;
  if (w<=0 || h<=0) return((byte *)-1);

  // Determine the zoom area to refresh

  if (x<zoom_x) do_zoom_x=zx-((zoom_x-x)<<zoom);
  else do_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) do_zoom_y=zy+((y-zoom_y)<<zoom);
  else do_zoom_y=zy+((y-zoom_y)<<zoom);
  do_zoom_width=w<<zoom; do_zoom_height=h<<zoom;

  if (do_zoom_x+do_zoom_width<=zx || do_zoom_y+do_zoom_height<=zy ||
      do_zoom_x>=zx+zwidth || do_zoom_y>=zy+zheight) {
    do_zoom_width=0; do_zoom_height=0;
  } else {
    if (do_zoom_x<zx) { do_zoom_width-=zx-do_zoom_x; do_zoom_x=zx; }
    if (do_zoom_y<zy) { do_zoom_height-=zy-do_zoom_y; do_zoom_y=zy; }
    if (do_zoom_x+do_zoom_width>zx+zwidth) do_zoom_width=zx+zwidth-do_zoom_x;
    if (do_zoom_y+do_zoom_height>zy+zheight) do_zoom_height=zy+zheight-do_zoom_y;
    if (!do_zoom) do_zoom=-1;
  }

  if (iundo) a=iundo-1; else a=max_undos-1;

  start=0; if (tundo[a].mode!=-1) start=tundo[a].end;

  if (start+w*h>undo_memory) start=0;

  // If an action takes more than undo_memory, then we do not save it.

  if ((end=start+w*h)<=undo_memory) {

    tundo[iundo].start=start; tundo[iundo].end=end;

    tundo[iundo].x=x; tundo[iundo].y=y;
    tundo[iundo].w=w; tundo[iundo].h=h;

    tundo[iundo].mode=mode;

    // We discard the overwritten blocks

    for (a=0;a<max_undos;a++)
      if (a!=iundo && tundo[a].mode!=-1)
        if ((tundo[a].start<start && tundo[a].end>start)||
           (tundo[a].start>=start && tundo[a].start<end))
           tundo[a].mode=-1;

    // We save the area that the performed action will occupy.

    copy_block(undo+start,map+x+y*map_width,w,h);

    tundo[iundo].code=map_code;

    iundo=(iundo+1)%max_undos; tundo[iundo].mode=-1; // undo_next() is forbidden

    ret=(byte *)(undo+start);

  } else {
    edit_background(0,0,vga_width,vga_height);
    blit_bars(1);
    blit_full=1; blit_screen(copy_buffer);
    v_text=(char *)texts[320]; dialog(err0); undo_error=1;
  }

  return(ret);
}

//═════════════════════════════════════════════════════════════════════════════
//      UNDO - Restore the area occupied by the last action.
//═════════════════════════════════════════════════════════════════════════════

int undo_back(void) {
  int x,y,w,h;

  if (iundo) a=iundo-1; else a=max_undos-1;

  if (tundo[a].mode!=-1 && tundo[a].code==map_code) {

    iundo=a; retrace();

    x=tundo[a].x; y=tundo[a].y;
    w=tundo[a].w; h=tundo[a].h;

    xchg_block(undo+tundo[a].start,map+x+y*map_width,w,h);

    // Determine the zoom area to refresh

    if (x<zoom_x) do_zoom_x=zx-((zoom_x-x)<<zoom);
    else do_zoom_x=zx+((x-zoom_x)<<zoom);
    if (y<zoom_y) do_zoom_y=zy+((y-zoom_y)<<zoom);
    else do_zoom_y=zy+((y-zoom_y)<<zoom);
    do_zoom_width=w<<zoom; do_zoom_height=h<<zoom;

    if (do_zoom_x+do_zoom_width<=zx || do_zoom_y+do_zoom_height<=zy ||
        do_zoom_x>=zx+zwidth || do_zoom_y>=zy+zheight) {
      do_zoom_width=0; do_zoom_height=0;
    } else {
      if (do_zoom_x<zx) { do_zoom_width-=zx-do_zoom_x; do_zoom_x=zx; }
      if (do_zoom_y<zy) { do_zoom_height-=zy-do_zoom_y; do_zoom_y=zy; }
      if (do_zoom_x+do_zoom_width>zx+zwidth) do_zoom_width=zx+zwidth-do_zoom_x;
      if (do_zoom_y+do_zoom_height>zy+zheight) do_zoom_height=zy+zheight-do_zoom_y;
      if (!do_zoom) do_zoom=-1;
    } return(1);
  } else return(0);
}

//═════════════════════════════════════════════════════════════════════════════
//      UNDO - Redo the last action undone with undo_back()
//═════════════════════════════════════════════════════════════════════════════

void undo_next(void) {
  int x,y,w,h;

  if (tundo[iundo].mode!=-1 && tundo[iundo].code==map_code) {

    retrace();

    x=tundo[iundo].x; y=tundo[iundo].y;
    w=tundo[iundo].w; h=tundo[iundo].h;

    xchg_block(undo+tundo[iundo].start,map+x+y*map_width,w,h);

    // Determine the zoom area to refresh

    if (x<zoom_x) do_zoom_x=zx-((zoom_x-x)<<zoom);
    else do_zoom_x=zx+((x-zoom_x)<<zoom);
    if (y<zoom_y) do_zoom_y=zy+((y-zoom_y)<<zoom);
    else do_zoom_y=zy+((y-zoom_y)<<zoom);
    do_zoom_width=w<<zoom; do_zoom_height=h<<zoom;

    if (do_zoom_x+do_zoom_width<=zx || do_zoom_y+do_zoom_height<=zy ||
        do_zoom_x>=zx+zwidth || do_zoom_y>=zy+zheight) {
      do_zoom_width=0; do_zoom_height=0;
    } else {
      if (do_zoom_x<zx) { do_zoom_width-=zx-do_zoom_x; do_zoom_x=zx; }
      if (do_zoom_y<zy) { do_zoom_height-=zy-do_zoom_y; do_zoom_y=zy; }
      if (do_zoom_x+do_zoom_width>zx+zwidth) do_zoom_width=zx+zwidth-do_zoom_x;
      if (do_zoom_y+do_zoom_height>zy+zheight) do_zoom_height=zy+zheight-do_zoom_y;
      if (!do_zoom) do_zoom=-1;
    }

    iundo=(iundo+1)%max_undos;
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Copy a block of the map to memory
//═════════════════════════════════════════════════════════════════════════════

void copy_block(byte *d,byte *s,int w,int h) {

  do {
    memcpy(d,s,w);
    d+=w; s+=map_width;
  } while (--h);
}

//═════════════════════════════════════════════════════════════════════════════
//      Swap a block between the map and memory
//═════════════════════════════════════════════════════════════════════════════

void xchg_block(byte *d,byte *s,int w,int h) {

  do {
    memxchg(d,s,w);
    d+=w; s+=map_width;
  } while (--h);
}

