/* $Id: mission.h,v 1.22 2004-10-24 12:46:49 schaffner Exp $ */
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
 * Header for mission.h
 *
 */

#ifndef _MISSION_H
#define _MISSION_H

#include "pstypes.h"

#define MAX_MISSIONS                    300
#define MAX_LEVELS_PER_MISSION          30
#define MAX_SECRET_LEVELS_PER_MISSION   6
#define MISSION_NAME_LEN                25

#define D1_MISSION_FILENAME             "descent"
#define D1_MISSION_NAME                 "Descent: First Strike"
#define D1_MISSION_HOGSIZE              6856701 // v1.4 - 1.5
#define D1_10_MISSION_HOGSIZE           7261423 // v1.0
#define D1_MAC_MISSION_HOGSIZE          7456179
#define D1_OEM_MISSION_NAME             "Destination Saturn"
#define D1_OEM_MISSION_HOGSIZE          4492107 // v1.4a
#define D1_OEM_10_MISSION_HOGSIZE       4494862 // v1.0
#define D1_SHAREWARE_MISSION_NAME       "Descent Demo"
#define D1_SHAREWARE_MISSION_HOGSIZE    2339773 // v1.4
#define D1_SHAREWARE_10_MISSION_HOGSIZE 2365676 // v1.0 - 1.2
#define D1_MAC_SHARE_MISSION_HOGSIZE    3370339

#define SHAREWARE_MISSION_FILENAME  "d2demo"
#define SHAREWARE_MISSION_NAME      "Descent 2 Demo"
#define SHAREWARE_MISSION_HOGSIZE   2292566 // v1.0 (d2demo.hog)
#define MAC_SHARE_MISSION_HOGSIZE   4292746

#define OEM_MISSION_FILENAME        "d2"
#define OEM_MISSION_NAME            "D2 Destination:Quartzon"
#define OEM_MISSION_HOGSIZE         6132957 // v1.1

#define FULL_MISSION_FILENAME       "d2"
#define FULL_MISSION_HOGSIZE        7595079 // v1.1 - 1.2
#define FULL_10_MISSION_HOGSIZE     7107354 // v1.0
#define MAC_FULL_MISSION_HOGSIZE    7110007 // v1.1 - 1.2

//where the missions go
#if 1 // ndef EDITOR
#define MISSION_DIR "missions/"
#else
#define MISSION_DIR "./"
#endif

typedef struct {
	char	mission_name[MISSION_NAME_LEN+1];
	char	filename[9];	// filename
	ubyte	descent_version;	// descent 1 or descent 2?
	bool	anarchy_only_flag;	// if true, mission is only for anarchy
	int	builtin_hogsize;	// the size of the hogfile for a builtin mission, and 0 for an add-on mission
	char	briefing_text_filename[FILENAME_LEN]; // name of briefing file
	char	ending_text_filename[FILENAME_LEN]; // name of ending file
	ubyte	last_level;
	sbyte	last_secret_level;
	ubyte	n_secret_levels;
	ubyte	secret_level_table[MAX_SECRET_LEVELS_PER_MISSION]; // originating level no for each secret level 
	// arrays of names of the level files
	char	level_names[MAX_LEVELS_PER_MISSION][FILENAME_LEN];
	char	secret_level_names[MAX_SECRET_LEVELS_PER_MISSION][FILENAME_LEN];
#if 0 //def EDITOR	Support for multiple levels, briefings etc open at once
	Window	window;
	Window	attributes;// Window for changing them
	ubyte	enhanced;
	void	*briefing;
	void	*ending;
	Level	*level[MAX_LEVELS_PER_MISSION];
	Level	*secLevel[MAX_SECRET_LEVELS_PER_MISSION];
	void	*others[MAX_HOGFILES];
	char	Other_file_names[MAX_HOGFILES][FILENAME_LEN];
#endif
} Mission;

extern Mission *Current_mission; // current mission

#define Current_mission_longname	Current_mission->mission_name
#define Current_mission_filename	Current_mission->filename
#define Briefing_text_filename		Current_mission->briefing_text_filename
#define Ending_text_filename		Current_mission->ending_text_filename
#define Last_level			Current_mission->last_level
#define Last_secret_level		Current_mission->last_secret_level
#define N_secret_levels			Current_mission->n_secret_levels
#define Secret_level_table		Current_mission->secret_level_table
#define Level_names			Current_mission->level_names
#define Secret_level_names		Current_mission->secret_level_names

#define is_SHAREWARE (Current_mission->builtin_hogsize == SHAREWARE_MISSION_HOGSIZE)
#define is_MAC_SHARE (Current_mission->builtin_hogsize == MAC_SHARE_MISSION_HOGSIZE)
#define is_D2_OEM (Current_mission->builtin_hogsize == OEM_MISSION_HOGSIZE)

#define PLAYING_BUILTIN_MISSION	(Current_mission->builtin_hogsize != 0)
#define EMULATING_D1		(Current_mission->descent_version == 1)
#define ANARCHY_ONLY_MISSION	(Current_mission->anarchy_only_flag == 1)

//loads the named mission if it exists.
//Returns true if mission loaded ok, else false.
int load_mission_by_name (char *mission_name);

//Handles creating and selecting from the mission list.
//Returns 1 if a mission was loaded.
int select_mission (int anarchy_mode, char *message);

#endif
