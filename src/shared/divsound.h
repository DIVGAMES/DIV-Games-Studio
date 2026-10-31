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
  int ctr;
  int num;
  int pos;
} tChannels;	

typedef struct _sound {
  SAMPLE smp;
#ifdef MIXER
  Mix_Chunk *sound;
  char loop;
#endif
} tSound;

typedef struct _song {
  char *ptr;
  int  loop;
  int  SongType;
#ifdef MIXER
  Mix_Music* music;
  SDL_RWops *rw;
#endif
} tSong;

extern tSound  sounds[128];
extern tSong songs[128];
extern int *NewSound;
extern int ChannelCon;

void InitSound              (void);
void ResetSound             (void);
int  LoadSound              (char *ptr, long Len, int Loop);
int  UnloadSound            (int NumSound);
int  DivPlaySound           (int NumSound,int Volume,int Frec);
int  StopSound              (int NumChannel);
int  ChangeSound            (int NumChannel,int Volume,int Frec);
int  ChangeChannel          (int NumChannel,int Volume,int Panning);
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

// For in-memory pcm to wav conversion 
typedef struct _HeadDC {
  uint32_t   dwUnknow;
  uint16_t wFormatTag;
  uint16_t wChannels;
  uint32_t   dwSamplePerSec;
  uint32_t   dwAvgBytesPerSec;
  uint16_t wBlockAlign;
  uint16_t wBits;
} HeadDC;

#endif
