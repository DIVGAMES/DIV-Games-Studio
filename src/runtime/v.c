
///////////////////////////////////////////////////////////////////////////////
// Video functions
///////////////////////////////////////////////////////////////////////////////

#include "inter.h"
//#include "..\inc\svga.h"
//#include "..\inc\vesa.h"

///////////////////////////////////////////////////////////////////////////////
//	Declarations and module-level data
///////////////////////////////////////////////////////////////////////////////

#define CRTC_INDEX      0x3d4   //CRT Controller Index
#define CRTC_OFFSET     0x13    //CRTC offset register index
#define SC_INDEX        0x3c4   //Sequence Controller Index
#define MISC_OUTPUT     0x3c2   //Miscellaneous Output register

#ifdef DOS
byte * vga = (byte *) 0xA0000; // Physical screen 
                               // TODO - (change this to SDL surface pixels)
#else
SDL_Surface *vga=NULL;
#endif

void snapshot(byte *p);
void volcadocsvga(byte *p);
void volcadoc320200(byte *p);
void volcadocx(byte * p);
void volcadopsvga(byte *p);
void volcadop320200(byte *p);
void volcadopx(byte * p);
int graba_PCX(byte *mapa,int an,int al,FILE *f);
void crear_ghost_vc(int m);
void crear_ghost_slow(void);






#define MAX_YRES 2048

short scan[MAX_YRES*4]; // Por scan [x,an,x,an] se definen hasta 2 segmentos a volcar

struct {
  byte dot;
  int crt[20];
} modox[5]={

  {0xe3,0x0d06,0x3e07,0x4109,0xea10,0xac11,0xdf12,0x0014,0xe715, //320x240
   0x0616,0xe317,0},

  {0xe3,0x4009,0x0014,0xe317,0}, //320x400

  {0xe7,0x6b00,0x5901,0x5a02,0x8e03,0x5e04,0x8a05,0x0d06,0x3e07, //360x240
   0x4109,0xea10,0xac11,0xdf12,0x2d13,0x0014,0xe715,0x0616,0xe317,0},

  {0xe7,0x6b00,0x5901,0x5a02,0x8e03,0x5e04,0x8a05,0x4009,0x8810, //360x360
   0x8511,0x6712,0x2d13,0x0014,0x6d15,0xba16,0xe317,0},

  {0xe7,0x6e00,0x5d01,0x5e02,0x9103,0x6204,0x8f05,0x6206,0xf007, //376x282
   0x6109,0x310f,0x3710,0x8911,0x3312,0x2f13,0x0014,0x3c15,0x5c16,0xe317,0}
};

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Tabla ghost
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

struct t_tpuntos { // Para la creaciขn de la tabla ghost
  int r,g,b;
  struct t_tpuntos * next;
} tpuntos[256];

struct t_tpuntos * vcubos[512]; // Para la creaciขn de la tabla ghost

extern int fli_palette_update;

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
// Funciขn para poner la paleta
//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

byte color_oscuro;

void set_paleta (void) {
  word n;

  n=abs(dacout_speed); // if (n>64) n=64;

  if (now_dacout_r<dacout_r) {
    if (now_dacout_r+n<dacout_r) now_dacout_r+=n; else now_dacout_r=dacout_r;
  } else if (now_dacout_r>dacout_r) {
    if (now_dacout_r-n>dacout_r) now_dacout_r-=n; else now_dacout_r=dacout_r;
  }

  if (now_dacout_g<dacout_g) {
    if (now_dacout_g+n<dacout_g) now_dacout_g+=n; else now_dacout_g=dacout_g;
  } else if (now_dacout_g>dacout_g) {
    if (now_dacout_g-n>dacout_g) now_dacout_g-=n; else now_dacout_g=dacout_g;
  }

  if (now_dacout_b<dacout_b) {
    if (now_dacout_b+n<dacout_b) now_dacout_b+=n; else now_dacout_b=dacout_b;
  } else if (now_dacout_b>dacout_b) {
    if (now_dacout_b-n>dacout_b) now_dacout_b-=n; else now_dacout_b=dacout_b;
  }

  n=0; do {
    if (now_dacout_r>paleta[n]) dac[n]=0; else dac[n]=paleta[n]-now_dacout_r;
    if (dac[n]>63) dac[n]=63; n++;
    if (now_dacout_g>paleta[n]) dac[n]=0; else dac[n]=paleta[n]-now_dacout_g;
    if (dac[n]>63) dac[n]=63; n++;
    if (now_dacout_b>paleta[n]) dac[n]=0; else dac[n]=paleta[n]-now_dacout_b;
    if (dac[n]>63) dac[n]=63; n++;
  } while (n<768);

  color_oscuro=0;

  if (process_active_palette!=NULL) process_active_palette();
}

void set_dac (void) {
if(!vga) return;
#ifndef DOS
	SDL_Color colors[256];
	int i;
	int b=0;
	for(i=0;i<256;i++){
          colors[i].r=dac[b]*4;
          colors[i].g=dac[b+1]*4;
          colors[i].b=dac[b+2]*4;
          b+=3;
    }
	if(!SDL_SetPalette(vga, SDL_LOGPAL|SDL_PHYSPAL, colors, 0, 256)) 
		printf("Failed to set palette :(\n"); 
	
	retrazo();
#else
  union REGS regs;
  word n=0;
  if (fli_palette_update) return;
  //retrazo();
  outp(0x3c8,0);
  do {
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
  } while (n<768);
  regs.w.ax=0x1001;
  regs.h.bh=color_oscuro;
  int386(0x010,&regs,&regs);
#endif
}

void set_dac2 (void) {
#ifdef DOS
  int n=0;
  outp(0x3c8,0);
  do {
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
  } while (n<768);
#endif
}

void retrazo (void) {
#ifdef DOS
  while (inp(0x3da)&8);
  while ((inp(0x3da)&8)==0);
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set Video Mode (vga_an y vga_al se definen en shared.h)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int LinealMode;
int modovesa;

extern float m_x,m_y;

void svmode(void) {
#ifdef STDOUTLOG
printf("setting new video mode %d %d %x\n",vga_an,vga_al,vga);
#endif
#ifdef __EMSCRIPTEN__
//	if(vga)
//	SDL_FreeSurface(vga);
//SDL_Quit();
//SDL_Init(SDL_INIT_VIDEO);
if(!vga)	
	vga=SDL_SetVideoMode(vga_an, vga_al, 8, 0);
#else
	vga=SDL_SetVideoMode(vga_an, vga_al, 8, 0);
#endif

#ifdef STDOUTLOG
	printf("SET VIDEO MODE %x\n",vga);
#endif
	SDL_WM_SetCaption( "DIV2015", "" );

	modovesa=1;
#ifdef DOS
  VBESCREEN Screen;

  int mode=0;
  int error=0,n;

  LinealMode=0;
  modovesa=0;

  // Comprueba primero si es un modo vesa

  for (n=0;n<num_video_modes;n++) {
    if (vga_an==video_modes[n].ancho && vga_al==video_modes[n].alto) {
      if (video_modes[n].modo && video_modes[n].modo<256000) { mode=video_modes[n].modo; break; }
    }
  }

  if (n<num_video_modes) {
    modovesa=1;
    if(VersionVesa<0x102) {
      if (!VBE_setVideoMode(mode)) error=1;
      else vga=(char *)0x0A0000;
    } else {
      if(VersionVesa<0x200) {
        if(!SV_setMode(mode)) error=1;
        else vga=(char *)0x0A0000;
      } else {
       	if (vbeSetMode (vga_an, vga_al, 8, &Screen) == 4) {
          LinealMode=1;
          mode|=vbeLinearBuffer;
          if(!SV_setMode(mode)) {
            LinealMode=0;
            mode^=vbeLinearBuffer;
            if(!SV_setMode(mode)) error=1;
            else vga=(char *)videoMem;
          } else vga=(char *)videoMem;
        } else {
          LinealMode=1;
          vga=Screen.adr;
        }
      }
    }
  } else switch(vga_an*1000+vga_al) {
    case 320200: _setvideomode(_MRES256COLOR); break;
    case 320240: svmodex(0); break;
    case 320400: svmodex(1); break;
    case 360240: svmodex(2); break;
    case 360360: svmodex(3); break;
    case 376282: svmodex(4); break;
    default: error=1; break;
  }

  // OJO!, esto provoca que, en equipos sin VESA, se vea en "320x200 BIG"
  
  if (error) {
    modovesa=0;
    vga_an=320; vga_al=200; _setvideomode(_MRES256COLOR);
  }
#endif

  m_x=(float)vga_an/2.0;
  m_y=(float)vga_al/2.0;

  if (demo) {
    texto[max_textos].tipo=0;
    texto[max_textos].centro=4;
    texto[max_textos].y=vga_al/2;
    texto[max_textos].x=vga_an/2;
    texto[max_textos].font=(byte*)fonts[0];
  } else texto[max_textos].font=0;

}

void svmodex(int m) {
#ifdef DOS
  int n=0;

  _setvideomode(_MRES256COLOR);

  outpw(SC_INDEX,0x604); //disable chain4 mode
  outpw(SC_INDEX,0x100);
  outp(MISC_OUTPUT,modox[m].dot);
  outpw(SC_INDEX,0x300);
  outp(CRTC_INDEX,0x11);
  outpw(CRTC_INDEX+1,inp(CRTC_INDEX+1)&0x7f);

  while (modox[m].crt[n]) outpw(CRTC_INDEX,modox[m].crt[n++]);

  outpw(SC_INDEX,0x0f02);
  memset(vga,0,65536);

  outp(CRTC_INDEX,CRTC_OFFSET);
  outp(CRTC_INDEX+1,vga_an/8);
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Reset Video Mode
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void rvmode(void) {
#ifdef DOS
  SV_restoreMode();
  _setvideomode(3);
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dump buffer to VGA
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ


void volcadosdl(byte *p) {
	if(!vga) {
		printf("setting up screen for first time %d %d\n",vga_an,vga_al);
		svmode();
		set_dac(); // tabla_ghost();
	}
			
	SDL_LockSurface(vga);
	byte *q = (byte *)vga->pixels;
	int vy=0;
	for (vy=0; vy<vga_al;vy++) {
		memcpy(q,p,vga_an);
		p+=vga_an;
		q+=vga->pitch;//vga_an;//*vga->pitch*vga->format->BytesPerPixel;
	}
//	printf("draw screen\n");
SDL_UnlockSurface(vga);
	SDL_Flip(vga);
}

long lasttick = 0;
long newtick =0;
long nexttick = 0;
extern int game_fps;
void volcado(byte *p) {
//printf("%d %d %d\n",game_fps,freloj,ireloj);//,reloj);
  if ((shift_status&4) && (shift_status&8) && key(_P)) {
    snapshot(p);
    do {} while(key(_P));
  }

  if (fli_palette_update) retrazo();

volcadosdl(p);

newtick = SDL_GetTicks();
nexttick = lasttick + (1000/game_fps);

//printf("%d %d\n",newtick,nexttick);

#ifndef __EMSCRIPTEN__
if(newtick<nexttick)
	SDL_Delay(nexttick-SDL_GetTicks());
#endif

lasttick=SDL_GetTicks();

#ifdef NOTYET

  if (volcado_completo) {
    if (modovesa) volcadocsvga(p);
    else switch(vga_an*1000+vga_al) {
      case 320200: volcadoc320200(p); break;
      case 320240: volcadocx(p); break;
      case 320400: volcadocx(p); break;
      case 360240: volcadocx(p); break;
      case 360360: volcadocx(p); break;
      case 376282: volcadocx(p); break;
    }
  } else {
    if (modovesa) volcadopsvga(p);
    else switch(vga_an*1000+vga_al) {
      case 320200: volcadop320200(p); break;
      case 320240: volcadopx(p); break;
      case 320400: volcadopx(p); break;
      case 360240: volcadopx(p); break;
      case 360360: volcadopx(p); break;
      case 376282: volcadopx(p); break;
    }
  }
#endif
  if (fli_palette_update) { fli_palette_update=0; set_dac2(); }
  init_volcado();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Snapshot
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void snapshot(byte *p) {
  FILE * f;
  int n=0;
  char cwork[128];

  do {
    sprintf(cwork,"SNAP%04d.PCX",n++);
    if ((f=fopen(cwork,"rb"))!=NULL) fclose(f);
  } while (f!=NULL);

  f=fopen(cwork,"wb");
  graba_PCX(p,vga_an,vga_al,f);
  fclose(f);
}

typedef struct _pcx_header {
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  short  xmin,ymin;
  short  xmax,ymax;
  short  hres;
  short  vres;
  char   palette16[48];
  char   reserved;
  char   color_planes;
  short  bytes_per_line;
  short  palette_type;
  short  Hresol;
  short  Vresol;
  char  filler[54];
}pcx_header;

struct pcx_struct {
  pcx_header header;
  unsigned char far *cimage;
  unsigned char palette[3*256];
  unsigned char far *image;
  int clength;
};

int graba_PCX(byte *mapa,int an,int al,FILE *f) {
  byte p[768];
  int x;
  byte *cbuffer;
  struct pcx_struct pcx;
  int ptr=0;
  int cptr=0;
  int Desborde=0;
  char ActPixel;
  char cntPixel=0;
  char Paletilla=12;

        pcx.header.manufacturer=10;
        pcx.header.version=5;
        pcx.header.encoding=1;
        pcx.header.bits_per_pixel=8;
        pcx.header.xmin=0;
        pcx.header.ymin=0;
        pcx.header.xmax=an-1;
        pcx.header.ymax=al-1;
        pcx.header.hres=an;
        pcx.header.vres=al;
        pcx.header.color_planes=1;
        pcx.header.bytes_per_line=an;
        pcx.header.palette_type=0;
        pcx.header.Hresol=an;
        pcx.header.Vresol=al;

        if ((cbuffer=(unsigned char *)malloc(an*al*2))==NULL) return(1);

        ActPixel=mapa[ptr];
        while (ptr < an*al)
        {
                while((mapa[ptr]==ActPixel) && (ptr<an*al))
                {
                        cntPixel++;
                        Desborde++;
                        ptr++;
                        if(Desborde==an)
                        {
                                Desborde=0;
                                break;
                        }
                        if(cntPixel==63)
                                break;
                }
                if(cntPixel==1)
                {
                        if(ActPixel>63)
                                cbuffer[cptr++] = 193;
                        cbuffer[cptr++] = ActPixel;
                }
                else
                {
                        cbuffer[cptr++] = 192+cntPixel;
                        cbuffer[cptr++] = ActPixel;
                }
                ActPixel=mapa[ptr];
                cntPixel=0;
        }

        fwrite(&pcx.header,1,sizeof(pcx_header),f);
        fwrite(cbuffer,1,cptr,f);
        fwrite(&Paletilla,1,1,f);
        for (x=0;x<768;x++) p[x]=paleta[x]*4;
        fwrite(p,1,768,f);
        free(cbuffer);
        return(0);
}

int graba_MAP (byte * mapa, int an, int al, FILE * f) {
  int y;
  char cwork[32]="";
  char reglas[576];

  fwrite("map\x1a\x0d\x0a\x00\x00",8,1,f);      // +000 Cabecera y version
  fwrite(&an,2,1,f);                   // +008 Ancho
  fwrite(&al,2,1,f);                   // +010 Alto
  y=1; fwrite(&y,4,1,f);// +012 Cขdigo

  fwrite(cwork,32,1,f);// +016 Descripcion
  fwrite(paleta,768,1,f);                          // +048 Paleta

  for (y=0;y<16;y++) {
    reglas[y*36]=16;
    reglas[y*36+1]=0;
    reglas[y*36+2]=0;
    reglas[y*36+3]=0;
    memset(&reglas[y*36+4],y*16,32);
  } fwrite(reglas,1,sizeof(reglas),f);            // +816 Reglas de color

  y=0; fwrite(&y,2,1,f);                     // +1392 Numero de puntos
  fwrite(mapa,an*al,1,f);
  return(0);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Restauraciขn parcial del fondo a la copia
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

//int scan0[MAX_YRES]; // indica para cada scan, pixel inicial (vga_an si n/a)
//int scan1[MAX_YRES]; // indica para cada scan, pixel final (0 si n/a)

//short scan[MAX_YRES*4]; // Por scan [x,an,x,an] se definen hasta 2 segmentos a volcar

void restore(byte *q, byte *p) {
  int y=0,n=0;
  if (vga_an<640 && vga_al>200) { // Modo-X
    while (y<vga_al) {
      n=y*4;
      if (scan[n+1]) memcpy(q+scan[n]*4,p+scan[n]*4,scan[n+1]*4);
      if (scan[n+3]) memcpy(q+scan[n+2]*4,p+scan[n+2]*4,scan[n+3]*4);
      q+=vga_an; p+=vga_an; y++;
    }
  } else {
    while (y<vga_al) {
      n=y*4;
      if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
      if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
      q+=vga_an; p+=vga_an; y++;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado en el modo 320x200
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadop320200(byte *p) {
  int y=0,n;
  byte * q=(byte *)vga->pixels;

  #ifdef GRABADORA
  RegScreen(p);
  #endif

  while (y<vga_al) {
    n=y*4;
    if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
    if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
    q+=vga_an; p+=vga_an; y++;
  }
}

void volcadoc320200(byte *p) {
  #ifdef GRABADORA
  RegScreen(p);
  #endif
  memcpy(vga,p,vga_an*vga_al);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado en SVGA
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadopsvga(byte *p) {
#ifdef DOS
  int y=0,page,old_page=-1751,point,t1,t2,n;
  char *q=vga;

  if(LinealMode) {
   while (y<vga_al) {
     n=y*4;
     if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
     if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
     q+=vga_an; p+=vga_an; y++;
   }
  } else while (y<vga_al) {
    n=y*4;
    if (scan[n+1]) {
      page=(y*vga_an+scan[n])/65536;
      point=(y*vga_an+scan[n])%65536;
      if (point+scan[n+1]>65536) {
        t1=65536-point;
        t2=scan[n+1]-t1;
        if (page!=old_page) SV_setBank((signed long)page);
        memcpy(vga+point,p+scan[n],t1);
        SV_setBank((signed long)page+1); old_page=page+1;
        memcpy(vga,p+scan[n]+t1,t2);
      } else {
        if (page!=old_page) SV_setBank((signed long)(old_page=page));
        memcpy(vga+point,p+scan[n],scan[n+1]);
      }
    }
    if (scan[n+3]) {
      page=(y*vga_an+scan[n+2])/65536;
      point=(y*vga_an+scan[n+2])%65536;
      if (point+scan[n+3]>65536) {
        t1=65536-point;
        t2=scan[n+3]-t1;
        if (page!=old_page) SV_setBank((signed long)page);
        memcpy(vga+point,p+scan[n+2],t1);
        SV_setBank((signed long)page+1); old_page=page+1;
        memcpy(vga,p+scan[n+2]+t1,t2);
      } else {
        if (page!=old_page) SV_setBank((signed long)(old_page=page));
        memcpy(vga+point,p+scan[n+2],scan[n+3]);
      }
    } p+=vga_an; y++;
  }
#endif

}

void volcadocsvga(byte *p) {
#ifdef DOS
  int cnt=vga_an*vga_al;
  int tpv=0,ActPge=0;

  if(LinealMode) memcpy(vga,p,cnt);
  else while(cnt>0) {
    SV_setBank((signed long)ActPge++);
    tpv=cnt>65536?65536:cnt;
    memcpy(vga,p,tpv);
    p+=tpv;
    cnt-=tpv;
  }
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado en un modo-x
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadopx(byte * p) {
#ifdef DOS
  int n,m=(vga_an*vga_al)/4,plano=0x100,y;
  byte * v2, * p2;

  do { v2=vga+m; y=0; p2=p++; outpw(SC_INDEX,2+plano); plano<<=1;
    while (y<vga_al) {
      n=y*4;
      if (scan[n+1]) vgacpy(v2+scan[n],p2+scan[n]*4,scan[n+1]);
      if (scan[n+3]) vgacpy(v2+scan[n+2],p2+scan[n+2]*4,scan[n+3]);
      v2+=vga_an/4; p2+=vga_an; y++; }
  } while (plano<=0x800);

  outpw(SC_INDEX,0xF02); outp(0x3CE,5); outp(0x3CF,(inp(0x3CF)&252)+1);
  y=0; v2=vga; while (y<vga_al) {
    n=y*4;
    if (scan[n+1]) memcpyb(v2+scan[n],v2+scan[n]+m,scan[n+1]);
    if (scan[n+3]) memcpyb(v2+scan[n+2],v2+scan[n+2]+m,scan[n+3]);
    v2+=vga_an/4; y++;
  } outp(0x3CE,5); outp(0x3CF,inp(0x3CF)&252);

#endif
}

void volcadocx(byte * p) {
#ifdef DOS
  int n=(vga_an*vga_al)/4;

  outpw(SC_INDEX,0x102); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x202); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x402); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x802); vgacpy(vga+n,p,n);

  outpw(SC_INDEX,0xF02); outp(0x3CE,5); outp(0x3CF,(inp(0x3CF)&252)+1);
  memcpyb(vga,vga+n,n); outp(0x3CE,5); outp(0x3CF,inp(0x3CF)&252);
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Subrutinas de volcado genricas
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void vgacpy(byte * q, byte * p, int n) {
  int m;

  m=n>>2; while (m--) {
    *(int*)q=*p+256*(*(p+4)+256*(*(p+8)+256*(*(p+12)))); q+=4; p+=16;
  }

  n&=3; while (n--) {
    *q=*p; q++; p+=4;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Selecciona una ventana para su posterior volcado
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_volcado(void) { memset(&scan[0],0,MAX_YRES*8); volcado_completo=0; }

void volcado_parcial(int x,int y,int an,int al) {
  int ymax,xmax,n,d1,d2,x2;

  if (an==vga_an && al==vga_al && x==0 && y==0) { volcado_completo=1; return; }

  if (an>0 && al>0 && x<vga_an && y<vga_al) {
    if (x<0) { an+=x; x=0; } if (y<0) { al+=y; y=0; }
    if (x+an>vga_an) an=vga_an-x; if (y+al>vga_al) al=vga_al-y;
    if (an<=0 || al<=0) return;
    xmax=x+an-1; ymax=y+al-1;

    if (!modovesa) {
      switch(vga_an*1000+vga_al) {
        case 320240: case 320400: case 360240: case 360360: case 376282: // Modos X
          x>>=2; xmax>>=2; an=xmax-x+1; break;
      }
    }

    while (y<=ymax) { n=y*4;
      if (scan[n+1]==0) {         // Caso 1, el scan estaba vacกo ...
        scan[n]=x; scan[n+1]=an;
      } else if (scan[n+3]==0) {  // Caso 2, ya hay un scan definido ...
        if (x>scan[n]+scan[n+1] || x+an<scan[n]) { // ... hueco entre medias
          if (x>scan[n]) {
            scan[n+2]=x; scan[n+3]=an;
          } else {
            scan[n+2]=scan[n]; scan[n+3]=scan[n+1];
            scan[n]=x; scan[n+1]=an;
          }
        } else { // ... no hay hueco, amplia el primer scan
          if (x<(x2=scan[n])) scan[n]=x;
          if (x+an>x2+scan[n+1]) scan[n+1]=x+an-scan[n];
          else scan[n+1]=x2+scan[n+1]-scan[n];
        }
      } else {                    // Caso 3, hay 2 scanes definidos ...
        if (x<=scan[n]+scan[n+1] && x+an>=scan[n+2]) {
          // Caso 3.1, se tapa el hueco anterior -> queda un solo scan
          if (x<scan[n]) scan[n]=x;
          if (x+an>scan[n+2]+scan[n+3]) scan[n+1]=x+an-scan[n]; else scan[n+1]=scan[n+2]+scan[n+3]-scan[n];
          scan[n+2]=0; scan[n+3]=0;
        } else {
          if (x>scan[n]+scan[n+1] || x+an<scan[n]) { // No choca con 1ง
            if (x>scan[n+2]+scan[n+3] || x+an<scan[n+2]) { // No choca con 2ง
              // Caso 3.4, el nuevo no colisiona con ninguno, se calcula el espacio
              // hasta ambos, y se fusiona con el m s cercano
              if (x+an<scan[n]) d1=scan[n]-(x+an); else d1=x-(scan[n]+scan[n+1]);
              if (x+an<scan[n+2]) d2=scan[n+2]-(x+an); else d2=x-(scan[n+2]+scan[n+3]);
              if (d1<=d2) {
                // Caso 3.4.1 se fusiona con el primero
                if (x<(x2=scan[n])) scan[n]=x;
                if (x+an>x2+scan[n+1]) scan[n+1]=x+an-scan[n];
                else scan[n+1]=x2+scan[n+1]-scan[n];
              } else {
                // Caso 3.4.2 se fusiona con el segundo
                if (x<(x2=scan[n+2])) scan[n+2]=x;
                if (x+an>x2+scan[n+3]) scan[n+3]=x+an-scan[n+2];
                else scan[n+3]=x2+scan[n+3]-scan[n+2];
              }
            } else {
              // Caso 3.3, el nuevo colisiona con el 2ง, se fusionan
              if (x<(x2=scan[n+2])) scan[n+2]=x;
              if (x+an>x2+scan[n+3]) scan[n+3]=x+an-scan[n+2];
              else scan[n+3]=x2+scan[n+3]-scan[n+2];
            }
          } else {
            // Caso 3.2, el nuevo colisiona con el 1ง, se fusionan
            if (x<(x2=scan[n])) scan[n]=x;
            if (x+an>x2+scan[n+1]) scan[n+1]=x+an-scan[n];
            else scan[n+1]=x2+scan[n+1]-scan[n];
          }
        }
      } y++;
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciones para la creaciขn de la tabla ghost
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_ghost(void) {

  int n,m;
  byte * d=paleta;

  for (n=0;n<768;n++) dac4[n]=paleta[n]*4;

  for (n=0;n<512;n++) vcubos[n]=NULL;

  for (n=0;n<256;n++) {
    tpuntos[n].r=*d++*4; tpuntos[n].g=*d++*4; tpuntos[n].b=*d++*4;
    m=(((int)tpuntos[n].r&224)<<1)+(((int)tpuntos[n].g&224)>>2)+((int)tpuntos[n].b>>5);

    if (vcubos[m]==NULL) {
      vcubos[m]=&tpuntos[n]; tpuntos[n].next=NULL;
    } else {
      tpuntos[n].next=vcubos[m]; vcubos[m]=&tpuntos[n];
    }
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Funciขn para la creaciขn de la tabla ghost
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int rr,gg,bb;
int num_puntos;

void crear_ghost(void) {

  int n,m;
  int r3,g3,b3,vcubo;
  byte * ptr;

  n=255; do {
    ptr=paleta+n*3; _r=*ptr; _g=*(ptr+1); _b=*(ptr+2); ptr=paleta;
    m=0; do {
      rr=((int)(*ptr+_r)<<7)&0x3f00;
      gg=((int)(*(ptr+1)+_g)<<7)&0x3f00;
      bb=((int)(*(ptr+2)+_b)<<7)&0x3f00;
      ptr+=3;

      r3=(rr&0x3800)>>5; g3=(gg&0x3800)>>8; b3=(bb&0x3800)>>11;
      vcubo=r3+g3+b3;

      find_min=65536;
      num_puntos=0;

      // Cubos de distancia sqr(0) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      crear_ghost_vc(vcubo);

      if (num_puntos>1) goto fast_ghost;

      // Cubos de distancia sqr(1) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      if (r3>0) crear_ghost_vc(vcubo-64);
      if (r3<7*64) crear_ghost_vc(vcubo+64);
      if (g3>0) crear_ghost_vc(vcubo-8);
      if (g3<7*8) crear_ghost_vc(vcubo+8);
      if (b3>0) crear_ghost_vc(vcubo-1);
      if (b3<7) crear_ghost_vc(vcubo+1);

      if (num_puntos>2) goto fast_ghost;

      // Cubos de distancia sqr(2) ฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤ

      if (r3>0) {
        if (g3>0) crear_ghost_vc(vcubo-64-8);
        else { if (g3<7*8) crear_ghost_vc(vcubo-64+8); }
        if (b3>0) crear_ghost_vc(vcubo-64-1);
        else { if (b3<7) crear_ghost_vc(vcubo-64+1); }
      } else if (r3<7*64) {
        if (g3>0) crear_ghost_vc(vcubo+64-8);
        else { if (g3<7*8) crear_ghost_vc(vcubo+64+8); }
        if (b3>0) crear_ghost_vc(vcubo+64-1);
        else { if (b3<7) crear_ghost_vc(vcubo+64+1); }
      }
      if (g3>0) if (b3>0) crear_ghost_vc(vcubo-8-1);
                else { if (b3<7) crear_ghost_vc(vcubo-8+1); }
      else if (g3<7*8) if (b3>0) crear_ghost_vc(vcubo+8-1);
                else { if (b3<7) crear_ghost_vc(vcubo+8+1); }

      if (find_min==65536) crear_ghost_slow();

      fast_ghost: *(ghost+n*256+m)=find_col;
                  *(ghost+m*256+n)=find_col;

      // if ((punto++&2047)==0) cprintf(".");

    } while (++m<n);
  } while (--n);

  do { *(ghost+n*256+n)=n; } while(++n<256);

  // if (puntos) cprintf(".\r\n");

  memcpy(ghost_inicial,ghost,256);

  n=0; ptr=ghost;
  do {
    *ptr++=n++;
  } while (n<256);

}

void crear_ghost_vc(int m) {

  int dif;
  struct t_tpuntos * p;

  if ((p=vcubos[m])!=NULL) do { num_puntos++;
    dif=*(int*)(cuad+rr+(*p).r);
    dif+=*(int*)(cuad+gg+(*p).g);
    dif+=*(int*)(cuad+bb+(*p).b);
    if (dif<find_min) { find_min=dif;
        find_col=((byte*)p-(byte*)tpuntos)/sizeof(struct t_tpuntos); }
  } while ((p=(*p).next)!=NULL);
}

void crear_ghost_slow (void) {

  int dmin,dif;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  do {
    dif=*(int*)(cuad+rr+*pal); pal++;
    dif+=*(int*)(cuad+gg+*pal); pal++;
    dif+=*(int*)(cuad+bb+*pal); pal+=4;
    if (dif<dmin) { dmin=dif; color=pal-6; }
  } while (pal<endpal);
  find_col=(color-dac4)/3;
}

void find_color(byte r, byte g,byte b) { // Encuentra un color (que no sea el 0)

  int dmin,dif;
  byte *pal,*endpal,*color;

  pal=paleta+3; endpal=paleta+768; dmin=65536;
  do {
    if (((pal-paleta)/3)==last_c1) pal+=3;
    dif=(int)(r-*pal)*(int)(r-*pal); pal++;
    dif+=(int)(g-*pal)*(int)(g-*pal); pal++;
    dif+=(int)(b-*pal)*(int)(b-*pal); pal++;
    if (dif<dmin) { dmin=dif; color=pal-3; }
  } while (pal<endpal);
  find_col=(color-paleta)/3;
}

byte media(byte a,byte b) {
  find_color(
      (paleta[a*3]+paleta[b*3])/2,
      (paleta[a*3+1]+paleta[b*3+1])/2,
      (paleta[a*3+2]+paleta[b*3+2])/2
    );
  return(find_col);
}

//ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
