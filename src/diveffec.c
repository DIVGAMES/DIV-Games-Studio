//═════════════════════════════════════════════════════════════════════════════
//      Module for creating explosions
//═════════════════════════════════════════════════════════════════════════════

#include "global.h"

//═════════════════════════════════════════════════════════════════════════════
//      Definitions
//═════════════════════════════════════════════════════════════════════════════

#define max_pun 64      // Maximum number of control points
#define max_exp 8       // Maximum number of base explosions
#define DEEP    4
#pragma on (check_stack);

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef byte
#define byte unsigned char
#endif

#ifndef ushort
#define ushort unsigned short
#endif

#ifndef word
#define word unsigned short
#endif

//═════════════════════════════════════════════════════════════════════════════
//      Declaration of internal procedures
//═════════════════════════════════════════════════════════════════════════════

void init_rnd(int,char*); // Initialize the random generator with an int and a key
byte rndb(void);	    	  // Get a byte from the random generator
int  rnd(void); 		      // Get an int from the random generator

void gradient0(void);
byte exp_colors[128];
struct _gcolor exp_gradient[9];

//═════════════════════════════════════════════════════════════════════════════
//      Global tables and variables
//═════════════════════════════════════════════════════════════════════════════

int cx,cy;
int ExpGradient;

struct tp {			// Structure for the explosion points
  int x,y;                      // Point coordinates
  int radius;                    // Effective radius of the point
  int force;                   // Central force of the point
  int xr,yr;                    // Coord. * 10000
  int ix,iy;                    // Increments
};

struct te {                     // Structure for a base explosion
  struct tp p[max_pun];
  byte * ptr;
} e[max_exp];

//═════════════════════════════════════════════════════════════════════════════
//      Parameters to create the explosion
//═════════════════════════════════════════════════════════════════════════════

int exp_width=32;      // 8..256
int exp_height=32;       // 8..256
int n_pun;      // 16..64
int n_exp;      // 1..8
int n_frames;   // 1..n
int _n_frames=6;   // 1..n
int step_frame;
int exp_type=1;
int per_points;
char *Buff_exp;

//═════════════════════════════════════════════════════════════════════════════
//	Create the array of points
//═════════════════════════════════════════════════════════════════════════════

void create_points(void) {
  int m,n,rx,ry;
  double ang,dist;

  for (m=0;m<n_exp;m++) {

    for (n=0;n<n_pun;n++) {

      ang=(double)rnd()/10000.0; // Angle (?)
      dist=(double)(rnd()%10000)/10000.0; // Distance (0 .. 0.8)

      e[m].p[n].ix=cos(ang)*10000.0;
      e[m].p[n].iy=sin(ang)*10000.0;

      e[m].p[n].x=cx+(int)(cos(ang)*dist*(float)(exp_width/2));
      e[m].p[n].y=cy+(int)(sin(ang)*dist*(float)(exp_height/2));

      e[m].p[n].xr=e[m].p[n].x*10000;
      e[m].p[n].yr=e[m].p[n].y*10000;

      if (e[m].p[n].x<cx) rx=e[m].p[n].x; else rx=exp_width-e[m].p[n].x;
      if (e[m].p[n].y<cy) ry=e[m].p[n].y; else ry=exp_height-e[m].p[n].y;
      if (rx<ry) e[m].p[n].radius=rx*10000; else e[m].p[n].radius=ry*10000;

      e[m].p[n].force=e[m].p[n].radius+rnd()%(e[m].p[n].radius)*4;
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//      Advance one frame in the explosion
//═════════════════════════════════════════════════════════════════════════════

void advance_points(void) {
  int m,n;
  for (m=0;m<n_exp;m++) {
        for (n=0;n<n_pun;n++)
        {
                e[m].p[n].xr+=(e[m].p[n].ix*step_frame)/10000;
                e[m].p[n].yr+=(e[m].p[n].iy*step_frame)/10000;
                e[m].p[n].x=e[m].p[n].xr/10000;
                e[m].p[n].y=e[m].p[n].yr/10000;
                if (e[m].p[n].force>e[m].p[n].radius)
                        e[m].p[n].force=(int)((float)e[m].p[n].force*(float)0.86);//0.98
                e[m].p[n].radius-=step_frame;
        }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//	Draw the explosion
//═════════════════════════════════════════════════════════════════════════════

int draw_explosion(void) {
  int m,x,y,dx,dy,n;
  int dist,exp_Color,exp_Coloraccum;

        Progress((char *)texts[434],0,exp_height);
        for (y=0;y<exp_height;y++)
        {
        Progress((char *)texts[434],y,exp_height);
        read_key(); if (key(_ESC)) { Progress((char *)texts[434],exp_height,exp_height); return(1); }
                for (x=0;x<exp_width;x++)
                {
                        exp_Coloraccum=0;
                        for (m=0;m<n_exp;m++)
                        {
                                exp_Color=0;
                                for (n=0;n<n_pun;n++)
                                {
                                        dx=abs(x-e[m].p[n].x);
                                        dy=abs(y-e[m].p[n].y);
                                        if ((dx<e[m].p[n].radius/10000) && (dy<e[m].p[n].radius/10000))
                                        {
                                                dist=sqrt(dx*dx+dy*dy);
                                                if (dist<e[m].p[n].radius/10000)
                                                        exp_Color+=(((e[m].p[n].radius/10000)-dist)*(e[m].p[n].force/10000))/(e[m].p[n].radius/10000);
                                        }
                                }
                                if (exp_Color>255)
                                        exp_Color=255;
                                switch(exp_type)
                                {
                                        case 0:
                                        case 3:
                                                exp_Coloraccum+=exp_Color;
                                                break;
                                        case 1:
                                        case 2:
                                                if(m%2)
                                                        exp_Coloraccum-=exp_Color;
                                                else
                                                        exp_Coloraccum+=exp_Color;
                                                break;
                                }
                        }
                        switch(exp_type)
                        {
                                case 0:
                                case 3:
                                        Buff_exp[y*exp_width+x]=exp_Coloraccum/n_exp;
                                        break;
                                case 1:
                                case 2:
                                        if(exp_Coloraccum>255)
                                                exp_Coloraccum=255;
                                        if(exp_Coloraccum<0)
                                                exp_Coloraccum=0;
                                        Buff_exp[y*exp_width+x]=exp_Coloraccum;
                                                break;
                                }
                }
        }
        Progress((char *)texts[434],exp_height,exp_height);
        for (n=0;n<(exp_width*exp_height)*per_points/100;n++)
        {
                x=(rnd()%(exp_width-2))+1;
                y=(rnd()%(exp_height -2))+1;
                if(Buff_exp[y*exp_width+x]>DEEP*2)
                        Buff_exp[y*exp_width+x]-=DEEP*2;

                if(Buff_exp[y*exp_width+x-1]>DEEP)
                        Buff_exp[y*exp_width+x-1]-=DEEP;

                if(Buff_exp[y*exp_width+x+1]>DEEP)
                        Buff_exp[y*exp_width+x+1]-=DEEP;

                if(Buff_exp[(y-1)*exp_width+x]>DEEP)
                        Buff_exp[(y-1)*exp_width+x]-=DEEP;
                if(Buff_exp[(y+1)*exp_width+x]>DEEP)
                        Buff_exp[(y+1)*exp_width+x]-=DEEP;
        }
        return(0);
}

char cFrames[4];
char cexp_width[5];
char cexp_height[5];
char cper_points[5];
int exp_Color0=-1,exp_Color1=-1,exp_Color2=-1;
int TypeA=1,TypeB=0,TypeC=0;
int OldTypeA=1,OldTypeB=0,OldTypeC=0;

void Explode1(void) {
  int w=v.w/big2, h=v.h/big2;
  int x;

  _show_items();

  create_gradient(exp_gradient, exp_colors);

//v.w=128;
//v.h=90+5;

  wbox(v.ptr,w,h,c0,2,31,w-4,1);

//wbox(v.ptr,w,h,c0,90,12,1,44);

//wbox(v.ptr,w,h,c0,90,10,1,h-30);
  wbox(v.ptr,w,h,c0,2,74,w-4,1);

//wwrite(v.ptr,w,h,115-21,11,0,texts[308],c3);

  wwrite(v.ptr,w,h,w-70,11,0,texts[181],c3);
  wrectangle(v.ptr,w,h,c0,w-70,18,66,11);
  for (x=0;x<64;x++) wbox(v.ptr,w,h,exp_colors[x*2],w-69+x,18+1,1,9);

/*
  wwrite(v.ptr,w,h,115-21,11,0,texts[308],c3);
  wbox(v.ptr,w,h,c0,94,19,30,16);
   wbox(v.ptr,w,h,exp_Color2,95,20,28,14);
  wbox(v.ptr,w,h,c0,94,37,30,16);
   wbox(v.ptr,w,h,exp_Color1,95,38,28,14);
  wbox(v.ptr,w,h,c0,94,55,30,16);
   wbox(v.ptr,w,h,exp_Color0,95,56,28,14);
*/
}

void Selcolor0();
extern int SelColorFont;
extern int SelColorOk;

void Explode2(void) {
  int w=v.w/big2;//, al=v.al/big2;
  int need_refresh=0;

  _process_items();
  switch(v.active_item)
  {
    case 0: end_dialog=1; v_accept=1; break;
    case 1: end_dialog=1; v_accept=0; break;
  }
  if(!(TypeA+TypeB+TypeC))
  {
        TypeA=OldTypeA;
        TypeB=OldTypeB;
        TypeC=OldTypeC;
        need_refresh=1;
  }
  if(TypeA!=OldTypeA)
  {
        TypeA=1;
        TypeB=TypeC=0;
        OldTypeB=OldTypeC=0;
        need_refresh=1;
  }
  if(TypeB!=OldTypeB)
  {
        TypeB=1;
        TypeA=TypeC=0;
        OldTypeA=OldTypeC=0;
        need_refresh=1;
  }
  if(TypeC!=OldTypeC)
  {
        TypeC=1;
        TypeA=TypeB=0;
        OldTypeA=OldTypeB=0;
        need_refresh=1;
  }
  OldTypeA=TypeA;
  OldTypeB=TypeB;
  OldTypeC=TypeC;
/*
  if((wmouse_x>=95)&&(wmouse_x<=95+28))
  {
          if((wmouse_y>=20)&&(wmouse_y<=20+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color2;
                dialog((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color2=SelColorFont;
                        wbox(v.ptr,v.w/big2,v.h/big2,exp_Color2,95,20,28,14);
                        need_refresh=1;
                }
          }
          if((wmouse_y>=38)&&(wmouse_y<=38+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color1;
                dialog((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color1=SelColorFont;
                        wbox(v.ptr,v.w/big2,v.h/big2,exp_Color1,95,38,28,14);
                        need_refresh=1;
                }
          }
          if((wmouse_y>=56)&&(wmouse_y<=56+14)&&(mouse_b))
          {
                _reselect_item();
                SelColorFont=exp_Color0;
                dialog((int)Selcolor0);
                if(SelColorOk)
                {
                        exp_Color0=SelColorFont;
                        wbox(v.ptr,v.w/big2,v.h/big2,exp_Color0,95,56,28,14);
                        need_refresh=1;
                }
          }

  }
*/
  if (wmouse_in(w-70,18,66,11) && (mouse_b&1))
  {
    gradient_buf=exp_colors;
    t_gradient=exp_gradient;
    dialog((voidReturnType)gradient0);
    if (v_accept) need_refresh=1;
  }
  if(need_refresh){
        call((voidReturnType )v.paint_handler);
        v.blit=1;
  }
}

void Explode3(void)
{

        if(!v_accept)
                return;

        _n_frames=n_frames=atoi(cFrames);
        exp_width=atoi(cexp_width);
        exp_height =atoi(cexp_height);
        per_points    =atoi(cper_points);
        if(TypeA)
                exp_type=0;
        if(TypeB)
                exp_type=1;
        if(TypeC)
                exp_type=2;
}

void Explode0(void) {

  v.type=1;

  v.title=texts[300];
  v.w=128;
  v.h=90+5;
  v.paint_handler=(voidReturnType)Explode1;
  v.click_handler=(voidReturnType)Explode2;
  v.close_handler=(voidReturnType)Explode3;

  _button(100,7     ,v.h-14,0);
  _button(101,v.w-8,v.h-14,2);
  TypeA=TypeB=TypeC=OldTypeA=OldTypeB=OldTypeC=0;
  if (exp_Color0==-1) exp_Color0=c4;
  if (exp_Color1==-1) exp_Color1=c2;
  if (exp_Color2==-1) exp_Color2=c0;
  switch(exp_type)
  {
        case 0:
                TypeA=OldTypeA=1;
                break;
        case 1:
                TypeB=OldTypeB=1;
                break;
        case 2:
                TypeC=OldTypeC=1;
                break;
  }

   sprintf(cFrames,"%d",n_frames);
   sprintf(cexp_width,"%d",exp_width);
   sprintf(cexp_height,"%d",exp_height);
   sprintf(cper_points,"%d",per_points);

  _get(133,4,11,21,(byte *)cexp_width,5,8,2000);
  _get(134,69-36,11,21,(byte *)cexp_height ,5,8,2000);

  _flag(301,4,16+19,&TypeA);
  _flag(302,4,16+34,&TypeB);
  _flag(303,4,16+48,&TypeC);

  _get(304,69-24+30,16+19,21,(byte *)cFrames,3,1,48);
  _get(305,69-24+30,16+38,21,(byte *)cper_points ,3,0,100);

  v_accept=0;
}

void GenExplodes()
{
int ExpDac[256];
//float r,g,b,incr,incg,incb;
int n=0,nf,y;
int x;
        n_frames=_n_frames;
        create_dac4();

        dialog((voidReturnType)Explode0);
        if(!v_accept)
                return;
        for(x=0;x<256;x++) ExpDac[x] = exp_colors[x/2];
/*
        //Gradient from exp_Color2 to exp_Color1
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

        //Gradient from exp_Color1 to exp_Color0
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
  Buff_exp=(char *)malloc(exp_width*exp_height);
  n_pun=32;
  switch(exp_type)
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

  cx=exp_width/2;
  cy=exp_height/2;
  nf=n_frames;
  create_points();
  n_frames*=2;
  if(exp_width<exp_height)
        step_frame=(exp_width*10000)/n_frames;
  else
        step_frame=(exp_height*10000)/n_frames;
  do {
    if (draw_explosion()) break;

    map_width=exp_width;
    map_height=exp_height;

    if (new_map(NULL)) break;

    for(x=0;x<map_width*map_height;x++)
        v.map->map[x]=ExpDac[Buff_exp[x]];

    v.map->zoom_cx=v.map->map_width/2;
    v.map->zoom_cy=v.map->map_height/2;

    x=(v.map->zoom_cx-vga_width/2);
    if (x<0)
        x=0;
    else
        if (x+vga_width>v.map->map_width)
          x=v.map->map_width-vga_width;
    y=(v.map->zoom_cy-vga_height/2);
    if (y<0)
                y=0;
        else
                if(y+vga_height>v.map->map_height)
                        y=v.map->map_height-vga_height;
    v.map->zoom=0;
    v.map->zoom_x=x;
    v.map->zoom_y=y;

    for (n=0;n<512;n++)
        v.map->points[n]=-1;
    v.map->Code=0;
    call((voidReturnType )v.paint_handler);
    wblit(copy_buffer,vga_width,vga_height,v.ptr,v.x,v.y,v.w,v.h,0);

    advance_points();
    n_frames-=2;
  } while (n_frames);

  free(Buff_exp);
}
