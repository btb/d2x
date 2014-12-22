/* $Id: songs.c,v 1.14 2004-08-29 17:57:23 schaffner Exp $ */
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
 * Routines to manage the songs in Descent.
 *
 */


#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#if !defined(_MSC_VER) && !defined(macintosh)
#include <unistd.h>
#endif

#include "inferno.h"
#include "error.h"
#include "pstypes.h"
#include "args.h"
#include "songs.h"
#include "mono.h"
#include "cfile.h"
#include "digi.h"
#include "rbaudio.h"
#include "kconfig.h"
#include "timer.h"

song_info Songs[MAX_NUM_SONGS];
int Songs_initialized = 0;

#ifndef MACINTOSH
int Num_songs;
#endif

extern void digi_stop_current_song();

int Redbook_enabled = 1;

//0 if redbook is no playing, else the track number
int Redbook_playing = 0;

#define NumLevelSongs (Num_songs - SONG_FIRST_LEVEL_SONG)

extern int CD_blast_mixer();

#ifndef MACINTOSH
#define REDBOOK_VOLUME_SCALE  (255/3)		//255 is MAX
#else
#define REDBOOK_VOLUME_SCALE	(255)
#endif

//takes volume in range 0..8
void set_redbook_volume(int volume)
{
	#ifndef MACINTOSH
	RBASetVolume(0);		// makes the macs sound really funny
	#endif
	RBASetVolume(volume*REDBOOK_VOLUME_SCALE/8);
}

extern char CDROM_dir[];

void songs_init()
{
	int i;
	char inputline[80+1];
	CFILE * fp;

	if ( Songs_initialized ) return;


	#if !defined(MACINTOSH) && !defined(WINDOWS)  	// don't crank it if on a macintosh!!!!!
		if (!FindArg("-nomixer"))
			CD_blast_mixer();   // Crank it!
	#endif


	if (cfexist("descent.sng")) {   // mac (demo?) datafiles don't have the .sng file
		fp = cfopen( "descent.sng", "rb" );
		if ( fp == NULL )
		{
			Error( "Couldn't open descent.sng" );
		}
		i = 0;
		while (cfgets(inputline, 80, fp ))
		{
			if ( strlen( inputline ) )
			{
				Assert( i < MAX_NUM_SONGS );
				sscanf( inputline, "%s %s %s",
						Songs[i].filename,
						Songs[i].melodic_bank_file,
						Songs[i].drum_bank_file );
				//printf( "%d. '%s' '%s' '%s'\n",i,Songs[i].filename,Songs[i].melodic_bank_file,Songs[i].drum_bank_file );
				i++;
			}
		}
		Num_songs = i;
		if (Num_songs <= SONG_FIRST_LEVEL_SONG)
			Error("Must have at least %d songs",SONG_FIRST_LEVEL_SONG+1);
		cfclose(fp);
	}

	Songs_initialized = 1;

	//	RBA Hook
		if (FindArg("-noredbook"))
		{
			Redbook_enabled = 0;
		}
		else	// use redbook
		{
			#ifndef __MSDOS__ // defined(WINDOWS) || defined(MACINTOSH)
				RBAInit();
			#else
				RBAInit(toupper(CDROM_dir[0]) - 'A');
			#endif

				if (RBAEnabled())
			{
				set_redbook_volume(Config_redbook_volume);
				RBARegisterCD();
			}
		}
		atexit(RBAStop);    // stop song on exit
}

#define FADE_TIME (f1_0/2)

//stop the redbook, so we can read off the CD
void songs_stop_redbook(void)
{
	int old_volume = Config_redbook_volume*REDBOOK_VOLUME_SCALE/8;
	fix old_time = timer_get_fixed_seconds();

	if (Redbook_playing) {		//fade out volume
		int new_volume;
		do {
			fix t = timer_get_fixed_seconds();

			new_volume = fixmuldiv(old_volume,(FADE_TIME - (t-old_time)),FADE_TIME);

			if (new_volume < 0)
				new_volume = 0;

			RBASetVolume(new_volume);

		} while (new_volume > 0);
	}

	RBAStop();              	// Stop CD, if playing

	RBASetVolume(old_volume);	//restore volume

	Redbook_playing = 0;		

}

//stop any songs - midi or redbook - that are currently playing
void songs_stop_all(void)
{
	digi_stop_current_song();	// Stop midi song, if playing

	songs_stop_redbook();			// Stop CD, if playing
}

int force_rb_register=0;

void reinit_redbook()
{
	#ifndef __MSDOS__ // defined(WINDOWS) || defined(MACINTOSH)
		RBAInit();
	#else
		RBAInit(toupper(CDROM_dir[0]) - 'A');
	#endif

	if (RBAEnabled())
	{
		set_redbook_volume(Config_redbook_volume);
		RBARegisterCD();
		force_rb_register=0;
	}
}


//returns 1 if track started sucessfully
//start at tracknum.  if keep_playing set, play to end of disc.  else
//play only specified track
int play_redbook_track(int tracknum,int keep_playing)
{
	Redbook_playing = 0;

	if (!RBAEnabled() && Redbook_enabled && !FindArg("-noredbook"))
		reinit_redbook();

	if (force_rb_register) {
		RBARegisterCD();			//get new track list for new CD
		force_rb_register = 0;
	}

	if (Redbook_enabled && RBAEnabled()) {
		int num_tracks = RBAGetNumberOfTracks();
		if (tracknum <= num_tracks)
			if (RBAPlayTracks(tracknum,keep_playing?num_tracks:tracknum))  {
				Redbook_playing = tracknum;
			}
	}

	return (Redbook_playing != 0);
}


#if 0

#define REDBOOK_TITLE_TRACK         2
#define REDBOOK_CREDITS_TRACK       3
#define REDBOOK_FIRST_LEVEL_TRACK   (songs_haved2_cd()?4:1)

// songs_haved2_cd returns 1 if the descent 2 CD is in the drive and
// 0 otherwise
int songs_haved2_cd()
{
	char temp[128],cwd[128];
	
	getcwd(cwd, 128);

	strcpy(temp,CDROM_dir);

	#ifndef MACINTOSH		//for PC, strip of trailing slash
	if (temp[strlen(temp)-1] == '\\')
		temp[strlen(temp)-1] = 0;
	#endif

	if ( !chdir(temp) ) {
		chdir(cwd);
		return 1;
	}

	return 0;
}

#else


/* Redbook versions of 13 songs from Descent 1 as found on the Macintosh version.
   All the same tracklist, but some versions have tracks mixed to different lengths
 1:  Data
 2:  Primitive Rage
 3:  Outer Limits
 4:  The Escape (aka Close Call)
 5:  Ether in the Air (aka The Darkness of Space)
 6:  Robotic Menace (aka Get It On)
 7:  Virtual Tension (aka Fight)
 8:  Time for the Big Guns (aka Death Lurks Beneath)
 9:  Mystery Metal (aka C-4 Home Recipe)
 10: Hydraulic Pressure (aka Escape)
 11: Not That Button! (aka Backwards Time)
 12: Industrial Accident (aka Crazyfactory)
 13: Overdrive (aka Machine Gun)
 14: A Big Problem (aka Insanity)
 */
#define D1_DISCID_1         0xb60d990e
#define D1_DISCID_2         0xde0feb0e
#define D1_DISCID_3         0xb70ee40e

#define D1_RB_TITLE             2
#define D1_RB_BRIEFING          3
#define D1_RB_ENDLEVEL          4
#define D1_RB_ENDGAME           3
#define D1_RB_CREDITS           5
#define D1_RB_FIRST_LEVEL_SONG  6

/* Descent II
 1:  Data
 2:  Title
 3:  Crawl
 4:  Glut
 5:  Gunner Down
 6:  Cold Reality
 7:  Ratzez
 8:  Crush
 9:  Untitled
 10: Haunted (Instrumental Remix)
 11: Are You Descent?
 12: Techno Industry
 13: Robot Jungle
 */
#define D2_DISCID_1         0x22115710 // Mac version, has some extended versions and 3 bonus tracks
#define D2_DISCID_2         0xac0bc30d
#define D2_DISCID_3         0xc40c0a0d
#define D2_DISCID_4         0xc610080d
#define D2_DISCID_5         0xcc101b0d
#define D2_DISCID_6         0xd00bf30d
#define D2_DISCID_7         0xd2101d0d
#define D2_DISCID_8         0xd410070d
#define D2_DISCID_9         0xda10370d

#define D2_RB_TITLE            2
#define D2_RB_CREDITS          3
#define D2_RB_FIRST_LEVEL_SONG 4

/* Same as above, but all tracks shifted by one
 1:  Data
 2:  Data
 3:  Title
 4:  Crawl
 5:  Glut
 6:  Gunner Down
 7:  Cold Reality
 8:  Ratzez
 9:  Crush
 10: Untitled
 11: Haunted (Instrumental Remix)
 12: Are You Descent?
 13: Techno Industry
 14: Robot Jungle
 */
#define D2_2_DISCID_1       0xe010a30e

#define D2_2_RB_TITLE               3
#define D2_2_RB_CREDITS             4
#define D2_2_RB_FIRST_LEVEL_SONG    5

/* Descent II: The Infinite Abyss
 1:  Data
 2:  Title
 3:  Cold Reality - Extended Remix
 4:  Crawl - Extended Remix
 5:  Gunner Down - Extended Remix
 6:  Ratzez - Extended Remix
 7:  Techno Industry - Extended Remix
 8:  Are You Descent? - Extended Remix
 9:  Robot Jungle - Extended Remix
 */
#define D2_IA_DISCID_1      0x7d0ff809
#define D2_IA_DISCID_2      0x8110ec09
#define D2_IA_DISCID_3      0x82104909
#define D2_IA_DISCID_4      0x85101d09
#define D2_IA_DISCID_5      0x87102209

#define D2_IA_RB_TITLE              2
#define D2_IA_RB_CREDITS            3
#define D2_IA_RB_FIRST_LEVEL_SONG   4

/* Descent II: Vertigo Series
 1:  Data
 2:  Crush - Extended Remix
 3:  Glut - Extended Remix
 4:  Haunted - Instrumental Re-Remix
 5:  New Song #1
 6:  Untitled - Extended Remix
 7:  New Song #2
 8:  New Song #3
 */
#define D2_X_DISCID_1       0x53078208
#define D2_X_DISCID_2       0x64071408

#define D2_X_RB_FIRST_LEVEL_SONG    2


int songs_redbook_track(int songnum)
{
	uint32_t discid;

	if (!Redbook_enabled)
		return 0;

	discid = RBAGetDiscID();

	switch (discid) {
		case D1_DISCID_1:
		case D1_DISCID_2:
		case D1_DISCID_3:
			switch (songnum) {
				case SONG_TITLE:            return D1_RB_TITLE;
				case SONG_BRIEFING:         return D1_RB_BRIEFING;
				case SONG_ENDLEVEL:         return D1_RB_ENDLEVEL;
				case SONG_ENDGAME:          return D1_RB_ENDGAME;
				case SONG_CREDITS:          return D1_RB_CREDITS;
				case SONG_FIRST_LEVEL_SONG: return D1_RB_FIRST_LEVEL_SONG;
				default: Int3(); break;
			}
		case D2_DISCID_1:
		case D2_DISCID_2:
		case D2_DISCID_3:
		case D2_DISCID_4:
		case D2_DISCID_5:
		case D2_DISCID_6:
		case D2_DISCID_7:
		case D2_DISCID_8:
		case D2_DISCID_9:
			switch (songnum) {
				case SONG_TITLE:            return D2_RB_TITLE;
				case SONG_CREDITS:          return D2_RB_CREDITS;
				case SONG_FIRST_LEVEL_SONG: return D2_RB_FIRST_LEVEL_SONG;
				default: Int3(); break;
			}
		case D2_2_DISCID_1:
			switch (songnum) {
				case SONG_TITLE:            return D2_2_RB_TITLE;
				case SONG_CREDITS:          return D2_2_RB_CREDITS;
				case SONG_FIRST_LEVEL_SONG: return D2_2_RB_FIRST_LEVEL_SONG;
				default: Int3(); break;
			}
		case D2_IA_DISCID_1:
		case D2_IA_DISCID_2:
		case D2_IA_DISCID_3:
		case D2_IA_DISCID_4:
		case D2_IA_DISCID_5:
			switch (songnum) {
				case SONG_TITLE:            return D2_IA_RB_TITLE;
				case SONG_CREDITS:          return D2_IA_RB_CREDITS;
				case SONG_FIRST_LEVEL_SONG: return D2_IA_RB_FIRST_LEVEL_SONG;
				default: Int3(); break;
			}
		case D2_X_DISCID_1:
		case D2_X_DISCID_2:
			return D2_X_RB_FIRST_LEVEL_SONG;

		default:
			con_printf(CON_DEBUG, "Unknown CD. discid: %x\n", discid);
			return 1;
	}
	return 1;
}

#define REDBOOK_TITLE_TRACK         (songs_redbook_track(SONG_TITLE))
#define REDBOOK_CREDITS_TRACK       (songs_redbook_track(SONG_CREDITS))
#define REDBOOK_FIRST_LEVEL_TRACK   (songs_redbook_track(SONG_FIRST_LEVEL_SONG))

#endif


void songs_play_song( int songnum, int repeat )
{
	#ifndef SHAREWARE
	//Assert(songnum != SONG_ENDLEVEL && songnum != SONG_ENDGAME);	//not in full version
	#endif

	if ( !Songs_initialized )
		songs_init();

	//stop any music already playing

	songs_stop_all();

	//do we want any of these to be redbook songs?

	if (force_rb_register) {
		RBARegisterCD();			//get new track list for new CD
		force_rb_register = 0;
	}

	if (songnum == SONG_TITLE)
		play_redbook_track(REDBOOK_TITLE_TRACK,0);
	else if (songnum == SONG_CREDITS)
		play_redbook_track(REDBOOK_CREDITS_TRACK,0);

	if (!Redbook_playing) {		//not playing redbook, so play midi

		#ifndef MACINTOSH
			digi_play_midi_song( Songs[songnum].filename, Songs[songnum].melodic_bank_file, Songs[songnum].drum_bank_file, repeat );
		#else
			digi_play_midi_song(songnum, repeat);
		#endif
	}
}

int current_song_level;

void songs_play_level_song( int levelnum )
{
	int songnum;
	int n_tracks;

	Assert( levelnum != 0 );

	if ( !Songs_initialized )
		songs_init();

	songs_stop_all();

	current_song_level = levelnum;

	songnum = (levelnum>0)?(levelnum-1):(-levelnum);

	if (!RBAEnabled() && Redbook_enabled && !FindArg("-noredbook"))
		reinit_redbook();

	if (force_rb_register) {
		RBARegisterCD();			//get new track list for new CD
		force_rb_register = 0;
	}

	if (Redbook_enabled && RBAEnabled() && (n_tracks = RBAGetNumberOfTracks()) > 1) {

		//try to play redbook

		mprintf((0,"n_tracks = %d\n",n_tracks));

		play_redbook_track(REDBOOK_FIRST_LEVEL_TRACK + (songnum % (n_tracks-REDBOOK_FIRST_LEVEL_TRACK+1)),1);
	}

	if (! Redbook_playing) {			//not playing redbook, so play midi

		songnum = SONG_FIRST_LEVEL_SONG + (songnum % NumLevelSongs);

		#ifndef MACINTOSH
			digi_play_midi_song( Songs[songnum].filename, Songs[songnum].melodic_bank_file, Songs[songnum].drum_bank_file, 1 );
		#else
			digi_play_midi_song( songnum, 1 );
		#endif

	}
}

//this should be called regularly to check for redbook restart
void songs_check_redbook_repeat()
{
	static fix last_check_time;
	fix current_time;

	if (!Redbook_playing || Config_redbook_volume==0) return;

	current_time = timer_get_fixed_seconds();
	if (current_time < last_check_time || (current_time - last_check_time) >= F2_0) {
		if (!RBAPeekPlayStatus()) {
			stop_time();
			// if title ends, start credit music
			// if credits music ends, restart it
			if (Redbook_playing == REDBOOK_TITLE_TRACK || Redbook_playing == REDBOOK_CREDITS_TRACK)
				play_redbook_track(REDBOOK_CREDITS_TRACK,0);
			else {
				//songs_goto_next_song();
	
				//new code plays all tracks to end of disk, so if disk has
				//stopped we must be at end.  So start again with level 1 song.
	
				songs_play_level_song(1);
			}
			start_time();
		}
		last_check_time = current_time;
	}
}

//goto the next level song
void songs_goto_next_song()
{
	if (Redbook_playing) 		//get correct track
		current_song_level = RBAGetTrackNum() - REDBOOK_FIRST_LEVEL_TRACK + 1;

	songs_play_level_song(current_song_level+1);

}

//goto the previous level song
void songs_goto_prev_song()
{
	if (Redbook_playing) 		//get correct track
		current_song_level = RBAGetTrackNum() - REDBOOK_FIRST_LEVEL_TRACK + 1;

	if (current_song_level > 1)
		songs_play_level_song(current_song_level-1);

}

