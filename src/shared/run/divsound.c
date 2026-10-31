#include "inter.h"
#include "divsound.h"

tSound  sounds[128];
tSong songs[128];
tChannels channels[64];

int SongType=0;
int MusicChannels=0;
int NextChannel=0;
int *NewSound;
int ChannelCon=0;

int SongInst[128];

int Freq_original[CHANNELS];

int SoundActive=1;
void print_init_flags(int flags)
{
#ifndef GP2X
#ifndef PS2
#ifndef PSP
#ifdef MIXER

#define PFLAG(a) if(flags&MIX_INIT_##a) printf(#a " ")
        PFLAG(FLAC);
        PFLAG(MOD);
        PFLAG(MP3);
        PFLAG(OGG);
        
        if(!flags)
                printf("None");
        
        printf("\n");
#endif
#endif
#endif
#endif
}

static int initted=0;
void InitSound(void)
{
#ifdef MIXER
  if(initted)
    return;

  int audio_rate = 44800;//44100;
  Uint16 audio_format = AUDIO_S16SYS;
  int audio_channels = 2;
  int audio_buffers = 512;
#ifndef GP2X
#ifndef PS2
#ifndef PSP
  int flags = MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG ;
  
  initted=Mix_Init(flags);


	
  if((initted&flags) != flags) {
	  printf("Mix_Init error: %s\n",Mix_GetError());
   }
#endif
#endif
#endif

  SDL_Init( SDL_INIT_AUDIO );

fprintf(stdout, "Opening Audio Device \n");
  if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
  	fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
//	exit(1);
}


  print_init_flags(initted);
#ifndef __EMSCRIPTEN__
SDL_version compile_version;
const SDL_version *link_version=Mix_Linked_Version();
SDL_MIXER_VERSION(&compile_version);
fprintf(stdout,"compiled with SDL_mixer version: %d.%d.%d\n", 
        compile_version.major,
        compile_version.minor,
        compile_version.patch);
fprintf(stdout,"running with SDL_mixer version: %d.%d.%d\n", 
        link_version->major,
        link_version->minor,
        link_version->patch);
#endif

  initted = 1;
#endif

#ifdef DOS
  FILE *File_Cfg;
  int ctr;
  int master=10,sound_fx=10,cd_audio=10;
  char Device[8]={DEV_NOSOUND,DEV_SB,DEV_SB,DEV_SBPRO,DEV_SB16,DEV_SB16,DEV_GUS,DEV_GUS};
  unsigned short Ports[6]={0x210,0x220,0x230,0x240,0x250,0x260};
  unsigned short IRQS[10]  ={2,3,5,7,10,11,12,13,14,15};
  unsigned short DMAS[10]  ={0,1,2,3,4,5,6,7,8,9};
  unsigned short DMAS2[10] ={0,1,2,3,4,5,6,7,8,9};

//int mixer         = QUALITYMIXER;
  int mixer         = FASTMIXER;
  int mixrate       = 44100;
  int mixmode       = SIXTEENBIT | STEREO;
//int mixmode       = EIGHTBIT | MONO;
  int interpolation = 1;
  int cfg_dev, cfg_port, cfg_irq, cfg_dma1, cfg_dma2;

  atexit(judas_uninit);
  atexit(timer_uninit);

  NewSound=mem+end_struct+32;
  if(SoundActive) {

    if(!(mem[0]&1))
    {
      File_Cfg=fopen("sound.cfg","rb");
      if(File_Cfg!=NULL) {
        fread(&cfg_dev,  1, 4, File_Cfg);
        fread(&cfg_port, 1, 4, File_Cfg);
        fread(&cfg_irq,  1, 4, File_Cfg);
        fread(&cfg_dma1, 1, 4, File_Cfg);
        fread(&cfg_dma2, 1, 4, File_Cfg);
        fread(&master,   1, 4, File_Cfg);
        fread(&sound_fx, 1, 4, File_Cfg);
        fread(&cd_audio, 1, 4, File_Cfg);

        // Nuevos valores del sound.cfg

        if (fread(&mixer, 1, 4, File_Cfg)==4) {
          fread(&mixrate, 1, 4, File_Cfg);
          fread(&mixmode, 1, 4, File_Cfg);
          if (mixer<1 || mixer>2) mixer=FASTMIXER;
          if (mixrate<11025) mixrate=11025;
          if (mixrate>44100) mixrate=44100;
          if (mixmode==16) mixmode=SIXTEENBIT | STEREO;
          else mixmode=EIGHTBIT | STEREO;
        }

        fclose(File_Cfg);
      }
    }
    File_Cfg=fopen("system\\exec.bin","rb");
    if(File_Cfg!=NULL) {
      fread(&sound_fx,1,1,File_Cfg);
      fread(&cd_audio,1,1,File_Cfg);
      fread(&master,1,1,File_Cfg);
      fclose(File_Cfg);
    }
    judas_config();

    if(!(mem[0]&1)) {
      File_Cfg=fopen("sound.cfg","rb");
      if(File_Cfg!=NULL) {
        fclose(File_Cfg);
        judascfg_device = Device[cfg_dev];
        judascfg_port   = Ports[cfg_port];
        judascfg_irq    = IRQS[cfg_irq];
        judascfg_dma1   = DMAS[cfg_dma1];
        judascfg_dma2   = DMAS2[cfg_dma2];
      }
    }

    setup->master   = master;
    setup->sound_fx = sound_fx;
    setup->cd_audio = cd_audio;
    setup->mixer    = mixer;
    setup->mixrate  = mixrate;
    if (mixmode==(SIXTEENBIT | STEREO)) setup->mixmode  = 16;
    else                                setup->mixmode  =  8;
    setup->card=setup->port=setup->irq=setup->dma=setup->dma2=0;

    switch(judascfg_device) {
      case DEV_NOSOUND: setup->card=0; break;
      case DEV_SB:      setup->card=1; break;
      case DEV_SBPRO:   setup->card=3; break;
      case DEV_SB16:    setup->card=4; break;
      case DEV_GUS:     setup->card=6; break;
      default: setup->card=0; break;
    }
    switch(judascfg_port) {
      case 0x210: setup->port=0; break;
      case 0x220: setup->port=1; break;
      case 0x230: setup->port=2; break;
      case 0x240: setup->port=3; break;
      case 0x250: setup->port=4; break;
      case 0x260: setup->port=5; break;
      default: setup->port=1; break;
    }
    switch(judascfg_irq) {
      case  2: setup->irq=0; break;
      case  3: setup->irq=1; break;
      case  5: setup->irq=2; break;
      case  7: setup->irq=3; break;
      case 10: setup->irq=4; break;
      case 11: setup->irq=5; break;
      case 12: setup->irq=6; break;
      case 13: setup->irq=7; break;
      case 14: setup->irq=8; break;
      case 15: setup->irq=9; break;
      default: setup->irq=7; break;
    }
    setup->dma  = judascfg_dma1;
    setup->dma2 = judascfg_dma2;

    if (setup->dma<0) setup->dma=0;
    if (setup->dma2<0) setup->dma2=0;
  }
  else {
    judascfg_device = DEV_NOSOUND;
  }

  timer_init(1193180 / 100, judas_update);
  if(!judas_init(mixrate, mixer, mixmode, interpolation)) {
    judascfg_device = DEV_NOSOUND;
    judas_init(mixrate, mixer, mixmode, interpolation);
  } else {
    judas_setmusicmastervolume(CHANNELS, 50);
    set_mixer();
  }

  for(ctr=0; ctr<128; ctr++) {
    UnloadSound(ctr);
    UnloadSong(ctr);
  }
  for(ctr=16; ctr<32; ctr++) judas_channel[ctr].smp=NULL;
  for(ctr= 0; ctr<32; ctr++) channel(ctr)=0;


#endif

  MusicChannels=0;



}

void ResetSound(void)
{
#ifdef DOS
  int ctr;
  char Device[8]={DEV_NOSOUND,DEV_SB,DEV_SB,DEV_SBPRO,DEV_SB16,DEV_SB16,DEV_GUS,DEV_GUS};
  unsigned short Ports[6]={0x210,0x220,0x230,0x240,0x250,0x260};
  unsigned short IRQS[10]  ={2,3,5,7,10,11,12,13,14,15};
  unsigned short DMAS[10]  ={0,1,2,3,4,5,6,7,8,9};
  unsigned short DMAS2[10] ={0,1,2,3,4,5,6,7,8,9};

//int mixer         = QUALITYMIXER;
  int mixer         = FASTMIXER;
  int mixrate       = 44100;
  int mixmode       = SIXTEENBIT | STEREO;
//int mixmode       = EIGHTBIT | MONO;
  int interpolation = 1;

  StopSong();
  for(ctr=0; ctr<32; ctr++) StopSound(ctr);

  judascfg_device = Device[setup->card];
  judascfg_port   = Ports[setup->port];
  judascfg_irq    = IRQS[setup->irq];
  judascfg_dma1   = DMAS[setup->dma];
  judascfg_dma2   = DMAS2[setup->dma2];

  if (setup->mixer<1 || setup->mixer>2) setup->mixer=FASTMIXER;
  if (setup->mixrate<11025) setup->mixrate=11025;
  if (setup->mixrate>44100) setup->mixrate=44100;
  if (setup->mixmode==16) setup->mixmode=16;
  else                    setup->mixmode=8;

  mixer  =setup->mixer;
  mixrate=setup->mixrate;
  if (setup->mixmode==16) mixmode=SIXTEENBIT | STEREO;
  else mixmode=EIGHTBIT | STEREO;

  if(!judas_init(mixrate, mixer, mixmode, interpolation))
  {
    judascfg_device = DEV_NOSOUND;
    judas_init(mixrate, mixer, mixmode, interpolation);
  } else {
    judas_setmusicmastervolume(CHANNELS, 50);
    set_mixer();
  }

  for(ctr=16; ctr<32; ctr++) judas_channel[ctr].smp=NULL;
  for(ctr= 0; ctr<32; ctr++) channel(ctr)=0;

  MusicChannels=0;

#endif
}

int LoadSound(char *ptr, long Len, int Loop)
{
  HeadDC MyHeadDC;

#ifdef MIXER
	int channel;
	Mix_Chunk *sound=NULL;
	SDL_RWops *rw;
	byte *dst; 
  byte *dst2;
	FILE *mem, *fdst;
  char *riff="RIFF";
  char *wavefmt = "WAVEfmt ";
  char *data = "data";
	int ctr=0;
	byte res=0;
	int32_t iLen = (int32_t)Len+36;

	while(ctr<128 && sounds[ctr].smp!=NULL) 
		ctr++;
	
	if(ctr==128) return(-1);
	
  // check if wav or OGG
  if((ptr[0]=='R' && ptr[1]=='I') || (ptr[0]=='O' && ptr[1]=='g')) {
  	rw = SDL_RWFromMem(ptr, Len);
  	sound = Mix_LoadWAV_RW(rw, 1);
  }

  // If sound cant be loaded, must be a pcm 
  if(!sound) {
  	dst = (byte *)malloc((int)iLen+50);

    if(dst==NULL)
      return(-1);
    
    memset(dst,0,(int)Len+40);
  	
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
    sound = Mix_LoadWAV_RW(rw, 1);
    
    if(!sound) {
		  printf("Mix_LoadWAV: %s\n", Mix_GetError());
  		return (-1);
	 }
  }
	// all ok. save our data to free() later
	sounds[ctr].smp = 1;
	sounds[ctr].sound = sound;
	sounds[ctr].loop = Loop;

	return ctr;
#endif

#ifdef DOS

  SoundInfo *SI=NULL;
  int ctr=0;

  while(ctr<128 && sounds[ctr].smp!=NULL) ctr++;
  if(ctr==128) return(-1);

  SI = judas_loadwav_mem(ptr);
  if(judas_error != JUDAS_OK && judas_error == JUDAS_WRONG_FORMAT)
  {
    if(SI != NULL) free(SI);
    SI = judas_loadrawsample_mem(ptr, Len, 0, 0, 0);
  }
  if(judas_error != JUDAS_OK)
  {
    if(SI != NULL) free(SI);
    return(-1);
  }

  if(Loop) (SI->sample)->voicemode |= VM_LOOP;
  sounds[ctr].smp  = SI->sample;
  sounds[ctr].freq = SI->SoundFreq;

  free(SI);

  return(ctr);
#endif
  return -1;
}

int UnloadSound(int NumSound)
{
#ifdef MIXER
	if(sounds[NumSound].sound) {
		Mix_FreeChunk(sounds[NumSound].sound);
		sounds[NumSound].smp=NULL;
		sounds[NumSound].sound=NULL;
	}
#endif
#ifdef DOS
  if(sounds[NumSound].smp)
  {
    judas_freesample(sounds[NumSound].smp);
    sounds[NumSound].smp=NULL;
  }
#endif
  return(1);

}
#ifdef MIXER

void doneEffect(int chan, void *data) {
//  if(Mix_Playing(chan))
//    StopSound(chan);
	return;
}
 	
// make a passthru processor function that alters the stream size
void freqEffect(int chan, void *stream, int len, void *udata)
{
  float x=0;
	tSound *s = &sounds[channels[chan].num];
	int pos = channels[chan].pos;

  if(!Mix_Playing(chan))
    return;

	if(channels[chan].freq>1024) channels[chan].freq=1024;
	if(channels[chan].freq<0) channels[chan].freq=0;

	float ratio = channels[chan].freq/256.0f;//(22050 +10000) / 22050.0f;

	short* samples = (short*)stream;
  memset(stream,0,len);
	uint16_t *input = (uint16_t *)(s->sound->abuf)+pos;
	int i = 0;
	float j = 0;
	for(x = 0; i < len/2 && pos+x<s->sound->alen/2; x += ratio) {
		if(pos+(int)x>=s->sound->alen/2) {
			if(s->loop==1) {
				x=0;
				j=0;
				pos=0;
				input = (uint16_t*)(s->sound->abuf);
			} else {
        fprintf(stdout, "Sound ended\n");
          while(i<len/2-1) {
            samples[i++]=0;
            x+=ratio;            
            j+=ratio;
          }
		  		i=len/2;
          pos = len/2+s->sound->alen/2;
          break;
  //        StopSound(chan);
			}
		}
    samples[i++] = input[(int)x];
		j+=ratio;
	}
  pos+=(int)j;

  if(pos>=s->sound->alen/2) {
  	if(s->loop==1)
  		pos=0;
  	else {

      for(; i < len/2; i++) {
        samples[i] = 0;
      }
      StopSound(chan);
      return;
    }
  } 
  channels[chan].pos=pos;

// fill rest with empty :(
// this needs to be moved into the loop ideally
	for(; i < len/2; i++) {
		samples[i] = 0;
	}
}
#endif
void channelDone(int channel) {
  // Done!
}

int DivPlaySound(int NumSound, int Volume, int Frec) // Vol y Frec (0..256)
{
  int ctr=0;
  int loop=-1;
//Mix_HaltChannel(-1);
#ifdef MIXER

#if defined( __EMSCRIPTEN__ ) && !defined (SDL2)
  loop = sounds[NumSound].loop?-1:0;
#endif
  // always play as loop, let the freqEffect manage stop_sound when loop is zero
  // this permits slow playing sound to run for the correct length.
	ctr = Mix_PlayChannel(-1, sounds[NumSound].sound, loop);


  // if unable to play, return 
  if(ctr==-1)
    return(0);
  
  // Make sure all old callbacks are cleared
#if ! defined( __EMSCRIPTEN__ ) || defined (SDL2)
  Mix_UnregisterAllEffects(ctr);
#endif

  channels[ctr].freq = Frec;
  channels[ctr].vol = Volume;
  channels[ctr].pos = 0;

#if ! defined( __EMSCRIPTEN__ ) || defined (SDL2)
  // Setup our callback to change frequency
		Mix_RegisterEffect(ctr, freqEffect, doneEffect,NULL);
    Mix_ChannelFinished(channelDone);
//  Mix_SetPostMix(freq)
#endif
	
	Mix_Volume(ctr,Volume/2);
	channels[ctr].num=NumSound;
	channels[ctr].ctr = ctr;

	Freq_original[ctr]=channels[ctr].freq;

#endif
  return(ctr);
}

int StopSound(int NumChannel)
{
  if(NumChannel == -2)
    NumChannel = -1;
  
#ifdef MIXER
int x=99;
#if ! defined( __EMSCRIPTEN__ ) || defined (SDL2)
  //   if(!Mix_UnregisterAllEffects(NumChannel)) {
  //     printf("Mix_UnregisterAllEffects: %s\n", Mix_GetError());
  //   }
#endif
 if(Mix_Playing(NumChannel)) {
    Mix_HaltChannel(NumChannel);
  }
  while(x-->0 && Mix_Playing(NumChannel))
    Mix_HaltChannel(NumChannel);
#ifdef DOS
  judas_stopsample(NumChannel);
#endif

#endif // MIXER
  return(1);
}

int ChangeSound(int NumChannel,int Volume,int Frec)
{
#ifdef MIXER
	channels[NumChannel].freq=Frec;
	Mix_Volume(NumChannel,Volume/2);
#endif

#ifdef DOS
  CHANNEL *chptr;

  if(NumChannel >= CHANNELS || NumChannel < MusicChannels) return(-1);

  chptr = &judas_channel[NumChannel];

  chptr->vol       = 32*Volume;
  chptr->freq      = (Freq_original[NumChannel]*Frec)/256;

#endif

  return(1);
}

int ChangeChannel(int NumChannel,int Volume,int Panning)
{
  // Set the volume
#ifdef MIXER
  Mix_Volume(NumChannel,Volume/2);

  // set the balance
  Mix_SetPanning(NumChannel, 255-Panning, Panning);
#endif
  
#ifdef DOS
  CHANNEL *chptr;

  if(NumChannel >= CHANNELS || NumChannel < MusicChannels) return(-1);

  chptr = &judas_channel[NumChannel];

  chptr->mastervol = Volume/2;
  chptr->panning   = Panning;
#endif
  return(1);
}

int IsPlayingSound(int NumChannel)
{
#ifdef MIXER
	return Mix_Playing(NumChannel);
#endif
#ifdef DOS
  SAMPLE *smp = judas_channel[NumChannel].smp;
  char *pos   = judas_channel[NumChannel].pos;
  
  if( (NumChannel >= CHANNELS) ||
      ((judas_channel[NumChannel].voicemode & VM_ON) == VM_OFF) ||
      (!judas_channel[NumChannel].smp) ) return(0);

  if (pos >= smp->end || pos < smp->start) return(0);

#endif

  return(1);
}

#ifdef DIV2
int LoadSong(char *ptr, int Len, int Loop)
{
#if ! defined( __EMSCRIPTEN__ ) || defined (SDL2)
#ifdef MIXER
  int ctr=0;

  // Check we can load the file
  SDL_RWops *rw = SDL_RWFromMem(ptr, Len); 

  Mix_Music* music = Mix_LoadMUS_RW(rw
#ifdef SDL2
    ,0
#endif
    ); 
  if(!music)
  	return(-1);

  Mix_FreeMusic(music);
  SDL_FreeRW(rw);

  while(ctr<128 && songs[ctr].ptr!=NULL) ctr++;
  if(ctr==128) return(-1);

#ifdef DOS

  judas_loadxm_mem(ptr);
  if(judas_error == JUDAS_OK)
  {
    songs[ctr].SongType=XM;
    judas_freexm();
  }
  else if(judas_error == JUDAS_WRONG_FORMAT)
  {
    judas_loads3m_mem(ptr);
    if(judas_error == JUDAS_OK)
    {
      songs[ctr].SongType=S3M;
      judas_frees3m();
    }
    else if(judas_error == JUDAS_WRONG_FORMAT)
    {
      judas_loadmod_mem(ptr);
      if(judas_error == JUDAS_OK)
      {
        songs[ctr].SongType=MOD;
        judas_freemod();
      }
    }
  }

  if(judas_error != JUDAS_OK) return(-1);
#endif

  if((songs[ctr].ptr=(char *)malloc(Len))==NULL) 
    return(-1);

  memcpy(songs[ctr].ptr, ptr, Len);
  songs[ctr].loop=Loop;

  rw = SDL_RWFromMem(songs[ctr].ptr, Len); 
  music = Mix_LoadMUS_RW(rw
#ifdef SDL2
    ,0
#endif
    ); 

  songs[ctr].music = music;
  songs[ctr].rw = rw;

  return(ctr);
#endif
#endif
return -1;
}

int PlaySong(int NumSong)
{
  if(NumSong>127 || !songs[NumSong].ptr) return(-1);

  StopSong();

#ifdef MIXER
	if(songs[NumSong].music) {
		Mix_PlayMusic(songs[NumSong].music, songs[NumSong].loop?-1:0);
	}
#endif

#ifdef DOS
  switch(songs[NumSong].SongType)
  {
    case XM:
      judas_loadxm_mem(songs[NumSong].ptr);
      if(songs[NumSong].loop) judas_playxm(1000000000);
      else                      judas_playxm(1);
      MusicChannels = judas_getxmchannels();
      break;
    case S3M:
      judas_loads3m_mem(songs[NumSong].ptr);
      if(songs[NumSong].loop) judas_plays3m(1000000000);
      else                      judas_plays3m(1);
      MusicChannels = judas_gets3mchannels();
      break;
    case MOD:
      judas_loadmod_mem(songs[NumSong].ptr);
      if(songs[NumSong].loop) judas_playmod(1000000000);
      else                      judas_playmod(1);
      MusicChannels = judas_getmodchannels();
      break;
  }

  SongType = songs[NumSong].SongType;

#endif
  return(1);
}

void StopSong(void)
{
#ifdef DOS
  if(!judas_songisplaying()) return;

  switch(SongType)
  {
    case XM:  judas_freexm();  break;
    case S3M: judas_frees3m(); break;
    case MOD: judas_freemod(); break;
  }

#endif
#ifdef MIXER
Mix_HaltMusic();
#endif

  MusicChannels=0;
}

void UnloadSong(int NumSong)
{
  if(NumSong>127 || !songs[NumSong].ptr) return;
#ifdef MIXER
Mix_FreeMusic(songs[NumSong].music);
  free(songs[NumSong].ptr);
  SDL_FreeRW(songs[NumSong].rw);

  songs[NumSong].music=NULL;
  songs[NumSong].rw=NULL;
#endif  
  songs[NumSong].ptr=NULL;
  songs[NumSong].loop=0;

}

void SetSongPos(int SongPat)
{
#ifdef DOS
  if(!judas_songisplaying()) return;

  switch(SongType)
  {
    case XM:  judas_set_xm_pos(SongPat);  break;
    case S3M: judas_set_s3m_pos(SongPat); break;
    case MOD: judas_set_mod_pos(SongPat); break;
  }
#endif
}

int GetSongPos(void)
{
#ifdef DOS
  int pos;

  if(!judas_songisplaying()) return(-1);

  switch(SongType)
  {
    case XM:  pos=judas_getxmpos();  break;
    case S3M: pos=judas_gets3mpos(); break;
    case MOD: pos=judas_getmodpos(); break;
  }

  return(pos);
#endif
}

int GetSongLine(void)
{
#ifdef DOS
  int pos;

  if(!judas_songisplaying()) return(-1);

  switch(SongType)
  {
    case XM:  pos=judas_getxmline();  break;
    case S3M: pos=judas_gets3mline(); break;
    case MOD: pos=judas_getmodline(); break;
  }

  return(pos);
#endif
}

int IsPlayingSong(void)
{
#ifdef DOS
  if(judas_songisplaying()) return(1);

  return(0);
#endif

#ifdef MIXER
	return (Mix_PlayingMusic());
#endif

}

#endif

void EndSound(void)
{
#ifdef DOS
  judas_uninit();
  timer_uninit();
#endif
}

