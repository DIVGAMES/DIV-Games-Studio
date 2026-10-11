#ifndef DIV_LR_H
#define DIV_LR_H

#include <stdint.h>

#define DIV_LR_MAX_W 1280
#define DIV_LR_MAX_H 1024

extern uint32_t div_lr_pixels[DIV_LR_MAX_W * DIV_LR_MAX_H];
extern int div_lr_width;
extern int div_lr_height;

int div_lr_boot(const char *path);
void div_lr_frame(void);
/* Add real elapsed time, in microseconds, to the DIV clock. */
void div_lr_add_usec(int64_t usec);
/* Advance the DIV clock by one game frame (1000/fps ms). */
void div_lr_advance_time(void);
/* Mix DIV sound effects into interleaved stereo S16 frames. */
void div_lr_mix_audio(int16_t *stereo, int frames);

#endif
