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
 * prototype definitions for descent.cfg reading/writing
 *
 */


#ifndef _CONFIG_H
#define _CONFIG_H

#include "player.h"
#include "cvar.h"

extern int ReadConfigFile(void);
extern int WriteConfigFile(void);

extern cvar_t config_last_player;
extern cvar_t config_last_mission;

extern cvar_t Config_digi_volume;
extern cvar_t Config_midi_volume;
#ifdef MACINTOSH
typedef struct ConfigInfoStruct
{
	ubyte	mDoNotDisplayOptions;
	ubyte	mUse11kSounds;
	ubyte	mDisableSound;
	ubyte	mDisableMIDIMusic;
	ubyte	mChangeResolution;
	ubyte	mDoNotPlayMovies;
	ubyte	mUserChoseQuit;
	ubyte	mGameMonitor;
	ubyte	mAcceleration;				// allow RAVE level acceleration
	ubyte	mInputSprockets;			// allow use of Input Sprocket devices
} ConfigInfo;

extern ConfigInfo gConfigInfo;
extern ubyte Config_master_volume;
#endif
extern cvar_t Config_redbook_volume;
extern cvar_t Config_control_type;
extern cvar_t Config_channels_reversed;
extern cvar_t Config_joystick_sensitivity;

//values for Config_control_type
#define CONTROL_USING_JOYSTICK  1
#define CONTROL_USING_MOUSE     2

#endif
