
#include "global.h"
#include "divsound.h"
#include "divmixer.hpp"
#include "divsb.h"

//#include <io.h>

// HACK
byte DmaBuf=0;
unsigned short dmacount ( void ) { return 0; }
unsigned char *aligned[2];
// END HACK

int    FilePos=0;
  
void FreeMOD                (void);
int  GetSongPos             (void);
int  GetSongLine            (void);
void show_mod_meters     (void);
void OpenSound              (void);
void OpenSoundFile          (void);
void OpenDesktopSound       (FILE *f);
void SaveSound              (pcminfo *mypcminfo, char *dst);
void SaveDesktopSound       (pcminfo *mypcminfo, FILE *f);
void CloseSound             (char *snd_path);
void OpenSong               (void);
void OpenDesktopSong        (void);
void PlaySong               (char *pathname);
void EditSound0             (void);
void EditSound1             (void);
void EditSound2             (void);
void EditSound3             (void);
void RecSound0              (void);
void RecSound1              (void);
void RecSound2              (void);
void RecSound3              (void);
void ModifySound            (int option);
void ChangeSoundFreq        (int freq);
void RecordSound            (void);
void PollRecord             (void);
int  JudasProgressRead      (int handle, void *buffer, int size);
void CopyNewSound           (pcminfo *mypcminfo, int begin, int end);
void PasteNewSounds         (void);
byte *SaveSoundMem          (pcminfo *mypcminfo);
int  IsWAV                  (char *FileName);
int  NewSample              (pcminfo *mypcminfo); // 1-OK, 0-ERROR
                                                  // Espacio (malloc+lock) en mypcminfo->SoundData para mypcminfo->SoundSize shorts
void wline                  (char *ptr,int realwidth,int w,int h,int x0, int y0, int x1, int y1, char color);
void wave_line_pixel            (char * ptr, int w, int realwidth, int h, int x, int y, char color);

void set_mixer(void);
int  determine_pcm(void);

char SoundName[255];
char SoundPathName[256];
char SongName[255];
char SongPathName[256];
byte *pcminfo_aux;
byte *modinfo_aux;
int  SongType=0;
int  SongCode=10;
int  SongChannels=0;
int  last_mod_clean=1;
int  wav_handle;
char SoundFile[256];
int  RecDevice[2] = { 1, 0 };
int  ModButton=0;
int  ModWindow=-1;
extern int clock_ticks;

void errhlp0(void);

//═════════════════════════════════════════════════════════════════════════════
//  Sounds Window
//═════════════════════════════════════════════════════════════════════════════

void PCM1(void) {
  int length;
  float step,position=0;
  int x,y,y0,y1,p0,p1;
  byte * ptr=v.ptr;
  int w=v.w,h=v.h;
  int Width,Height,First=1,lx,ly;
  pcminfo *mypcminfo;
  short *buffer;
  short sample;

  if (big) { w/=2; h/=2; }
  mypcminfo=(pcminfo *)v.aux;

  wbox(ptr,w,h,c1,2,10,w-4,h-12);
  Width=v.w-4*big2;
  Height=v.h-12*big2;
  ptr+=(2+(10*v.w))*big2+(Height/2)*v.w;
  buffer=mypcminfo->SoundData;
  length=mypcminfo->SoundSize;

  if (length>1)
  if (length<3*Width)
  {
    step = (float)Width/(float)(length-1);
    for(x=0; x<length; x++)
    {
      sample=buffer[x], y=(sample*Height/65536);

      if(First)
      {
        First = 0;
        lx    = (int)position;
        ly    = y;
      }
      else
      {
        wline((char *)ptr, v.w, Width, Height, lx, ly, (int)position, y, c_g_low);
        lx = (int)position;
        ly = y;
      }
      position += step;
    }
  }
  else
  {
    step=(float)length/(float)Width;
    for(x=0;x<Width;x++)
    {
      p0=(memptrsize)position;
      position+=step;
      p1=(memptrsize)position;

      sample=buffer[p0], y0=y1=(sample*Height/65536);

      do {
        sample=buffer[p0], y=(sample*Height/65536);
        if (y<y0) y0=y; else if (y>y1) y1=y;
        p0+=2;
      } while (p0<p1);

      y=y0;
      do {
        ptr[x+y*v.w]=c_g_low;
      } while (y++<y1);
    }
  }
}

extern int ns,chn;
extern int bload;
int sound_window;

void PCM2(void) {
  pcminfo *mypcminfo=(pcminfo *)v.aux;

  if(mouse_b&1) {
    if ( false /*judascfg_device == DEV_NOSOUND */) {
      if ( SoundError ) {
        v_text=(char *)texts[549]; dialog(errhlp0);
        if (v_accept) help(2008);
      } else {
        v_text=(char *)texts[548]; dialog(errhlp0);
        if (v_accept) help(2009);
      }
      return;
    } else {
      if(mypcminfo->SoundData) {
#ifdef MIXER
		  Mix_PlayChannel(0, mypcminfo->SI, 0);
#endif
		}
      while (mouse_b&1) read_mouse();
    }
  }
}

void PCM3(void) {
  pcminfo *mypcminfo=(pcminfo *)v.aux;

  if(mypcminfo->SoundData) {
#ifdef MIXER
	  Mix_FreeChunk(mypcminfo->SI);
	  mypcminfo->SI=NULL;
#endif
    //judas_freesample(mypcminfo->sample);
    mypcminfo->SoundData=NULL;
    mypcminfo->SoundSize=0;
  }
  free(v.aux);
}

void PCM0(void) {
  pcminfo *mypcminfo;

  v.type=105;
  v.w=80;
  v.h=50;

  v.paint_handler=PCM1;
  v.click_handler=PCM2;
  v.close_handler=PCM3;

  v.aux=pcminfo_aux;
  pcminfo_aux=NULL;
  mypcminfo=(pcminfo *)v.aux;

  v.title=(byte *)mypcminfo->name;
  v.name=(byte *)mypcminfo->name;
}

//═════════════════════════════════════════════════════════════════════════════
//  Song (mod/s3m/xm) window
//═════════════════════════════════════════════════════════════════════════════

void MOD1(void) {
  modinfo *mymodinfo=(modinfo *)v.aux;
  int w=v.w/big2, h=v.h/big2;

  _show_items();

  wbox       (v.ptr, w, h, c1, 2, 10, w-4, h-12-10);
  wbox       (v.ptr, w, h, c2, 2, 39, w-4,        9);
  wrectangle(v.ptr, w, h, c0, 1, 38, w-2,       11);
  wrectangle(v.ptr, w, h, c0, 1, 38,   21,       11);

  if(Mix_PlayingMusic() && mymodinfo->SongCode == SongCode) {
    if(ModButton && ModWindow==v.order) wput(v.ptr, w, h, 2, 39, -214);
    else                                wput(v.ptr, w, h, 2, 39, -234);
  } else {
    if(ModButton && ModWindow==v.order) wput(v.ptr, w, h, 2, 39, -215);
    else                                wput(v.ptr, w, h, 2, 39, -235);
  }
}

void MOD2(void) {
  modinfo *mymodinfo=(modinfo *)v.aux;
  int need_refresh=0;

  _process_items();

  ModButton = 0;
  if(wmouse_in(2, 39, 19, 9))
  {
    if( mouse_b&1 )
    {
      ModButton = 1;
      ModWindow = v.order;
      need_refresh=1;
    }
    else if( old_mouse_b&1 )
    {
      ModWindow=v.order;
      need_refresh=1;
#ifdef MIXER
      if(Mix_PlayingMusic() && mymodinfo->SongCode==SongCode) {
        FreeMOD();
        Mix_FreeMusic(mymodinfo->music);
      } else {
        FreeMOD();
        PlaySong(mymodinfo->pathname);
      }
#endif
    }
  } else if(ModWindow != -1) ModWindow = -1, need_refresh=1;

  if(need_refresh)
  {
    call(v.paint_handler);
    v.blit=1;
  }
}

void MOD3(void) {
  modinfo *mymodinfo=(modinfo *)v.aux;
#ifdef MIXER
  if(mymodinfo->SongCode == SongCode) {
    FreeMOD();
    Mix_FreeMusic(mymodinfo->music);
  }
#endif
  free(v.aux);
}

void MOD0(void) {
  modinfo *mymodinfo;

  v.type=107;
  v.w=68;
  v.h=50;

  v.paint_handler=MOD1;
  v.click_handler=MOD2;
  v.close_handler=MOD3;

  v.aux=modinfo_aux;
  modinfo_aux=NULL;
  mymodinfo=(modinfo *)v.aux;

  mymodinfo->SongCode=0;

  v.title=(byte *)mymodinfo->name;
  v.name=(byte *)mymodinfo->name;
}

void FreeMOD(void)
{
  Mix_HaltMusic();
  Mix_SetPostMix(NULL, NULL);
/*  switch(SongType)
  {
    case XM:   judas_freexm();  break;
    case S3M: judas_frees3m(); break;
    case MOD: judas_freemod(); break;
  }
*/
  SongType=0;
}
int songpos;
int songline;
int GetSongPos(void)
{
  int pos;
// TODO - JUDAS GET SONG POS
/*
  switch(SongType)
  {
    case XM:  pos=judas_getxmpos();  break;
    case S3M: pos=judas_gets3mpos(); break;
    case MOD: pos=judas_getmodpos(); break;
  }

  return(pos);
  * */
  return songpos;
  
}

int GetSongLine(void)
{
  int pos;
/*
  switch(SongType)
  {
    case XM:  pos=judas_getxmline();  break;
    case S3M: pos=judas_gets3mline(); break;
    case MOD: pos=judas_getmodline(); break;
  }

  return(pos);
  */
  return songline;
}

void show_mod_meters(void)
{
	
  modinfo *mymodinfo=(modinfo *)v.aux;
  int     w=v.w/big2, h=v.h/big2;
  int     x, y, ctr, channel, bar_width;
  int     begin=v.w*v.h-v.w*((2+10)*big2+1)+2*big2;
  char    cwork[10];

  if(mymodinfo->SongCode == SongCode)
  {
    if(Mix_PlayingMusic())
    {
      wbox(v.ptr, w, h, c1,  2, 10,    w-4, h-12-10);
      wbox(v.ptr, w, h, c2, 22, 39, w-4-20,        9);

//      wbox(v.ptr, w, h, c1, 22, 39, 6, 9);
//      wbox(v.ptr, w, h, c1, 44, 39, 6, 9);

      wwrite(v.ptr, w, h, 29, 40, 2, "L ", c1);
      wwrite(v.ptr, w, h, 28, 40, 2, "L ", c4);
      sprintf(cwork,"%03d",GetSongLine());
      wwrite(v.ptr, w, h, 42, 40, 2, cwork, c3);

      wwrite(v.ptr, w, h, 52, 40, 2, "P ", c1);
      wwrite(v.ptr, w, h, 51, 40, 2, "P ", c4);
      sprintf(cwork,"%03d",GetSongPos());
      wwrite(v.ptr, w, h, 64, 40, 2, cwork, c3);

      bar_width = (v.w-4*big2)/SongChannels;

      for(channel=0; channel<SongChannels; channel++)
      {
        x = bar_width*channel;
        y = (int)rand()%25*big2;//(judas_getvumeter(canal)*(float)25)*big2;

        for(ctr=0; ctr<y; ctr++)
        {
          if(channel&1) memset(v.ptr+begin+x-ctr*v.w, c_g_low, bar_width);
          else        memset(v.ptr+begin+x-ctr*v.w,     c_g, bar_width);
        }
      }
    }
    else
    {
      SongCode++;
      last_mod_clean = 0;
    }
    v.blit=1;
  }
  else if(mymodinfo->SongCode == SongCode-1)
  {
    if(!last_mod_clean)
    {
//    wbox(v.ptr,w,h,c1,2,10,w-4,h-12-10);
      call(v.paint_handler);
      v.blit=1;
      last_mod_clean = 1;
    }
  }
  
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones de sonidos
//═════════════════════════════════════════════════════════════════════════════

#define max_files 512 // ─────────────────────────────── Listbox de archivos
extern struct t_listboxbr lfilesbr;
extern t_thumb thumb[max_files];
extern int num_taggeds;

typedef struct _HeadDC {
  unsigned int   dwUnknow;
  unsigned short wFormatTag;
  unsigned short wChannels;
  unsigned int   dwSamplePerSec;
  unsigned int   dwAvgBytesPerSec;
  unsigned short wBlockAlign;
  unsigned short wBits;
} HeadDC;

#ifdef MIXER
Mix_Chunk *DIVMIX_LoadPCM(char *path) {
  FILE *f;
  HeadDC MyHeadDC;
  char *riff="RIFF";
  char *wavefmt = "WAVEfmt ";
  char *data = "data";
  byte *dst; 
  byte *ptr; 
  int32_t iLen, Len;
  SDL_RWops *rw;
  Mix_Chunk *smp = NULL;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  _splitpath(full,(char *)drive,(char *)dir,(char *)fname,(char *)ext);
  strupr((char *)ext);

  // if file isnt a pcm, reject
  if(strcmp(ext,".PCM")) {
    return NULL;
  }

  f=fopen(full,"r");
  if(f) {
    fseek(f,0,SEEK_END);
    Len = ftell(f);
    fseek(f,0,SEEK_SET);

    dst = (byte *)malloc((int)Len+50);
    ptr = (byte *)malloc((int)Len+50);

    iLen = Len + 50;

    if(dst==NULL)
      return(-1);

    memset(dst,0,(int)Len+40);
    fread(ptr,1,Len,f);
    fclose(f);

    MyHeadDC.dwUnknow         = 16;
    MyHeadDC.wFormatTag       = 1;
    MyHeadDC.wChannels        = 1;
    MyHeadDC.dwSamplePerSec   = 11025;
    MyHeadDC.dwAvgBytesPerSec = 11025;
    MyHeadDC.wBlockAlign      = 1;
    MyHeadDC.wBits            = 8;

    memcpy(dst,riff,4);
    memcpy(dst+4,&iLen,4);
    memcpy(dst+8,wavefmt,8);
    memcpy(dst+16,&MyHeadDC,sizeof(HeadDC));
    memcpy(dst+16+sizeof(HeadDC),data,4);
    memcpy(dst+20+sizeof(HeadDC),&Len,4);
    memcpy(dst+24+sizeof(HeadDC),ptr,Len);

    rw = SDL_RWFromMem((void *)dst, (int)(Len+24+sizeof(HeadDC)));
    smp = Mix_LoadWAV_RW(rw, 1);
    free(dst);
    free(ptr);
  }
  return smp;

}
#endif

void OpenSound(void) {
  pcminfo   *mypcminfo;
  Uint32 wav_length;
  Uint8 *wav_buffer;

#ifdef MIXER

Mix_Chunk *SI;

#endif

//  SoundInfo *SI;

  int       num;

  v_mode=0;
  v_type=7;
  v_text=(char *)texts[341];
  dialog(browser0);
  if(!v_finished) return;

  if(!num_taggeds) {
    strcpy(full,type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_exists=1;
    } else v_exists=0;
    strcpy(lfilesbr.list,input);
    lfilesbr.maximum=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);
      strcpy(SoundName,input);
      strcpy(SoundPathName,full);

      if (!v_exists) {
        v_text=(char *)texts[43];
        dialog(err0);
        continue;
      }

      if((pcminfo_aux=(byte *)malloc(sizeof(pcminfo)))==NULL)
      {
        v_text=(char *)texts[45];
        dialog(err0);
        continue;
      }
      
      memset(pcminfo_aux,0,sizeof(pcminfo));  

      mypcminfo=(pcminfo *)pcminfo_aux;

#ifdef MIXER

	SI = Mix_LoadWAV(SoundPathName);
	
	if(SI==NULL) 
    SI=DIVMIX_LoadPCM(SoundPathName);

  if(SI==NULL) {

//		 free(pcminfo_aux);
        //if(SI) free(SI);
        v_text=(char *)texts[46];
        dialog(err0);
        continue;
   }
		
#endif
	
#ifdef NOTYET
      SI = judas_loadwav(SoundPathName);
      if(judas_error != JUDAS_OK && judas_error == JUDAS_WRONG_FORMAT)
      {
        if(SI) free(SI);
        SI = judas_loadrawsample(SoundPathName, 0, 0, 0);
      }
      if(SI==NULL || judas_error != JUDAS_OK)
      {
        free(pcminfo_aux);
        if(SI) free(SI);
        v_text=(char *)texts[46];
        dialog(err0);
        continue;
      }

      memcpy(mypcminfo->name,SoundName,14);
      memcpy(mypcminfo->pathname,SoundPathName,256);
      mypcminfo->SoundFreq = SI->SoundFreq;
      mypcminfo->SoundBits = SI->SoundBits;
      mypcminfo->SoundSize = SI->SoundSize;
      mypcminfo->SoundData = SI->SoundData;
      mypcminfo->sample    = SI->sample;

      free(SI);
#endif
      memcpy(mypcminfo->name,SoundName,14);
      memcpy(mypcminfo->pathname,SoundPathName,256);
	  mypcminfo->SoundFreq = 44100;
      mypcminfo->SoundBits = 16;
#ifdef MIXER
      mypcminfo->SoundSize = SI->alen/2;
      mypcminfo->SoundData = (short *)malloc(SI->alen);
      if(mypcminfo->SoundData!=NULL) 
		memcpy(mypcminfo->SoundData,SI->abuf, SI->alen);
      
//      mypcminfo->SoundData = (short *)SI->abuf;
      mypcminfo->SI = SI;
//      mypcminfo->sample    = (char *)wav_buffer;    
#endif
      new_window(PCM0);
    }
  }
  
}

//#define SoundInfo char

void OpenSoundFile(void) // Open the file SoundPathName
{
	fprintf(stdout,"TODO - divpcm.cpp OpenSoundFile\n");

  pcminfo   *mypcminfo;
#ifdef MIXER
  Mix_Chunk *SI=NULL;
#endif
debugprintf("SoundName %s\n",input);
debugprintf("SOundPath %s\n",full);


  strcpy(SoundName,input);
  strcpy(SoundPathName,full);

//CloseSound(SoundPathName);

  if((pcminfo_aux=(byte *)malloc(sizeof(pcminfo)))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }
  mypcminfo=(pcminfo *)pcminfo_aux;

#ifdef MIXER

  SI = Mix_LoadWAV(SoundPathName);
  
  if(SI==NULL) 
    SI=DIVMIX_LoadPCM(SoundPathName);

  if(SI==NULL) {

//     free(pcminfo_aux);
        //if(SI) free(SI);
      v_text=(char *)texts[46];
      dialog(err0);
      free(mypcminfo);
      return;        
   }
    
#endif
/*  SI = judas_loadwav(SoundPathName);
  if(judas_error == JUDAS_WRONG_FORMAT)
  {
    if (SI) free(SI);
    SI = judas_loadrawsample(SoundPathName, 0, 0, 0);
  }

  if(SI==NULL || judas_error!=JUDAS_OK)
  {
    free(pcminfo_aux);
    if(SI) free(SI);
    v_text=(char *)texts[46];
    dialog(err0);
    return;
  }
*/
  memcpy(mypcminfo->name,SoundName,14);
  memcpy(mypcminfo->pathname,SoundPathName,256);
  mypcminfo->SoundFreq = 44100;
  mypcminfo->SoundBits = 16;
#ifdef MIXER
  mypcminfo->SoundSize = SI->alen/2;
  mypcminfo->SoundData = (short *)malloc(SI->alen);
  if(mypcminfo->SoundData!=NULL) 
    memcpy(mypcminfo->SoundData,SI->abuf, SI->alen);

//  mypcminfo->SoundData = (short *)SI->abuf;
  mypcminfo->SI = SI;

//  free(SI);
#endif
  new_window(PCM0);

}
int new_window_load(voidReturnType init_handler,int nx,int ny);
//void new_window_load(int init_handler,int nx,int ny);
extern struct twindow window_aux;

void OpenDesktopSound(FILE *f)
{
  pcminfo *mypcminfo;

  pcminfo_aux=(byte *)malloc(sizeof(pcminfo));
  if(pcminfo_aux==NULL) return; // OJO !!!
  mypcminfo=(pcminfo *)pcminfo_aux;

  fread(mypcminfo->name,       1,  14, f);
  fread(mypcminfo->pathname,   1, 256, f);
  fread(&mypcminfo->SoundFreq, 1,   4, f);
  fread(&mypcminfo->SoundBits, 1,   4, f);
  fread(&mypcminfo->SoundSize, 1,   4, f);

  if(!NewSample(mypcminfo))
  {
    free(pcminfo_aux);
    fseek(f, mypcminfo->SoundSize*2, SEEK_CUR);
    return;
  }

  fread(mypcminfo->SoundData, 2, mypcminfo->SoundSize, f);

/*
  smp = judas_allocsample(mypcminfo->SoundSize*2);
  mypcminfo->SoundData = (short *)smp->start;
  mypcminfo->sample    = smp;
  fread(mypcminfo->SoundData, 2, mypcminfo->SoundSize, f);
  smp->repeat    = smp->start;
  smp->end       = smp->start + mypcminfo->SoundSize*2;
  smp->voicemode = VM_ON | VM_16BIT;
  judas_ipcorrect(smp);
*/

  new_window_load(PCM0,window_aux.x,window_aux.y);

}

void SaveSound(pcminfo *mypcminfo, char *dst)
{
	printf("TODO - divpcm.cpp SaveSound\n");
  FILE   *dstfile;
  HeadDC MyHeadDC;
  int    length;
  int    ctr;
  byte   *byte_ptr=(byte *)mypcminfo->SoundData;
  float  step,pos;
  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  _splitpath(dst,(char *)drive,(char *)dir,(char *)fname,(char *)ext);
  strupr((char *)ext);
  length=mypcminfo->SoundSize;

  if((dstfile=fopen(dst,"wb"))==NULL)
  {
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }

  if(!strcmp(ext,".WAV"))
  {
    if(mypcminfo->SoundBits==16) length *= 2;
    fputc('R',dstfile);
    fputc('I',dstfile);
    fputc('F',dstfile);
    fputc('F',dstfile);

    length+=36;
    if(fwrite(&length,1,4,dstfile)!=4)
    {
      fclose(dstfile);
      DaniDel(dst);
      v_text=(char *)texts[47];
      dialog(err0);
      return;
    }
    length-=36;

    fputc('W',dstfile);
    fputc('A',dstfile);
    fputc('V',dstfile);
    fputc('E',dstfile);
    fputc('f',dstfile);
    fputc('m',dstfile);
    fputc('t',dstfile);
    fputc(' ',dstfile);

    MyHeadDC.dwUnknow         = 16;
    MyHeadDC.wFormatTag       = 1;
    MyHeadDC.wChannels        = 1;
    MyHeadDC.dwSamplePerSec   = mypcminfo->SoundFreq;
    MyHeadDC.dwAvgBytesPerSec = mypcminfo->SoundFreq*(mypcminfo->SoundBits/8);
    MyHeadDC.wBlockAlign      = 1;
    MyHeadDC.wBits            = mypcminfo->SoundBits;

    if(fwrite(&MyHeadDC,1,sizeof(HeadDC),dstfile)!=sizeof(HeadDC))
    {
      fclose(dstfile);
      DaniDel(dst);
      v_text=(char *)texts[47];
      dialog(err0);
      return;
    }

    fputc('d',dstfile);
    fputc('a',dstfile);
    fputc('t',dstfile);
    fputc('a',dstfile);

    if(fwrite(&length,1,4,dstfile)!=4)
    {
      fclose(dstfile);
      DaniDel(dst);
      v_text=(char *)texts[47];
      dialog(err0);
      return;
    }
    if(mypcminfo->SoundBits==8)
    {
      if((byte_ptr=(byte *)malloc(length))==NULL)
      {
        fclose(dstfile);
        DaniDel(dst);
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }
      for(ctr=0; ctr<length; ctr++) byte_ptr[ctr]=mypcminfo->SoundData[ctr]/0xFF+0x80;
    }
  }
  else
  {
    step   = (float)mypcminfo->SoundFreq/(float)11025;
    pos    = (float)length/step;
    length = (int)(pos+0.5);
    if((byte_ptr=(byte *)malloc(length))==NULL)
    {
      fclose(dstfile);
      DaniDel(dst);
      v_text=(char *)texts[45];
      dialog(err0);
      return;
    }
    pos = 0;
    for(ctr=0; ctr<length; ctr++)
    {
      byte_ptr[ctr]=mypcminfo->SoundData[(int)pos]/0xFF+0x80;
      pos+=step;
    }
  }

  if(fwrite(byte_ptr,1,length,dstfile)!=length)
  {
    fclose(dstfile);
    if(byte_ptr != (byte *)mypcminfo->SoundData) free(byte_ptr);
    DaniDel(dst);
    v_text=(char *)texts[47];
    dialog(err0);
    return;
  }

  if(byte_ptr != (byte *)mypcminfo->SoundData) free(byte_ptr);
  fclose(dstfile);


}

void SaveDesktopSound(pcminfo *mypcminfo, FILE *f)
{
  fwrite(mypcminfo->name,       1,  14, f);
  fwrite(mypcminfo->pathname,   1, 256, f);
  fwrite(&mypcminfo->SoundFreq, 1,   4, f);
  fwrite(&mypcminfo->SoundBits, 1,   4, f);
  fwrite(&mypcminfo->SoundSize, 1,   4, f);
  fwrite(mypcminfo->SoundData,  2, mypcminfo->SoundSize, f);
}

void CloseSound(char *snd_path)
{
  pcminfo *mypcminfo;
  int m;

  for(m=0; m<max_windows; m++)
  {
    if(window[m].type==105)
    {
      mypcminfo = (pcminfo *)window[m].aux;
      if(!strcmp(snd_path, mypcminfo->pathname))
      {
        move(0,m);
        close_window();
        break;
      }
    }
  }
}

//═════════════════════════════════════════════════════════════════════════════
//  Funciones de canciones
//═════════════════════════════════════════════════════════════════════════════

void OpenSong(void) {
  int num;
  modinfo *mymodinfo;

  v_mode=0;
  v_type=16;
  v_text=(char *)texts[558];
  dialog(browser0);
  if(!v_finished) return;

  if(!num_taggeds) {
    strcpy(full,type[v_type].path);
    if (full[strlen(full)-1]!='/') strcat(full,"/");
    strcat(full, input);
    if ((f=fopen(full,"rb"))!=NULL) {
      fclose(f);
      v_exists=1;
    } else v_exists=0;
    strcpy(lfilesbr.list,input);
    lfilesbr.maximum=1;
    thumb[0].tagged=1;
    num_taggeds=1;
  }

  for(num=0; num<lfilesbr.maximum; num++)
  {
    if(thumb[num].tagged)
    {
      strcpy(input,lfilesbr.list+lfilesbr.list_width*num);
      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);
      strcpy(SongName,input);
      strcpy(SongPathName,full);

      if (!v_exists) {
        v_text=(char *)texts[43];
        dialog(err0);
        continue;
      }

      if((modinfo_aux=(byte *)malloc(sizeof(modinfo)))==NULL)
      {
        v_text=(char *)texts[45];
        dialog(err0);
        continue;
      }
      mymodinfo=(modinfo *)modinfo_aux;

      memcpy(mymodinfo->name,SongName,14);
      memcpy(mymodinfo->pathname,SongPathName,256);

      new_window(MOD0);
    }
  }
}

void OpenDesktopSong(void) {
debugprintf("TODO - divpcm.cpp OpenDesktopSong\n");

#ifdef NOTYET
  modinfo *mymodinfo;

  if((modinfo_aux=(char *)malloc(sizeof(modinfo)))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }
  mymodinfo=(modinfo *)modinfo_aux;

  if(judas_channel[0].smp) judas_stopsample(0);

  judas_loadxm(SongPathName);
  if(judas_error != JUDAS_OK && judas_error == JUDAS_WRONG_FORMAT)
  {
    judas_loads3m(SongPathName);
    if(judas_error != JUDAS_OK && judas_error == JUDAS_WRONG_FORMAT)
    {
      judas_loadmod(SongPathName);
    }
  }
  if(judas_error != JUDAS_OK)
  {
    free(modinfo_aux);
    v_text=(char *)texts[46];
    dialog(err0);
    return;
  }

  memcpy(mymodinfo->name,SongName,14);
  memcpy(mymodinfo->pathname,SongPathName,256);

  new_window_load((int)MOD0,window_aux.x,window_aux.y);
#endif

}

int songposcount=0;
// make a passthru processor function that does nothing...
void noEffect(void *udata, Uint8 *stream, int len)
{
  songposcount++;
  if(songposcount==32) {
    songpos++;
    songposcount=0;
    if(songpos==64) {
      songpos=1;
      songline++;
      // stop overflow
      if(songline==1000)
        songline=0;
    }
  }
    // you could work with stream here...
//  if(udata!=NULL)
//    fprintf(stdout,"%c %x %x %d\n",((char *)udata)[0],udata,stream,len);
//  memset(stream,255,len);
}

void PlaySong(char *pathname)
{
	printf("TODO - divpcm.cpp PlaySong\n");
#ifdef MIXER
	Mix_Music *music;
	modinfo *mymodinfo=(modinfo *)v.aux;
  SongCode++;
  mymodinfo->SongCode=SongCode;
  last_mod_clean=0;

	printf("loadsong [%s]\n",pathname);

	music=Mix_LoadMUS(pathname);
	
  mymodinfo->music=music;

	if(!music) {
		v_text=strdup((char *)Mix_GetError());//texts[46];
    if(strlen(v_text)==0) {
      free(v_text);
      v_text=strdup(texts[46]);
    }
		dialog(err0);
    free(v_text);
		return;
	}
  songposcount=0;
  songpos=1;
  songline=1;
  Mix_SetPostMix(noEffect, NULL);
	if(Mix_PlayMusic(music, 1)==-1) {
    debugprintf("Mix_PlayMusic: %s\n", Mix_GetError());
    // well, there's no music, but most games don't break without music...
  }
  
  SongChannels = 8;

#ifdef NOTYET
  if(judas_channel[0].smp) judas_stopsample(0);

  FreeMOD();
  SongCode++;
  mymodinfo->SongCode=SongCode;
  last_mod_clean=0;

  judas_loadxm(pathname);
  if(judas_error == JUDAS_OK) { judas_playxm(1000000); SongType=XM; }
  else if(judas_error == JUDAS_WRONG_FORMAT)
  {
    judas_loads3m(pathname);
    if(judas_error == JUDAS_OK) { judas_plays3m(1000000); SongType=S3M; }
    else if(judas_error == JUDAS_WRONG_FORMAT)
    {
      judas_loadmod(pathname);
      if(judas_error == JUDAS_OK) { judas_playmod(1000000); SongType=MOD; }
    }
  }

  if(judas_error != JUDAS_OK)
  {
    v_text=(char *)texts[46];
    dialog(err0);
    return;
  }

  switch(SongType)
  {
    case XM:  SongChannels = judas_getxmchannels();  break;
    case S3M: SongChannels = judas_gets3mchannels(); break;
    case MOD: SongChannels = judas_getmodchannels(); break;
  }

#endif

#endif


}

//═════════════════════════════════════════════════════════════════════════════
//  Editor de sonidos
//═════════════════════════════════════════════════════════════════════════════

int window_width, window_height;
int sel_1, sel_2;

typedef struct _CLP {
  int    SoundFreq;
  int    SoundSize;
  short  *SoundData;
} CLP;

typedef struct _SND {
  char  name[14];
  int   SoundFreq;
  int   SoundBits;
  int   SoundSize;
  short *SoundData;
} SND;

SND     DesktopSND[100];
int     NumSND=0;   // Contador de sonidos pegados durante una edicion
int     ConSND=1; // Contador de sonidos pegados durante toda la sesion
//SAMPLE  sample;
CLP     Clipboard = { 0, 0, NULL };
int     PosY=31;

void EditSound0(void)
{
  int pos;

  v.type       = 1;
  v.title     = texts[349];
  v.name     = texts[349];
  v.w         = 308;
  v.h         = 151; // 116

  v.paint_handler=EditSound1;
  v.click_handler=EditSound2;
  v.close_handler=EditSound3;

  window_width = (v.w-8)*big2;
  window_height  = 64*big2;
  sel_1         = 0;
  sel_2         = 0;
  //sel_2         = window_width-1;

  for(NumSND=0; NumSND<100; NumSND++)
  {
    DesktopSND[NumSND].name[0]=0;
    DesktopSND[NumSND].SoundFreq=0;
    DesktopSND[NumSND].SoundBits=0;
    DesktopSND[NumSND].SoundSize=0;
    DesktopSND[NumSND].SoundData=NULL;
  }
  NumSND=0;

  pos  = 7;                           _button(501,pos,v.h-34,0);
  pos += text_len(texts[501])+3;      _button(502,pos,v.h-34,0);
  pos += text_len(texts[502])+3;      _button(503,pos,v.h-34,0);
  pos += text_len(texts[503])+3;      _button(504,pos,v.h-34,0);

  pos += text_len(texts[504])+3;
  pos += v.w-1-text_len(texts[506]);
  pos -= text_len(texts[507])+3;
  pos -= text_len(texts[508])+3;
  pos -= text_len(texts[505])+3;
  pos /=2;                            _button(505,pos,v.h-34,0);

  pos  = v.w-1-text_len(texts[506]); _button(506,pos,v.h-34,0);
  pos -= text_len(texts[507])+3;      _button(507,pos,v.h-34,0);
  pos -= text_len(texts[508])+3;      _button(508,pos,v.h-34,0);

  _button(509,7,v.h-14,0);      // Prueba
  _button(512,v.w-8,v.h-14,2); // Copiar en ventana

  pos=193 + text_len(texts[555])+3 ; // 241;

  _button(553,pos,     15,0); pos += text_len(texts[553])+3;
  _button(554,pos,     15,0);

  pos  = 193;

  _button(555,pos,     30,0); pos += text_len(texts[555])+3;
  _button(556,pos,     30,0); pos += text_len(texts[556])+3;
  _button(557,pos,     30,0); pos += text_len(texts[557])+3;

  v_finished=0;
}

void EditSound1(void)
{
  pcminfo *mypcminfo=(pcminfo *)pcminfo_aux;
  short   *buffer;
  byte    *ptr=v.ptr;
  float   step, position=0;
  int     w=v.w/big2, h=v.h/big2;
  int     v_width=window_width/big2, v_height=window_height/big2;
  int     length;
  int     x, y, y0, y1, p0, p1;
  int     Width, Height, First=1, lx, ly;
  short   sample;
  char    cwork[6];

  _show_items();

  // Info del sonido
  wbox       (v.ptr, w, h,  c1, 4, 12, 69, 28);
  wrectangle(v.ptr, w, h,  c0, 3, 11, 71, 30);
  wrectangle(v.ptr, w, h,  c0, 3, 21, 71, 11);
  wwrite(v.ptr, w, h, 39, 13, 1, (byte *)mypcminfo->name, c3);
  itoa(mypcminfo->SoundFreq, cwork, 10);
  wwrite(v.ptr, w, h, 39, 23, 1, (byte *)cwork, c3);
  sprintf(cwork,"%02d bit",mypcminfo->SoundBits);
  wwrite(v.ptr, w, h, 39, 33, 1, (byte *)cwork, c3);

  // Opciones de conversion

/*
  wbox       (v.ptr, w, h, c12, 191, 13, text_len(texts[552])+2, 11);
  wrectangle(v.ptr, w, h,  c0, 190, 12, text_len(texts[552])+4, 13);
  wwrite(v.ptr, w, h, 192, 15, 0, texts[552], c3);
*/

  // Zona de edicion
  wrectangle(v.ptr, w, h, c0, 3, 11+PosY, v_width+2, v_height+2);
  wbox       (v.ptr, w, h, c1, 4, 12+PosY,   v_width,   v_height);

  wrectangle(v.ptr, w, h, c0, 1, 11+PosY+v_height+5, w-2, 19);

  if(sel_1<=sel_2)
  {
//  No pone cajas de 1 de ancho
//  wbox(v.ptr, w, h, c3, (sel_1/big2)+4, 12+PosY, (sel_2/big2)-(sel_1/big2)+1, v_height);

    p0=4*big2; y0=(12+PosY)*big2;
    for(x=sel_1+p0; x<sel_2+p0+1; x++)
      for(y=0; y<window_height; y++)
        ptr[x+(y0+y)*v.w]=c3;
  }
  else
  {
//  No pone cajas de 1 de ancho
//  wbox(v.ptr, w, h, c3, (sel_2/big2)+4, 12+PosY, (sel_1/big2)-(sel_2/big2)+1, v_height);

    p0=4*big2; y0=(12+PosY)*big2;
    for(x=sel_2+p0; x<sel_1+p0+1; x++)
      for(y=0; y<window_height; y++)
        ptr[x+(y0+y)*v.w]=c3;
  }

  Width = window_width;
  Height  = window_height;
  ptr+=(4+((12+PosY)*v.w))*big2+(Height/2)*v.w;
#ifdef MIXER
  buffer=(short *)mypcminfo->SI->abuf;
  length=mypcminfo->SI->alen/2;
#endif
  if (length>1)
  if (length<3*Width)
  {
    step = (float)Width/(float)(length-1);
    for(x=0; x<length; x++)
    {
      sample=buffer[x], y=(sample*Height/65536);

      if(First)
      {
        First = 0;
        lx    = (int)position;
        ly    = y;
      }
      else
      {
        wline((char *)ptr, v.w, Width, Height, lx, ly, (int)position, y, c_g_low);
        lx = (int)position;
        ly = y;
      }
      position += step;
    }
  }
  else
  {
    step=(float)length/(float)Width;
    for(x=0;x<Width;x++)
    {
      p0=(memptrsize)position;
      position+=step;
      p1=(memptrsize)position;

      sample=buffer[p0];
      y0=y1=(sample*Height/65536);

      do {
        sample=buffer[p0], y=(sample*Height/65536);
        if (y<y0) y0=y; else if (y>y1) y1=y;
        p0+=2;
      } while (p0<p1);

      y=y0;
      do {
        ptr[x+y*v.w]=c_g_low;
      } while (y++<y1);
    }
  }
}

void EditSound2(void)
{
  int v_width=window_width/big2, v_height=window_height/big2;
  int need_refresh=0;

  _process_items();

  if(v.active_item != -1)
  {
    ModifySound(v.active_item);
    need_refresh=1;
  }

  if(wmouse_in(4, 12+PosY, v_width, v_height))
  {
    if(mouse_b&1)
    {
      if(!(old_mouse_b&1)) sel_1=mouse_x-v.x-(4*big2), sel_2=mouse_x-v.x-(4*big2);
      else                 sel_2=mouse_x-v.x-(4*big2);
      need_refresh=1;
    }
    if(mouse_b&2)
    {
      sel_1=0, sel_2=window_width-1;
      need_refresh=1;
    }
  }

  if(need_refresh)
  {
    call(v.paint_handler);
    v.blit=1;
  }
}

void EditSound3(void)
{
//	printf("TODO - divpcm.cpp EditSound3\n");
Mix_HaltChannel(-1);
#ifdef NOTYET
  if(judas_channel[0].smp) judas_stopsample(0);
#endif
}

//═════════════════════════════════════════════════════════════════════════════
//  Grabador de sonidos
//═════════════════════════════════════════════════════════════════════════════

void RecSound0(void)
{
  v.type   = 1;
  v.title = texts[560];
  v.name = texts[560];
  v.w     = 200;
  v.h     = 68;

  v.paint_handler=RecSound1;
  v.click_handler=RecSound2;
  v.close_handler=RecSound3;

  strcpy(SoundFile, type[7].path);
  if (SoundFile[strlen(SoundFile)-1]!='/') strcat(SoundFile, "/");
  strcat(SoundFile, "SAMPLE.WAV");

  // Botones Aceptar/Cancelar
  _button(100,      7, v.h-14, 0);
  _button(101, v.w-8, v.h-14, 2);

  // Flags MIC/CD
  _flag(561,                         3, 20, &RecDevice[0]);
  _flag(562, 3+11+text_len(texts[561]), 20, &RecDevice[1]);

  // Boton de seleccion de fichero
  _button(121, v.w-12, 17, 0);

  v_accept=0;
}

void RecSound1(void)
{
	printf("TODO - divpcm.cpp RecSound1\n");

  int w=v.w/big2,h=v.h/big2;

  char drive[_MAX_DRIVE+1];
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];
  char cwork[13];

  _show_items();

  // Fuente de grabacion
  wwrite(v.ptr,w,h,3,11,0,texts[565],c3);

  // Fichero WAV a grabar
  wwrite(v.ptr,w,h,w-86,11,0,texts[566],c3);
  _splitpath(SoundFile,drive,dir,fname,ext);
  strcpy(cwork, fname); strcat(cwork, ext);
  wbox(v.ptr, w, h, c12, w-86, 19, 69, 8);
  wwrite_in_box(v.ptr,w,w-12,h,w-85,20,0,(byte *)cwork,c4);

  // Mensaje de ayuda
  wbox(v.ptr, w, h, c12, 3, 29, w-6, 20);
  wrectangle(v.ptr, w, h,  c0, 3, 29, w-6, 20);
  wwrite_in_box(v.ptr,w,w-4,h,w/2,31,1,texts[563],c3);
  wwrite_in_box(v.ptr,w,w-4,h,w/2,40,1,texts[564],c3);

}

void RecSound2(void)
{
//int  w=v.w/big2;
  int  need_refresh=0;

  _process_items();

  switch(v.active_item)
  {
    case 0: // Aceptar
      end_dialog=1; v_accept=1;
      break;
    case 1: // Cancelar
      end_dialog=1;
      break;
    case 2: // MIC
      RecDevice[1]=!RecDevice[0];
      need_refresh=1;
      break;
    case 3: // CD
      RecDevice[0]=!RecDevice[1];
      need_refresh=1;
      break;
    case 4: // Seleccion de fichero
      v_mode=1;
      v_type=7;
      v_text=(char *)texts[339];
      dialog(browser0);

      strcpy(full,type[v_type].path);
      if (full[strlen(full)-1]!='/') strcat(full,"/");
      strcat(full, input);

      if(v_finished)
      {
        if (v_exists) {
          v_title=(char *)texts[340]; v_text=input;
          dialog(accept0);
        } else v_accept=1;
        if (v_accept) {
          strcpy(SoundFile, full);
          need_refresh=1;
        }
        v_accept=0;
      }
      break;
  }
  if(need_refresh)
  {
    call(v.paint_handler);
    v.blit=1;
  }
}

void RecSound3(void)
{
  if(v_accept)
  {
    RecordSound();
  }
}

void ModifySound(int option)
{
	printf("TODO - divpcm.cpp ModifySound\n");
#ifdef MIXER
	SDL_RWops *rw;
  pcminfo   *mypcminfo=(pcminfo *)pcminfo_aux;
  pcminfo   pcminfo_bak;
  Mix_Chunk *SI=NULL;
  byte      *FileBuffer;
  float     start, final, size_rel, fade;
  int       pos, value, n;
  int       begin, end;
  short     *buffer    = (short *)mypcminfo->SI->abuf;
  short     *short_ptr = Clipboard.SoundData;
  int       length;
  float     step, pos_f;

  if(sel_1<=sel_2) start = sel_1, final = sel_2+1;
  else             start = sel_2, final = sel_1+1;

  size_rel = (float)mypcminfo->SoundSize/(float)window_width;

  begin = (float)start*size_rel;
  end = (float)final*size_rel;
  if(final==window_width) end=mypcminfo->SoundSize;

	Mix_HaltChannel(-1);
	
//  if(judas_channel[0].smp) judas_stopsample(0);

  switch(option)
  {
    case 5: // Copy
      if(Clipboard.SoundData!=NULL)
      {
        free(Clipboard.SoundData);
        Clipboard.SoundData=NULL;
      }
      if(mypcminfo->SoundData==NULL) return;
      Clipboard.SoundFreq=mypcminfo->SoundFreq;
      Clipboard.SoundSize=end-begin;
      if(Clipboard.SoundSize<=0) return;
      Clipboard.SoundData=(short *)malloc(Clipboard.SoundSize*2);
      if(Clipboard.SoundData==NULL)
      {
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }
      memcpy(Clipboard.SoundData, buffer+begin, Clipboard.SoundSize*2);
      return;
    case 6: // Cut
      if(Clipboard.SoundData!=NULL)
      {
        free(Clipboard.SoundData);
        Clipboard.SoundData=NULL;
      }
      if(mypcminfo->SoundData==NULL) return;
      Clipboard.SoundFreq=mypcminfo->SoundFreq;
      Clipboard.SoundSize=end-begin;
      if(Clipboard.SoundSize<=0) return;
      Clipboard.SoundData=(short *)malloc(Clipboard.SoundSize*2);
      if(Clipboard.SoundData==NULL)
      {
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }
      memcpy(Clipboard.SoundData, buffer+begin, Clipboard.SoundSize*2);
      if(mypcminfo->SoundSize-Clipboard.SoundSize<=0)
      {
        if(mypcminfo->SoundData) {
		//	Mix_FreeChunk(mypcminfo->SI);
//          judas_freesample(mypcminfo->sample);
          mypcminfo->SoundData=NULL;
          mypcminfo->SoundSize=0;
        }
      }
      else
      {
        if((pcminfo_bak.SoundData=(short *)malloc((mypcminfo->SoundSize-Clipboard.SoundSize)*2))==NULL)
        {
          free(Clipboard.SoundData);
          Clipboard.SoundData=NULL;
          v_text=(char *)texts[45];
          dialog(err0);
          return;
        }
        pcminfo_bak.SoundBits = mypcminfo->SoundBits;
        pcminfo_bak.SoundFreq = mypcminfo->SoundFreq;
        pcminfo_bak.SoundSize = mypcminfo->SoundSize-Clipboard.SoundSize;

        memcpy(pcminfo_bak.SoundData, buffer, begin*2);
        memcpy(pcminfo_bak.SoundData+begin, buffer+end, (mypcminfo->SoundSize-end)*2);

        if( (FileBuffer=SaveSoundMem(&pcminfo_bak))==NULL )
        {
          free(pcminfo_bak.SoundData);
          free(Clipboard.SoundData);
          Clipboard.SoundData=NULL;
          v_text=(char *)texts[45];
          dialog(err0);
          return;
        }
//		rw = SDL_RWFromMem(pcminfo_bak.SoundBits,pcminfo_bak.SoundSize);
//		rw = SDL_RWFromMem(FileBuffer,FilePos);
//		SI = Mix_LoadWAV_RW(rw,1);
		SI = Mix_QuickLoad_RAW(FileBuffer,FilePos);
//		Mix_PlayChannel(-1,SI,0);
		pcminfo_bak.SI=SI;
//        SI = judas_loadwav_mem(FileBuffer);

        if(SI==NULL)// || judas_error!=JUDAS_OK)
        {
//          if(SI)         free(SI);
          if(FileBuffer) free(FileBuffer);
          free(pcminfo_bak.SoundData);
          free(Clipboard.SoundData);
          Clipboard.SoundData=NULL;
          v_text=(char *)texts[45];
          dialog(err0);
          return;
        }

        if(mypcminfo->SoundData) {
		//	Mix_FreeChunk(SI);
//          judas_freesample(mypcminfo->sample);
          mypcminfo->SoundData=NULL;
          mypcminfo->SoundSize=0;
        }

        mypcminfo->SoundSize = SI->alen/2;
        
        mypcminfo->SoundData = (short *)malloc(SI->alen);

		if(mypcminfo->SoundData!=NULL) 
			memcpy(mypcminfo->SoundData,SI->abuf, SI->alen);

//        mypcminfo->SoundData = SI->abuf;
        mypcminfo->sample    = NULL;////SI->sample;
        mypcminfo->SI		 = SI;
        

//        free(SI);
        free(FileBuffer);
        free(pcminfo_bak.SoundData);
      }
      return;
    case 7: // Paste
      if(Clipboard.SoundData==NULL) return;
      if(Clipboard.SoundSize <= 0) return;
      length = Clipboard.SoundSize;
      if(Clipboard.SoundFreq != mypcminfo->SoundFreq)
      {
        step   = (float)Clipboard.SoundFreq/(float)mypcminfo->SoundFreq;
        pos_f  = (float)length/step;
        length = (int)(pos_f+0.5);
        pos_f  = 0;
        if((short_ptr=(short *)malloc(length*2))==NULL)
        {
          v_text=(char *)texts[45];
          dialog(err0);
          return;
        }
        for(pos=0; pos<length; pos++)
        {
          short_ptr[pos]=Clipboard.SoundData[(int)pos_f];
          pos_f+=step;
        }
      }

      if((pcminfo_bak.SoundData=(short *)malloc((mypcminfo->SoundSize+length)*2))==NULL)
      {
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }
      pcminfo_bak.SoundBits = mypcminfo->SoundBits;
      pcminfo_bak.SoundFreq = mypcminfo->SoundFreq;
      pcminfo_bak.SoundSize = mypcminfo->SoundSize+length;

      memcpy(pcminfo_bak.SoundData, buffer, begin*2);
      memcpy(pcminfo_bak.SoundData+begin, short_ptr, length*2);
      memcpy(pcminfo_bak.SoundData+begin+length, buffer+begin, (mypcminfo->SoundSize-begin)*2);
      if(Clipboard.SoundFreq != mypcminfo->SoundFreq) free(short_ptr);

      if( (FileBuffer=SaveSoundMem(&pcminfo_bak))==NULL )
      {
        free(pcminfo_bak.SoundData);
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }
      free(pcminfo_bak.SoundData);
		SI = Mix_QuickLoad_RAW(FileBuffer, FilePos);
		pcminfo_bak.SI=SI;
		
      if(SI==NULL) // || judas_error!=JUDAS_OK)
      {
//        if(SI)         free(SI);
        if(FileBuffer) free(FileBuffer);
        v_text=(char *)texts[45];
        dialog(err0);
        return;
      }

      if(mypcminfo->SoundData) {
		//  Mix_FreeChunk(SI);
//        judas_freesample(mypcminfo->sample);
        mypcminfo->SoundData=NULL;
        mypcminfo->SoundSize=0;
      }

      mypcminfo->SoundSize = SI->alen/2;

      mypcminfo->SoundData = (short *)malloc(SI->alen);
	  if(mypcminfo->SoundData!=NULL) 
		memcpy(mypcminfo->SoundData,SI->abuf, SI->alen);

//      mypcminfo->SoundData = SI->SoundData;
      mypcminfo->sample    = NULL;////SI->sample;
		mypcminfo->SI=SI;
		
//      free(SI);
      free(FileBuffer);
      return;
    case 8: // Play
#ifdef NOTYET
      if ( judascfg_device == DEV_NOSOUND) {
        if ( SoundError ) {
          v_text=(char *)texts[549]; dialog(err0);
        } else {
          v_text=(char *)texts[548]; dialog(err0);
        } return;
      }
#endif
      if(mypcminfo->SoundData==NULL) return;
#ifdef NOTYET

      if(sel_1==sel_2) {
        sample.start  = (mypcminfo->sample)->start;
        sample.repeat = (mypcminfo->sample)->repeat;
        sample.end    = (mypcminfo->sample)->end;
      } else {
        sample.start  = (mypcminfo->sample)->start+begin*2;
        sample.repeat = (mypcminfo->sample)->start+begin*2;
        sample.end    = (mypcminfo->sample)->start+end*2;
      }
      sample.vuprofile = (mypcminfo->sample)->vuprofile;
      sample.voicemode = (mypcminfo->sample)->voicemode;

      judas_playsample(&sample, 0, mypcminfo->SoundFreq, 64*256, MIDDLE);
#endif
	
	Mix_PlayChannel(-1,mypcminfo->SI,0);
	
      return;
    case 9: // Pega el sonido en el escritorio
      CopyNewSound(mypcminfo, begin, end);
      return;
    case 10: // Convertir a 8 bit
      mypcminfo->SoundBits=8;
      return;
    case 11: // Convertir a 16 bit
      mypcminfo->SoundBits=16;
      return;
    case 12: // Convertir a 11025 Hz
      ChangeSoundFreq(11025);
      return;
    case 13: // Convertir a 22050 Hz
      ChangeSoundFreq(22050);
      return;
    case 14: // Convertir a 44100 Hz
      ChangeSoundFreq(44100);
      return;
  }

  for(pos=begin; pos<end; pos++)
  {
    value=buffer[pos];
    switch(option)
    {
      case 0: // Volume down
        if(value>0)
        {
          n=(float)value*0.1;
          if (n) value-=n; else value--;
        }
        else if(value<0)
        {
          n=(float)value*0.1;
          if (n) value-=n; else value++;
        }
        break;
      case 1: // Volume up
        if(value>0)
        {
          n=(float)value*0.1;
          if (n) value+=n; else value++;
        }
        else if(value<0)
        {
          n=(float)value*0.1;
          if (n) value+=n; else value--;
        }
        break;
      case 2: // Fade in
        fade=(float)(pos-begin)/(float)(end-begin);
        value=(float)value*fade;
        break;
      case 3: // Fade out
        fade=(float)(pos-begin)/(float)(end-begin);
        value=(float)value*(1.0-fade);
        break;
      case 4: // Silence
        value=0;
        break;
    }
    if(value<-32768) value=-32768;
    if(value> 32767) value= 32767;
    buffer[pos]=(short)value;
  }
#endif
}

void ChangeSoundFreq(int freq)
{
	printf("TODO - divpcm.cpp ChangeSoundFreq\n");
#ifdef NOTYET
  pcminfo   *mypcminfo=(pcminfo *)pcminfo_aux;
  pcminfo   pcminfo_bak;
  byte      *FileBuffer;
  SoundInfo *SI=NULL;
  short     *short_ptr;
  float     step, pos_f;
  int       length = mypcminfo->SoundSize;
  int       pos;

  if(mypcminfo->SoundData == NULL || mypcminfo->SoundFreq == freq) return;

  step   = (float)mypcminfo->SoundFreq/(float)freq;
  pos_f  = (float)length/step;
  length = (int)(pos_f+0.5);
  pos_f  = 0;
  if((short_ptr=(short *)malloc(length*2))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }
  for(pos=0; pos<length; pos++)
  {
    short_ptr[pos]=mypcminfo->SoundData[(int)pos_f];
    pos_f+=step;
  }

  pcminfo_bak.SoundFreq = freq;
  pcminfo_bak.SoundSize = length;
  pcminfo_bak.SoundData = short_ptr;

  if( (FileBuffer=SaveSoundMem(&pcminfo_bak))==NULL )
  {
    free(short_ptr);
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  SI = judas_loadwav_mem(FileBuffer);

  if(SI==NULL || judas_error != JUDAS_OK)
  {
    if(SI)         free(SI);
    if(FileBuffer) free(FileBuffer);
    free(short_ptr);
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }

  if(mypcminfo->SoundData) {
    judas_freesample(mypcminfo->sample);
    mypcminfo->SoundData=NULL;
    mypcminfo->SoundSize=0;
  }

  mypcminfo->SoundFreq = freq;
  mypcminfo->SoundSize = SI->SoundSize;
  mypcminfo->SoundData = SI->SoundData;
  mypcminfo->sample    = SI->sample;

  free(SI);
  free(FileBuffer);
  free(short_ptr);
#endif

}


void RecordSound(void)
{
	printf("TODO - divpcm.cpp RecordSound\n");

  HeadDC         MyHeadDC;
  FILE           *f;
  unsigned short ra;
  unsigned char  ca;
  int            length;
debugprintf("SoundFile to record: %s\n",SoundFile);

  if((f=fopen(SoundFile,"wb"))==NULL)
  {
    v_text=(char *)texts[43];
    dialog(err0);
    return;
  }

  fputc('R',f);
  fputc('I',f);
  fputc('F',f);
  fputc('F',f);

  length+=36;
  if(fwrite(&length,1,4,f)!=4)
  {
    fclose(f);
    DaniDel(SoundFile);
    v_text=(char *)texts[47];
    dialog(err0);
    return;
  }
  length-=36;

  fputc('W',f);
  fputc('A',f);
  fputc('V',f);
  fputc('E',f);
  fputc('f',f);
  fputc('m',f);
  fputc('t',f);
  fputc(' ',f);

  MyHeadDC.dwUnknow         = 16;
  MyHeadDC.wFormatTag       = 1;
  MyHeadDC.wChannels        = 1;
  MyHeadDC.wBlockAlign      = 1;
  
  if(true /*judascfg_device == DEV_SB16 */) {
    MyHeadDC.dwSamplePerSec   = 44100;
    MyHeadDC.dwAvgBytesPerSec = 44100;
    MyHeadDC.wBits            = 8;
  } else {
    MyHeadDC.dwSamplePerSec   = 22050;
    MyHeadDC.dwAvgBytesPerSec = 22050;
    MyHeadDC.wBits            = 8;
  }

  if(fwrite(&MyHeadDC,1,sizeof(HeadDC),f)!=sizeof(HeadDC))
  {
    fclose(f);
    DaniDel(SoundFile);
    v_text=(char *)texts[47];
    dialog(err0);
    return;
  }

  fputc('d',f);
  fputc('a',f);
  fputc('t',f);
  fputc('a',f);

  if(fwrite(&length,1,4,f)!=4)
  {
    fclose(f);
    DaniDel(SoundFile);
    v_text=(char *)texts[47];
    dialog(err0);
    return;
  }

  fclose(f);

  DmaBuf=0;
#ifdef NOTYET
  judas_uninit();
  timer_uninit();

  if(!sbinit()) return;
  MIX_Reset();
  set_mixer();
#endif

  if(RecDevice[0])
  {
    MIX_SetInput(MIX_IN_MICRO|MIX_NO_FILT);
    MIX_SetVolume(MIX_MICRO_VOL, 5, 5);
    SetCDVolume(0);
  }
  else
  {
    MIX_SetInput(MIX_IN_CD|MIX_NO_FILT);
  }
//MIX_SetVolume(MIX_INPUT, 15, 15);


#ifdef NOTYET
  if(judascfg_device == DEV_SB16 ) {
//  ra=44100;
//  dspwrite(0x42);
//  dspwrite((unsigned char)((ra>>8)&0xFF));
//  dspwrite((unsigned char)(ra&0xFF));
    ra=44100;
    ca=256UL-(1000000UL/ra);
    sbsettc(ca);
  } else {
    ra=22050;
    ca=256UL-(1000000UL/ra);
    sbsettc(ca);
  }

  spkon();
  sbrec(65000);
  dmastatus();
#endif
  PollRecord();

}

void PollRecord(void)
{
	printf("TODO - divpcm.cpp PollRecord\n");

  FILE *f;
  int  length;

  if((f=fopen(SoundFile,"ab"))==NULL) {
    v_text=(char *)texts[47];  // OJO !!! Cambiar mensaje de error
    dialog(err0);
#ifdef NOTYET
    spkoff();
    InitSound();
    MIX_Reset();
    set_mixer();
#endif
    return;
  }

  kbdFLAGS[_ESC]=0;
#ifdef NOTYET

  while(!kbdFLAGS[_ESC])
  {
    if(dmastatus())
    {
      inp(judascfg_port+0x0E);
      if(DmaBuf) DmaBuf=0; else DmaBuf=1;
      sbrec(65000); //Inicializa DMA de nuevo
      if(DmaBuf) DmaBuf=0; else DmaBuf=1;
      fwrite(aligned[DmaBuf],1,65000,f);
      if(DmaBuf) DmaBuf=0; else DmaBuf=1;
    }
  }
#endif

  fwrite(aligned[DmaBuf],1,65000-dmacount(),f);
  length=ftell(f)-44;
  fclose(f);
  if((f=fopen(SoundFile,"rb+"))==NULL)
  {
    v_text=(char *)texts[47];  // OJO !!! Cambiar mensaje de error
    dialog(err0);
#ifdef NOTYET
    spkoff();
    InitSound();
    MIX_Reset();
    set_mixer();
#endif
    return;
  }
//  printf("1836 length %d fseek %x\n",length,f);
  
  fseek(f, 40, SEEK_SET);
  fwrite(&length,1,4,f);
  fclose(f);
#ifdef NOTYET
  while(!dmastatus());
  inp(judascfg_port+0x0E);
  spkoff();
  InitSound();
  MIX_Reset();
  set_mixer();

#endif
}

int JudasProgressRead(int handle, void *buffer, int size)
{
  int  ctr, steps, rest;
  byte *byte_ptr = (byte *)buffer;

  steps=size/4096;
  rest=size-(steps*4096);
  if(steps<1) steps=1, rest=0;
  for(ctr=0; ctr<steps; ctr++)
  {
    if (read(handle, &byte_ptr[ctr*4096], 4096) != 4096)
    {
      Progress((char *)texts[559], steps, steps);
      return(0);
    }
    Progress((char *)texts[559], ctr, steps);
  }
  if(rest)
  {
    if (read(handle, &byte_ptr[ctr*4096], rest) != rest)
    {
      Progress((char *)texts[559], steps, steps);
      return(0);
    }
    Progress((char *)texts[559], steps, steps);
  }
  return(1);
}

void CopyNewSound(pcminfo *mypcminfo, int begin, int end)
{
  char  aux[5];
  short *SoundData;
  int   SoundBytes=(end-begin)*2;

  if(end == -1) end=mypcminfo->SoundSize;

  if( mypcminfo->SoundData==NULL || mypcminfo->SoundSize==0 ||
      begin > mypcminfo->SoundSize || end > mypcminfo->SoundSize ||
      begin >= end ) return;

  if(NumSND>99)
  {
    v_text=(char *)texts[574];
    dialog(err0);
    return;
  }

  if((SoundData=(short *)malloc(SoundBytes))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return;
  }
  memcpy(SoundData, mypcminfo->SoundData+begin, SoundBytes);

  strcpy(DesktopSND[NumSND].name, (char *)texts[137]);
  strcat(DesktopSND[NumSND].name, itoa((ConSND), aux, 10));
  DesktopSND[NumSND].SoundFreq=mypcminfo->SoundFreq;
  DesktopSND[NumSND].SoundBits=mypcminfo->SoundBits;
  DesktopSND[NumSND].SoundSize=end-begin;
  DesktopSND[NumSND].SoundData=SoundData;
  ConSND++; if(ConSND>99) ConSND=0;
  NumSND++;
}

void PasteNewSounds(void)
{
	printf("TODO - divpcm.cpp PasteNewSounds\n");

#ifdef NOTYET
  pcminfo   *mypcminfo;
  byte      *FileBuffer;
  SoundInfo *SI=NULL;
  int       ctr;

  for(ctr=0; ctr<NumSND; ctr++)
  {
    if((pcminfo_aux=(char *)malloc(sizeof(pcminfo)))==NULL)
    {
      v_text=(char *)texts[45];
      dialog(err0);
      continue;
    }
    mypcminfo=(pcminfo *)pcminfo_aux;

    mypcminfo->SoundFreq = DesktopSND[ctr].SoundFreq;
    mypcminfo->SoundSize = DesktopSND[ctr].SoundSize;
    mypcminfo->SoundData = DesktopSND[ctr].SoundData;

    if( (FileBuffer=SaveSoundMem(mypcminfo))==NULL )
    {
      free(DesktopSND[ctr].SoundData);
      free(pcminfo_aux);
      v_text=(char *)texts[46];
      dialog(err0);
      continue;
    }

    SI = judas_loadwav_mem(FileBuffer);

    if(SI==NULL || judas_error != JUDAS_OK)
    {
      if(SI)         free(SI);
      if(FileBuffer) free(FileBuffer);
      free(DesktopSND[ctr].SoundData);
      free(pcminfo_aux);
      v_text=(char *)texts[46];
      dialog(err0);
      continue;
    }

    memcpy(mypcminfo->name,DesktopSND[ctr].name,14);
    mypcminfo->pathname[0] = 0;
    mypcminfo->SoundFreq = SI->SoundFreq;
    mypcminfo->SoundBits = DesktopSND[ctr].SoundBits;
    mypcminfo->SoundSize = SI->SoundSize;
    mypcminfo->SoundData = SI->SoundData;
    mypcminfo->sample    = SI->sample;

    free(SI);
    free(FileBuffer);
    free(DesktopSND[ctr].SoundData);

    new_window((int)PCM0);
  }
#endif
  
}

byte *SaveSoundMem(pcminfo *mypcminfo)
{
  HeadDC MyHeadDC;
  byte   *byte_ptr=(byte *)mypcminfo->SoundData;
  byte   *FileBuffer;
  int    length;

  FilePos=0;
  length=mypcminfo->SoundSize*2;

  if((FileBuffer=(byte *)malloc(length+44))==NULL)
  {
    v_text=(char *)texts[45];
    dialog(err0);
    return(NULL);
  }

  FileBuffer[FilePos] = 'R'; FilePos++;
  FileBuffer[FilePos] = 'I'; FilePos++;
  FileBuffer[FilePos] = 'F'; FilePos++;
  FileBuffer[FilePos] = 'F'; FilePos++;

  length+=36;
  memcpy(FileBuffer+FilePos, &length, 4); FilePos+=4;
  length-=36;

  FileBuffer[FilePos] = 'W'; FilePos++;
  FileBuffer[FilePos] = 'A'; FilePos++;
  FileBuffer[FilePos] = 'V'; FilePos++;
  FileBuffer[FilePos] = 'E'; FilePos++;
  FileBuffer[FilePos] = 'f'; FilePos++;
  FileBuffer[FilePos] = 'm'; FilePos++;
  FileBuffer[FilePos] = 't'; FilePos++;
  FileBuffer[FilePos] = ' '; FilePos++;

  MyHeadDC.dwUnknow         = 16;
  MyHeadDC.wFormatTag       = 1;
  MyHeadDC.wChannels        = 2;
  MyHeadDC.dwSamplePerSec   = mypcminfo->SoundFreq;
  MyHeadDC.dwAvgBytesPerSec = mypcminfo->SoundFreq*(mypcminfo->SoundBits/8);
  MyHeadDC.wBlockAlign      = 1;
  MyHeadDC.wBits            = 16;

  memcpy(FileBuffer+FilePos, &MyHeadDC, sizeof(HeadDC)); FilePos+=sizeof(HeadDC);

  FileBuffer[FilePos] = 'd'; FilePos++;
  FileBuffer[FilePos] = 'a'; FilePos++;
  FileBuffer[FilePos] = 't'; FilePos++;
  FileBuffer[FilePos] = 'a'; FilePos++;

  memcpy(FileBuffer+FilePos, &length, 4); FilePos+=4;
  memcpy(FileBuffer+FilePos, byte_ptr, length); FilePos+=length;

  return(FileBuffer);
}

int IsWAV(char *FileName)
{
  FILE *f;
  int  ok=1;

  if((f=fopen(FileName,"rb"))==NULL) return(0);

  if(fgetc(f)!='R') ok=0;
  if(fgetc(f)!='I') ok=0;
  if(fgetc(f)!='F') ok=0;
  if(fgetc(f)!='F') ok=0;
  fseek(f, 4, SEEK_CUR);
  if(fgetc(f)!='W') ok=0;
  if(fgetc(f)!='A') ok=0;
  if(fgetc(f)!='V') ok=0;
  if(fgetc(f)!='E') ok=0;

  fclose(f);

  return(ok);
}

int NewSample(pcminfo *mypcminfo)
{
	printf("TODO - divpcm.cpp NewSample\n");
#ifdef NOTYET
  mypcminfo->sample = judas_allocsample(mypcminfo->SoundSize*2);
  if(judas_error != JUDAS_OK) return(0);
  mypcminfo->sample->repeat    = mypcminfo->sample->start;
  mypcminfo->sample->end       = mypcminfo->sample->start + mypcminfo->SoundSize*2;
  mypcminfo->sample->voicemode = VM_ON | VM_16BIT;
  judas_ipcorrect(mypcminfo->sample);
  mypcminfo->SoundData = (short *)(mypcminfo->sample->start);
#endif
  return(1);
 
}

void wline(char *ptr,int realwidth,int w,int h,int x0, int y0, int x1, int y1, char color) {
  int dx,dy,a,b,d,x,y;

  if (x0>x1) {x=x1; dx=x0-x1;} else {x=x0; dx=x1-x0;}
  if (y0>y1) {y=y1; dy=y0-y1;} else {y=y0; dy=y1-y0;}

  if (!dx && !dy) wave_line_pixel(ptr,w,realwidth,h,x0,y0,color); else {
    wave_line_pixel(ptr,w,realwidth,h,x0,y0,color);
    if (dy<=dx) {
      if (x0>x1) { wave_line_pixel(ptr,w,realwidth,h,x1,y1,color); x0--; swap(x0,x1); swap(y0,y1); }
      d=2*dy-dx; a=2*dy; b=2*(dy-dx); x=x0; y=y0;
      if (y0<=y1) while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y++; }
        wave_line_pixel(ptr,w,realwidth,h,x,y,color);
      } else while (x<x1) {
        if (d<=0) { d+=a; x++; } else { d+=b; x++; y--; }
        wave_line_pixel(ptr,w,realwidth,h,x,y,color);
      }
    } else  {
      if (y0>y1) { wave_line_pixel(ptr,w,realwidth,h,x1,y1,color); y0--; swap(x0,x1); swap(y0,y1); }
      d=2*dx-dy; a=2*dx; b=2*(dx-dy); x=x0; y=y0;
      if (x0<=x1) while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x++; }
        wave_line_pixel(ptr,w,realwidth,h,x,y,color);
      } else while (y<y1) {
        if (d<=0) { d+=a; y++; } else { d+=b; y++; x--; }
        wave_line_pixel(ptr,w,realwidth,h,x,y,color);
      }
    }
  }
}

void wave_line_pixel(char * ptr, int w, int realwidth, int h, int x, int y, char color) {
  if (x>=0 && y>=0 && x<w && y<h) {
    ptr[x+y*realwidth]=color;
  }
}

