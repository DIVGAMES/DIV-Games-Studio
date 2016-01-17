#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <conio.h>
//#include <mem.h>
#include "topflc.h"
#include "inter.h"

static void Error_Reporter(char *msg);
static void Palette_Update(TFUByte (*palette)[256][3]);

TFAnimation *animation=NULL;
char *CBuffer;
int CBuff_alt,TFOffset;
int CBuff_anc;
TFUByte *TFframe, (*TFpalette)[256][3];
TFAnimationInfo info;
char TFNombre[256];

int fli_palette_update=0;

struct {
  FILE *file;
  Uint8 *pMembuf;
  Uint32 membufSize;
  Uint8 *pFrame;
  Uint8 *pChunk;
  Uint16 FrameCount;    /* Frame Counter */
  /*
  */
  Uint32 HeaderSize;    /* Fli file size */
  Uint16 HeaderCheck;   /* Fli header check */
  Uint16 HeaderFrames;  /* Number of frames in flic */
  Uint16 HeaderWidth;   /* Fli width */
  Uint16 HeaderHeight;  /* Fli heigth */
  Uint16 HeaderDepth;   /* Color depth */
  Uint16 HeaderSpeed;   /* Number of video ticks between frame */
  Uint32 FrameSize;     /* Frame size in bytes */
  Uint16 FrameCheck;    /* Frame check */
  Uint16 FrameChunks;   /* Number of chunks in frame */
  Uint32 ChunkSize;     /* Size of chunk */
  Uint16 ChunkType;     /* Type of chunk */
  /*
  */
  SDL_Surface *mainscreen;
  SDL_Color colors[256];
  uint8_t *buffer;
  int offx;
  int offy;
  int screen_w;
  int screen_h;
  int screen_depth;
  int loop;
  byte ok;
} flc;

void ReadU16(uint16_t *tmp1, uint8_t *tmp2) {
	*tmp1= ((Uint8)*(tmp2+1)<<8)+(Uint8)*(tmp2);
}

void ReadU32(uint32_t *tmp1, uint8_t *tmp2) {
	*tmp1= (((((((Uint8)*(tmp2+3)<<8)+((Uint8)*(tmp2+2)))<<8)+((Uint8)*(tmp2+1)))<<8)+(Uint8)*(tmp2));
}

void FlcReadFile(Uint32 size)
{ if(size>flc.membufSize) {
    if(!(flc.pMembuf=(byte *)realloc(flc.pMembuf, size+1))) {
      printf("Realloc failed: %d\n", size);
      exit(1);
    }
  }

  if(fread(flc.pMembuf, sizeof(Uint8), size, flc.file)==0) {
    printf("Can't read flx file");
    exit(1);
  }
} /* FlcReadFile */



int FlcCheckHeader(char *filename)
{ if((flc.file=fopen(filename, "rb"))==NULL) {
    return(1);
  }

  FlcReadFile(128);

  ReadU32(&flc.HeaderSize, flc.pMembuf);
  ReadU16(&flc.HeaderCheck, flc.pMembuf+4);
  ReadU16(&flc.HeaderFrames, flc.pMembuf+6);
  ReadU16(&flc.HeaderWidth, flc.pMembuf+8);
  ReadU16(&flc.HeaderHeight, flc.pMembuf+10);
  ReadU16(&flc.HeaderDepth, flc.pMembuf+12);
  ReadU16(&flc.HeaderSpeed, flc.pMembuf+16);

#ifdef DEBUG
  printf("flc.HeaderSize: %d\n", flc.HeaderSize);
  printf("flc.HeaderCheck: %d\n", flc.HeaderCheck);
  printf("flc.HeaderFrames: %d\n", flc.HeaderFrames);
  printf("flc.HeaderWidth: %d\n", flc.HeaderWidth);
  printf("flc.HeaderHeight: %d\n", flc.HeaderHeight);
  printf("flc.HeaderDepth: %d\n", flc.HeaderDepth);
  printf("flc.HeaderSpeed: %d\n", flc.HeaderSpeed);
#endif

  if((flc.HeaderCheck==0x0AF12) || (flc.HeaderCheck==0x0AF11)) { 
    flc.screen_w=flc.HeaderWidth;
    flc.screen_h=flc.HeaderHeight;
    flc.screen_depth=8;
    if(flc.HeaderCheck==0x0AF11) {
      flc.HeaderSpeed*=1000/70;
    }
    return(0);
  }
  return(1);
} /* FlcCheckHeader */

void FlcInitFirstFrame()
{ flc.FrameSize=16;
  flc.FrameCount=0;
  if(fseek(flc.file, 128, SEEK_SET)) {
    printf("Fseek read failed\n");
    exit (1);
  };
  FlcReadFile(flc.FrameSize);
} /* FlcInitFirstFrame */

extern SDL_Surface *vga;

int StartFLI(char *nombre, char *Buffer, int Buff_anc,int Buff_alt,int cx,int cy)
{
	flc.pMembuf=NULL;
	flc.membufSize=0;
	flc.mainscreen=vga;//Buffer;
	flc.buffer=(byte *)Buffer;
	if(FlcCheckHeader(nombre)) {
		printf("Wrong header\n");
		return 0;
	}
//  SDLInit(nombre);

printf("Loaded %s\n",nombre);
FlcInitFirstFrame();
printf("Frames: %d\n",flc.HeaderFrames);
return flc.HeaderFrames;

#ifdef NOTYET
  if(animation!=NULL) EndFli();

  strcpy(TFNombre,nombre);
  TFErrorHandler_Set(Error_Reporter);
  animation=TFAnimation_NewFile(nombre);
  if(animation==NULL) return(0);
  CBuffer  =Buffer;
  CBuff_alt=Buff_alt;
  CBuff_anc=Buff_anc;
  TFOffset =cy*Buff_anc+cx;

  TFAnimation_SetLooping(animation, TF_TRUE);
  TFAnimation_SetPaletteFunction(animation, Palette_Update);
  TFAnimation_GetInfo(animation, &info);

  if((cx<0)||(cy<0)) return(0);
  if((info.Width+cx>Buff_anc)||(info.Height+cy>Buff_alt)) return(0);
  if((TFframe=(TFUByte *)malloc(info.Height*info.Width))==NULL) return(0);
  if( (TFpalette=malloc(768) )==NULL) return(0);

  TFBuffers_Set(animation, TFframe, TFpalette);
  Nextframe();
  Nextframe();
  return(info.NumFrames);
#endif
}


#ifdef NOTYET
static void Palette_Update() {
  char *d=&flc.colors;
  int n,m=0;

  for (n=0;n<192;n++) m+=*((int*)d+n);
  if (m) {

  fli_palette_update=1;
  memcpy(paleta,d,768);
  dr=now_dacout_r=dacout_r=0;
  dg=now_dacout_g=dacout_g=0;
  db=now_dacout_b=dacout_b=0;
  nueva_paleta();
  paleta_cargada=1;

  }
}

#endif

void COLORS256()
{ Uint8 *pSrc;
  Uint16 NumColorPackets;
  Uint16 NumColors;
  Uint8 NumColorsSkip;
  int i;

  pSrc=flc.pChunk+6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc+=2;
  while(NumColorPackets--) {
    NumColorsSkip=*(pSrc++);
    if(!(NumColors=*(pSrc++))) {
      NumColors=256;
    }
    i=0;
    while(NumColors--) {
      flc.colors[i].r=*(pSrc++);
      flc.colors[i].g=*(pSrc++);
      flc.colors[i].b=*(pSrc++);
      i++;
    }
//    Palette_Update();
    SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
  }
} /* COLORS256 */

void SS2()
{ Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 ColumSkip, Fill1, Fill2;
  Uint16 Lines, Count;

  pSrc=flc.pChunk+6;
  pDst=flc.buffer;//mainscreen;//->pixels;
  ReadU16(&Lines, pSrc);
  pSrc+=2;
  while(Lines--) {
    ReadU16(&Count, pSrc);
    pSrc+=2;

    while(Count & 0xc000) {
/* Upper bits 11 - Lines skip 
*/
      if((Count & 0xc000)==0xc000) {  // 0xc000h = 1100000000000000
        pDst+=(0x10000-Count)*vga_an;//flc.mainscreen->pitch;
      }

      if((Count & 0xc000)==0x4000) {  // 0x4000h = 0100000000000000
/* Upper bits 01 - Last pixel
*/
#ifdef DEBUG
            printf("Last pixel not implemented");
#endif
      }
      ReadU16(&Count, pSrc);
      pSrc+=2;
    }

    if((Count & 0xc000)==0x0000) {      // 0xc000h = 1100000000000000
      pTmpDst=pDst;
      while(Count--) {
        ColumSkip=*(pSrc++);
        pTmpDst+=ColumSkip;
        CountData=*(pSrc++);
        if(CountData>0) {
          while(CountData--) {
            *(pTmpDst++)=*(pSrc++);
            *(pTmpDst++)=*(pSrc++);
          }
        } else { 
          if(CountData<0) {
            CountData=(0x100-CountData);
            Fill1=*(pSrc++);
            Fill2=*(pSrc++);
            while(CountData--) {
              *(pTmpDst++)=Fill1;
              *(pTmpDst++)=Fill2;
            }
          }
        }
      }
      pDst+=vga_an;//=flc.mainscreen->pitch;
    } 
  }
} /* SS2 */

void DECODE_COLOR()
{ Uint8 *pSrc;
  Uint16 NumColors, NumColorPackets;
  Uint8 NumColorsSkip;
  int i;

  pSrc=flc.pChunk+6;
  ReadU16(&NumColorPackets, pSrc);
  pSrc+=2;
  while(NumColorPackets--) {
    NumColorsSkip=*(pSrc++);
    if(!(NumColors=*(pSrc++))) {
      NumColors=256;
    }
    i=0;
    while(NumColors--) {
      flc.colors[i].r=*(pSrc++)<<2;
      flc.colors[i].g=*(pSrc++)<<2;
      flc.colors[i].b=*(pSrc++)<<2;
      i++;
    }
//    Palette_Update();
    SDL_SetColors(flc.mainscreen, flc.colors, NumColorsSkip, i);
  }
} /* DECODE_COLOR  */


void DECODE_COPY()
{ Uint8 *pSrc, *pDst;
  int Lines = flc.screen_h;
  pSrc=flc.pChunk+6;
  pDst=flc.buffer;//mainscreen;/->pixels;
  while(Lines-- > 0) {
    memcpy(pDst, pSrc, flc.screen_w);
    pSrc+=flc.screen_w;
    pDst+=vga_an;//flc.mainscreen->pitch;
  }
} /* DECODE_COPY */

void _BLACK()
{ Uint8 *pDst;
  int Lines = flc.screen_h;
  pDst=flc.buffer;//mainscreen->pixels;
  while(Lines-- > 0) {
    memset(pDst, 0, flc.screen_w);
    pDst+=vga_an;//flc.mainscreen->pitch;
  }
} /* BLACK */

void DECODE_BRUN()
{ Uint8 *pSrc, *pDst, *pTmpDst, Fill;
  Sint8 CountData;
  int HeightCount, PacketsCount;

  HeightCount=flc.HeaderHeight;
  pSrc=flc.pChunk+6;
  pDst=flc.buffer;//mainscreen->pixels;
  while(HeightCount--) {
    pTmpDst=pDst;
    PacketsCount=*(pSrc++);
    while(PacketsCount--) {
      CountData=*(pSrc++);
      if(CountData>0) {
        Fill=*(pSrc++);
        while(CountData--) {
          *(pTmpDst++)=Fill;
        }
      } else { 
        if(CountData<0) {
          CountData=(0x100-CountData);
          while(CountData--) {
          *(pTmpDst++)=*(pSrc++);
          }
        }
      }
    }
    pDst+=vga_an;//flc.mainscreen->pitch;
  }
} /* DECODE_BRUN */


void DECODE_LC() 
{ Uint8 *pSrc, *pDst, *pTmpDst;
  Sint8 CountData;
  Uint8 CountSkip;
  Uint8 Fill;
  Uint16 Lines, tmp;
  int PacketsCount;

  pSrc=flc.pChunk+6;
  pDst=flc.buffer;//mainscreen->pixels;

  ReadU16(&tmp, pSrc);
  pSrc+=2;
  pDst+=vga_an;//tmp*flc.mainscreen->pitch;
  ReadU16(&Lines, pSrc);
  pSrc+=2;
  while(Lines--) {
    pTmpDst=pDst;
    PacketsCount=*(pSrc++);
    while(PacketsCount--) {
      CountSkip=*(pSrc++);
      pTmpDst+=CountSkip;
      CountData=*(pSrc++);
      if(CountData>0) {
        while(CountData--) {
          *(pTmpDst++)=*(pSrc++);
        }
      } else { 
        if(CountData<0) {
          CountData=(0x100-CountData);
          Fill=*(pSrc++);
          while(CountData--) {
            *(pTmpDst++)=Fill;
          }
        }
      }
    }
    pDst+=vga_an;//flc.mainscreen->pitch;
  }
} /* DECODE_LC */


int FlcCheckFrame()
{ flc.pFrame=flc.pMembuf+flc.FrameSize-16;
  ReadU32(&flc.FrameSize, flc.pFrame+0);
  ReadU16(&flc.FrameCheck, flc.pFrame+4);
  ReadU16(&flc.FrameChunks, flc.pFrame+6);

#ifdef DEBUG
  printf("flc.FrameSize: %d\n", flc.FrameSize);
  printf("flc.FrameCheck: %d\n", flc.FrameCheck);
  printf("flc.FrameChunks: %d\n", flc.FrameChunks);
#endif

  flc.pFrame+=16;
  if(flc.FrameCheck==0x0f1fa) { 
    return(0);
  }

  if(flc.FrameCheck==0x0f100) { 
#ifdef DEBUG
    printf("Ani info!!!\n");
#endif
    return(0);
  }

  return(1);
} /* FlcCheckFrame */


extern SDL_Surface *vga;

void FlcDoOneFrame()
{ int ChunkCount; 
  ChunkCount=flc.FrameChunks;
  flc.pChunk=flc.pMembuf;
//  if ( SDL_LockSurface(flc.mainscreen) < 0 )
//    return;
  while(ChunkCount--) {
    ReadU32(&flc.ChunkSize, flc.pChunk+0);
    ReadU16(&flc.ChunkType, flc.pChunk+4);

#ifdef DEBUG
    printf("flc.ChunkSize: %d\n", flc.ChunkSize);
    printf("flc.ChunkType: %d\n", flc.ChunkType);
#endif

    switch(flc.ChunkType) {
      case 4:
        COLORS256();
      break;
      case 7:
        SS2();
      break;
      case 11:
        DECODE_COLOR();
      break;
      case 12:
        DECODE_LC();
      break;
      case 13:
        _BLACK();
      break;
      case 15:
        DECODE_BRUN();
      break;
      case 16:
        DECODE_COPY();
      break;
      case 18:
#ifdef DEBUG
        printf("Chunk 18 not yet done.\n");
#endif
      break;
      default:
        printf("Ieek an non implemented chunk type: %d\n", flc.ChunkType);
    }
    
    flc.pChunk+=flc.ChunkSize;
  }
//  SDL_UnlockSurface(flc.mainscreen);
} /* FlcDoOneFrame */
int Nextframe()
{
	flc.FrameCount++;
	
	if(flc.FrameCount>flc.HeaderFrames)
		return 0;
    
    if(FlcCheckFrame()) {
      if (flc.FrameCount<=flc.HeaderFrames) {
        printf("Frame failure -- corrupt file?\n");
        return 0;
      } else {
		  return 0;
	  }
    }
     FlcReadFile(flc.FrameSize);

    if(flc.FrameCheck!=0x0f100) {
      FlcDoOneFrame();
//      SDLWaitFrame();
      /* TODO: Track which rectangles have really changed */
      SDL_UpdateRect(flc.mainscreen, 0, 0, 0, 0);
    }

#ifdef NOTYET
int i;
  if(animation==NULL) return 0;
  TFFrame_Decode(animation);
  for(i=0; i<info.Height; i++) memcpy(CBuffer+TFOffset+i*CBuff_anc, TFframe+i*info.Width, info.Width);
  TFAnimation_GetInfo(animation, &info);
  if(info.CurFrame<info.NumFrames) return(1);
#endif
return(flc.FrameCount);
}

void EndFli()
{
	if(flc.file!=NULL)
		fclose(flc.file);
	if(flc.pMembuf!=NULL)
		free(flc.pMembuf);

	flc.pMembuf=NULL;
	flc.file=NULL;

#ifdef NOTYET
  if(animation==NULL) return;
  TFAnimation_Delete(animation);
  animation=NULL;
  free(TFframe);
  free(TFpalette);
#endif
}

void ResetFli()
{
#ifdef NOTYET
  if(animation==NULL) return;
  TFAnimation_Delete(animation);
  animation=TFAnimation_NewFile(TFNombre);
  if(animation==NULL) return;
  TFBuffers_Set(animation, TFframe, TFpalette);
  Nextframe();
#endif
}

int quit_warning;

static void Error_Reporter(char *msg)
{
#ifdef NOTYET
  quit_warning=(int)msg;
#endif
}

static void Palette_Update(TFUByte (*TFpalette)[256][3]) {
#ifdef NOTYET
  char *d=(char*)TFpalette;
  int n,m=0;

  for (n=0;n<192;n++) m+=*((int*)d+n);
  if (m) {

  fli_palette_update=1;
  memcpy(paleta,d,768);
  dr=now_dacout_r=dacout_r=0;
  dg=now_dacout_g=dacout_g=0;
  db=now_dacout_b=dacout_b=0;
  nueva_paleta();
  paleta_cargada=1;

  }
#endif
}
