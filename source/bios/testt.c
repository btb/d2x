/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#include <stdio.h>
#if defined(__DOS__) || defined(WINDOWS)
#include <i86.h>
#else
#include <SDL.h>
#endif

#include "timer.h"
// #include "key.h"
#include "fix.h"

#if defined(__DOS__) || defined(WINDOWS)
#define TICKER (*(volatile unsigned int *)0x46C)
#else
#define TICKER (SDL_GetPerformanceCounter())
#endif

int main(void)
{
   fix MyTimer, t, t1, t2, ot;
#if defined(__DOS__) || defined(WINDOWS)
   int s;
#else
   Uint64 s;
   Uint64 freq = SDL_GetPerformanceFrequency();
#endif

   // key_init();

#ifdef __DOS__
   //timer_init( 4661, NULL );
   timer_init( 0, NULL );
#else
   timer_init();
#endif

   s = TICKER;
   MyTimer = timer_get_fixed_seconds();

   t1 = 0;
   while(t1 < 655360)
   {
      delay(1);
      ot =t;
      t = timer_get_fixed_seconds();
#if defined(__DOS__) || defined(WINDOWS)
      t1 = (TICKER - s)*655360/182;
#else
      t1 = fl2f((float)(TICKER - s) / (float)freq);
#endif
      //printf( "%d\t%d\t%u\t%d\t%u\t%u\t%u\n", (TICKER-s)*10000/182, myc, t, (int)t-(int)ot, key_down_time(KEY_G), key_down_count(KEY_G), key_up_count(KEY_G) );
      printf( "%u\t%u\t%d\n", t1, t, (int)t - (int)t1 );
   }

   t1 = timer_get_fixed_seconds();
   delay(100);
   t2 = timer_get_fixed_seconds();

   printf( "This number should be about %d:  %d\n", F0_1, t2-t1 );

   t1 = timer_get_fixed_seconds();
   t2 = timer_get_fixed_seconds();

   printf( "Overhead for 'timer_get_fixed_seconds' call:  %f Seconds\n", f2fl(t2-t1) );


   timer_close();

   return 0;
}
