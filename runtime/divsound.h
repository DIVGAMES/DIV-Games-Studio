#ifndef __DIV_SOUND_
#define __DIV_SOUND_

#define MOD 1
#define S3M 2
#define XM  3

#define SAMPLE char


typedef struct _sonido {
  SAMPLE *smp;
  int    freq;
} tSonido;

typedef struct _cancion {
  char *ptr;
  int  loop;
  int  SongType;
} tCancion;

extern tSonido  sonido[128];
extern tCancion cancion[128];
extern int *NewSound;
extern int ChannelCon;

void InitSound              (void);
void ResetSound             (void);
int  LoadSound              (char *ptr, long Len, int Loop);
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
