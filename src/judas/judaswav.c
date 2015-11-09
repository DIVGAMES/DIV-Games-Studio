/*
 * JUDAS WAV handling
 */

#include <stdlib.h>
#include <stdio.h>
//#include <mem.h>
//#include <io.h>
#include <fcntl.h>
#include "judas.h"
#include "osdep.h"

extern char *filewriterbuffer;
extern int filewriterbuffersize;
extern unsigned char judas_initialized;
extern void safemixer(void *address, int length);

int JudasProgressRead(int handle, void *buffer, int size);

/*
#define max_textos 2048
int Progress(char *titulo,int current,int total);
extern char *texto[max_textos];
*/

#pragma pack (push, 1);
typedef struct
{
        char rifftext[4];
        unsigned totallength;
        char wavetext[4];
        char formattext[4];
        unsigned formatlength;
        unsigned short format;
        unsigned short channels;
        unsigned freq;
        unsigned avgbytes;
        unsigned short blockalign;
        unsigned short bits;
        char datatext[4];
        unsigned datalength;
} WAV_HEADER;
#pragma pack (pop);

/*
 * Opens a file for wav writing.
 * Returns nonnegative file handle if successful, -1 on error
 */
int judas_wavwriter_open(char *name)
{
  WAV_HEADER header;
  int handle;
  if (!judas_initialized) return -1;
  header.totallength = 0; // Will be determined after writing the pcm data
  header.datalength = 0; // Will be determined after writing the pcm data
  memcpy(header.rifftext, "RIFF", 4);
  memcpy(header.wavetext, "WAVE", 4);
  memcpy(header.formattext, "fmt ", 4);
  memcpy(header.datatext, "data", 4);
  header.formatlength = 16;
  header.format = 1;
  header.freq = judas_mixrate;
  switch (judas_mixmode) {
    case (MONO | EIGHTBIT):
      header.channels = 1;
      header.avgbytes = header.freq;
      header.blockalign = 1;
      header.bits = 8;
    break;
    case (MONO | SIXTEENBIT):
      header.channels = 1;
      header.avgbytes = header.freq * 2;
      header.blockalign = 2;
      header.bits = 16;
    break;
    case (STEREO | EIGHTBIT):
      header.channels = 2;
      header.avgbytes = header.freq * 2;
      header.blockalign = 2;
      header.bits = 8;
    break;
    case (STEREO | SIXTEENBIT):
      header.channels = 2;
      header.avgbytes = header.freq * 4;
      header.blockalign = 4;
      header.bits = 16;
    break;
  }
  handle = open(name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, S_IREAD | S_IWRITE);
  if (handle == -1) {
    return handle;
  }
  if (write(handle, &header, sizeof(WAV_HEADER)) < sizeof(WAV_HEADER)) {
    close(handle);
    return -1;
  }

  if (filewriterbuffer == NULL) {
    if (!(filewriterbuffer = locked_malloc(filewriterbuffersize))) return -1;
  }

  return handle;
}

/*
 * Wav writer. Writes one bufferlength of mixed data into the wav file.
 * Returns nonnegative file handle if successful, -1 on error
 */
int judas_wavwriter_writesome(int handle)
{
  if (!judas_initialized) return -1;
  if (handle == -1) return -1;

  safemixer(filewriterbuffer, filewriterbuffersize);
  if (write(handle, filewriterbuffer, filewriterbuffersize) < filewriterbuffersize) {
    close(handle);
    return -1;
  }
  return handle;
}

/*
 * Finishes wav writing and closes the wav writer file.
 * Returns 0 on success, -1 on error
 */
int judas_wavwriter_close(int handle)
{
  int totallength;
  int datalength;

  if (filewriterbuffer) locked_free(filewriterbuffer);

  totallength = filelength(handle);
  datalength = totallength - sizeof(WAV_HEADER);
  totallength -= 8;
  if (datalength < 0) {
    close(handle);
    return -1;
  }
  if (lseek(handle, 4, SEEK_SET) == -1) {
    close(handle);
    return -1;
  };
  if (write(handle, &totallength, 4) == -1) {
    close(handle);
    return -1;
  }
  if (lseek(handle, sizeof(WAV_HEADER)-4, SEEK_SET) == -1) {
    close(handle);
    return -1;
  };
  if (write(handle, &datalength, 4) == -1) {
    close(handle);
    return -1;
  }
  return close(handle);
}

SoundInfo *judas_loadwav(char *name)
{
        int length;
        int reallength;
        int handle;
        SAMPLE *smp;
        WAV_HEADER header;
        unsigned char *SI_aux;
        SoundInfo *SI=NULL;
//      int con, pasos, resto;

        if((SI_aux=(char *)malloc(sizeof(SoundInfo)))==NULL) return(SI);
        SI=(SoundInfo *)SI_aux;

        SI->SoundFreq = 0;
        SI->SoundBits = 0;
        SI->SoundSize = 0;
        SI->SoundData = NULL;
        SI->sample    = &fakesample;

        /* Don't waste memory if Nosound */
        judas_error = JUDAS_OK;
/*
        if (judas_device == DEV_NOSOUND)
        {
                return(SI);
        }
*/
        /*
         * Try to open
         */
        judas_error = JUDAS_OPEN_ERROR;
        handle = judas_open(name);
        if (handle == -1) return(SI);

        /*
         * Read identification
         */
        judas_error = JUDAS_READ_ERROR;
        if (judas_read(handle, &header, 12) != 12)
        {
                judas_close(handle);
                return(SI);
        }
        judas_error = JUDAS_WRONG_FORMAT;
        if (memcmp("RIFF", header.rifftext, 4))
        {
                judas_close(handle);
                return(SI);
        }
        if (memcmp("WAVE", header.wavetext, 4))
        {
                judas_close(handle);
                return(SI);
        }
        /*
         * Search for the FORMAT chunk
         */
        for (;;)
        {
                judas_error = JUDAS_READ_ERROR;
                if (judas_read(handle, &header.formattext, 8) != 8)
                {
                        judas_close(handle);
                        return(SI);
                }
                if (!memcmp("fmt ", &header.formattext, 4)) break;
                if (judas_seek(handle, header.formatlength, SEEK_CUR) == -1)
                {
                        judas_close(handle);
                        return(SI);
                }
        }
        /*
         * Read in the FORMAT chunk
         */
        if (judas_read(handle, &header.format, 16) != 16)
        {
                judas_close(handle);
                return(SI);
        }
        /*
         * Skip data if the format chunk was bigger than what we use
         */
        if (judas_seek(handle, header.formatlength - 16, SEEK_CUR) == -1)
        {
                judas_close(handle);
                return(SI);
        }
        /*
         * Check for correct format
         */
        judas_error = JUDAS_WRONG_FORMAT;
        if (header.format != 1)
        {
                judas_close(handle);
                return(SI);
        }
        /*
         * Search for the DATA chunk
         */
        for (;;)
        {
                judas_error = JUDAS_READ_ERROR;
                if (judas_read(handle, &header.datatext, 8) != 8)
                {
                        judas_close(handle);
                        return(SI);
                }
                if (!memcmp("data", &header.datatext, 4)) break;
                if (judas_seek(handle, header.datalength, SEEK_CUR) == -1)
                {
                        judas_close(handle);
                        return(SI);
                }
        }
        /*
         * Allocate sample, load audio data, do processing (unsigned->signed,
         * stereo->mono)
         */
        length = header.datalength;
        reallength = length;
        if (header.channels == 2) reallength >>= 1;
        if (header.bits     == 8) reallength <<= 1;
        smp = judas_allocsample(reallength);
        if (!smp)
        {
                judas_close(handle);
                return(SI);
        }
        if (header.channels == 2) // Stereo
        {
                if (header.bits == 16)
                {
                        unsigned count = length >> 2;
                        short    *buffer;
                        short    *src;
                        short    *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                judas_close(handle);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        if(length<500000)
                        {
                          if (judas_read(handle, buffer, length) != length)
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        else
                        {
                          if (!JudasProgressRead(handle, buffer, length))
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        src  = buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                int average = (src[0] + src[1]) / 2;
                                *dest = average;
                                src += 2;
                                dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
                else
                {
                        unsigned      count = length >> 1;
                        short         *buffer;
                        unsigned char *src;
                        short         *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                judas_close(handle);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        if(length<500000)
                        {
                          if (judas_read(handle, buffer, length) != length)
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        else
                        {
                          if (!JudasProgressRead(handle, buffer, length))
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        src = (unsigned char *)buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                short average;
                                average = (short)((((src[0]+src[1])/2)+0x80)*0x100);
                                *dest = average;
                                src += 2;
                                dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
        }
        else // Mono
        {
                if (header.bits == 16)
                {
                        judas_error = JUDAS_READ_ERROR;
                        if(length<500000)
                        {
                          if (judas_read(handle, smp->start, length) != length)
                          {
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        else
                        {
                          if (!JudasProgressRead(handle, smp->start, length))
                          {
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
                else
                {
                        unsigned      count = length;
                        short         *buffer;
                        unsigned char *src;
                        short         *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                judas_close(handle);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        if(length<500000)
                        {
                          if (judas_read(handle, buffer, length) != length)
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        else
                        {
                          if (!JudasProgressRead(handle, buffer, length))
                          {
                                  free(buffer);
                                  judas_freesample(smp);
                                  judas_close(handle);
                                  return(SI);
                          }
                        }
                        src  = (unsigned char *)buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                *dest = (short)(*src+0x80)*0x100;
                                src++; dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
        }
        judas_ipcorrect(smp);
        judas_error = JUDAS_OK;
        judas_close(handle);

        SI->SoundFreq = header.freq;
        SI->SoundBits = header.bits;
        SI->SoundSize = reallength/2;
        SI->SoundData = (short *)smp->start;
        SI->sample    = smp;

        return(SI);
}

SoundInfo *judas_loadwav_mem(unsigned char *FileBuffer)
{
        int length;
        int reallength;
        SAMPLE *smp;
        WAV_HEADER header;
        unsigned char *SI_aux;
        SoundInfo *SI=NULL;
//      int con, pasos, resto;
        int FilePos=0;

        if((SI_aux=(char *)malloc(sizeof(SoundInfo)))==NULL) return(SI);
        SI=(SoundInfo *)SI_aux;

        SI->SoundFreq = 0;
        SI->SoundBits = 0;
        SI->SoundSize = 0;
        SI->SoundData = NULL;
        SI->sample    = &fakesample;

        /* Don't waste memory if Nosound */
        judas_error = JUDAS_OK;
/*
        if (judas_device == DEV_NOSOUND)
        {
                return(SI);
        }
*/
        /*
         * Read identification
         */
        judas_error = JUDAS_READ_ERROR;
        memcpy(&header, FileBuffer+FilePos, 12); FilePos+=12;
        judas_error = JUDAS_WRONG_FORMAT;
        if (memcmp("RIFF", header.rifftext, 4))
        {
                return(SI);
        }
        if (memcmp("WAVE", header.wavetext, 4))
        {
                return(SI);
        }
        /*
         * Search for the FORMAT chunk
         */
        for (;;)
        {
                judas_error = JUDAS_READ_ERROR;
                memcpy(header.formattext, FileBuffer+FilePos, 8); FilePos+=8;
                if (!memcmp("fmt ", &header.formattext, 4)) break;
                FilePos+=header.formatlength;
        }
        /*
         * Read in the FORMAT chunk
         */
        memcpy(&header.format, FileBuffer+FilePos, 16); FilePos+=16;
        /*
         * Skip data if the format chunk was bigger than what we use
         */
        FilePos+=(header.formatlength - 16);
        /*
         * Check for correct format
         */
        judas_error = JUDAS_WRONG_FORMAT;
        if (header.format != 1)
        {
                return(SI);
        }
        /*
         * Search for the DATA chunk
         */
        for (;;)
        {
                judas_error = JUDAS_READ_ERROR;
                memcpy(header.datatext, FileBuffer+FilePos, 8); FilePos+=8;
                if (!memcmp("data", &header.datatext, 4)) break;
                FilePos+=header.datalength;
        }
        /*
         * Allocate sample, load audio data, do processing (unsigned->signed,
         * stereo->mono)
         */
        length = header.datalength;
        reallength = length;
        if (header.channels == 2) reallength >>= 1;
        if (header.bits     == 8) reallength <<= 1;
        smp = judas_allocsample(reallength);
        if (!smp)
        {
                return(SI);
        }
        if (header.channels == 2)
        {
                if (header.bits == 16)
                {
                        unsigned count = length >> 2;
                        short    *buffer;
                        short    *src;
                        short    *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        memcpy(buffer, FileBuffer+FilePos, length); FilePos+=length;
                        src  = buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                int average = (src[0] + src[1]) / 2;
                                *dest = average;
                                src += 2;
                                dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
                else
                {
                        unsigned      count = length >> 1;
                        short         *buffer;
                        unsigned char *src;
                        short         *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        memcpy(buffer, FileBuffer+FilePos, length); FilePos+=length;
                        src = (unsigned char *)buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                short average;
                                average = (short)((((src[0]+src[1])/2)+0x80)*0x100);
                                *dest = average;
                                src += 2;
                                dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
        }
        else
        {
                if (header.bits == 16)
                {
                        judas_error = JUDAS_READ_ERROR;
                        memcpy(smp->start, FileBuffer+FilePos, length); FilePos+=length;
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
                else
                {
                        unsigned      count = length;
                        short         *buffer;
                        unsigned char *src;
                        short         *dest;

                        judas_error = JUDAS_OUT_OF_MEMORY;
                        if((buffer=(short *)malloc(length))==NULL)
                        {
                                judas_freesample(smp);
                                return(SI);
                        }
                        judas_error = JUDAS_READ_ERROR;
                        memcpy(buffer, FileBuffer+FilePos, length); FilePos+=length;
                        src  = (unsigned char *)buffer;
                        dest = (short *)smp->start;
                        while (count--)
                        {
                                *dest = (short)(*src+0x80)*0x100;
                                src++; dest++;
                        }
                        free(buffer);
                        smp->repeat = smp->start;
                        smp->end = smp->start + reallength;
                        smp->voicemode = VM_ON | VM_16BIT;
                }
        }
        judas_ipcorrect(smp);
        judas_error = JUDAS_OK;

        SI->SoundFreq = header.freq;
        SI->SoundBits = header.bits;
        SI->SoundSize = reallength/2;
        SI->SoundData = (short *)smp->start;
        SI->sample    = smp;

        return(SI);
}

