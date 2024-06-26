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
 * Defines and exported variables for multi.c
 *
 */

#ifndef _MULTI_H
#define _MULTI_H

#define MAX_MESSAGE_LEN 35

// Defines
#include "gameseq.h"
#include "piggy.h"

// What version of the multiplayer protocol is this?

#ifdef SHAREWARE
#define MULTI_PROTO_VERSION 3
#else
#define MULTI_PROTO_VERSION 4
#endif

// Protocol versions:
//   1 Descent Shareware
//   2 Descent Registered/Commercial
//   3 Descent II Shareware
//   4 Descent II Commercial

// Save multiplayer games?

#define MULTI_SAVE

// How many simultaneous network players do we support?

#define MAX_NUM_NET_PLAYERS     8

#define MULTI_POSITION          0
#define MULTI_REAPPEAR          1
#define MULTI_FIRE              2
#define MULTI_KILL              3
#define MULTI_REMOVE_OBJECT     4
#define MULTI_PLAYER_EXPLODE    5
#define MULTI_MESSAGE           6
#define MULTI_QUIT              7
#define MULTI_PLAY_SOUND        8
#define MULTI_BEGIN_SYNC        9
#define MULTI_CONTROLCEN        10
#define MULTI_ROBOT_CLAIM       11
#define MULTI_END_SYNC          12
#define MULTI_CLOAK             13
#define MULTI_ENDLEVEL_START    14
#define MULTI_DOOR_OPEN         15
#define MULTI_CREATE_EXPLOSION  16
#define MULTI_CONTROLCEN_FIRE   17
#define MULTI_PLAYER_DROP       18
#define MULTI_CREATE_POWERUP    19
#define MULTI_CONSISTENCY       20
#define MULTI_DECLOAK           21
#define MULTI_MENU_CHOICE       22
#define MULTI_ROBOT_POSITION    23
#define MULTI_ROBOT_EXPLODE     24
#define MULTI_ROBOT_RELEASE     25
#define MULTI_ROBOT_FIRE        26
#define MULTI_SCORE             27
#define MULTI_CREATE_ROBOT      28
#define MULTI_TRIGGER           29
#define MULTI_BOSS_ACTIONS      30
#define MULTI_CREATE_ROBOT_POWERUPS 31
#define MULTI_HOSTAGE_DOOR      32

#define MULTI_SAVE_GAME         33
#define MULTI_RESTORE_GAME      34

#define MULTI_REQ_PLAYER        35  // Someone requests my player structure
#define MULTI_SEND_PLAYER       36  // Sending someone my player structure
#define MULTI_MARKER            37
#define MULTI_DROP_WEAPON       38
#define MULTI_GUIDED            39
#define MULTI_STOLEN_ITEMS      40
#define MULTI_WALL_STATUS       41  // send to new players
#define MULTI_HEARTBEAT         42
#define MULTI_KILLGOALS         43
#define MULTI_SEISMIC           44
#define MULTI_LIGHT             45
#define MULTI_START_TRIGGER     46
#define MULTI_FLAGS             47
#define MULTI_DROP_BLOB         48
#define MULTI_POWERUP_UPDATE    49
#define MULTI_ACTIVE_DOOR       50
#define MULTI_SOUND_FUNCTION    51
#define MULTI_CAPTURE_BONUS     52
#define MULTI_GOT_FLAG          53
#define MULTI_DROP_FLAG         54
#define MULTI_ROBOT_CONTROLS    55
#define MULTI_FINISH_GAME       56
#define MULTI_RANK              57
#define MULTI_MODEM_PING        58
#define MULTI_MODEM_PING_RETURN 59
#define MULTI_ORB_BONUS         60
#define MULTI_GOT_ORB           61
#define MULTI_DROP_ORB          62
#define MULTI_PLAY_BY_PLAY      63

#define MULTI_MAX_TYPE          63

#define MAX_NET_CREATE_OBJECTS  40

#define MAX_MULTI_MESSAGE_LEN   120

// Exported functions

int objnum_remote_to_local(int remote_obj, int owner);
int objnum_local_to_remote(int local_obj, sbyte *owner);
void map_objnum_local_to_remote(int local, int remote, int owner);
void map_objnum_local_to_local(int objnum);
void reset_network_objects(void);

void multi_init_objects(void);
void multi_show_player_list(void);
void multi_do_frame(void);


void multi_send_flags(char);
void multi_send_fire(void);
void multi_send_destroy_controlcen(int objnum, int player);
void multi_send_endlevel_start(int);
void multi_send_player_explode(char type);
void multi_send_message(void);
void multi_send_position(int objnum);
void multi_send_reappear(void);
void multi_send_kill(int objnum);
void multi_send_remobj(int objnum);
void multi_send_quit(int why);
void multi_send_door_open(int segnum, int side,ubyte flag);
void multi_send_create_explosion(int player_num);
void multi_send_controlcen_fire(vms_vector *to_target, int gun_num, int objnum);
void multi_send_cloak(void);
void multi_send_decloak(void);
void multi_send_create_powerup(int powerup_type, int segnum, int objnum, vms_vector *pos);
void multi_send_play_sound(int sound_num, fix volume);
void multi_send_audio_taunt(int taunt_num);
void multi_send_score(void);
void multi_send_trigger(int trigger);
void multi_send_hostage_door_status(int wallnum);
void multi_send_netplayer_stats_request(ubyte player_num);
void multi_send_drop_weapon (int objnum,int seed);
void multi_send_drop_marker (int player,vms_vector position,char messagenum,char text[]);
void multi_send_guided_info (object *miss,char);


void multi_endlevel_score(void);
void multi_prep_level(void);
int multi_endlevel(int *secret);
int multi_menu_poll(void);
void multi_leave_game(void);
void multi_process_data(char *dat, int len);
void multi_process_bigdata(char *buf, int len);
void multi_do_death(int objnum);
void multi_send_message_dialog(void);
int multi_delete_extra_objects(void);
void multi_make_ghost_player(int objnum);
void multi_make_player_ghost(int objnum);
void multi_define_macro(int key);
void multi_send_macro(int key);
int multi_get_kill_list(int *plist);
void multi_new_game(void);
void multi_sort_kill_list(void);
void multi_reset_stuff(void);

void multi_send_data(char *buf, int len, int repeat);

int get_team(int pnum);

// Exported variables


extern int Network_active;
extern int Network_laser_gun;
extern int Network_laser_fired;
extern int Network_laser_level;
extern int Network_laser_flags;
extern int Netlife_kills,Netlife_killed;

extern char multibuf[MAX_MULTI_MESSAGE_LEN+4];
extern short Network_laser_track;

extern int who_killed_controlcen;

extern int Net_create_objnums[MAX_NET_CREATE_OBJECTS];
extern int Net_create_loc;

extern short kill_matrix[MAX_NUM_NET_PLAYERS][MAX_NUM_NET_PLAYERS];
extern short team_kills[2];

extern int multi_goto_secret;

//do we draw the kill list on the HUD?
extern int Show_kill_list;
extern int Show_reticle_name;
extern fix Show_kill_list_timer;

// Used to send network messages

extern char Network_message[MAX_MESSAGE_LEN];
extern cvar_t Network_message_macro[4];
extern int Network_message_reciever;

// Which player 'owns' each local object for network purposes
extern sbyte object_owner[MAX_OBJECTS];

extern int multi_in_menu; // Flag to tell if we're executing GameLoop from within a newmenu.
extern int multi_leave_menu;
extern int multi_quit_game;

extern int multi_sending_message;
extern int multi_defining_message;
extern void multi_message_input_sub( int key );
extern void multi_send_message_start(void);
extern void multi_send_message_end(void);

extern int multi_powerup_is_4pack(int );
extern void multi_send_orb_bonus( char pnum );
extern void multi_send_got_orb( char pnum );
extern void multi_add_lifetime_kills(void);

extern int control_invul_time;

#define N_PLAYER_SHIP_TEXTURES 6

extern bitmap_index multi_player_textures[MAX_NUM_NET_PLAYERS][N_PLAYER_SHIP_TEXTURES];

#define NETGAME_FLAG_CLOSED             1
#define NETGAME_FLAG_SHOW_ID            2
#define NETGAME_FLAG_SHOW_MAP           4
#define NETGAME_FLAG_HOARD              8
#define NETGAME_FLAG_TEAM_HOARD         16
#define NETGAME_FLAG_REALLY_ENDLEVEL    32
#define NETGAME_FLAG_REALLY_FORMING     64

#define NETGAME_NAME_LEN                15
#define NETGAME_AUX_SIZE                20  // Amount of extra data for the network protocol to store in the netgame packet

enum comp_type {DOS,WIN_32,WIN_95,MAC} __pack__ ;

// sigh...the socket structure member was moved away from it's friends.
// I'll have to create a union for appletalk network info with just
// the server and node members since I cannot change the order ot these
// members.

typedef struct netplayer_info {
	char    callsign[CALLSIGN_LEN+1];
	union {
		struct {
			ubyte   server[4];
			ubyte   node[6];
		} ipx;
		struct {
			ushort  net;
			ubyte   node;
			ubyte   socket;
		} appletalk;
	} network;

	ubyte   version_major;
	ubyte   version_minor;
	enum comp_type computer_type;
	sbyte    connected;

	ushort  socket;

	ubyte   rank;

} __pack__ netplayer_info;

typedef struct AllNetPlayers_info
{
	char    type;
	int     Security;
	struct netplayer_info players[MAX_PLAYERS+4];
} __pack__ AllNetPlayers_info;

typedef struct netgame_info {
	ubyte   type;
	int     Security;
	char    game_name[NETGAME_NAME_LEN+1];
	char    mission_title[MISSION_NAME_LEN+1];
	char    mission_name[9];
	int     levelnum;
	ubyte   gamemode;
	ubyte   RefusePlayers;
	ubyte   difficulty;
	ubyte   game_status;
	ubyte   numplayers;
	ubyte   max_numplayers;
	ubyte   numconnected;
	ubyte   game_flags;
	ubyte   protocol_version;
	ubyte   version_major;
	ubyte   version_minor;
	ubyte   team_vector;

// change the order of the bit fields for the mac compiler.
// doing so will mean I don't have to do screwy things to
// send this as network information

#ifndef WORDS_BIGENDIAN
	unsigned short DoMegas:1;
	unsigned short DoSmarts:1;
	unsigned short DoFusions:1;
	unsigned short DoHelix:1;
	unsigned short DoPhoenix:1;
	unsigned short DoAfterburner:1;
	unsigned short DoInvulnerability:1;
	unsigned short DoCloak:1;
	unsigned short DoGauss:1;
	unsigned short DoVulcan:1;
	unsigned short DoPlasma:1;
	unsigned short DoOmega:1;
	unsigned short DoSuperLaser:1;
	unsigned short DoProximity:1;
	unsigned short DoSpread:1;
	unsigned short DoSmartMine:1;
	unsigned short DoFlash:1;
	unsigned short DoGuided:1;
	unsigned short DoEarthShaker:1;
	unsigned short DoMercury:1;
	unsigned short Allow_marker_view:1;
	unsigned short AlwaysLighting:1;
	unsigned short DoAmmoRack:1;
	unsigned short DoConverter:1;
	unsigned short DoHeadlight:1;
	unsigned short DoHoming:1;
	unsigned short DoLaserUpgrade:1;
	unsigned short DoQuadLasers:1;
	unsigned short ShowAllNames:1;
	unsigned short BrightPlayers:1;
	unsigned short invul:1;
	unsigned short bitfield_not_used2:1;
#else
	unsigned short DoSmartMine:1;
	unsigned short DoSpread:1;
	unsigned short DoProximity:1;
	unsigned short DoSuperLaser:1;
	unsigned short DoOmega:1;
	unsigned short DoPlasma:1;
	unsigned short DoVulcan:1;
	unsigned short DoGauss:1;
	unsigned short DoCloak:1;
	unsigned short DoInvulnerability:1;
	unsigned short DoAfterburner:1;
	unsigned short DoPhoenix:1;
	unsigned short DoHelix:1;
	unsigned short DoFusions:1;
	unsigned short DoSmarts:1;
	unsigned short DoMegas:1;

	unsigned short bitfield_not_used2:1;
	unsigned short invul:1;
	unsigned short BrightPlayers:1;
	unsigned short ShowAllNames:1;
	unsigned short DoQuadLasers:1;
	unsigned short DoLaserUpgrade:1;
	unsigned short DoHoming:1;
	unsigned short DoHeadlight:1;
	unsigned short DoConverter:1;
	unsigned short DoAmmoRack:1;
	unsigned short AlwaysLighting:1;
	unsigned short Allow_marker_view:1;
	unsigned short DoMercury:1;
	unsigned short DoEarthShaker:1;
	unsigned short DoGuided:1;
	unsigned short DoFlash:1;
#endif

	char    team_name[2][CALLSIGN_LEN+1];
	int     locations[MAX_PLAYERS];
	short   kills[MAX_PLAYERS][MAX_PLAYERS];
	ushort  segments_checksum;
	short   team_kills[2];
	short   killed[MAX_PLAYERS];
	short   player_kills[MAX_PLAYERS];
	int     KillGoal;
	fix     PlayTimeAllowed;
	fix     level_time;
	int     control_invul_time;
	int     monitor_vector;
	int     player_score[MAX_PLAYERS];
	ubyte   player_flags[MAX_PLAYERS];
	short   PacketsPerSec;
	ubyte   ShortPackets;
	ubyte   AuxData[NETGAME_AUX_SIZE];  // Storage for protocol-specific data (e.g., multicast session and port)

} __pack__ netgame_info;

extern struct netgame_info Netgame;
extern struct AllNetPlayers_info NetPlayers;

int network_i_am_master(void);
void change_playernum_to(int new_pnum);

//how to encode missiles & flares in weapon packets
#define MISSILE_ADJUST  100
#define FLARE_ADJUST    127

#ifdef EDITOR
void save_hoard_data(void);
#endif

#endif /* _MULTI_H */
