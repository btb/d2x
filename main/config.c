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
cvar_t Config_control_joystick  = { "in_joystick", "0", 1 };
cvar_t Config_control_mouse     = { "in_mouse", "0", 1 };
cvar_t Config_channels_reversed = { "StereoReverse", "0", 1 };
cvar_t Config_joystick_sensitivity[] = {
	{ "joy_forwardsensitivity", "1.0", 1 },
	{ "joy_pitchsensitivity", "1.0", 1 },
	{ "joy_sidesensitivity", "1.0", 1 },
	{ "joy_yawsensitivity", "1.0", 1 },
	{ "joy_upsensitivity", "1.0", 1 },
	{ "joy_banksensitivity", "1.0", 1 },
};
cvar_t Config_joystick_deadzone[] = {
	{ "joy_forwardthreshold", "0.16", 1 },
	{ "joy_pitchthreshold", "0.08", 1 },
	{ "joy_sidethreshold", "0.08", 1 },
	{ "joy_yawthreshold", "0.08", 1 },
	{ "joy_upthreshold", "0.08", 1 },
	{ "joy_bankthreshold", "0.08", 1 },
};
cvar_t Config_mouse_sensitivity[] = {
	{ "m_forward", "1.0", 1 },
	{ "m_pitch", "1.0", 1 },
	{ "m_side", "1.0", 1 },
	{ "m_yaw", "1.0", 1 },
	{ "m_up", "1.0", 1 },
	{ "m_bank", "1.0", 1 },
};
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
cvar_t Config_display_mode      = { "vid_mode", "0", 1 };


#define _CRYSTAL_LAKE_8_ST		0xe201
#define _CRYSTAL_LAKE_16_ST	0xe202
#define _AWE32_8_ST				0xe208
#define _AWE32_16_ST				0xe209


extern sbyte Object_complexity, Object_detail, Wall_detail, Wall_render_depth, Debris_amount, SoundChannels;

void set_custom_detail_vars(void);

uint32_t legacy_display_mode[] = { SM(320,200), SM(640,480), SM(320,400), SM(640,400), SM(800,600), SM(1024,768), SM(1280,1024) };


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
	int i;

	/* make sure our cvars are registered */
	cvar_registervariable(&Config_digi_volume);
	cvar_registervariable(&Config_midi_volume);
	cvar_registervariable(&Redbook_enabled);
	cvar_registervariable(&Config_redbook_volume);
	cvar_registervariable(&Config_channels_reversed);
	cvar_registervariable(&Config_gamma_level);
	cvar_registervariable(&Config_detail_level);
	cvar_registervariable(&Config_control_joystick);
	cvar_registervariable(&Config_control_mouse);
	for (i = 0; i < 6; i++)
		cvar_registervariable(&Config_joystick_sensitivity[i]);
	for (i = 0; i < 6; i++)
		cvar_registervariable(&Config_joystick_deadzone[i]);
	for (i = 0; i < 6; i++)
		cvar_registervariable(&Config_mouse_sensitivity[i]);
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
	cvar_registervariable(&Game_window_w);
	cvar_registervariable(&Game_window_h);
	cvar_registervariable(&Player_default_difficulty);
	cvar_registervariable(&Auto_leveling_on);
	cvar_registervariable(&Reticle_on);
	cvar_registervariable(&Cockpit_mode);
	cvar_registervariable(&Config_display_mode);
	cvar_registervariable(&Missile_view_enabled);
	cvar_registervariable(&Headlight_active_default);
	cvar_registervariable(&Guided_in_big_window);
	cvar_registervariable(&Automap_always_hires);

	config_initialized = 1;
}


static int get_lifetime_checksum (int a,int b)
{
	int num;

	// confusing enough to beat amateur disassemblers? Lets hope so

	num=(a<<8 ^ b);
	num^=(a | b);
	num*=num>>2;
	return (num);
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

	cmd_append("bind - sizedown");
	cmd_append("bind \"=\" sizeup");
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
	cvar_setint( &Config_control_joystick, 0 );
	cvar_setint( &Config_control_mouse, 0 );
	cvar_setint( &Config_channels_reversed, 0);

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

	cvar_setint(&Player_default_difficulty, 1);
	cvar_setint(&Auto_leveling_on, 1);

	if (cfexist("descent.cfg"))
		cmd_append("exec descent.cfg");
	else
		LoadConfigDefaults();

	cmd_queue_process();

	/* TODO: allow cvars to define a callback that will carry out these initialization things on change. */

	gr_palette_set_gamma( Config_gamma_level.intval );

	Detail_level = (int)strtol(Config_detail_level.string, NULL, 10);
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

	Default_display_mode = legacy_display_mode[Config_display_mode.intval];

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
	int i;
	
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

	cvar_setint(&Cockpit_mode, (Cockpit_mode_save != -1)?Cockpit_mode_save:Cockpit_mode.intval); //if have saved mode, write it instead of letterbox/rear view

	for (i = 0; i < (sizeof(legacy_display_mode) / sizeof(uint32_t)); i++) {
		if (legacy_display_mode[i] == Current_display_mode)
			cvar_setint(&Config_display_mode, i);
	}

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
