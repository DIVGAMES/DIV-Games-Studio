/*
 * JUDAS 31-instrument mod loading/playing
 *
 * Changes:
 * V2.02 Removed checking of read error in instrument loading
 *       Added end > length check in instrument loading. DALLAS.MOD works now!
 * V2.03 Removed some weird printf instruction
 *       Moved tables only used by MODs here
 *       If there's a volslide with both up & down parameters nonzero must use
 *       the up parameter only!
 * V2.04 Scaled channel default pannings to 1/2 original, no more hurting ears!
 *       Added panning command "E8?" (Undocumented, but CP supports it)
 *       Corrected behaviour of instrumentnumber without a note when followed
 *       by a note without instrumentnumber!
 * V2.05 When playing a MOD and there's retrig without note then must ALWAYS
 *       retrig on tick 0! And E90 mustn't do anything, as well as a notedelay
 *       without a note!
 *       Changed also mt_patterns to unsigned because there can actually be
 *       256 patterns if the song is saved with FT2 in MOD-format!
 * V2.06 Phase of the sine vibrato rotated 180 degrees!
 *       Corrected behaviour of two pattern breaks on the same row
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

/*
 * A useful function by HEADLESS!
 */
#define HEU16_TO_UNSIGNED_SHORT(heu16)                                         \
	(((unsigned short)(heu16).high << 8) | (heu16).low)

#define AMIGA_CLOCK 3579364
#define MOD_INSTRUMENTS 31
#define MOD_MAXLENGTH 128
#define MOD_MAXCHANNELS 32
#define MOD_INFOBLOCK 1084
#define MOD_NAMEOFFSET 0
#define MOD_INSTROFFSET 20
#define MOD_LENGTHOFFSET 950
#define MOD_ORDEROFFSET 952
#define MOD_IDENTOFFSET 1080

/*
 * Motorola format 16-bit integer
 */
#pragma pack(push, 1);
typedef struct {
	unsigned char high;
	unsigned char low;
} HEU16;

/*
 * mod identification entry
 */
typedef struct {
	char *string;
	int channels;
} MOD_IDENT;

/*
 * Converted note structure
 */
typedef struct {
	unsigned char note;
	unsigned char instrument;
	unsigned char command;
	unsigned char data;
} NOTE;

/*
 * Mod instrument structure (in Motorola format...)
 */
typedef struct {
	char name[22];
	HEU16 length;
	unsigned char finetune;
	char vol;
	HEU16 repeat;
	HEU16 replen;
} MOD_INSTRUMENT;
#pragma pack(pop);

/*
 * Our instrument structure
 */
typedef struct {
	SAMPLE *smp;
	char vol;
	unsigned char finetune;
} INSTRUMENT;

/*
 * Track structure for mod playing
 */
typedef struct {
	INSTRUMENT *ip;
	unsigned char newnote;
	unsigned char note;
	unsigned char instrument;
	unsigned char newinstrument;
	unsigned char effect;
	unsigned char effectdata;
	unsigned char nybble1;
	unsigned char nybble2;
	unsigned char smp;
	signed char finetune;
	signed char vol;
	short baseperiod;
	short period;
	short targetperiod;
	unsigned char tp;
	unsigned char tpspeed;
	unsigned char volspeedup;
	unsigned char volspeeddown;
	unsigned char portaspeedup;
	unsigned char portaspeeddown;
	unsigned char vibratotype;
	unsigned char vibratospeed;
	unsigned char vibratodepth;
	unsigned char vibratophase;
	unsigned char sampleoffset;
	unsigned char usesampleoffset;
	unsigned char glissando;
	unsigned char tremolotype;
	unsigned char tremolospeed;
	unsigned char tremolodepth;
	unsigned char tremolophase;
	unsigned char patternloopline;
	unsigned char patternloopcount;
	unsigned char retrigcount;
} TRACK;

/*
 * Prototypes
 */
int judas_loadmod(char *name);
void judas_freemod(void);
void judas_playmod(int rounds);
void judas_stopmod(void);
unsigned char judas_getmodpos(void);
unsigned char judas_getmodline(void);
unsigned char judas_getmodtick(void);
unsigned char judas_getmodchannels(void);
char *judas_getmodname(void);
void judas_forwardmod(void);
void judas_rewindmod(void);
static void judas_mutechannelsmod();
void modplayer(void);
static int init_modplayer(void);
static void modplayer_code_lock_start(void);
static void modplayer_code_lock_end(void);
static void startnewnote(TRACK *tptr, CHANNEL *chptr);
static void extendedcommand(TRACK *tptr, CHANNEL *chptr);

/*
 * Mod indentification entries, don't know if anyone writes mods with
 * for example 2 channels, but they can be created with FT2!
 */
static MOD_IDENT ident[] = {
    {"2CHN", 2},  {"M.K.", 4},  {"M!K!", 4},  {"4CHN", 4},  {"6CHN", 6},
    {"8CHN", 8},  {"10CH", 10}, {"12CH", 12}, {"14CH", 14}, {"16CH", 16},
    {"18CH", 18}, {"20CH", 20}, {"22CH", 22}, {"24CH", 24}, {"26CH", 26},
    {"28CH", 28}, {"30CH", 30}, {"32CH", 32}};

/*
 * Panning table (AMIGA hardware emulation!)
 */
static int panningtable[] = {64, 191, 191, 64};

/* Period table for MODs, lowest octave (0) with all finetunes */
unsigned short periodtable[16][12] = {
    {6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4064, 3840, 3624},
    {6800, 6416, 6056, 5720, 5392, 5096, 4808, 4536, 4280, 4040, 3816, 3600},
    {6752, 6368, 6016, 5672, 5360, 5056, 4776, 4504, 4256, 4016, 3792, 3576},
    {6704, 6328, 5968, 5632, 5320, 5024, 4736, 4472, 4224, 3984, 3760, 3552},
    {6656, 6280, 5928, 5592, 5280, 4984, 4704, 4440, 4192, 3960, 3736, 3528},
    {6608, 6232, 5888, 5552, 5240, 4952, 4672, 4408, 4160, 3928, 3704, 3496},
    {6560, 6192, 5840, 5512, 5208, 4912, 4640, 4376, 4128, 3896, 3680, 3472},
    {6512, 6144, 5800, 5472, 5168, 4880, 4600, 4344, 4104, 3872, 3656, 3448},
    {7256, 6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4032, 3840},
    {7200, 6800, 6416, 6056, 5720, 5400, 5088, 4808, 4536, 4280, 4040, 3816},
    {7152, 6752, 6368, 6016, 5672, 5360, 5056, 4776, 4504, 4256, 4016, 3792},
    {7096, 6704, 6328, 5968, 5632, 5320, 5024, 4736, 4472, 4224, 3984, 3760},
    {7048, 6656, 6280, 5928, 5592, 5280, 4984, 4704, 4440, 4192, 3952, 3736},
    {7000, 6608, 6232, 5888, 5552, 5240, 4952, 4672, 4408, 4160, 3928, 3704},
    {6944, 6560, 6192, 5840, 5512, 5208, 4912, 4640, 4376, 4128, 3896, 3680},
    {6896, 6512, 6144, 5800, 5472, 5168, 4880, 4600, 4344, 4104, 3872, 3656}};

/* Module infoblock pointer */
static unsigned char *mod_info = NULL;

/* Module patterns pointer */
static unsigned char *mod_patterns = NULL;

/* Module status stuff */
static unsigned char mt_oldpos;
static unsigned char mt_pos;
static unsigned char mt_line;
static unsigned char mt_length;
static unsigned char mt_channels;
static unsigned char mt_ticktempo;
static unsigned char mt_tickcount;
static unsigned char mt_patternbreak;
static unsigned char mt_patterndelay;
static unsigned mt_patterns;
static int mt_patternlength;
static unsigned char *mt_order;
static char *mt_poshasbeenplayed;
static INSTRUMENT instrument[MOD_INSTRUMENTS];
static TRACK track[MOD_MAXCHANNELS];
static char modplayer_firsttime = 1;
static int mt_roundcounter;
static int mt_wind = 0;
static int mt_windingpause = 0;

/* External variables */
extern unsigned char judas_bpmtempo;
extern unsigned judas_bpmcount;
extern void (*judas_player)(void);

void judas_forwardmod(void) { mt_wind = 1; }

void judas_rewindmod(void) { mt_wind = -1; }

int judas_loadmod(char *name)
{
	int count;
	int handle;
	NOTE *destptr;
	unsigned char *srcptr;

	/* Don't waste memory if Nosound */
	judas_error = JUDAS_OK;
	if (judas_device == DEV_NOSOUND)
		return 1;

	judas_error = JUDAS_OUT_OF_MEMORY;
	if (modplayer_firsttime) {
		if (!init_modplayer())
			return 0;
	}
	/* Unload previous mod */
	judas_freemod();

	judas_error = JUDAS_OPEN_ERROR;
	handle = judas_open(name);
	if (handle == -1)
		return 0;

	judas_error = JUDAS_OUT_OF_MEMORY;
	/* Get memory for the mod infoblock */
	mod_info = locked_malloc(MOD_INFOBLOCK);
	if (!mod_info) {
		judas_close(handle);
		return 0;
	}

	judas_error = JUDAS_WRONG_FORMAT;
	/* Read the infoblock */
	if (judas_read(handle, mod_info, MOD_INFOBLOCK) != MOD_INFOBLOCK) {
		judas_freemod();
		judas_close(handle);
		return 0;
	}
	/*
	 * Clear byte 20 of header to prevent weird shit when printing
	 * modulename
	 */
	mod_info[20] = 0;

	/* Determine number of channels */
	mt_channels = 0;
	for (count = 0; count < 18; count++) {
		if (!memcmp(ident[count].string, &mod_info[MOD_IDENTOFFSET], 4))
			mt_channels = ident[count].channels;
	}
	/* If none of these then it's a format we can't handle!!! */
	if (!mt_channels) {
		judas_freemod();
		judas_close(handle);
		return 0;
	}
	/*
	 * Check that there aren't too many channels
	 */
	if (mt_channels > CHANNELS) {
		judas_error = JUDAS_OUT_OF_CHANNELS;
		mt_channels = CHANNELS;
		judas_freemod();
		judas_close(handle);
		return 0;
	}
	/* Calculate patternlength */
	mt_patternlength = mt_channels * 256;

	/* Now search thru orderlist to find out amount of patterns */
	mt_length = mod_info[MOD_LENGTHOFFSET];
	mt_order = &mod_info[MOD_ORDEROFFSET];
	mt_poshasbeenplayed = locked_malloc(MOD_MAXLENGTH);
	if (!mt_poshasbeenplayed) {
		judas_freemod();
		judas_close(handle);
		return 0;
	}
	mt_patterns = 0;
	for (count = 0; count < MOD_MAXLENGTH; count++) {
		if (mt_order[count] > mt_patterns)
			mt_patterns = mt_order[count];
	}
	mt_patterns++;

	judas_error = JUDAS_OUT_OF_MEMORY;
	/* Reserve memory for patterns and load them */
	mod_patterns = locked_malloc(mt_patternlength * mt_patterns);
	if (!mod_patterns) {
		judas_freemod();
		judas_close(handle);
		return 0;
	}
	judas_error = JUDAS_READ_ERROR;
	if (judas_read(handle, mod_patterns, mt_patternlength * mt_patterns) !=
	    mt_patternlength * mt_patterns) {
		judas_freemod();
		judas_close(handle);
		return 0;
	}

	/* Convert patterns into easier-to-read format */
	destptr = (NOTE *)mod_patterns;
	srcptr = mod_patterns;
	for (count = 0; count < mt_patternlength * mt_patterns / 4; count++) {
		/*
		 * Note: FT2 saves the 13th bit of period into 5th bit of
		 * samplenumber, and when loading it ofcourse cannot read
		 * the period back correctly! We don't use the 13th bit!
		 */
		unsigned short period = ((srcptr[0] & 0x0f) << 8) | srcptr[1];
		unsigned char note = 0, instrument, command;
		if (period) {
			int findnote;
			int offset = 0x7fffffff;

			for (findnote = 0; findnote < 96; findnote++) {
				if (abs(period -
					(periodtable[0][findnote % 12] >>
					 (findnote / 12))) < offset) {
					note = findnote + 1;
					offset = abs(
					    period -
					    (periodtable[0][findnote % 12] >>
					     (findnote / 12)));
				}
			}
		}
		instrument = (srcptr[0] & 0xf0) | ((srcptr[2] & 0xf0) >> 4);
		command = srcptr[2] & 0x0f;
		destptr->note = note;
		destptr->instrument = instrument;
		destptr->command = command;
		srcptr += 4;
		destptr++;
	}

	/* Now load instruments */
	{
		INSTRUMENT *i_ptr = &instrument[0];
		MOD_INSTRUMENT *mod_instr_ptr =
		    (MOD_INSTRUMENT *)(&mod_info[MOD_INSTROFFSET]);

		for (count = 0; count < MOD_INSTRUMENTS; count++) {
			int length, repeat, end;
			unsigned char voicemode;

			length = HEU16_TO_UNSIGNED_SHORT(mod_instr_ptr->length)
				 << 1;
			repeat = HEU16_TO_UNSIGNED_SHORT(mod_instr_ptr->repeat)
				 << 1;
			end = HEU16_TO_UNSIGNED_SHORT(mod_instr_ptr->replen)
			      << 1;
			i_ptr->finetune = mod_instr_ptr->finetune;
			i_ptr->vol = mod_instr_ptr->vol;
			if (length) {
				if (end > 2) {
					voicemode = VM_LOOP | VM_ON;
					end += repeat;
					if (end > length)
						end = length;
				} else {
					voicemode = VM_ONESHOT | VM_ON;
					end = length;
				}
				judas_error = JUDAS_OUT_OF_MEMORY;
				i_ptr->smp = judas_allocsample(length);
				if (!i_ptr->smp) {
					judas_freemod();
					judas_close(handle);
					return 0;
				}
				judas_read(handle, i_ptr->smp->start, length);
				i_ptr->smp->repeat = i_ptr->smp->start + repeat;
				i_ptr->smp->end = i_ptr->smp->start + end;
				i_ptr->smp->voicemode = voicemode;
				judas_ipcorrect(i_ptr->smp);
			} else {
				i_ptr->smp = NULL;
			}
			i_ptr++;
			mod_instr_ptr++;
		}
	}
	/* Loading done successfully! */
	judas_error = JUDAS_OK;
	judas_close(handle);
	return 1;
}

void judas_freemod(void)
{
	int count;

	if (modplayer_firsttime) {
		if (!init_modplayer())
			return;
	}
	judas_stopmod();

	/* Free infoblock & patterns */
	if (mod_info) {
		locked_free(mod_info);
		mod_info = NULL;
	}
	if (mod_patterns) {
		locked_free(mod_patterns);
		mod_patterns = NULL;
	}

	/* Remove all samples used by song */
	for (count = 0; count < MOD_INSTRUMENTS; count++) {
		judas_freesample(instrument[count].smp);
		instrument[count].smp = NULL;
	}
}

unsigned char judas_getmodpos(void) { return mt_pos; }

unsigned char judas_getmodline(void) { return mt_line; }

unsigned char judas_getmodtick(void) { return mt_tickcount; }

unsigned char judas_getmodchannels(void) { return mt_channels; }

char *judas_getmodname(void)
{
	if (mod_info)
		return &mod_info[MOD_NAMEOFFSET];
	else
		return NULL;
}

/*
 * This is called the first time when loading or unloading a mod. It clears
 * all sample pointers to prevent weird shit and locks all code & data touched
 * by the player routine.
 */
static int init_modplayer(void)
{
	int count;

	mt_channels = 0;
	for (count = 0; count < MOD_INSTRUMENTS; count++) {
		instrument[count].smp = NULL;
	}
	if (!judas_locktables())
		return 0;
	if (!judas_memlock(&modplayer_code_lock_start,
			   (int)&modplayer_code_lock_end -
			       (int)&modplayer_code_lock_start))
		return 0;
	if (!judas_memlock(&periodtable[0][0], sizeof periodtable))
		return 0;
	if (!judas_memlock(&instrument[0], sizeof instrument))
		return 0;
	if (!judas_memlock(&track[0], sizeof track))
		return 0;
	if (!judas_memlock(&mod_info, sizeof mod_info))
		return 0;
	if (!judas_memlock(&mod_patterns, sizeof mod_patterns))
		return 0;
	if (!judas_memlock(&mt_oldpos, sizeof mt_oldpos))
		return 0;
	if (!judas_memlock(&mt_pos, sizeof mt_pos))
		return 0;
	if (!judas_memlock(&mt_line, sizeof mt_line))
		return 0;
	if (!judas_memlock(&mt_length, sizeof mt_length))
		return 0;
	if (!judas_memlock(&mt_channels, sizeof mt_channels))
		return 0;
	if (!judas_memlock(&mt_patterns, sizeof mt_patterns))
		return 0;
	if (!judas_memlock(&mt_ticktempo, sizeof mt_ticktempo))
		return 0;
	if (!judas_memlock(&mt_tickcount, sizeof mt_tickcount))
		return 0;
	if (!judas_memlock(&mt_patternlength, sizeof mt_patternlength))
		return 0;
	if (!judas_memlock(&mt_order, sizeof mt_order))
		return 0;
	if (!judas_memlock(&mt_poshasbeenplayed, sizeof mt_poshasbeenplayed))
		return 0;
	if (!judas_memlock(&mt_patternbreak, sizeof mt_patternbreak))
		return 0;
	if (!judas_memlock(&mt_patterndelay, sizeof mt_patterndelay))
		return 0;
	if (!judas_memlock(&mt_roundcounter, sizeof mt_roundcounter))
		return 0;
	if (!judas_memlock(&mt_wind, sizeof mt_wind))
		return 0;
	if (!judas_memlock(&mt_windingpause, sizeof mt_windingpause))
		return 0;
	modplayer_firsttime = 0;
	return 1;
}

static void modplayer_code_lock_start(void) {}

void judas_mutechannelsmod(void)
{
	int count;
	TRACK *tptr = &track[0];
	CHANNEL *chptr = &judas_channel[0];

	for (count = 0; count < mt_channels; count++) {
		chptr->smp = NULL;
		chptr->voicemode = VM_OFF;
		chptr->vol = 0;
		chptr->panning = panningtable[count % 4];
		tptr->ip = &instrument[0];
		tptr->instrument = 0;
		tptr->effect = 0;
		tptr->effectdata = 0;
		tptr->nybble1 = 0;
		tptr->nybble2 = 0;
		tptr->tp = 0;
		tptr->tpspeed = 0;
		tptr->volspeedup = 0;
		tptr->volspeeddown = 0;
		tptr->glissando = 0;
		tptr->patternloopline = 0;
		tptr->patternloopcount = 0;
		tptr->retrigcount = 0;
		tptr->sampleoffset = 0;
		tptr->usesampleoffset = 0;
		tptr->tremolotype = 0;
		tptr->vibratotype = 0;
		chptr++;
		tptr++;
	}
}

void judas_playmod(int rounds)
{
	int count;

	if (!mod_info)
		return;
	judas_player = NULL;
	mt_roundcounter = rounds;
	mt_wind = 0;
	mt_pos = 0;
	mt_line = 0;
	mt_tickcount = 0;
	mt_ticktempo = 6;
	mt_patterndelay = 0;
	judas_bpmcount = 0;
	judas_bpmtempo = 125;
	for (count = 1; count < MOD_MAXLENGTH; count++) {
		mt_poshasbeenplayed[count] = 0;
	}
	judas_mutechannelsmod();
	mt_poshasbeenplayed[0] = 1;
	judas_player = &modplayer;
}

void judas_stopmod(void)
{
	CHANNEL *chptr = &judas_channel[0];
	int count;

	if (mt_channels > CHANNELS)
		mt_channels = CHANNELS;
	if (!mod_info)
		return;
	judas_player = NULL;
	for (count = 0; count < mt_channels; count++) {
		chptr->voicemode = VM_OFF;
		chptr->smp = NULL;
		chptr++;
	}
}

void modplayer(void)
{
	TRACK *tptr = &track[0];
	CHANNEL *chptr = &judas_channel[0];
	int count;

	/*
	 * Song winding
	 */
	if (mt_wind > 0) {
		mt_wind = 0;
		if (mt_pos < mt_length - 1) {
			int count;
			mt_pos++;
			mt_oldpos = mt_pos;
			mt_line = 0;
			mt_tickcount = 0;
			mt_patterndelay = 0;
			for (count = 0; count < MOD_MAXLENGTH; count++) {
				mt_poshasbeenplayed[count] = 0;
			}
			mt_poshasbeenplayed[mt_pos] = 1;
			judas_mutechannelsmod();
			mt_windingpause = 1;
			return;
		}
	} else if (mt_wind < 0) {
		mt_wind = 0;
		if (mt_pos > 0) {
			int count;
			mt_pos--;
			mt_oldpos = mt_pos;
			mt_line = 0;
			mt_tickcount = 0;
			mt_patterndelay = 0;
			for (count = 0; count < MOD_MAXLENGTH; count++) {
				mt_poshasbeenplayed[count] = 0;
			}
			mt_poshasbeenplayed[mt_pos] = 1;
			judas_mutechannelsmod();
			mt_windingpause = 1;
			return;
		}
	}
	if (mt_windingpause) {
		mt_windingpause = 0;
		return;
	}

	/*
	 * Set new notes or do something else?
	 */
	if ((!mt_tickcount) && (!mt_patterndelay)) {
		NOTE *noteptr;

		/*
		 * Beware of illegal patternnumbers
		 */
		if (mt_order[mt_pos] >= mt_patterns)
			return;
		noteptr = (NOTE *)(mod_patterns +
				   mt_order[mt_pos] * mt_patternlength +
				   mt_line * mt_channels * sizeof(NOTE));
		mt_patternbreak = 0;
		mt_oldpos = mt_pos;
		for (count = mt_channels; count > 0; count--) {
			tptr->newnote = 0;
			tptr->retrigcount = 0;

			/*
			 * Get note (if any)
			 */
			if (noteptr->note) {
				tptr->note = noteptr->note - 1;
				tptr->newnote = 1;
			}
			/*
			 * Get effect, effect data etc.
			 */
			tptr->effect = noteptr->command;
			tptr->effectdata = noteptr->data;
			tptr->nybble1 = noteptr->data >> 4;
			tptr->nybble2 = noteptr->data & 0xf;
			tptr->newinstrument = noteptr->instrument;

			/*
			 * Set sampleoffset here
			 */
			if (tptr->newinstrument)
				tptr->usesampleoffset = 0;
			if (tptr->effect == 0x9) {
				if (tptr->effectdata)
					tptr->sampleoffset = tptr->effectdata;
				tptr->usesampleoffset = 1;
			}

			/*
			 * Start new note if there is one; but check there
			 * isn't notedelay (toneportamento is handled by
			 * startnewnote()!)
			 */
			if ((tptr->effect != 0xe) || (tptr->nybble1 != 0xd) ||
			    (tptr->nybble2 == 0)) {
				if (tptr->newnote)
					startnewnote(tptr, chptr);
				if (tptr->newinstrument) {
					tptr->instrument =
					    tptr->newinstrument - 1;
					tptr->ip =
					    &instrument[tptr->instrument];
					tptr->vol = tptr->ip->vol;
					chptr->vol = tptr->vol * 256;
				}
			}

			/*
			 * Reset period if not vibrato or toneportamento
			 */
			if ((tptr->effect < 0x3) || (tptr->effect > 0x6)) {
				tptr->period = tptr->baseperiod;
			}
			/*
			 * Reset volume if not tremolo
			 */
			if (tptr->effect != 0x7) {
				chptr->vol = tptr->vol * 256;
			}
			switch (tptr->effect) {
			case 0x0:
				break;

			/* Set portamento speed up */
			case 0x1:
				if (tptr->effectdata)
					tptr->portaspeedup = tptr->effectdata;
				break;

			/* Set portamento speed down */
			case 0x2:
				if (tptr->effectdata)
					tptr->portaspeeddown = tptr->effectdata;
				break;

			/* Set TP. speed */
			case 0x3:
				if (tptr->effectdata)
					tptr->tpspeed = tptr->effectdata;
				break;

			/* Set vibrato */
			case 0x4:
				if (tptr->nybble1)
					tptr->vibratospeed = tptr->nybble1;
				if (tptr->nybble2)
					tptr->vibratodepth = tptr->nybble2;
				break;

			/* Set tremolo */
			case 0x7:
				if (tptr->nybble1)
					tptr->tremolospeed = tptr->nybble1;
				if (tptr->nybble2)
					tptr->tremolodepth = tptr->nybble2;
				break;

			/* Set Panning */
			case 0x8:
				chptr->panning = tptr->effectdata;
				break;

			/* Volume slide speed set */
			case 0x5:
			case 0x6:
			case 0xa:
				if (tptr->effectdata) {
					tptr->volspeedup = tptr->nybble1;
					tptr->volspeeddown = tptr->nybble2;
				}
				break;

			/* Pos. jump */
			case 0xb:
				mt_line = 63;
				mt_pos = tptr->effectdata - 1;
				break;

			/* Set volume */
			case 0xc:
				chptr->vol = tptr->effectdata * 256;
				if (chptr->vol < 0)
					chptr->vol = 0;
				if (chptr->vol > 64 * 256)
					chptr->vol = 64 * 256;
				tptr->vol = chptr->vol / 256;
				break;

			/* Pattern break */
			case 0xd:
				mt_line =
				    tptr->nybble1 * 10 + tptr->nybble2 - 1;
				if (!mt_patternbreak) {
					mt_patternbreak = 1;
					mt_pos++;
				}
				break;

			/* Extended command */
			case 0xe:
				extendedcommand(tptr, chptr);
				break;

			/* Set tempo */
			case 0xf:
				if (!tptr->effectdata) {
					if (mt_roundcounter != 1) {
						judas_stopmod();
						judas_playmod(mt_roundcounter);
						if (mt_roundcounter)
							mt_roundcounter--;
						return;
					} else {
						judas_stopmod();
						return;
					}
				}
				if (tptr->effectdata < 32)
					mt_ticktempo = tptr->effectdata;
				else
					judas_bpmtempo = tptr->effectdata;
				break;
			}
			if (tptr->period)
				chptr->freq = AMIGA_CLOCK / tptr->period;
			noteptr++;
			chptr++;
			tptr++;
		}
	}
	if (mt_tickcount) {
		/*
		 * If tick isn't 0, update effects like portamento & arpeggio
		 */
		for (count = mt_channels; count > 0; count--) {
			switch (tptr->effect) {
			/* Arpeggio */
			case 0x0: {
				if (tptr->effectdata) {
					char phase = mt_tickcount % 3;
					switch (phase) {
						unsigned char arpnote;

					case 0:
						tptr->period = tptr->baseperiod;
						break;

					case 1:
						arpnote =
						    tptr->note + tptr->nybble1;
						if (arpnote > 95)
							arpnote = 95;
						tptr->period =
						    periodtable[tptr->finetune]
							       [arpnote % 12] >>
						    (arpnote / 12);
						break;

					case 2:
						arpnote =
						    tptr->note + tptr->nybble2;
						if (arpnote > 95)
							arpnote = 95;
						tptr->period =
						    periodtable[tptr->finetune]
							       [arpnote % 12] >>
						    (arpnote / 12);
						break;
					}
				}
			} break;

			/* Portamento up */
			case 0x1:
				tptr->baseperiod -= tptr->portaspeedup;
				if (tptr->baseperiod < 27)
					tptr->baseperiod = 27;
				tptr->period = tptr->baseperiod;
				break;

			/* Portamento down */
			case 0x2:
				tptr->baseperiod += tptr->portaspeeddown;
				if (tptr->baseperiod > 7256)
					tptr->baseperiod = 7256;
				tptr->period = tptr->baseperiod;
				break;

			/* Toneportamento */
			case 0x3:
				if (tptr->tp) {
					if (tptr->baseperiod <
					    tptr->targetperiod) {
						tptr->baseperiod +=
						    tptr->tpspeed;
						if (tptr->baseperiod >=
						    tptr->targetperiod) {
							tptr->baseperiod =
							    tptr->targetperiod;
							tptr->tp = 0;
						}
					}
					if (tptr->baseperiod >
					    tptr->targetperiod) {
						tptr->baseperiod -=
						    tptr->tpspeed;
						if (tptr->baseperiod <=
						    tptr->targetperiod) {
							tptr->baseperiod =
							    tptr->targetperiod;
							tptr->tp = 0;
						}
					}
					tptr->period = tptr->baseperiod;
					if (tptr->glissando) {
						int offset = 0x7fffffff;
						int sc;
						short bestperiod;

						for (sc = 0; sc < 96; sc++) {
							int newoffset = abs(
							    tptr->period -
							    (periodtable
								 [tptr->finetune]
								 [sc % 12] >>
							     (sc / 12)));

							if (newoffset <
							    offset) {
								bestperiod =
								    periodtable
									[tptr->finetune]
									[sc %
									 12] >>
								    (sc / 12);
								offset =
								    newoffset;
							}
						}
						tptr->period = bestperiod;
					}
				}
				break;

			/* Vibrato */
			case 0x4:
				tptr->vibratophase += tptr->vibratospeed * 4;
				tptr->period =
				    tptr->baseperiod +
				    ((vibratotable[vibratotypetable
						       [tptr->vibratotype & 3]]
						  [tptr->vibratophase] *
				      tptr->vibratodepth) >>
				     5);
				if (tptr->period < 27)
					tptr->period = 27;
				if (tptr->period > 7256)
					tptr->period = 7256;
				break;

			/* Toneportamento + volslide */
			case 0x5:
				if (tptr->tp) {
					if (tptr->baseperiod <
					    tptr->targetperiod) {
						tptr->baseperiod +=
						    tptr->tpspeed;
						if (tptr->baseperiod >=
						    tptr->targetperiod) {
							tptr->baseperiod =
							    tptr->targetperiod;
							tptr->tp = 0;
						}
					}
					if (tptr->baseperiod >
					    tptr->targetperiod) {
						tptr->baseperiod -=
						    tptr->tpspeed;
						if (tptr->baseperiod <=
						    tptr->targetperiod) {
							tptr->baseperiod =
							    tptr->targetperiod;
							tptr->tp = 0;
						}
					}
					tptr->period = tptr->baseperiod;
					if (tptr->glissando) {
						int offset = 0x7fffffff;
						int sc;
						short bestperiod;

						for (sc = 0; sc < 96; sc++) {
							int newoffset = abs(
							    tptr->period -
							    (periodtable
								 [tptr->finetune]
								 [sc % 12] >>
							     (sc / 12)));

							if (newoffset <
							    offset) {
								bestperiod =
								    periodtable
									[tptr->finetune]
									[sc %
									 12] >>
								    (sc / 12);
								offset =
								    newoffset;
							}
						}
						tptr->period = bestperiod;
					}
				}
				if (tptr->volspeedup) {
					chptr->vol += tptr->volspeedup * 256;
					if (chptr->vol > 64 * 256)
						chptr->vol = 64 * 256;
				} else {
					chptr->vol -= tptr->volspeeddown * 256;
					if (chptr->vol < 0)
						chptr->vol = 0;
				}
				tptr->vol = chptr->vol / 256;
				break;

			/* Vibrato + volslide */
			case 0x6:
				tptr->vibratophase += tptr->vibratospeed * 4;
				tptr->period =
				    tptr->baseperiod +
				    ((vibratotable[vibratotypetable
						       [tptr->vibratotype & 3]]
						  [tptr->vibratophase] *
				      tptr->vibratodepth) >>
				     5);
				if (tptr->period < 27)
					tptr->period = 27;
				if (tptr->period > 7256)
					tptr->period = 7256;
				if (tptr->volspeedup) {
					chptr->vol += tptr->volspeedup * 256;
					if (chptr->vol > 64 * 256)
						chptr->vol = 64 * 256;
				} else {
					chptr->vol -= tptr->volspeeddown * 256;
					if (chptr->vol < 0)
						chptr->vol = 0;
				}
				tptr->vol = chptr->vol / 256;
				break;

			/* Tremolo */
			case 0x7:
				tptr->tremolophase += tptr->tremolospeed * 4;
				chptr->vol =
				    (tptr->vol +
				     ((vibratotable[tptr->tremolotype & 3]
						   [tptr->tremolophase] *
				       tptr->tremolodepth) >>
				      4)) *
				    256;
				if (chptr->vol < 0)
					chptr->vol = 0;
				if (chptr->vol > 64 * 256)
					chptr->vol = 64 * 256;
				break;

			/* Volume Slide */
			case 0xa:
				if (tptr->volspeedup) {
					chptr->vol += tptr->volspeedup * 256;
					if (chptr->vol > 64 * 256)
						chptr->vol = 64 * 256;
				} else {
					chptr->vol -= tptr->volspeeddown * 256;
					if (chptr->vol < 0)
						chptr->vol = 0;
				}
				tptr->vol = chptr->vol / 256;
				break;

			/* Extended command */
			case 0xe:
				extendedcommand(tptr, chptr);
				break;
			}
			if (tptr->period)
				chptr->freq = AMIGA_CLOCK / tptr->period;
			chptr++;
			tptr++;
		}
	}

	/*
	 * Advance song
	 */
	mt_tickcount++;
	if (mt_tickcount >= mt_ticktempo) {
		mt_tickcount = 0;
		if (mt_patterndelay) {
			mt_patterndelay--;
		}
		if (!mt_patterndelay) {
			mt_line++;
			if (mt_line >= 64) {
				mt_line = 0;
				mt_pos++;
			}
			if (mt_pos >= mt_length) {
				mt_pos = 0;
				mt_oldpos = mt_pos;
				if (mt_roundcounter != 1) {
					int count;
					if (mt_roundcounter)
						mt_roundcounter--;
					for (count = 0; count < MOD_MAXLENGTH;
					     count++) {
						mt_poshasbeenplayed[count] = 0;
					}
					mt_poshasbeenplayed[mt_pos] = 1;
				} else {
					judas_stopmod();
					return;
				}
			}
			/*
			 * Song looping detection & control!
			 */
			if (mt_pos != mt_oldpos) {
				if (mt_poshasbeenplayed[mt_pos]) {
					if (mt_roundcounter != 1) {
						int count;
						if (mt_roundcounter)
							mt_roundcounter--;
						for (count = 0;
						     count < MOD_MAXLENGTH;
						     count++) {
							mt_poshasbeenplayed
							    [count] = 0;
						}
						mt_poshasbeenplayed[mt_pos] = 1;
					} else {
						judas_stopmod();
						return;
					}
				} else
					mt_poshasbeenplayed[mt_pos] = 1;
			}
		}
	}
	/*
	 * We're Done!
	 */
}

static void startnewnote(TRACK *tptr, CHANNEL *chptr)
{
	/*
	 * Change instrument if necessary
	 */
	if (tptr->newinstrument) {
		tptr->instrument = tptr->newinstrument - 1;
		tptr->ip = &instrument[tptr->instrument];
	}

	/*
	 * Now we set the note on
	 */
	tptr->finetune = tptr->ip->finetune;
	if (!(tptr->vibratotype & 4))
		tptr->vibratophase = 0;
	if (!(tptr->tremolotype & 4))
		tptr->tremolophase = 0;
	if ((tptr->effect == 0x3) || (tptr->effect == 0x5)) {
		/*
		 * Toneportamento
		 */
		tptr->targetperiod =
		    periodtable[tptr->finetune][tptr->note % 12] >>
		    (tptr->note / 12);
		tptr->tp = 1;
	} else {
		/*
		 * Normal note start
		 */
		tptr->baseperiod =
		    periodtable[tptr->finetune][tptr->note % 12] >>
		    (tptr->note / 12);
		tptr->period = tptr->baseperiod;
		tptr->tp = 0;
		if (tptr->ip->smp) {
			chptr->fractpos = 0;
			chptr->repeat = tptr->ip->smp->repeat;
			chptr->end = tptr->ip->smp->end;
			chptr->voicemode = tptr->ip->smp->voicemode;
			chptr->smp = tptr->ip->smp;
			if (tptr->usesampleoffset) {
				chptr->pos = tptr->ip->smp->start +
					     (tptr->sampleoffset << 8);
				if (chptr->pos >= tptr->ip->smp->end) {
					if (chptr->voicemode & VM_LOOP) {
						chptr->pos =
						    tptr->ip->smp->repeat;
					} else {
						chptr->voicemode = VM_OFF;
						chptr->smp = NULL;
					}
				}
			} else {
				chptr->pos = tptr->ip->smp->start;
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
	switch (tptr->nybble1) {
	/* Fine porta up */
	case 0x1:
		if (!mt_tickcount) {
			if (tptr->nybble2)
				tptr->portaspeedup = tptr->nybble2;
			tptr->baseperiod -= tptr->portaspeedup;
			if (tptr->baseperiod < 27)
				tptr->baseperiod = 27;
		}
		break;

	/* Fine porta down */
	case 0x2:
		if (!mt_tickcount) {
			if (tptr->nybble2)
				tptr->portaspeeddown = tptr->nybble2;
			tptr->baseperiod += tptr->portaspeeddown;
			if (tptr->baseperiod > 7256)
				tptr->baseperiod = 7256;
		}
		break;

	/* Set glissando */
	case 0x3:
		if (!mt_tickcount)
			tptr->glissando = tptr->nybble2;
		break;

	/* Set vibrato waveform */
	case 0x4:
		if (!mt_tickcount) {
			tptr->vibratotype = tptr->nybble2 & 3;
			tptr->vibratotype |= tptr->nybble2 & 4;
		}
		break;

	/* Set finetune */
	case 0x5:
		if ((!mt_tickcount) && (tptr->newnote)) {
			tptr->finetune = (tptr->nybble2 - 8) & 15;
			tptr->baseperiod =
			    periodtable[tptr->finetune][tptr->note % 12] >>
			    (tptr->note / 12);
			tptr->period = tptr->baseperiod;
		}
		break;

	/* Patternloop */
	case 0x6:
		if (!mt_tickcount) {
			if (!tptr->nybble2)
				tptr->patternloopline = mt_line;
			else {
				if (!tptr->patternloopcount) {
					tptr->patternloopcount = tptr->nybble2;
					mt_line = tptr->patternloopline - 1;
				} else {
					tptr->patternloopcount--;
					if (tptr->patternloopcount)
						mt_line =
						    tptr->patternloopline - 1;
				}
			}
		}
		break;

	/* Set tremolo waveform */
	case 0x7:
		if (!mt_tickcount) {
			tptr->tremolotype = vibratotypetable[tptr->nybble2 & 3];
			tptr->tremolotype |= tptr->nybble2 & 4;
		}
		break;

	/* Set panning (Undocumented) */
	case 0x8: {
		chptr->panning = (tptr->nybble2 << 4) | tptr->nybble2;
	} break;

	/* Retrig */
	case 0x9:
		if (tptr->nybble2) {
			/*
			 * This looks fuckin' weird, but achieves the right
			 * effect...
			 */
			if ((!tptr->newnote) && (!mt_tickcount)) {
				tptr->retrigcount = mt_ticktempo;
			}
			if (tptr->retrigcount >= tptr->nybble2) {
				tptr->retrigcount = 0;
				startnewnote(tptr, chptr);
			}
		}
		tptr->retrigcount++;
		break;

	/* Notedelay */
	case 0xd:
		/* Don't start on tick 0 or if there's no note */
		if ((!mt_tickcount) || (!tptr->newnote))
			break;
		if (mt_tickcount == tptr->nybble2) {
			startnewnote(tptr, chptr);
			if (tptr->newinstrument) {
				tptr->vol = tptr->ip->vol;
				chptr->vol = tptr->vol * 256;
			}
		}
		break;

	/* Cut note */
	case 0xc:
		if (mt_tickcount == tptr->nybble2) {
			chptr->vol = 0;
			tptr->vol = 0;
		}
		break;

	/* Fine volslide up */
	case 0xa:
		if (!mt_tickcount) {
			if (tptr->nybble2)
				tptr->volspeedup = tptr->nybble2;
			chptr->vol += tptr->volspeedup * 256;
			if (chptr->vol > 64 * 256)
				chptr->vol = 64 * 256;
			tptr->vol = chptr->vol / 256;
		}
		break;

	/* Fine volslide down */
	case 0xb:
		if (!mt_tickcount) {
			if (tptr->nybble2)
				tptr->volspeeddown = tptr->nybble2;
			chptr->vol -= tptr->volspeeddown * 256;
			if (chptr->vol < 0)
				chptr->vol = 0;
			tptr->vol = chptr->vol / 256;
		}
		break;

	/* Patterndelay */
	case 0xe:
		if (!mt_tickcount) {
			mt_patterndelay = tptr->nybble2 + 1;
		}
		break;
	}
}

static void modplayer_code_lock_end(void) {}
