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
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

/*
 *
 * Include file for sound hardware.
 *
 */



#ifndef _DIGI_H
#define _DIGI_H

#include "pstypes.h"
#include "vecmat.h"

/*
#ifdef __DJGPP__
#define ALLEGRO
#endif
*/

#ifdef ALLEGRO
#include "allg_snd.h"
typedef SAMPLE digi_sound;
#else
typedef struct digi_sound       {
	int length;
	ubyte * data;
} digi_sound;
#endif

#define SAMPLE_RATE_11K         11025
#define SAMPLE_RATE_22K         22050



#ifdef __DJGPP__
extern int digi_driver_board;
extern cvar_t digi_driver_port;
extern cvar_t digi_driver_irq;
extern int digi_driver_dma;
extern int digi_midi_type;
extern cvar_t digi_midi_port;
#endif

extern int digi_sample_rate;

extern int digi_get_settings(void);
extern int digi_init(void);
extern void digi_reset(void);
extern void digi_close(void);

// Volume is max at F1_0.
extern void digi_play_sample( int sndnum, fix max_volume );
extern void digi_play_sample_once( int sndnum, fix max_volume );
extern int digi_link_sound_to_object( int soundnum, short objnum, int forever, fix max_volume );
extern int digi_link_sound_to_pos( int soundnum, short segnum, short sidenum, vms_vector * pos, int forever, fix max_volume );
// Same as above, but you pass the max distance sound can be heard.  The old way uses f1_0*256 for max_distance.
extern int digi_link_sound_to_object2( int soundnum, short objnum, int forever, fix max_volume, fix  max_distance );
extern int digi_link_sound_to_pos2( int soundnum, short segnum, short sidenum, vms_vector * pos, int forever, fix max_volume, fix max_distance );

extern int digi_link_sound_to_object3( int org_soundnum, short objnum, int forever, fix max_volume, fix  max_distance, int loop_start, int loop_end );

extern void digi_play_midi_song( char * filename, char * melodic_bank, char * drum_bank, int loop );

extern void digi_play_sample_3d( int soundno, int angle, int volume, int no_dups ); // Volume from 0-0x7fff

extern void digi_init_sounds(void);
extern void digi_sync_sounds(void);
extern void digi_kill_sound_linked_to_segment( int segnum, int sidenum, int soundnum );
extern void digi_kill_sound_linked_to_object( int objnum );

extern void digi_set_midi_volume( int mvolume );
extern void digi_set_digi_volume( int dvolume );
extern void digi_set_volume( int dvolume, int mvolume );

extern int digi_is_sound_playing(int soundno);

extern void digi_pause_all(void);
extern void digi_resume_all(void);
extern void digi_pause_digi_sounds(void);
extern void digi_resume_digi_sounds(void);
extern void digi_stop_all(void);

extern void digi_set_max_channels(int n);
extern int digi_get_max_channels(void);

extern int digi_lomem;

extern int digi_xlat_sound(int soundno);

extern void digi_stop_sound( int channel );

// Returns the channel a sound number is playing on, or
// -1 if none.
extern int digi_find_channel(int soundno);

// Volume 0-F1_0
extern int digi_start_sound(short soundnum, fix volume, int pan, int looping, int loop_start, int loop_end, int soundobj);

// Stops all sounds that are playing
void digi_stop_all_channels(void);

extern void digi_end_sound( int channel );
extern void digi_set_channel_pan( int channel, int pan );
extern void digi_set_channel_volume( int channel, int volume );
extern int digi_is_channel_playing(int channel);
extern void digi_pause_midi(void);
extern void digi_debug(void);
extern void digi_stop_current_song(void);

extern void digi_play_sample_looping( int soundno, fix max_volume,int loop_start, int loop_end );
extern void digi_change_looping_volume( fix volume );
extern void digi_stop_looping_sound(void);

// Plays a queued voice sound.
extern void digi_start_sound_queued( short soundnum, fix volume );

#endif
