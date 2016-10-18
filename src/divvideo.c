
///////////////////////////////////////////////////////////////////////////////
//      Video Access Module
///////////////////////////////////////////////////////////////////////////////

#include "global.h"
//#include "inc\svga.h"
//#include "inc\vesa.h"
#include "lib/sdlgfx/SDL_framerate.h"


#ifdef TTF
#define CDEPTH 32
#else
#define CDEPTH 8
#endif

void snapshot(byte *p);
void volcadocsvga(byte *p);
void volcadoc320200(byte *p);
void volcadocx(byte * p);
void volcadopsvga(byte *p);
void volcadop320200(byte *p);
void volcadopx(byte * p);
void volcadosdl(byte *p);

///////////////////////////////////////////////////////////////////////////////
//	Declarations and module-level data
///////////////////////////////////////////////////////////////////////////////

#define CRTC_INDEX      0x3d4   //CRT Controller Index
#define CRTC_OFFSET     0x13    //CRTC offset register index
#define SC_INDEX        0x3c4   //Sequence Controller Index
#define MISC_OUTPUT     0x3c2   //Miscellaneous Output register

//byte * vga = (byte *) 0xA0000; // Pantalla fisica

SDL_Surface *vga;
#ifdef SDL2
SDL_Window *divWindow;
SDL_Renderer *divRender;
#endif

int IsFullScreen(SDL_Surface *surface)
{
	return OSDEP_IsFullScreen();
}

void SDL_ToggleFS(SDL_Surface *surface)
{
    if (IsFullScreen(surface))
		fsmode=0;
	else 
		fsmode=1;
	
	svmode();
	set_dac(dac);
}

int nothing(SDL_Surface *surface) {
        // Switch to WINDOWED mode
     Uint32 flags = surface->flags; // Get the video surface flags

   if (IsFullScreen(surface)) {
        if ((vga = OSDEP_SetVideoMode(vga_an, vga_al, CDEPTH, 0)) == NULL) 
			return 0;
		
		vga = OSDEP_SetVideoMode(vga_an, vga_al, CDEPTH, 0);
		fsmode=0;
    } else {
		
		vga = OSDEP_SetVideoMode(vga_an,vga_al, CDEPTH,1);// | SDL_HWSURFACE | SDL_DOUBLEBUF);
	
		if (vga == NULL) {
			vga = OSDEP_SetVideoMode(vga_an,vga_al, CDEPTH, 0);
		}
		fsmode=1;
	}
	set_dac(dac);
    
    return 1;
}


#define MAX_YRES 2048

static short scan[MAX_YRES*4]; // Por scan [x,an,x,an] se definen hasta 2 segmentos a volcar

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

///////////////////////////////////////////////////////////////////////////////
//      Awaits the arrival of the vertical retrace (vsync)
///////////////////////////////////////////////////////////////////////////////

FPSmanager fpsman;

void retrazo(void) {

//printf("retrazo (vsync)\n");
SDL_framerateDelay(&fpsman);

#ifdef NOTYET
  while (inp(0x3da)&8);
  while ((inp(0x3da)&8)==0);
#endif

}

///////////////////////////////////////////////////////////////////////////////
//      Activate a palette
///////////////////////////////////////////////////////////////////////////////


void set_dac(byte *_dac) {
	int i;
	int b=0;
	for(i=0;i<256;i++){
          colors[i].r=_dac[b]*4;
          colors[i].g=_dac[b+1]*4;
          colors[i].b=_dac[b+2]*4;
          b+=3;
    }
//	if(vga->format->BitsPerPixel==8) {
		if(!OSDEP_SetPalette(vga, colors, 0, 256)) 
			printf("Failed to set palette :(\n"); 
//	}	
	retrazo();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Set Video Mode (vga_an y vga_al se definen en shared.h)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

int LinealMode;
int modovesa;

#ifdef GCW
float w_ratio=1.0;
float h_ratio=1.0;
#endif

SDL_Surface* copy_surface(SDL_Surface* source)
{
    SDL_Surface *target;

    target = SDL_CreateRGBSurface(0, source->w, source->h,
                                  source->format->BitsPerPixel,
                                  source->format->Rmask, source->format->Gmask,
                                  source->format->Bmask, source->format->Amask);

    /*
     * I really don't understand why this is necessary. This is supposed to
     * clear the SDL_SRCALPHA flag presumably set by IMG_Load. But why wouldn't
     * blitting work otherwise?
     */
//    SDL_SetAlpha(source, 0, 0);
    
    SDL_BlitSurface(source, 0, target, 0);
    return target;
}


void svmode(void) {
//	printf("TODO - Set video mode (%dx%d)\n",vga_an,vga_al);
 Uint32 colorkey=0;
 int vn=0;

SDL_initFramerate(&fpsman);
SDL_setFramerate(&fpsman, 60);

 
  fprintf(stdout,"full screen: %d\n",fsmode);

#ifdef GCW_SOFTSTRETCH
	vga=OSDEP_SetVideoMode(GCW_W,GCW_H, 8,  SDL_HWSURFACE | SDL_DOUBLEBUF);//SDL_HWPALETTE|SDL_SRCCOLORKEY|SDL_HWSURFACE|SDL_DOUBLEBUF);
	w_ratio = vga_an / (float)(GCW_W*1.0);
	h_ratio = vga_al / (float)(GCW_H*1.0);
#else

	if(fsmode==0)
		vga=OSDEP_SetVideoMode(vga_an, vga_al, CDEPTH, 0);

	else
		vga=OSDEP_SetVideoMode(vga_an, vga_al, CDEPTH,  1);
	
	//SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);//SDL_HWPALETTE|SDL_SRCCOLORKEY|SDL_HWSURFACE|SDL_DOUBLEBUF);
#endif

//	if(copia_surface)
//		SDL_FreeSurface(copia_surface);
//			SDL_SetAlpha(copia_surface,SDL_SRCALPHA | SDL_RLEACCEL,128);

//	copia_surface = SDL_DisplayFormat( vga );

//	colorkey = SDL_MapRGB( copia_surface->format, 0xFF, 0, 0xFF );

//	SDL_FillRect(copia_surface, NULL, 0);

//	if(SDL_SetColorKey(copia_surface , SDL_SRCCOLORKEY , colorkey)==-1)
//		fprintf(stderr, "Warning: colorkey will not be used, reason: %s\n", SDL_GetError());;
	

#ifdef TTF
	if(vga!=NULL) {
		
		if(tapiz_surface!=NULL) {
			tempsurface = SDL_DisplayFormat(tapiz_surface);
			if(tempsurface!=NULL) {
				SDL_FreeSurface(tapiz_surface);
				tapiz_surface=tempsurface;
				tempsurface=NULL;
			}
		}
		
		if(mouse_surface!=NULL) {
			tempsurface = SDL_DisplayFormatAlpha(mouse_surface);
			if(tempsurface!=NULL) {
				SDL_FreeSurface(mouse_surface);
				mouse_surface=tempsurface;
				tempsurface=NULL;
			}
		}
		
		for(vn=max_windows-1;vn>=0; vn--) {
			if(ventana[vn].surfaceptr!=NULL) {
				tempsurface=SDL_DisplayFormat(ventana[vn].surfaceptr);
				if(tempsurface!=NULL) {
					SDL_FreeSurface(ventana[vn].surfaceptr);
					ventana[vn].surfaceptr=tempsurface;
					tempsurface=NULL;
				}
			}
		}	
	}
#endif

	modovesa=1;
	
	set_dac(dac);

#ifdef NOTYET

  VBESCREEN Screen;

  int mode=0;
  int error=0,n;

  LinealMode=0;
  modovesa=0;

  // Check first if a VESA mode

  for (n=0;n<num_modos;n++) {
    if (vga_an==modos[n].ancho && vga_al==modos[n].alto) {
      if (modos[n].modo) { mode=modos[n].modo; break; }
    }
  }
  
  if (n<num_modos) {
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
}

void svmodex(int m) {
	printf("TODO - divvideo.cpp svmodex.cpp\n");
#ifdef NOTYET
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
	if(IsFullScreen(vga))
		SDL_ToggleFS(vga);
//	SDL_FreeSurface(copia_surface);
	
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dump buffer to vga (screen)
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
#ifdef GCW_SOFTSTRETCH

void volcadogcw(byte *p) {
	// blit screen to smaller 320x240 screen
	byte *q = (byte *)vga->pixels;
	float vy=0;
	float vx=0;
	
	byte *c;

	if(SDL_MUSTLOCK(vga))
		SDL_LockSurface(vga);

	for (vy=0; vy<vga_al;vy+=h_ratio) {
		// calculate the pixel
		c=&p[vga_an*(int)vy];
	
		for(vx=0;vx<vga_an;vx+=w_ratio) {			
			*q=c[(int)vx];
			q++;
		}
	}
	
	if(SDL_MUSTLOCK(vga))
		SDL_UnlockSurface(vga);

}
#endif

void vgacpy(byte * q, byte * p, int n) ;


void volcadosdlp(byte *p) {
volcadosdl(p);
return;

	  int y=0,n;
	  byte *oldp = (byte *)p;
	  uint32_t x1=vga_an,y1=vga_al,w1=1,h1=1;
	SDL_Rect rc;  
	byte *q = (byte *)vga->pixels;
	uint32_t *q32 = (uint32_t *)vga->pixels;
	SDL_LockSurface(vga);
	
  while (y<vga_al) {
    n=y*4;
    if (scan[n+1]) {
//		printf("y:%d n: %d\n",y,n+1); 
		memcpy(q+scan[n],p+scan[n],scan[n+1]);
		if(scan[n]<x1)
			x1=scan[n];
		if(y<y1)
			y1=y;
		if(scan[n]+scan[n+1]+1-x1>w1)
			w1=scan[n]+scan[n+1]+1-x1;
			
		if(y+1-y1>h1)
			h1=(y+1-y1);
			
	}
    if (scan[n+3]) {
//		printf("y:%d n: %d\n",y,n+3); 
		memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
		if(scan[n+2]<x1)
			x1=scan[n+2];
		if(y<y1)
			y1=y;
		if(scan[n+2]+scan[n+3]+1-x1>w1)
			w1=scan[n+2]+scan[n+3]+1-x1;
			
		if(y+1-y1>h1)
			h1=(y+1-y1);
			
//		SDL_UpdateRect(vga,scan[n+2],y,scan[n+3],1);

	}

//    q+=vga_an; 
    q+=vga->pitch; 
    p+=vga_an; 
    y++;
  }
  SDL_UnlockSurface(vga);
  printf("changed rect: %d %d %d %d     \r",x1,y1,w1,h1);
  
 OSDEP_UpdateRect(vga,x1,y1,w1,h1);
	 
//	SDL_Flip(vga);
//	volcadosdl(p);
	return;
}

void volcadosdl(byte *p) {
	int vy;
	int vx;

	byte *oldp=(byte *)p;
	
	uint32_t colorkey = 0;
	SDL_Rect trc;
	int vn=0;
		
#ifdef GCW_SOFTSTRETCH
	if(vga_an>=GCW_W && vga_al>=GCW_H) {
		volcadogcw(p);
		SDL_Flip(vga);
		return;
	}
#endif

	int64_t n,m=(vga_an*vga_al)/4,plano=0x100,y;
	byte * v2, * p2;

	if(SDL_MUSTLOCK(vga))
		SDL_LockSurface(vga);

	byte *q = (byte *)vga->pixels;
	uint32_t *q32 = (uint32_t *)vga->pixels;

//	SDL_FillRect(vga, NULL, 0);


#ifndef TTF	
	if(0) {
		do { 
			v2=q+m; 
			y=0; 
			p2=p++; 
			plano<<=1;
			while (y<vga_al) {
				n=y*4;
				
				if (scan[n+1]) 
					vgacpy(v2+scan[n],p2+scan[n]*4,scan[n+1]);
				
				if (scan[n+3]) 
					vgacpy(v2+scan[n+2],p2+scan[n+2]*4,scan[n+3]);
				
				v2+=vga_an/4; 
					p2+=vga_an; y++; 
			}
		} while (plano<=0x800);

		y=0; 
		v2=(byte *)vga->pixels; 

		while (y<vga_al) {
			n=y*4;
			if (scan[n+1]) 
				memcpyb(v2+scan[n],v2+scan[n]+m,scan[n+1]);
			if (scan[n+3]) 
				memcpyb(v2+scan[n+2],v2+scan[n+2]+m,scan[n+3]);
			v2+=vga_an/4; y++;
		} 

	} else {
		for (vy=0; vy<vga_al;vy++) {
			switch(vga->format->BitsPerPixel) {
				case 32:
					q32= (uint32_t*)q;
					for(vx=0;vx<vga_an;vx++) {
						q32[vx]=(uint32_t)((colors[*p].b)+(colors[*p].g<<8)+(colors[*p].r<<16));
						*p++;
					} 
					break;

				case 24:
					for(vx=0;vx<vga_an;vx++) {
						q[vx*3]=colors[*p].b;
						q[vx*3+1]=colors[*p].g;
						q[vx*3+2]=colors[*p].r;
						*p++;
					}
					break;

					
				case 16:
					for(vx=0;vx<vga_an;vx++) {
						q[vx*2]=colors[*p].b+colors[*p].g<<8;
						q[vx*2+1]=colors[*p].g;
						*p++;
					}
					break;
				
				case 8: 
					memcpy(q,p,vga_an);
					p+=vga_an;
			}
		
			q+=vga->pitch;

		}
	}
	
#else
	
	
	if(tapiz_surface!=NULL);
		SDL_BlitSurface(tapiz_surface, NULL, vga, NULL);

	for(vn=max_windows-1;vn>=0; vn--) {
		if(ventana[vn].surfaceptr!=NULL) {
			trc.x=ventana[vn].x;
			trc.y=ventana[vn].y;
			trc.w=ventana[vn].an;
			trc.h=ventana[vn].al;

			
			//SDL_SetAlpha(ventana[vn].surfaceptr,SDL_SRCALPHA | SDL_RLEACCEL, 230);
			
			if(ventana[vn].exploding==1) {
				SDL_SetAlpha(ventana[vn].surfaceptr,SDL_SRCALPHA | SDL_RLEACCEL,explode_num*25);
			}
			SDL_BlitSurface(ventana[vn].surfaceptr,NULL,vga,&trc);
			
		}
		
	}

	trc.x=mouse_x;
	trc.y=mouse_y;
	trc.w=mouse_surface->w;
	trc.h=mouse_surface->h;

	SDL_SetAlpha(mouse_surface,SDL_SRCALPHA | SDL_RLEACCEL,32);
	
	SDL_BlitSurface(mouse_surface,NULL,vga,&trc);

//printf("window zero is %d\n",v.tipo);
#endif
		
	OSDEP_UpdateRect(vga,0,0,vga_an,vga_al);

	if(SDL_MUSTLOCK(vga))
		SDL_UnlockSurface(vga);

	OSDEP_Flip(vga);

//	colorkey = SDL_MapRGB( copia_surface->format, 0xFF, 0, 0xFF );
//	SDL_FillRect(copia_surface, NULL, colorkey);


	memset(oldp,vga_an*vga_al,0);

}

void volcado(byte *p) {

  if ((shift_status&4) && (shift_status&8) && scan_code==_P) snapshot(p);


  if (volcado_completo) {
    if (modovesa) volcadosdl(p);
    else switch(vga_an*1000+vga_al) {
      case 320200: volcadoc320200(p); break;
      case 320240: volcadocx(p); break;
      case 320400: volcadocx(p); break;
      case 360240: volcadocx(p); break;
      case 360360: volcadocx(p); break;
      case 376282: volcadocx(p); break;
    }
  } else {
    if (modovesa) volcadosdlp(p); 
    else switch(vga_an*1000+vga_al) {
      case 320200: volcadop320200(p); break;
      case 320240: volcadopx(p); break;
      case 320400: volcadopx(p); break;
      case 360240: volcadopx(p); break;
      case 360360: volcadopx(p); break;
      case 376282: volcadopx(p); break;
    }
  } 

  if (shift_status&8 && key(_ENTER)) {
	SDL_ToggleFS(vga);
 } 


  init_volcado();


}

void snapshot(byte *p) {
  FILE * f;
  int man=map_an,mal=map_al,n=0;
  char cwork[128];

  do {
    sprintf(cwork,"DIV_%04d.PCX",n++);
    if ((f=fopen(cwork,"rb"))!=NULL) fclose(f);
  } while (f!=NULL);

  f=fopen(cwork,"wb");
  map_an=vga_an; map_al=vga_al;
  graba_PCX(p,f);
  map_an=man; map_al=mal;
  fclose(f);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Dump mode 320x200
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadop320200(byte *p) { // PARTIAL
//printf("partial dump\n");
#ifdef NOTYET
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
#endif
//SDL_Flip(vga);
}

void volcadoc320200(byte *p) { // COMPLETE
//printf("complete dump\n");
#ifdef NOTYET
  #ifdef GRABADORA
  RegScreen(p);
  #endif
  memcpy(vga,p,vga_an*vga_al);
#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      SVGA DUMP
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadopsvga(byte *p) {
//	printf("divvideo.cpp - volcadopsvga\n");
//#ifdef NOTYET
  int y=0,page,old_page=-1751,point,t1,t2,n;
  char *q=(char *)vga->pixels;

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
 //       if (page!=old_page) SV_setBank((signed long)page);
        memcpy(vga+point,p+scan[n],t1);
 //       SV_setBank((signed long)page+1); old_page=page+1;
        memcpy(vga,p+scan[n]+t1,t2);
      } else {
       // if (page!=old_page) SV_setBank((signed long)(old_page=page));
        memcpy(vga+point,p+scan[n],scan[n+1]);
      }
    }
    if (scan[n+3]) {
      page=(y*vga_an+scan[n+2])/65536;
      point=(y*vga_an+scan[n+2])%65536;
      if (point+scan[n+3]>65536) {
        t1=65536-point;
        t2=scan[n+3]-t1;
   //     if (page!=old_page) SV_setBank((signed long)page);
        memcpy(vga+point,p+scan[n+2],t1);
    //    SV_setBank((signed long)page+1); old_page=page+1;
        memcpy(vga,p+scan[n+2]+t1,t2);
      } else {
     //   if (page!=old_page) SV_setBank((signed long)(old_page=page));
        memcpy(vga+point,p+scan[n+2],scan[n+3]);
      }
    } p+=vga_an; y++;
  }
//#endif
}

void volcadocsvga(byte *p) {
//	printf("divvideo.cpp - volcadocsvga\n");
//#ifdef NOTYET
  int cnt=vga_an*vga_al;
  int tpv=0,ActPge=0;

  if(LinealMode) memcpy(vga,p,cnt);
  else while(cnt>0) {
    //SV_setBank((signed long)ActPge++);
    tpv=cnt>65536?65536:cnt;
    memcpy(vga,p,tpv);
    p+=tpv;
    cnt-=tpv;
  }
//#endif
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Volcado en un modo-x
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void volcadopx(byte * p) {
debugprintf("divvideo.cpp volcadopx\n");
#ifdef NOTYET
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
	printf("divvideo.cpp - volcadocx\n");
#ifdef NOTYET
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

return;

  m=n>>2; while (m--) {
    *(int*)q=*p+256*(*(p+4)+256*(*(p+8)+256*(*(p+12)))); q+=4; p+=16;
  }

  n&=3; while (n--) {
    *q=*p; q++; p+=4;
  }
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//      Select a window for subsequent dump
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void init_volcado(void) { 
	memset(&scan[0],0,MAX_YRES*8); volcado_completo=0; 
}

void volcado_parcial(int x,int y,int an,int al) {
  int ymax=0,xmax=0,n=0,d1=0,d2=0,x2=0;

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

