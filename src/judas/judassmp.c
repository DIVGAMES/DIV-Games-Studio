/*
 * JUDAS sample handling
 */

#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>
#include "judas.h"
#include "judascfg.h"
#include "osdep.h"
#include <math.h>

/* A "volume profile" for each sample will be precalculated.
 *
 * VUPROFILESAMPLES defines how many samplepoints are used in the estimation of
 * one volume value. VUPROFILERATIO defines how many samplepoints there are
 * between the volume estimations. The first sample in the sampledata will be
 * in the middle of the first chunk to investigate.
 */
#define VUPROFILERATIO 512

/*
 * This is something interesting...the sample pointer might go this many bytes
 * over the sample end while in the mixing inner loop. Here's the formula:
 *
 *         maximum_playspeed / minimum_mixspeed (round upwards for safety)
 * bytes = ---------------------------------------------------------------
 *                       repeats_in_mixing_inner_loop
 *
 * Maximum playspeed is 535232 (highest speed in linear freq. table), minimum
 * mixspeed is 5000 and there are 16 repeats.
 * And, of course, because there can be 16bit samples the number of bytes must
 * be multiplied with 2.
 */
#define ENDSAFETYAREA ((3456 > 4 * VUPROFILERATIO) ? 3456 : 4 * VUPROFILERATIO)
#define STARTSAFETYAREA 4

SAMPLE *judas_allocsample(int length);
void judas_freesample(SAMPLE *smp);
void judas_playsample(SAMPLE *smp, unsigned chnum, unsigned frequency,
		      unsigned short volume, unsigned char panning);
void judas_ipcorrect(SAMPLE *smp);
void judas_stopsample(unsigned chnum);
void judas_preventdistortion(unsigned channels);
void judas_setmastervolume(unsigned chnum, unsigned char mastervol);
void judas_setmusicmastervolume(unsigned musicchannels,
				unsigned char mastervol);
void judas_setsfxmastervolume(unsigned musicchannels, unsigned char mastervol);
void judas_calcvuprofile(SAMPLE *smp);
float judas_getvumeter(unsigned chnum);

SAMPLE *judas_allocsample(int length)
{
	SAMPLE *smp;
	int vuprofilelen = 1 + length / VUPROFILERATIO;

	judas_error = JUDAS_OUT_OF_MEMORY;

	length += STARTSAFETYAREA + ENDSAFETYAREA; // ***
	smp = (SAMPLE *)locked_malloc(sizeof(SAMPLE));
	if (!smp)
		return NULL;
	smp->start = locked_malloc(length);
	if (!smp->start) {
		judas_freesample(smp);
		return NULL;
	}
	// vuprofile allocation:
	smp->vuprofile = locked_malloc(
	    vuprofilelen + 1); // Some extra, because of interpolation
	if (!smp->vuprofile) {
		judas_freesample(smp);
		return NULL;
	}

	smp->start += STARTSAFETYAREA; // ***
	smp->repeat = NULL;
	smp->end = NULL;
	judas_error = JUDAS_OK;
	return smp;
}

void judas_freesample(SAMPLE *smp)
{
	int c;
	CHANNEL *chptr;

	if (!smp)
		return;
	if (smp == &fakesample)
		return;
	/*
	 * Go thru all channels; if that sample is playing then stop the
	 * channel; we don't want a crash or audible shit!
	 */
	chptr = &judas_channel[0];
	for (c = CHANNELS; c > 0; c--) {
		if (chptr->smp == smp) {
			chptr->smp = NULL;
			chptr->voicemode = VM_OFF;
		}
		chptr++;
	}
	/*
	 * Free the sample data and then the sample structure itself
	 */
	if (smp->vuprofile)
		locked_free(smp->vuprofile);
	if (smp->start)
		locked_free(smp->start - STARTSAFETYAREA); // ***
	locked_free(smp);
}

void judas_ipcorrect(SAMPLE *smp)
{
	if (!smp)
		return;
	if (smp == &fakesample)
		return;

	{
		int count;
		for (count = -STARTSAFETYAREA; count < 0; count++)
			*(smp->start + count) = 0;
	}

	if (smp->voicemode & VM_LOOP) {
		int count, lcount = smp->end - smp->repeat;
		unsigned char *src = &smp->repeat[0], *dest = &smp->end[0];

		if (!src)
			return;
		if (!dest)
			return;

		if (smp->voicemode & VM_16BIT)
			count = ENDSAFETYAREA;
		else
			count = ENDSAFETYAREA / 2;
		while (count) {
			if (lcount <= 0) {
				lcount = smp->end - smp->repeat;
				src = &smp->repeat[0];
			}
			*dest++ = *src++;
			lcount--;
			count--;
		}
	} else {
		/* Since V2.04 the data beyond sample end is presumed to be
		 * what it was at sample end, not zero! (Because of q-mixer)
		 */
		if (!smp->end)
			return;
		if (smp->voicemode & VM_16BIT) {
			int count = ENDSAFETYAREA / 2;
			char *dest = &smp->end[0];
			while (count) {
				*dest = smp->end[-2];
				*(dest + 1) = smp->end[-1];
				dest += 2;
				count--;
			}
		} else {
			int count = ENDSAFETYAREA;
			char *dest = &smp->end[0];
			while (count) {
				*dest++ = smp->end[-1]; /* Toivottavasti samplen
							   pituus > 0 */
				count--;
			}
		}
	}
	judas_calcvuprofile(smp);
}

void judas_calcvuprofile(SAMPLE *smp)
{
	if (smp->voicemode & VM_16BIT) {
		if (smp->voicemode & VM_LOOP) {
			unsigned char *vup = smp->vuprofile;
			int samples = (smp->end - smp->start) / 2;
			int chunks = 1 + samples / VUPROFILERATIO +
				     1; // +1 for interpolation
			int chunk;
			for (chunk = 0; chunk < chunks; chunk++) {
				float thisvol = 0;
				int chunksample;
				signed short *firstsmpinchunk =
				    (short *)smp->start - VUPROFILERATIO / 2 +
				    chunk * VUPROFILERATIO;
				if (firstsmpinchunk < (short *)smp->start)
					firstsmpinchunk = (short *)smp->start;
				for (chunksample = 0;
				     (chunksample < VUPROFILERATIO);
				     chunksample++) {
					thisvol +=
					    ((float)
						 firstsmpinchunk[chunksample] *
					     firstsmpinchunk[chunksample]) /
					    16384;
				}
				thisvol = sqrt(255 * sqrt((float)thisvol /
							  VUPROFILERATIO));
				if (thisvol > 255)
					thisvol = 255;
				*vup = thisvol;
				vup++;
			}
		} else {
			unsigned char *vup = smp->vuprofile;
			int samples = (smp->end - smp->start) / 2;
			int chunks = 1 + samples / VUPROFILERATIO +
				     1; // +1 for interpolation
			int chunk;
			for (chunk = 0; chunk < chunks; chunk++) {
				float thisvol = 0;
				int chunksample;
				signed short *firstsmpinchunk =
				    (short *)smp->start - VUPROFILERATIO / 2 +
				    chunk * VUPROFILERATIO;
				if (firstsmpinchunk < (short *)smp->start)
					firstsmpinchunk = (short *)smp->start;
				for (chunksample = 0;
				     (chunksample < VUPROFILERATIO);
				     chunksample++) {
					if (&firstsmpinchunk[chunksample] >=
					    (short *)smp->end)
						break;
					thisvol +=
					    ((float)
						 firstsmpinchunk[chunksample] *
					     firstsmpinchunk[chunksample]) /
					    16384;
				}
				thisvol = sqrt(255 * sqrt((float)thisvol /
							  VUPROFILERATIO));
				if (thisvol > 255)
					thisvol = 255;
				*vup = thisvol;
				vup++;
			}
		}
	} else {
		if (smp->voicemode & VM_LOOP) {
			unsigned char *vup = smp->vuprofile;
			int samples = (smp->end - smp->start);
			int chunks = 1 + samples / VUPROFILERATIO +
				     1; // +1 for interpolation
			int chunk;
			for (chunk = 0; chunk < chunks; chunk++) {
				float thisvol = 0;
				int chunksample;
				signed char *firstsmpinchunk =
				    smp->start - VUPROFILERATIO / 2 +
				    chunk * VUPROFILERATIO;
				if (firstsmpinchunk < smp->start)
					firstsmpinchunk = smp->start;
				for (chunksample = 0;
				     (chunksample < VUPROFILERATIO);
				     chunksample++) {
					thisvol +=
					    ((float)
						 firstsmpinchunk[chunksample] *
					     firstsmpinchunk[chunksample]);
				}
				thisvol = sqrt(255 * sqrt(4 * (float)thisvol /
							  VUPROFILERATIO));
				if (thisvol > 255)
					thisvol = 255;
				*vup = thisvol;
				vup++;
			}
		} else {
			unsigned char *vup = smp->vuprofile;
			int samples = (smp->end - smp->start);
			int chunks = 1 + samples / VUPROFILERATIO +
				     1; // +1 for interpolation
			int chunk;
			for (chunk = 0; chunk < chunks; chunk++) {
				float thisvol = 0;
				int chunksample;
				signed char *firstsmpinchunk =
				    smp->start - VUPROFILERATIO / 2 +
				    chunk * VUPROFILERATIO;
				if (firstsmpinchunk < smp->start)
					firstsmpinchunk = smp->start;
				for (chunksample = 0;
				     (chunksample < VUPROFILERATIO);
				     chunksample++) {
					if (&firstsmpinchunk[chunksample] >=
					    smp->end)
						break;
					thisvol +=
					    ((float)
						 firstsmpinchunk[chunksample] *
					     firstsmpinchunk[chunksample]);
				}
				thisvol = sqrt(255 * sqrt(4 * (float)thisvol /
							  VUPROFILERATIO));
				if (thisvol > 255)
					thisvol = 255;
				*vup = thisvol;
				vup++;
			}
		}
	}
}

float judas_getvumeter(unsigned chnum)
{
	if ((chnum >= CHANNELS) ||
	    ((judas_channel[chnum].voicemode & VM_ON) == VM_OFF) ||
	    (!judas_channel[chnum].smp))
		return 0;
	{
		SAMPLE *smp = judas_channel[chnum].smp;
		float vu = judas_channel[chnum].vol;
		char *pos = judas_channel[chnum].pos;
		int chunk;
		float fractpos, vuprofile0, vuprofile1;
		if (pos >= smp->end || pos < smp->start)
			return 0; // Might have changed!!!
		if (smp->voicemode & VM_16BIT) {
			chunk = (pos - smp->start) / (2 * VUPROFILERATIO);
			fractpos =
			    (float)(pos - smp->start) / (2 * VUPROFILERATIO) -
			    chunk;
		} else {
			chunk = (pos - smp->start) / (VUPROFILERATIO);
			fractpos =
			    (float)(pos - smp->start) / (VUPROFILERATIO)-chunk;
		}
		// Interpolation:
		vuprofile0 = smp->vuprofile[chunk];
		vuprofile1 = smp->vuprofile[chunk + 1];
		vuprofile0 *= vuprofile0 / 255;
		vuprofile1 *= vuprofile1 / 255;
		vu *= fractpos * (vuprofile1 - vuprofile0) + vuprofile0;
		// max now 255*64*256
		vu /= (float)64 * 255 * 256; // Shrink to 0..1 but not 1
		return (vu < 1) ? vu : 0.99999999;
	}
}

void judas_playsample(SAMPLE *smp, unsigned chnum, unsigned frequency,
		      unsigned short volume, unsigned char panning)
{
	CHANNEL *chptr;

	if (!smp)
		return;
	if (smp == &fakesample)
		return;
	if (chnum >= CHANNELS)
		return;
	chptr = &judas_channel[chnum];
	chptr->voicemode = VM_OFF;
	chptr->pos = smp->start;
	chptr->repeat = smp->repeat;
	chptr->end = smp->end;
	chptr->smp = smp;
	chptr->fractpos = 0;
	chptr->freq = frequency;
	chptr->vol = volume;
	chptr->panning = panning;
	chptr->voicemode = smp->voicemode;
}

void judas_stopsample(unsigned chnum)
{
	CHANNEL *chptr;

	if (chnum >= CHANNELS)
		return;
	chptr = &judas_channel[chnum];
	chptr->voicemode = VM_OFF;
	chptr->smp = NULL;
}

void judas_preventdistortion(unsigned active_channels)
{
	int count;
	unsigned char mastervol;

	if (active_channels < 2)
		mastervol = 255;
	else
		mastervol = 256 / active_channels;
	for (count = 0; count < CHANNELS; count++) {
		judas_setmastervolume(count, mastervol);
	}
}

void judas_setmastervolume(unsigned chnum, unsigned char mastervol)
{
	CHANNEL *chptr;

	if (chnum >= CHANNELS)
		return;
	chptr = &judas_channel[chnum];
	chptr->mastervol = mastervol;
}

void judas_setmusicmastervolume(unsigned musicchannels, unsigned char mastervol)
{
	CHANNEL *chptr = &judas_channel[0];
	int count;

	if (musicchannels > CHANNELS)
		musicchannels = CHANNELS;
	for (count = 0; count < musicchannels; count++) {
		chptr->mastervol = mastervol;
		chptr++;
	}
}

void judas_setsfxmastervolume(unsigned musicchannels, unsigned char mastervol)
{
	CHANNEL *chptr;
	int count;

	if (musicchannels >= CHANNELS)
		return;
	chptr = &judas_channel[musicchannels];
	for (count = musicchannels; count < CHANNELS; count++) {
		chptr->mastervol = mastervol;
		chptr++;
	}
}
