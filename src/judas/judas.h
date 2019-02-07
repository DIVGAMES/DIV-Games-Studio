
// Versión de las Judas para DIV 2 (para el entorno, que no el intérprete).

// Estan tocados JUDAS/RAW/WAV para que carguen los sonidos incluso si
// judas_device es DEV_SOUND (ya que se tienen que seguir pudiendo editar).

/*
 * JUDAS main header file
 */
#include "judascfg.h"
#include "judaserr.h"

/*
 * Mixer numbers
 */
#define FASTMIXER 1
#define QUALITYMIXER 2

/*
 * Mixmode bits
 */
#define MONO 0
#define STEREO 1
#define EIGHTBIT 0
#define SIXTEENBIT 2

/*
 * Sound device numbers
 */
#define DEV_NOSOUND 0
#define DEV_SB 1
#define DEV_SBPRO 2
#define DEV_SB16 3
#define DEV_GUS 4
#define DEV_FILE 5

/*
 * Voicemode bits
 */
#define VM_OFF 0
#define VM_ON 1
#define VM_ONESHOT 0
#define VM_LOOP 2
#define VM_16BIT 4

/*
 * Panning values (these specific panning positions are mixed with optimized
 * routines which greatly reduce processing time if fast mixer is used!)
 */
#define LEFT 0
#define MIDDLE 128
#define RIGHT 255

/*
 * Sample structure
 */
typedef struct {
	char *start;
	char *repeat;
	char *end;
	char *vuprofile;
	unsigned char voicemode;
} SAMPLE;

/*
 * Informacion devuelta a DIV
 */
typedef struct _SoundInfo {
	int SoundFreq;
	int SoundBits;
	int SoundSize;
	short *SoundData;
	SAMPLE *sample;
} SoundInfo;

/*
 * Channel structure
 */
#ifdef DOS
#pragma pack(push, 1);
#endif

typedef struct {
	char *volatile pos;   /* Position which is currently played */
	char *repeat;	 /* Goes here after hitting end */
	char *end;	    /* Position of last sample + 1 */
	SAMPLE *volatile smp; /* Pointer to sample currently used */
	unsigned freq;	/* Playing frequency in hertz */
	volatile unsigned short fractpos; /* 16bit fractional pos */
	unsigned char mastervol; /* 255 = whole volume range of output */
	unsigned char panning;   /* 0 left, 128 middle, 255 right */
	signed short vol;	/* Note volume, ranges from 0 to 64*256 */
	volatile unsigned char voicemode; /* See the bit definitions above */

	/* Quality mixer internal variables (No need to understand) */
	volatile char
	    prevvm; /* voicemode != prevvm if discontinuity in the sound */
	char *volatile prevpos; /* Pos != prevpos if discontinuity in the sound
				 */
	volatile int lastvalleft; /* Last value from prev. mixed chunk (after
				     volume scaling) */
	volatile int lastvalright;
	volatile int
	    smoothvoll; /* Final volume. Smoothly slided towards given volume */
	volatile int smoothvolr;
} CHANNEL;
#ifdef DOS
#pragma pack(pop);
#endif

/*
 * General functions
 */
void judas_config(void);
int judas_init(unsigned mixrate, unsigned mixer, unsigned mixmode,
	       int interpolation);
void judas_update(void);
void judas_uninit(void);
int judas_songisplaying(void);

/*
 * Sample & channel functions
 */
SAMPLE *judas_allocsample(int length);
void judas_ipcorrect(SAMPLE *smp);
void judas_freesample(SAMPLE *smp);
void judas_playsample(SAMPLE *smp, unsigned chnum, unsigned frequency,
		      unsigned short volume, unsigned char panning);
void judas_stopsample(unsigned chnum);
void judas_preventdistortion(unsigned active_channels);
void judas_setmastervolume(unsigned chnum, unsigned char mastervol);
void judas_setmusicmastervolume(unsigned musicchannels,
				unsigned char mastervol);
void judas_setsfxmastervolume(unsigned musicchannels, unsigned char mastervol);
float judas_getvumeter(unsigned chnum); /* Returns 0..1 */

/*
 * RAW sample & WAV functions
 */
SoundInfo *judas_loadrawsample(char *name, int repeat, int end,
			       unsigned char voicemode);
SoundInfo *judas_loadrawsample_mem(char *FileBuffer, int length, int repeat,
				   int end, unsigned char voicemode);
SoundInfo *judas_loadwav(char *name);
SoundInfo *judas_loadwav_mem(unsigned char *FileBuffer);

/*
 * WAV writer functions
 */
int judas_wavwriter_open(char *name);
int judas_wavwriter_writesome(int handle);
int judas_wavwriter_close(int handle);

/*
 * XM player functions
 */
int judas_loadxm(char *name);
void judas_freexm(void);
void judas_playxm(int rounds);
void judas_stopxm(void);
void judas_forwardxm(void);
void judas_rewindxm(void);
unsigned char judas_getxmpos(void);
unsigned char judas_getxmline(void);
unsigned char judas_getxmtick(void);
unsigned char judas_getxmchannels(void);
char *judas_getxmname(void);

/*
 * MOD player functions
 */
int judas_loadmod(char *name);
void judas_freemod(void);
void judas_playmod(int rounds);
void judas_stopmod(void);
void judas_forwardmod(void);
void judas_rewindmod(void);
unsigned char judas_getmodpos(void);
unsigned char judas_getmodline(void);
unsigned char judas_getmodtick(void);
unsigned char judas_getmodchannels(void);
char *judas_getmodname(void);

/*
 * S3M player functions
 */
int judas_loads3m(char *name);
void judas_frees3m(void);
void judas_plays3m(int rounds);
void judas_stops3m(void);
void judas_forwards3m(void);
void judas_rewinds3m(void);
unsigned char judas_gets3mpos(void);
unsigned char judas_gets3mline(void);
unsigned char judas_gets3mtick(void);
unsigned char judas_gets3mchannels(void);
char *judas_gets3mname(void);

/*
 * JUDAS IO functions
 */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
int judas_open(char *name);
int judas_seek(int handle, int bytes, int whence);
int judas_read(int handle, void *buffer, int size);
void judas_close(int handle);

/*
 * DPMI memory locking functions (use them, and no more crashes under
 * multitaskers!)
 */
int judas_memlock(void *start, unsigned size);
int judas_memunlock(void *start, unsigned size);
void *locked_malloc(int size);
void locked_free(void *address);

/*
 * Sound configuration (may be changed anytime, used only by judas_init())
 */
extern unsigned judascfg_device;
extern unsigned judascfg_port;
extern unsigned judascfg_irq;
extern unsigned judascfg_dma1;
extern unsigned judascfg_dma2; /* SB High DMA, not needed for GUS */

/*
 * Device, port, IRQ, DMA, mixrate & mixmode currently in use.
 * Don't change them!
 */
extern unsigned judas_device;
extern unsigned judas_port;
extern unsigned judas_irq;
extern unsigned judas_dma;
extern unsigned judas_mixrate;
extern unsigned char judas_mixmode;

/*
 * Sound device names
 */
extern char *judas_devname[];

/*
 * Mixer names
 */
extern char *judas_mixername[];

/*
 * Mixmode names
 */
extern char *judas_mixmodename[];

/*
 * Interpolation mode names
 */
extern char *judas_ipmodename[];

/*
 * Error code
 */
extern int judas_error;

/*
 * Text strings for the error codes
 */
extern char *judas_errortext[];

/*
 * Channel array
 */
extern CHANNEL judas_channel[CHANNELS];

/*
 * Fake sample returned by the routines when in NOSOUND mode
 */
extern SAMPLE fakesample;

/*
 * Clipping indicator (quality mixer only) indicates the hardest
 * clipping occurred. 0 = has not clipped, 64 = clipping starts,
 * 128 = half volume wouldn't clip, 255 = 1/4 volume wouldn't clip
 * Does not give values in range 1..63.
 */
extern volatile char judas_clipped;

// Debug functions
void DebugInfo(char *Msg);
void DebugData(int Val);
