#include "inter.h"
#include "osdep.h"
#include "divsound.h"

#ifdef __APPLE__
#include "osx/fmemopen.h"
#endif

tSonido  sonido[128];
tCancion cancion[128];
tChannels channels[64];

int Freq_original[CHANNELS];

int SongType=0;
int MusicChannels=0;
int NextChannel=0;
int *NewSound;
int ChannelCon=0;

#ifdef MIXER
 	
// make a passthru processor function that alters the stream size
void freqEffect(int chan, void *stream, int len, void *udata)
{
	float x=0;
	tSonido *s = &sonido[channels[chan].num];
	int pos = channels[chan].pos;
	
	if(channels[chan].freq>1024) channels[chan].freq=1024;
	if(channels[chan].freq<0) channels[chan].freq=0;

	float ratio = channels[chan].freq/256.0f;//(22050 +10000) / 22050.0f;

//printf("Ratio: %f Freq: %d\n",ratio,channels[chan].freq);

	short* samples = (short*)stream;
	uint16_t *input = (uint16_t *)(s->sound->abuf)+pos;
	int i = 0;
	float j = 0;
	for(x = 0; i < len/2-1 && pos+x<s->sound->alen/2; x += ratio) {
		//float p = x - int(x);
		samples[i++] = input[(int)x];// + p * input[int(x) + 1];
		if(pos+x>s->sound->alen/2) {
			if(s->loop==1) {
				x=0;
				j=0;
				pos=0;
				input = (uint16_t*)(s->sound->abuf)+pos;
			} else {
//				pos=0;
		  		i=len/2;
			}
		}
		j+=ratio;
	}
pos+=(int)j;

//printf("pos: %d %f\n",pos,j);
if(pos>=s->sound->alen/2) {
	if(s->loop==1)
		pos=0;
	else {
//		Mix_HaltChannel(chan);
		StopSound(chan);
	}
} 
channels[chan].pos=pos;

// fill rest with empty :(

	for(; i < len/2; i++) {
		samples[i] = 0;
	}
}
#endif


void EndSound(void) {
	
	
}

#ifdef MIXER
void print_init_flags(int flags)
{
#define PFLAG(a) if(flags&MIX_INIT_##a) printf(#a " ")
        PFLAG(FLAC);
        PFLAG(MOD);
        PFLAG(MP3);
        PFLAG(OGG);
        if(!flags)
                printf("None");
        printf("\n");
}
#endif

static int initted=0;
void InitSound(void)
{
#ifdef MIXER
if(initted)
return;

int audio_rate = 44100;
Uint16 audio_format = AUDIO_S16SYS;
int audio_channels = 2;
int audio_buffers = 1024;
  int flags = MIX_INIT_MOD|MIX_INIT_OGG|MIX_INIT_FLAC;
  
  initted=Mix_Init(flags);

SDL_Init( SDL_INIT_AUDIO );

//print_init_flags(initted);
	
  if((initted&flags) != flags) {
	  printf("Mix_Init: Failed to init required ogg and mod support!\n");
	  printf("Mix_Init: %d %s\n", initted, Mix_GetError());
   }
 


if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
	fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
//	exit(1);
}

initted = 1;
#endif

  MusicChannels=0;



}


int LoadSound(char *file, int Loop) {
	
	
#ifdef MIXER
	int channel;
	int Len;
	Mix_Chunk *sound;
	SDL_RWops *rw;
	byte *dst; 
	FILE *mem, *fdst, *src;
#ifdef WIN32
	FILE *wsrc, *wdst;
#endif
	int con=0;
	byte res=0;

//	printf("Loading %s with (%d) loop\n",file,loop);
	
	while(con<128 && sonido[con].smp!=NULL) 
		con++;
	
	if(con==128) return(-1);

// get len of file

src = fopen(file,"rb");
if(!src)
	return -1;
	
	fseek(src,0,SEEK_END);
	Len = ftell(src);
	
	
	dst = (byte *)malloc((int)Len+255);
	if(dst==NULL)
		return(-1);
		
	memset(dst,0,(int)Len+40);

	mem = fopen(file,"rb"); // open our temp wav	

#ifdef WIN32
//  __APPLE__

	fdst = fopen("divwav.tmp","wb");
	if(fdst==NULL)
		return -1;
#else
	fdst = fmemopen((void *)dst,(int)Len+255,(const char *)"wb");
#endif
	
	res=pcm2wav(mem,Len,fdst,(int)Len+40);
	fclose(mem);
	fseek(fdst,0,SEEK_SET);
	fflush(fdst);
	
	if(res>1) { // 0 = ok. 1 = already wav. 2=failed save, 3=failed read, 4=out of mem
		printf("failed: err %d\n",res);
		free(dst);
		return(-1);
	}
	fclose(fdst);
#ifdef WIN32
	sound = Mix_LoadWAV("divwav.tmp");
	remove("divwav.tmp");
//	remove("divpcm.tmp");
#else
//fflush(fdst);
//rewind(fdst);
	//sound = Mix_LoadWAV(fdst);
	if(res==1) {
		sound = Mix_LoadWAV(file);
	} else {
		rw = SDL_RWFromMem(dst, (int)(Len+255));
		sound = Mix_LoadWAV_RW(rw, 1);
	}
#endif
	if(!sound) {
		printf("Mix_LoadWAV: %s\n", Mix_GetError());
		return (-1);
	}
	free(dst);
	// all ok. save our data to free() later
	sonido[con].smp = 1;
	sonido[con].sound = sound;
	sonido[con].loop = Loop;
//printf("loaded sound in slot: %d\n",con);
	return con;
#endif

#ifdef DOS

  SoundInfo *SI=NULL;
  int con=0;

  while(con<128 && sonido[con].smp!=NULL) con++;
  if(con==128) return(-1);

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
  sonido[con].smp  = SI->sample;
  sonido[con].freq = SI->SoundFreq;

  free(SI);

  return(con);
#endif
return -1;
	
}



int PlaySound(int NumSonido, int Volumen, int Frec) // Vol y Frec (0..256)
{
  int con=0;
  int loop=-1; // always loop if callback used

#ifdef MIXER

#ifdef __EMSCRIPTEN__
  loop = sonido[NumSonido].loop?-1:0;
#endif

//	printf("playing sound %d at vol %d, feq %d\n",NumSonido, Volumen, Frec);
  /*int InitChannel=16;

  if(MusicChannels>InitChannel) InitChannel=MusicChannels;
  if(InitChannel>=32) return(-1);
  if(!sonido[NumSonido].smp) return(-1);

  con=InitChannel;
  while(con<32 && IsPlayingSound(con)) con++;
  if(con==32) {
    con=InitChannel+NextChannel;
    NextChannel++;
    if(InitChannel+NextChannel>=32) NextChannel=0;
    if(con>=32) con=InitChannel;
  }
  */

//  StopSound(con);

// always play as loop, let the freqEffect manage stop_sound when loop is zero
// this permits slow playing sound to run for the correct length.

	con = Mix_PlayChannel(-1, sonido[NumSonido].sound, loop);

if(con==-1) return -1;
  channels[con].freq = Frec;
  channels[con].vol = Volumen;
  channels[con].pos = 0;

//printf("Playing sound %d, %d, Vol %d, Freq %d %x\n",NumSonido, con, Volumen, Frec, sonido[NumSonido].sound);
	
//	if(Frec!=256)
#ifndef __EMSCRIPTEN__
		Mix_RegisterEffect(con, freqEffect, NULL,NULL);
#endif
	
	Mix_Volume(con,Volumen/2);
	channels[con].num=NumSonido;
	channels[con].con = con;
//	printf("playing via channel %d\n",con);
//  judas_playsample(sonido[NumSonido].smp, con, (sonido[NumSonido].freq*Frec)/256, 32*Volumen, MIDDLE);

	Freq_original[con]=channels[con].freq;

//  channel(con)=1;
#endif
//printf("Playing con: %d\n",con);
  return(con);
}

int StopSound(int NumChannel)
{
  //printf("Stopping sound %d\n",NumChannel);
#ifdef MIXER
if(Mix_Playing(NumChannel))
  Mix_HaltChannel(NumChannel);

  if(NumChannel >= CHANNELS) return(-1);
#ifdef DOS
  judas_stopsample(NumChannel);
#endif
#endif
  return(1);
}

int ChangeSound(int NumChannel,int Volumen,int Frec)
{
#ifdef MIXER
//	printf("Channel %d new vol %d new freq %d\n",NumChannel, Volumen, Frec);
	channels[NumChannel].freq=Frec;
	Mix_Volume(NumChannel,Volumen/2);
#endif

#ifdef DOS
  CHANNEL *chptr;

  if(NumChannel >= CHANNELS || NumChannel < MusicChannels) return(-1);

  chptr = &judas_channel[NumChannel];

  chptr->vol       = 32*Volumen;
  chptr->freq      = (Freq_original[NumChannel]*Frec)/256;

#endif

  return(1);
}

int ChangeChannel(int NumChannel,int Volumen,int Panning)
{
#ifdef DOS
  CHANNEL *chptr;

  if(NumChannel >= CHANNELS || NumChannel < MusicChannels) return(-1);

  chptr = &judas_channel[NumChannel];

  chptr->mastervol = Volumen/2;
  chptr->panning   = Panning;
#endif
  return(1);
}

int IsPlayingSound(int NumChannel)
{
	return Mix_Playing(NumChannel);
#ifdef DOS
  SAMPLE *smp = judas_channel[NumChannel].smp;
  char *pos   = judas_channel[NumChannel].pos;

  if( (NumChannel >= CHANNELS) ||
      ((judas_channel[NumChannel].voicemode & VM_ON) == VM_OFF) ||
      (!judas_channel[NumChannel].smp) ) return(0);

//if(NumChannel >= CHANNELS) return(0);

  if (pos >= smp->end || pos < smp->start) return(0);

//printf("%d - %d\n", chptr->pos, chptr->end);

#endif

  return(1);
}
int UnloadSound(int NumSonido) {}


int StartFLI(char *nombre, char *Buffer, int Buff_anc,int Buff_alt,int cx,int cy) {return 0;}
int Nextframe(void) {return 0;}
void ResetFli(void) {}
void EndFli(void) {}


