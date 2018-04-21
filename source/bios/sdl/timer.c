/*
 *
 * SDL library timer functions
 *
 */

#include <SDL.h>

#include "fix.h"
#include "timer.h"


void timer_close(void)
{
	if (!SDL_WasInit(SDL_INIT_TIMER))
		return;

	SDL_QuitSubSystem(SDL_INIT_TIMER);
}

void timer_init(void)
{
	if (SDL_WasInit(SDL_INIT_TIMER))
		return;

	SDL_InitSubSystem(SDL_INIT_TIMER);
}

fix timer_get_fixed_seconds(void)
{
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

fix timer_get_fixed_secondsX(void)
{
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

fix timer_get_approx_seconds(void)
{
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

void timer_delay(fix seconds)
{
	Uint32 numticks = f2i(fixmul(seconds, i2f(1000)));

	SDL_Delay(numticks);
}

void delay(unsigned int milliseconds)
{
	SDL_Delay(milliseconds);
}
