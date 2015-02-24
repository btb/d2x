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

/*
 *
 * Header file for Inferno.  Should be included in all source files.
 *
 */

#ifndef _INFERNO_H
#define _INFERNO_H

#include "pstypes.h"

// MACRO for single line #ifdef WINDOWS #else DOS
#ifdef WINDOWS
#define WIN(x) x
#else
#define WIN(x)
#endif

#ifdef MACINTOSH
#define MAC(x) x
#else
#define MAC(x)
#endif

#define STRINGIFY(x) #x


/**
 **	Constants
 **/

// How close two points must be in all dimensions to be considered the
// same point.
#define	FIX_EPSILON	10

// the maximum length of a filename
#define FILENAME_LEN 13

//for Function_mode variable
#define FMODE_EXIT		0		// leaving the program
#define FMODE_MENU		1		// Using the menu
#define FMODE_GAME		2		// running the game
#define FMODE_EDITOR	3		// running the editor

// This constant doesn't really belong here, but it is because of
// horrible circular dependencies involving object.h, aistruct.h,
// polyobj.h, & robot.h
#define MAX_SUBMODELS	10		// how many animating sub-objects per model


#include "ai.h"
#include "aistruct.h"
#include "automap.h"
#include "bm.h"
#ifdef EDITOR
#include "bmread.h"
#endif
#include "cli.h"
#include "cmd.h"
#include "cntrlcen.h"
#include "collide.h"
#include "config.h"
#include "console.h"
#include "controls.h"
#include "credits.h"
#include "digi.h"
#include "effects.h"
#include "endlevel.h"
#include "entity.h"
#include "escort.h"
#include "fuelcen.h"
#include "fvi.h"
#include "gamefont.h"
#include "gamemine.h"
#include "gamepal.h"
#include "gamesave.h"
#include "gameseq.h"
#include "gauges.h"
#include "hostage.h"
#include "hudmsg.h"
#include "fireball.h"
#include "joydefs.h"
#include "kconfig.h"
#include "laser.h"
#include "lighting.h"
#include "menu.h"
#include "morph.h"
#include "movie.h"
#ifdef NETWORK
#include "kmatrix.h"
#include "modem.h"
#include "multi.h"
#include "multibot.h"
#include "netmisc.h"
#include "network.h"
#endif
#include "newdemo.h"
#include "newmenu.h"
#include "object.h"
#include "paging.h"
#include "physics.h"
#include "polyobj.h"
#include "powerup.h"
#include "render.h"
#include "reorder.h"
#include "robot.h"
#include "scores.h"
#include "segment.h"
#include "segpoint.h"
#include "songs.h"
#include "screens.h"
#include "slew.h"
#include "state.h"
#include "switch.h"
#include "terrain.h"
#include "text.h"
#include "texmerge.h"
#include "textures.h"
#include "titles.h"
#include "vers_id.h"
#include "wall.h"


/**
 **	Global variables
 **/

extern int Function_mode;		// in game or editor?
extern int Screen_mode;			// editor screen or game screen?

// The version number of the game
extern ubyte Version_major, Version_minor;

#ifdef MACINTOSH
extern ubyte Version_fix;
#endif

/**
 **	Functions
 **/

void quit_request();


#endif


