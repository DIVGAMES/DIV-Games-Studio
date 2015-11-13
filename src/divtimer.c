/*
 * Timer handler, calls a user function at the desired PIT clock frequency.
 *
 * Doesn't "officially" belong to JUDAS but feel free to use! This is
 * blasphemy-ware too!
 */

#include "global.h"

//#include <conio.h>
//#include <dos.h>
//#include <mem.h>
//#include "judas\judasmem.h"

int timer_init(unsigned short frequency, void (*function)());
void timer_uninit(void);
static int timer_lock(void);
static void timer_unlock(void);
void timer_handler(void) {}
unsigned short timer_get_ds(void);

#pragma aux timer_get_ds = \
"mov ax, ds" \
modify [ax] \
value [ax];

static unsigned char timer_initialized = 0;
extern void (__interrupt __far *timer_oldvect)();
void (__interrupt __far *timer_newvect)() = &timer_handler;
extern void (*timer_function)();
//extern unsigned timer_count;
extern unsigned short timer_frequency;
extern unsigned short timer_systemcount;
extern unsigned short timer_ds;
extern int timer_code_lock_start;
extern int timer_code_lock_end;

int timer_init(unsigned short frequency, void (*function)())
{
#ifdef NOTYET
	if (timer_initialized) return 1;
        if (!timer_lock()) return 0;
        timer_function = function;
        timer_count = 0;
        timer_systemcount = 0;
        timer_frequency = frequency;
        timer_ds = timer_get_ds();
        timer_oldvect = _dos_getvect(8);
        _disable();
        _dos_setvect(8, timer_newvect);
        outp(0x43, 0x34);
        outp(0x40, frequency);
        outp(0x40, frequency >> 8);
        _enable();
        timer_initialized = 1;
#endif
        return 1;
}

void timer_uninit(void)
{
#ifdef NOTYET
	if (!timer_initialized) return;
        _disable();
        _dos_setvect(8, timer_oldvect);
        outp(0x43, 0x34);
        outp(0x40, 0x00);
        outp(0x40, 0x00);
        _enable();
        timer_unlock();
        timer_initialized = 0;
#endif
}

static int timer_lock(void)
{
#ifdef NOTYET
        if (!judas_memlock(&timer_code_lock_start, (int)&timer_code_lock_end - (int)&timer_code_lock_start)) return 0;
#endif
        return 1;
}

static void timer_unlock(void)
{
#ifdef NOTYET
        judas_memunlock(&timer_code_lock_start, (int)&timer_code_lock_end - (int)&timer_code_lock_start);
#endif
}
