///////////////////////////////////////////////////////////////////////////////
//      Module that contains the basic (graphics) primitives
///////////////////////////////////////////////////////////////////////////////

#include "global.h"

void ayuda_dibujo(int); // Drawing support


void draw_selection_box(int _x0,int _y0,int _x1,int _y1);
void draw_selection_mab(void);
void fill_inter(int an,int al);
void volcado_inter(int an);
void fill_scan(word x,word y);
void fill_draw(void);
void copy_block(byte *d,byte *s,int an,int al);
void xchg_block(byte *d,byte *s,int an,int al);


///////////////////////////////////////////////////////////////////////////////
//      Global module variables
///////////////////////////////////////////////////////////////////////////////

word * m0, * m1;
word an_original;

extern int num_punto;

///////////////////////////////////////////////////////////////////////////////
//      Functions to print text
///////////////////////////////////////////////////////////////////////////////

void writetxt(int x,int y,int centro,byte * ptr) {
  wwrite(copia,vga_an/big2,vga_al,x,y,centro,ptr,text_color);
}

///////////////////////////////////////////////////////////////////////////////
//      Draw a box 
///////////////////////////////////////////////////////////////////////////////

void box(byte c,int x0,int y0,int x1,int y1) {
  wbox(copia,vga_an,vga_al,c,x0,y0,x1-x0+1,y1-y0+1);
}

///////////////////////////////////////////////////////////////////////////////
//      Draw a rectangle
///////////////////////////////////////////////////////////////////////////////

void rectangulo(byte c,int x,int y,int an,int al) {
  wrectangulo(copia,vga_an,vga_al,c,x,y,an,al);
}

///////////////////////////////////////////////////////////////////////////////
//      Save ( flag = 0 ) or retrieve ( flag = 1) of a virtual box
///////////////////////////////////////////////////////////////////////////////

void salvaguarda(byte * p, int x, int y, int n, int flag) {
  byte *q;
  int an,al;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  volcado_parcial(x,y,an,al);

  q=copia+y*vga_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>vga_an) resto_x=x+an-vga_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>vga_al) resto_y=y+al-vga_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;
  do {
    do {
      if (flag) *q=*p; else *p=*q; p++; q++;
    } while (--an);
    q+=vga_an-(an=long_x); p+=resto_x;
  } while (--long_y);
}

///////////////////////////////////////////////////////////////////////////////
//      Put a graphic
///////////////////////////////////////////////////////////////////////////////

void put(int x,int y,int n) {
//	printf("put (%d %d) %d %d %d\n",vga_an, vga_al, x,y,n);
  wput_in_box(copia,vga_an,vga_an,vga_al,x,y,n);
}

void put_bw(int x,int y,int n) { // Puts a contrasting graphic (mouse edition )
  int al,an;
  byte *p,*q;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  p=graf[n]+8;

  al=*((word*)(graf[n]+2));
  an=*((word*)graf[n]);

  x-=*((word*)(graf[n]+4));
  y-=*((word*)(graf[n]+6));

  if (x>=vga_an || y>=vga_al || x+an<=0 || y+al<=0) return;

  q=copia+y*vga_an+x;

  if (x<0) salta_x=-x; else salta_x=0;
  if (x+an>vga_an) resto_x=x+an-vga_an; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<0) salta_y=-y; else salta_y=0;
  if (y+al>vga_al) resto_y=y+al-vga_al; else resto_y=0;
  long_y=al-salta_y-resto_y;

  p+=an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
  resto_x+=salta_x; an=long_x;

  al=(int)c0*3; salta_x=dac[al]+dac[al+1]+dac[al+2];
  al=(int)c4*3; salta_x+=dac[al]+dac[al+1]+dac[al+2]; salta_x/=2;

  do {
    do {
      if (*p) {
        al=(int)(*q)*3;
        if (dac[al]+dac[al+1]+dac[al+2]>salta_x) *q=c0; else *q=c4;
      } p++; q++;
    } while (--an);
    q+=vga_an-(an=long_x); p+=resto_x;
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
//      MEGARUTINA para reponer el fondo de una caja en ediciขn
///////////////////////////////////////////////////////////////////////////////

int zoom_porcion=0;
int zoom_an,zoom_al;
byte *zoom_p,*zoom_q;

void fondo_edicion(int x,int y,int an,int al) {

  int _x0,_y0,_x1,_y1;
  int _zoom_x,_zoom_y;
  int _zx,_zy,_zan,_zal;
  int _big=big;

  if (x<0) { an+=x; x=0; }
  if (y<0) { al+=y; y=0; }
  if (x+an>vga_an) an=vga_an-x;
  if (y+al>vga_al) al=vga_al-y;
  if (an<=0 || al<=0) return;

  volcado_parcial(x,y,an,al);

  // Repone el fondo del sistema de ventanas, OJO *** mejorar actualiza_caja();

  if (zx || zy) if (x<zx || y<zy || x+an>zx+zan || y+al>zy+zal) actualiza_caja(x,y,an,al);

  if (_big) { big=0; big2=1; }

  if (zy) {
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c2,zx-2-x,zy-2-y,zan+4,1);
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c0,zx-1-x,zy-1-y,zan+2,1);
  }
  if (zy+zal<vga_al) {
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c2,zx-2-x,zy+zal+1-y,zan+4,1);
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c0,zx-1-x,zy+zal-y,zan+2,1);
  }

  if (zx) {
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c2,zx-2-x,zy-1-y,1,zal+2);
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c0,zx-1-x,zy-y,1,zal);
  }
  if (zx+zan<vga_an) {
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c2,zx+zan+1-x,zy-1-y,1,zal+2);
    wbox_in_box(copia+y*vga_an+x,vga_an,an,al,c0,zx+zan-x,zy-y,1,zal);
  }

  if (_big) { big=1; big2=2; }

  // Llama a zoom_map para que actualice la caja correspondiente

  // 1 - Intersecciขn entre zoom y ventana en la copia

  _x0=(x>zx)?x:zx;
  _y0=(y>zy)?y:zy;
  _x1=(x+an<zx+zan)?x+an-1:zx+zan-1;
  _y1=(y+al<zy+zal)?y+al-1:zy+zal-1;

  if (_x1<_x0 || _y1<_y0) return;

  // 2 - Calcular el pimer pixel y ultimo pixel del mapa

  _x0=((_x0-zx)>>zoom)+zoom_x;
  _y0=((_y0-zy)>>zoom)+zoom_y;
  _x1=((_x1-zx)>>zoom)+zoom_x;
  _y1=((_y1-zy)>>zoom)+zoom_y;

  zoom_p=map+_y0*map_an+_x0;
  zoom_q=copia+(zy+((_y0-zoom_y)<<zoom))*vga_an+zx+((_x0-zoom_x)<<zoom);
  zoom_an=_x1-_x0+1;
  zoom_al=_y1-_y0+1;

  _zoom_x=zoom_x; _zoom_y=zoom_y; _zx=zx; _zy=zy; _zan=zan; _zal=zal;
  zx=zx+((_x0-zoom_x)<<zoom); zy=zy+((_y0-zoom_y)<<zoom);
  zoom_x=_x0; zoom_y=_y0;
  zan=(_x1-_x0+1)<<zoom; zal=(_y1-_y0+1)<<zoom;
  volcado_parcial(zx,zy,zan,zal);
  zoom_porcion=1; zoom_map(); zoom_porcion=0;
  zoom_x=_zoom_x; zoom_y=_zoom_y; zx=_zx; zy=_zy; zan=_zan; zal=_zal;

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Zoom, volcado de map(map_an*map_al) a copia segun zoom,zoom_x/y
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int an,al;

void zoom_map(void) {

  int n,m,c;
  byte *p,*q;
  int _big=big;

  if (!zoom_porcion) {

    p=map+zoom_y*map_an+zoom_x;
    q=copia;

    if ((map_an<<zoom)<vga_an) { // Copia de este trozo en select_zoom()
      zan=map_an<<zoom; zx=(vga_an-zan)/2; q+=zx; an=map_an;
    } else {
      zx=0; an=vga_an>>zoom; zan=an<<zoom;
    }

    if ((map_al<<zoom)<vga_al) {
      zal=map_al<<zoom; zy=(vga_al-zal)/2; q+=zy*vga_an; al=map_al;
    } else {
      zy=0; al=vga_al>>zoom; zal=al<<zoom;
    }

    if (!zoom_background) {
      if (zx || zy) {
        actualiza_background();
        volcado_completo=1;
      } else {
        if (_big) { big=0; big2=1; }
        if (zy+zal<vga_al) {
          wbox(copia,vga_an,vga_al,c2,zx-2,zy+zal+1,zan+4,1);
          wbox(copia,vga_an,vga_al,c0,zx-1,zy+zal,zan+2,1);
          volcado_parcial(0,zy+zal,vga_an,2); }
        if (zx+zan<vga_an) {
          wbox(copia,vga_an,vga_al,c2,zx-2,zy-2,zan+4,1);
          wbox(copia,vga_an,vga_al,c0,zx-1,zy-1,zan+2,1);
          volcado_parcial(zy+zan,0,2,vga_al); }
        if (_big) { big=1; big2=2; }
      }
      zoom_background=1;
    }

    if (zx || zy) volcado_parcial(zx,zy,zan,zal); else volcado_completo=1;

  } else {
    p=zoom_p; q=zoom_q; an=zoom_an; al=zoom_al;
  }

  switch (zoom) {
  case 0:
    m=al; do {
      memcpy(q,p,an);
      p+=map_an;
      q+=vga_an;
    } while (--m);
    break;

  case 1:
    m=al; do {
      n=an; do {
        c=*p; c+=c*256;
        *(word*)q=c;
        *(word*)(q+vga_an)=c;
        p++; q+=2;
      } while (--n);
      p+=map_an-an;
      q+=vga_an*2-an*2;
    } while (--m);
    break;

  case 2:
    m=al; do {
      n=an; do {
        c=*p; c+=c*256; c+=c*65536;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=vga_an*3-4;
        p++;
      } while (--n);
      p+=map_an-an;
      q+=vga_an*4-an*4;
    } while (--m);
    break;

  case 3:
    m=al; do {
      n=an; do {
        c=*p; c+=c*256; c+=c*65536;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q+=4;
        *(int*)q=c; q+=vga_an;
        *(int*)q=c; q-=4;
        *(int*)q=c; q-=vga_an*7-8;
        p++;
      } while (--n);
      p+=map_an-an;
      q+=vga_an*8-an*8;
    } while (--m);
    break;
  }

  if (!zoom_porcion) cclock=(*system_clock)>>1;

  if (sel_status) switch(modo_seleccion) {
    case -1: for (n=0;n<512;n+=2) if (v.mapa->puntos[n]!=-1) {
      draw_selection_box(v.mapa->puntos[n],v.mapa->puntos[n+1],v.mapa->puntos[n],v.mapa->puntos[n+1]);
      if (num_punto==n/2) {
        draw_selection_box(v.mapa->puntos[n]-1,v.mapa->puntos[n+1]-1,v.mapa->puntos[n]+1,v.mapa->puntos[n+1]+1);
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

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Sub barra de fill interpolativo
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void modo_inter(void) {

  int an,al;

  an=mab_x1-mab_x0+1+4; al=mab_y1-mab_y0+1+4;

  if ((m0=(word*)malloc(an*al*2))!=NULL)
  if ((m1=(word*)malloc(an*al*2))!=NULL) {

    make_near_regla();

    bar[0]=101+zoom; bar[1]=120; bar[2]=154; bar[3]=0;
    dibuja_barra(0); put_barra(10,2,118); hacer_zoom=1;

    memset(m0,0,an*al*2);
    memset(m1,0,an*al*2);

    do {
      ayuda_dibujo(1295);
      read_mouse(); select_zoom(); test_mouse();

      if (((mouse_b&1) && selected_icon==2)) {
        fill_inter(an,al); hacer_zoom=1;
      } else if ((mouse_b&1) && selected_icon==1) {
        fill_inter(an,al); hacer_zoom=1;
        do { read_mouse(); } while (mouse_b&1);
      }

      volcado_inter(an);

      volcado_edicion();
    } while (!(mouse_b&2) && !key(_ESC) && modo<100 &&
      !(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)));

    volcado_inter(an);

    if (key(_ESC)||(mouse_b && mouse_in(barra_x,barra_y+10,barra_x+9,barra_y+18)))
      { put_barra(2,10,45); volcar_barras(0);
        put(mouse_x,mouse_y,mouse_graf); volcado(copia); }

    free(m1); free(m0);
  } else free(m0);

  if (m0==NULL || m1==NULL) {
    v_texto=(char *)texto[45]; dialogo((int)err0);
  }

  modo+=100;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Interpolaciขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fill_inter(int an,int al) { // Rellena con medias una zona

  int x,y;
  word *si,*di,n0,n1,n2,n3;
  int n;

  // 1 - Desplaza m0 hacia derecha abajo un pixel

  for (y=al-1;y>0;y--) for (x=an-1;x>0;x--) *(m0+y*an+x)=*(m0+(y-1)*an+x-1);
  for (x=0;x<an;x++) *(m0+x)=0; for (y=0;y<al;y++) *(m0+y*an)=0;

  // 2 - Refresca en m0 el alambre de puntos que rodean a la zona

  for (y=mab_y0-1;y<=mab_y1+1;y++) if (y>=0 && y<map_al)
    for (x=mab_x0-1;x<=mab_x1+1;x++) if (x>=0 && x<map_an)
      if (is_near_mab(x,y)) {
        n=*(original+(y-mab_y0+1)*(an_original)+(x-mab_x0+1));
        n=(dac[n*3]+dac[n*3+1]+dac[n*3+2])*256+256;
        *(m0+(y-mab_y0+2)*an+(x-mab_x0+2))=n;
      } else if (!is_mab(x,y)) {
        *(m0+(y-mab_y0+2)*an+(x-mab_x0+2))=0;
      }

  // Interpola de m0 a m1

  si=m0; di=m1;

  for (y=0;y<al-1;y++) {
    for (x=0;x<an-1;x++) {

        if (n0=*si) // Pone en *(di) la media de los cuatro *(si+?)
             if (n1=*(si+1))
                  if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n0+n1+n2+n3)>>2;
                       else *di=(n0+n1+n2)/3;
                  else if (n3=*(si+an+1))
                            *di=(n0+n1+n3)/3;
                       else *di=(n0+n1)>>1;
             else if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n0+n2+n3)/3;
                       else *di=(n0+n2)>>1;
                  else if (n3=*(si+an+1))
                            *di=(n0+n3)>>1;
                       else *di=n0;
        else if (n1=*(si+1))
                  if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n1+n2+n3)/3;
                       else *di=(n1+n2)>>1;
                  else if (n3=*(si+an+1))
                            *di=(n1+n3)>>1;
                       else *di=n1;
             else if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n2+n3)>>1;
                       else *di=n2;
                  else if (n3=*(si+an+1))
                            *di=n3;
                       else *di=0;
      si++; di++;
    } si++; di++;
  }

  // Y ahora interpola de m1 a m0

  si=m1; di=m0;

  for (y=0;y<al-1;y++) {
    for (x=0;x<an-1;x++) {

        if (n0=*si) // Pone en *(di) la media de los cuatro *(si+?)
             if (n1=*(si+1))
                  if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n0+n1+n2+n3)>>2;
                       else *di=(n0+n1+n2)/3;
                  else if (n3=*(si+an+1))
                            *di=(n0+n1+n3)/3;
                       else *di=(n0+n1)>>1;
             else if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n0+n2+n3)/3;
                       else *di=(n0+n2)>>1;
                  else if (n3=*(si+an+1))
                            *di=(n0+n3)>>1;
                       else *di=n0;
        else if (n1=*(si+1))
                  if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n1+n2+n3)/3;
                       else *di=(n1+n2)>>1;
                  else if (n3=*(si+an+1))
                            *di=(n1+n3)>>1;
                       else *di=n1;
             else if (n2=*(si+an))
                       if (n3=*(si+an+1))
                            *di=(n2+n3)>>1;
                       else *di=n2;
                  else if (n3=*(si+an+1))
                            *di=n3;
                       else *di=0;

      si++; di++;
    } si++; di++;
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Selecciona un bloque por un relleno (x,y,fill_dac)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fill_select(word x,word y) {

  int _modo_fill;

  _modo_fill=modo_fill; modo_fill=1;
  if ((fss=(word*)malloc(60000))!=NULL) {
    fsp=fss; fsp_max=fss+30000; fill_scan(x,y); free(fss);
  } else {
    v_texto=(char *)texto[45]; dialogo((int)err0);
  }
  modo_fill=_modo_fill;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Relleno de una zona (que contiene colores fill_dac) con un color
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fill(word x,word y) {

  int a,b,c,d;

  if ((fss=(word*)malloc(60000))!=NULL) {

    fsp=fss; fsp_max=fss+30000;

    memset(mab,0,((map_an*map_al+31)/32)*4);

    if (modo_fill==2) {
      memset(fill_dac,1,256); // Rellena hasta el color seleccionado
      fill_dac[color]=0;
    } else {
      memset(fill_dac,0,256); // Rellena el color de map(x,y)
      fill_dac[*(map+y*map_an+x)]=1;
    }

    mab_x0=map_an; mab_y0=map_al; mab_x1=0; mab_y1=0;

    fill_scan(x,y);

    if (modo_fill==3) {
      if (mab_x0) a=mab_x0-1; else a=mab_x0;
      if (mab_y0) b=mab_y0-1; else b=mab_y0;
      if (mab_x1<map_an-1) c=mab_x1+1; else c=mab_x1;
      if (mab_y1<map_al-1) d=mab_y1+1; else d=mab_y1;
      c=c-a+1; d=d-b+1; an_original=c;
      if ((original=(byte*)save_undo(a,b,c,d))!=NULL) {
        if (!mab_x0) original--;
        if (!mab_y0) original-=an_original;
        modo_inter();
      }
    } else {
      if (save_undo(mab_x0,mab_y0,mab_x1-mab_x0+1,mab_y1-mab_y0+1))
        fill_draw();
    }

    free(fss);
  } else {
    v_texto=(char *)texto[45]; dialogo((int)err0);
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Algoritmo de fill (por scanes)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void fill_scan(word x,word y) {
  int x0,x1;

  nuevo_scan:

  x0=x; do x0--; while (x0>=0 && fill_dac[*(map+y*map_an+x0)]); x0++;
  x1=x; do x1++; while (x1<map_an && fill_dac[*(map+y*map_an+x1)]); x1--;

  if (x0<mab_x0) mab_x0=x0; if (x1>mab_x1) mab_x1=x1;
  if (y<mab_y0) mab_y0=y; if (y>mab_y1) mab_y1=y;

  for (x=x0;x<=x1;x++) set_mab(x,y);

  if (modo_fill==1) { if (x0>0) x0--; if (x1<map_an-1) x1++; }

  x=x0; sigue_scan_0:

  if (y>0) while (x<=x1) {
    if (!is_mab(x,y-1)) // Si no est  pintado
      if (fill_dac[*(map+(y-1)*map_an+x)]) // Si se tiene que pintar
        if (fsp<fsp_max) { // Si se puede pintar
          *fsp=x0; fsp++; *fsp=x1; fsp++;
          *fsp=x+2; fsp++; *fsp=y; fsp++;
          *fsp=0; fsp++; y--;
          goto nuevo_scan;
        }
    x++;
  }

  x=x0; sigue_scan_1:

  if (y<map_al-1) while (x<=x1) {
    if (!is_mab(x,y+1)) // Si no est  pintado
      if (fill_dac[*(map+(y+1)*map_an+x)]) // Si se tiene que pintar
        if (fsp<fsp_max) { // Si se puede pintar
          *fsp=x0; fsp++; *fsp=x1; fsp++;
          *fsp=x+2; fsp++; *fsp=y; fsp++;
          *fsp=1; fsp++; y++;
          goto nuevo_scan;
        }
    x++;
  }

  if (fsp>fss) { // Mientras queden scanes sin terminar
    fsp-=2; y=*fsp; fsp--; x=*fsp;
    fsp--; x1=*fsp; fsp--; x0=*fsp;
    if (*(fsp+4)) goto sigue_scan_1; else goto sigue_scan_0;
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Relleno del fill
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

extern byte * textura_color;
byte get_color(int x,int y);

void fill_draw(void) {
  int x,y;

  if (textura_color) {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+y*map_an+x)=get_color(x,y);
  } else {
    for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) *(map+y*map_an+x)=color;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado de la zona interpolada
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcado_inter(int an) { // Vuelca a pantalla la zona interpolada

  int x,y,n;

      for (y=mab_y0;y<=mab_y1;y++)
      for (x=mab_x0;x<=mab_x1;x++)
        if (is_mab(x,y)) {
          n=*(m0+(y-mab_y0+1)*an+(x-mab_x0+1)); if (n>256) n-=256; else n=0;
          *(map+y*map_an+x)=near_regla[n/256]; }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dibuja una selecciขn de mapa de bits (segฃn zoom,zoom_x/y,mab_*)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void draw_selection_mab(void) {

  int x0,y0,x1,y1; // Intersecciขn de la zona seleccionada y lo visto en zoom
  int x,y,c;
  byte *p,g0=c0,g4=c4;
  word g04,g40;
  int inc32,*_mab;

  x0=zoom_x; if (mab_x0>x0) x0=mab_x0;
  y0=zoom_y; if (mab_y0>y0) y0=mab_y0;
  x1=zoom_x+(zan>>zoom)-1; if (mab_x1<x1) x1=mab_x1;
  y1=zoom_y+(zal>>zoom)-1; if (mab_y1<y1) y1=mab_y1;

  // OJO !!!
  // p=copia+(zy+((y0-zoom_y)<<zoom))*vga_an+zx+((x0-zoom_x)<<zoom);

  if (x1>=x0 && y1>=y0) switch(zoom) {

    case 0: for (y=y0;y<=y1;y++) {
      inc32=y*map_an+x0; _mab=mab+(inc32>>5); inc32&=31;
      p=copia+(zy+y-zoom_y)*vga_an+(zx+x0-zoom_x); c=(cclock^y^x0)&1;
      for (x=x0;x<=x1;x++,p++,c^=1) {
        if (*_mab) if ((*_mab)&(1<<inc32))
            if (y==0 || x==0 || y==map_al-1 || x==map_an-1)  {
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
      inc32=y*map_an+x0; _mab=mab+(inc32>>5); inc32&=31;
      p=copia+(zy+(y-zoom_y)*2)*vga_an+zx+(x0-zoom_x)*2;
      for (x=x0;x<=x1;x++,p+=2) {
        if (*_mab) if ((*_mab)&(1<<inc32)) {
          if (y==0 || (y!=0 && !is_mab(x,y-1))) {
            *(word*)p=g04;
            if (y==map_al-1 || (y<map_al-1 && !is_mab(x,y+1))) {
              *(word*)(p+vga_an)=g40;
            } else {
              if (x==0 || (x!=0 && !is_mab(x-1,y))) {
                *(p+vga_an)=g4;
              }
              if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
                *(p+vga_an+1)=g0;
              }
            }
          } else if (x==0 || (x!=0 && !is_mab(x-1,y))) {
            *p=g0;
            *(p+vga_an)=g4;
            if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
              *(p+1)=g4;
              *(p+vga_an+1)=g0;
            } else if (y==map_al-1 || (y<map_al-1 && !is_mab(x,y+1))) {
              *(p+vga_an+1)=g0;
            }
          } else if (y==map_al-1 || (y<map_al-1 && !is_mab(x,y+1))) {
            *(word*)(p+vga_an)=g40;
            if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
              *(p+1)=g4;
            }
          } else if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
             *(p+1)=g4;
             *(p+vga_an+1)=g0;
          }
        }
        if (!(inc32=(inc32+1)&31)) _mab++;
      }
    } break;

    case 2: if (cclock&1) { g0=c4; g4=c0; }
    for (y=y0;y<=y1;y++) {
      p=copia+(zy+(y-zoom_y)*4)*vga_an+zx+(x0-zoom_x)*4;
      for (x=x0;x<=x1;x++,p+=4) if (is_mab(x,y)) {
        if (y==0 || (y!=0 && !is_mab(x,y-1))) {
          *p=g0; *(p+1)=g4;
          *(p+2)=g0; *(p+3)=g4; }
        if (x==0 || (x!=0 && !is_mab(x-1,y))) {
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p-=vga_an*3; }
        if (y==map_al-1 || (y<map_al-1 && !is_mab(x,y+1))) {
          p+=vga_an*3;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p-=vga_an*3+3; }
        if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
          p+=3;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p-=vga_an*3+3; }
      }
    } break;

    case 3: if (cclock&1) { g0=c4; g4=c0; }
    for (y=y0;y<=y1;y++) {
      p=copia+(zy+(y-zoom_y)*8)*vga_an+zx+(x0-zoom_x)*8;
      for (x=x0;x<=x1;x++,p+=8) if (is_mab(x,y)) {
        if (y==0 || (y!=0 && !is_mab(x,y-1))) {
          *p=g0; *(p+1)=g4;
          *(p+2)=g0; *(p+3)=g4;
          *(p+4)=g0; *(p+5)=g4;
          *(p+6)=g0; *(p+7)=g4; }
        if (x==0 || (x!=0 && !is_mab(x-1,y))) {
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p-=vga_an*7; }
        if (y==map_al-1 || (y<map_al-1 && !is_mab(x,y+1))) {
          p+=vga_an*7;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p++;
          *p=g4; p++;
          *p=g0; p-=vga_an*7+7; }
        if (x==map_an-1 || (x<map_an-1 && !is_mab(x+1,y))) {
          p+=7;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p+=vga_an;
          *p=g4; p+=vga_an;
          *p=g0; p-=vga_an*7+7; }
      }
    } break;
  }

}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dibuja una caja de selecciขn (segฃn zoom,zoom_x/y)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void draw_selection_box(int _x0,int _y0,int _x1,int _y1) {

  int x0,y0,x1,y1,n;
  byte *p;

  if (_x0>_x1) swap(_x0,_x1); if (_y0>_y1) swap(_y0,_y1);

  x0=zx+(_x0<<zoom)-(zoom_x<<zoom);
  y0=zy+(_y0<<zoom)-(zoom_y<<zoom);
  x1=zx+(_x1<<zoom)-(zoom_x<<zoom)+(1<<zoom)-1;
  y1=zy+(_y1<<zoom)-(zoom_y<<zoom)+(1<<zoom)-1;

  if (x1>=zx && x0<zx+zan && y1>=zy && y0<zy+zal) {

    if (y0>=zy) {
      if (x0<zx) x0=zx; if (x1>=zx+zan) x1=zx+zan-1;
      n=x1-x0+1;
      p=copia+vga_an*y0+x0;
      if ((y0^cclock)&1) do {
        if ((int)p&1) *p=c0; else *p=c4; p++;
      } while (--n);
      else do {
        if ((int)p&1) *p=c4; else *p=c0; p++;
      } while (--n);
    }

    if (y1<zy+zal) {
      if (x0<zx) x0=zx; if (x1>=zx+zan) x1=zx+zan-1;
      n=x1-x0+1;
      p=copia+vga_an*y1+x0;
      if ((y1^cclock)&1) do {
        if ((int)p&1) *p=c0; else *p=c4; p++;
      } while (--n);
      else do {
        if ((int)p&1) *p=c4; else *p=c0; p++;
      } while (--n);
    }

    x0=zx+(_x0<<zoom)-(zoom_x<<zoom);
    x1=zx+(_x1<<zoom)-(zoom_x<<zoom)+(1<<zoom)-1;

    if (x0>=zx) {
      if (y0<zy) y0=zy; if (y1>=zy+zal) y1=zy+zal-1;
      n=y1-y0+1;
      p=copia+vga_an*y0+x0;
      if ((x0^cclock)&1) do {
        if (y0&1) *p=c0; else *p=c4; p+=vga_an; y0++;
      } while (--n);
      else do {
        if (y0&1) *p=c4; else *p=c0; p+=vga_an; y0++;
      } while (--n);
      y0=zy+(_y0<<zoom)-(zoom_y<<zoom);
    }

    if (x1<zx+zan) {
      if (y0<zy) y0=zy; if (y1>=zy+zal) y1=zy+zal-1;
      n=y1-y0+1;
      p=copia+vga_an*y0+x1;
      if ((x1^cclock)&1) do {
        if (y0&1) *p=c0; else *p=c4; p+=vga_an; y0++;
      } while (--n);
      else do {
        if (y0&1) *p=c4; else *p=c0; p+=vga_an; y0++;
      } while (--n);
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones del mapa de bits, poner un pixel
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void set_mab(int x,int y) { // Pone un pixel en el mapa de bits

  x+=y*map_an; // Nง de bit en el buffer
  y=x>>5; // Nง de int en el buffer
  x&=31; // Nง de bit en el int
  *(mab+y)|=1<<x;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones del mapa de bits, consultar un pixel
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int is_mab(int x,int y) { // Consulta un pixel en el mapa de bits

  x+=y*map_an; // Nง de bit en el buffer
  y=x>>5; // Nง de int en el buffer
  x&=31; // Nง de bit en el int
  return(*(mab+y)&(1<<x));
}

int is_near_mab(int x,int y) {

  int c;

  if (x) c=is_mab(x-1,y); else c=0;
  if (x<map_an-1) c|=is_mab(x+1,y);
  if (y) c|=is_mab(x,y-1);
  if (y<map_al-1) c|=is_mab(x,y+1);

  return(c&& !is_mab(x,y));
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      UNDO - Guarda la zona ocupada por una acciขn, antes de realizarla claro
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int save_undo(int x, int y, int an, int al) {

  int a,start,end; // Inicio y fin del bloque guardado (en undo[])
  int ret=0;

  if (x<0) {an+=x; x=0; } if (y<0) {al+=y; y=0; }
  if (x+an>map_an) an=map_an-x; if (y+al>map_al) al=map_al-y;
  if (an<=0 || al<=0) return(-1);

  // Determina la zona de zoom a refrescar

  if (x<zoom_x) hacer_zoom_x=zx-((zoom_x-x)<<zoom);
  else hacer_zoom_x=zx+((x-zoom_x)<<zoom);
  if (y<zoom_y) hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  else hacer_zoom_y=zy+((y-zoom_y)<<zoom);
  hacer_zoom_an=an<<zoom; hacer_zoom_al=al<<zoom;

  if (hacer_zoom_x+hacer_zoom_an<=zx || hacer_zoom_y+hacer_zoom_al<=zy ||
      hacer_zoom_x>=zx+zan || hacer_zoom_y>=zy+zal) {
    hacer_zoom_an=0; hacer_zoom_al=0;
  } else {
    if (hacer_zoom_x<zx) { hacer_zoom_an-=zx-hacer_zoom_x; hacer_zoom_x=zx; }
    if (hacer_zoom_y<zy) { hacer_zoom_al-=zy-hacer_zoom_y; hacer_zoom_y=zy; }
    if (hacer_zoom_x+hacer_zoom_an>zx+zan) hacer_zoom_an=zx+zan-hacer_zoom_x;
    if (hacer_zoom_y+hacer_zoom_al>zy+zal) hacer_zoom_al=zy+zal-hacer_zoom_y;
    if (!hacer_zoom) hacer_zoom=-1;
  }

  if (iundo) a=iundo-1; else a=max_undos-1;

  start=0; if (tundo[a].modo!=-1) start=tundo[a].end;

  if (start+an*al>undo_memory) start=0;

  // Si una acciขn ocupa m s de undo_memory, entonces no la guardamos.

  if ((end=start+an*al)<=undo_memory) {

    tundo[iundo].start=start; tundo[iundo].end=end;

    tundo[iundo].x=x; tundo[iundo].y=y;
    tundo[iundo].an=an; tundo[iundo].al=al;

    tundo[iundo].modo=modo;

    // Nos cargamos los bloques machacados

    for (a=0;a<max_undos;a++)
      if (a!=iundo && tundo[a].modo!=-1)
        if ((tundo[a].start<start && tundo[a].end>start)||
           (tundo[a].start>=start && tundo[a].start<end))
           tundo[a].modo=-1;

    // Guardamos la zona que ocupar  la acciขn realizada.

    copy_block(undo+start,map+x+y*map_an,an,al);

    tundo[iundo].codigo=codigo_mapa;

    iundo=(iundo+1)%max_undos; tundo[iundo].modo=-1; // Se prohibe undo_next()

    ret=(int)(undo+start);

  } else {
    fondo_edicion(0,0,vga_an,vga_al);
    volcar_barras(1);
    volcado_completo=1; volcado(copia);
    v_texto=(char *)texto[320]; dialogo((int)err0); undo_error=1;
  }

  return(ret);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      UNDO - Restaura la zona ocupada por la ฃltima acciขn.
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int undo_back(void) {
  int x,y,an,al;

  if (iundo) a=iundo-1; else a=max_undos-1;

  if (tundo[a].modo!=-1 && tundo[a].codigo==codigo_mapa) {

    iundo=a; retrazo();

    x=tundo[a].x; y=tundo[a].y;
    an=tundo[a].an; al=tundo[a].al;

    xchg_block(undo+tundo[a].start,map+x+y*map_an,an,al);

    // Determina la zona de zoom a refrescar

    if (x<zoom_x) hacer_zoom_x=zx-((zoom_x-x)<<zoom);
    else hacer_zoom_x=zx+((x-zoom_x)<<zoom);
    if (y<zoom_y) hacer_zoom_y=zy+((y-zoom_y)<<zoom);
    else hacer_zoom_y=zy+((y-zoom_y)<<zoom);
    hacer_zoom_an=an<<zoom; hacer_zoom_al=al<<zoom;

    if (hacer_zoom_x+hacer_zoom_an<=zx || hacer_zoom_y+hacer_zoom_al<=zy ||
        hacer_zoom_x>=zx+zan || hacer_zoom_y>=zy+zal) {
      hacer_zoom_an=0; hacer_zoom_al=0;
    } else {
      if (hacer_zoom_x<zx) { hacer_zoom_an-=zx-hacer_zoom_x; hacer_zoom_x=zx; }
      if (hacer_zoom_y<zy) { hacer_zoom_al-=zy-hacer_zoom_y; hacer_zoom_y=zy; }
      if (hacer_zoom_x+hacer_zoom_an>zx+zan) hacer_zoom_an=zx+zan-hacer_zoom_x;
      if (hacer_zoom_y+hacer_zoom_al>zy+zal) hacer_zoom_al=zy+zal-hacer_zoom_y;
      if (!hacer_zoom) hacer_zoom=-1;
    } return(1);
  } else return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      UNDO - Rehace la untima acciขn desecha con undo_back()
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void undo_next(void) {
  int x,y,an,al;

  if (tundo[iundo].modo!=-1 && tundo[iundo].codigo==codigo_mapa) {

    retrazo();

    x=tundo[iundo].x; y=tundo[iundo].y;
    an=tundo[iundo].an; al=tundo[iundo].al;

    xchg_block(undo+tundo[iundo].start,map+x+y*map_an,an,al);

    // Determina la zona de zoom a refrescar

    if (x<zoom_x) hacer_zoom_x=zx-((zoom_x-x)<<zoom);
    else hacer_zoom_x=zx+((x-zoom_x)<<zoom);
    if (y<zoom_y) hacer_zoom_y=zy+((y-zoom_y)<<zoom);
    else hacer_zoom_y=zy+((y-zoom_y)<<zoom);
    hacer_zoom_an=an<<zoom; hacer_zoom_al=al<<zoom;

    if (hacer_zoom_x+hacer_zoom_an<=zx || hacer_zoom_y+hacer_zoom_al<=zy ||
        hacer_zoom_x>=zx+zan || hacer_zoom_y>=zy+zal) {
      hacer_zoom_an=0; hacer_zoom_al=0;
    } else {
      if (hacer_zoom_x<zx) { hacer_zoom_an-=zx-hacer_zoom_x; hacer_zoom_x=zx; }
      if (hacer_zoom_y<zy) { hacer_zoom_al-=zy-hacer_zoom_y; hacer_zoom_y=zy; }
      if (hacer_zoom_x+hacer_zoom_an>zx+zan) hacer_zoom_an=zx+zan-hacer_zoom_x;
      if (hacer_zoom_y+hacer_zoom_al>zy+zal) hacer_zoom_al=zy+zal-hacer_zoom_y;
      if (!hacer_zoom) hacer_zoom=-1;
    }

    iundo=(iundo+1)%max_undos;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Copia un bloque del mapa a memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void copy_block(byte *d,byte *s,int an,int al) {

  do {
    memcpy(d,s,an);
    d+=an; s+=map_an;
  } while (--al);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Intercambia un bloque entre el mapa y memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void xchg_block(byte *d,byte *s,int an,int al) {

  do {
    memxchg(d,s,an);
    d+=an; s+=map_an;
  } while (--al);
}

