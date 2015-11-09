
// Versi¢n de las Judas para DIV 2 (para el entorno, que no el int‚rprete).

// Estan tocados JUDAS/RAW/WAV para que carguen los sonidos incluso si
// judas_device es DEV_SOUND (ya que se tienen que seguir pudiendo editar).

/*
 * JUDAS Apocalyptic Softwaremixing Sound System V2.06y by Faust & Yehar
 *
 * Supported:
 * - SOUND BLASTER (8bit, mono, up to 22050 Hz)
 * - SOUND BLASTER PRO (8bit, stereo, up to 22050 Hz or mono up to 44100 Hz)
 * - SOUND BLASTER 16 (16bit, stereo, up to 44100 Hz)
 * - ULTRASOUND (16bit, stereo, up to 44100 Hz)
 * - WAV file output
 * - XMs, MODs, S3Ms, WAVs, raw samples
 *
 * Other features:
 * - Clipping of sound output
 * - Interpolation options: None, linear, cubic
 * - Quality mixer: Click removing, high quality, 32 bit, clipping indicator
 *
 * This is the main module, where soundcards & mixer are initialized. It's not
 * very good coding style, but who cares!
 *
 * Changes:
 * V2.01  Doesn't fuck up if soundcard variables contain XXX-shit!
 * V2.04y Added selectable quality mixer. Old mixer is now called fast mixer
 *        Added file writer
 * V2.06y Wrote "char *volatile pos" instead of old "volatile char *pos"!
 *        Removed file writer and made the WAV writer in JUDASWAV.C
 *        (Read JUDAS.DOC for complete history)
 */

//#include <io.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
//#include <conio.h>
#include <stdlib.h>
//#include <dos.h>
//#include <mem.h>
#include "judasdma.h"
#include "judasmem.h"
#include "judascfg.h"
#include "judasgus.h"
#include "judaserr.h"
#include "osdep.h"

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
 * Interrupt controller ports
 */
#define PICPORT1 0x20
#define PICPORT2 0xa0
#define PICMASK1 0x21
#define PICMASK2 0xa1

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
 * Voicemode bits
 */
#define VM_OFF 0
#define VM_ON 1
#define VM_LOOP 2
#define VM_16BIT 4

/*
 * Sample & channel structures
 */
typedef struct
{
        char *start;
        char *repeat;
        char *end;
        char *vuprofile;
        unsigned char voicemode;
} SAMPLE;

#pragma pack (push, 1);
typedef struct
{
        char *volatile pos;
        char *repeat;
        char *end;
        SAMPLE *volatile sample;
        unsigned freq;
        volatile unsigned short fractpos;
        unsigned char mastervol;
        unsigned char panning;
        signed short vol;
        volatile unsigned char voicemode;

        /* For quality mixer */
        volatile char prevvm;
        char *volatile prevpos;
        volatile int lastvalleft;
        volatile int lastvalright;
        volatile int smoothvoll;
        volatile int smoothvolr;
} CHANNEL;
#pragma pack (pop);

/*
 * Prototypes
 */
void judas_config(void);
int judas_init(unsigned mixrate, unsigned mixer, unsigned mixmode, int interpolation);
void judas_uninit(void);
int judas_songisplaying(void);
static int judas_lock(void);
static int judas_unlock(void);
static int initmixer(void);
static void sb_delay(void);
static void sb_write(unsigned char value);
static unsigned char sb_read(void);
static int sb_reset(void);
static void sb_getversion(void);
static int gus_detect(void);
static void gus_reset(void);
static void gus_delay(void);
static void gus_setupchannels(void);
static void gus_setupchannel(unsigned char channel, unsigned start, unsigned length);
static void gus_stopchannel(unsigned char channel);

/*
 * Assembler functions in JUDASASM.ASM
 */

//#define interrupt 

void judas_update(void);
void sb_handler(void);
void sb_aihandler(void);
void sb16_handler(void);
void gus_handler(void);
void gus_poke(unsigned location, unsigned char data);
unsigned char gus_peek(unsigned location);
void gus_startchannels(void);
void gus_dmaprogram(unsigned pos, unsigned length);
void gus_dmainit(void);
void gus_dmawait(void);
void fmixer(void *address, int length);
void qmixer(void *address, int length);
void safemixer(void *address, int length);
void normalmix(void);
void ipmix(void);
void qmix_linear(void);
void qmix_cubic(void);
void judas_code_lock_start(void);
void judas_code_lock_end(void);

/*
 * Inline function to get the value of DS
 */
unsigned short judas_get_ds(void);

#pragma aux judas_get_ds = \
"mov ax, ds" \
modify [ax] \
value [ax];

/*
 * Variables
 */
int judas_error = JUDAS_OK;
void (*judas_player)(void) = NULL;
void (*judas_mixroutine)(void) = &normalmix;
void (*judas_mixersys)(void *address, int length) = &qmixer;
static void (__interrupt __far *judas_oldvect)();
static void (__interrupt __far *judas_newvect)();
unsigned judascfg_device = DEV_NOSOUND;
unsigned judascfg_port = -1;
unsigned judascfg_irq = -1;
unsigned judascfg_dma1 = -1;
unsigned judascfg_dma2 = -1;
unsigned judas_irqcount = 0;
unsigned judas_device;
unsigned judas_port;
unsigned judas_irq;
unsigned judas_dma;
unsigned judas_int;
unsigned judas_mixrate;
unsigned judas_mixpos;
unsigned judas_bufferlength;
unsigned judas_buffermask;
unsigned judas_bpmcount;
int *judas_clipbuffer;
int *judas_zladdbuffer; /* Alustukset, „„li”! *** */
int judas_zerolevell = 0;
int judas_zerolevelr = 0;
short *judas_cliptable;
int *judas_volumetable;
unsigned short judas_ds;
static unsigned short dsp_version;
static unsigned char mixer_firsttime = 1;
static unsigned char judas_locked = 0;
static unsigned char judas_oldpicmask1;
static unsigned char judas_oldpicmask2;
unsigned char judas_initialized = 0;
unsigned char judas_mixer;
unsigned char judas_mixmode;
unsigned char judas_bpmtempo;
unsigned char judas_samplesize;
CHANNEL judas_channel[CHANNELS];
char judas_clipped = 0;
char *filewriterbuffer;
int filewriterbuffersize = 65536;

/*
 * JUDAS device names
 */
char *judas_devname[] =
{
        "No Sound",
        "Sound Blaster",
        "Sound Blaster Pro",
        "Sound Blaster 16",
        "UltraSound",
        "File Writer"
};

/*
 * Mixer names
 */
char *judas_mixername[] =
{
        "(No mixer, big shit will happen)",
        "Fast Mixer",
        "Quality Mixer",
};

/*
 * Mixmode names
 */
char *judas_mixmodename[] =
{
        "8-bit mono",
        "8-bit stereo",
        "16-bit mono",
        "16-bit stereo"
};

/*
 * Interpolation mode names
 */
char *judas_ipmodename[] =
{
        "(?)",
        "(?)"
};

/*
 * JUDAS error texts
 */
char *judas_errortext[] =
{
        "Everything OK",
        "Couldn't open file",
        "Couldn't read file",
        "Incorrect file format",
        "Out of memory",
        "Hardware init failure",
        "Configuration incorrect",
        "Out of channels"
};

static unsigned char gus_dmalatch[] =
{
        0x40, 0x41, 0x40, 0x42, 0x40, 0x43, 0x44, 0x45
};

static unsigned char gus_irqlatch[] =
{
        0x40, 0x40, 0x40, 0x43, 0x40, 0x42, 0x40, 0x44,
        0x40, 0x41, 0x40, 0x45, 0x46, 0x40, 0x40, 0x47
};

/*
 * Fake sample returned by the routines when in NOSOUND mode
 */
SAMPLE fakesample = {NULL, NULL, NULL, NULL, VM_OFF};

int judas_songisplaying(void) {
  return (judas_player != NULL)? 1:0;
}

void judas_config(void)
{
        char *envstr;

        judascfg_device = DEV_NOSOUND;
        /*
         * Try to find BLASTER enviroment variable
         */
        envstr = getenv("BLASTER");
        if (envstr)
        {
                judascfg_device = DEV_SB;
                judascfg_port = -1;
                judascfg_irq = -1;
                judascfg_dma1 = -1;
                judascfg_dma2 = -1;
                while (*envstr)
                {
                        unsigned sb_type = 0;
                        if ((*envstr == 'A') || (*envstr == 'a')) sscanf(envstr + 1, "%x", &judascfg_port);
                        if ((*envstr == 'I') || (*envstr == 'i')) sscanf(envstr + 1, "%d", &judascfg_irq);
                        if ((*envstr == 'D') || (*envstr == 'd'))
                        {
                                sscanf(envstr + 1, "%d", &judascfg_dma1);
                        }
                        if ((*envstr == 'T') || (*envstr == 't'))
                        {
                                sscanf(envstr + 1, "%d", &sb_type);
                                if ((sb_type == 2) || (sb_type == 4)) judascfg_device = DEV_SBPRO;
                        }
                        if ((*envstr == 'H') || (*envstr == 'h'))
                        {
                                sscanf(envstr + 1, "%d", &judascfg_dma2);
                                judascfg_device = DEV_SB16;
                        }
                        envstr++;
                }
        }
        /*
         * Try to find ULTRASND enviroment variable
         */
        envstr = getenv("ULTRASND");
        if (envstr)
        {
                unsigned irq2;
                judascfg_device = DEV_GUS;
                judascfg_port = -1;
                judascfg_irq = -1;
                judascfg_dma1 = -1;
                judascfg_dma2 = -1;
                sscanf(envstr, "%x,%d,%d,%d,%d", &judascfg_port, &judascfg_dma1,
                        &judascfg_dma2, &judascfg_irq, &irq2);
                /* If MIDI IRQ is lower then use it (for DOS4G) */
                if (irq2 < judascfg_irq) judascfg_irq = irq2;
        }
}

int judas_init(unsigned mixrate, unsigned mixer, unsigned mixmode, int interpolation)
{
        int sbrate;
        /*
         * Check for illegal values
         */
        judas_error = JUDAS_ILLEGAL_CONFIG;
        if (judascfg_device > DEV_FILE) return 0;
        if (judascfg_device != DEV_NOSOUND && judascfg_device != DEV_FILE)
        {
                if (judascfg_port < 0x200) return 0;
                if (judascfg_port > 0x2ff) return 0;
                if (judascfg_irq < 2) return 0;
                if (judascfg_irq > 15) return 0;
                if (judascfg_dma1 > 7) return 0;
                if (judascfg_device == DEV_SB16)
                {
                        if (judascfg_dma2 > 7) return 0;
                }
                if (judascfg_irq == 9) judascfg_irq = 2;
        }
        if (mixrate < 5000) mixrate = 5000;
        if (mixrate > 44100) mixrate = 44100;
        if ((mixer != FASTMIXER) && (mixer != QUALITYMIXER)) return 0;
        /*
         * If user wants to re-initialize, shutdown first
         */
        if (judas_initialized) judas_uninit();
        judas_mixrate = mixrate;
        judas_mixmode = mixmode & (SIXTEENBIT | STEREO);
        judas_mixer = mixer;
        /*
         * Copy the config to currently used values
         */
        judas_device = judascfg_device;
        judas_port = judascfg_port;
        judas_irq = judascfg_irq;
        judas_dma = judascfg_dma1;
        if (mixer == QUALITYMIXER) {
                judas_mixersys = &qmixer;
                if (interpolation) judas_mixroutine = &qmix_cubic;
                else judas_mixroutine = &qmix_linear;
                judas_ipmodename[0] = "Linear";
                judas_ipmodename[1] = "Cubic";
        } else {
                judas_ipmodename[0] = "No";
                judas_ipmodename[1] = "Linear";
                judas_mixersys = &fmixer;
                if (interpolation) judas_mixroutine = &ipmix;
                else judas_mixroutine = &normalmix;
        }

        /*
         * If it's NOSOUND, don't go further
         */
        if (judas_device == DEV_NOSOUND)
        {
                judas_error = JUDAS_OK;
                return 1;
        }
        /*
         * Detect soundcard
         */

        judas_error = JUDAS_HARDWARE_ERROR;
        switch (judas_device)
        {
                case DEV_SB:
                {
                        sbrate = 256 - 1000000 / judas_mixrate;
                        if (sbrate > 210) sbrate = 210;
                        judas_mixrate = 1000000 / (256 - sbrate);
                }
                if (!sb_reset()) return 0;
                sb_getversion();
                if (dsp_version > 0x200) judas_newvect = &sb_aihandler;
                else judas_newvect = sb_handler;
                judas_mixmode = EIGHTBIT | MONO;
                break;

                case DEV_SBPRO:
                if (judas_mixmode & STEREO)
                {
                        sbrate = 256 - 500000 / judas_mixrate;
                        if (sbrate > 233) sbrate = 233;
                        judas_mixrate = 500000 / (256 - sbrate);
                }
                else
                {
                        sbrate = 256 - 1000000 / judas_mixrate;
                        if (sbrate > 233) sbrate = 233;
                        judas_mixrate = 1000000 / (256 - sbrate);
                }
                if (!sb_reset()) return 0;
                sb_getversion();
                if (dsp_version < 0x300) return 0;
                judas_newvect = &sb_aihandler;
                judas_mixmode &= STEREO;
                break;

                case DEV_SB16:
                if (!sb_reset()) return 0;
                sb_getversion();
                if (dsp_version < 0x400) return 0;
                if (judas_mixmode & SIXTEENBIT)
                {
                        judas_dma = judascfg_dma2;
                        judas_newvect = &sb16_handler;
                }
                else judas_newvect = &sb_aihandler;
                break;

                case DEV_GUS:
                if (!gus_detect()) return 0;
                gus_reset();
                gus_dmainit();
                {
                        unsigned gus_rate;

                        /*
                         * This stupidity is needed to keep mixrate above 5000
                         */
                        if (judas_mixrate < 5100) judas_mixrate = 5100;
                        gus_rate = (judas_mixrate << 9) / 44100;
                        judas_mixrate = (gus_rate * 44100) >> 9;
                }
                judas_newvect = &gus_handler;
                break;
        }

        /*
         * Calculate sample size & buffer length, set initial mixing pos.
         */
        judas_samplesize = 1;
        judas_buffermask = 0xfffffff8;
        if (judas_mixmode & STEREO)
        {
                judas_samplesize <<= 1;
                judas_buffermask <<= 1;
        }
        if (judas_mixmode & SIXTEENBIT)
        {
                judas_samplesize <<= 1;
                judas_buffermask <<= 1;
        }
        /*
         * For Standard GUS, mask is always 64 bytes to ensure proper DMA
         * transfer alignment even with 16-bit DMA + stereo output.
         */
        if (judas_device == DEV_GUS)
        {
                judas_buffermask = 0xffffffc0;
        }
        judas_bufferlength = judas_mixrate / PER_SECOND * judas_samplesize;
        if (judas_bufferlength < 512) judas_bufferlength = 512;
        if (judas_bufferlength > (DMA_MAXSIZE - 64)) judas_bufferlength = DMA_MAXSIZE - 64;
        judas_bufferlength &= judas_buffermask;
        judas_mixpos = 0;
        /*
         * Reserve dma buffer, initialize mixer tables and lock JUDAS code &
         * data, each of them must be successful
         */
        judas_error = JUDAS_OUT_OF_MEMORY;
        if (judas_device != DEV_FILE) {
          if (!dma_reserve(44100 / PER_SECOND * 4 + 64)) return 0;
        }
        if (!initmixer()) return 0;
        if (!judas_lock()) return 0;
        judas_ds = judas_get_ds();
        /*
         * Unmask IRQ & set vector
         */
        if (judas_device != DEV_FILE) {
          judas_oldpicmask1 = inp(PICMASK1);
          judas_oldpicmask2 = inp(PICMASK2);
          if (judas_irq < 8)
          {
                  outp(PICMASK1, judas_oldpicmask1 & ~(1 << judas_irq));
                  judas_int = judas_irq + 0x8;
          }
          else
          {
                  outp(PICMASK1, judas_oldpicmask1 & 0xfb);
                  outp(PICMASK2, judas_oldpicmask2 & ~(1 << (judas_irq & 3)));
                  judas_int = judas_irq + 0x68;
          }
          judas_oldvect = _dos_getvect(judas_int);
          _dos_setvect(judas_int, judas_newvect);
        }
        /*
         * Everything is prepared. Now just set up the soundcard for output!
         */
        switch (judas_device)
        {
                case DEV_SB:
                memset((char *)dma_address, 0x80, judas_bufferlength);
                dma_program(DMA_WRITE_LOOP, 0, judas_bufferlength);
                sb_write(0xd1);
                sb_write(0x40);
                sb_write(sbrate);
                if (dsp_version <= 0x200)
                {
                        sb_write(0x14);
                        sb_write(0xf0);
                        sb_write(0xff);
                }
                else
                {
                        sb_write(0x48);
                        sb_write(0xf0);
                        sb_write(0xff);
                        sb_write(0x1c);
                }
                break;

                case DEV_SBPRO:
                memset((char *)dma_address, 0x80, judas_bufferlength);
                if (judas_mixmode & STEREO)
                {
                        int timeout = 0xfffff;

                        outp(judas_port + 4, 0xe);
                        outp(judas_port + 5, inp(judas_port + 5) | 0x2);
                        /*
                         * To make left & right correct, send one silent
                         * byte with singlecycle output, and wait for the
                         * transfer to complete.
                         */
                        judas_irqcount = 0;
                        dma_program(DMA_WRITE_ONESHOT, 0, 1);
                        sb_write(0x14);
                        sb_write(0x0);
                        sb_write(0x0);
                        while (!judas_irqcount)
                        {
                                timeout--;
                                if (!timeout) break;
                        }
                }
                else
                {
                        outp(judas_port + 4, 0xe);
                        outp(judas_port + 5, inp(judas_port + 5) & 0xfd);
                }
                dma_program(DMA_WRITE_LOOP, 0, judas_bufferlength);
                sb_write(0xd1);
                sb_write(0x40);
                sb_write(sbrate);
                sb_write(0x48);
                sb_write(0xf0);
                sb_write(0xff);
                /* Use highspeed mode if timeconstant > 210 */
                if (sbrate > 210) sb_write(0x90);
                else sb_write(0x1c);
                break;

                case DEV_SB16:
                sb_write(0x41);
                sb_write(judas_mixrate >> 8);
                sb_write(judas_mixrate & 0xff);
                if (judas_mixmode & SIXTEENBIT) memset((char *)dma_address, 0, judas_bufferlength);
                else memset((char *)dma_address, 0x80, judas_bufferlength);
                dma_program(DMA_WRITE_LOOP, 0, judas_bufferlength);
                if (judas_mixmode & SIXTEENBIT)
                {
                        if (judas_mixmode & STEREO)
                        {
                                sb_write(0xb6);
                                sb_write(0x30);
                                sb_write(0xf0);
                                sb_write(0xff);
                        }
                        else
                        {
                                sb_write(0xb6);
                                sb_write(0x10);
                                sb_write(0xf0);
                                sb_write(0xff);
                        }
                }
                else
                {
                        if (judas_mixmode & STEREO)
                        {
                                sb_write(0xc6);
                                sb_write(0x20);
                                sb_write(0xf0);
                                sb_write(0xff);
                        }
                        else
                        {
                                sb_write(0xc6);
                                sb_write(0x00);
                                sb_write(0xf0);
                                sb_write(0xff);
                        }
                }
                break;

                case DEV_GUS:
                if (judas_mixmode & SIXTEENBIT) memset((char *)dma_address, 0, judas_bufferlength + 64);
                else memset((char *)dma_address, 0x80, judas_bufferlength + 64);
                gus_dmaprogram(0, judas_bufferlength + 64);
                gus_dmawait();
                gus_setupchannels();
                gus_startchannels();
                break;
        }
        judas_initialized = 1;
        judas_clipped = 0;
        judas_error = JUDAS_OK;
        return 1;
}


void judas_uninit(void)
{
        judas_error = JUDAS_OK;
        if (!judas_initialized) return;
        judas_initialized = 0;
        /*
         * Soundblaster is best shut down by doing a DSP reset twice. For SBPRO,
         * we also deactivate the stereo mode. GUS is shut down by simply
         * stopping the channels.
         */
        switch (judas_device)
        {
                case DEV_SB:
                case DEV_SB16:
                sb_reset();
                sb_reset();
                break;

                case DEV_SBPRO:
                sb_reset();
                sb_reset();
                outp(judas_port + 4, 0xe);
                outp(judas_port + 5, inp(judas_port + 5) & 0xfd);
                break;

                case DEV_GUS:
                gus_stopchannel(0);
                gus_stopchannel(1);
                break;
        }
        /*
         * Then restore the PIC mask and IRQ vector.
         */
        if (judas_device != DEV_FILE) {
          outp(PICMASK1, judas_oldpicmask1);
          outp(PICMASK2, judas_oldpicmask2);
          _dos_setvect(judas_int, judas_oldvect);
        }
        judas_unlock();
        free_the_XXXXXXX_dma_memory();
}

static void sb_delay(void)
{
        unsigned char temp;
        char counter = 15;

        while (counter--) temp = inp(judas_port + 6);
}

static void sb_write(unsigned char value)
{
        int timeout = 0xffff;
        while (inp(judas_port + 12) & 0x80)
        {
                timeout--;
                if (!timeout) return;
        }
        outp(judas_port + 12, value);
}

static unsigned char sb_read(void)
{
        int timeout = 0xffff;
        while (!(inp(judas_port + 14) & 0x80))
        {
                timeout--;
                if (!timeout) return 0;
        }
        return inp(judas_port + 10);
}

static int sb_reset(void)
{
        outp(judas_port + 6, 1);
        sb_delay();
        outp(judas_port + 6, 0);
        if (sb_read() == 0xaa) return 1;
        return 0;
}

static void sb_getversion(void)
{
        sb_write(0xe1);
        dsp_version = sb_read() << 8;
        dsp_version += sb_read();
}

static void gus_delay(void)
{
        int count = 70;
        unsigned char temp;
        while (count--) temp = inp(judas_port);
}

static int gus_detect(void)
{
        outp(judas_port + GF1_REG_SELECT, MASTER_RESET);
        outp(judas_port + GF1_DATA_HI, 0x0);
        gus_delay();
        outp(judas_port + GF1_REG_SELECT, MASTER_RESET);
        outp(judas_port + GF1_DATA_HI, GF1_MASTER_RESET);
        gus_delay();
        gus_poke(0, 0xaa);
        gus_poke(1, 0x55);
        if (gus_peek(0) != 0xaa) return 0;
        return 1;
}

static void gus_reset(void)
{
        unsigned char temp;

        outp(judas_port + 0xf, 0x5);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_OUTPUT);
        outp(judas_port + GF1_IRQ_CTRL, 0x0);
        outp(judas_port + 0xf, 0x0);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_OUTPUT);
        outp(judas_port + GF1_IRQ_CTRL, gus_dmalatch[judas_dma] | 0x80);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_OUTPUT | SELECT_GF1_REG);
        outp(judas_port + GF1_IRQ_CTRL, gus_irqlatch[judas_irq]);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_OUTPUT);
        outp(judas_port + GF1_IRQ_CTRL, gus_dmalatch[judas_dma] | 0x80);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_OUTPUT | SELECT_GF1_REG);
        outp(judas_port + GF1_IRQ_CTRL, gus_irqlatch[judas_irq]);
        outp(judas_port + GF1_PAGE, 0x0);
        outp(judas_port, ENABLE_LINE_IN | ENABLE_GF1_IRQ);
        outp(judas_port + GF1_REG_SELECT, DMA_CONTROL);
        outp(judas_port + GF1_DATA_HI, 0x0);
        outp(judas_port + GF1_REG_SELECT, TIMER_CONTROL);
        outp(judas_port + GF1_DATA_HI, 0x0);
        outp(judas_port + GF1_REG_SELECT, SAMPLE_CONTROL);
        outp(judas_port + GF1_DATA_HI, 0x0);
        outp(judas_port + GF1_REG_SELECT, SET_VOICES);
        outp(judas_port + GF1_DATA_HI, 13 | 0xc0);
        temp = inp(judas_port + GF1_IRQ_STAT);
        outp(judas_port + GF1_REG_SELECT, DMA_CONTROL);
        temp = inp(judas_port + GF1_DATA_HI);
        outp(judas_port + GF1_REG_SELECT, SAMPLE_CONTROL);
        temp = inp(judas_port + GF1_DATA_HI);
        outp(judas_port + GF1_REG_SELECT, GET_IRQV);
        temp = inp(judas_port + GF1_DATA_HI);
        for (temp = 0; temp < 32; temp++)
        {
                outp(judas_port + GF1_PAGE, temp);
                outp(judas_port + GF1_REG_SELECT, SET_CONTROL);
                outp(judas_port + GF1_DATA_HI, VOICE_STOPPED | STOP_VOICE);
                gus_delay();
                outp(judas_port + GF1_DATA_HI, VOICE_STOPPED | STOP_VOICE);
                outp(judas_port + GF1_REG_SELECT, SET_VOLUME_CONTROL);
                outp(judas_port + GF1_DATA_HI, VOLUME_STOPPED | STOP_VOLUME);
                gus_delay();
                outp(judas_port + GF1_DATA_HI, VOLUME_STOPPED | STOP_VOLUME);
                outp(judas_port + GF1_REG_SELECT, SET_VOLUME);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_START_HIGH);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_START_LOW);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_END_HIGH);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_END_LOW);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_ACC_HIGH);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
                outp(judas_port + GF1_REG_SELECT, SET_ACC_LOW);
                outpw(judas_port + GF1_DATA_LOW, 0x0);
        }
        temp = inp(judas_port + GF1_IRQ_STAT);
        outp(judas_port + GF1_REG_SELECT, DMA_CONTROL);
        temp = inp(judas_port + GF1_DATA_HI);
        outp(judas_port + GF1_REG_SELECT, SAMPLE_CONTROL);
        temp = inp(judas_port + GF1_DATA_HI);
        outp(judas_port + GF1_REG_SELECT, GET_IRQV);
        temp = inp(judas_port + GF1_DATA_HI);
        outp(judas_port + GF1_REG_SELECT, MASTER_RESET);
        outp(judas_port + GF1_DATA_HI, GF1_MASTER_RESET | GF1_OUTPUT_ENABLE | GF1_MASTER_IRQ);
}

static void gus_setupchannels(void)
{
        if (judas_mixmode & SIXTEENBIT)
        {
                if (judas_mixmode & STEREO)
                {
                        gus_setupchannel(0, 0, judas_bufferlength >> 2);
                        gus_setupchannel(1, (judas_bufferlength >> 2) + 16, judas_bufferlength >> 2);
                }
                else
                {
                        gus_setupchannel(0, 0, judas_bufferlength >> 1);
                        gus_setupchannel(1, 0, judas_bufferlength >> 1);
                }
        }
        else
        {
                if (judas_mixmode & STEREO)
                {
                        gus_setupchannel(0, 0, judas_bufferlength >> 1);
                        gus_setupchannel(1, (judas_bufferlength >> 1) + 32, judas_bufferlength >> 1);
                }
                else
                {
                        gus_setupchannel(0, 0, judas_bufferlength);
                        gus_setupchannel(1, 0, judas_bufferlength);
                }
        }
}

static void gus_setupchannel(unsigned char channel, unsigned start, unsigned length)
{
        length += start; /* Length is actually end address */
        outp(judas_port + GF1_PAGE, channel);
        outp(judas_port + GF1_REG_SELECT, SET_BALANCE);
        outp(judas_port + GF1_DATA_HI, channel * 15);
        outp(judas_port + GF1_REG_SELECT, SET_START_HIGH);
        outpw(judas_port + GF1_DATA_LOW, start >> 7);
        outp(judas_port + GF1_REG_SELECT, SET_START_LOW);
        outpw(judas_port + GF1_DATA_LOW, start << 9);
        outp(judas_port + GF1_REG_SELECT, SET_END_HIGH);
        outpw(judas_port + GF1_DATA_LOW, length >> 7);
        outp(judas_port + GF1_REG_SELECT, SET_END_LOW);
        outpw(judas_port + GF1_DATA_LOW, length << 9);
        outp(judas_port + GF1_REG_SELECT, SET_ACC_HIGH);
        outpw(judas_port + GF1_DATA_LOW, start >> 7);
        outp(judas_port + GF1_REG_SELECT, SET_ACC_LOW);
        outpw(judas_port + GF1_DATA_LOW, start << 9);
        outp(judas_port + GF1_REG_SELECT, SET_FREQUENCY);
        outpw(judas_port + GF1_DATA_LOW, (((judas_mixrate << 9) + 22050) / 44100) << 1);
        outp(judas_port + GF1_REG_SELECT, SET_VOLUME);
        outpw(judas_port + GF1_DATA_LOW, 0xf800);
}

static void gus_stopchannel(unsigned char channel)
{
        outp(judas_port + GF1_PAGE, channel);
        outp(judas_port + GF1_REG_SELECT, SET_VOLUME);
        outpw(judas_port + GF1_DATA_LOW, 0x0);
        outp(judas_port + GF1_REG_SELECT, SET_CONTROL);
        outp(judas_port + GF1_DATA_HI, VOICE_STOPPED | STOP_VOICE);
        gus_delay();
        outp(judas_port + GF1_DATA_HI, VOICE_STOPPED | STOP_VOICE);
}

int initmixer(void)
{
        int v, s, sv;

        /*
         * If this is the first time we are initializing, we must allocate the
         * lookup tables, clipbuffer & zladdbuffer and lock them as well.
         * Volume table needs to be calculated only once.
         *
         */
        if (mixer_firsttime)
        {
                int *volptr;
                CHANNEL *chptr;
                int buffersize;

                judas_cliptable = locked_malloc(65536 * sizeof(short));
                if (!judas_cliptable)
                {
                        return 0;
                }

                if (judas_device == DEV_FILE) {
                  buffersize = (int) filewriterbuffersize * 8;
                } else buffersize = 44100 / PER_SECOND * 8;
                judas_clipbuffer = locked_malloc(buffersize);
                if (!judas_clipbuffer)
                {
                        locked_free(judas_cliptable);
                        return 0;
                }
                judas_zladdbuffer = locked_malloc(buffersize);
                if (!judas_zladdbuffer)
                {
                        return 0;
                }
                judas_volumetable = locked_malloc(256 * 256 * sizeof(int) + 1024);
                if (!judas_volumetable)
                {
                        locked_free(judas_cliptable);
                        locked_free(judas_clipbuffer);
                        locked_free(judas_zladdbuffer);
                        return 0;
                }
                /*
                 * Adjust the volumetable to begin on a 1024 byte boundary;
                 * the mixing routines need this!
                 */
                judas_volumetable = (int *)((((unsigned)judas_volumetable) + 1023) & 0xfffffc00);
                volptr = &judas_volumetable[0];
                /*
                 * Note: although there is an optimized routine for zero volume,
                 * we need the zero volume table because in stereo mixing the
                 * other channel's volume could be zero.
                 */
                for (v = 0; v < 256; v++)
                {
                        for (s = 0; s < 256; s++)
                        {
                                sv = s;
                                if (sv > 127) sv -= 256;
                                sv *= v;
                                sv >>= (16 - SIGNIFICANT_BITS_16);
                                *volptr = sv;
                                volptr++;
                        }
                }
                /*
                 * The mixing routines need the address shifted, and since we
                 * don't need the pointer anymore...
                 */
                judas_volumetable = (int *)((unsigned)judas_volumetable >> 2);

                chptr = &judas_channel[0];

                /*
                 * Init all channels (no sound played, no sample, mastervolume
                 * 64)
                 */
                for (s = CHANNELS; s > 0; s--)
                {
                        chptr->voicemode = VM_OFF;
                        chptr->sample = NULL;
                        chptr->mastervol = 64;
                        chptr++;
                }
                mixer_firsttime = 0;
        }

        if (judas_mixmode & SIXTEENBIT)
        {
                short *clipptr = &judas_cliptable[0];

                for (s = 0; s < 65536; s++)
                {
                        sv = s;
                        if (sv > 32767) sv -= 65536;
                        sv <<= (16 - SIGNIFICANT_BITS_16);
                        if (sv < -32768) sv = -32768;
                        if (sv > 32767) sv = 32767;
                        *clipptr = sv;
                        clipptr++;
                }
        }
        else
        {
                unsigned char *clipptr = (unsigned char *)&judas_cliptable[0];

                for (s = 0; s < 65536; s++)
                {
                        int sv = s;
                        if (sv > 32767) sv = s - 65536;
                        sv <<= (16 - SIGNIFICANT_BITS_16);
                        if (sv < -32768) sv = -32768;
                        if (sv > 32767) sv = 32767;
                        *clipptr = (sv >> 8) + 128;
                        clipptr++;
                }
        }
        return 1;
}

static int judas_lock(void)
{
        if (judas_locked) return 1;
        if (!judas_memlock(&judas_code_lock_start, (int)&judas_code_lock_end - (int)&judas_code_lock_start)) return 0;
        if (!judas_memlock(&judas_device, sizeof judas_device)) return 0;
        if (!judas_memlock(&judas_mixroutine, sizeof judas_mixroutine)) return 0;
        if (!judas_memlock(&judas_mixersys, sizeof judas_mixersys)) return 0;
        if (!judas_memlock(&judas_initialized, sizeof judas_initialized)) return 0;
        if (!judas_memlock(&judas_ds, sizeof judas_ds)) return 0;
        if (!judas_memlock(&judas_irq, sizeof judas_irq)) return 0;
        if (!judas_memlock(&judas_int, sizeof judas_int)) return 0;
        if (!judas_memlock(&judas_dma, sizeof judas_dma)) return 0;
        if (!judas_memlock(&judas_port, sizeof judas_port)) return 0;
        if (!judas_memlock(&judas_irqcount, sizeof judas_irqcount)) return 0;
        if (!judas_memlock(&dsp_version, sizeof dsp_version)) return 0;
        if (!judas_memlock(&judas_mixrate, sizeof judas_mixrate)) return 0;
        if (!judas_memlock(&judas_mixmode, sizeof judas_mixmode)) return 0;
        if (!judas_memlock(&judas_bufferlength, sizeof judas_bufferlength)) return 0;
        if (!judas_memlock(&judas_buffermask, sizeof judas_buffermask)) return 0;
        if (!judas_memlock(&judas_clipbuffer, sizeof judas_clipbuffer)) return 0;
        if (!judas_memlock(&judas_zladdbuffer, sizeof judas_zladdbuffer)) return 0;
        if (!judas_memlock(&judas_cliptable, sizeof judas_cliptable)) return 0;
        if (!judas_memlock(&judas_volumetable, sizeof judas_volumetable)) return 0;
        if (!judas_memlock(&judas_mixpos, sizeof judas_mixpos)) return 0;
        if (!judas_memlock(&judas_samplesize, sizeof judas_samplesize)) return 0;
        if (!judas_memlock(&judas_player, sizeof judas_player)) return 0;
        if (!judas_memlock(&judas_bpmcount, sizeof judas_bpmcount)) return 0;
        if (!judas_memlock(&judas_bpmtempo, sizeof judas_bpmtempo)) return 0;
        if (!judas_memlock(&judas_channel, sizeof judas_channel * CHANNELS)) return 0;
        if (!judas_memlock(&judas_zerolevell, sizeof judas_zerolevell)) return 0;
        if (!judas_memlock(&judas_zerolevelr, sizeof judas_zerolevelr)) return 0;
        if (!judas_memlock(&judas_clipped, sizeof judas_clipped)) return 0;
        if (!judas_memlock(&filewriterbuffer, sizeof filewriterbuffer)) return 0;
        if (!judas_memlock(&filewriterbuffersize, sizeof filewriterbuffersize)) return 0;
        judas_locked = 1;
        return 1;
}

static int judas_unlock(void)
{
        if (!judas_locked) return 1;
        if (!judas_memunlock(&judas_code_lock_start, (int)&judas_code_lock_end - (int)&judas_code_lock_start)) return 0;
        if (!judas_memunlock(&judas_device, sizeof judas_device)) return 0;
        if (!judas_memunlock(&judas_mixroutine, sizeof judas_mixroutine)) return 0;
        if (!judas_memunlock(&judas_mixersys, sizeof judas_mixersys)) return 0;
        if (!judas_memunlock(&judas_initialized, sizeof judas_initialized)) return 0;
        if (!judas_memunlock(&judas_ds, sizeof judas_ds)) return 0;
        if (!judas_memunlock(&judas_irq, sizeof judas_irq)) return 0;
        if (!judas_memunlock(&judas_int, sizeof judas_int)) return 0;
        if (!judas_memunlock(&judas_dma, sizeof judas_dma)) return 0;
        if (!judas_memunlock(&judas_port, sizeof judas_port)) return 0;
        if (!judas_memunlock(&judas_irqcount, sizeof judas_irqcount)) return 0;
        if (!judas_memunlock(&dsp_version, sizeof dsp_version)) return 0;
        if (!judas_memunlock(&judas_mixrate, sizeof judas_mixrate)) return 0;
        if (!judas_memunlock(&judas_mixmode, sizeof judas_mixmode)) return 0;
        if (!judas_memunlock(&judas_bufferlength, sizeof judas_bufferlength)) return 0;
        if (!judas_memunlock(&judas_buffermask, sizeof judas_buffermask)) return 0;
        if (!judas_memunlock(&judas_clipbuffer, sizeof judas_clipbuffer)) return 0;
        if (!judas_memunlock(&judas_zladdbuffer, sizeof judas_zladdbuffer)) return 0;
        if (!judas_memunlock(&judas_cliptable, sizeof judas_cliptable)) return 0;
        if (!judas_memunlock(&judas_volumetable, sizeof judas_volumetable)) return 0;
        if (!judas_memunlock(&judas_mixpos, sizeof judas_mixpos)) return 0;
        if (!judas_memunlock(&judas_samplesize, sizeof judas_samplesize)) return 0;
        if (!judas_memunlock(&judas_player, sizeof judas_player)) return 0;
        if (!judas_memunlock(&judas_bpmcount, sizeof judas_bpmcount)) return 0;
        if (!judas_memunlock(&judas_bpmtempo, sizeof judas_bpmtempo)) return 0;
        if (!judas_memunlock(&judas_channel, sizeof judas_channel * CHANNELS)) return 0;
        if (!judas_memunlock(&judas_zerolevell, sizeof judas_zerolevell)) return 0;
        if (!judas_memunlock(&judas_zerolevelr, sizeof judas_zerolevelr)) return 0;
        if (!judas_memunlock(&judas_clipped, sizeof judas_clipped)) return 0;
        if (!judas_memunlock(&filewriterbuffer, sizeof filewriterbuffer)) return 0;
        if (!judas_memunlock(&filewriterbuffersize, sizeof filewriterbuffersize)) return 0;
        judas_locked = 0;
        return 1;
}

