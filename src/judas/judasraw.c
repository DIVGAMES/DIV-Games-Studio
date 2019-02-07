/*
 * JUDAS raw sample routines
 */

#include <stdio.h>
#include <stdlib.h>
//#include <mem.h>
#include "judas.h"
#include "osdep.h"

int JudasProgressRead(int handle, void *buffer, int size);

SoundInfo *judas_loadrawsample(char *name, int repeat, int end,
			       unsigned char voicemode)
{
	int length;
	int handle;
	SAMPLE *smp;
	unsigned char *SI_aux, *temp;
	short *short_ptr;
	SoundInfo *SI = NULL;
	int con;

	if ((SI_aux = (char *)malloc(sizeof(SoundInfo))) == NULL)
		return (SI);
	SI = (SoundInfo *)SI_aux;

	SI->SoundFreq = 0;
	SI->SoundBits = 0;
	SI->SoundSize = 0;
	SI->SoundData = NULL;
	SI->sample = &fakesample;

	/* Don't waste memory if Nosound */
	judas_error = JUDAS_OK;

	/*
	if (judas_device == DEV_NOSOUND)
	{
		return(SI);
	}
*/

	judas_error = JUDAS_OPEN_ERROR;
	handle = judas_open(name);
	if (handle == -1)
		return (SI);
	length = judas_seek(handle, 0, SEEK_END);
	if (length == -1) {
		judas_close(handle);
		return (SI);
	}
	judas_seek(handle, 0, SEEK_SET);
	if ((temp = (char *)malloc(length)) == NULL) {
		judas_close(handle);
		return (SI);
	}
	smp = judas_allocsample(length * 2);
	if (!smp) {
		judas_close(handle);
		return (SI);
	}
	if (end == 0)
		end = length * 2;
	if (end > length * 2)
		end = length * 2;
	if (repeat > length * 2 - 1)
		repeat = length * 2 - 1;
	judas_error = JUDAS_READ_ERROR;
	if (length < 500000) {
		if (judas_read(handle, temp, length) != length) {
			judas_freesample(smp);
			judas_close(handle);
			free(temp);
			return (SI);
		}
	} else {
		if (!JudasProgressRead(handle, temp, length)) {
			judas_freesample(smp);
			judas_close(handle);
			free(temp);
			return (SI);
		}
	}
	judas_close(handle);
	short_ptr = (short *)smp->start;
	for (con = 0; con < length; con++)
		short_ptr[con] = (short)(((*(temp + con)) + 0x80) * 0x100);
	free(temp);
	smp->repeat = smp->start + repeat;
	smp->end = smp->start + end;
	smp->voicemode = voicemode | VM_ON | VM_16BIT;
	judas_ipcorrect(smp);
	judas_error = JUDAS_OK;

	SI->SoundFreq = 11025;
	SI->SoundBits = 8;
	SI->SoundSize = length;
	SI->SoundData = (short *)smp->start;
	SI->sample = smp;

	return (SI);
}

SoundInfo *judas_loadrawsample_mem(char *FileBuffer, int length, int repeat,
				   int end, unsigned char voicemode)
{
	SAMPLE *smp;
	unsigned char *SI_aux, *temp;
	short *short_ptr;
	SoundInfo *SI = NULL;
	int con;

	if ((SI_aux = (char *)malloc(sizeof(SoundInfo))) == NULL)
		return (SI);
	SI = (SoundInfo *)SI_aux;

	SI->SoundFreq = 0;
	SI->SoundBits = 0;
	SI->SoundSize = 0;
	SI->SoundData = NULL;
	SI->sample = &fakesample;

	/* Don't waste memory if Nosound */
	judas_error = JUDAS_OK;

	/*
	if (judas_device == DEV_NOSOUND)
	{
		return(SI);
	}
*/

	if ((temp = (char *)malloc(length)) == NULL) {
		return (SI);
	}
	smp = judas_allocsample(length * 2);
	if (!smp) {
		return (SI);
	}
	if (end == 0)
		end = length * 2;
	if (end > length * 2)
		end = length * 2;
	if (repeat > length * 2 - 1)
		repeat = length * 2 - 1;
	judas_error = JUDAS_READ_ERROR;
	memcpy(temp, FileBuffer, length);
	short_ptr = (short *)smp->start;
	for (con = 0; con < length; con++)
		short_ptr[con] = (short)(((*(temp + con)) + 0x80) * 0x100);
	free(temp);
	smp->repeat = smp->start + repeat;
	smp->end = smp->start + end;
	smp->voicemode = voicemode | VM_ON | VM_16BIT;
	judas_ipcorrect(smp);
	judas_error = JUDAS_OK;

	SI->SoundFreq = 11025;
	SI->SoundBits = 8;
	SI->SoundSize = length;
	SI->SoundData = (short *)smp->start;
	SI->sample = smp;

	return (SI);
}
