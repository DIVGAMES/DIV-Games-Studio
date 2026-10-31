
///////////////////////////////////////////////////////////////////////////////
// Video functions
///////////////////////////////////////////////////////////////////////////////

#include "inter.h"
#include "madewith.h"

#ifdef GCW
//#define GCW_W 640
//#define GCW_H 480

#define GCW_W 320
#define GCW_H 240

float w_ratio=1.0;
float h_ratio=1.0;
#endif

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
#ifdef SDL2
SDL_Window *divWindow=NULL;
SDL_Renderer *divRender=NULL;
SDL_Texture *divTexture=NULL;
#endif

#endif

void snapshot(byte *p);
void blitcsvga(byte *p);
void blitc320200(byte *p);
void blitcx(byte * p);
void blitpsvga(byte *p);
void blitp320200(byte *p);
void blitpx(byte * p);
int save_PCX(byte *map,int w,int h,FILE *f);
void create_ghost_vc(int m);
void create_ghost_slow(void);

// Check if SDL is already loaded

#ifndef __EMSCRIPTEN__
int IsFullScreen(SDL_Surface *surface)
{
    return OSDEP_IsFullScreen(); // Return false if surface is windowed
}

int SDL_ToggleFS(SDL_Surface *surface)
{

    if (IsFullScreen(surface))
    fsmode=0;
  else 
    fsmode=1;
  
  svmode();
  set_dac();
    return 1;
}
#endif

#define MAX_YRES 2048

short scan[MAX_YRES*4]; // Per scan [x,w,x,w] up to 2 segments to dump are defined

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

//�����������������������������������������������������������������������������
//      Ghost table
//�����������������������������������������������������������������������������

struct t_tpoints { // For the creation of the ghost table
  int r,g,b;
  struct t_tpoints * next;
} tpoints[256];

struct t_tpoints * vcubes[512]; // For the creation of the ghost table

extern int fli_palette_update;

//����������������������������������������������������������������������������
// Function to set the palette
//����������������������������������������������������������������������������

byte dark_color;

void set_palette (void) {
  word n;
//printf("set palette\n");
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
    if (now_dacout_r>palette[n]) dac[n]=0; else dac[n]=palette[n]-now_dacout_r;
    if (dac[n]>63) dac[n]=63; n++;
    if (now_dacout_g>palette[n]) dac[n]=0; else dac[n]=palette[n]-now_dacout_g;
    if (dac[n]>63) dac[n]=63; n++;
    if (now_dacout_b>palette[n]) dac[n]=0; else dac[n]=palette[n]-now_dacout_b;
    if (dac[n]>63) dac[n]=63; n++;
  } while (n<768);

  dark_color=0;

  if (process_active_palette!=NULL) process_active_palette();
}
extern int splashtime;

void set_dac (void) {
#ifndef DEBUG
	if(splashtime>0)	
		return;
#endif
	if(vga==NULL) 
		return;
	/* An FLI frame already installed its palette. set_dac2 restores dac after the blit. */
	if (fli_palette_update)
		return;
#ifndef DOS

	SDL_Color colors[512];

	int i;
	int b=0;
	for(i=0;i<256;i++){
          colors[i].r=dac[b]*4;
          colors[i].g=dac[b+1]*4;
          colors[i].b=dac[b+2]*4;
          b+=3;
    }
	if(!OSDEP_SetPalette(vga, colors, 0, 256)) 
		printf("Failed to set palette :(\n"); 
	
	retrace();
#else
  union REGS regs;
  word n=0;
  //retrace();
  outp(0x3c8,0);
  do {
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
    outp(0x3c9,dac[n++]);
  } while (n<768);
  regs.w.ax=0x1001;
  regs.h.bh=dark_color;
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
#else
  set_dac();
#endif
}

void retrace (void) {
#ifdef DOS
  while (inp(0x3da)&8);
  while ((inp(0x3da)&8)==0);
#endif
}

//�����������������������������������������������������������������������������
//      Set Video Mode (vga_width and vga_height are defined in shared.h)
//�����������������������������������������������������������������������������

int LinearMode;
int vesamode;

extern float m_x,m_y;


extern int oldticks;
void madewith(void) {

	SDL_RWops *rwops = NULL;
	SDL_Surface *mwsurface, *image;

	rwops = SDL_RWFromMem(madewithsplash,256138);
	mwsurface = SDL_LoadBMP_RW(rwops,1);

	SDL_BlitSurface(mwsurface,NULL,vga,NULL);
	OSDEP_Flip(mwsurface);
  SDL_FreeSurface(mwsurface);


}

void svmode(void) {

//#ifdef STDOUTLOG
printf("setting new video mode %d %d %x\n",vga_width,vga_height,vga);
//#endif

//hide the mouse
SDL_ShowCursor(SDL_DISABLE);
//	if(vga)
//		SDL_FreeSurface(vga);	
	vga=NULL;
#ifdef __EMSCRIPTEN__
	
	if(!vga)	
		vga=OSDEP_SetVideoMode(vga_width, vga_height, 8, 0);
#else

#ifdef PANDORA
		vga=OSDEP_SetVideoMode(vga_width, vga_height, 8, 1);
#else

#ifdef PSP
	fsmode=1;
//		if(!vga)
			vga=OSDEP_SetVideoMode(480, 272, 8, 0);
			//SDL_SWSURFACE | SDL_FULLSCREEN);
#else

		if(fsmode==1)
			vga=OSDEP_SetVideoMode(vga_width, vga_height, 8, 1);
#endif

		if(!vga || fsmode==0)
			vga=OSDEP_SetVideoMode(vga_width, vga_height, 8, 0);//, SDL_FULLSCREEN | SDL_HWSURFACE | SDL_DOUBLEBUF);

#ifdef GCW
    
  if(vga_width>640 || vga_height>480) {
    
    vga=OSDEP_SetVideoMode(640,480, 8, 0);
    if(!vga) 
      vga=OSDEP_SetVideoMode(320,240, 8, 0);

    if(vga) {
//    vga=SDL_SetVideoMode(GCW_W,GCW_H, 8, 0);
//    if(!vga) {
//      vga=SDL_SetVideoMode(320,240, 8, 0);
//    }

//    w_ratio = vga_width / (float)(vga->w*1.0);
//    h_ratio = vga_height / (float)(vga->h*1.0);   

      printf("Setting soft mode %d %d\n",vga->w,vga->h);
    
    } 
  }
  // hardware scale
#endif

#endif
		printf("Set mode: %d,%d\n",vga->w,vga->h);
		

#endif


#ifdef STDOUTLOG
	printf("SET VIDEO MODE %x\n",vga);
#endif
	OSDEP_SetCaption( "DIVDX 3.01", "" );

	vesamode=1;
#ifdef DOS
  VBESCREEN Screen;

  int mode=0;
  int error=0,n;

  LinearMode=0;
  vesamode=0;

  // First check whether it is a vesa mode

  for (n=0;n<num_video_modes;n++) {
    if (vga_width==video_modes[n].width && vga_height==video_modes[n].height) {
      if (video_modes[n].mode && video_modes[n].mode<256000) { mode=video_modes[n].mode; break; }
    }
  }

  if (n<num_video_modes) {
    vesamode=1;
    if(VersionVesa<0x102) {
      if (!VBE_setVideoMode(mode)) error=1;
      else vga=(char *)0x0A0000;
    } else {
      if(VersionVesa<0x200) {
        if(!SV_setMode(mode)) error=1;
        else vga=(char *)0x0A0000;
      } else {
       	if (vbeSetMode (vga_width, vga_height, 8, &Screen) == 4) {
          LinearMode=1;
          mode|=vbeLinearBuffer;
          if(!SV_setMode(mode)) {
            LinearMode=0;
            mode^=vbeLinearBuffer;
            if(!SV_setMode(mode)) error=1;
            else vga=(char *)videoMem;
          } else vga=(char *)videoMem;
        } else {
          LinearMode=1;
          vga=Screen.adr;
        }
      }
    }
  } else switch(vga_width*1000+vga_height) {
    case 320200: _setvideomode(_MRES256COLOR); break;
    case 320240: svmodex(0); break;
    case 320400: svmodex(1); break;
    case 360240: svmodex(2); break;
    case 360360: svmodex(3); break;
    case 376282: svmodex(4); break;
    default: error=1; break;
  }

  // NOTE!, this causes that, on machines without VESA, it is shown in "320x200 BIG"
  
  if (error) {
    vesamode=0;
    vga_width=320; vga_height=200; _setvideomode(_MRES256COLOR);
  }
#endif

  m_x=(float)vga_width/2.0;
  m_y=(float)vga_height/2.0;

  if (demo) {
    texts[max_texts].type=0;
    texts[max_texts].center=4;
    texts[max_texts].y=vga_height/2;
    texts[max_texts].x=vga_width/2;
    texts[max_texts].font=(byte*)fonts[0];
  } else texts[max_texts].font=0;

#ifndef DEBUG
if(splashtime>0)
  madewith();
#endif
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
  outp(CRTC_INDEX+1,vga_width/8);
#endif
}

//�����������������������������������������������������������������������������
//      Reset Video Mode
//�����������������������������������������������������������������������������

void rvmode(void) {

// Emscripten has its own fullscreen controls
#ifndef __EMSCRIPTEN__
	if(IsFullScreen(vga))
		SDL_ToggleFS(vga);
#endif

#ifdef DOS
  SV_restoreMode();
  _setvideomode(3);
#endif
}

//�����������������������������������������������������������������������������
//      Dump buffer to VGA
//�����������������������������������������������������������������������������
#ifdef GCW
void blitgcw(byte *p) {
	// blit screen to smaller 320x240 screen
	byte *qt = (byte *)vga->pixels;
	byte *q = qt;
	int row=0;
	float vy=0;
	float vx=0;
	float wratio = vga_width / (float)(vga->w*1.0);
	float hratio = vga_height / (float)(vga->h*1.0);
	byte *c;
	//printf("ratio is %fx%f\n",wratio,hratio);
	
	if(SDL_MUSTLOCK(vga))
		SDL_LockSurface(vga);

	for (vy=0; vy<vga_height;vy+=hratio) {
		// calculate the pixel
		c=&p[vga_width*(int)vy];
		
		for(vx=0;vx<vga_width;vx+=wratio) {
			
			*q=c[(int)vx];
//			c+=(int)wratio;
			q++;
		}
//		memcpy(q,p,vga_width);
//			p=p[(int)(vga_width*hratio)];
		//q+=vga->pitch;//vga_width;//*vga->pitch*vga->format->BytesPerPixel;
	}
	if(SDL_MUSTLOCK(vga))
		SDL_UnlockSurface(vga);

	return;
	for (vy=0; vy<vga_height;vy+=hratio) {
//printf("%d %d %d vy is %f %d\n",row,GCW_W, GCW_H, vy,(int)vy);
		// calculate the pixel
		c=&p[vga->w*(int)(row/hratio)];
		q=&qt[vga->pitch*(int)row];
//		qt = &q[(int)vy *GCW_W];
		for(vx=0;vx<vga_width;vx+=wratio) {
//printf("plotting %f %f %x %x\n",vy,vx,q,vga->pixels);		
			*q=c[(int)vx];
//			c+=(int)wratio;
			q++;
		}
		row++;
//		memcpy(q,p,vga_width);
//			p=p[(int)(vga_width*hratio)];
		//q+=vga->pitch;//vga_width;//*vga->pitch*vga->format->BytesPerPixel;
	}
	
}
#endif

void blitsdl(byte *p) {
	
#ifndef SDL2
	if(!vga) {
		printf("setting up screen for first time %d %d\n",vga_width,vga_height);
		svmode();
		set_dac(); // ghost_table();
	}
#else
	SDL_UpdateTexture(divTexture, NULL, copy_buffer, vga_width * sizeof (Uint8));
SDL_RenderClear(divRender);
SDL_RenderCopy(divRender, divTexture, NULL, NULL);
SDL_RenderPresent(divRender);
#endif

#ifdef GCW 
	if(vga_width>640 || vga_height>480) {
		blitgcw(p);
		OSDEP_Flip(vga);
		return;
	}
#endif

	if(SDL_MUSTLOCK(vga))
		SDL_LockSurface(vga);

	byte *q = (byte *)vga->pixels;
	int vy=0;
	for (vy=0; vy<vga_height;vy++) {
		memcpy(q,p,vga_width);
		p+=vga_width;
		q+=vga->pitch;//vga_width;//*vga->pitch*vga->format->BytesPerPixel;
	}

//	printf("draw screen\n");
	if(SDL_MUSTLOCK(vga))
		SDL_UnlockSurface(vga);

//	SDL_UpdateRect(vga,0,0,vga_width,vga_height);
	OSDEP_Flip(vga);
}

long lasttick = 0;
long newtick =0;
long nexttick = 0;
extern int game_fps;
int framecount=0;
int recording = 0;
int tfs =0 ;
extern int alt_x;

#define maxframes 30000

void blit_screen(byte *p) {
//printf("%d %d %d\n",game_fps,fclock,iclock);//,clock_ticks);
#ifndef __EMSCRIPTEN__
if ((shift_status&4) && (shift_status&8) && key(_0)) {
	recording = 1;
}
if ((shift_status&4) && (shift_status&8) && key(_9)) {
	recording = 0;
}

	if(recording) {

//		if(programRunning )  {
			
			framecount++;
//		}
			if(!(framecount%(fps/12)) && framecount>5 && framecount<maxframes) { // && framecount%3==1) {
				char filename[255];
				memset(filename,0,255);
				sprintf(filename,"/home/mike/Desktop/out/out%05d.bmp",framecount);
				SDL_SaveBMP(vga, filename);
			} else {
				if(framecount>maxframes)
				alt_x = 1;
			}
	//	}
	}
	// CTRL + ALT + P

	
	
  if ((shift_status&4) && (shift_status&8) && key(_P)) {
    snapshot(p);
    do {read_key();} while(key(_P));
  }
  
  if (shift_status&8 && key(_ENTER)) {
	SDL_ToggleFS(vga);
	do{read_key();} while (key(_ENTER));
 } 
  
  
  
#endif
  if (fli_palette_update) retrace();

blitsdl(p);

#ifdef NOTYET

  if (blit_full) {
    if (vesamode) blitcsvga(p);
    else switch(vga_width*1000+vga_height) {
      case 320200: blitc320200(p); break;
      case 320240: blitcx(p); break;
      case 320400: blitcx(p); break;
      case 360240: blitcx(p); break;
      case 360360: blitcx(p); break;
      case 376282: blitcx(p); break;
    }
  } else {
    if (vesamode) blitpsvga(p);
    else switch(vga_width*1000+vga_height) {
      case 320200: blitp320200(p); break;
      case 320240: blitpx(p); break;
      case 320400: blitpx(p); break;
      case 360240: blitpx(p); break;
      case 360360: blitpx(p); break;
      case 376282: blitpx(p); break;
    }
  }
#endif
  if (fli_palette_update) { fli_palette_update=0; set_dac2(); }
  init_blit();
}

//�����������������������������������������������������������������������������
//      Snapshot
//�����������������������������������������������������������������������������

void snapshot(byte *p) {
  FILE * f;
  int n=0;
  char cwork[128];

  do {
    sprintf(cwork,"SNAP%04d.PCX",n++);
    if ((f=fopen(cwork,"rb"))!=NULL) fclose(f);
  } while (f!=NULL);

  f=fopen(cwork,"wb");
  save_PCX(p,vga_width,vga_height,f);
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

int save_PCX(byte *map,int w,int h,FILE *f) {
  byte p[768];
  int x;
  byte *cbuffer;
  struct pcx_struct pcx;
  int ptr=0;
  int cptr=0;
  int Overflow=0;
  char ActPixel;
  char cntPixel=0;
  char Smallpalette=12;

        pcx.header.manufacturer=10;
        pcx.header.version=5;
        pcx.header.encoding=1;
        pcx.header.bits_per_pixel=8;
        pcx.header.xmin=0;
        pcx.header.ymin=0;
        pcx.header.xmax=w-1;
        pcx.header.ymax=h-1;
        pcx.header.hres=w;
        pcx.header.vres=h;
        pcx.header.color_planes=1;
        pcx.header.bytes_per_line=w;
        pcx.header.palette_type=0;
        pcx.header.Hresol=w;
        pcx.header.Vresol=h;

        if ((cbuffer=(unsigned char *)malloc(w*h*2))==NULL) return(1);

        ActPixel=map[ptr];
        while (ptr < w*h)
        {
                while((map[ptr]==ActPixel) && (ptr<w*h))
                {
                        cntPixel++;
                        Overflow++;
                        ptr++;
                        if(Overflow==w)
                        {
                                Overflow=0;
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
                ActPixel=map[ptr];
                cntPixel=0;
        }

        fwrite(&pcx.header,1,sizeof(pcx_header),f);
        fwrite(cbuffer,1,cptr,f);
        fwrite(&Smallpalette,1,1,f);
        for (x=0;x<768;x++) p[x]=palette[x]*4;
        fwrite(p,1,768,f);
        free(cbuffer);
        return(0);
}

int save_MAP (byte * map, int w, int h, FILE * f) {
  int y;
  char cwork[32]="";
  char rules[576];

  fwrite("map\x1a\x0d\x0a\x00\x00",8,1,f);      // +000 Header and version
  fwrite(&w,2,1,f);                   // +008 Width
  fwrite(&h,2,1,f);                   // +010 Height
  y=1; fwrite(&y,4,1,f);// +012 Code

  fwrite(cwork,32,1,f);// +016 Description
  fwrite(palette,768,1,f);                          // +048 Palette

  for (y=0;y<16;y++) {
    rules[y*36]=16;
    rules[y*36+1]=0;
    rules[y*36+2]=0;
    rules[y*36+3]=0;
    memset(&rules[y*36+4],y*16,32);
  } fwrite(rules,1,sizeof(rules),f);            // +816 Color rules

  y=0; fwrite(&y,2,1,f);                     // +1392 Number of points
  fwrite(map,w*h,1,f);
  return(0);
}

//�����������������������������������������������������������������������������
//      Partial restore of the background to the copy
//�����������������������������������������������������������������������������

//int scan0[MAX_YRES]; // tells, for each scan, the initial pixel (vga_width if n/a)
//int scan1[MAX_YRES]; // tells, for each scan, the final pixel (0 if n/a)

//short scan[MAX_YRES*4]; // Per scan [x,w,x,w] up to 2 segments to blit are defined

void restore(byte *q, byte *p) {
  int y=0,n=0;
  if (vga_width<640 && vga_height>200) { // Mode-X
    while (y<vga_height) {
      n=y*4;
      if (scan[n+1]) memcpy(q+scan[n]*4,p+scan[n]*4,scan[n+1]*4);
      if (scan[n+3]) memcpy(q+scan[n+2]*4,p+scan[n+2]*4,scan[n+3]*4);
      q+=vga_width; p+=vga_width; y++;
    }
  } else {
    while (y<vga_height) {
      n=y*4;
      if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
      if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
      q+=vga_width; p+=vga_width; y++;
    }
  }
}

//�����������������������������������������������������������������������������
//      Dump in 320x200 mode
//�����������������������������������������������������������������������������

void blitp320200(byte *p) {
  int y=0,n;
  byte * q=(byte *)vga->pixels;

  #ifdef RECORDER
  RegScreen(p);
  #endif

  while (y<vga_height) {
    n=y*4;
    if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
    if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
    q+=vga_width; p+=vga_width; y++;
  }
}

void blitc320200(byte *p) {
  #ifdef RECORDER
  RegScreen(p);
  #endif
  memcpy(vga,p,vga_width*vga_height);
}

//�����������������������������������������������������������������������������
//      Dump in SVGA
//�����������������������������������������������������������������������������

void blitpsvga(byte *p) {
#ifdef DOS
  int y=0,page,old_page=-1751,point,t1,t2,n;
  char *q=vga;

  if(LinearMode) {
   while (y<vga_height) {
     n=y*4;
     if (scan[n+1]) memcpy(q+scan[n],p+scan[n],scan[n+1]);
     if (scan[n+3]) memcpy(q+scan[n+2],p+scan[n+2],scan[n+3]);
     q+=vga_width; p+=vga_width; y++;
   }
  } else while (y<vga_height) {
    n=y*4;
    if (scan[n+1]) {
      page=(y*vga_width+scan[n])/65536;
      point=(y*vga_width+scan[n])%65536;
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
      page=(y*vga_width+scan[n+2])/65536;
      point=(y*vga_width+scan[n+2])%65536;
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
    } p+=vga_width; y++;
  }
#endif

}

void blitcsvga(byte *p) {
#ifdef DOS
  int cnt=vga_width*vga_height;
  int tpv=0,ActPge=0;

  if(LinearMode) memcpy(vga,p,cnt);
  else while(cnt>0) {
    SV_setBank((signed long)ActPge++);
    tpv=cnt>65536?65536:cnt;
    memcpy(vga,p,tpv);
    p+=tpv;
    cnt-=tpv;
  }
#endif
}

//�����������������������������������������������������������������������������
//      Dump in a mode-x
//�����������������������������������������������������������������������������

void blitpx(byte * p) {
#ifdef DOS
  int n,m=(vga_width*vga_height)/4,plane=0x100,y;
  byte * v2, * p2;

  do { v2=vga+m; y=0; p2=p++; outpw(SC_INDEX,2+plane); plane<<=1;
    while (y<vga_height) {
      n=y*4;
      if (scan[n+1]) vgacpy(v2+scan[n],p2+scan[n]*4,scan[n+1]);
      if (scan[n+3]) vgacpy(v2+scan[n+2],p2+scan[n+2]*4,scan[n+3]);
      v2+=vga_width/4; p2+=vga_width; y++; }
  } while (plane<=0x800);

  outpw(SC_INDEX,0xF02); outp(0x3CE,5); outp(0x3CF,(inp(0x3CF)&252)+1);
  y=0; v2=vga; while (y<vga_height) {
    n=y*4;
    if (scan[n+1]) memcpyb(v2+scan[n],v2+scan[n]+m,scan[n+1]);
    if (scan[n+3]) memcpyb(v2+scan[n+2],v2+scan[n+2]+m,scan[n+3]);
    v2+=vga_width/4; y++;
  } outp(0x3CE,5); outp(0x3CF,inp(0x3CF)&252);

#endif
}

void blitcx(byte * p) {
#ifdef DOS
  int n=(vga_width*vga_height)/4;

  outpw(SC_INDEX,0x102); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x202); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x402); vgacpy(vga+n,p,n); p++;
  outpw(SC_INDEX,0x802); vgacpy(vga+n,p,n);

  outpw(SC_INDEX,0xF02); outp(0x3CE,5); outp(0x3CF,(inp(0x3CF)&252)+1);
  memcpyb(vga,vga+n,n); outp(0x3CE,5); outp(0x3CF,inp(0x3CF)&252);
#endif
}

//�����������������������������������������������������������������������������
//      Generic dump subroutines
//�����������������������������������������������������������������������������

void vgacpy(byte * q, byte * p, int n) {
  int m;

  m=n>>2; while (m--) {
    *(int*)q=*p+256*(*(p+4)+256*(*(p+8)+256*(*(p+12)))); q+=4; p+=16;
  }

  n&=3; while (n--) {
    *q=*p; q++; p+=4;
  }
}

//�����������������������������������������������������������������������������
//      Select a window for later dumping
//�����������������������������������������������������������������������������

void init_blit(void) {
#ifndef DROID
	 memset(&scan[0],0,MAX_YRES*sizeof(short)); 
#endif
 blit_full=0; 
}

void blit_partial(int x,int y,int w,int h) {
  int ymax,xmax,n,d1,d2,x2;

  if (w==vga_width && h==vga_height && x==0 && y==0) { blit_full=1; return; }

  if (w>0 && h>0 && x<vga_width && y<vga_height) {
    if (x<0) { w+=x; x=0; } if (y<0) { h+=y; y=0; }
    if (x+w>vga_width) w=vga_width-x; if (y+h>vga_height) h=vga_height-y;
    if (w<=0 || h<=0) return;
    xmax=x+w-1; ymax=y+h-1;

    if (!vesamode) {
      switch(vga_width*1000+vga_height) {
        case 320240: case 320400: case 360240: case 360360: case 376282: // X modes
          x>>=2; xmax>>=2; w=xmax-x+1; break;
      }
    }

    while (y<=ymax) { n=y*4;
      if (scan[n+1]==0) {         // Case 1, the scan was empty ...
        scan[n]=x; scan[n+1]=w;
      } else if (scan[n+3]==0) {  // Case 2, there is already a scan defined ...
        if (x>scan[n]+scan[n+1] || x+w<scan[n]) { // ... gap in between
          if (x>scan[n]) {
            scan[n+2]=x; scan[n+3]=w;
          } else {
            scan[n+2]=scan[n]; scan[n+3]=scan[n+1];
            scan[n]=x; scan[n+1]=w;
          }
        } else { // ... there is no gap, extend the first scan
          if (x<(x2=scan[n])) scan[n]=x;
          if (x+w>x2+scan[n+1]) scan[n+1]=x+w-scan[n];
          else scan[n+1]=x2+scan[n+1]-scan[n];
        }
      } else {                    // Case 3, there are 2 scans defined ...
        if (x<=scan[n]+scan[n+1] && x+w>=scan[n+2]) {
          // Case 3.1, the previous gap is covered -> a single scan is left
          if (x<scan[n]) scan[n]=x;
          if (x+w>scan[n+2]+scan[n+3]) scan[n+1]=x+w-scan[n]; else scan[n+1]=scan[n+2]+scan[n+3]-scan[n];
          scan[n+2]=0; scan[n+3]=0;
        } else {
          if (x>scan[n]+scan[n+1] || x+w<scan[n]) { // Does not collide with the 1st
            if (x>scan[n+2]+scan[n+3] || x+w<scan[n+2]) { // Does not collide with the 2nd
              // Case 3.4, the new one does not collide with any, the space up to both
              // is computed, and it is merged with the closest one
              if (x+w<scan[n]) d1=scan[n]-(x+w); else d1=x-(scan[n]+scan[n+1]);
              if (x+w<scan[n+2]) d2=scan[n+2]-(x+w); else d2=x-(scan[n+2]+scan[n+3]);
              if (d1<=d2) {
                // Case 3.4.1 it is merged with the first one
                if (x<(x2=scan[n])) scan[n]=x;
                if (x+w>x2+scan[n+1]) scan[n+1]=x+w-scan[n];
                else scan[n+1]=x2+scan[n+1]-scan[n];
              } else {
                // Case 3.4.2 it is merged with the second one
                if (x<(x2=scan[n+2])) scan[n+2]=x;
                if (x+w>x2+scan[n+3]) scan[n+3]=x+w-scan[n+2];
                else scan[n+3]=x2+scan[n+3]-scan[n+2];
              }
            } else {
              // Case 3.3, the new one collides with the 2nd, they are merged
              if (x<(x2=scan[n+2])) scan[n+2]=x;
              if (x+w>x2+scan[n+3]) scan[n+3]=x+w-scan[n+2];
              else scan[n+3]=x2+scan[n+3]-scan[n+2];
            }
          } else {
            // Case 3.2, the new one collides with the 1st, they are merged
            if (x<(x2=scan[n])) scan[n]=x;
            if (x+w>x2+scan[n+1]) scan[n+1]=x+w-scan[n];
            else scan[n+1]=x2+scan[n+1]-scan[n];
          }
        }
      } y++;
    }
  }
}

//�����������������������������������������������������������������������������
//      Functions for the creation of the ghost table
//�����������������������������������������������������������������������������

void init_ghost(void) {

  int n,m;
  byte * d=palette;

  for (n=0;n<768;n++) dac4[n]=palette[n]*4;

  for (n=0;n<512;n++) vcubes[n]=NULL;

  for (n=0;n<256;n++) {
    tpoints[n].r=*d++*4; tpoints[n].g=*d++*4; tpoints[n].b=*d++*4;
    m=(((int)tpoints[n].r&224)<<1)+(((int)tpoints[n].g&224)>>2)+((int)tpoints[n].b>>5);

    if (vcubes[m]==NULL) {
      vcubes[m]=&tpoints[n]; tpoints[n].next=NULL;
    } else {
      tpoints[n].next=vcubes[m]; vcubes[m]=&tpoints[n];
    }
  }
}

//�����������������������������������������������������������������������������
//      Function for the creation of the ghost table
//�����������������������������������������������������������������������������

int rr,gg,bb;
int num_points;

void create_ghost(void) {

  int n,m;
  int r3,g3,b3,vcube;
  byte * ptr;

  n=255; do {
    ptr=palette+n*3; _r=*ptr; _g=*(ptr+1); _b=*(ptr+2); ptr=palette;
    m=0; do {
      rr=((int)(*ptr+_r)<<7)&0x3f00;
      gg=((int)(*(ptr+1)+_g)<<7)&0x3f00;
      bb=((int)(*(ptr+2)+_b)<<7)&0x3f00;
      ptr+=3;

      r3=(rr&0x3800)>>5; g3=(gg&0x3800)>>8; b3=(bb&0x3800)>>11;
      vcube=r3+g3+b3;

      find_min=65536;
      num_points=0;

      // Distance cubes sqr(0) ────────────────────────────────────────────

      create_ghost_vc(vcube);

      if (num_points>1) goto fast_ghost;

      // Distance cubes sqr(1) ────────────────────────────────────────────

      if (r3>0) create_ghost_vc(vcube-64);
      if (r3<7*64) create_ghost_vc(vcube+64);
      if (g3>0) create_ghost_vc(vcube-8);
      if (g3<7*8) create_ghost_vc(vcube+8);
      if (b3>0) create_ghost_vc(vcube-1);
      if (b3<7) create_ghost_vc(vcube+1);

      if (num_points>2) goto fast_ghost;

      // Distance cubes sqr(2) ────────────────────────────────────────────

      if (r3>0) {
        if (g3>0) create_ghost_vc(vcube-64-8);
        else { if (g3<7*8) create_ghost_vc(vcube-64+8); }
        if (b3>0) create_ghost_vc(vcube-64-1);
        else { if (b3<7) create_ghost_vc(vcube-64+1); }
      } else if (r3<7*64) {
        if (g3>0) create_ghost_vc(vcube+64-8);
        else { if (g3<7*8) create_ghost_vc(vcube+64+8); }
        if (b3>0) create_ghost_vc(vcube+64-1);
        else { if (b3<7) create_ghost_vc(vcube+64+1); }
      }
      if (g3>0) if (b3>0) create_ghost_vc(vcube-8-1);
                else { if (b3<7) create_ghost_vc(vcube-8+1); }
      else if (g3<7*8) if (b3>0) create_ghost_vc(vcube+8-1);
                else { if (b3<7) create_ghost_vc(vcube+8+1); }

      if (find_min==65536) create_ghost_slow();

      fast_ghost: *(ghost+n*256+m)=find_col;
                  *(ghost+m*256+n)=find_col;

      // if ((point++&2047)==0) cprintf(".");

    } while (++m<n);
  } while (--n);

  do { *(ghost+n*256+n)=n; } while(++n<256);

  // if (points) cprintf(".\r\n");
#ifdef DIV2
  memcpy(initial_ghost,ghost,256);

  n=0; ptr=ghost;
  do {
    *ptr++=n++;
  } while (n<256);
#endif
}

void create_ghost_vc(int m) {

  int dif;
  struct t_tpoints * p;

  if ((p=vcubes[m])!=NULL) do { num_points++;
    dif=*(int*)(squares+rr+(*p).r);
    dif+=*(int*)(squares+gg+(*p).g);
    dif+=*(int*)(squares+bb+(*p).b);
    if (dif<find_min) { find_min=dif;
        find_col=((byte*)p-(byte*)tpoints)/sizeof(struct t_tpoints); }
  } while ((p=(*p).next)!=NULL);
}

void create_ghost_slow (void) {

  int dmin,dif;
  byte *pal,*endpal,*color;

  pal=dac4; endpal=dac4+768; dmin=65536;
  do {
    dif=*(int*)(squares+rr+*pal); pal++;
    dif+=*(int*)(squares+gg+*pal); pal++;
    dif+=*(int*)(squares+bb+*pal); pal+=4;
    if (dif<dmin) { dmin=dif; color=pal-6; }
  } while (pal<endpal);
  find_col=(color-dac4)/3;
}


byte ffind_color(byte r, byte g,byte b) { // Find a color (other than 0)

  byte *color;

  byte* pal=palette+3; 
  byte* endpal=palette+768; 
  int dmin=65536;
  do {
    if (((pal-palette)/3)==last_c1) pal+=3;
    int dif=(int)(r-*pal)*(int)(r-*pal); pal++;
    dif+=(int)(g-*pal)*(int)(g-*pal); pal++;
    dif+=(int)(b-*pal)*(int)(b-*pal); pal++;
    if (dif<dmin) { dmin=dif; color=pal-3; }
  } while (pal<endpal);
  return (color-palette)/3;
}

void find_color(byte r, byte g,byte b) { // Find a color (other than 0)
	find_col = ffind_color(r,g,b);
}

byte media(byte a,byte b) {
  find_color(
      (palette[a*3]+palette[b*3])/2,
      (palette[a*3+1]+palette[b*3+1])/2,
      (palette[a*3+2]+palette[b*3+2])/2
    );
  return(find_col);
}

//����������������������������������������������������������������������������
