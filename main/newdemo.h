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
 * header for demo playback system
 *
 */


#ifndef _NEWDEMO_H
#define _NEWDEMO_H

#ifdef NEWDEMO

#define ND_STATE_NORMAL             0
#define ND_STATE_RECORDING          1
#define ND_STATE_PLAYBACK           2
#define ND_STATE_PAUSED             3
#define ND_STATE_REWINDING          4
#define ND_STATE_FASTFORWARD        5
#define ND_STATE_ONEFRAMEFORWARD    6
#define ND_STATE_ONEFRAMEBACKWARD   7
#define ND_STATE_PRINTSCREEN        8

#ifndef MACINTOSH
#define DEMO_DIR                "demos/"
#else
#define DEMO_DIR                ":Demos:"
#endif

// Gives state of recorder
extern int Newdemo_state;
extern int NewdemoFrameCount;
extern int Newdemo_game_mode;

extern int Newdemo_vcr_state;
extern sbyte Newdemo_do_interpolate;

//Does demo start automatically?
extern int Auto_demo;

// Functions called during recording process...
extern void newdemo_record_start_demo(void);
extern void newdemo_record_start_frame(int frame_number, fix frame_time );
extern void newdemo_record_render_object(object * obj);
extern void newdemo_record_viewer_object(object * obj);
extern void newdemo_record_sound_3d( int soundno, int angle, int volume );
extern void newdemo_record_sound_3d_once( int soundno, int angle, int volume );
extern void newdemo_record_sound_once( int soundno );
extern void newdemo_record_sound( int soundno );
extern void newdemo_record_wall_hit_process( int segnum, int side, int damage, int playernum );
extern void newdemo_record_trigger( int segnum, int side, int objnum,int shot );
extern void newdemo_record_hostage_rescued( int hostage_num );
extern void newdemo_record_morph_frame(morph_data *md);
extern void newdemo_record_player_stats(int shields, int energy, int score );
extern void newdemo_record_player_afterburner(fix old_afterburner, fix afterburner);
extern void newdemo_record_wall_toggle(int segnum, int side );
extern void newdemo_record_control_center_destroyed(void);
extern void newdemo_record_hud_message(char *s);
extern void newdemo_record_palette_effect(short r, short g, short b);
extern void newdemo_record_player_energy(int, int);
extern void newdemo_record_player_shields(int, int);
extern void newdemo_record_player_flags(uint, uint);
extern void newdemo_record_player_weapon(int, int);
extern void newdemo_record_effect_blowup(short, int, vms_vector *);
extern void newdemo_record_homing_distance(fix);
extern void newdemo_record_letterbox(void);
extern void newdemo_record_rearview(void);
extern void newdemo_record_restore_cockpit(void);
extern void newdemo_record_wall_set_tmap_num1(short seg,ubyte side,short cseg,ubyte cside,short tmap);
extern void newdemo_record_wall_set_tmap_num2(short seg,ubyte side,short cseg,ubyte cside,short tmap);
extern void newdemo_record_multi_cloak(int pnum);
extern void newdemo_record_multi_decloak(int pnum);
extern void newdemo_set_new_level(int level_num);
extern void newdemo_record_restore_rearview(void);

extern void newdemo_record_multi_death(int pnum);
extern void newdemo_record_multi_kill(int pnum, sbyte kill);
extern void newdemo_record_multi_connect(int pnum, int new_player, char *new_callsign);
extern void newdemo_record_multi_reconnect(int pnum);
extern void newdemo_record_multi_disconnect(int pnum);
extern void newdemo_record_player_score(int score);
extern void newdemo_record_multi_score(int pnum, int score);
extern void newdemo_record_primary_ammo(int old_ammo, int new_ammo);
extern void newdemo_record_secondary_ammo(int old_ammo, int new_ammo);
extern void newdemo_record_door_opening(int segnum, int side);
extern void newdemo_record_laser_level(sbyte old_level, sbyte new_level);
extern void newdemo_record_cloaking_wall(int front_wall_num, int back_wall_num, ubyte type, ubyte state, fix cloak_value, fix l0, fix l1, fix l2, fix l3);
extern void newdemo_record_secret_exit_blown(int truth);


// Functions called during playback process...
extern void newdemo_object_move_all(void);
extern void newdemo_playback_one_frame(void);
extern void newdemo_goto_end(void);
extern void newdemo_goto_beginning(void);

// Interactive functions to control playback/record;
extern void newdemo_start_playback( char * filename );
extern void newdemo_stop_playback(void);
extern void newdemo_start_recording(void);
extern void newdemo_stop_recording(void);

extern int newdemo_get_percent_done(void);

extern void newdemo_record_link_sound_to_object3( int soundno, short objnum, fix max_volume, fix  max_distance, int loop_start, int loop_end );
extern int newdemo_find_object( int signature );
extern void newdemo_record_kill_sound_linked_to_object( int objnum );

#endif // NEWDEMO

#endif // _NEWDEMO_H
