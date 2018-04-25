/*
 *
 * SDL library timer functions
 *
 */

#include <SDL.h>

#include "fix.h"
#include "timer.h"
#include "error.h"


void timer_close(void)
{
	Int3();
	if (!SDL_WasInit(SDL_INIT_TIMER))
		return;

	SDL_QuitSubSystem(SDL_INIT_TIMER);
}

void timer_init(void)
{
	Int3();
	if (SDL_WasInit(SDL_INIT_TIMER))
		return;

	SDL_InitSubSystem(SDL_INIT_TIMER);
}

fix timer_get_fixed_seconds(void)
{
	Int3();
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

fix timer_get_fixed_secondsX(void)
{
	Int3();
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

fix timer_get_approx_seconds(void)
{
	Int3();
	fix val;
	Uint32 time;

	time = SDL_GetTicks();

	val = fixdiv(time, 1000);

	return val;
}

void timer_delay(fix seconds)
{
	Int3();
	Uint32 numticks = f2i(fixmul(seconds, i2f(1000)));

	SDL_Delay(numticks);
}

void delay(unsigned int milliseconds)
{
	Int3();
	SDL_Delay(milliseconds);
}
