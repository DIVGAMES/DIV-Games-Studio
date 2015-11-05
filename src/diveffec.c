//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Mขdulo para la creacion de explosiones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#include "global.h"

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Definiciones
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

#define max_pun 64      // Nฃmero m ximo de puntos de control
#define max_exp 8       // Nฃmero m ximo de explosiones base
#define DEEP    4
#pragma on (check_stack);

#define uchar unsigned char
#define byte unsigned char
#define ushort unsigned short
#define word unsigned short

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Declaracion de procedimientos internos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_rnd(int,char*); // Inicializa el generador aleatorio con un int y clave
byte rndb(void);	    	  // Obtiene un byte del generador aleatorio
int  rnd(void); 		      // Obtiene un int del generador aleatorio

void gama0(void);
byte exp_colores[128];
struct _gcolor exp_gama[9];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Tablas y variables globales
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int cx,cy;
int ExpGama;

struct tp {			// Estructura para los puntos de la explosiขn
  int x,y;                      // Coordenadas del punto
  int radio;                    // Radio efectivo del punto
  int fuerza;                   // Fuerza central del punto
  int xr,yr;                    // Coord. * 10000
  int ix,iy;                    // Incrementos
};

struct te {                     // Estructura para una explosiขn base
  struct tp p[max_pun];
  byte * ptr;
} e[max_exp];

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Par metros para crear la explosiขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int exp_ancho=32;      // 8..256
int exp_alto=32;       // 8..256
int n_pun;      // 16..64
int n_exp;      // 1..8
int n_frames;   // 1..n
int _n_frames=6;   // 1..n
int paso_frame;
int tipo_exp=1;
int per_points;
char *Buff_exp;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//	Crea el array de puntos
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void crear_puntos(void) {
  int m,n,rx,ry;
  double ang,dist;

  for (m=0;m<n_exp;m++) {

    for (n=0;n<n_pun;n++) {

      ang=(double)rnd()/10000.0; // Angulo (?)
      dist=(double)(rnd()%10000)/10000.0; // Distancia (0 .. 0.8)

      e[m].p[n].ix=cos(ang)*10000.0;
      e[m].p[n].iy=sin(ang)*10000.0;

      e[m].p[n].x=cx+(int)(cos(ang)*dist*(float)(exp_ancho/2));
      e[m].p[n].y=cy+(int)(sin(ang)*dist*(float)(exp_alto/2));

      e[m].p[n].xr=e[m].p[n].x*10000;
      e[m].p[n].yr=e[m].p[n].y*10000;

      if (e[m].p[n].x<cx) rx=e[m].p[n].x; else rx=exp_ancho-e[m].p[n].x;
      if (e[m].p[n].y<cy) ry=e[m].p[n].y; else ry=exp_alto-e[m].p[n].y;
      if (rx<ry) e[m].p[n].radio=rx*10000; else e[m].p[n].radio=ry*10000;

      e[m].p[n].fuerza=e[m].p[n].radio+rnd()%(e[m].p[n].radio)*4;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Avanza un frame en la explosiขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void avanzar_puntos(void) {
  int m,n;
  for (m=0;m<n_exp;m++) {
        for (n=0;n<n_pun;n++)
        {
                e[m].p[n].xr+=(e[m].p[n].ix*paso_frame)/10000;
                e[m].p[n].yr+=(e[m].p[n].iy*paso_frame)/10000;
                e[m].p[n].x=e[m].p[n].xr/10000;
                e[m].p[n].y=e[m].p[n].yr/10000;
                if (e[m].p[n].fuerza>e[m].p[n].radio)
                        e[m].p[n].fuerza=(int)((float)e[m].p[n].fuerza*(float)0.86);//0.98
                e[m].p[n].radio-=paso_frame;
        }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//	Pinta la explosiขn
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int pintar_explosion(void) {
  int m,x,y,dx,dy,n;
  int dist,exp_Color,exp_Coloracum;

        Progress((char *)texto[434],0,exp_alto);
        for (y=0;y<exp_alto;y++)
        {
        Progress((char *)texto[434],y,exp_alto);
        tecla(); if (key(_ESC)) { Progress((char *)texto[434],exp_alto,exp_alto); return(1); }
                for (x=0;x<exp_ancho;x++)
                {
                        exp_Coloracum=0;
                        for (m=0;m<n_exp;m++)
                        {
                                exp_Color=0;
                                for (n=0;n<n_pun;n++)
                                {
                                        dx=abs(x-e[m].p[n].x);
                                        dy=abs(y-e[m].p[n].y);
                                        if ((dx<e[m].p[n].radio/10000) && (dy<e[m].p[n].radio/10000))
                                        {
                                                dist=sqrt(dx*dx+dy*dy);
                                                if (dist<e[m].p[n].radio/10000)
                                                        exp_Color+=(((e[m].p[n].radio/10000)-dist)*(e[m].p[n].fuerza/10000))/(e[m].p[n].radio/10000);
                                        }
                                }
                                if (exp_Color>255)
                                        exp_Color=255;
                                switch(tipo_exp)
                                {
                                        case 0:
                                        case 3:
                                                exp_Coloracum+=exp_Color;
                                                break;
                                        case 1:
                                        case 2:
                                                if(m%2)
                                                        exp_Coloracum-=exp_Color;
                                                else
                                                        exp_Coloracum+=exp_Color;
                                                break;
                                }
                        }
                        switch(tipo_exp)
                        {
                                case 0:
                                case 3:
                                        Buff_exp[y*exp_ancho+x]=exp_Coloracum/n_exp;
                                        break;
                                case 1:
                                case 2:
                                        if(exp_Coloracum>255)
                                                exp_Coloracum=255;
                                        if(exp_Coloracum<0)
                                                exp_Coloracum=0;
                                        Buff_exp[y*exp_ancho+x]=exp_Coloracum;
                                                break;
                                }
                }
        }
        Progress((char *)texto[434],exp_alto,exp_alto);
        for (n=0;n<(exp_ancho*exp_alto)*per_points/100;n++)
        {
                x=(rnd()%(exp_ancho-2))+1;
                y=(rnd()%(exp_alto -2))+1;
                if(Buff_exp[y*exp_ancho+x]>DEEP*2)
                        Buff_exp[y*exp_ancho+x]-=DEEP*2;

                if(Buff_exp[y*exp_ancho+x-1]>DEEP)
                        Buff_exp[y*exp_ancho+x-1]-=DEEP;

                if(Buff_exp[y*exp_ancho+x+1]>DEEP)
                        Buff_exp[y*exp_ancho+x+1]-=DEEP;

                if(Buff_exp[(y-1)*exp_ancho+x]>DEEP)
                        Buff_exp[(y-1)*exp_ancho+x]-=DEEP;
                if(Buff_exp[(y+1)*exp_ancho+x]>DEEP)
                        Buff_exp[(y+1)*exp_ancho+x]-=DEEP;
        }
        return(0);
}

char cFrames[4];
char cexp_ancho[5];
char cexp_alto[5];
char cper_points[5];
int exp_Color0=-1,exp_Color1=-1,exp_Color2=-1;
int TipoA=1,TipoB=0,TipoC=0;
int OldTipoA=1,OldTipoB=0,OldTipoC=0;

void Explode1(void) {
  int an=v.an/big2, al=v.al/big2;
  int x;

  _show_items();

  crea_gama(exp_gama, exp_colores);

//v.an=128;
//v.al=90+5;

  wbox(v.ptr,an,al,c0,2,31,an-4,1);

//wbox(v.ptr,an,al,c0,90,12,1,44);

//wbox(v.ptr,an,al,c0,90,10,1,al-30);
  wbox(v.ptr,an,al,c0,2,74,an-4,1);

//wwrite(v.ptr,an,al,115-21,11,0,texto[308],c3);

  wwrite(v.ptr,an,al,an-70,11,0,texto[181],c3);
  wrectangulo(v.ptr,an,al,c0,an-70,18,66,11);
  for (x=0;x<64;x++) wbox(v.ptr,an,al,exp_colores[x*2],an-69+x,18+1,1,9);

/*
  wwrite(v.ptr,an,al,115-21,11,0,texto[308],c3);
  wbox(v.ptr,an,al,c0,94,19,30,16);
   wbox(v.ptr,an,al,exp_Color2,95,20,28,14);
  wbox(v.ptr,an,al,c0,94,37,30,16);
   wbox(v.ptr,an,al,exp_Color1,95,38,28,14);
  wbox(v.ptr,an,al,c0,94,55,30,16);
   wbox(v.ptr,an,al,exp_Color0,95,56,28,14);
*/
}

void Selcolor0();
extern int SelColorFont;
extern int SelColorOk;

void Explode2(void) {
  int an=v.an/big2;//, al=v.al/big2;
  int need_refresh=0;

  _process_items();
  switch(v.active_item)
  {
    case 0: fin_dialogo=1; v_aceptar=1; break;
    case 1: fin_dialogo=1; v_aceptar=0; break;
  }
  if(!(TipoA+TipoB+TipoC))
  {
        TipoA=OldTipoA;
        TipoB=OldTipoB;
        TipoC=OldTipoC;
        need_refresh=1;
  }
  if(TipoA!=OldTipoA)
  {
        TipoA=1;
        TipoB=TipoC=0;
        OldTipoB=OldTipoC=0;
        need_refresh=1;
  }
  if(TipoB!=OldTipoB)
  {
        TipoB=1;
        TipoA=TipoC=0;
        OldTipoA=OldTipoC=0;
        need_refresh=1;
  }
  if(TipoC!=OldTipoC)
  {
        TipoC=1;
        TipoA=TipoB=0;
        OldTipoA=OldTipoB=0;
        need_refresh=1;
  }
  OldTipoA=TipoA;
  OldTipoB=TipoB;
  OldTipoC=TipoC;
/*
  if((wmouse_x>=95)&&(wmouse_x<=95+28))
  {
          if((wmouse_y>=20)&&(wmouse_y<=20+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color2;
                dialogo((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color2=SelColorFont;
                        wbox(v.ptr,v.an/big2,v.al/big2,exp_Color2,95,20,28,14);
                        need_refresh=1;
                }
          }
          if((wmouse_y>=38)&&(wmouse_y<=38+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color1;
                dialogo((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color1=SelColorFont;
                        wbox(v.ptr,v.an/big2,v.al/big2,exp_Color1,95,38,28,14);
                        need_refresh=1;
                }
          }
          if((wmouse_y>=56)&&(wmouse_y<=56+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color0;
                dialogo((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color0=SelColorFont;
                        wbox(v.ptr,v.an/big2,v.al/big2,exp_Color0,95,56,28,14);
                        need_refresh=1;
                }
          }

  }
*/
  if (wmouse_in(an-70,18,66,11) && (mouse_b&1))
  {
    gama=exp_colores;
    t_gama=exp_gama;
    dialogo((memptrsize)gama0);
    if (v_aceptar) need_refresh=1;
  }
  if(need_refresh){
        call((voidReturnType )v.paint_handler);
        v.volcar=1;
  }
}

void Explode3(void)
{

        if(!v_aceptar)
                return;

        _n_frames=n_frames=atoi(cFrames);
        exp_ancho=atoi(cexp_ancho);
        exp_alto =atoi(cexp_alto);
        per_points    =atoi(cper_points);
        if(TipoA)
                tipo_exp=0;
        if(TipoB)
                tipo_exp=1;
        if(TipoC)
                tipo_exp=2;
}

void Explode0(void) {

  v.tipo=1;

  v.titulo=texto[300];
  v.an=128;
  v.al=90+5;
  v.paint_handler=(memptrsize)Explode1;
  v.click_handler=(memptrsize)Explode2;
  v.close_handler=(memptrsize)Explode3;

  _button(100,7     ,v.al-14,0);
  _button(101,v.an-8,v.al-14,2);
  TipoA=TipoB=TipoC=OldTipoA=OldTipoB=OldTipoC=0;
  if (exp_Color0==-1) exp_Color0=c4;
  if (exp_Color1==-1) exp_Color1=c2;
  if (exp_Color2==-1) exp_Color2=c0;
  switch(tipo_exp)
  {
        case 0:
                TipoA=OldTipoA=1;
                break;
        case 1:
                TipoB=OldTipoB=1;
                break;
        case 2:
                TipoC=OldTipoC=1;
                break;
  }

   sprintf(cFrames,"%d",n_frames);
   sprintf(cexp_ancho,"%d",exp_ancho);
   sprintf(cexp_alto,"%d",exp_alto);
   sprintf(cper_points,"%d",per_points);

  _get(133,4,11,21,(byte *)cexp_ancho,5,8,2000);
  _get(134,69-36,11,21,(byte *)cexp_alto ,5,8,2000);

  _flag(301,4,16+19,&TipoA);
  _flag(302,4,16+34,&TipoB);
  _flag(303,4,16+48,&TipoC);

  _get(304,69-24+30,16+19,21,(byte *)cFrames,3,1,48);
  _get(305,69-24+30,16+38,21,(byte *)cper_points ,3,0,100);

  v_aceptar=0;
}

void GenExplodes()
{
int ExpDac[256];
//float r,g,b,incr,incg,incb;
int n=0,nf,y;
int x;
        n_frames=_n_frames;
        create_dac4();

        dialogo((memptrsize)Explode0);
        if(!v_aceptar)
                return;
        for(x=0;x<256;x++) ExpDac[x] = exp_colores[x/2];
/*
        //Gamma del exp_Color2 al exp_Color1
        r=(float)dac[exp_Color2*3  ];
        g=(float)dac[exp_Color2*3+1];
        b=(float)dac[exp_Color2*3+2];
        incr=((float)dac[exp_Color1*3  ]-r)/111.0;
        incg=((float)dac[exp_Color1*3+1]-g)/111.0;
        incb=((float)dac[exp_Color1*3+2]-b)/111.0;
        create_dac4();
        for(x=0;x<16;x++)
                ExpDac[x]=find_color(0,0,0);
        for(x=16;x<128;x++)
        {
                ExpDac[x]=find_color((int)r,(int)g,(int)b);
                r+=incr;
                g+=incg;
                b+=incb;
        }

        //Gamma del exp_Color1 al exp_Color0
        r=(float)dac[exp_Color1*3  ];
        g=(float)dac[exp_Color1*3+1];
        b=(float)dac[exp_Color1*3+2];
        incr=((float)dac[exp_Color0*3  ]-r)/127.0;
        incg=((float)dac[exp_Color0*3+1]-g)/127.0;
        incb=((float)dac[exp_Color0*3+2]-b)/127.0;
        for(x=128;x<256;x++)
        {
                ExpDac[x]=find_color((int)r,(int)g,(int)b);
                r+=incr;
                g+=incg;
                b+=incb;
        }
*/
  init_rnd(*system_clock,NULL);
  Buff_exp=(char *)malloc(exp_ancho*exp_alto);
  n_pun=32;
  switch(tipo_exp)
  {
        case 0:
                n_exp=4;
                break;
        case 1:
                n_exp=3;
                break;
        case 2:
                n_exp=5;
                break;
        case 3:
                n_exp=1;
                break;
  }

  cx=exp_ancho/2;
  cy=exp_alto/2;
  nf=n_frames;
  crear_puntos();
  n_frames*=2;
  if(exp_ancho<exp_alto)
        paso_frame=(exp_ancho*10000)/n_frames;
  else
        paso_frame=(exp_alto*10000)/n_frames;
  do {
    if (pintar_explosion()) break;

    map_an=exp_ancho;
    map_al=exp_alto;

    if (nuevo_mapa(NULL)) break;

    for(x=0;x<map_an*map_al;x++)
        v.mapa->map[x]=ExpDac[Buff_exp[x]];

    v.mapa->zoom_cx=v.mapa->map_an/2;
    v.mapa->zoom_cy=v.mapa->map_al/2;

    x=(v.mapa->zoom_cx-vga_an/2);
    if (x<0)
        x=0;
    else
        if (x+vga_an>v.mapa->map_an)
          x=v.mapa->map_an-vga_an;
    y=(v.mapa->zoom_cy-vga_al/2);
    if (y<0)
                y=0;
        else
                if(y+vga_al>v.mapa->map_al)
                        y=v.mapa->map_al-vga_al;
    v.mapa->zoom=0;
    v.mapa->zoom_x=x;
    v.mapa->zoom_y=y;

    for (n=0;n<512;n++)
        v.mapa->puntos[n]=-1;
    v.mapa->Codigo=0;
    call((voidReturnType )v.paint_handler);
    wvolcado(copia,vga_an,vga_al,v.ptr,v.x,v.y,v.an,v.al,0);

    avanzar_puntos();
    n_frames-=2;
  } while (n_frames);

  free(Buff_exp);
}
