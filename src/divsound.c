
#include "global.h"
#include "divsound.h"
#include "divsb.h"
#include "divmixer.hpp"

int reloj=0;
extern int safe;

int SoundActive=0;
//int SoundActive=1;

void InitSound(void)
{
//	SDL_Init(SDL_INIT_AUDIO);
//	printf("Sound system initialised\n");


//SDL_InitSubSystem( SDL_INIT_AUDIO );
#ifdef MIXER

int audio_rate = 44100;
Uint16 audio_format = AUDIO_S16SYS;
int audio_channels = 2;
int audio_buffers = 4096;


 
if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
	fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
//	exit(1);
}

#endif

#ifdef NOTYET
  FILE *File_Cfg;
  int master=7,sound_fx=7,cd_audio=7;
  char Device[8]={DEV_NOSOUND,DEV_SB,DEV_SB,DEV_SBPRO,DEV_SB16,DEV_SB16,DEV_GUS,DEV_GUS};
  unsigned short Puertos[6]={0x210,0x220,0x230,0x240,0x250,0x260};
  unsigned short IRQS[10]  ={2,3,5,7,10,11,12,13,14,15};
  unsigned short DMAS[10]  ={0,1,2,3,4,5,6,7,8,9};
  unsigned short DMAS2[10] ={0,1,2,3,4,5,6,7,8,9};

  int mixer         = QUALITYMIXER;
//int mixer         = FASTMIXER;
  int mixrate       = 44100;
  int mixmode       = SIXTEENBIT | MONO;
//int mixmode       = SIXTEENBIT | STEREO;
//int mixmode       = EIGHTBIT | MONO;
  int interpolation = 1;
  int cfg_dev, cfg_port, cfg_irq, cfg_dma1, cfg_dma2;

  SoundError=0;

  atexit(judas_uninit);
  atexit(timer_uninit);

  if(SoundActive) {
    judas_config();
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
        if (mixer<1 || mixer>2) mixer=1;
        if (mixrate<11025) mixrate=11025;
        if (mixrate>44100) mixrate=44100;
        if (mixmode==16) mixmode=SIXTEENBIT | STEREO;
        else mixmode=EIGHTBIT | STEREO;
      }

      fclose(File_Cfg);
    }

    judas_config();

    File_Cfg=fopen("sound.cfg","rb");
    if(File_Cfg!=NULL) {
      fclose(File_Cfg);
      judascfg_device = Device[cfg_dev];
      judascfg_port   = Puertos[cfg_port];
      judascfg_irq    = IRQS[cfg_irq];
      judascfg_dma1   = DMAS[cfg_dma1];
      judascfg_dma2   = DMAS2[cfg_dma2];
    }
  } else {
    judascfg_device = DEV_NOSOUND;
  }

//  timer_init(1193180 / 100, judas_update);

  if (safe==33) { // Modo a prueba de fallos

    judascfg_device = DEV_NOSOUND;
    judas_init(mixrate, mixer, mixmode, interpolation);

  } else {

    if(!judas_init(mixrate, mixer, mixmode, interpolation)) {
      if (judas_error==JUDAS_HARDWARE_ERROR) SoundError=1;
      judascfg_device=DEV_NOSOUND;
      judas_init(mixrate, mixer, mixmode, interpolation);
    } else {
      judas_setmusicmastervolume(CHANNELS, 50);
    }

  }
#endif
}

void EndSound(void)
{
#ifdef MIXER
Mix_CloseAudio();
#endif

//SDL_QuitSubSystem(SDL_INIT_AUDIO);
	
#ifdef NOTYET
  judas_uninit();
  timer_uninit();
#endif
}

