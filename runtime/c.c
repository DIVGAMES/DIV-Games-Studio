
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Cขdigo de las funciones de detecciขn de colisiขn
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
#include "inter.h"
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Cabeceras
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
int  comprobar_colisiones(int i,int bloque,int scroll);
void test_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags);
void sp_rotado_p(byte * si, int an, int al, int flags);
void test_normal(byte * p, int x, int y, int an, int al, int flags);
void test_cortado(byte * p, int x, int y, int an, int al, int flags);
void test_escalado(byte * old_si, int x, int y, int an, int al, int xg, int yg,int size, int flags);
void test_rotado(byte * si, int an, int al, int flags);
void test_scanc(byte * p,short n,short m,short o,byte * si,int an,int x0,int y0,int x1,int y1);
void test_scan(byte * p,short n,byte * si,int an,int x0,int y0,int x1,int y1);

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Variables definidas a nivel de mขdulo
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int colisiona;
int p[24];

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Out_region(id,region) - solo para gr ficos de pantalla o scroll
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void out_region(void) {

  int id,file,graph,reg,angle;
  int xg,yg,xg1,yg1,x,y,an,al;
  int * ptr,n,m;

  reg=pila[sp--]; id=pila[sp]; pila[sp]=1; // Por defecto no est  en la region

  if (mem[id+_Ctype]==2) { e(137); return; }

  file=mem[id+_File]; graph=mem[id+_Graph]; angle=mem[id+_Angle];

  if (reg<0 || reg>=max_region) { e(108); return; } // Ya tiene region[reg].(x0..y1)

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

  an=ptr[13]; al=ptr[14];

  if (angle) {
    sp_size(&x,&y,&an,&al,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);
    an=an-x+1; al=al-y+1;
  } else if (mem[id+_Size]!=100) {
    sp_size_scaled(&x,&y,&an,&al,xg,yg,mem[id+_Size],mem[id+_Flags]);
    an=an-x+1; al=al-y+1;
  } else {
    if (mem[id+_Flags]&1) xg=an-1-xg; x-=xg;
    if (mem[id+_Flags]&2) yg=al-1-yg; y-=yg;
  }

  // Ya tiene region[reg].(x0..y1) y sprite(x,y,an,al)

  if (mem[id+_Ctype]==1) {

    for (n=0;n<10;n++)if(iscroll[n].on&&(mem[id+_Cnumber]==0||(mem[id+_Cnumber]&(1<<n)))) {

      xg=x+iscroll[n].x-iscroll[n].map1_x; // Posiciขn del sprite en el scroll
      yg=y+iscroll[n].y-iscroll[n].map1_y;

      xg1=xg+an; yg1=yg+al; // 1ง Intersecciขn de xg,yg,an,al e iscroll[n]

      if (iscroll[n].x>xg) xg=iscroll[n].x;
      if (iscroll[n].y>yg) yg=iscroll[n].y;
      if (iscroll[n].x+iscroll[n].an<xg1) xg1=iscroll[n].x+iscroll[n].an;
      if (iscroll[n].y+iscroll[n].al<yg1) yg1=iscroll[n].y+iscroll[n].al;

      if (xg>=xg1 || yg>=yg1) continue; // Si no hay intersecciขn, continua

      if (xg<region[reg].x1 && yg<region[reg].y1 && xg1>region[reg].x0 &&
        yg1>region[reg].y0) { pila[sp]=0; return; } // Si est  en la regiขn

    }

  } else if (mem[id+_Ctype]==0) {

    if (x<region[reg].x1 && y<region[reg].y1 && x+an>region[reg].x0 &&
        y+al>region[reg].y0) pila[sp]=0; // Si est  en la regiขn

  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Graphic_info(file,graph,info)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void graphic_info(void) {

  int info,graph,file;
  int xg,yg;
  int * ptr;

  info=pila[sp--]; graph=pila[sp--]; file=pila[sp]; pila[sp]=0;

  if (file<0 || file>max_fpgs) { e(109); return; }
  if (file) max_grf=1000; else max_grf=2000;
  if (graph<=0 || graph>=max_grf) { e(110); return; }
  if (g[file].grf==NULL) { e(111); return; }
  if ((ptr=g[file].grf[graph])==NULL) { e(121); return; }

  if (ptr[15]==0 || *((word*)ptr+32)==65535) { xg=ptr[13]/2; yg=ptr[14]/2;
  } else { xg=*((word*)ptr+32); yg=*((word*)ptr+33); }

  switch(pila[sp+2]) {
    case 0: pila[sp]=ptr[13]; break;                    //g_an (original)
    case 1: pila[sp]=ptr[14]; break;                    //g_al (original)
    case 2: pila[sp]=xg; break;                         //g_x_centro
    case 3: pila[sp]=yg; break;                         //g_y_centro
    default: e(138);
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Funcion principal del lenguaje collision(tipo de proceso)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

// *** OJO, Detecta choques pantalla-pantalla, scroll-scroll y pantalla-scroll
// Pero los gr ficos que est n en scroll detectan colisiones sขlo en el primer
// scroll en el que est n (aunque estn en varios)

void collision(void) {

  int i,bloque; // Recorre procesos de _IdScan en adelante, si _IdScan=0 desde start
  int file,graph,angle;
  int x,y,n,m;  // Coordenadas del proceso actual
  short xg,yg;  // Centro gravitatorio del gr fico del proceso actual
  int * ptr;

  bloque=pila[sp]; pila[sp]=0; // Por defecto no colisiona

  if (mem[id+_Ctype]==2) { e(139); return; }

  if (mem[id+_IdScan]==0 || bloque!=mem[id+_BlScan]) {
    mem[id+_BlScan]=bloque; i=id_start;
  } else if (mem[id+_IdScan]>id_end) { pila[sp]=0; return; }
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

  buffer_an=clipx1-clipx0+1; buffer_al=clipy1-clipy0+1;
  if ((buffer=(byte *)malloc(buffer_an*buffer_al))==NULL) { e(100); return; }
  memset(buffer,0,buffer_an*buffer_al);

  // Ahora se tiene que pintar el sprite en el buffer(clip...)

  put_collision(buffer,ptr,x,y,xg,yg,angle,mem[id+_Size],mem[id+_Flags]);

  // Ya tiene region del sprite clipx0..clipy1 (si es en pantalla)

  if (mem[id+_Ctype]==1) {
    for(n=0;n<10;n++)if(iscroll[n].on&&(mem[id+_Cnumber]==0||(mem[id+_Cnumber]&(1<<n)))) {
      xg=iscroll[n].x-iscroll[n].map1_x;
      yg=iscroll[n].y-iscroll[n].map1_y;
      clipx0+=xg; clipx1+=xg; clipy0+=yg; clipy1+=yg;
      pila[sp]=comprobar_colisiones(i,bloque,n); break;
    }
  } else pila[sp]=comprobar_colisiones(i,bloque,-1);

  free(buffer);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Comprueba colisiones del sprite id (en buffer) con el resto (i..id_end)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int comprobar_colisiones(int i,int bloque,int scroll) {
  int file,graph,angle;
  int *ptr,n,m,j;
  int x,y,dist=0;
  int xx,yy,an,al;
  short xg,yg,xxg,yyg;

  if (bloque==0) { // collision(type mouse)
    if (mouse->x>=clipx0 && mouse->x<=clipx1 && mouse->y>=clipy0 && mouse->y<=clipy1) {
      if (*(buffer+buffer_an*(mouse->y-clipy0)+(mouse->x-clipx0))) return(id); else return(0);
    } else return(0);
  }

  for (;i<=id_end;i+=iloc_len) {

    if (i!=id && mem[i+_Bloque]==bloque && (mem[i+_Status]==2 ||
      	mem[i+_Status]==4) && mem[i+_Ctype]<2) {

      if (mem[i+_Ctype]==1) { // Si es de scroll, determina que scroll (n)
        if (scroll<0) { // Choque de pantalla con scroll - el primero activo
      	  for(n=0;n<10;n++)
      	    if(iscroll[n].on&&(mem[i+_Cnumber]==0||(mem[i+_Cnumber]&(1<<n)))) break;
      	  if (n==10) continue;
        } else { // Choque de scroll con scroll - simpre que coincidan en el 1ง
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

      colisiona=0;

      if (angle) {

        // Distancia de seguridad

        if (dist==0) dist=(int)sqrt((buffer_an*buffer_an+buffer_al*buffer_al)/4);
        an=((int)sqrt(ptr[13]*ptr[13]+ptr[14]*ptr[14])*mem[i+_Size])/100+dist;

        // Distancia real

        xx=(clipx0+clipx1)/2-x; yy=(clipy0+clipy1)/2-y; al=(int)sqrt(xx*xx+yy*yy);

        if (al<=an) {
          xx=x; yy=y; an=ptr[13]; al=ptr[14];
          sp_size(&xx,&yy,&an,&al,xg,yg,angle,mem[i+_Size],mem[i+_Flags]);

          if (clipx1>=xx && clipx0<=an)
            if (clipy1>=yy && clipy0<al)
              colisiona=1;
        }

      } else if (mem[i+_Size]!=100) {

        xx=x; yy=y; an=ptr[13]; al=ptr[14];
        sp_size_scaled(&xx,&yy,&an,&al,xg,yg,mem[i+_Size],mem[i+_Flags]);

        if (clipx1>=xx && clipx0<=an)
          if (clipy1>=yy && clipy0<al) colisiona=1;

      } else {

        an=ptr[13]; al=ptr[14];
        if (mem[i+_Flags]&1) xxg=an-1-xg; else xxg=xg; xx=x-xxg;
        if (mem[i+_Flags]&2) yyg=al-1-yg; else yyg=yg; yy=y-yyg;
        if (clipx1>=xx && clipx0<xx+an)
          if (clipy1>=yy && clipy0<yy+al) colisiona=1;

      }

      if (colisiona==1) {
        colisiona=0;
        test_collision(buffer,ptr,x,y,xg,yg,angle,mem[i+_Size],mem[i+_Flags]);
        if (colisiona) {
      	  mem[id+_IdScan]=i+iloc_len; return(i);
        }
      }

    }
  }

  mem[id+_IdScan]=i; return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Devuelve la caja en la que se encuentra un gr fico rotado y escalado
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
//      Devuelve la caja en la que se encuentra un gr fico rotado y/o escalado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_size_scaled( int *x, int *y, int *xx, int *yy, int xg, int yg,
                     int size, int flags) {

  int x0,y0,x1,y1; // Ventana ocupada por el sprite en el plano de copia

  if (flags&1) x0=*x-((*xx-1-xg)*size)/100;
  else x0=*x-(xg*size)/100;
  if (flags&2) y0=*y-((*yy-1-yg)*size)/100;
  else y0=*y-(yg*size)/100;

  x1=x0+(*xx*size)/100-1; y1=y0+(*yy*size)/100-1;

  if (x1<x0) x1=x0; if (y1<y0) y1=y0; // ***

  *x=x0; *y=y0; *xx=x1; *yy=y1;

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funcion externa para pintar gr ficos (para las colisiones)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void put_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags) {

  byte * si;
  int an,al; // Informaciขn respecto a pantalla del grafico
  byte * _copia;
  int _vga_an,_vga_al;
  int ix,iy;

  an=ptr[13]; al=ptr[14]; si=(byte*)ptr+64+ptr[15]*4;

  ix=clipx0; iy=clipy0;

  _copia=copia; copia=buffer;
  _vga_an=vga_an; vga_an=buffer_an;
  _vga_al=vga_al; vga_al=buffer_al;
  clipx0=0; clipx1-=ix-1;
  clipy0=0; clipy1-=iy-1;

  if (angle) {
    for (x=0;x<8;x+=2) { p[x]-=ix; p[x+1]-=iy; }
    sp_rotado_p(si,an,al,flags);
  } else if (size!=100) {
    x-=ix; y-=iy;
    sp_escalado(si,x,y,an,al,xg,yg,size,flags);
  } else {
    if (flags&1) xg=an-1-xg; x-=xg+ix;
    if (flags&2) yg=al-1-yg; y-=yg+iy;
    if (x>=clipx0 && x+an<=clipx1 && y>=clipy0 && y+al<=clipy1) // Pinta sprite sin cortar
      sp_normal(si,x,y,an,al,flags);
    else if (x<clipx1 && y<clipy1 && x+an>clipx0 && y+al>clipy0) // Pinta sprite cortado
      sp_cortado(si,x,y,an,al,flags);
  }

  copia=_copia;
  vga_an=_vga_an;
  vga_al=_vga_al;
  clipx0=ix; clipx1+=ix-1;
  clipy0=iy; clipy1+=iy-1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Sprite - rotado [escalado] [cortado] [espejado] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void sp_rotado_p(byte * si, int an, int al, int flags) {

  int h,hmin,hmax; // Altura minima y maxima
  int n,l0,l1;   // Lado 0 y lado 1 (indices p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y,kk;
  byte * ptrcopia;

  memcpy(&p[8],&p[0],sizeof(p[0])*8); memcpy(&p[16],&p[0],sizeof(p[0])*8);

  hmin=32767; hmax=-32768; n=8; do {
    if (p[n+1]<hmin) { l0=n; hmin=p[n+1]; }
    if (p[n+1]>hmax) hmax=p[n+1];
    n+=2;
  } while (n<16);

  l1=l0; hmax0=hmin; hmax1=hmin; ptrcopia=copia+hmin*vga_an;

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

   if ((flags&3)==1 || (flags&3)==2) {
     kk=x0.l; x0.l=x1.l; x1.l=kk;
     kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
     kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    if (h<clipy1 && h>=clipy0 && x0.w[1]<clipx1 && x1.w[1]>=clipx0 && x1.w[1]>x0.w[1])
      sp_scan(ptrcopia+x0.w[1],x1.w[1]-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l);

    if ((flags&3)==1 || (flags&3)==2) {
      kk=x0.l; x0.l=x1.l; x1.l=kk;
      kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
      kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrcopia+=vga_an;

  } while (h++<hmax);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funcion externa para comprobar gr ficos (para las colisiones)
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_collision(byte * buffer, int * ptr, int x, int y, int xg, int yg, int angle, int size, int flags) {

  byte * si;
  int an,al; // Informaciขn respecto a pantalla del grafico
  byte * _copia;
  int _vga_an,_vga_al;
  int ix,iy;

  an=ptr[13]; al=ptr[14]; si=(byte*)ptr+64+ptr[15]*4;

  ix=clipx0; iy=clipy0;

  _copia=copia; copia=buffer;
  _vga_an=vga_an; vga_an=buffer_an;
  _vga_al=vga_al; vga_al=buffer_al;
  clipx0=0; clipx1-=ix-1;
  clipy0=0; clipy1-=iy-1;

  if (angle) {
    for (x=0;x<8;x+=2) { p[x]-=ix; p[x+1]-=iy; }
    test_rotado(si,an,al,flags);
  } else if (size!=100) {
    x-=ix; y-=iy;
    test_escalado(si,x,y,an,al,xg,yg,size,flags);
  } else {
    if (flags&1) xg=an-1-xg; x-=xg+ix;
    if (flags&2) yg=al-1-yg; y-=yg+iy;
    if (x>=clipx0 && x+an<=clipx1 && y>=clipy0 && y+al<=clipy1) // Pinta sprite sin cortar
      test_normal(si,x,y,an,al,flags);
    else if (x<clipx1 && y<clipy1 && x+an>clipx0 && y+al>clipy0) // Pinta sprite cortado
      test_cortado(si,x,y,an,al,flags);
  }

  copia=_copia;
  vga_an=_vga_an;
  vga_al=_vga_al;
  clipx0=ix; clipx1+=ix-1;
  clipy0=iy; clipy1+=iy-1;
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - [espejado] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_normal(byte * p, int x, int y, int an, int al, int flags) {

  byte *q=copia+y*vga_an+x;
  int ancho=an;

  switch (flags&3) {
    case 0: //--
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p++; q++;
        } while (--an);
        q+=vga_an-(an=ancho);
      } while (--al); break;
    case 1: //h-
      p+=an-1;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p--; q++;
        } while (--an);
        q+=vga_an-(an=ancho); p+=an*2;
      } while (--al); break;
    case 2: //-v
      p+=(al-1)*an;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p++; q++;
        } while (--an);
        q+=vga_an-(an=ancho); p-=an*2;
      } while (--al); break;
    case 3: //hv
      p+=al*an-1;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p--; q++;
        } while (--an);
        q+=vga_an-(an=ancho);
      } while (--al); break;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - cortado [espejado] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_cortado(byte * p, int x, int y, int an, int al, int flags) {

  byte *q=copia+y*vga_an+x;
  int salta_x, long_x, resto_x;
  int salta_y, long_y, resto_y;

  if (x<clipx0) salta_x=clipx0-x; else salta_x=0;
  if (x+an>clipx1) resto_x=x+an-clipx1; else resto_x=0;
  long_x=an-salta_x-resto_x;

  if (y<clipy0) salta_y=clipy0-y; else salta_y=0;
  if (y+al>clipy1) resto_y=y+al-clipy1; else resto_y=0;
  long_y=al-salta_y-resto_y;

  switch (flags&3) {
    case 0: //--
      p+=an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
      resto_x+=salta_x; an=long_x;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p++; q++;
        } while (--an);
        q+=vga_an-(an=long_x); p+=resto_x;
      } while (--long_y); break;
    case 1: //h-
      p+=an*salta_y+an-1-salta_x; q+=vga_an*salta_y+salta_x;
      resto_x+=salta_x; salta_x=long_x;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p--; q++;
        } while (--salta_x);
        q+=vga_an-(salta_x=long_x); p+=an+long_x;
      } while (--long_y); break;
    case 2: //-v
      p+=(al-1)*an-an*salta_y+salta_x; q+=vga_an*salta_y+salta_x;
      resto_x+=salta_x; salta_x=long_x;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p++; q++;
        } while (--salta_x);
        q+=vga_an-(salta_x=long_x); p+=resto_x-an*2;
      } while (--long_y); break;
    case 3: //hv
      p+=al*an-1-an*salta_y-salta_x; q+=vga_an*salta_y+salta_x;
      resto_x+=salta_x; salta_x=long_x;
      do {
        do {
          if (*p && *q) { colisiona=1; return;}
          p--; q++;
        } while (--salta_x);
        q+=vga_an-(salta_x=long_x); p-=resto_x;
      } while (--long_y); break;
  }
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - escalado [cortado] [espejado] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_escalado(byte * old_si, int x, int y, int an, int al, int xg, int yg,
                   int size, int flags) {

  int x0,y0,x1,y1; // Ventana ocupada por el sprite en el plano de copia
  int salta_x, long_x, resto_x; // Referidas a pantalla
  int salta_y, long_y, resto_y;
  int xr,ixr,yr,iyr,old_xr,old_an;
  byte *si,*di;

  if (flags&1) x0=x-((an-1-xg)*size)/100;
  else x0=x-(xg*size)/100;
  if (flags&2) y0=y-((al-1-yg)*size)/100;
  else y0=y-(yg*size)/100;
  x1=x0+(an*size)/100-1; y1=y0+(al*size)/100-1;

  if (x1<x0 || y1<y0) return;

  ixr=(float)(an*256)/(float)(x1-x0+1); iyr=(float)(al*256)/(float)(y1-y0+1);

  if (x1<clipx0 || y1<clipy0 || x0>=clipx1 || y0>=clipy1) return;

  di=copia+y0*vga_an+x0;

  if (x0<clipx0) salta_x=clipx0-x0; else salta_x=0;
  if (x1>=clipx1) resto_x=x1-clipx1+1; else resto_x=0;
  long_x=(an*size)/100-salta_x-resto_x;

  if (y0<clipy0) salta_y=clipy0-y0; else salta_y=0;
  if (y1>=clipy1) resto_y=y1-clipy1+1; else resto_y=0;
  long_y=(al*size)/100-salta_y-resto_y;

  if (flags&1) { xr=an*256-salta_x*ixr-1; ixr=-ixr; } else xr=salta_x*ixr;
  if (flags&2) { yr=al*256-salta_y*iyr-1; iyr=-iyr; } else yr=salta_y*iyr;

  old_xr=xr; old_an=an; di+=vga_an*salta_y+salta_x; an=long_x;

  do {
    si=old_si+(yr>>8)*old_an;
    do {
      if (*(si+(xr>>8)) && *di) { colisiona=1; return;}
      di++; xr+=ixr;
    } while (--an);
    yr+=iyr; xr=old_xr;
    di+=vga_an-(an=long_x);
  } while (--long_y);

}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Test sprite - rotado [escalado] [cortado] [espejado] [ghost]
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void test_rotado(byte * si, int an, int al, int flags) {

  int h,hmin,hmax; // Altura minima y maxima
  int n,l0,l1;     // Lado 0 y lado 1 (indices p[])

  int hmax0,hmax1;
  union { int l; short w[2]; } x0,x1,g0x,g1x,g0y,g1y;
  int ix0,ix1,ig0x,ig1x,ig0y,ig1y,kk;
  byte * ptrcopia;

  memcpy(&p[8],&p[0],sizeof(p[0])*8); memcpy(&p[16],&p[0],sizeof(p[0])*8);

  hmin=32767; hmax=-32768; n=8; do {
    if (p[n+1]<hmin) { l0=n; hmin=p[n+1]; }
    if (p[n+1]>hmax) hmax=p[n+1];
    n+=2;
  } while (n<16);

  l1=l0; hmax0=hmin; hmax1=hmin; ptrcopia=copia+hmin*vga_an;

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

   if ((flags&3)==1 || (flags&3)==2) {
     kk=x0.l; x0.l=x1.l; x1.l=kk;
     kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
     kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    if (h<clipy1 && h>=clipy0 && x0.w[1]<clipx1 && x1.w[1]>=clipx0 && x1.w[1]>x0.w[1])
    if (x0.w[1]<clipx0) if (x1.w[1]>=clipx1)
      test_scanc(ptrcopia+clipx0,x1.w[1]-x0.w[1],clipx1-clipx0-1,clipx0-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l);
    else test_scanc(ptrcopia+clipx0,x1.w[1]-x0.w[1],x1.w[1]-clipx0,clipx0-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l);
    else if (x1.w[1]>=clipx1)
      test_scanc(ptrcopia+x0.w[1],x1.w[1]-x0.w[1],clipx1-1-x0.w[1],0,si,an,g0x.l,g0y.l,g1x.l,g1y.l);
    else test_scan(ptrcopia+x0.w[1],x1.w[1]-x0.w[1],si,an,g0x.l,g0y.l,g1x.l,g1y.l);

    if ((flags&3)==1 || (flags&3)==2) {
      kk=x0.l; x0.l=x1.l; x1.l=kk;
      kk=g0x.l; g0x.l=g1x.l; g1x.l=kk;
      kk=g0y.l; g0y.l=g1y.l; g1y.l=kk;
    }

    x0.l+=ix0; x1.l+=ix1; g0x.l+=ig0x; g1x.l+=ig1x; g0y.l+=ig0y; g1y.l+=ig1y;

    ptrcopia+=vga_an;

  } while (h++<hmax);

}

void test_scanc(byte * p,short n,short m,short o,byte * si,int an,int x0,int y0,int x1,int y1) {

  union { int l; short w[2]; } x,y;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  if (o) { x.l+=x0*o; y.l+=y0*o; }

  do {
    if (*p && *(si+x.w[1]+y.w[1]*an)) { colisiona=1; return;}
    p++; x.l+=x0; y.l+=y0;
  } while (m--);
}

void test_scan(byte * p,short n,byte * si,int an,int x0,int y0,int x1,int y1) {

  union { int l; short w[2]; } x,y;

  x.l=x0; y.l=y0; x0=(x1-x0)/n; y0=(y1-y0)/n;

  do {
    if (*p && *(si+x.w[1]+y.w[1]*an)) { colisiona=1; return;}
    p++; x.l+=x0; y.l+=y0;
  } while (n--);
}

