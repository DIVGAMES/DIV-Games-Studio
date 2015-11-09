 /*
   This file contains the C routines to access the game port and determine
   the status of the four joystick buttons and the position of the four
   joystick axes.

   If using an earlier version of Turbo C, __emit__() probably won't
   compile.  These are just used to insert NOPs into the code.  Try
   deleting them completely.

   These routines are designed to work independently of any support by the
   BIOS (int 15h, function 84h).
 */

#include <conio.h>
#include <dos.h>
#include "joy.h"

 /*
   Read status of button (range 0-3).
   Returns 1 if button is pushed, 0 if button is up, and -1 on error.
 */

int   get_joy_button(int button)
{
   /* check argument */
   if(button < 0  ||  button > 3)
      return(-1);

   /* get button status */
   if( inp(GAME_PORT) & (1 << (4 + button)) )
      return(0); //no pulsado
   else
      return(1); //pulsado
}

 /*
   Read joystick position axis (range 0-3).
   Returns -1 on error, 0 on timeout, and position on success.
 */

int   get_joy_position(int axis)
{
   unsigned    start;
   unsigned    finish;
   unsigned    result;
   int         i;
   int         mask;

   /* check argument */
   if(axis < 0  ||  axis > 3)
      return(-1);

   mask = 1 << axis;    /* compute mask for joystick axis bit     */

   _disable();           /* disable interrupts for accurate timing */

   /* read system timer value direct from timer chip         */
   outp(TIMER_PORT+3, 0);          /* latch value        */
   start  = inp(TIMER_PORT);       /* get low byte       */
   start += inp(TIMER_PORT) << 8;  /* merge in high byte */

   outp(GAME_PORT, 0);             /* trigger joystick   */

   /* wait for joystick bit to flip or else time out */
   for(i=0; i<TIME_OUT; i++)
      if( (inp(GAME_PORT) & mask) == 0 )
         break;

/*
   if(i == TIME_OUT)  // time out
      {
      _enable();      // re-enable interrupts
      return(0);      // indicate time out
      }
   else {             // else get value from system timer chip
*/

     outp(TIMER_PORT+3, 0);       /* sequence as before  */
     finish  = inp(TIMER_PORT);
     finish += inp(TIMER_PORT) << 8;

     _enable();      /* re-enable interrupts */

     if(start >= finish)
       result = start - finish;            /* compute difference         */
     else
       result = 0xffff - finish + start;   /* compute difference, w/wrap */
//   }

   // let other joystick timers run down
   for(i=0; i<TIME_OUT; i++)
      if( (inp(GAME_PORT) & 0x0f) == 0 ) // 0x0f: mask for all 4 axes
         break;

   return( (result & 0x1ff0) >> 4 );         // reproduce IBM-like results
}

