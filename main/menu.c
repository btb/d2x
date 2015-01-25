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
 * Inferno main menu.
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <string.h>

#include "inferno.h"
#include "game.h"
#include "gr.h"
#include "vid.h"
#include "key.h"
#include "iff.h"
#include "u_mem.h"
#include "error.h"
#include "bm.h"
#include "screens.h"
#include "mono.h"
#include "joy.h"
#include "vecmat.h"
#include "effects.h"
#include "slew.h"
#include "gamemine.h"
#include "gamesave.h"
#include "palette.h"
#include "args.h"
#include "newdemo.h"
#include "timer.h"
#include "sounds.h"
#include "gameseq.h"
#include "text.h"
#include "gamefont.h"
#include "newmenu.h"
#ifdef NETWORK
#  include "network.h"
#  include "ipx.h"
#  include "multi.h"
#endif
#include "scores.h"
#include "joydefs.h"
#ifdef NETWORK
#include "modem.h"
#endif
#include "kconfig.h"
#include "titles.h"
#include "credits.h"
#include "texmap.h"
#include "polyobj.h"
#include "state.h"
#include "mission.h"
#include "songs.h"
#include "config.h"
#include "movie.h"
#include "gamepal.h"
#include "gauges.h"
#include "powerup.h"
#include "strutil.h"
#include "reorder.h"

#ifdef MACINTOSH
	#include "resource.h"
	#include "isp.h"
	#include <Dialogs.h>
#endif

#ifdef EDITOR
#include "editor/editor.h"
#endif

//char *menu_difficulty_text[] = { "Trainee", "Rookie", "Fighter", "Hotshot", "Insane" };
//char *menu_detail_text[] = { "Lowest", "Low", "Medium", "High", "Highest", "", "Custom..." };

#define MENU_NEW_GAME                   0
#define MENU_GAME                       1 
#define MENU_EDITOR                     2
#define MENU_VIEW_SCORES                3
#define MENU_QUIT                       4
#define MENU_LOAD_GAME                  5
#define MENU_SAVE_GAME                  6
#define MENU_DEMO_PLAY                  8
#define MENU_LOAD_LEVEL                 9
#define MENU_START_IPX_NETGAME          10
#define MENU_JOIN_IPX_NETGAME           11
#define MENU_CONFIG                     13
#define MENU_REJOIN_NETGAME             14
#define MENU_DIFFICULTY                 15
#define MENU_START_SERIAL               18
#define MENU_HELP                       19
#define MENU_NEW_PLAYER                 20
#define MENU_MULTIPLAYER                21
#define MENU_STOP_MODEM                 22
#define MENU_SHOW_CREDITS               23
#define MENU_ORDER_INFO                 24
#define MENU_PLAY_SONG                  25
//#define MENU_START_TCP_NETGAME          26 // TCP/IP support was planned in Descent II,
//#define MENU_JOIN_TCP_NETGAME           27 // but never realized.
#define MENU_START_APPLETALK_NETGAME    28
#define MENU_JOIN_APPLETALK_NETGAME     29
#define MENU_START_UDP_NETGAME          30 // UDP/IP support copied from d1x
#define MENU_JOIN_UDP_NETGAME           31
#define MENU_START_KALI_NETGAME         32 // Kali support copied from d1x
#define MENU_JOIN_KALI_NETGAME          33
#define MENU_START_MCAST4_NETGAME       34 // UDP/IP over multicast networks
#define MENU_JOIN_MCAST4_NETGAME        35

//ADD_ITEM("Start netgame...", MENU_START_NETGAME, -1 );
//ADD_ITEM("Send net message...", MENU_SEND_NET_MESSAGE, -1 );

#define ADD_ITEM(t,value,key)  do { m[num_options].type=NM_TYPE_MENU; m[num_options].text=t; menu_choice[num_options]=value;num_options++; } while (0)

//unused - extern int last_joy_time;               //last time the joystick was used
#ifndef NDEBUG
extern int Speedtest_on;
#else
#define Speedtest_on 0
#endif

void do_sound_menu();
void do_toggles_menu();

ubyte do_auto_demo = 1;                 // Flag used to enable auto demo starting in main menu.
cvar_t Player_default_difficulty = { "skill", "1", 1 }; // Last difficulty level chosen by the player
cvar_t Auto_leveling_on = { "AutoLeveling", "1", 1 };
cvar_t Guided_in_big_window = { "GuidedBig", "0", 1 };
int Menu_draw_copyright = 0;
int EscortHotKeys=1;

// Function Prototypes added after LINTING
void do_option(int select);
void do_detail_level_menu_custom(void);
void do_new_game_menu(void);
#ifdef NETWORK
void do_multi_player_menu(void);
void ipx_set_driver(int ipx_driver);
#endif //NETWORK

//returns the number of demo files on the disk
int newdemo_count_demos();
extern ubyte Version_major,Version_minor;

// ------------------------------------------------------------------------
void autodemo_menu_check(int nitems, newmenu_item * items, int *last_key, int citem )
{
	int curtime;

	//draw copyright message
	if ( Menu_draw_copyright )              {
		int w,h,aw;

		Menu_draw_copyright = 0;
		gr_set_current_canvas(NULL);
		gr_set_curfont(GAME_FONT);
		gr_set_fontcolor(BM_XRGB(6,6,6),-1);

		gr_get_string_size("V2.2", &w, &h, &aw );
	
			gr_printf(0x8000,grd_curcanv->cv_bitmap.bm_h-GAME_FONT->ft_h-2,TXT_COPYRIGHT);
			#ifdef MACINTOSH	// print out fix level as well if it exists
				if (Version_fix != 0)
				{
					gr_get_string_size("V2.2.2", &w, &h, &aw );
					gr_printf(grd_curcanv->cv_bitmap.bm_w-w-2,
							  grd_curcanv->cv_bitmap.bm_h-GAME_FONT->ft_h-2,
							  "V%d.%d.%d",
							  Version_major,Version_minor,Version_fix);
				}
				else
				{
					gr_printf(grd_curcanv->cv_bitmap.bm_w-w-2,
							  grd_curcanv->cv_bitmap.bm_h-GAME_FONT->ft_h-2,
							  "V%d.%d",
							  Version_major,Version_minor);
				}
			#else
				gr_printf(grd_curcanv->cv_bitmap.bm_w-w-2,grd_curcanv->cv_bitmap.bm_h-GAME_FONT->ft_h-2,"V%d.%d",Version_major,Version_minor);
			#endif

		//say this is vertigo version
		if (cfexist(MISSION_DIR "d2x.hog")) {
			gr_set_curfont(MEDIUM2_FONT);
			gr_printf(MenuHires?495:248, MenuHires?88:37, "Vertigo");
		}
	}
	
	// Don't allow them to hit ESC in the main menu.
	if (*last_key==KEY_ESC) *last_key = 0;

	if ( do_auto_demo )     {
		curtime = timer_get_approx_seconds();
		//if ( ((keyd_time_when_last_pressed+i2f(20)) < curtime) && ((last_joy_time+i2f(20)) < curtime) && (!Speedtest_on)  ) {
		#ifndef MACINTOSH		// for now only!!!!
		if ( ((keyd_time_when_last_pressed+i2f(25)) < curtime) && (!Speedtest_on)  )
		#else
		if ( (keyd_time_when_last_pressed+i2f(40)) < curtime )
		#endif
		{
			int n_demos;

			n_demos = newdemo_count_demos();

try_again:;

			if ((d_rand() % (n_demos+1)) == 0)
			{
				#ifndef SHAREWARE
#ifdef OGL
					Screen_mode = -1;
#endif
					PlayMovie("intro.mve",0);
					songs_play_song(SONG_TITLE,1);
					*last_key = -3; //exit menu to force redraw even if not going to game mode. -3 tells menu system not to restore
					set_screen_mode(SCREEN_MENU);
				#endif // end of ifndef shareware
			}
			else {
				WIN(HideCursorW());
				keyd_time_when_last_pressed = curtime;                  // Reset timer so that disk won't thrash if no demos.
				newdemo_start_playback(NULL);           // Randomly pick a file
				if (Newdemo_state == ND_STATE_PLAYBACK) {
					Function_mode = FMODE_GAME;
					*last_key = -3; //exit menu to get into game mode. -3 tells menu system not to restore
				}
				else
					goto try_again;	//keep trying until we get a demo that works
			}
		}
	}
}

//static int First_time = 1;
static int main_menu_choice = 0;

//      -----------------------------------------------------------------------------
//      Create the main menu.
void create_main_menu(newmenu_item *m, int *menu_choice, int *callers_num_options)
{
	int     num_options;

	#ifndef DEMO_ONLY
	num_options = 0;

	set_screen_mode (SCREEN_MENU);

	ADD_ITEM(TXT_NEW_GAME,MENU_NEW_GAME,KEY_N);

	ADD_ITEM(TXT_LOAD_GAME,MENU_LOAD_GAME,KEY_L);

#ifdef NETWORK
	ADD_ITEM(TXT_MULTIPLAYER_,MENU_MULTIPLAYER,-1);
#endif

	ADD_ITEM(TXT_OPTIONS_, MENU_CONFIG, -1 );
	ADD_ITEM(TXT_CHANGE_PILOTS,MENU_NEW_PLAYER,unused);
	ADD_ITEM(TXT_VIEW_DEMO,MENU_DEMO_PLAY,0);
	ADD_ITEM(TXT_VIEW_SCORES,MENU_VIEW_SCORES,KEY_V);
	if (cfexist("orderd2.pcx")) /* SHAREWARE */
		ADD_ITEM(TXT_ORDERING_INFO,MENU_ORDER_INFO,-1);
	ADD_ITEM(TXT_CREDITS,MENU_SHOW_CREDITS,-1);
	#endif
	ADD_ITEM(TXT_QUIT,MENU_QUIT,KEY_Q);

	#ifndef RELEASE
	if (!(Game_mode & GM_MULTI ))   {
		//m[num_options].type=NM_TYPE_TEXT;
		//m[num_options++].text=" Debug options:";

		ADD_ITEM("  Load level...",MENU_LOAD_LEVEL ,KEY_N);
		#ifdef EDITOR
		ADD_ITEM("  Editor", MENU_EDITOR, KEY_E);
		#endif
	}

	//ADD_ITEM( "  Play song", MENU_PLAY_SONG, -1 );
	#endif

	*callers_num_options = num_options;
}

//returns number of item chosen
int DoMenu() 
{
	int menu_choice[25];
	newmenu_item m[25];
	int num_options = 0;

	load_palette(MENU_PALETTE,0,1);		//get correct palette

	if ( Players[Player_num].callsign[0]==0 )       {
		RegisterPlayer();
		return 0;
	}
	
	if ((Game_mode & GM_SERIAL) || (Game_mode & GM_MODEM)) {
		do_option(MENU_START_SERIAL);
		return 0;
	}

	do {
		create_main_menu(m, menu_choice, &num_options); // may have to change, eg, maybe selected pilot and no save games.

		keyd_time_when_last_pressed = timer_get_fixed_seconds();                // .. 20 seconds from now!
		if (main_menu_choice < 0 )
			main_menu_choice = 0;
		Menu_draw_copyright = 1;
		main_menu_choice = newmenu_do2( "", NULL, num_options, m, autodemo_menu_check, main_menu_choice, Menu_pcx_name);
		if ( main_menu_choice > -1 ) do_option(menu_choice[main_menu_choice]);
	} while( Function_mode==FMODE_MENU );

//      if (main_menu_choice != -2)
//              do_auto_demo = 0;               // No more auto demos
	if ( Function_mode==FMODE_GAME )
		gr_palette_fade_out( gr_palette, 32, 0 );

	return main_menu_choice;
}

extern void show_order_form(void);      // John didn't want this in inferno.h so I just externed it.

//returns flag, true means quit menu
void do_option ( int select) 
{
	switch (select) {
		case MENU_NEW_GAME:
			do_new_game_menu();
			break;
		case MENU_GAME:
			break;
		case MENU_DEMO_PLAY:
		{
			char demo_file[16];
			if (newmenu_get_filename(TXT_SELECT_DEMO, "dem", demo_file, 1))
				newdemo_start_playback(demo_file);
			break;
		}
		case MENU_LOAD_GAME:
			state_restore_all(0, 0, NULL);
			break;
		#ifdef EDITOR
		case MENU_EDITOR:
			Function_mode = FMODE_EDITOR;
			init_cockpit();
			break;
		#endif
		case MENU_VIEW_SCORES:
			gr_palette_fade_out( gr_palette,32,0 );
			scores_view(-1);
			break;
#if 1 //def SHAREWARE
		case MENU_ORDER_INFO:
			show_order_form();
			break;
#endif
		case MENU_QUIT:
			#ifdef EDITOR
			if (! SafetyCheck()) break;
			#endif
			gr_palette_fade_out( gr_palette,32,0);
			Function_mode = FMODE_EXIT;
			break;
		case MENU_NEW_PLAYER:
			RegisterPlayer();               //1 == allow escape out of menu
			break;

		case MENU_HELP:
			do_show_help();
			break;

#ifndef RELEASE

		case MENU_PLAY_SONG:    {
				int i;
				char * m[MAX_NUM_SONGS];

				for (i=0;i<Num_songs;i++) {
					m[i] = Songs[i].filename;
				}
				i = newmenu_listbox( "Select Song", Num_songs, m, 1, NULL );

				if ( i > -1 )   {
					songs_play_song( i, 0 );
				}
			}
			break;
	case MENU_LOAD_LEVEL:
		if (Current_mission || select_mission(0, "Load Level\n\nSelect mission"))
		{
			newmenu_item m;
			char text[10]="";
			int new_level_num;

			m.type=NM_TYPE_INPUT; m.text_len = 10; m.text = text;

			newmenu_do( NULL, "Enter level to load", 1, &m, NULL );

			new_level_num = atoi(m.text);

			if (new_level_num!=0 && new_level_num>=Last_secret_level && new_level_num<=Last_level)  {
				gr_palette_fade_out( gr_palette, 32, 0 );
				StartNewGame(new_level_num);
			}
		}
		break;

#endif //ifndef RELEASE


#ifdef NETWORK
		//case MENU_START_TCP_NETGAME:
		//case MENU_JOIN_TCP_NETGAME:
		case MENU_START_IPX_NETGAME:
		case MENU_JOIN_IPX_NETGAME:
		case MENU_START_UDP_NETGAME:
		case MENU_JOIN_UDP_NETGAME:
		case MENU_START_KALI_NETGAME:
		case MENU_JOIN_KALI_NETGAME:
		case MENU_START_MCAST4_NETGAME:
		case MENU_JOIN_MCAST4_NETGAME:
//			load_mission(Builtin_mission_num);
#ifdef MACINTOSH
			Network_game_type = IPX_GAME;
#endif
//			WIN(ipx_create_read_thread());
			switch (select & ~0x1) {
			case MENU_START_IPX_NETGAME: ipx_set_driver(IPX_DRIVER_IPX); break;
			case MENU_START_UDP_NETGAME: ipx_set_driver(IPX_DRIVER_UDP); break;
			case MENU_START_KALI_NETGAME: ipx_set_driver(IPX_DRIVER_KALI); break;
			case MENU_START_MCAST4_NETGAME: ipx_set_driver(IPX_DRIVER_MCAST4); break;
			default: Int3();
			}

			if ((select & 0x1) == 0) // MENU_START_*_NETGAME
				network_start_game();
			else // MENU_JOIN_*_NETGAME
				network_join_game();
			break;

#ifdef MACINTOSH
		case MENU_START_APPLETALK_NETGAME:
//			load_mission(Builtin_mission_num);
			#ifdef MACINTOSH
			Network_game_type = APPLETALK_GAME;
			#endif
			network_start_game();
			break;

		case MENU_JOIN_APPLETALK_NETGAME:
//			load_mission(Builtin_mission_num);
			#ifdef MACINTOSH
			Network_game_type = APPLETALK_GAME;
			#endif
			network_join_game();
			break;
#endif
#if 0
		case MENU_START_TCP_NETGAME:
		case MENU_JOIN_TCP_NETGAME:
			nm_messagebox (TXT_SORRY,1,TXT_OK,"Not available in shareware version!");
			// DoNewIPAddress();
			break;
#endif
		case MENU_START_SERIAL:
			com_main_menu();
			break;
		case MENU_MULTIPLAYER:
			do_multi_player_menu();
			break;
#endif //NETWORK
		case MENU_CONFIG:
			do_options_menu();
			break;
		case MENU_SHOW_CREDITS:
			gr_palette_fade_out( gr_palette,32,0);
			songs_stop_all();
			credits_show(NULL); 
			break;
		default:
			Error("Unknown option %d in do_option",select);
			break;
	}

}

int do_difficulty_menu()
{
	int s;
	newmenu_item m[5];

	m[0].type=NM_TYPE_MENU; m[0].text=MENU_DIFFICULTY_TEXT(0);
	m[1].type=NM_TYPE_MENU; m[1].text=MENU_DIFFICULTY_TEXT(1);
	m[2].type=NM_TYPE_MENU; m[2].text=MENU_DIFFICULTY_TEXT(2);
	m[3].type=NM_TYPE_MENU; m[3].text=MENU_DIFFICULTY_TEXT(3);
	m[4].type=NM_TYPE_MENU; m[4].text=MENU_DIFFICULTY_TEXT(4);

	s = newmenu_do1( NULL, TXT_DIFFICULTY_LEVEL, NDL, m, NULL, Difficulty_level);

	if (s > -1 )    {
		if (s != Difficulty_level)
		{       
			cvar_setint(&Player_default_difficulty, s);
			WriteConfigFile();
		}
		Difficulty_level = s;
		mprintf((0, "%s %s %i\n", TXT_DIFFICULTY_LEVEL, TXT_SET_TO, Difficulty_level));
		return 1;
	}
	return 0;
}

int     Max_debris_objects, Max_objects_onscreen_detailed;
int     Max_linear_depth_objects;

sbyte   Object_complexity=2, Object_detail=2;
sbyte   Wall_detail=2, Wall_render_depth=2, Debris_amount=2, SoundChannels = 2;

sbyte   Render_depths[NUM_DETAIL_LEVELS-1] =                        { 6,  9, 12, 15, 50};
sbyte   Max_perspective_depths[NUM_DETAIL_LEVELS-1] =               { 1,  2,  3,  5,  8};
sbyte   Max_linear_depths[NUM_DETAIL_LEVELS-1] =                    { 3,  5,  7, 10, 50};
sbyte   Max_linear_depths_objects[NUM_DETAIL_LEVELS-1] =            { 1,  2,  3,  7, 20};
sbyte   Max_debris_objects_list[NUM_DETAIL_LEVELS-1] =              { 2,  4,  7, 10, 15};
sbyte   Max_objects_onscreen_detailed_list[NUM_DETAIL_LEVELS-1] =   { 2,  4,  7, 10, 15};
sbyte   Smts_list[NUM_DETAIL_LEVELS-1] =                            { 2,  4,  8, 16, 50};   //      threshold for models to go to lower detail model, gets multiplied by obj->size
sbyte   Max_sound_channels[NUM_DETAIL_LEVELS-1] =                   { 2,  4,  8, 12, 16};

//      -----------------------------------------------------------------------------
//      Set detail level based stuff.
//      Note: Highest detail level (detail_level == NUM_DETAIL_LEVELS-1) is custom detail level.
void set_detail_level_parameters(int detail_level)
{
	Assert((detail_level >= 0) && (detail_level < NUM_DETAIL_LEVELS));

	if (detail_level < NUM_DETAIL_LEVELS-1) {
		Render_depth = Render_depths[detail_level];
		Max_perspective_depth = Max_perspective_depths[detail_level];
		Max_linear_depth = Max_linear_depths[detail_level];
		Max_linear_depth_objects = Max_linear_depths_objects[detail_level];

		Max_debris_objects = Max_debris_objects_list[detail_level];
		Max_objects_onscreen_detailed = Max_objects_onscreen_detailed_list[detail_level];

		Simple_model_threshhold_scale = Smts_list[detail_level];

		digi_set_max_channels( Max_sound_channels[ detail_level ] );

		//      Set custom menu defaults.
		Object_complexity = detail_level;
		Wall_render_depth = detail_level;
		Object_detail = detail_level;
		Wall_detail = detail_level;
		Debris_amount = detail_level;
		SoundChannels = detail_level;
	}
}

//      -----------------------------------------------------------------------------
void do_detail_level_menu(void)
{
	int s;
	newmenu_item m[8];

	m[0].type=NM_TYPE_MENU; m[0].text=MENU_DETAIL_TEXT(0);
	m[1].type=NM_TYPE_MENU; m[1].text=MENU_DETAIL_TEXT(1);
	m[2].type=NM_TYPE_MENU; m[2].text=MENU_DETAIL_TEXT(2);
	m[3].type=NM_TYPE_MENU; m[3].text=MENU_DETAIL_TEXT(3);
	m[4].type=NM_TYPE_MENU; m[4].text=MENU_DETAIL_TEXT(4);
	m[5].type=NM_TYPE_TEXT; m[5].text="";
	m[6].type=NM_TYPE_MENU; m[6].text=MENU_DETAIL_TEXT(5);
	m[7].type=NM_TYPE_CHECK; m[7].text="Show High Res movies"; m[7].value=MovieHires.intval;

	s = newmenu_do1( NULL, TXT_DETAIL_LEVEL , NDL+3, m, NULL, Detail_level);

	if (s > -1 )    {
		switch (s)      {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
				Detail_level = s;
				mprintf((0, "Detail level set to %i\n", Detail_level));
				set_detail_level_parameters(Detail_level);
				break;
			case 6:
				Detail_level = 5;
				do_detail_level_menu_custom();
				break;
		}
	}
	cvar_setint( &MovieHires, m[7].value );
}

//      -----------------------------------------------------------------------------
void do_detail_level_menu_custom_menuset(int nitems, newmenu_item * items, int *last_key, int citem )
{
	Object_complexity = items[0].value;
	Object_detail = items[1].value;
	Wall_detail = items[2].value;
	Wall_render_depth = items[3].value;
	Debris_amount = items[4].value;
	SoundChannels = items[5].value;
}

void set_custom_detail_vars(void)
{
	Render_depth = Render_depths[Wall_render_depth];

	Max_perspective_depth = Max_perspective_depths[Wall_detail];
	Max_linear_depth = Max_linear_depths[Wall_detail];

	Max_debris_objects = Max_debris_objects_list[Debris_amount];

	Max_objects_onscreen_detailed = Max_objects_onscreen_detailed_list[Object_complexity];
	Simple_model_threshhold_scale = Smts_list[Object_complexity];
	Max_linear_depth_objects = Max_linear_depths_objects[Object_detail];

	digi_set_max_channels( Max_sound_channels[ SoundChannels ] );
	
}

#define	DL_MAX	10

//      -----------------------------------------------------------------------------

void do_detail_level_menu_custom(void)
{
	int	count;
	int	s=0;
	newmenu_item m[DL_MAX];

	do {
		count = 0;
		m[count].type = NM_TYPE_SLIDER;
		m[count].text = TXT_OBJ_COMPLEXITY;
		m[count].value = Object_complexity;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_SLIDER;
		m[count].text = TXT_OBJ_DETAIL;
		m[count].value = Object_detail;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_SLIDER;
		m[count].text = TXT_WALL_DETAIL;
		m[count].value = Wall_detail;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_SLIDER;
		m[count].text = TXT_WALL_RENDER_DEPTH;
		m[count].value = Wall_render_depth;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_SLIDER;
		m[count].text= TXT_DEBRIS_AMOUNT;
		m[count].value = Debris_amount;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_SLIDER;
		m[count].text= TXT_SOUND_CHANNELS;
		m[count].value = SoundChannels;
		m[count].min_value = 0;
		m[count++].max_value = NDL-1;

		m[count].type = NM_TYPE_TEXT;
		m[count++].text= TXT_LO_HI;

		Assert(count < DL_MAX);

		s = newmenu_do1( NULL, TXT_DETAIL_CUSTOM, count, m, do_detail_level_menu_custom_menuset, s);
	} while (s > -1);

	set_custom_detail_vars();
}

#ifndef MACINTOSH
uint32_t Default_display_mode = SM(320,200);
uint32_t Current_display_mode = SM(320,200);
#else
uint32_t Default_display_mode = SM(640,480);
uint32_t Current_display_mode = SM(640,480);
#endif

extern int MenuHiresAvailable;


void set_display_mode(uint32_t mode)
{
	if ((Current_display_mode == 0xffffffff) || (VR_render_mode != VR_NONE)) //special VR mode
		return;								//...don't change

	if (!MenuHiresAvailable && (mode != SM(320,400)))
		mode = SM(320,200);

	if (vid_check_mode(mode) != 0) // can't do mode
		mode = Default_display_mode;

	Current_display_mode = mode;

	if (Current_display_mode != 0xffffffff) {
		short flags = 0;

		// flags need to be refacored
		switch (mode)
		{
			case SM(320, 200):
			case SM(640, 480):
				flags = VRF_ALLOW_COCKPIT + VRF_COMPATIBLE_MENUS;
				break;
			case SM(320, 400):
				flags = VRF_USE_PAGING;
				break;
			case SM(640, 400):
			case SM(800, 600):
			case SM(1024, 768):
			case SM(1280, 1024):
				flags = VRF_COMPATIBLE_MENUS;
				break;
		}

#ifdef __MSDOS__
		if (FindArg("-nodoublebuffer"))
#endif
		{
			flags &= ~VRF_USE_PAGING;
		}

		game_init_render_buffers(mode, SM_W(mode), SM_H(mode), VR_NONE, flags);
		Default_display_mode = Current_display_mode;
	}

	Screen_mode = -1;		//force screen reset
}


void do_screen_res_menu()
{
#define N_SCREENRES_ITEMS 12
#ifdef VID_SUPPORTS_FULLSCREEN_TOGGLE
	int fullscreenc;
#endif
	newmenu_item m[N_SCREENRES_ITEMS];
	int citem;
	int i = 0, customc;
	int n_items;
	uint32_t modes[N_SCREENRES_ITEMS];
	uint32_t screen_mode = 0;
	char customres[16];

	if ((Current_display_mode == 0xffffffff) || (VR_render_mode != VR_NONE)) { //special VR mode
		nm_messagebox(TXT_SORRY, 1, TXT_OK, 
				"You may not change screen\n"
				"resolution when VR modes enabled.");
		return;
	}

	m[i].type=NM_TYPE_TEXT;  m[i].value=0;               m[i].text="Modes w/ Cockpit:";  modes[i] = 0;             i++;
#ifndef MACINTOSH
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 320x200";           modes[i] = SM(320,200);   i++;
#endif
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 640x480";           modes[i] = SM(640,480);   i++;
	m[i].type=NM_TYPE_TEXT;  m[i].value=0;               m[i].text="Modes w/o Cockpit:"; modes[i] = 0;             i++;
#ifndef MACINTOSH
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 320x400";           modes[i] = SM(320,400);   i++;
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 640x400";           modes[i] = SM(640,400);   i++;
#endif
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 800x600";           modes[i] = SM(800,600);   i++;
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 1024x768";          modes[i] = SM(1024,768);  i++;
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text=" 1280x1024";         modes[i] = SM(1280,1024); i++;

	customc = i;
	m[i].type=NM_TYPE_RADIO; m[i].value=0; m[i].group=0; m[i].text="Custom:";            modes[i] = 0;             i++;
	sprintf(customres, "%ix%i", SM_W(Current_display_mode), SM_H(Current_display_mode));
	m[i].type = NM_TYPE_INPUT; m[i].text = customres; m[i].text_len = 11; modes[i] = 0; i++;

#ifdef VID_SUPPORTS_FULLSCREEN_TOGGLE
	m[i].type = NM_TYPE_CHECK; m[i].text = "Fullscreen";
	m[i].value = vid_check_fullscreen();
	fullscreenc = i++;
#endif

	n_items = i;

	citem = customc;
	for (i = 0; i < n_items; i++) {
		if (modes[i] == Current_display_mode) {
			citem = i;
			break;
		}
	}

	m[citem].value = 1;

	newmenu_do1( NULL, "Select screen mode", n_items, m, NULL, citem);

#ifdef VID_SUPPORTS_FULLSCREEN_TOGGLE
	if (m[fullscreenc].value != vid_check_fullscreen()) {
		vid_toggle_fullscreen();
		Game_screen_mode = -1;
	}
#endif

	for (i =0;i<n_items;i++)
		if (m[i].value)
			break;

	if (i == customc)
	{
		char *h = strchr(customres, 'x');
		if (!h)
			return;
		screen_mode = SM(atoi(customres), atoi(h+1));
	}
	else
	{
		screen_mode = modes[i];
	}
	
	if (((SM_H(screen_mode) > 320) && !MenuHiresAvailable) || vid_check_mode(screen_mode)) {
		nm_messagebox(TXT_SORRY, 1, TXT_OK, 
				"Cannot set requested\n"
				"mode on this video card.");
		return;
	}

	if (screen_mode != Current_display_mode)
		set_display_mode(screen_mode);

#ifdef MACINTOSH
	reset_cockpit();
#endif

}


void do_new_game_menu()
{
	int new_level_num;

    if (!select_mission(0, "New Game\n\nSelect mission"))
        return;
    
	new_level_num = 1;

	mission_read_config();

	if (Player_highest_level.intval > Last_level)
		cvar_setint(&Player_highest_level, Last_level);

	if (Player_highest_level.intval > 1) {
		newmenu_item m[4];
		char info_text[80];
		char num_text[10];
		int choice;
		int n_items;

try_again:
		sprintf(info_text, "%s %d", TXT_START_ANY_LEVEL, Player_highest_level.intval);

		m[0].type=NM_TYPE_TEXT; m[0].text = info_text;
		m[1].type=NM_TYPE_INPUT; m[1].text_len = 10; m[1].text = num_text;
		n_items = 2;

		#ifdef WINDOWS
		m[2].type = NM_TYPE_TEXT; m[2].text = "";
		m[3].type = NM_TYPE_MENU; m[3].text = "          Ok";
		n_items = 4;
		#endif

		strcpy(num_text,"1");

		choice = newmenu_do( NULL, TXT_SELECT_START_LEV, n_items, m, NULL );

		if (choice==-1 || m[1].text[0]==0)
			return;

		new_level_num = atoi(m[1].text);

		if (!(new_level_num > 0 && new_level_num <= Player_highest_level.intval)) {
			m[0].text = TXT_ENTER_TO_CONT;
			nm_messagebox( NULL, 1, TXT_OK, TXT_INVALID_LEVEL); 
			goto try_again;
		}
	}

	Difficulty_level = Player_default_difficulty.intval;

	if (!do_difficulty_menu())
		return;

	gr_palette_fade_out( gr_palette, 32, 0 );
	StartNewGame(new_level_num);

}

extern void GameLoop(int, int );


void options_menuset(int nitems, newmenu_item * items, int *last_key, int citem )
{
	if ( citem==5)
	{
		gr_palette_set_gamma(items[5].value);
	}

	nitems++;		//kill warning
	last_key++;		//kill warning
}


//added on 8/18/98 by Victor Rachels to add d1x options menu, maxfps setting
//added/edited on 8/18/98 by Victor Rachels to set maxfps always on, max=80
//added/edited on 9/7/98 by Victor Rachels to attempt dir browsing.  failed.

void d2x_options_menu_poll(int nitems, newmenu_item * menus, int * key, int citem)
{
}


void d2x_options_menu()
{
	newmenu_item m[14];
	int i=0;
	int opt = 0;
	int inputs, commands;
#if 0
	int checks;
#endif

	char smaxfps[4];
#if 0
	char shudmaxnumdisp[4];
	char thogdir[64];
	extern int gr_message_color_level;

	sprintf(thogdir,AltHogDir);
#endif
	sprintf(smaxfps,"%d",maxfps);
#if 0
	sprintf(shudmaxnumdisp,"%d",HUD_max_num_disp);
#endif

	m[opt].type = NM_TYPE_MENU;  m[opt].text = "D2X Keys"; opt++;

	commands=opt;
#if 0
	//added on 2/2/99 by Victor Rachels for bans
#ifdef NETWORK
	m[opt].type = NM_TYPE_MENU; m[opt].text = "Save bans now"; opt++;
#endif
	//end this section addition - VR
#endif // 0

	m[opt].type = NM_TYPE_TEXT;  m[opt].text = "Maximum Framerate (1-80):";       opt++;


	inputs=opt;
	m[opt].type = NM_TYPE_INPUT; m[opt].text = smaxfps; m[opt].text_len=3;         opt++;
#if 0
	m[opt].type = NM_TYPE_TEXT;  m[opt].text = "Mission Directory";                opt++;
	m[opt].type = NM_TYPE_INPUT; m[opt].text = thogdir; m[opt].text_len=64;        opt++;
	m[opt].type = NM_TYPE_TEXT;  m[opt].text = "Hud Messages lines (1-80):";       opt++;
	m[opt].type = NM_TYPE_INPUT; m[opt].text = shudmaxnumdisp; m[opt].text_len=3;  opt++;
	m[opt].type = NM_TYPE_SLIDER; m[opt].text = "Message colorization level"; m[opt].value=gr_message_color_level;m[opt].min_value=0;m[opt].max_value=3;  opt++;
	checks=opt;
#ifdef __MSDOS__
	m[opt].type = NM_TYPE_CHECK; m[opt].text = "Joy is sidewinder"; m[opt].value=Joy_is_Sidewinder;  opt++;
#endif
#ifdef SUPPORTS_NICEFPS
	m[opt].type = NM_TYPE_CHECK; m[opt].text = "Nice FPS (free cpu cycles)"; m[opt].value = use_nice_fps; opt++;
#endif
#endif // 0

	for(;;)
	{
		i=newmenu_do1( NULL, "D2X options", opt, m, &d2x_options_menu_poll, i);

		if(i>-1)
		{
            if(i<commands)
			{
				switch(i)
				{
				case 0: kconfig(4, "D2X Keys"); break;
				}
			}

#if 0
            //added on 2/4/99 by Victor Rachels for bans
#ifdef NETWORK
            if(i==commands+0)
			{              

				nm_messagebox(NULL,1,TXT_OK, "%i Bans saved",writebans());

			}
#endif
            //end this section addition - VR
#endif // 0

            if(i == inputs+0)
			{
				maxfps = atoi(smaxfps);
				if(maxfps < 1 || maxfps > MAX_FPS)
				{
					nm_messagebox(TXT_ERROR, 1, TXT_OK, "Invalid value for maximum framerate");
					maxfps = MAX_FPS;
					i = (inputs+0);
				}
			}
#if 0
            else if(i==inputs+2)
				cfile_use_alternate_hogdir(thogdir);
			else if(i==inputs+4)
			{
				HUD_max_num_disp = atoi(shudmaxnumdisp);
                if(HUD_max_num_disp < 1||HUD_max_num_disp>HUD_MAX_NUM)
				{
					nm_messagebox(TXT_ERROR, 1, TXT_OK, "Invalid value for hud lines");
					HUD_max_num_disp=4;
					//                   i=(inputs+4);//???
				}
			}
			gr_message_color_level=m[inputs+5].value;

			sprintf(shudmaxnumdisp,"%d",HUD_max_num_disp);
#endif // 0
			sprintf(smaxfps,"%d",maxfps);
			//           m[inputs+0].text=smaxfps;//redundant.. its not going anywhere
#if 0
			sprintf(thogdir,AltHogDir);
			//           m[inputs+2].text=thogdir;//redundant
#endif
		}
		else
			break;
	}

#if 0
	WriteConfigFile();

#ifdef __MSDOS__
	Joy_is_Sidewinder=m[(checks+0)].value;
#endif
#ifdef __linux__
	Joy_is_Sidewinder=0;
#endif
#ifdef SUPPORTS_NICEFPS
	use_nice_fps=m[(checks+0)].value;
#else
	use_nice_fps=0;
#endif
#endif // 0
}

//end edit - Victor Rachels
//end addition - Victor Rachels


void do_options_menu()
{
	newmenu_item m[13];
	int i = 0;

	do {
		m[ 0].type = NM_TYPE_MENU;   m[ 0].text="Sound effects & music...";
		m[ 1].type = NM_TYPE_TEXT;   m[ 1].text="";
		#if defined(MACINTOSH) && defined(APPLE_DEMO)
		m [2].type = NM_TYPE_TEXT;   m[ 2].text="";
		#else
		m[ 2].type = NM_TYPE_MENU;   m[ 2].text=TXT_CONTROLS_;
		#endif
		m[ 3].type = NM_TYPE_MENU;   m[ 3].text=TXT_CAL_JOYSTICK;
		m[ 4].type = NM_TYPE_TEXT;   m[ 4].text="";

		m[5].type = NM_TYPE_SLIDER;
		m[5].text = TXT_BRIGHTNESS;
		m[5].value = gr_palette_get_gamma();
		m[5].min_value = 0;
		m[5].max_value = 16; // CCA too dim, was 8;


		m[ 6].type = NM_TYPE_MENU;   m[ 6].text=TXT_DETAIL_LEVELS;
		m[ 7].type = NM_TYPE_MENU;   m[ 7].text="Screen resolution...";

		m[ 8].type = NM_TYPE_TEXT;   m[ 8].text="";
		m[ 9].type = NM_TYPE_MENU;   m[ 9].text="Primary autoselect ordering...";
		m[10].type = NM_TYPE_MENU;   m[10].text="Secondary autoselect ordering...";
		m[11].type = NM_TYPE_MENU;   m[11].text="Toggles...";

		m[12].type = NM_TYPE_MENU;   m[12].text="D2X options...";

		i = newmenu_do1( NULL, TXT_OPTIONS, sizeof(m)/sizeof(*m), m, options_menuset, i );
			
		switch(i)       {
			case  0: do_sound_menu();			break;
			case  2: joydefs_config();			break;
			case  3: joydefs_calibrate();		break;
			case  6: do_detail_level_menu(); 	break;
			case  7: do_screen_res_menu();		break;
			case  9: ReorderPrimary();			break;
			case 10: ReorderSecondary();		break;
			case 11: do_toggles_menu();			break;
			case 12: d2x_options_menu();        break;
		}

	} while( i>-1 );

	WriteConfigFile();
}

extern int Redbook_playing;
void set_redbook_volume(int volume);

WIN(static BOOL windigi_driver_off=FALSE;)

void sound_menuset(int nitems, newmenu_item * items, int *last_key, int citem )
{
	if ( Config_digi_volume.intval != items[0].value )     {
		cvar_setint( &Config_digi_volume, items[0].value );

		#ifdef WINDOWS
			if (windigi_driver_off) {
				digi_midi_wait();
				digi_init_digi();
				Sleep(500);
				windigi_driver_off = FALSE;
			}
		#endif			
		
		#ifndef MACINTOSH
			digi_set_digi_volume( (Config_digi_volume.intval * 32768) / 8 );
		#else
			digi_set_digi_volume( (Config_digi_volume.intval * 256) / 8 );
		#endif
		digi_play_sample_once( SOUND_DROP_BOMB, F1_0 );
	}

#ifdef WINDOWS
	if (!wmidi_support_volchange()) {
		if (!items[1].value && Config_midi_volume.intval) {
			cvar_setint( &Config_midi_volume, 0 );
			digi_set_midi_volume(0);
			digi_play_midi_song( NULL, NULL, NULL, 0 );
		}
		else if (Config_midi_volume.intval == 0 && items[1].value) {
			digi_set_midi_volume(64);
			cvar_setint( &Config_midi_volume, 4 );
		}
	}
	else 	 // LINK TO BELOW IF
#endif
	if (Config_midi_volume.intval != items[1].value )   {
		cvar_setint( &Config_midi_volume, items[1].value );
		#ifdef WINDOWS
			if (!windigi_driver_off) {
				Sleep(200);
				digi_close_digi();
				Sleep(100);
				windigi_driver_off = TRUE;
			}
		#endif
		#ifndef MACINTOSH
			digi_set_midi_volume( (Config_midi_volume.intval * 128) / 8 );
		#else
			digi_set_midi_volume( (Config_midi_volume.intval * 256) / 8 );
		#endif
	}
#ifdef MACINTOSH
	if (Config_master_volume != items[3].value ) {
		Config_master_volume = items[3].value;
		digi_set_master_volume( Config_master_volume );
		digi_play_sample_once( SOUND_DROP_BOMB, F1_0 );
	}
#endif

	if (Config_redbook_volume.intval != items[2].value )   {
		cvar_setint( &Config_redbook_volume, items[2].value );
		set_redbook_volume(Config_redbook_volume.intval);
	}

	if (items[4].value != (Redbook_playing!=0)) {

		if (items[4].value && FindArg("-noredbook")) {
			nm_messagebox (TXT_SORRY,1,TXT_OK,"Redbook audio has been disabled\non the command line");
			items[4].value = 0;
			items[4].redraw = 1;
		}
		else {
			cvar_setint( &Redbook_enabled, items[4].value );

			mprintf((1, "Redbook_enabled = %d\n", Redbook_enabled.intval));

			if (Function_mode == FMODE_MENU)
				songs_play_song(SONG_TITLE,1);
			else if (Function_mode == FMODE_GAME)
				songs_play_level_song( Current_level_num );
			else
				Int3();

			if (items[4].value && !Redbook_playing) {
				nm_messagebox (TXT_SORRY,1,TXT_OK,"Cannot start CD Music.  Insert\nyour Descent II CD and try again");
				items[4].value = 0;
				items[4].redraw = 1;
			}

			items[1].type = (Redbook_playing?NM_TYPE_TEXT:NM_TYPE_SLIDER);
			items[1].redraw = 1;
			items[2].type = (Redbook_playing?NM_TYPE_SLIDER:NM_TYPE_TEXT);
			items[2].redraw = 1;

		}
	}

	citem++;		//kill warning
}

void do_sound_menu()
{
   newmenu_item m[6];
	int i = 0;

 #ifdef WINDOWS
 	extern BOOL DIGIDriverInit;
 	if (!DIGIDriverInit) windigi_driver_off = TRUE;
 	else windigi_driver_off = FALSE;
 #endif

	do {
		m[ 0].type = NM_TYPE_SLIDER; m[ 0].text=TXT_FX_VOLUME; m[0].value=Config_digi_volume.intval;m[0].min_value=0; m[0].max_value=8;
		m[ 1].type = (Redbook_playing?NM_TYPE_TEXT:NM_TYPE_SLIDER); m[ 1].text="MIDI music volume"; m[1].value=Config_midi_volume.intval;m[1].min_value=0; m[1].max_value=8;

	#ifdef WINDOWS
		if (!wmidi_support_volchange() && !Redbook_playing) {
			m[1].type = NM_TYPE_CHECK;
			m[1].text = "MIDI MUSIC";
			if (Config_midi_volume.intval) m[1].value = 1;
		}
	#endif

		m[ 2].type = (Redbook_playing?NM_TYPE_SLIDER:NM_TYPE_TEXT); m[ 2].text="CD music volume"; m[2].value=Config_redbook_volume.intval;m[2].min_value=0; m[2].max_value=8;
#ifndef MACINTOSH
		m[ 3].type = NM_TYPE_TEXT; m[ 3].text="";
#else
		m[ 3].type = NM_TYPE_SLIDER; m[ 3].text="Sound Manager Volume"; m[3].value=Config_master_volume;m[3].min_value=0; m[3].max_value=8;
#endif
		m[ 4].type = NM_TYPE_CHECK;  m[ 4].text="CD Music (Redbook) enabled"; m[4].value=(Redbook_playing!=0);
		m[ 5].type = NM_TYPE_CHECK;  m[ 5].text=TXT_REVERSE_STEREO; m[5].value=Config_channels_reversed.intval;
				
		i = newmenu_do1( NULL, "Sound Effects & Music", sizeof(m)/sizeof(*m), m, sound_menuset, i );

		cvar_setint( &Redbook_enabled, m[4].value );
		cvar_setint( &Config_channels_reversed, m[5].value );

	} while( i>-1 );

#ifdef WINDOWS
	if (windigi_driver_off) {
		digi_midi_wait();
		Sleep(500);
		digi_init_digi();
		windigi_driver_off=FALSE;
	}
#endif


	if ( Config_midi_volume.intval < 1 )   {
		#ifndef MACINTOSH
			digi_play_midi_song( NULL, NULL, NULL, 0 );
		#else
			digi_play_midi_song(-1, 0);
		#endif
	}

}


#define ADD_CHECK(n,txt,v)  do { m[n].type=NM_TYPE_CHECK; m[n].text=txt; m[n].value=v;} while (0)

void do_toggles_menu()
{
#define N_TOGGLE_ITEMS 6
	newmenu_item m[N_TOGGLE_ITEMS];
	int i = 0;

	do {
		#if defined(MACINTOSH) && defined(USE_ISP)
			if (ISpEnabled())
			{
				m[0].type = NM_TYPE_TEXT; m[0].text = "";
			}
			else
			{
				ADD_CHECK(0, "Ship auto-leveling", Auto_leveling_on.intval);
			}
		#else 
			ADD_CHECK(0, "Ship auto-leveling", Auto_leveling_on.intval);
		#endif
		ADD_CHECK(1, "Show reticle", Reticle_on.intval);
		ADD_CHECK(2, "Missile view", Missile_view_enabled.intval);
		ADD_CHECK(3, "Headlight on when picked up", Headlight_active_default.intval);
		ADD_CHECK(4, "Show guided missile in main display", Guided_in_big_window.intval);
		ADD_CHECK(5, "Escort robot hot keys",EscortHotKeys);
		//ADD_CHECK(6, "Always use 640x480 or greater automap", Automap_always_hires.intval);
		//when adding more options, change N_TOGGLE_ITEMS above

		i = newmenu_do1( NULL, "Toggles", N_TOGGLE_ITEMS, m, NULL, i );
			
		cvar_setint(&Auto_leveling_on,          m[0].value);
		cvar_setint(&Reticle_on,                m[1].value);
		cvar_setint(&Missile_view_enabled,      m[2].value);
		cvar_setint(&Headlight_active_default,  m[3].value);
		cvar_setint(&Guided_in_big_window,      m[4].value);
		EscortHotKeys				= m[5].value;

#if 0
		if (MenuHiresAvailable)
			cvar_setint(&Automap_always_hires,  m[6].value);
		else if (m[6].value)
			nm_messagebox(TXT_SORRY,1,"OK","High Resolution modes are\nnot available on this video card");
#endif
	} while( i>-1 );

}

#ifdef NETWORK
void do_multi_player_menu()
{
	int menu_choice[9];
	newmenu_item m[9];
	int choice = 0, num_options = 0;
	int old_game_mode;

	do {
//		WIN(ipx_destroy_read_thread());

		old_game_mode = Game_mode;
		num_options = 0;

#ifdef NATIVE_IPX
		ADD_ITEM(TXT_START_IPX_NET_GAME, MENU_START_IPX_NETGAME, -1);
		ADD_ITEM(TXT_JOIN_IPX_NET_GAME, MENU_JOIN_IPX_NETGAME, -1);
#endif //NATIVE_IPX
		//ADD_ITEM(TXT_START_TCP_NET_GAME, MENU_START_TCP_NETGAME, -1);
		//ADD_ITEM(TXT_JOIN_TCP_NET_GAME, MENU_JOIN_TCP_NETGAME, -1);
		ADD_ITEM("Start UDP/IP Netgame", MENU_START_UDP_NETGAME, -1);
		ADD_ITEM("Join UDP/IP Netgame\n", MENU_JOIN_UDP_NETGAME, -1);
		ADD_ITEM("Start Multicast UDP/IP Netgame", MENU_START_MCAST4_NETGAME, -1);
		ADD_ITEM("Join Multicast UDP/IP Netgame\n", MENU_JOIN_MCAST4_NETGAME, -1);
#ifdef KALINIX
		ADD_ITEM("Start Kali Netgame", MENU_START_KALI_NETGAME, -1);
		ADD_ITEM("Join Kali Netgame\n", MENU_JOIN_KALI_NETGAME, -1);
#endif // KALINIX

#ifdef MACINTOSH
		ADD_ITEM("Start Appletalk Netgame", MENU_START_APPLETALK_NETGAME, -1 );
		ADD_ITEM("Join Appletalk Netgame\n", MENU_JOIN_APPLETALK_NETGAME, -1 );
#endif

		ADD_ITEM(TXT_MODEM_GAME, MENU_START_SERIAL, -1);

		choice = newmenu_do1( NULL, TXT_MULTIPLAYER, num_options, m, NULL, choice );
		
		if ( choice > -1 )      
			do_option(menu_choice[choice]);
	
		if (old_game_mode != Game_mode)
			break;          // leave menu

	} while( choice > -1 );

}

/*
 * ipx_set_driver was called do_network_init and located in main/inferno
 * before the change which allows the user to choose the network driver
 * from the game menu instead of having to supply command line args.
 */
void ipx_set_driver(int ipx_driver)
{
	ipx_close();

	if (!FindArg("-nonetwork")) {
		int ipx_error;
		int socket = 0, t;

		con_printf(CON_VERBOSE, "\n%s ", TXT_INITIALIZING_NETWORK);

		if ((t = FindArg("-socket")))
			socket = atoi(Args[t + 1]);

		arch_ipx_set_driver(ipx_driver);

		if ((ipx_error = ipx_init(IPX_DEFAULT_SOCKET + socket)) == IPX_INIT_OK) {
			con_printf(CON_VERBOSE, "%s %d.\n", TXT_IPX_CHANNEL, socket );
			Network_active = 1;
		} else {
			switch(ipx_error) {
			case IPX_NOT_INSTALLED: con_printf(CON_VERBOSE, "%s\n", TXT_NO_NETWORK); break;
			case IPX_SOCKET_TABLE_FULL: con_printf(CON_VERBOSE, "%s 0x%x.\n", TXT_SOCKET_ERROR, IPX_DEFAULT_SOCKET+socket); break;
			case IPX_NO_LOW_DOS_MEM: con_printf(CON_VERBOSE, "%s\n", TXT_MEMORY_IPX ); break;
			default: con_printf(CON_VERBOSE, "%s %d", TXT_ERROR_IPX, ipx_error );
			}
			con_printf(CON_VERBOSE, "%s\n",TXT_NETWORK_DISABLED);
			Network_active = 0;		// Assume no network
		}
		ipx_read_user_file("descent.usr");
		ipx_read_network_file("descent.net");
		//@@if (FindArg("-dynamicsockets"))
		//@@	Network_allow_socket_changes = 1;
		//@@else
		//@@	Network_allow_socket_changes = 0;
	} else {
		con_printf(CON_VERBOSE, "%s\n", TXT_NETWORK_DISABLED);
		Network_active = 0;		// Assume no network
	}
}

void DoNewIPAddress ()
 {
  newmenu_item m[4];
  char IPText[30];
  int choice;

  m[0].type=NM_TYPE_TEXT; m[0].text = "Enter an address or hostname:";
  m[1].type=NM_TYPE_INPUT; m[1].text_len = 50; m[1].text = IPText;
  IPText[0]=0;

  choice = newmenu_do( NULL, "Join a TCPIP game", 2, m, NULL );

  if (choice==-1 || m[1].text[0]==0)
   return;

  nm_messagebox (TXT_SORRY,1,TXT_OK,"That address is not valid!");
 }

#endif // NETWORK
