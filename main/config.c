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
 * contains routine(s) to read in the configuration file which contains
 * game configuration stuff like detail level, sound card, etc
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifndef MACINTOSH			// I'm going to totally seperate these routines -- yeeech!!!!
							// see end of file for macintosh equivs

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "winapp.h"
#else
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <physfs.h>

#include "inferno.h"
#include "menu.h"
#include "movie.h"
#include "digi.h"
#include "kconfig.h"
#include "palette.h"
#include "joy.h"
#include "songs.h"
#include "args.h"
#include "player.h"
#include "mission.h"
#include "mono.h"
#include "key.h"
#include "physfsx.h"
#include "multi.h"


#ifdef __MSDOS__
cvar_t Config_digi_type         = { "DigiDeviceID8", "0", 1 };
cvar_t digi_driver_board_16     = { "DigiDeviceID16", "0", 1 };
//cvar_t digi_driver_port         = { "DigiPort", "0", 1 };
//cvar_t digi_driver_irq          = { "DigiIrq", "0", 1 };
cvar_t Config_digi_dma          = { "DigiDma8", "0", 1 };
cvar_t digi_driver_dma_16       = { "DigiDma16", "0", 1 };
cvar_t Config_midi_type         = { "MidiDeviceID", "0", 1 };
//cvar_t digi_midi_port           = { "MidiPort", "0", 1 };
#endif
cvar_t Config_digi_volume       = { "DigiVolume", "8", 1 };
cvar_t Config_midi_volume       = { "MidiVolume", "8", 1 };
cvar_t Config_redbook_volume    = { "RedbookVolume", "8", 1 };
cvar_t Config_detail_level      = { "DetailLevel", "4", 1 };
cvar_t Config_gamma_level       = { "GammaLevel", "0", 1 };
cvar_t Config_control_type      = { "ControlType", "0", 1 };
cvar_t Config_channels_reversed = { "StereoReverse", "0", 1 };
cvar_t Config_joystick_sensitivity = { "JoystickSensitivity", "8", 1 };
cvar_t Config_joystick_min      = { "JoystickMin", "0,0,0,0", 1 };
cvar_t Config_joystick_max      = { "JoystickMax", "0,0,0,0", 1 };
cvar_t Config_joystick_cen      = { "JoystickCen", "0,0,0,0", 1 };
cvar_t config_last_player       = { "LastPlayer", "", 1 };
cvar_t config_last_mission      = { "LastMission", "", 1 };
cvar_t Config_vr_type           = { "VR_type", "0", 1 };
cvar_t Config_vr_resolution     = { "VR_resolution", "0", 1 };
cvar_t Config_vr_tracking       = { "VR_tracking", "0", 1 };
cvar_t Config_primary_order     = { "PrimaryOrder", "", 1 };
cvar_t Config_secondary_order   = { "SecondaryOrder", "", 1 };
cvar_t Config_lifetime_kills    = { "LifetimeKills", "0", 1 };
cvar_t Config_lifetime_killed   = { "LifetimeKilled", "0", 1 };
cvar_t Config_lifetime_checksum = { "LifetimeChecksum", "0", 1 };


#define _CRYSTAL_LAKE_8_ST		0xe201
#define _CRYSTAL_LAKE_16_ST	0xe202
#define _AWE32_8_ST				0xe208
#define _AWE32_16_ST				0xe209


extern sbyte Object_complexity, Object_detail, Wall_detail, Wall_render_depth, Debris_amount, SoundChannels;

void set_custom_detail_vars(void);
int get_lifetime_checksum(int a, int b);


#define CL_MC0 0xF8F
#define CL_MC1 0xF8D
/*
void CrystalLakeWriteMCP( ushort mc_addr, ubyte mc_data )
{
	_disable();
	outp( CL_MC0, 0xE2 );				// Write password
	outp( mc_addr, mc_data );		// Write data
	_enable();
}

ubyte CrystalLakeReadMCP( ushort mc_addr )
{
	ubyte value;
	_disable();
	outp( CL_MC0, 0xE2 );		// Write password
	value = inp( mc_addr );		// Read data
	_enable();
	return value;
}

void CrystalLakeSetSB()
{
	ubyte tmp;
	tmp = CrystalLakeReadMCP( CL_MC1 );
	tmp &= 0x7F;
	CrystalLakeWriteMCP( CL_MC1, tmp );
}

void CrystalLakeSetWSS()
{
	ubyte tmp;
	tmp = CrystalLakeReadMCP( CL_MC1 );
	tmp |= 0x80;
	CrystalLakeWriteMCP( CL_MC1, tmp );
}
*/
//MovieHires might be changed by -nohighres, so save a "real" copy of it
int SaveMovieHires;
int save_redbook_enabled;

#ifdef WINDOWS
void CheckMovieAttributes()
{
		HKEY hKey;
		DWORD len, type, val;
		long lres;
 
		lres = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Parallax\\Descent II\\1.1\\INSTALL",
							0, KEY_READ, &hKey);
		if (lres == ERROR_SUCCESS) {
			len = sizeof(val);
			lres = RegQueryValueEx(hKey, "HIRES", NULL, &type, &val, &len);
			if (lres == ERROR_SUCCESS) {
				cvar_setint( &MovieHires, val );
				logentry("HIRES=%d\n", val);
			}
			RegCloseKey(hKey);
		}
}
#endif


static int config_initialized;

static void config_init(void)
{
	/* make sure our cvars are registered */
	cvar_registervariable(&Config_digi_volume);
	cvar_registervariable(&Config_midi_volume);
	cvar_registervariable(&Redbook_enabled);
	cvar_registervariable(&Config_redbook_volume);
	cvar_registervariable(&Config_channels_reversed);
	cvar_registervariable(&Config_gamma_level);
	cvar_registervariable(&Config_detail_level);
	cvar_registervariable(&Config_control_type);
	cvar_registervariable(&Config_joystick_sensitivity);
	cvar_registervariable(&Config_joystick_min);
	cvar_registervariable(&Config_joystick_cen);
	cvar_registervariable(&Config_joystick_max);
	cvar_registervariable(&config_last_player);
	cvar_registervariable(&config_last_mission);
	cvar_registervariable(&Config_vr_type);
	cvar_registervariable(&Config_vr_resolution);
	cvar_registervariable(&Config_vr_tracking);
	cvar_registervariable(&MovieHires);
	cvar_registervariable(&real_guidebot_name);
	cvar_registervariable(&Config_primary_order);
	cvar_registervariable(&Config_secondary_order);
	cvar_registervariable(&Cockpit_3d_view[0]);
	cvar_registervariable(&Cockpit_3d_view[1]);
	cvar_registervariable(&Config_lifetime_kills);
	cvar_registervariable(&Config_lifetime_killed);
	cvar_registervariable(&Config_lifetime_checksum);
#ifdef NETWORK
	cvar_registervariable(&Network_message_macro[0]);
	cvar_registervariable(&Network_message_macro[1]);
	cvar_registervariable(&Network_message_macro[2]);
	cvar_registervariable(&Network_message_macro[3]);
#endif

	config_initialized = 1;
}


void LoadConfigDefaults(void)
{
	cmd_append("bind UP     +lookdown;      bind PAD8   +lookdown");
	cmd_append("bind DOWN   +lookup;        bind PAD2   +lookup");
	cmd_append("bind LEFT   +left;          bind PAD4   +left");
	cmd_append("bind RIGHT  +right;         bind PAD6   +right");

	cmd_append("bind LALT   +strafe");
	cmd_append("bind PAD1   +moveleft");
	cmd_append("bind PAD3   +moveright");
	cmd_append("bind PAD-   +moveup");
	cmd_append("bind PAD+   +movedown");

	cmd_append("bind Q      +bankleft;      bind PAD7   +bankleft");
	cmd_append("bind E      +bankright;     bind PAD9   +bankright");

	cmd_append("bind ,      +cycle");
	cmd_append("bind .      +cycle2");

	cmd_append("bind LCTRL  +attack;        bind RCTRL  +attack");
	cmd_append("bind SPC    +attack2");
	cmd_append("bind F      +flare");
	cmd_append("bind B      +bomb");

	cmd_append("bind R      +rearview");
	cmd_append("bind TAB    +automap");

	cmd_append("bind A      +forward");
	cmd_append("bind Z      +back");
	cmd_append("bind S      +afterburner");

	cmd_append("bind H      +headlight");
	cmd_append("bind T      +nrgshield");

	cmd_append("bind J1B1   +attack");
	cmd_append("bind J1B2   +attack2");

	cmd_append("bind MB1    +attack");
	cmd_append("bind MB2    +attack2");

	cmd_append("bind 1 weapon 1");
	cmd_append("bind 2 weapon 2");
	cmd_append("bind 3 weapon 3");
	cmd_append("bind 4 weapon 4");
	cmd_append("bind 5 weapon 5");
	cmd_append("bind 6 weapon 6");
	cmd_append("bind 7 weapon 7");
	cmd_append("bind 8 weapon 8");
	cmd_append("bind 9 weapon 9");
	cmd_append("bind 0 weapon 0");
}


int ReadConfigFile()
{
	int joy_axis_min[7];
	int joy_axis_center[7];
	int joy_axis_max[7];
	int i;

	if (!config_initialized)
		config_init();

	cvar_set_cvar( &config_last_player, "" );

	joy_axis_min[0] = joy_axis_min[1] = joy_axis_min[2] = joy_axis_min[3] = 0;
	joy_axis_max[0] = joy_axis_max[1] = joy_axis_max[2] = joy_axis_max[3] = 0;
	joy_axis_center[0] = joy_axis_center[1] = joy_axis_center[2] = joy_axis_center[3] = 0;

	joy_set_cal_vals(joy_axis_min, joy_axis_center, joy_axis_max);

#if 0
	cvar_setint(&digi_driver_board, 0);
	cvar_setint(&digi_driver_port, 0);
	cvar_setint(&digi_driver_irq, 0);
	cvar_setint(&digi_driver_dma, 0);
	cvar_setint(&digi_midi_type, 0);
	cvar_setint(&digi_midi_port, 0);
#endif

	cvar_setint( &Config_digi_volume, 8 );
	cvar_setint( &Config_midi_volume, 8 );
	cvar_setint( &Config_redbook_volume, 8 );
	cvar_setint( &Config_control_type, CONTROL_NONE );
	cvar_setint( &Config_channels_reversed, 0);
	cvar_setint( &Config_joystick_sensitivity, 8 );

	//set these here in case no cfg file
	SaveMovieHires = MovieHires.intval;
	save_redbook_enabled = Redbook_enabled.intval;

	InitWeaponOrdering(); // setup default weapon priorities
	cvar_set_cvarf(&Config_primary_order, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		PrimaryOrder[0], PrimaryOrder[1], PrimaryOrder[2], PrimaryOrder[3],
		PrimaryOrder[4], PrimaryOrder[5], PrimaryOrder[6], PrimaryOrder[7],
		PrimaryOrder[8], PrimaryOrder[9], PrimaryOrder[10]);
	cvar_set_cvarf(&Config_secondary_order, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		SecondaryOrder[0], SecondaryOrder[1], SecondaryOrder[2], SecondaryOrder[3],
		SecondaryOrder[4], SecondaryOrder[5], SecondaryOrder[6], SecondaryOrder[7],
		SecondaryOrder[8], SecondaryOrder[9], SecondaryOrder[10]);

	cvar_setint( &Cockpit_3d_view[0], CV_NONE );
	cvar_setint( &Cockpit_3d_view[1], CV_NONE );

	cvar_setint( &Config_lifetime_kills, 0 );
	cvar_setint( &Config_lifetime_killed, 0 );
	cvar_setint( &Config_lifetime_checksum, 0 );

	// Default taunt macros
#ifdef NETWORK
	cvar_set_cvar(&Network_message_macro[0], "Why can't we all just get along?");
	cvar_set_cvar(&Network_message_macro[1], "Hey, I got a present for ya");
	cvar_set_cvar(&Network_message_macro[2], "I got a hankerin' for a spankerin'");
	cvar_set_cvar(&Network_message_macro[3], "This one's headed for Uranus");
#endif

	if (cfexist("descent.cfg"))
		cmd_append("exec descent.cfg");
	else
		LoadConfigDefaults();

	cmd_queue_process();

	/* TODO: allow cvars to define a callback that will carry out these initialization things on change. */

	gr_palette_set_gamma( Config_gamma_level.intval );

	Detail_level = strtol(Config_detail_level.string, NULL, 10);
	if (Detail_level == NUM_DETAIL_LEVELS - 1) {
		int count,dummy,oc,od,wd,wrd,da,sc;

		count = sscanf (Config_detail_level.string, "%d,%d,%d,%d,%d,%d,%d\n",&dummy,&oc,&od,&wd,&wrd,&da,&sc);

		if (count == 7) {
			Object_complexity = oc;
			Object_detail = od;
			Wall_detail = wd;
			Wall_render_depth = wrd;
			Debris_amount = da;
			SoundChannels = sc;
			set_custom_detail_vars();
		}
	}

	sscanf( Config_joystick_min.string, "%d,%d,%d,%d", &joy_axis_min[0], &joy_axis_min[1], &joy_axis_min[2], &joy_axis_min[3] );
	sscanf( Config_joystick_max.string, "%d,%d,%d,%d", &joy_axis_max[0], &joy_axis_max[1], &joy_axis_max[2], &joy_axis_max[3] );
	sscanf( Config_joystick_cen.string, "%d,%d,%d,%d", &joy_axis_center[0], &joy_axis_center[1], &joy_axis_center[2], &joy_axis_center[3] );

	joy_set_cal_vals(joy_axis_min, joy_axis_center, joy_axis_max);

	i = FindArg( "-volume" );
	
	if ( i > 0 )	{
		i = atoi( Args[i+1] );
		if ( i < 0 ) i = 0;
		if ( i > 100 ) i = 100;
		cvar_setint( &Config_digi_volume, (i * 8) / 100 );
		cvar_setint( &Config_midi_volume, (i * 8) / 100 );
		cvar_setint( &Config_redbook_volume, (i * 8) / 100 );
	}

	if ( Config_digi_volume.intval > 8 ) cvar_setint( &Config_digi_volume, 8 );
	if ( Config_midi_volume.intval > 8 ) cvar_setint( &Config_midi_volume, 8 );
	if ( Config_redbook_volume.intval > 8 ) cvar_setint( &Config_redbook_volume, 8 );

	digi_set_volume( (Config_digi_volume.intval * 32768) / 8, (Config_midi_volume.intval * 128) / 8 );

	kc_set_controls();

	strncpy(guidebot_name, real_guidebot_name.string, GUIDEBOT_NAME_LEN);
	guidebot_name[GUIDEBOT_NAME_LEN] = 0;

	sscanf(Config_primary_order.string, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		(int *)&PrimaryOrder[0], (int *)&PrimaryOrder[1], (int *)&PrimaryOrder[2], (int *)&PrimaryOrder[3],
		(int *)&PrimaryOrder[4], (int *)&PrimaryOrder[5], (int *)&PrimaryOrder[6], (int *)&PrimaryOrder[7],
		(int *)&PrimaryOrder[8], (int *)&PrimaryOrder[9], (int *)&PrimaryOrder[10]);
	sscanf(Config_secondary_order.string, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		(int *)&SecondaryOrder[0], (int *)&SecondaryOrder[1], (int *)&SecondaryOrder[2], (int *)&SecondaryOrder[3],
		(int *)&SecondaryOrder[4], (int *)&SecondaryOrder[5], (int *)&SecondaryOrder[6], (int *)&SecondaryOrder[7],
		(int *)&SecondaryOrder[8], (int *)&SecondaryOrder[9], (int *)&SecondaryOrder[10]);

#ifdef NETWORK
	Netlife_kills = Config_lifetime_kills.intval;
	Netlife_killed = Config_lifetime_killed.intval;

	con_printf(CON_DEBUG, "Reading: lifetime checksum is %d\n", Config_lifetime_checksum.intval);
	if (Config_lifetime_checksum.intval != get_lifetime_checksum(Netlife_kills, Netlife_killed)) {
		Netlife_kills = Netlife_killed = 0;
		Warning("Shame on me\nTrying to cheat eh?");
	}
#endif

#if 0
	printf( "DigiDeviceID: 0x%x\n", digi_driver_board );
	printf( "DigiPort: 0x%x\n", digi_driver_port.intval );
	printf( "DigiIrq: 0x%x\n",  digi_driver_irq.intval );
	printf( "DigiDma: 0x%x\n",	digi_driver_dma.intval );
	printf( "MidiDeviceID: 0x%x\n", digi_midi_type.intval );
	printf( "MidiPort: 0x%x\n", digi_midi_port.intval );
  	key_getch();

	cvar_setint( &Config_midi_type, digi_midi_type );
	cvar_setint( &Config_digi_type, digi_driver_board );
	cvar_setint( &Config_digi_dma, digi_driver_dma );
#endif

#if 0
	if (digi_driver_board_16.intval > 0 && !FindArg("-no16bit") && digi_driver_board_16.intval != _GUS_16_ST) {
		digi_driver_board = digi_driver_board_16.intval;
		digi_driver_dma = digi_driver_dma_16.intval;
	}

	// HACK!!!
	//Hack to make some cards look like others, such as
	//the Crytal Lake look like Microsoft Sound System
	if ( digi_driver_board == _CRYSTAL_LAKE_8_ST )	{
		ubyte tmp;
		tmp = CrystalLakeReadMCP( CL_MC1 );
		if ( !(tmp & 0x80) )
			atexit( CrystalLakeSetSB );		// Restore to SB when done.
	 	CrystalLakeSetWSS();
		digi_driver_board = _MICROSOFT_8_ST;
	} else if ( digi_driver_board == _CRYSTAL_LAKE_16_ST )	{
		ubyte tmp;
		tmp = CrystalLakeReadMCP( CL_MC1 );
		if ( !(tmp & 0x80) )
			atexit( CrystalLakeSetSB );		// Restore to SB when done.
	 	CrystalLakeSetWSS();
		digi_driver_board = _MICROSOFT_16_ST;
	} else if ( digi_driver_board == _AWE32_8_ST )	{
		digi_driver_board = _SB16_8_ST;
	} else if ( digi_driver_board == _AWE32_16_ST )	{
		digi_driver_board = _SB16_16_ST;
	} else
		digi_driver_board		= digi_driver_board;
#else

	if (cfexist("descentw.cfg")) {
		cmd_append("exec descentw.cfg");
		cmd_queue_process();

		sscanf( Config_joystick_min.string, "%d,%d,%d,%d,%d,%d,%d", &joy_axis_min[0], &joy_axis_min[1], &joy_axis_min[2], &joy_axis_min[3], &joy_axis_min[4], &joy_axis_min[5], &joy_axis_min[6] );
		sscanf( Config_joystick_max.string, "%d,%d,%d,%d,%d,%d,%d", &joy_axis_max[0], &joy_axis_max[1], &joy_axis_max[2], &joy_axis_max[3], &joy_axis_max[4], &joy_axis_max[5], &joy_axis_max[6] );
		sscanf( Config_joystick_cen.string, "%d,%d,%d,%d,%d,%d,%d", &joy_axis_center[0], &joy_axis_center[1], &joy_axis_center[2], &joy_axis_center[3], &joy_axis_center[4], &joy_axis_center[5], &joy_axis_center[6] );
	}
#endif

	return 0;
}

int WriteConfigFile()
{
	PHYSFS_file *outfile;
	int joy_axis_min[7];
	int joy_axis_center[7];
	int joy_axis_max[7];
	
	joy_get_cal_vals(joy_axis_min, joy_axis_center, joy_axis_max);

	if (FindArg("-noredbook"))
		cvar_setint( &Redbook_enabled, save_redbook_enabled );

	cvar_setint( &Config_gamma_level, gr_palette_get_gamma() );

	if (Detail_level == NUM_DETAIL_LEVELS-1)
		cvar_set_cvarf( &Config_detail_level, "%d,%d,%d,%d,%d,%d,%d", Detail_level,
					   Object_complexity,Object_detail,Wall_detail,Wall_render_depth,Debris_amount,SoundChannels );
	else
		cvar_setint( &Config_detail_level, Detail_level );

	cvar_set_cvarf( &Config_joystick_min, "%d,%d,%d,%d", joy_axis_min[0], joy_axis_min[1], joy_axis_min[2], joy_axis_min[3] );
	cvar_set_cvarf( &Config_joystick_cen, "%d,%d,%d,%d", joy_axis_center[0], joy_axis_center[1], joy_axis_center[2], joy_axis_center[3] );
	cvar_set_cvarf( &Config_joystick_max, "%d,%d,%d,%d", joy_axis_max[0], joy_axis_max[1], joy_axis_max[2], joy_axis_max[3] );

	cvar_set_cvar( &config_last_player, Players[Player_num].callsign );

	cvar_setint( &MovieHires, SaveMovieHires );

	cvar_set_cvarf(&Config_primary_order, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		PrimaryOrder[0], PrimaryOrder[1], PrimaryOrder[2], PrimaryOrder[3],
		PrimaryOrder[4], PrimaryOrder[5], PrimaryOrder[6], PrimaryOrder[7],
		PrimaryOrder[8], PrimaryOrder[9], PrimaryOrder[10]);
	cvar_set_cvarf(&Config_secondary_order, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
		SecondaryOrder[0], SecondaryOrder[1], SecondaryOrder[2], SecondaryOrder[3],
		SecondaryOrder[4], SecondaryOrder[5], SecondaryOrder[6], SecondaryOrder[7],
		SecondaryOrder[8], SecondaryOrder[9], SecondaryOrder[10]);

#ifdef NETWORK
	cvar_setint(&Config_lifetime_kills, Netlife_kills);
	cvar_setint(&Config_lifetime_killed, Netlife_killed);
	cvar_setint(&Config_lifetime_checksum, get_lifetime_checksum(Netlife_kills, Netlife_killed));
	con_printf(CON_DEBUG, "Writing: Lifetime checksum is %d\n", Config_lifetime_checksum.intval);
#endif

	outfile = PHYSFSX_openWriteBuffered("descent.cfg");
	if (outfile == NULL)
		return 1;
	cvar_write(outfile);
	key_write_bindings(outfile);
	PHYSFS_close(outfile);

	if (FindArg("-nohires") || FindArg("-nohighres") || FindArg("-lowresmovies"))
		cvar_setint( &MovieHires, 0 );

#ifdef WINDOWS
	CheckMovieAttributes();
#endif

	return 0;
}		

#else		// !defined(MACINTOSH)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <Memory.h>
#include <Folders.h>
#include <GestaltEqu.h>
#include <Errors.h>
#include <Processes.h>
#include <Resources.h>
#include <LowMem.h>

#include "error.h"
#include "pstypes.h"
#include "game.h"
#include "digi.h"
#include "kconfig.h"
#include "palette.h"
#include "joy.h"
#include "args.h"
#include "player.h"
#include "mission.h"
#include "prefs.h"			// prefs file for configuration stuff -- from DeSalvo

#define MAX_CTB_LEN	512

typedef struct preferences {
	ubyte	digi_volume;
	ubyte	midi_volume;
	ubyte	stereo_reverse;
	ubyte	detail_level;
	ubyte	oc;					// object complexity
	ubyte	od;					// object detail
	ubyte	wd;					// wall detail
	ubyte	wrd;				// wall render depth
	ubyte	da;					// debris amount
	ubyte	sc;					// sound channels
	ubyte	gamma_level;
	ubyte	pixel_double;
	int		joy_axis_min[4];
	int		joy_axis_max[4];
	int		joy_axis_center[4];
	char	lastplayer[CALLSIGN_LEN+1];
	char	lastmission[MISSION_NAME_LEN+1];
	char	ctb_config[MAX_CTB_LEN];
	int		ctb_tool;
	ubyte	master_volume;
	ubyte	display_dialog;
	ubyte	change_resolution;
	ubyte	nosound;
	ubyte	nomidi;
	ubyte	sound_11k;
	ubyte	no_movies;
	ubyte	game_monitor;
	ubyte	redbook_volume;
	ubyte	enable_rave;
	ubyte	enable_input_sprockets;
} Preferences;

char config_last_player[CALLSIGN_LEN+1] = "";
char config_last_mission[MISSION_NAME_LEN+1] = "";
char config_last_ctb_cfg[MAX_CTB_LEN] = "";
int config_last_ctb_tool;
ubyte Config_master_volume = 4;
ubyte Config_digi_volume = 8;
ubyte Config_midi_volume = 8;
ubyte Config_redbook_volume = 8;
ubyte Config_control_type = 0;
ubyte Config_channels_reversed = 0;
ubyte Config_joystick_sensitivity = 8;

int Config_vr_type = 0;
int Config_vr_resolution = 0;
int Config_vr_tracking = 0;

extern sbyte Object_complexity, Object_detail, Wall_detail, Wall_render_depth, Debris_amount, SoundChannels;
extern void digi_set_master_volume( int volume );

void set_custom_detail_vars(void);

static ubyte have_prefs = 0;

//¥	------------------------------	Private Definitions
//¥	------------------------------	Private Types

typedef struct
{
	Str31	fileName;
	OSType	creator;
	OSType	fileType;
	OSType	resType;
	short	resID;
} PrefsInfo, *PrefsInfoPtr, **PrefsInfoHandle;

//¥	------------------------------	Private Variables

static PrefsInfo		prefsInfo;
static Boolean		prefsInited = 0;

//¥	------------------------------	Private Functions

static void Pstrcpy(StringPtr dst, StringPtr src);
static void Pstrcat(StringPtr dst, StringPtr src);
static Boolean FindPrefsFile(short *prefVRefNum, long *prefDirID);

//¥	--------------------	Pstrcpy

static void
Pstrcpy(StringPtr dst, StringPtr src)
{
	BlockMove(src, dst, (*src) + 1);
}

//¥	--------------------	Pstrcat

static void
Pstrcat(StringPtr dst, StringPtr src)
{
	BlockMove(src + 1, dst + (*dst) + 1, *src);
	*dst += *src;
}

//¥	--------------------	FindPrefsFile

static Boolean
FindPrefsFile(short *prefVRefNum, long *prefDirID)
{
OSErr		theErr;
long			response;
CInfoPBRec	infoPB;

	if (! prefsInited)
		return (0);
		
	theErr = Gestalt(gestaltFindFolderAttr, &response);
	if (theErr == noErr && ((response >> gestaltFindFolderPresent) & 1))
	{
		//¥	Find (or make) it the easy way...
		theErr = FindFolder(kOnSystemDisk, kPreferencesFolderType, kCreateFolder, prefVRefNum, prefDirID);
	}
	else
	{
	SysEnvRec	theSysEnv;
	StringPtr		prefFolderName = "\pPreferences";

		//¥	yeachh -- we have to do it all by hand!
		theErr = SysEnvirons(1, &theSysEnv);
		if (theErr != noErr)
			return (0);
			
		*prefVRefNum = theSysEnv.sysVRefNum;
		
		//¥	Check whether Preferences folder already exists
		infoPB.hFileInfo.ioCompletion	= 0;
		infoPB.hFileInfo.ioNamePtr	= prefFolderName;
		infoPB.hFileInfo.ioVRefNum	= *prefVRefNum;
		infoPB.hFileInfo.ioFDirIndex	= 0;
		infoPB.hFileInfo.ioDirID		= 0;

		theErr = PBGetCatInfo(&infoPB, 0);
		if (theErr == noErr)
		{
			*prefDirID = infoPB.hFileInfo.ioDirID;
		}
		else if (theErr == fnfErr)		//¥	Preferences doesn't already exist
		{
		HParamBlockRec	dirPB;
		
			//¥	Create "Preferences" folder
			dirPB.fileParam.ioCompletion	= 0;
			dirPB.fileParam.ioVRefNum	= *prefVRefNum;
			dirPB.fileParam.ioNamePtr	= prefFolderName;
			dirPB.fileParam.ioDirID		= 0;

			theErr = PBDirCreate(&dirPB, 0);
			if (theErr == noErr)
				*prefDirID = dirPB.fileParam.ioDirID;
		}
	}
	
	//¥	If we make it here OK, create Preferences file if necessary
	if (theErr == noErr)
	{
		infoPB.hFileInfo.ioCompletion	= 0;
		infoPB.hFileInfo.ioNamePtr	= prefsInfo.fileName;
		infoPB.hFileInfo.ioVRefNum	= *prefVRefNum;
		infoPB.hFileInfo.ioFDirIndex	= 0;
		infoPB.hFileInfo.ioDirID		= *prefDirID;

		theErr = PBGetCatInfo(&infoPB, 0);
		if (theErr == fnfErr)
		{
			theErr = HCreate(*prefVRefNum, *prefDirID, prefsInfo.fileName, prefsInfo.creator, prefsInfo.fileType);
			if (theErr == noErr)
			{
				HCreateResFile(*prefVRefNum, *prefDirID, prefsInfo.fileName);
				theErr = ResError();
			}
		}
	}
	
	return (theErr == noErr);
}

//¥	--------------------	InitPrefsFile

#define UNKNOWN_TYPE 0x3f3f3f3f

void
InitPrefsFile(OSType creator)
{
	OSErr err;
PrefsInfoHandle		piHdl;
	
	if ((piHdl = (PrefsInfoHandle) GetResource('PRFI', 0)) == nil)
	{
	ProcessSerialNumber	thePSN;
	ProcessInfoRec			thePIR;
	FSSpec				appSpec;
	StringPtr			app_string;

#if 0	
		GetCurrentProcess(&thePSN);
		thePIR.processName = nil;
		thePIR.processAppSpec = &appSpec;
		
		//¥	Set default to 'ÇApplicationÈ Prefs', PREF 0
		err = GetProcessInformation(&thePSN, &thePIR);
		if (err)
			Int3();
#endif
		app_string = LMGetCurApName();
//		Pstrcpy(prefsInfo.fileName, appSpec.name);
		Pstrcpy(prefsInfo.fileName, app_string);
		Pstrcat(prefsInfo.fileName, "\p Preferences");
		
		//¥	Set creator to calling application's signature (should be able to
		//¥	Determine this automatically, but unable to for some reason)
		prefsInfo.creator = creator;
		prefsInfo.fileType = 'pref';
		prefsInfo.resType = 'pref';
		prefsInfo.resID = 0;
	}
	else
	{
		//¥	Get Preferences file setup from PRFI 0
		BlockMove(*piHdl, &prefsInfo, sizeof (prefsInfo));
		ReleaseResource((Handle) piHdl);
		
		if (prefsInfo.creator == UNKNOWN_TYPE)
			prefsInfo.creator = creator;
	}
	
	prefsInited = 1;
}

//¥	--------------------	LoadPrefsFile

OSErr
LoadPrefsFile(Handle prefsHdl)
{
short	prefVRefNum, prefRefNum;
long		prefDirID;
OSErr	theErr = noErr;
Handle	origHdl;
Size		prefSize, origSize;

	if (prefsHdl == nil)
		return (nilHandleErr);

	prefSize = GetHandleSize(prefsHdl);
		
	if (! FindPrefsFile(&prefVRefNum, &prefDirID))
		return (fnfErr);

	prefRefNum = HOpenResFile(prefVRefNum, prefDirID, prefsInfo.fileName, fsRdWrPerm);
	if (prefRefNum == -1)
		return (ResError());
	
	//¥	Not finding the resource is not an error -- caller will use default data
	if ((origHdl = Get1Resource(prefsInfo.resType, prefsInfo.resID)) != nil)
	{
		origSize = GetHandleSize(origHdl);
		if (origSize > prefSize)			//¥	Extend handle for extra stored data
			SetHandleSize(prefsHdl, origSize);

		BlockMove(*origHdl, *prefsHdl, origSize);
		ReleaseResource(origHdl);
	}
	
	CloseResFile(prefRefNum);

	if (theErr == noErr)
		theErr = ResError();
	
	return (theErr);
}

//¥	--------------------	SavePrefsFile

OSErr
SavePrefsFile(Handle prefHdl)
{
short	prefVRefNum, prefRefNum;
long		prefDirID;
Handle	origHdl = nil;
Size		origSize, prefSize;
OSErr	theErr = noErr;
	
	if (! FindPrefsFile(&prefVRefNum, &prefDirID))
		return (fnfErr);
	
	if (prefHdl == nil)
		return (nilHandleErr);

	prefSize = GetHandleSize(prefHdl);

	prefRefNum = HOpenResFile(prefVRefNum, prefDirID, prefsInfo.fileName, fsRdWrPerm);
	if (prefRefNum == -1)
		return (ResError());
		
	if ((origHdl = Get1Resource(prefsInfo.resType, prefsInfo.resID)) != nil)
	{
		//¥	Overwrite existing preferences
		origSize = GetHandleSize(origHdl);
		if (prefSize > origSize)
			SetHandleSize(origHdl, prefSize);
			
		BlockMove(*prefHdl, *origHdl, prefSize);
		ChangedResource(origHdl);
		WriteResource(origHdl);
		ReleaseResource(origHdl);
	}
	else
	{
		//¥	Store specified preferences for the first time
		AddResource(prefHdl, prefsInfo.resType, prefsInfo.resID, "\p");
		WriteResource(prefHdl);
		DetachResource(prefHdl);
	}
	
	CloseResFile(prefRefNum);

	if (theErr == noErr)
		theErr = ResError();
	
	return (theErr);
}

//¥	-------------------------------------------------------------------------------------------

/*

	This module provides the ability to save and load a preferences file in the
	Preferences folder in the System Folder.  An optional resource, PRFI 0,
	is used to provide specifications for the preferences file (creator, etc.).

	Three functions are provided:

		void InitPrefsFile(OSType creator)

	This function will initialize the preferences file, that is, it will create
	it in the appropriate place if it doesn't currently exist.  It should be
	called with the creator code for the application.  Note that the creator
	code specified in PRFI 0 (if any) will be used only if the creator code
	passed to this function is '????'.  Without the PRFI 0 resource, the default
	specifications are:

	File Name: "{Application} Prefs" (i.e., the name of the app plus " Prefs"
	Creator: the creator passed to InitPrefsFile
	Type: 'PREF'
	Pref Resource Type: 'PREF'
 	Pref Resource ID: 0

	The PRFI 0 resource allows you to specify overrides for each of the above
	values.  This is useful for development, since the application name might
	go through changes, but the preferences file name is held constant.

	 	OSErr LoadPrefsFile(Handle prefsHndl)

	This function will attempt to copy the data stored in the preferences
	file to the given handle (which must be pre-allocated).  If the handle is too
	small, then it will be enlarged.  If it is too large, it will not be resized.
	The data in the preferences file (normally in PREF 0) will then be copied
	into the handle.  If the preferences file did not exist, the original data
	in the handle will not change.

		OSErr SavePrefsFile(Handle prefsHndl)

	This function will attempt to save the given handle to the preferences
	file.  Its contents will completely replace the previous data (normally
	the PREF 0 resource).

	In typical use, you will use InitPrefsFile once, then allocate a handle large
	enough to contain your preferences data, and initialize it with default values.
	Throughout the course of your program, the handle will undergo modification as
	user preferences change.  You can use SavePrefsFile anytime to update the
	preferences file, or wait until program exit to do so.

*/

int ReadConfigFile()
{
	int i;
	OSErr err;
	Handle prefs_handle;
	Preferences *prefs;
	char *p;
	
	if (!have_prefs) {			// not initialized....get a handle to the preferences file
		InitPrefsFile('DCT2');
		have_prefs = 1;
	}
	
	prefs_handle = NewHandleClear(sizeof(Preferences));		// new prefs handle
	if (prefs_handle == NULL)
		return;
		
	prefs = (Preferences *)(*prefs_handle);
	err = LoadPrefsFile(prefs_handle);
	if (err) {
		DisposeHandle(prefs_handle);
		return -1;
	}

	p = (char *)prefs;
	for (i = 0; i < sizeof(Preferences); i++) {
		if (*p != 0)
			break;
		p++;
	}
	if ( i == sizeof(Preferences) )
		return -1;
	
	Config_digi_volume = prefs->digi_volume;
	Config_midi_volume = prefs->midi_volume;
	Config_master_volume = prefs->master_volume;
	Config_redbook_volume = prefs->redbook_volume;
	Config_channels_reversed = prefs->stereo_reverse;
	gr_palette_set_gamma( (int)(prefs->gamma_level) );

	Scanline_double = (int)prefs->pixel_double;
	if ( PAEnabled )
		Scanline_double = 0;		// can't double with hardware acceleration
		
	Detail_level = prefs->detail_level;
	if (Detail_level == NUM_DETAIL_LEVELS-1) {
		Object_complexity = prefs->oc;
		Object_detail = prefs->od;
		Wall_detail = prefs->wd;
		Wall_render_depth = prefs->wrd;
		Debris_amount = prefs->da;
		SoundChannels = prefs->sc;
		set_custom_detail_vars();
	}

	strncpy( config_last_player, prefs->lastplayer, CALLSIGN_LEN );
	p = strchr(config_last_player, '\n' );
	if (p) *p = 0;
	
	strncpy(config_last_mission, prefs->lastmission, MISSION_NAME_LEN);
	p = strchr(config_last_mission, '\n' );
	if (p) *p = 0;

	strcpy(config_last_ctb_cfg, prefs->ctb_config);
	
	if ( Config_digi_volume > 8 ) Config_digi_volume = 8;

	if ( Config_midi_volume > 8 ) Config_midi_volume = 8;

	joy_set_cal_vals( prefs->joy_axis_min, prefs->joy_axis_center, prefs->joy_axis_max);
	digi_set_volume( (Config_digi_volume*256)/8, (Config_midi_volume*256)/8 );
	digi_set_master_volume(Config_master_volume);
	
	gConfigInfo.mDoNotDisplayOptions = prefs->display_dialog;
	gConfigInfo.mUse11kSounds = prefs->sound_11k;
	gConfigInfo.mDisableSound = prefs->nosound;
	gConfigInfo.mDisableMIDIMusic = prefs->nomidi;
	gConfigInfo.mChangeResolution = prefs->change_resolution;
	gConfigInfo.mDoNotPlayMovies = prefs->no_movies;
	gConfigInfo.mGameMonitor = prefs->game_monitor;
	gConfigInfo.mAcceleration = prefs->enable_rave;
	gConfigInfo.mInputSprockets = prefs->enable_input_sprockets;
	
	DisposeHandle(prefs_handle);
	return 0;
}

int WriteConfigFile()
{
	OSErr err;
	Handle prefs_handle;
	Preferences *prefs;
	
	prefs_handle = NewHandleClear(sizeof(Preferences));		// new prefs handle
	if (prefs_handle == NULL)
		return;
		
	prefs = (Preferences *)(*prefs_handle);
	
	joy_get_cal_vals(prefs->joy_axis_min, prefs->joy_axis_center, prefs->joy_axis_max);
	prefs->digi_volume = Config_digi_volume;
	prefs->midi_volume = Config_midi_volume;
	prefs->stereo_reverse = Config_channels_reversed;
	prefs->detail_level = Detail_level;
	if (Detail_level == NUM_DETAIL_LEVELS-1) {
		prefs->oc = Object_complexity;
		prefs->od = Object_detail;
		prefs->wd = Wall_detail;
		prefs->wrd = Wall_render_depth;
		prefs->da = Debris_amount;
		prefs->sc = SoundChannels;
	}
	prefs->gamma_level = (ubyte)gr_palette_get_gamma();

	if ( !PAEnabled )
		prefs->pixel_double = (ubyte)Scanline_double;		// hmm..don't write this out if doing hardware accel.
		
	strncpy( prefs->lastplayer, Players[Player_num].callsign, CALLSIGN_LEN );
	strncpy( prefs->lastmission, config_last_mission, MISSION_NAME_LEN );
	strcpy( prefs->ctb_config, config_last_ctb_cfg);
	prefs->ctb_tool = config_last_ctb_tool;
	prefs->master_volume = Config_master_volume;
	prefs->display_dialog = gConfigInfo.mDoNotDisplayOptions;
	prefs->change_resolution = gConfigInfo.mChangeResolution;
	prefs->nosound = gConfigInfo.mDisableSound;
	prefs->nomidi = gConfigInfo.mDisableMIDIMusic;
	prefs->sound_11k = gConfigInfo.mUse11kSounds;
	prefs->no_movies = gConfigInfo.mDoNotPlayMovies;
	prefs->game_monitor = gConfigInfo.mGameMonitor;
	prefs->redbook_volume = Config_redbook_volume;
	prefs->enable_rave = gConfigInfo.mAcceleration;
	prefs->enable_input_sprockets = gConfigInfo.mInputSprockets;

	err = SavePrefsFile(prefs_handle);
	DisposeHandle(prefs_handle);
	return (int)err;
}

#endif

