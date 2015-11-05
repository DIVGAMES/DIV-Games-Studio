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

int StartFLI(char *nombre, char *Buffer, int Buff_anc,int Buff_alt,int cx,int cy)
{
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

int Nextframe()
{
#ifdef NOTYET
int i;
  if(animation==NULL) return 0;
  TFFrame_Decode(animation);
  for(i=0; i<info.Height; i++) memcpy(CBuffer+TFOffset+i*CBuff_anc, TFframe+i*info.Width, info.Width);
  TFAnimation_GetInfo(animation, &info);
  if(info.CurFrame<info.NumFrames) return(1);
#endif
return(0);
}

void EndFli()
{
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
  quit_warning=(int)msg;
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
