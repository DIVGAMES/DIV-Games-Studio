#ifndef __DIV_SOUND_
#define __DIV_SOUND_

#define CHANNELS    128
#define DEV_NOSOUND 0
#define DEV_SB      1
#define DEV_SBPRO   2
#define DEV_SB16    3
#define DEV_GUS     4
#define FASTMIXER   0
#define SIXTEENBIT  0
#define STEREO      0
#define EIGHTBIT    0

#define MOD 1
#define S3M 2
#define XM  3
#ifndef DOS
#define IT  4 // (Impulse Tracker, added to modern platforms)
#endif

#define SAMPLE char

typedef struct _t_channels {
	int freq;
	int vol;
	int con;
	int num;
	int pos;
} t_channels;

typedef struct _t_sonido {
	SAMPLE     smp;
#ifdef MIXER
	Mix_Chunk *sound;
	char       loop;
#endif
} t_sonido;

typedef struct _t_cancion {
	char      *ptr;
	int        loop;
	int        SongType;
#ifdef MIXER
	Mix_Music *music;
	SDL_RWops *rw;
#endif
} t_cancion;

extern t_sonido  sonido[128];
extern t_cancion cancion[128];
extern int *NewSound;
extern int ChannelCon;

void DIVInitSound(void);
void DIVResetSound(void);
int  DIVLoadSound(char *ptr, long Len, int Loop);
int  DIVUnloadSound(int NumSonido);
int  DIVPlaySound(int NumSonido, int Volumen, int Frec);
int  DIVStopSound(int NumChannel);
int  DIVChangeSound(int NumChannel, int Volumen, int Frec);
int  DIVChangeChannel(int NumChannel, int Volumen, int Panning);
int  DIVIsPlayingSound(int NumChannel);
int  DIVLoadSong(char *ptr, int Len, int Loop);
int  DIVPlaySong(int NumSong);
void DIVStopSong(void);
void DIVUnloadSong(int NumSong);
void DIVSetSongPos(int SongPat);
int  DIVGetSongPos(void);
int  DIVGetSongLine(void);
int  DIVIsPlayingSong(void);
void DIVEndSound(void);

// For in-memory pcm to wav conversion
typedef struct _t_HeadDC {
	uint32_t dwUnknow;
	uint16_t wFormatTag;
	uint16_t wChannels;
	uint32_t dwSamplePerSec;
	uint32_t dwAvgBytesPerSec;
	uint16_t wBlockAlign;
	uint16_t wBits;
} t_HeadDC;

#endif
