#ifndef _DIV_SDL_MIXER_SHIM_H
#define _DIV_SDL_MIXER_SHIM_H

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDL_MIXER_MAJOR_VERSION 1
#define SDL_MIXER_MINOR_VERSION 2
#define SDL_MIXER_PATCHLEVEL 12
#define SDL_MIXER_VERSION(X) \
	do { \
		(X)->major = SDL_MIXER_MAJOR_VERSION; \
		(X)->minor = SDL_MIXER_MINOR_VERSION; \
		(X)->patch = SDL_MIXER_PATCHLEVEL; \
	} while (0)

#define MIX_INIT_FLAC 0x00000001
#define MIX_INIT_MOD 0x00000002
#define MIX_INIT_MP3 0x00000008
#define MIX_INIT_OGG 0x00000010

#define AUDIO_U8 0x0008
#define AUDIO_S16LSB 0x8010
#define AUDIO_S16MSB 0x9010
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_S16SYS AUDIO_S16LSB
#else
#define AUDIO_S16SYS AUDIO_S16MSB
#endif

typedef struct Mix_Chunk {
	int allocated;
	Uint8 *abuf;
	Uint32 alen;
	Uint8 volume;
} Mix_Chunk;

typedef struct Mix_Music Mix_Music;

typedef void (*Mix_EffectFunc_t)(int chan, void *stream, int len, void *udata);
typedef void (*Mix_EffectDone_t)(int chan, void *udata);
typedef void (*Mix_Channel_Finished_t)(int channel);

int Mix_Init(int flags);
const char *Mix_GetError(void);
const SDL_version *Mix_Linked_Version(void);
int Mix_OpenAudio(int frequency, Uint16 format, int channels, int chunksize);
void Mix_CloseAudio(void);
void Mix_Quit(void);
int Mix_VolumeMusic(int volume);
int Mix_AllocateChannels(int numchans);
Mix_Chunk *Mix_LoadWAV_RW(SDL_RWops *src, int freesrc);
void Mix_FreeChunk(Mix_Chunk *chunk);
int Mix_PlayChannel(int channel, Mix_Chunk *chunk, int loops);
int Mix_Playing(int channel);
int Mix_HaltChannel(int channel);
int Mix_Volume(int channel, int volume);
int Mix_SetPanning(int channel, Uint8 left, Uint8 right);
int Mix_RegisterEffect(int chan, Mix_EffectFunc_t f, Mix_EffectDone_t d, void *arg);
int Mix_UnregisterAllEffects(int channel);
void Mix_ChannelFinished(Mix_Channel_Finished_t channel_finished);
Mix_Music *Mix_LoadMUS_RW(SDL_RWops *src);
void Mix_FreeMusic(Mix_Music *music);
int Mix_PlayMusic(Mix_Music *music, int loops);
int Mix_HaltMusic(void);
int Mix_PlayingMusic(void);

#ifdef __cplusplus
}
#endif

#endif
