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


#ifndef _VID_H
#define _VID_H

#include "pstypes.h"


void vid_close(void);
int vid_set_mode(uint32_t mode);
int vid_init(void);
int vid_check_mode(uint32_t mode);

extern uint32_t Vid_current_mode;

extern void vid_update(void);

/*
 * currently SDL and OGL are the only things that supports toggling
 * fullscreen.  otherwise add other checks to the #if -MPM
 */
#if (defined(SDL_VIDEO) || defined(OGL))
#define VID_SUPPORTS_FULLSCREEN_TOGGLE

/*
 * must return 0 if windowed, 1 if fullscreen
 */
int vid_check_fullscreen(void);

/*
 * returns state after toggling (ie, same as if you had called
 * check_fullscreen immediatly after)
 */
int vid_toggle_fullscreen(void);

#endif /* defined(SDL_VIDEO) || defined(OGL)) */

/* currently only OGL can toggle in the menus, because its screen data
 * is not used (and stays in the same place).  whereas software modes,
 * toggling fullscreen would very likely make the data buffer point to
 * a different location, and all the subbitmaps of it would have
 * invalid addresses in them. */
#ifdef OGL
#define VID_SUPPORTS_FULLSCREEN_MENU_TOGGLE
#endif

/*
 * returns state after toggling (ie, same as if you had called check_fullscreen immediately after)
 */
int vid_toggle_fullscreen_menu(void);


#endif
