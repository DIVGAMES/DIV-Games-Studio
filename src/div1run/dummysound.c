#include "inter.h"
#include "osdep.h"
#include "divsound.h"


tSonido  sonido[128];
tCancion cancion[128];
tChannels channels[64];

int SongType=0;
int MusicChannels=0;
int NextChannel=0;
int *NewSound;
int ChannelCon=0;

void EndSound(void) {}

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

print_init_flags(initted);
	
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


int LoadSound(char *file, int loop) {
	
	printf("Loading %s with (%d) loop\n",file,loop);
	
	return 0;
	
}


int UnloadSound(int NumSonido) {}
int PlaySound(int n, int m, int o) {
	printf("Playing sound %d %d %d\n",n,m,o);
	
	return 0;}
int StopSound(int n) {return ;}
int ChangeSound(int n, int m, int o) {return;}


int StartFLI(char *nombre, char *Buffer, int Buff_anc,int Buff_alt,int cx,int cy) {return 0;}
int Nextframe(void) {return 0;}
void ResetFli(void) {}
void EndFli(void) {}


