/*
 * $Source: /cvs/cvsroot/d2x/arch/sdl_init.c,v $
 * $Revision: 1.4 $
 * $Author: bradleyb $
 * $Date: 2001-01-29 13:35:09 $
 *
 * SDL architecture support
 *
 * $Log: not supported by cvs2svn $
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include "text.h"
#include "event.h"
#include "error.h"
#include "args.h"
#include "digi.h"

extern void d_mouse_init();

void sdl_close()
{
	SDL_Quit();
}

void arch_sdl_init()
{
 // Initialise the library
//edited on 01/03/99 by Matt Mueller - if we use SDL_INIT_EVERYTHING, cdrom is initialized even if -nocdaudio is used
#ifdef SDL_INPUT
 if (!FindArg("-nomouse"))
   d_mouse_init();
#endif
 if (!FindArg("-nosound"))
   digi_init();
 atexit(sdl_close);
}
