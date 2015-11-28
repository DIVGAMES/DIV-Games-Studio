#ifndef __DIV_SOUND_
#define __DIV_SOUND_

#define CHANNELS 128
#define DEV_NOSOUND 0
#define DEV_SB 1
#define DEV_SBPRO 2
#define DEV_SB16 3
#define DEV_GUS 4
#define FASTMIXER 0
#define SIXTEENBIT 0
#define STEREO 0
#define EIGHTBIT 0


#define MOD 1
#define S3M 2
#define XM  3

#define SAMPLE char

typedef struct _channels {
  int freq;
  int vol;
  int con;
  int num;
  int pos;
} tChannels;	

typedef struct _sonido {
  SAMPLE smp;
#ifdef MIXER
  Mix_Chunk *sound;
  char loop;
#endif
} tSonido;

typedef struct _cancion {
  char *ptr;
  int  loop;
  int  SongType;
#ifdef MIXER
  Mix_Music* music;
  SDL_RWops *rw;
#endif
} tCancion;

extern tSonido  sonido[128];
extern tCancion cancion[128];
extern int *NewSound;
extern int ChannelCon;

void InitSound              (void);
void ResetSound             (void);
int  LoadSound              (char *file, int Loop);
int  UnloadSound            (int NumSonido);
int  PlaySound              (int NumSonido,int Volumen,int Frec);
int  StopSound              (int NumChannel);
int  ChangeSound            (int NumChannel,int Volumen,int Frec);
int  ChangeChannel          (int NumChannel,int Volumen,int Panning);
int  IsPlayingSound         (int NumChannel);
int  LoadSong               (char *ptr, int Len, int Loop);
int  PlaySong               (int NumSong);
void StopSong               (void);
void UnloadSong             (int NumSong);
void SetSongPos             (int SongPat);
int  GetSongPos             (void);
int  GetSongLine            (void);
int  IsPlayingSong          (void);
void EndSound               (void);

#endif
