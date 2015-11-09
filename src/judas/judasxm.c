/*
 * JUDAS Extended Module loading/playing
 *
 * Changes:
 * V2.03 Added support for empty patterns
 *       Moved tables only used by XMs here
 *       If both volslide params. nonzero must only slide up
 * V2.04 Corrected global volume limits. ("Gf0" fucks up no more)
 *       MUSTN'T change instrptr/sampleptr when starting a note with tone-
 *       portamento!
 * V2.05 Retrig counter must be always zeroed at the start of a tick!
 *       Plus: retrigcount & multiretrigcount aren't the same thing!!!
 * V2.06 Phase of the track (not instrument) sine vibrato rotated 180 degrees
 *       Corrected behaviour of two pattern breaks on the same row
 *       Note cut didn't work at all, now it does!
 *       Added song looping control. Moving to an already played song position
 *       is treated as looping. This may cause false alarms in some weird songs.
 *       Added support for vumeters (smp structure address provided).
 *       Added support for the more precise volume
 *       Added song rewinding/forwarding
 *
 */

#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>
#include "judas.h"
#include "judastbl.h"

#define ENV_ON 1
#define ENV_SUSTAIN 2
#define ENV_LOOP 4
#define SMP_LOOP 1
#define SMP_PINGPONGLOOP 2
#define SMP_16BIT 16
#define MAX_CHANNELS 32
#define MAX_PATTERNS 256
#define MAX_INSTRUMENTS 128
#define MAX_SAMPLES 16
#define KEYOFF 0x61

#pragma pack (push, 1);
typedef struct
{
        char id[17];
        char modname[20];
        unsigned char idbyte;
        char trackername[20];
        unsigned short version;
} XM_ID;

typedef struct
{
        unsigned char note;
        unsigned char instrument;
        unsigned char voleffect;
        unsigned char effect;
        unsigned char effectdata;
} XM_NOTE;

typedef struct
{
        unsigned headersize;
        unsigned short songlength;
        unsigned short restartpos;
        unsigned short channels;
        unsigned short patterns;
        unsigned short instruments;
        unsigned short uselinear;
        unsigned short defaulttempo;
        unsigned short defaultbpmtempo;
        unsigned char order[256];
} XM_HEADER;

typedef struct
{
        unsigned headersize;
        unsigned char packingtype;
        unsigned short rows;
        unsigned short packsize;
} XM_PATTERN;

typedef struct
{
        unsigned short x;
        unsigned short y;
} ENVELOPE;

typedef struct
{
        unsigned headersize;
        char name[22];
        unsigned char type;
        unsigned short numbersamples;
        unsigned sampleheadersize;
        unsigned char sampletable[96];
        ENVELOPE volenv[12];
        ENVELOPE panenv[12];
        unsigned char volenvpoints;
        unsigned char panenvpoints;
        unsigned char volsustpoint;
        unsigned char volloopstart;
        unsigned char volloopend;
        unsigned char pansustpoint;
        unsigned char panloopstart;
        unsigned char panloopend;
        unsigned char volenvflags;
        unsigned char panenvflags;
        unsigned char vibratotype;
        unsigned char vibratosweep;
        unsigned char vibratodepth;
        unsigned char vibratorate;
        short fadeout;
        short reserved;
        unsigned char c_volenv[325];
        unsigned char c_panenv[325];
} XM_INSTRUMENT;

typedef struct
{
        unsigned length;
        unsigned loopstart;
        unsigned looplength;
        unsigned char vol;
        signed char finetune;
        unsigned char sampleflags;
        unsigned char panning;
        signed char relativenote;
        unsigned char reserved;
        char name[22];
} XM_SAMPLE;
#pragma pack (pop);

typedef struct
{
        unsigned short rows;
        unsigned char *data;
} OUR_PATTERN;

typedef struct
{
        XM_INSTRUMENT *ip;
        XM_SAMPLE *xsp;
        SAMPLE *sp;
        unsigned char newnote;
        short realnote;
        unsigned char note;
        unsigned char instrument;
        unsigned char newinstrument;
        unsigned char voleffect;
        unsigned char effect;
        unsigned char effectdata;
        unsigned char nybble1;
        unsigned char nybble2;
        unsigned char smp;
        signed char notevol;
        signed char vol;
        signed char finetune;
        short baseperiod;
        short period;
        short targetperiod;
        unsigned char notepanning;
        unsigned char keyon;
        int volenvpos;
        int panenvpos;
        int fadeoutvalue;
        unsigned char tp;
        unsigned char tpspeed;
        unsigned char volspeedup;
        unsigned char volspeeddown;
        unsigned char portaspeedup;
        unsigned char portaspeeddown;
        unsigned char panspeedright;
        unsigned char panspeedleft;
        unsigned char vibratotype;
        unsigned char vibratospeed;
        unsigned char vibratodepth;
        unsigned char vibratophase;
        unsigned char sampleoffset;
        unsigned char glissando;
        unsigned char tremolotype;
        unsigned char tremolospeed;
        unsigned char tremolodepth;
        unsigned char tremolophase;
        unsigned char instrvibratophase;
        unsigned short instrvibratodepth;
        unsigned char multiretrigcount;
        unsigned char retrigcount;
        unsigned char retriginterval;
        unsigned char retrigvolchange;
        unsigned char patternloopline;
        unsigned char patternloopcount;
        unsigned char tremorcount;
        unsigned char tremorontime;
        unsigned char tremorofftime;
        unsigned char tremorstatus;
} TRACK;

/* Prototypes */
int judas_loadxm(char *name);
void judas_freexm(void);
void judas_playxm(int rounds);
void judas_stopxm(void);
unsigned char judas_getxmpos(void);
unsigned char judas_getxmline(void);
unsigned char judas_getxmtick(void);
unsigned char judas_getxmchannels(void);
char *judas_getxmname(void);
void judas_forwardxm(void);
void judas_rewindxm(void);
static void judas_mutechannelsxm();
static int init_xmplayer(void);
static void xmplayer_code_lock_start(void);
static void xmplayer(void);
static void startnewnote(TRACK *tptr, CHANNEL *chptr);
static void changeinstrument(TRACK *tptr);
static void extendedcommand(TRACK *tptr, CHANNEL *chptr);
static int getamigaperiod(int note, int finetune);
static void xmplayer_code_lock_end(void);

/* XM linear frequency table */
static unsigned linearfreqtable[] =
{
        535232, 534749, 534266, 533784, 533303, 532822, 532341, 531861, 531381, 530902,
        530423, 529944, 529466, 528988, 528511, 528034, 527558, 527082, 526607, 526131,
        525657, 525183, 524709, 524236, 523763, 523290, 522818, 522346, 521875, 521404,
        520934, 520464, 519994, 519525, 519057, 518588, 518121, 517653, 517186, 516720,
        516253, 515788, 515322, 514858, 514393, 513929, 513465, 513002, 512539, 512077,
        511615, 511154, 510692, 510232, 509771, 509312, 508852, 508393, 507934, 507476,
        507018, 506561, 506104, 505647, 505191, 504735, 504280, 503825, 503371, 502917,
        502463, 502010, 501557, 501104, 500652, 500201, 499749, 499298, 498848, 498398,
        497948, 497499, 497050, 496602, 496154, 495706, 495259, 494812, 494366, 493920,
        493474, 493029, 492585, 492140, 491696, 491253, 490809, 490367, 489924, 489482,
        489041, 488600, 488159, 487718, 487278, 486839, 486400, 485961, 485522, 485084,
        484647, 484210, 483773, 483336, 482900, 482465, 482029, 481595, 481160, 480726,
        480292, 479859, 479426, 478994, 478562, 478130, 477699, 477268, 476837, 476407,
        475977, 475548, 475119, 474690, 474262, 473834, 473407, 472979, 472553, 472126,
        471701, 471275, 470850, 470425, 470001, 469577, 469153, 468730, 468307, 467884,
        467462, 467041, 466619, 466198, 465778, 465358, 464938, 464518, 464099, 463681,
        463262, 462844, 462427, 462010, 461593, 461177, 460760, 460345, 459930, 459515,
        459100, 458686, 458272, 457859, 457446, 457033, 456621, 456209, 455797, 455386,
        454975, 454565, 454155, 453745, 453336, 452927, 452518, 452110, 451702, 451294,
        450887, 450481, 450074, 449668, 449262, 448857, 448452, 448048, 447644, 447240,
        446836, 446433, 446030, 445628, 445226, 444824, 444423, 444022, 443622, 443221,
        442821, 442422, 442023, 441624, 441226, 440828, 440430, 440033, 439636, 439239,
        438843, 438447, 438051, 437656, 437261, 436867, 436473, 436079, 435686, 435293,
        434900, 434508, 434116, 433724, 433333, 432942, 432551, 432161, 431771, 431382,
        430992, 430604, 430215, 429827, 429439, 429052, 428665, 428278, 427892, 427506,
        427120, 426735, 426350, 425965, 425581, 425197, 424813, 424430, 424047, 423665,
        423283, 422901, 422519, 422138, 421757, 421377, 420997, 420617, 420237, 419858,
        419479, 419101, 418723, 418345, 417968, 417591, 417214, 416838, 416462, 416086,
        415711, 415336, 414961, 414586, 414212, 413839, 413465, 413092, 412720, 412347,
        411975, 411604, 411232, 410862, 410491, 410121, 409751, 409381, 409012, 408643,
        408274, 407906, 407538, 407170, 406803, 406436, 406069, 405703, 405337, 404971,
        404606, 404241, 403876, 403512, 403148, 402784, 402421, 402058, 401695, 401333,
        400970, 400609, 400247, 399886, 399525, 399165, 398805, 398445, 398086, 397727,
        397368, 397009, 396651, 396293, 395936, 395579, 395222, 394865, 394509, 394153,
        393798, 393442, 393087, 392733, 392378, 392024, 391671, 391317, 390964, 390612,
        390259, 389907, 389556, 389204, 388853, 388502, 388152, 387802, 387452, 387102,
        386753, 386404, 386056, 385707, 385359, 385012, 384664, 384317, 383971, 383624,
        383278, 382932, 382587, 382242, 381897, 381552, 381208, 380864, 380521, 380177,
        379834, 379492, 379149, 378807, 378466, 378124, 377783, 377442, 377102, 376762,
        376422, 376082, 375743, 375404, 375065, 374727, 374389, 374051, 373714, 373377,
        373040, 372703, 372367, 372031, 371695, 371360, 371025, 370690, 370356, 370022,
        369688, 369355, 369021, 368688, 368356, 368023, 367691, 367360, 367028, 366697,
        366366, 366036, 365706, 365376, 365046, 364717, 364388, 364059, 363731, 363403,
        363075, 362747, 362420, 362093, 361766, 361440, 361114, 360788, 360463, 360137,
        359813, 359488, 359164, 358840, 358516, 358193, 357869, 357547, 357224, 356902,
        356580, 356258, 355937, 355616, 355295, 354974, 354654, 354334, 354014, 353695,
        353376, 353057, 352739, 352420, 352103, 351785, 351468, 351150, 350834, 350517,
        350201, 349885, 349569, 349254, 348939, 348624, 348310, 347995, 347682, 347368,
        347055, 346741, 346429, 346116, 345804, 345492, 345180, 344869, 344558, 344247,
        343936, 343626, 343316, 343006, 342697, 342388, 342079, 341770, 341462, 341154,
        340846, 340539, 340231, 339924, 339618, 339311, 339005, 338700, 338394, 338089,
        337784, 337479, 337175, 336870, 336566, 336263, 335959, 335656, 335354, 335051,
        334749, 334447, 334145, 333844, 333542, 333242, 332941, 332641, 332341, 332041,
        331741, 331442, 331143, 330844, 330546, 330247, 329950, 329652, 329355, 329057,
        328761, 328464, 328168, 327872, 327576, 327280, 326985, 326690, 326395, 326101,
        325807, 325513, 325219, 324926, 324633, 324340, 324047, 323755, 323463, 323171,
        322879, 322588, 322297, 322006, 321716, 321426, 321136, 320846, 320557, 320267,
        319978, 319690, 319401, 319113, 318825, 318538, 318250, 317963, 317676, 317390,
        317103, 316817, 316532, 316246, 315961, 315676, 315391, 315106, 314822, 314538,
        314254, 313971, 313688, 313405, 313122, 312839, 312557, 312275, 311994, 311712,
        311431, 311150, 310869, 310589, 310309, 310029, 309749, 309470, 309190, 308911,
        308633, 308354, 308076, 307798, 307521, 307243, 306966, 306689, 306412, 306136,
        305860, 305584, 305308, 305033, 304758, 304483, 304208, 303934, 303659, 303385,
        303112, 302838, 302565, 302292, 302019, 301747, 301475, 301203, 300931, 300660,
        300388, 300117, 299847, 299576, 299306, 299036, 298766, 298497, 298227, 297958,
        297689, 297421, 297153, 296884, 296617, 296349, 296082, 295815, 295548, 295281,
        295015, 294749, 294483, 294217, 293952, 293686, 293421, 293157, 292892, 292628,
        292364, 292100, 291837, 291574, 291311, 291048, 290785, 290523, 290261, 289999,
        289737, 289476, 289215, 288954, 288693, 288433, 288173, 287913, 287653, 287393,
        287134, 286875, 286616, 286358, 286099, 285841, 285583, 285326, 285068, 284811,
        284554, 284298, 284041, 283785, 283529, 283273, 283017, 282762, 282507, 282252,
        281998, 281743, 281489, 281235, 280981, 280728, 280475, 280222, 279969, 279716,
        279464, 279212, 278960, 278708, 278457, 278206, 277955, 277704, 277453, 277203,
        276953, 276703, 276453, 276204, 275955, 275706, 275457, 275209, 274960, 274712,
        274465, 274217, 273970, 273722, 273476, 273229, 272982, 272736, 272490, 272244,
        271999, 271753, 271508, 271263, 271018, 270774, 270530, 270286, 270042, 269798,
        269555, 269312, 269069, 268826, 268583, 268341, 268099, 267857
};

/* XM Amiga frequency table. */
unsigned amigafreqtable[] =
{
        907, 900, 894, 887, 881, 875, 868, 862, 856, 850, 844, 838, 832, 826, 820, 814,
        808, 802, 796, 791, 785, 779, 774, 768, 762, 757, 752, 746, 741, 736, 730, 725,
        720, 715, 709, 704, 699, 694, 689, 684, 678, 675, 670, 665, 660, 655, 651, 646,
        640, 636, 632, 628, 623, 619, 614, 610, 604, 601, 597, 592, 588, 584, 580, 575,
        570, 567, 563, 559, 555, 551, 547, 543, 538, 535, 532, 528, 524, 520, 516, 513,
        508, 505, 502, 498, 494, 491, 487, 484, 480, 477, 474, 470, 467, 463, 460, 457,
        453, 450, 447, 443, 440, 437, 434, 431, 428
};

/* Variables and structures */
static XM_ID xm_id;
static XM_HEADER xm_header;
static XM_PATTERN load_pattern;
static OUR_PATTERN pattern[MAX_PATTERNS];
static XM_INSTRUMENT *first_instr_ptr;
static XM_SAMPLE *xm_smp_ptr[MAX_INSTRUMENTS * MAX_SAMPLES];
static SAMPLE *smp_ptr[MAX_INSTRUMENTS * MAX_SAMPLES];
static TRACK track[MAX_CHANNELS];
static XM_NOTE emptynote = {0, 0, 0, 0, 0};
static char xm_loaded = 0;
static char xmplayer_firsttime = 1;
static unsigned short xm_oldpos;
static unsigned short xm_pos;
static unsigned short xm_line;
static unsigned short xm_ticktempo;
static unsigned short xm_tickcount;
static unsigned char xm_globalvol;
static unsigned char xm_globalvolspeedup;
static unsigned char xm_globalvolspeeddown;
static unsigned char xm_patternbreak;
static unsigned char xm_patterndelay;
static int lowperiod;
static int highperiod;
static int xm_roundcounter;
static char *xm_poshasbeenplayed;
static int xm_wind = 0;
static int xm_windingpause = 0;

/* External variables */
extern unsigned char judas_bpmtempo;
extern unsigned judas_bpmcount;
extern void (*judas_player)(void);

void judas_forwardxm(void)
{
  xm_wind = 1;
}

void judas_rewindxm(void)
{
  xm_wind = -1;
}

int judas_loadxm(char *name)
{
        int handle;
        int count;

        /* Don't waste memory if Nosound */
        judas_error = JUDAS_OK;
        if (judas_device == DEV_NOSOUND) return 1;
        judas_error = JUDAS_OUT_OF_MEMORY;
        if (xmplayer_firsttime)
        {
                if (!init_xmplayer()) return 0;
        }
        /*
         * Free previous XM
         */
        judas_freexm();
        /*
         * Try to open the file
         */
        judas_error = JUDAS_OPEN_ERROR;
        handle = judas_open(name);
        if (handle == -1)
        {
                return 0;
        }
        /*
         * Read in the identification
         */
        judas_error = JUDAS_WRONG_FORMAT;
        if (judas_read(handle, &xm_id, sizeof xm_id) != sizeof xm_id)
        {
                judas_close(handle);
                return 0;
        }
        /*
         * Check that it's an XM and the version is correct
         */
        if ((memcmp("Extended Module:", xm_id.id, 16)) || (xm_id.idbyte != 0x1a))
        {
                judas_close(handle);
                return 0;
        }
        xm_id.idbyte = 0x0; /* Serves as the songname endzero from now on */
        if (xm_id.version < 0x103)
        {
                judas_close(handle);
                return 0;
        }
        /*
         * Read in the header
         */
        judas_error = JUDAS_READ_ERROR;
        if (judas_read(handle, &xm_header, sizeof xm_header) != sizeof xm_header)
        {
                judas_close(handle);
                return 0;
        }
        /*
         * Check that there aren't too many channels
         */
        if (xm_header.channels > CHANNELS)
        {
                xm_header.channels = CHANNELS;
                judas_error = JUDAS_OUT_OF_CHANNELS;
                judas_close(handle);
                return 0;
        }

        xm_poshasbeenplayed = locked_malloc(256);
        if (!xm_poshasbeenplayed) {
                judas_close(handle);
                return 0;
        }
        /*
         * If header is longer that what we use of it, skip the extra bytes
         */
        if (judas_seek(handle, xm_header.headersize - sizeof xm_header, SEEK_CUR) == -1)
        {
                judas_close(handle);
                return 0;
        }
        /*
         * Load the patterns
         */
        for (count = 0; count < xm_header.patterns; count++)
        {
                unsigned char *packptr;
                unsigned char *unpackptr;
                unsigned packleft;
                /*
                 * Load pattern header
                 */
                judas_error = JUDAS_READ_ERROR;
                if (judas_read(handle, &load_pattern, sizeof load_pattern) != sizeof load_pattern)
                {
                        judas_freexm();
                        judas_close(handle);
                        return 0;
                }
                /*
                 * Skip extra data in pattern header
                 */
                if (judas_seek(handle, load_pattern.headersize - sizeof load_pattern, SEEK_CUR) == -1)
                {
                        judas_freexm();
                        judas_close(handle);
                        return 0;
                }
                /*
                 * Allocate memory for unpacked pattern & clear it
                 */
                pattern[count].rows = load_pattern.rows;
                pattern[count].data = locked_malloc(5 * load_pattern.rows * xm_header.channels);
                judas_error = JUDAS_OUT_OF_MEMORY;
                if (!pattern[count].data)
                {
                        judas_freexm();
                        judas_close(handle);
                        return 0;
                }
                memset(pattern[count].data, 0, 5 * load_pattern.rows * xm_header.channels);
                if (load_pattern.packsize)
                {
                        unsigned char *pack_buffer;
                        /*
                         * Allocate packeddata-buffer
                         */
                        pack_buffer = malloc(load_pattern.packsize);
                        if (!pack_buffer)
                        {
                                judas_freexm();
                                judas_close(handle);
                                return 0;
                        }
                        /*
                         * Read the packed data in
                         */
                        judas_error = JUDAS_READ_ERROR;
                        if (judas_read(handle, pack_buffer, load_pattern.packsize) != load_pattern.packsize)
                        {
                                free(pack_buffer);
                                judas_freexm();
                                judas_close(handle);
                                return 0;
                        }
                        packptr = pack_buffer;
                        unpackptr = pattern[count].data;
                        packleft = load_pattern.packsize;
                        while (packleft)
                        {
                                unsigned char control = *packptr++;
                                packleft--;
                                /* Packed? */
                                if (control & 0x80)
                                {
                                        /* Note? */
                                        if (control & 0x01)
                                        {
                                                *unpackptr++ = *packptr++;
                                                packleft--;
                                        }
                                        else unpackptr++;
                                        /* Instrument? */
                                        if (control & 0x02)
                                        {
                                                *unpackptr++ = *packptr++;
                                                packleft--;
                                        }
                                        else unpackptr++;
                                        /* Volume column? */
                                        if (control & 0x04)
                                        {
                                                *unpackptr++ = *packptr++;
                                                packleft--;
                                        }
                                        else unpackptr++;
                                        /* Effect? */
                                        if (control & 0x08)
                                        {
                                                *unpackptr++ = *packptr++;
                                                packleft--;
                                        }
                                        else unpackptr++;
                                        /* Effect parameter? */
                                        if (control & 0x10)
                                        {
                                                *unpackptr++ = *packptr++;
                                                packleft--;
                                        }
                                        else unpackptr++;
                                }
                                else
                                {
                                        *unpackptr++ = control; /* Note */
                                        *unpackptr++ = *packptr++; /* Instrument */
                                        *unpackptr++ = *packptr++; /* Volume c. */
                                        *unpackptr++ = *packptr++; /* Effect */
                                        *unpackptr++ = *packptr++; /* Effect p. */
                                        packleft -= 4;
                                }
                        }
                        free(pack_buffer);
                }
        }
        /*
         * Load instruments. This is heavy shit.
         */
        first_instr_ptr = locked_malloc(sizeof (XM_INSTRUMENT) * xm_header.instruments);
        judas_error = JUDAS_OUT_OF_MEMORY;
        if (!first_instr_ptr)
        {
                judas_freexm();
                judas_close(handle);
                return 0;
        }
        for (count = 0; count < xm_header.instruments; count++)
        {
                XM_INSTRUMENT *instr_ptr = first_instr_ptr + count;

                /*
                 * Read the part common to all instruments
                 */
                judas_error = JUDAS_READ_ERROR;
                if (judas_read(handle, instr_ptr, 29) != 29)
                {
                        judas_freexm();
                        judas_close(handle);
                        return 0;
                }
                if (instr_ptr->numbersamples)
                {
                        int s;

                        /*
                         * There are samples; read the rest in
                         */
                        if (judas_read(handle, ((char *)instr_ptr) + 29, sizeof(XM_INSTRUMENT) - 29 - 650) != sizeof(XM_INSTRUMENT) - 29 - 650)
                        {
                                judas_freexm();
                                judas_close(handle);
                                return 0;
                        }
                        /*
                         * Skip over extra shit in instr. header
                         */
                        if (judas_seek(handle, instr_ptr->headersize - (sizeof(XM_INSTRUMENT) - 650), SEEK_CUR) == -1)
                        {
                                judas_freexm();
                                judas_close(handle);
                                return 0;
                        }
                        /*
                         * Precalculate envelopes
                         */
                        if (instr_ptr->volenvflags & ENV_ON)
                        {
                                for (s = 0; s < instr_ptr->volenvpoints; s++)
                                {
                                        /* Paranoid check */
                                        if (instr_ptr->volenv[s].x > 324) instr_ptr->volenv[s].x = 324;
                                }
                                for (s = 0; s < instr_ptr->volenvpoints - 1; s++)
                                {
                                        int x, y, dx, dy;

                                        x = instr_ptr->volenv[s].x;
                                        y = instr_ptr->volenv[s].y;
                                        dx = instr_ptr->volenv[s+1].x - instr_ptr->volenv[s].x;
                                        dy = instr_ptr->volenv[s+1].y - instr_ptr->volenv[s].y;
                                        if (dx)
                                        {
                                                for (x = 0; x < dx; x++)
                                                {
                                                        instr_ptr->c_volenv[x + instr_ptr->volenv[s].x] =
                                                        y + dy * x / dx;
                                                }
                                        }
                                        if (s == instr_ptr->volenvpoints - 2)
                                        {
                                                for (x = instr_ptr->volenv[s+1].x; x < 325; x++)
                                                {
                                                        instr_ptr->c_volenv[x] = instr_ptr->volenv[s+1].y;
                                                }
                                        }
                                }
                                instr_ptr->volsustpoint = instr_ptr->volenv[instr_ptr->volsustpoint].x;
                                instr_ptr->volloopstart = instr_ptr->volenv[instr_ptr->volloopstart].x;
                                instr_ptr->volloopend = instr_ptr->volenv[instr_ptr->volloopend].x;
                                /* If zero length loop, must deactivate loop */
                                if (instr_ptr->volloopstart == instr_ptr->volloopend)
                                {
                                        instr_ptr->volenvflags &= ~ENV_LOOP;
                                }
                        }
                        if (instr_ptr->panenvflags & ENV_ON)
                        {
                                for (s = 0; s < instr_ptr->panenvpoints; s++)
                                {
                                        /* Paranoid check */
                                        if (instr_ptr->panenv[s].x > 324) instr_ptr->panenv[s].x = 324;
                                }
                                for (s = 0; s < instr_ptr->panenvpoints - 1; s++)
                                {
                                        int x, y, dx, dy;

                                        x = instr_ptr->panenv[s].x;
                                        y = instr_ptr->panenv[s].y;
                                        dx = instr_ptr->panenv[s+1].x - instr_ptr->panenv[s].x;
                                        dy = instr_ptr->panenv[s+1].y - instr_ptr->panenv[s].y;
                                        if (dx)
                                        {
                                                for (x = 0; x < dx; x++)
                                                {
                                                        instr_ptr->c_panenv[x + instr_ptr->panenv[s].x] =
                                                        y + dy * x / dx;
                                                }
                                        }
                                        if (s == instr_ptr->panenvpoints - 2)
                                        {
                                                for (x = instr_ptr->panenv[s+1].x; x < 325; x++)
                                                {
                                                        instr_ptr->c_panenv[x] = instr_ptr->panenv[s+1].y;
                                                }
                                        }
                                }
                                instr_ptr->pansustpoint = instr_ptr->panenv[instr_ptr->pansustpoint].x;
                                instr_ptr->panloopstart = instr_ptr->panenv[instr_ptr->panloopstart].x;
                                instr_ptr->panloopend = instr_ptr->panenv[instr_ptr->panloopend].x;
                                /* If zero length loop, must deactivate loop */
                                if (instr_ptr->panloopstart == instr_ptr->panloopend)
                                {
                                        instr_ptr->panenvflags &= ~ENV_LOOP;
                                }
                        }
                        /*
                         * First round, allocate xmsample & sample structures,
                         * but do not read any audio data yet
                         */
                        for (s = 0; s < instr_ptr->numbersamples; s++)
                        {
                                XM_SAMPLE *xsp;
                                SAMPLE *sp;
                                unsigned reserve;

                                /*
                                 * Allocate memory for sample header
                                 */
                                xsp = locked_malloc(sizeof(XM_SAMPLE));
                                judas_error = JUDAS_OUT_OF_MEMORY;
                                if (!xsp)
                                {
                                        judas_freexm();
                                        judas_close(handle);
                                        return 0;
                                }
                                xm_smp_ptr[count * MAX_SAMPLES + s] = xsp;

                                /*
                                 * Read in sample header
                                 */
                                judas_error = JUDAS_READ_ERROR;
                                if (judas_read(handle, xsp, sizeof(XM_SAMPLE)) != sizeof(XM_SAMPLE))
                                {
                                        judas_freexm();
                                        judas_close(handle);
                                        return 0;
                                }
                                /*
                                 * Hop over extra shit in sample header
                                 */
                                if (judas_seek(handle, instr_ptr->sampleheadersize - sizeof(XM_SAMPLE), SEEK_CUR) == -1)
                                {
                                        judas_freexm();
                                        judas_close(handle);
                                        return 0;
                                }
                                /*
                                 * If looplength is zero, loop must be de-
                                 * activated
                                 */
                                if (!xsp->looplength)
                                {
                                        xsp->sampleflags &= ~(SMP_LOOP | SMP_PINGPONGLOOP);
                                }
                                reserve = xsp->length;
                                if (xsp->sampleflags & SMP_PINGPONGLOOP) reserve += xsp->looplength;
                                /*
                                 * There might be zero length samples, don't
                                 * go to hell because of them
                                 */
                                if (reserve)
                                {
                                        /*
                                         * Reserve sample, go to hell if fail
                                         */
                                        sp = judas_allocsample(reserve);
                                        if (!sp)
                                        {
                                                judas_freexm();
                                                judas_close(handle);
                                                return 0;
                                        }
                                        smp_ptr[count * MAX_SAMPLES + s] = sp;
                                }
                        }
                        /*
                         * Second round: time for the big shit!
                         */
                        for (s = 0; s < instr_ptr->numbersamples; s++)
                        {
                                XM_SAMPLE *xsp = xm_smp_ptr[count * MAX_SAMPLES + s];
                                SAMPLE *sp = smp_ptr[count * MAX_SAMPLES + s];
                                /*
                                 * Check if this sample exists
                                 */
                                if (sp)
                                {
                                        /*
                                         * Read sample data (delta values)
                                         */

                                        judas_error = JUDAS_READ_ERROR;
                                        if (judas_read(handle, sp->start, xsp->length) != xsp->length)
                                        {
                                                judas_freexm();
                                                judas_close(handle);
                                                return 0;
                                        }

                                        sp->voicemode = VM_ON;
                                        if (xsp->sampleflags & (SMP_LOOP | SMP_PINGPONGLOOP)) sp->voicemode |= VM_LOOP;
                                        /*
                                         * Convert to normal signed data
                                         */
                                        if (xsp->sampleflags & SMP_16BIT)
                                        {
                                                int cc = xsp->length >> 1;
                                                unsigned short old = 0;
                                                unsigned short *cptr = (unsigned short *)sp->start;

                                                sp->voicemode |= VM_16BIT;
                                                while (cc--)
                                                {
                                                        *cptr += old;
                                                        old = *cptr++;
                                                }
                                        }
                                        else
                                        {
                                                int cc = xsp->length;
                                                unsigned char old = 0;
                                                unsigned char *cptr = sp->start;

                                                while (cc--)
                                                {
                                                        *cptr += old;
                                                        old = *cptr++;
                                                }
                                        }
                                        /*
                                         * Ugly shit: "unroll" pingpong-loop
                                         */
                                        if (xsp->sampleflags & SMP_PINGPONGLOOP)
                                        {
                                                sp->voicemode |= VM_LOOP;
                                                if (xsp->sampleflags & SMP_16BIT)
                                                {
                                                        int uc = xsp->looplength >> 1;
                                                        short *source = (short *)(sp->start + xsp->loopstart + xsp->looplength - 2);
                                                        short *dest = (short *)(sp->start + xsp->loopstart + xsp->looplength);

                                                        while (uc--) *dest++ = *source--;
                                                }
                                                else
                                                {
                                                        int uc = xsp->looplength;
                                                        char *source = sp->start + xsp->loopstart + xsp->looplength - 1;
                                                        char *dest = sp->start + xsp->loopstart + xsp->looplength;

                                                        while (uc--) *dest++ = *source--;
                                                }
                                                xsp->looplength <<= 1;
                                        }
                                        /*
                                         * Fix repeat & end in "our" sample structure
                                         */
                                        if (sp->voicemode & VM_LOOP)
                                        {
                                                sp->repeat = sp->start + xsp->loopstart;
                                                sp->end = sp->start + xsp->loopstart + xsp->looplength;
                                        }
                                        else sp->end = sp->start + xsp->length;
                                        /*
                                         * Interpolation-correction
                                         */
                                        judas_ipcorrect(sp);
                                        /*
                                         * We're done with this sample!
                                         */
                                }
                        }
                }
                else
                {
                        /*
                         * Header without samples; skip over extra if any
                         */
                        judas_error = JUDAS_READ_ERROR;
                        if (judas_seek(handle, instr_ptr->headersize - 29, SEEK_CUR) == -1)
                        {
                                judas_freexm();
                                judas_close(handle);
                                return 0;
                        }
                }
        }
        judas_close(handle);
        judas_error = JUDAS_OK;
        xm_loaded = 1;
        return 1;
}

void judas_freexm(void)
{
        int count;

        if (xmplayer_firsttime)
        {
                if (!init_xmplayer()) return;
        }
        judas_stopxm();
        xm_loaded = 0;
        for (count = 0; count < MAX_PATTERNS; count++)
        {
                if (pattern[count].data)
                {
                        locked_free(pattern[count].data);
                        pattern[count].data = NULL;
                }
                pattern[count].rows = 64;
        }
        if (first_instr_ptr)
        {
                locked_free(first_instr_ptr);
                first_instr_ptr = NULL;
        }
        for (count = 0; count < MAX_SAMPLES * MAX_INSTRUMENTS; count++)
        {
                if (xm_smp_ptr[count])
                {
                        locked_free(xm_smp_ptr[count]);
                        xm_smp_ptr[count] = NULL;
                }
                if (smp_ptr[count])
                {
                        judas_freesample(smp_ptr[count]);
                        smp_ptr[count] = NULL;
                }
        }
}

unsigned char judas_getxmpos(void)
{
        return xm_pos;
}

unsigned char judas_getxmline(void)
{
        return xm_line;
}

unsigned char judas_getxmtick(void)
{
        return xm_tickcount;
}

unsigned char judas_getxmchannels(void)
{
        return xm_header.channels;
}

char *judas_getxmname(void)
{
        if (xm_loaded) return xm_id.modname;
        else return NULL;
}

static int init_xmplayer(void)
{
        int count;

        xm_header.channels = 0;
        for (count = 0; count < MAX_PATTERNS; count++)
        {
                pattern[count].data = NULL;
        }
        first_instr_ptr = NULL;
        for (count = 0; count < MAX_SAMPLES * MAX_INSTRUMENTS; count++)
        {
                xm_smp_ptr[count] = NULL;
                smp_ptr[count] = NULL;
        }
        if (!judas_locktables()) return 0;
        if (!judas_memlock(&xmplayer_code_lock_start, (int)&xmplayer_code_lock_end - (int)&xmplayer_code_lock_start)) return 0;
        if (!judas_memlock(&linearfreqtable, sizeof linearfreqtable)) return 0;
        if (!judas_memlock(&amigafreqtable, sizeof amigafreqtable)) return 0;
        if (!judas_memlock(&xm_header, sizeof xm_header)) return 0;
        if (!judas_memlock(&pattern[0], sizeof pattern)) return 0;
        if (!judas_memlock(&first_instr_ptr, sizeof first_instr_ptr)) return 0;
        if (!judas_memlock(&xm_smp_ptr[0], sizeof xm_smp_ptr)) return 0;
        if (!judas_memlock(&smp_ptr[0], sizeof smp_ptr)) return 0;
        if (!judas_memlock(&track[0], sizeof track)) return 0;
        if (!judas_memlock(&xm_loaded, sizeof xm_loaded)) return 0;
        if (!judas_memlock(&xm_poshasbeenplayed, sizeof xm_poshasbeenplayed)) return 0; // **
        if (!judas_memlock(&xm_oldpos, sizeof xm_oldpos)) return 0; // **
        if (!judas_memlock(&xm_pos, sizeof xm_pos)) return 0;
        if (!judas_memlock(&xm_line, sizeof xm_line)) return 0;
        if (!judas_memlock(&xm_ticktempo, sizeof xm_ticktempo)) return 0;
        if (!judas_memlock(&xm_tickcount, sizeof xm_tickcount)) return 0;
        if (!judas_memlock(&xm_patternbreak, sizeof xm_patternbreak)) return 0;
        if (!judas_memlock(&xm_patterndelay, sizeof xm_patterndelay)) return 0;
        if (!judas_memlock(&xm_globalvol, sizeof xm_globalvol)) return 0;
        if (!judas_memlock(&xm_globalvolspeedup, sizeof xm_globalvolspeedup)) return 0;
        if (!judas_memlock(&xm_globalvolspeeddown, sizeof xm_globalvolspeeddown)) return 0;
        if (!judas_memlock(&lowperiod, sizeof lowperiod)) return 0;
        if (!judas_memlock(&highperiod, sizeof highperiod)) return 0;
        if (!judas_memlock(&emptynote, sizeof emptynote)) return 0;
        if (!judas_memlock(&xm_roundcounter, sizeof xm_roundcounter)) return 0;
        if (!judas_memlock(&xm_wind, sizeof xm_wind)) return 0;
        if (!judas_memlock(&xm_windingpause, sizeof xm_windingpause)) return 0;
        xmplayer_firsttime = 0;
        return 1;
}

static void xmplayer_code_lock_start(void)
{
}

void judas_mutechannelsxm()
{
  int count;
  TRACK *tptr = &track[0];
  CHANNEL *chptr = &judas_channel[0];
  for (count = xm_header.channels; count > 0; count--)
  {
          chptr->smp = NULL;
          chptr->voicemode = VM_OFF;
          tptr->ip = first_instr_ptr;
          tptr->xsp = NULL;
          tptr->sp = NULL;
          tptr->instrument = 0;
          tptr->smp = 0;
          tptr->notevol = 0;
          tptr->vol = 0;
          tptr->effect = 0;
          tptr->effectdata = 0;
          tptr->nybble1 = 0;
          tptr->nybble2 = 0;
          tptr->voleffect = 0;
          tptr->volenvpos = 0;
          tptr->panenvpos = 0;
          tptr->keyon = 0;
          tptr->tp = 0;
          tptr->tpspeed = 0;
          tptr->volspeedup = 0;
          tptr->volspeeddown = 0;
          tptr->glissando = 0;
          tptr->retriginterval = 0;
          tptr->retrigvolchange = 0;
          tptr->patternloopline = 0;
          tptr->patternloopcount = 0;
          tptr->sampleoffset = 0;
          tptr->tremolotype = 0;
          tptr->vibratotype = 0;
          chptr++;
          tptr++;
  }
}

void judas_playxm(int rounds)
{
        int count;

        if (!xm_loaded) return;
        judas_player = NULL;
        xm_roundcounter = rounds;
        xm_wind = 0;
        xm_pos = 0;
        xm_line = 0;
        xm_tickcount = 0;
        xm_ticktempo = xm_header.defaulttempo;
        xm_globalvol = 64;
        xm_patterndelay = 0;
        judas_bpmcount = 0;
        judas_bpmtempo = xm_header.defaultbpmtempo;
        for (count = 1; count < 256; count++)
        {
          xm_poshasbeenplayed[count] = 0;
        }
        xm_poshasbeenplayed[0] = 1;
        judas_mutechannelsxm();
        if (xm_header.uselinear)
        {
                lowperiod = 7743;
                highperiod = 64;
        }
        else
        {
                lowperiod = 29024;
                highperiod = 28;
        }
        judas_player = &xmplayer;
}

void judas_stopxm(void)
{
        int count;
        CHANNEL *chptr = &judas_channel[0];

        if (xm_header.channels > CHANNELS) xm_header.channels = CHANNELS;
        if (!xm_loaded) return;
        judas_player = NULL;
        for (count = xm_header.channels; count > 0; count--)
        {
                chptr->smp = NULL;
                chptr->voicemode = VM_OFF;
                chptr++;
        }
}

void xmplayer(void)
{
        TRACK *tptr = &track[0];
        CHANNEL *chptr = &judas_channel[0];
        OUR_PATTERN *pattptr = &pattern[xm_header.order[xm_pos]];
        int count;

        /*
         * Song winding
         */
        if (xm_wind > 0) {
          xm_wind = 0;
          if (xm_pos < xm_header.songlength-1) {
            int count;
            xm_pos++;
            xm_oldpos = xm_pos;
            xm_line = 0;
            xm_tickcount = 0;
            xm_patterndelay = 0;
            for (count = 0; count < 256; count++)
            {
              xm_poshasbeenplayed[count] = 0;
            }
            xm_poshasbeenplayed[xm_pos] = 1;
            judas_mutechannelsxm();
            xm_windingpause = 1;
            return;
          }
        } else
        if (xm_wind < 0) {
          xm_wind = 0;
          if (xm_pos > 0) {
            int count;
            xm_pos--;
            xm_oldpos = xm_pos;
            xm_line = 0;
            xm_tickcount = 0;
            xm_patterndelay = 0;
            for (count = 0; count < 256; count++)
            {
              xm_poshasbeenplayed[count] = 0;
            }
            xm_poshasbeenplayed[xm_pos] = 1;
            judas_mutechannelsxm();
            xm_windingpause = 1;
            return;
          }
        }
        if (xm_windingpause) {
          xm_windingpause = 0;
          return;
        }

        /*
         * Set new notes or do something else?
         */
        if ((!xm_tickcount) && (!xm_patterndelay))
        {
                XM_NOTE *noteptr = (XM_NOTE *)pattptr->data;

                if (!noteptr) noteptr = &emptynote;
                else noteptr += xm_header.channels * xm_line;

                xm_patternbreak = 0;
                xm_oldpos = xm_pos;
                for (count = xm_header.channels; count > 0; count--)
                {
                        tptr->newnote = 0;
                        tptr->retrigcount = 0;

                        /*
                         * Get note (if any)
                         */
                        if (noteptr->note)
                        {
                                tptr->note = noteptr->note - 1;
                                tptr->newnote = 1;
                        }
                        /*
                         * Get effect, effect data etc.
                         */
                        tptr->voleffect = noteptr->voleffect;
                        tptr->effect = noteptr->effect;
                        tptr->effectdata = noteptr->effectdata;
                        tptr->nybble1 = noteptr->effectdata >> 4;
                        tptr->nybble2 = noteptr->effectdata & 0xf;
                        tptr->newinstrument = noteptr->instrument;

                        /*
                         * Set sampleoffset here
                         */
                        if (tptr->effect == 0x9)
                        {
                                if (tptr->effectdata) tptr->sampleoffset = tptr->effectdata;
                        }

                        /*
                         * Start new note if there is one; but check there
                         * isn't notedelay (toneportamento is handled by
                         * startnewnote()!)
                         * Also restart shit if an instrument number!
                         */
                        if ((tptr->effect != 0xe) || (tptr->nybble1 != 0xd) || (tptr->nybble2 == 0))
                        {
                                if (tptr->newnote) startnewnote(tptr, chptr);
                                if (tptr->newinstrument) changeinstrument(tptr);
                        }

                        /*
                         * Reset period if not vibrato or toneportamento
                         */
                        if (((tptr->effect < 0x3) || (tptr->effect > 0x6)) && (tptr->voleffect != 0xb) && (tptr->voleffect != 0xf))
                        {
                                tptr->period = tptr->baseperiod;
                        }
                        /*
                         * Reset volume if not tremolo / tremor
                         */
                        if ((tptr->effect != 0x7) && (tptr->effect != 0x1d))
                        {
                                tptr->vol = tptr->notevol;
                        }
                        /*
                         * Now check some effects: volume column done first.
                         */
                        switch (tptr->voleffect >> 4)
                        {
                                case 0x0:
                                break;

                                /* Set volume */
                                case 0x1:
                                case 0x2:
                                case 0x3:
                                case 0x4:
                                case 0x5:
                                /* Applies only if there isn't notedelay */
                                if ((tptr->effect != 0xe) || (tptr->nybble1 != 0xd) || (tptr->nybble2 == 0))
                                {
                                        tptr->notevol = tptr->voleffect - 0x10;
                                        if (tptr->notevol > 64) tptr->notevol = 64;
                                        tptr->vol = tptr->notevol;
                                }
                                break;

                                /* Fine volslide down */
                                case 0x8:
                                tptr->notevol -= tptr->voleffect & 0xf;
                                if (tptr->notevol < 0) tptr->notevol = 0;
                                tptr->vol = tptr->notevol;
                                break;

                                /* Fine volslide up */
                                case 0x9:
                                tptr->notevol += tptr->voleffect & 0xf;
                                if (tptr->notevol > 64) tptr->notevol = 64;
                                tptr->vol = tptr->notevol;
                                break;

                                /* Set vibrato speed */
                                case 0xa:
                                if (tptr->voleffect & 0xf)
                                {
                                        tptr->vibratospeed = tptr->voleffect & 0xf;
                                }
                                break;

                                /* Vibrato */
                                case 0xb:
                                if (tptr->voleffect & 0xf)
                                {
                                        tptr->vibratodepth = tptr->voleffect & 0xf;
                                }
                                break;

                                /* Set panning */
                                case 0xc:
                                /* Applies only if there isn't notedelay */
                                if ((tptr->effect != 0xe) || (tptr->nybble1 != 0xd) || (tptr->nybble2 == 0))
                                {
                                        tptr->notepanning = (tptr->voleffect & 0xf) << 4 | (tptr->voleffect & 0xf);
                                }
                                break;

                                /* Toneportamento */
                                case 0xf:
                                if (tptr->voleffect & 0xf)
                                {
                                        tptr->tpspeed = (tptr->voleffect & 0xf) << 4;
                                }
                                break;
                        }

                        /*
                         * Then the regular effects
                         */
                        switch (tptr->effect)
                        {
                                case 0x0:
                                break;

                                /* Set portamento speed up */
                                case 0x1:
                                if (tptr->effectdata) tptr->portaspeedup = tptr->effectdata;
                                break;

                                /* Set portamento speed down */
                                case 0x2:
                                if (tptr->effectdata) tptr->portaspeeddown = tptr->effectdata;
                                break;

                                /* Set TP. speed */
                                case 0x3:
                                if (tptr->effectdata) tptr->tpspeed = tptr->effectdata;
                                break;

                                /* Set vibrato */
                                case 0x4:
                                if (tptr->nybble1) tptr->vibratospeed = tptr->nybble1;
                                if (tptr->nybble2) tptr->vibratodepth = tptr->nybble2;
                                break;

                                /* Set tremolo */
                                case 0x7:
                                if (tptr->nybble1) tptr->tremolospeed = tptr->nybble1;
                                if (tptr->nybble2) tptr->tremolodepth = tptr->nybble2;
                                break;

                                /* Set Panning */
                                case 0x8:
                                tptr->notepanning = tptr->effectdata;
                                break;

                                /* Volume slide speed set */
                                case 0x5:
                                case 0x6:
                                case 0xa:
                                if (tptr->effectdata)
                                {
                                        tptr->volspeedup = tptr->nybble1;
                                        tptr->volspeeddown = tptr->nybble2;
                                }
                                break;

                                /* Pos. jump */
                                case 0xb:
                                xm_line = pattptr->rows - 1;
                                xm_pos = tptr->effectdata - 1;
                                break;

                                /* Set volume */
                                case 0xc:
                                tptr->notevol = tptr->effectdata;
                                if (tptr->notevol < 0) tptr->notevol = 0;
                                if (tptr->notevol > 64) tptr->notevol = 64;
                                tptr->vol = tptr->notevol;
                                break;

                                /* Pattern break */
                                case 0xd:
                                xm_line = tptr->nybble1 * 10 + tptr->nybble2 - 1;
                                if (!xm_patternbreak)
                                {
                                        xm_patternbreak = 1;
                                        xm_pos++;
                                }
                                break;

                                /* Extended command */
                                case 0xe:
                                extendedcommand(tptr, chptr);
                                break;

                                /* Set tempo */
                                case 0xf:
                                if (!tptr->effectdata) {
                                  if (xm_roundcounter != 1)
                                  {                  
                                    judas_stopxm();
                                    judas_playxm(xm_roundcounter);
                                    if (xm_roundcounter) xm_roundcounter--;
                                    return;
                                  } else {
                                    judas_stopxm();
                                    return;
                                  }
                                }
                                if (tptr->effectdata < 32) xm_ticktempo = tptr->effectdata;
                                else judas_bpmtempo = tptr->effectdata;
                                break;

                                /* Global volume */
                                case 0x10:
                                xm_globalvol = tptr->effectdata;
                                if (xm_globalvol > 64) xm_globalvol = 64;
                                break;

                                /* Global volume slide */
                                case 0x11:
                                if (tptr->effectdata)
                                {
                                        xm_globalvolspeedup = tptr->nybble1;
                                        xm_globalvolspeeddown = tptr->nybble2;
                                }
                                break;

                                /* Keyoff */
                                case 0x14:
                                tptr->keyon = 0;
                                if (!(tptr->ip->volenvflags & ENV_ON))
                                {
                                        tptr->notevol = 0;
                                        tptr->vol = 0;
                                }
                                break;

                                /* Set envpos */
                                case 0x15:
                                tptr->volenvpos = tptr->effectdata;
                                tptr->panenvpos = tptr->effectdata;
                                break;

                                /* Panning slide */
                                case 0x19:
                                if (tptr->effectdata)
                                {
                                        tptr->panspeedright = tptr->nybble1;
                                        tptr->panspeedleft = tptr->nybble2;
                                }
                                break;

                                /* Multi retrig */
                                case 0x1b:
                                if (tptr->nybble1) tptr->retrigvolchange = tptr->nybble1;
                                if (tptr->nybble2) tptr->retriginterval = tptr->nybble2;
                                if (tptr->multiretrigcount >= tptr->retriginterval)
                                {
                                        startnewnote(tptr, chptr);
                                        tptr->multiretrigcount = 0;
                                        /* Now modify volume */
                                        if (!retrigmultable[tptr->retrigvolchange])
                                        {
                                                tptr->notevol += retrigaddtable[tptr->retrigvolchange];
                                                if (tptr->notevol < 0) tptr->notevol = 0;
                                                if (tptr->notevol > 64) tptr->notevol = 64;
                                                tptr->vol = tptr->notevol;
                                        }
                                        else
                                        {
                                                tptr->notevol = (tptr->notevol * retrigmultable[tptr->retrigvolchange]) >> 4;
                                                if (tptr->notevol > 64) tptr->notevol = 64;
                                                tptr->vol = tptr->notevol;
                                        }
                                }
                                tptr->multiretrigcount++;
                                break;

                                /* Tremor */
                                case 0x1d:
                                if (tptr->effectdata)
                                {
                                        tptr->tremorontime = tptr->nybble1;
                                        tptr->tremorofftime = tptr->nybble2;
                                }
                                break;

                                /* Extra fine portamento */
                                case 0x21:
                                switch (tptr->nybble1)
                                {
                                        case 1:
                                        if (tptr->nybble2) tptr->portaspeedup = tptr->nybble2;
                                        tptr->baseperiod -= tptr->portaspeedup;
                                        if (tptr->baseperiod < highperiod) tptr->baseperiod = highperiod;
                                        tptr->period = tptr->baseperiod;
                                        break;

                                        case 2:
                                        if (tptr->nybble2) tptr->portaspeeddown = tptr->nybble2;
                                        tptr->baseperiod += tptr->portaspeeddown;
                                        if (tptr->baseperiod > lowperiod) tptr->baseperiod = lowperiod;
                                        tptr->period = tptr->baseperiod;
                                        break;
                                }
                                break;
                        }
                        if (noteptr != &emptynote) noteptr++;
                        chptr++;
                        tptr++;
                }
        }
        if (xm_tickcount)
        {
                /*
                 * If tick isn't 0, update "continuous" effects
                 */
                for (count = xm_header.channels; count > 0; count--)
                {
                        /*
                         * Volume column
                         */
                        switch (tptr->voleffect >> 4)
                        {
                                case 0x0:
                                break;

                                /* Volslide down */
                                case 0x6:
                                tptr->notevol -= tptr->voleffect & 0xf;
                                if (tptr->notevol < 0) tptr->notevol = 0;
                                tptr->vol = tptr->notevol;
                                break;

                                /* Volslide up */
                                case 0x7:
                                tptr->notevol += tptr->voleffect & 0xf;
                                if (tptr->notevol > 64) tptr->notevol = 64;
                                tptr->vol = tptr->notevol;
                                break;

                                /* Vibrato */
                                case 0xb:
                                tptr->vibratophase += tptr->vibratospeed * 4;
                                tptr->period = tptr->baseperiod + ((vibratotable[vibratotypetable[tptr->vibratotype & 3]][tptr->vibratophase] * tptr->vibratodepth) >> 3);
                                if (tptr->period < highperiod) tptr->period = highperiod;
                                if (tptr->period > lowperiod) tptr->period = lowperiod;
                                break;

                                /* Panslide left */
                                case 0xd:
                                {
                                        int new = tptr->notepanning;
                                        new -= tptr->voleffect & 0xf;
                                        if (new < 0) new = 0;
                                        tptr->notepanning = new;
                                }
                                break;

                                /* Panslide right */
                                case 0xe:
                                {
                                        int new = tptr->notepanning;
                                        new += tptr->voleffect & 0xf;
                                        if (new > 255) new = 255;
                                        tptr->notepanning = new;
                                }
                                break;

                                /* Toneportamento */
                                case 0xf:
                                if (tptr->tp)
                                {
                                        if (tptr->baseperiod < tptr->targetperiod)
                                        {
                                                tptr->baseperiod += tptr->tpspeed * 4;
                                                if (tptr->baseperiod >= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        if (tptr->baseperiod > tptr->targetperiod)
                                        {
                                                tptr->baseperiod -= tptr->tpspeed * 4;
                                                if (tptr->baseperiod <= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        tptr->period = tptr->baseperiod;
                                        if (tptr->glissando)
                                        {
                                                if (xm_header.uselinear)
                                                {
                                                        tptr->period += tptr->finetune / 2;
                                                        tptr->period += 32;
                                                        tptr->period &= 0xffc0;
                                                        tptr->period -= tptr->finetune / 2;
                                                }
                                                else
                                                {
                                                        int offset = 0x7fffffff;
                                                        int sc;
                                                        short bestperiod;

                                                        for (sc = 0; sc <= 118; sc++)
                                                        {
                                                                int newoffset = abs(tptr->period - getamigaperiod(sc, tptr->finetune));

                                                                if (newoffset < offset)
                                                                {
                                                                        bestperiod = getamigaperiod(sc, tptr->finetune);
                                                                        offset = newoffset;
                                                                }
                                                        }
                                                        tptr->period = bestperiod;
                                                }
                                        }
                                }
                                break;
                        }


                        /*
                         * Regular effects
                         */
                        switch (tptr->effect)
                        {
                                /* Arpeggio */
                                case 0x0:
                                if (tptr->effectdata)
                                {
                                        char phase = xm_tickcount % 3;
                                        switch (phase)
                                        {
                                                case 0:
                                                tptr->period = tptr->baseperiod;
                                                break;

                                                case 1:
                                                if (xm_header.uselinear) tptr->period = tptr->baseperiod - tptr->nybble1 * 64;
                                                else tptr->period = getamigaperiod(tptr->realnote + tptr->nybble1, tptr->finetune);
                                                if (tptr->period < highperiod) tptr->period = highperiod;
                                                break;

                                                case 2:
                                                if (xm_header.uselinear) tptr->period = tptr->baseperiod - tptr->nybble2 * 64;
                                                else tptr->period = getamigaperiod(tptr->realnote + tptr->nybble2, tptr->finetune);
                                                if (tptr->period < highperiod) tptr->period = highperiod;
                                                break;
                                        }
                                }
                                break;

                                /* Portamento up */
                                case 0x1:
                                tptr->baseperiod -= tptr->portaspeedup * 4;
                                if (tptr->baseperiod < highperiod) tptr->baseperiod = highperiod;
                                tptr->period = tptr->baseperiod;
                                break;

                                /* Portamento down */
                                case 0x2:
                                tptr->baseperiod += tptr->portaspeeddown * 4;
                                if (tptr->baseperiod > lowperiod) tptr->baseperiod = lowperiod;
                                tptr->period = tptr->baseperiod;
                                break;

                                /* Toneportamento */
                                case 0x3:
                                if (tptr->tp)
                                {
                                        if (tptr->baseperiod < tptr->targetperiod)
                                        {
                                                tptr->baseperiod += tptr->tpspeed * 4;
                                                if (tptr->baseperiod >= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        if (tptr->baseperiod > tptr->targetperiod)
                                        {
                                                tptr->baseperiod -= tptr->tpspeed * 4;
                                                if (tptr->baseperiod <= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        tptr->period = tptr->baseperiod;
                                        if (tptr->glissando)
                                        {
                                                if (xm_header.uselinear)
                                                {
                                                        tptr->period += tptr->finetune / 2;
                                                        tptr->period += 32;
                                                        tptr->period &= 0xffc0;
                                                        tptr->period -= tptr->finetune / 2;
                                                }
                                                else
                                                {
                                                        int offset = 0x7fffffff;
                                                        int sc;
                                                        short bestperiod;

                                                        for (sc = 0; sc <= 118; sc++)
                                                        {
                                                                int newoffset = abs(tptr->period - getamigaperiod(sc, tptr->finetune));

                                                                if (newoffset < offset)
                                                                {
                                                                        bestperiod = getamigaperiod(sc, tptr->finetune);
                                                                        offset = newoffset;
                                                                }
                                                        }
                                                        tptr->period = bestperiod;
                                                }
                                        }
                                }
                                break;

                                /* Vibrato */
                                case 0x4:
                                tptr->vibratophase += tptr->vibratospeed * 4;
                                tptr->period = tptr->baseperiod + ((vibratotable[vibratotypetable[tptr->vibratotype & 3]][tptr->vibratophase] * tptr->vibratodepth) >> 3);
                                if (tptr->period < highperiod) tptr->period = highperiod;
                                if (tptr->period > lowperiod) tptr->period = lowperiod;
                                break;

                                /* Toneportamento + volslide */
                                case 0x5:
                                if (tptr->tp)
                                {
                                        if (tptr->baseperiod < tptr->targetperiod)
                                        {
                                                tptr->baseperiod += tptr->tpspeed * 4;
                                                if (tptr->baseperiod >= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        if (tptr->baseperiod > tptr->targetperiod)
                                        {
                                                tptr->baseperiod -= tptr->tpspeed * 4;
                                                if (tptr->baseperiod <= tptr->targetperiod)
                                                {
                                                        tptr->baseperiod = tptr->targetperiod;
                                                        tptr->tp = 0;
                                                }
                                        }
                                        tptr->period = tptr->baseperiod;
                                        if (tptr->glissando)
                                        {
                                                if (xm_header.uselinear)
                                                {
                                                        tptr->period += tptr->finetune / 2;
                                                        tptr->period += 32;
                                                        tptr->period &= 0xffc0;
                                                        tptr->period -= tptr->finetune / 2;
                                                }
                                                else
                                                {
                                                        int offset = 0x7fffffff;
                                                        int sc;
                                                        short bestperiod;

                                                        for (sc = 0; sc <= 118; sc++)
                                                        {
                                                                int newoffset = abs(tptr->period - getamigaperiod(sc, tptr->finetune));

                                                                if (newoffset < offset)
                                                                {
                                                                        bestperiod = getamigaperiod(sc, tptr->finetune);
                                                                        offset = newoffset;
                                                                }
                                                        }
                                                        tptr->period = bestperiod;
                                                }
                                        }
                                }
                                if (tptr->volspeedup)
                                {
                                        tptr->notevol += tptr->volspeedup;
                                        if (tptr->notevol > 64) tptr->notevol = 64;
                                }
                                else
                                {
                                        tptr->notevol -= tptr->volspeeddown;
                                        if (tptr->notevol < 0) tptr->notevol = 0;
                                }
                                tptr->vol = tptr->notevol;
                                break;

                                /* Vibrato + volslide */
                                case 0x6:
                                tptr->vibratophase += tptr->vibratospeed * 4;
                                tptr->period = tptr->baseperiod + ((vibratotable[vibratotypetable[tptr->vibratotype & 3]][tptr->vibratophase] * tptr->vibratodepth) >> 3);
                                if (tptr->period < highperiod) tptr->period = highperiod;
                                if (tptr->period > lowperiod) tptr->period = lowperiod;
                                if (tptr->volspeedup)
                                {
                                        tptr->notevol += tptr->volspeedup;
                                        if (tptr->notevol > 64) tptr->notevol = 64;
                                }
                                else
                                {
                                        tptr->notevol -= tptr->volspeeddown;
                                        if (tptr->notevol < 0) tptr->notevol = 0;
                                }
                                tptr->vol = tptr->notevol;
                                break;

                                /* Tremolo */
                                case 0x7:
                                tptr->tremolophase += tptr->tremolospeed * 4;
                                tptr->vol = tptr->notevol + ((vibratotable[tptr->tremolotype & 3][tptr->tremolophase] * tptr->tremolodepth) >> 4);
                                if (tptr->vol < 0) tptr->vol = 0;
                                if (tptr->vol > 64) tptr->vol = 64;
                                break;

                                /* Volume Slide */
                                case 0xa:
                                if (tptr->volspeedup)
                                {
                                        tptr->notevol += tptr->volspeedup;
                                        if (tptr->notevol > 64) tptr->notevol = 64;
                                }
                                else
                                {
                                        tptr->notevol -= tptr->volspeeddown;
                                        if (tptr->notevol < 0) tptr->notevol = 0;
                                }
                                tptr->vol = tptr->notevol;
                                break;

                                /* Extended command */
                                case 0xe:
                                extendedcommand(tptr, chptr);
                                break;

                                /* Global volume slide */
                                case 0x11:
                                if (xm_globalvolspeedup)
                                {
                                        xm_globalvol += xm_globalvolspeedup;
                                        if (xm_globalvol > 64) xm_globalvol = 64;
                                }
                                else
                                {
                                        if (xm_globalvol > xm_globalvolspeeddown)
                                        xm_globalvol -= xm_globalvolspeeddown;
                                        else xm_globalvol = 0;
                                }
                                break;

                                /* Panning slide */
                                case 0x19:
                                {
                                        int new = tptr->notepanning;

                                        new += tptr->panspeedright;
                                        if (new < 0) new = 0;
                                        new -= tptr->panspeedleft;
                                        if (new > 255) new = 255;
                                        tptr->notepanning = new;
                                }
                                break;

                                /* Multi retrig */
                                case 0x1b:
                                if (tptr->multiretrigcount >= tptr->retriginterval)
                                {
                                        startnewnote(tptr, chptr);
                                        tptr->multiretrigcount = 0;
                                        /* Now modify volume */
                                        if (!retrigmultable[tptr->retrigvolchange])
                                        {
                                                tptr->notevol += retrigaddtable[tptr->retrigvolchange];
                                                if (tptr->notevol < 0) tptr->notevol = 0;
                                                if (tptr->notevol > 64) tptr->notevol = 64;
                                                tptr->vol = tptr->notevol;
                                        }
                                        else
                                        {
                                                tptr->notevol = (tptr->notevol * retrigmultable[tptr->retrigvolchange]) >> 4;
                                                if (tptr->notevol > 64) tptr->notevol = 64;
                                                tptr->vol = tptr->notevol;
                                        }
                                }
                                tptr->multiretrigcount++;
                                break;

                                /* Tremor */
                                case 0x1d:
                                if (!tptr->tremorcount)
                                {
                                        tptr->tremorstatus ^= 1;
                                        if (tptr->tremorstatus) tptr->tremorcount = tptr->tremorontime + 1;
                                        else tptr->tremorcount = tptr->tremorofftime + 1;
                                }
                                if (tptr->tremorstatus) tptr->vol = tptr->notevol;
                                else tptr->vol = 0;
                                tptr->tremorcount--;
                                break;
                        }
                        chptr++;
                        tptr++;
                }
        }

        /*
         * Update envelopes and set the frequency, volume & panning of
         * the channel (unlike in mods, these depend on many things)
         */
        tptr = &track[0];
        chptr = &judas_channel[0];
        for (count = xm_header.channels; count > 0; count--)
        {
                XM_INSTRUMENT *ip = tptr->ip;
                short finalperiod = tptr->period;

                /*
                 * Update fadeout (must be done BEFORE before calculating
                 * the final volume -- checked visually by recording FT2's
                 * sound output to tape and then sampling it!)
                 */
                if (!tptr->keyon)
                {
                        tptr->fadeoutvalue -= ip->fadeout;
                        if (tptr->fadeoutvalue < 0) tptr->fadeoutvalue = 0;
                }

                /*
                 * Calculate final channel values
                 */
                {
                        int finalpan;
                        int finalvol;
                        if (ip->volenvflags & ENV_ON) finalvol = (tptr->vol * xm_globalvol * ip->c_volenv[tptr->volenvpos] * (tptr->fadeoutvalue >> 9)) >> 10;
                        else finalvol = (tptr->vol * xm_globalvol * (tptr->fadeoutvalue >> 9)) >> 4;
                        if (ip->panenvflags & ENV_ON) finalpan = tptr->notepanning + (ip->c_panenv[tptr->panenvpos] - 32) * (128 - abs(tptr->notepanning - 128)) / 32;
                        else finalpan = tptr->notepanning;
                        if (finalpan < 0) finalpan = 0;
                        if (finalpan > 255) finalpan = 255;
                        if (finalvol > 64*256) finalvol = 64*256;
                        chptr->vol = finalvol;
                        chptr->panning = finalpan;
                }
                /*
                 * Update instrument vibrato
                 */
                if ((ip->vibratodepth) && (ip->vibratorate))
                {
                        if (ip->vibratosweep)
                        {
                                if (tptr->keyon)
                                {
                                        tptr->instrvibratodepth += (ip->vibratodepth << 8) / ip->vibratosweep;
                                        if (tptr->instrvibratodepth > (ip->vibratodepth << 8)) tptr->instrvibratodepth = ip->vibratodepth << 8;
                                }
                        }
                        else tptr->instrvibratodepth = ip->vibratodepth << 8;
                        tptr->instrvibratophase += ip->vibratorate;
                        finalperiod += (vibratotable[ip->vibratotype & 3][tptr->instrvibratophase] * tptr->instrvibratodepth) >> 14;
                }
                if (finalperiod < (highperiod - 15)) finalperiod = highperiod - 15;
                if (finalperiod > (lowperiod + 15)) finalperiod = lowperiod + 15;
                if (xm_header.uselinear) chptr->freq = linearfreqtable[finalperiod % 768] >> (finalperiod / 768);
                else chptr->freq = 14317456 / finalperiod;
                /*
                 * Update envelopes (must be done AFTER setting the finalvol
                 * or else EASTWING.XM will sound incorrect!)
                 */
                if ((ip->volenvflags & ENV_ON) && ((!(ip->volenvflags & ENV_SUSTAIN)) || (!tptr->keyon) || (tptr->volenvpos != ip->volsustpoint)))
                {
                        tptr->volenvpos++;
                        if (tptr->volenvpos > 324) tptr->volenvpos = 324;
                        if (ip->volenvflags & ENV_LOOP)
                        {
                                if (tptr->volenvpos >= ip->volloopend)
                                        tptr->volenvpos = ip->volloopstart;
                        }
                }
                if ((ip->panenvflags & ENV_ON) && ((!(ip->panenvflags & ENV_SUSTAIN)) || (!tptr->keyon) || (tptr->panenvpos != ip->pansustpoint)))
                {
                        tptr->panenvpos++;
                        if (tptr->panenvpos > 324) tptr->panenvpos = 324;
                        if (ip->panenvflags & ENV_LOOP)
                        {
                                if (tptr->panenvpos >= ip->panloopend)
                                        tptr->panenvpos = ip->panloopstart;
                        }
                }

                tptr++;
                chptr++;
        }
        /*
         * Advance song
         */
        if (xm_ticktempo)
        {
                xm_tickcount++;
                if (xm_tickcount >= xm_ticktempo)
                {
                        xm_tickcount = 0;
                        if (xm_patterndelay)
                        {
                                xm_patterndelay--;
                        }
                        if (!xm_patterndelay)
                        {
                                xm_line++;
                                if (xm_line >= pattptr->rows)
                                {
                                        xm_line = 0;
                                        xm_pos++;
                                }
                                if (xm_pos >= xm_header.songlength) {
                                  xm_pos = xm_header.restartpos;
                                  xm_oldpos = xm_pos;
                                  if (xm_roundcounter != 1)
                                  {
                                    int count;
                                    if (xm_roundcounter) xm_roundcounter--;
                                    for (count = 0; count < 256; count++)
                                    {
                                      xm_poshasbeenplayed[count] = 0;
                                    }
                                    xm_poshasbeenplayed[xm_pos] = 1;
                                  }
                                  else {
                                    judas_stopxm();
                                    return;
                                  }
                                }
                                /*
                                 * Song looping detection & control!
                                 */
                                if (xm_pos != xm_oldpos)
                                {
                                  if (xm_poshasbeenplayed[xm_pos])
                                  {
                                    if (xm_roundcounter != 1)
                                    {
                                      int count;
                                      if (xm_roundcounter) xm_roundcounter--;
                                      for (count = 0; count < 256; count++)
                                      {
                                        xm_poshasbeenplayed[count] = 0;
                                      }
                                      xm_poshasbeenplayed[xm_pos] = 1;
                                    }
                                    else {
                                      judas_stopxm();
                                      return;
                                    }
                                  }
                                  else xm_poshasbeenplayed[xm_pos] = 1;
                                }
                        }
                }
        }
}

static void startnewnote(TRACK *tptr, CHANNEL *chptr)
{
        if (tptr->newinstrument) tptr->instrument = tptr->newinstrument - 1;
        /*
         * Handle keyoff
         */
        if (tptr->note == KEYOFF - 1)
        {
                tptr->keyon = 0;
                if (!(tptr->ip->volenvflags & ENV_ON))
                {
                        tptr->notevol = 0;
                        tptr->vol = 0;
                }
                return;
        }

        /*
         * Now get instrptr & sampleptr, but DON'T change them if using
         * toneportamento
         */
        if ((tptr->effect == 0x3) || (tptr->effect == 0x5) || ((tptr->voleffect >> 4) == 0xf))
        {
                tptr->tp = 1;
        }
        else
        {
                tptr->tp = 0;
                if (tptr->instrument < xm_header.instruments) tptr->ip = first_instr_ptr + tptr->instrument;
                tptr->smp = tptr->ip->sampletable[tptr->note];
                tptr->xsp = xm_smp_ptr[tptr->instrument * MAX_SAMPLES + tptr->smp];
                tptr->sp = smp_ptr[tptr->instrument * MAX_SAMPLES + tptr->smp];
        }
        /*
         * Don't go further if sample doesn't exist
         */
        if ((tptr->xsp) && (tptr->sp))
        {
                tptr->finetune = tptr->xsp->finetune;
                tptr->multiretrigcount = 1;
                if (!(tptr->vibratotype & 4)) tptr->vibratophase = 0;
                if (!(tptr->tremolotype & 4)) tptr->tremolophase = 0;
                tptr->tremorcount = 0;
                tptr->tremorstatus = 0;
                tptr->realnote = tptr->note + tptr->xsp->relativenote;
                if (tptr->realnote < 0) tptr->realnote = 0;
                if (tptr->realnote > 118) tptr->realnote = 118;
                if (tptr->tp)
                {
                        /*
                         * Toneportamento
                         */
                        if (xm_header.uselinear) tptr->targetperiod = 7680 - tptr->realnote * 64 - tptr->finetune / 2;
                        else tptr->targetperiod = getamigaperiod(tptr->realnote, tptr->finetune);
                }
                else
                {
                        /*
                         * Normal note start
                         */
                        if (xm_header.uselinear) tptr->baseperiod = 7680 - tptr->realnote * 64 - tptr->finetune / 2;
                        else tptr->baseperiod = getamigaperiod(tptr->realnote, tptr->finetune);
                        tptr->period = tptr->baseperiod;
                        chptr->fractpos = 0;
                        chptr->pos = tptr->sp->start;
                        chptr->repeat = tptr->sp->repeat;
                        chptr->end = tptr->sp->end;
                        chptr->voicemode = tptr->sp->voicemode;
                        chptr->smp = tptr->sp;
                        if (tptr->effect == 0x9)
                        {
                                char *newpos;

                                if (tptr->sp->voicemode & VM_16BIT) newpos = tptr->sp->start + (tptr->sampleoffset << 9);
                                        else newpos = tptr->sp->start + (tptr->sampleoffset << 8);
                                if (newpos >= tptr->sp->end)
                                {
                                        chptr->voicemode = VM_OFF;
                                        chptr->smp = NULL;
                                }
                                else chptr->pos = newpos;
                        }
                }
        }
}

/*
 * Extended commands can occur both at tick 0 and on other ticks; make it a
 * function to prevent having to write it twice in the code
 */
static void extendedcommand(TRACK *tptr, CHANNEL *chptr)
{
        switch(tptr->nybble1)
        {
                /* Fine porta up */
                case 0x1:
                if (!xm_tickcount)
                {
                        if (tptr->nybble2) tptr->portaspeedup = tptr->nybble2;
                        tptr->baseperiod -= tptr->portaspeedup * 4;
                        if (tptr->baseperiod < highperiod) tptr->baseperiod = highperiod;
                }
                break;

                /* Fine porta down */
                case 0x2:
                if (!xm_tickcount)
                {
                        if (tptr->nybble2) tptr->portaspeeddown = tptr->nybble2;
                        tptr->baseperiod += tptr->portaspeeddown * 4;
                        if (tptr->baseperiod > lowperiod) tptr->baseperiod = lowperiod;
                }
                break;

                /* Set glissando */
                case 0x3:
                if (!xm_tickcount) tptr->glissando = tptr->nybble2;
                break;

                /* Set vibrato waveform */
                case 0x4:
                if (!xm_tickcount)
                {
                        tptr->vibratotype = tptr->nybble2 & 3;
                        tptr->vibratotype |= tptr->nybble2 & 4;
                }
                break;

                /* Set finetune */
                case 0x5:
                if ((!xm_tickcount) && (tptr->newnote))
                {
                        tptr->finetune = (tptr->nybble2 << 4) - 128;
                        if (xm_header.uselinear) tptr->baseperiod = 7680 - tptr->realnote * 64 - tptr->finetune / 2;
                        else tptr->baseperiod = getamigaperiod(tptr->realnote, tptr->finetune);
                        tptr->period = tptr->baseperiod;
                }
                break;

                /* Patternloop */
                case 0x6:
                if (!xm_tickcount)
                {
                        if (!tptr->nybble2) tptr->patternloopline = xm_line;
                        else
                        {
                                if (!tptr->patternloopcount)
                                {
                                        tptr->patternloopcount = tptr->nybble2;
                                        xm_line = tptr->patternloopline - 1;
                                }
                                else
                                {
                                        tptr->patternloopcount--;
                                        if (tptr->patternloopcount) xm_line = tptr->patternloopline - 1;
                                }
                        }
                }
                break;

                /* Set tremolo waveform */
                case 0x7:
                if (!xm_tickcount)
                {
                        tptr->tremolotype = vibratotypetable[tptr->nybble2 & 3];
                        tptr->tremolotype |= tptr->nybble2 & 4;
                }
                break;

                /* Retrig */
                case 0x9:
                if (tptr->nybble2)
                {
                        if (tptr->retrigcount >= tptr->nybble2)
                        {
                                tptr->retrigcount = 0;
                                /* Don't retrig on tick 0 */
                                if (xm_tickcount)
                                {
                                        startnewnote(tptr, chptr);
                                        tptr->keyon = 1;
                                        tptr->fadeoutvalue = 32768;
                                        tptr->volenvpos = 0;
                                        tptr->panenvpos = 0;
                                        tptr->instrvibratophase = 0;
                                        tptr->instrvibratodepth = 0;
                                }
                        }
                }
                else
                {
                        /*
                         * Special case e90: starts the note at tick 0 but
                         * doesn't restart envelope!
                         */
                        if (!xm_tickcount)
                        {
                                startnewnote(tptr, chptr);
                        }
                }
                tptr->retrigcount++;
                break;

                /* Notedelay */
                case 0xd:
                /* Don't start on tick 0 */
                if (!xm_tickcount) break;
                if (xm_tickcount == tptr->nybble2)
                {
                        startnewnote(tptr, chptr);
                        if (tptr->newinstrument) changeinstrument(tptr);
                        else
                        {
                                tptr->keyon = 1;
                                tptr->fadeoutvalue = 32768;
                                tptr->volenvpos = 0;
                                tptr->panenvpos = 0;
                                tptr->instrvibratophase = 0;
                                tptr->instrvibratodepth = 0;
                        }
                        if ((tptr->voleffect >= 0x10) && (tptr->voleffect <= 0x50))
                        {
                                tptr->notevol = tptr->voleffect - 0x10;
                                tptr->vol = tptr->notevol;
                        }
                        if ((tptr->voleffect >> 4) == 0xc)
                        {
                                tptr->notepanning = (tptr->voleffect & 0xf) << 4 | (tptr->voleffect & 0xf);
                        }
                }
                break;

                /* Cut note */
                case 0xc:
                if (xm_tickcount == tptr->nybble2)
                {
                        tptr->notevol = 0;
                        tptr->vol = 0; 
                }
                break;

                /* Fine volslide up */
                case 0xa:
                if (!xm_tickcount)
                {
                        if (tptr->nybble2) tptr->volspeedup = tptr->nybble2;
                        tptr->notevol += tptr->volspeedup;
                        if (tptr->notevol > 64) tptr->notevol = 64;
                }
                break;

                /* Fine volslide down */
                case 0xb:
                if (!xm_tickcount)
                {
                        if (tptr->nybble2) tptr->volspeeddown = tptr->nybble2;
                        tptr->notevol -= tptr->volspeeddown;
                        if (tptr->notevol < 0) tptr->notevol = 0;
                }
                break;

                /* Patterndelay */
                case 0xe:
                if (!xm_tickcount)
                {
                        xm_patterndelay = tptr->nybble2 + 1;
                }
                break;
        }
}

static void changeinstrument(TRACK *tptr)
{
        tptr->instrument = tptr->newinstrument - 1;
        if (tptr->xsp)
        {
                tptr->notepanning = tptr->xsp->panning;
                tptr->notevol = tptr->xsp->vol;
                tptr->vol = tptr->notevol;
        }
        /*
         * Must NOT restart envelopes if there's a keyoff just in that place
         */
        if ((!tptr->newnote) || (tptr->note != KEYOFF - 1))
        {
                tptr->keyon = 1;
                tptr->fadeoutvalue = 32768;
                tptr->volenvpos = 0;
                tptr->panenvpos = 0;
                tptr->instrvibratophase = 0;
                tptr->instrvibratodepth = 0;
        }
}

static int getamigaperiod(int note, int finetune)
{
        int noteoffset = (note % 12) << 3;
        if (finetune >= 0)
        {
                int roughfinetune = finetune / 16;
                int finefinetune = finetune & 15;
                return ((amigafreqtable[8 + noteoffset + roughfinetune] * (16 - finefinetune) +
                        amigafreqtable[9 + noteoffset + roughfinetune] * finefinetune)
                        * 2) >> (note / 12);
        }
        else
        {
                int roughfinetune = (finetune - 15) / 16;
                int finefinetune = finetune & 15;
                return ((amigafreqtable[8 + noteoffset + roughfinetune] * (16 - finefinetune) +
                        amigafreqtable[9 + noteoffset + roughfinetune] * finefinetune)
                        * 2) >> (note / 12);
        }
}

static void xmplayer_code_lock_end(void)
{
}

