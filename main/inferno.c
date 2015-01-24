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
 * inferno.c: Entry point of program (main procedure)
 *
 * After main initializes everything, most of the time is spent in the loop
 * while (Function_mode != FMODE_EXIT)
 * In this loop, the main menu is brought up first.
 *
 * main() for Inferno
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

char copyright[] = "DESCENT II  COPYRIGHT (C) 1994-1996 PARALLAX SOFTWARE CORPORATION";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef __unix__
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <physfs.h>

#include "pstypes.h"
#include "strutil.h"
#include "console.h"
#include "gr.h"
#include "fix.h"
#include "vecmat.h"
#include "mono.h"
#include "key.h"
#include "timer.h"
#include "3d.h"
#include "inferno.h"
#include "inferno.h"
#include "error.h"
#include "game.h"
#include "segment.h"		//for Side_to_verts
#include "u_mem.h"
#include "segpoint.h"
#include "screens.h"
#include "texmap.h"
#include "texmerge.h"
#include "menu.h"
#include "wall.h"
#include "polyobj.h"
#include "effects.h"
#include "digi.h"
#include "palette.h"
#include "args.h"
#include "sounds.h"
#include "titles.h"
#include "player.h"
#include "text.h"
#include "newdemo.h"
#ifdef NETWORK
#include "network.h"
#include "modem.h"
#endif
#include "gamefont.h"
#include "kconfig.h"
#include "mouse.h"
#include "joy.h"
#include "newmenu.h"
#include "desc_id.h"
#include "config.h"
#include "joydefs.h"
#include "multi.h"
#include "songs.h"
#include "cfile.h"
#include "gameseq.h"
#include "gamepal.h"
#include "mission.h"
#include "movie.h"

// #  include "3dfx_des.h"

//added on 9/30/98 by Matt Mueller for selectable automap modes
#include "automap.h"
//end addition -MM

#include "../texmap/scanline.h" //for select_tmap -MM

#ifdef EDITOR
#include "editor/editor.h"
#include "editor/kdefs.h"
#include "ui.h"
#endif

#ifndef __MSDOS__
#include <SDL.h>
#endif

#include "vers_id.h"

void mem_init(void);
void arch_init(void);
void arch_init_start(void);

//Current version number

ubyte Version_major = 1;		//FULL VERSION
ubyte Version_minor = 2;

//static const char desc_id_checksum_str[] = DESC_ID_CHKSUM_TAG "0000"; // 4-byte checksum
char desc_id_exit_num = 0;

int Function_mode=FMODE_MENU;		//game or editor?
int Screen_mode=-1;					//game screen or editor screen?

//--unused-- grs_bitmap Inferno_bitmap_title;

int WVIDEO_running=0;		//debugger can set to 1 if running

#ifdef __WATCOMC__
int __far descent_critical_error_handler( unsigned deverr, unsigned errcode, unsigned __far * devhdr );
#endif

void check_joystick_calibration(void);

void show_order_form(void);

//--------------------------------------------------------------------------

extern int piggy_low_memory;


int descent_critical_error = 0;
unsigned descent_critical_deverror = 0;
unsigned descent_critical_errcode = 0;

extern int Network_allow_socket_changes;

extern void vfx_set_palette_sub(ubyte *);

extern int VR_low_res;

#define LINE_LEN	100

//read help from a file & print to screen
void print_commandline_help()
{
	CFILE *ifile;
	int have_binary=0;
	char line[LINE_LEN];

	ifile = cfopen("help.tex","rb");
	if (!ifile) {
		ifile = cfopen("help.txb","rb");
		if (!ifile)
			Warning("Cannot load help text file.");
		have_binary = 1;
	}

	if (ifile)
	{
        char *end;
        
		while ((end = cfgets(line,LINE_LEN,ifile))) {

			if (have_binary)
				decode_text_line (line);

            // This is the only use of cfgets that needs the CR
			strcat(end, "\n");

			if (line[0] == ';')
				continue;		//don't show comments

			printf("%s",line);

		}

		cfclose(ifile);

	}

//	printf( " Diagnostic:\n\n");
//	printf( "  -emul           %s\n", "Certain video cards need this option in order to run game");
//	printf(	"  -ddemul         %s\n", "If -emul doesn't work, use this option");
//	printf( "\n");
#ifdef EDITOR
	printf( " Editor Options:\n\n");
	printf( "  -autoload <file>%s\n", "Autoload a level in the editor");
	printf( "  -hoarddata      %s\n","Make the hoard ham file from some files, then exit");
//	printf( "  -nobm           %s\n","FIXME: Undocumented");
	printf( "\n");
#endif
	printf( " D2X Options:\n\n");
	printf( "  -noredundancy   %s\n", "Do not send messages when picking up redundant items in multi");
	printf( "  -shortpackets   %s\n", "Set shortpackets to default as on");
#ifdef OGL // currently only does anything on ogl build, so don't advertise othewise.
	printf("  -renderstats    %s\n", "Enable renderstats info by default");
#endif
	printf( "  -maxfps <n>     %s\n", "Set maximum framerate (1-100)");
	printf( "  -notitles       %s\n", "Do not show titlescreens on startup");
	printf( "  -hogdir <dir>   %s\n", "set shared data directory to <dir>");
#ifdef __unix__
	printf( "  -nohogdir       %s\n", "don't try to use shared data directory");
	printf( "  -userdir <dir>  %s\n", "set user dir to <dir> instead of $HOME/.d2x");
#endif
	printf( "  -autoexec <file> %s\n", "autoexec file (execute console commands), defaults to autoexec.cfg");
	printf( "  -ini <file>     %s\n", "option file (alternate to command line), defaults to d2x.ini");
	printf( "  -autodemo       %s\n", "Start in demo mode");
	printf( "  -bigpig         %s\n","FIXME: Undocumented");
	printf( "  -bspgen         %s\n","FIXME: Undocumented");
//	printf( "  -cdproxy        %s\n","FIXME: Undocumented");
#ifndef NDEBUG
	printf( "  -checktime      %s\n","FIXME: Undocumented");
	printf( "  -showmeminfo    %s\n","FIXME: Undocumented");
#endif
//	printf( "  -codereadonly   %s\n","FIXME: Undocumented");
//	printf( "  -cyberimpact    %s\n","FIXME: Undocumented");
	printf( "  -debug          %s\n","Enable very verbose output");
//	printf( "  -debugmode      %s\n","FIXME: Undocumented");
//	printf( "  -disallowgfx    %s\n","FIXME: Undocumented");
//	printf( "  -disallowreboot %s\n","FIXME: Undocumented");
//	printf( "  -dynamicsockets %s\n","FIXME: Undocumented");
//	printf( "  -forcegfx       %s\n","FIXME: Undocumented");
//	printf( "  -hw_3dacc       %s\n","FIXME: Undocumented");
#ifndef RELEASE
	printf( "  -invulnerability %s\n","Make yourself invulnerable");
#endif
	printf( "  -ipxnetwork <num> %s\n","Use IPX network number <num>");
	printf( "  -jasen          %s\n","FIXME: Undocumented");
	printf( "  -joyslow        %s\n","FIXME: Undocumented");
#ifdef NETWORK
	printf( "  -kali           %s\n","use Kali for networking");
#endif
//	printf( "  -logfile        %s\n","FIXME: Undocumented");
	printf( "  -lowresmovies   %s\n","Play low resolution movies if available (for slow machines)");
#if defined(EDITOR) || !defined(MACDATA)
	printf( "  -macdata        %s\n","Read (and, for editor, write) mac data files (swap colors)");
#endif
//	printf( "  -memdbg         %s\n","FIXME: Undocumented");
//	printf( "  -monodebug      %s\n","FIXME: Undocumented");
	printf( "  -nocdrom        %s\n","FIXME: Undocumented");
#ifndef NDEBUG
	printf( "  -nofade         %s\n","Disable fades");
#endif
#ifdef NETWORK
	printf( "  -nomatrixcheat  %s\n","FIXME: Undocumented");
	printf( "  -norankings     %s\n","Disable multiplayer ranking system");
	printf( "  -packets <num>  %s\n","Specifies the number of packets per second\n");
//	printf( "  -showaddress    %s\n","FIXME: Undocumented");
	printf( "  -socket         %s\n","FIXME: Undocumented");
#endif
#if !defined(MACINTOSH) && !defined(WINDOWS)
	printf( "  -nomixer        %s\n","Don't crank music volume");
#endif
//	printf( "  -nomodex        %s\n","FIXME: Undocumented");
#ifndef RELEASE
	printf( "  -nomovies       %s\n","Don't play movies");
	printf( "  -noscreens      %s\n","Skip briefing screens");
#endif
	printf( "  -noredbook      %s\n","Disable redbook audio");
	printf( "  -norun          %s\n","Bail out after initialization");
//	printf( "  -ordinaljoy     %s\n","FIXME: Undocumented");
//	printf( "  -rtscts         %s\n","Same as -ctsrts");
//	printf( "  -semiwin        %s\n","Use non-fullscreen mode");
//	printf( "  -specialdevice  %s\n","FIXME: Undocumented");
#ifdef TACTILE
	printf( "  -stickmag       %s\n","FIXME: Undocumented");
#endif
//	printf( "  -stopwatch      %s\n","FIXME: Undocumented");
	printf( "  -subtitles      %s\n","Turn on movie subtitles (English-only)");
//	printf( "  -sysram         %s\n","FIXME: Undocumented");
	printf( "  -text <file>    %s\n","Specify alternate .tex file");
//	printf( "  -tsengdebug1    %s\n","FIXME: Undocumented");
//	printf( "  -tsengdebug2    %s\n","FIXME: Undocumented");
//	printf( "  -tsengdebug3    %s\n","FIXME: Undocumented");
//	printf( "  -vidram         %s\n","FIXME: Undocumented");
	printf( "  -tmap <t>       %s\n","select texmapper to use (c,fp,i386,pent,ppro)");
#ifdef __MSDOS__
	printf( "  -<X>x<Y>        %s\n", "Change screen resolution. Options:");
	printf( "                     320x100;320x200;320x240;320x400;640x400;640x480;800x600;1024x768\n");
#else
	printf( "  -<X>x<Y>        %s\n", "Change screen resolution to <X> by <Y>");
#endif
	printf("  -niceautomap    %s\n", "Free cpu while doing automap");
	printf( "  -automap<X>x<Y> %s\n","Set automap resolution to <X> by <Y>");
	printf( "  -automap_gameres %s\n","Set automap to use the same resolution as in game");
//	printf( "  -menu<X>x<Y>    %s\n","Set menu resolution to <X> by <Y>");
//	printf( "  -menu_gameres   %s\n","Set menus to use the same resolution as in game");
	printf("  -rearviewtime t %s\n", "time holding rearview key to use toggle mode (default 0.0625 seconds)");
	printf( "\n");

	printf( "D2X System Options:\n\n");
#ifdef __MSDOS__
	printf("  -ihaveabrokenmouse %s\n", "try to make mouse work if it is not currently");
	printf( "  -joy209         %s\n", "Use alternate port 209 for joystick");
#endif
#ifdef GR_SUPPORTS_FULLSCREEN_TOGGLE
	printf( "  -fullscreen     %s\n", "Use fullscreen mode if available");
#endif
#ifdef OGL
	printf( "  -gl_texmagfilt <f> %s\n","set GL_TEXTURE_MAG_FILTER (see readme.d1x)");
	printf( "  -gl_texminfilt <f> %s\n","set GL_TEXTURE_MIN_FILTER (see readme.d1x)");
	printf("  -gl_mipmap      %s\n", "set gl texture filters to \"standard\" (bilinear) mipmapping");
	printf("  -gl_trilinear   %s\n", "set gl texture filters to trilinear mipmapping");
	printf( "  -gl_simple      %s\n","set gl texture filters to gl_nearest for \"original\" look. (default)");
	printf("  -gl_anisotropy <f> %s\n", "set maximum degree of anisotropy to <f>");
	printf( "  -gl_alttexmerge %s\n","use new texmerge, usually uses less ram (default)");
	printf( "  -gl_stdtexmerge %s\n","use old texmerge, uses more ram, but _might_ be a bit faster");
#ifdef GR_SUPPORTS_FULLSCREEN_TOGGLE
	printf( "  -gl_voodoo      %s\n","force fullscreen mode only");
#endif
	printf( "  -gl_16bittextures %s\n","attempt to use 16bit textures");
	printf("  -gl_16bpp       %s\n", "attempt to use 16bit screen mode");
	printf( "  -gl_reticle <r> %s\n","use OGL reticle 0=never 1=above 320x* 2=always");
	printf( "  -gl_intensity4_ok %s\n","FIXME: Undocumented");
	printf( "  -gl_luminance4_alpha4_ok %s\n","FIXME: Undocumented");
	printf( "  -gl_readpixels_ok %s\n","FIXME: Undocumented");
	printf( "  -gl_rgba2_ok    %s\n","FIXME: Undocumented");
//	printf( "  -gl_test1       %s\n","FIXME: Undocumented");
	printf( "  -gl_test2       %s\n","FIXME: Undocumented");
	printf( "  -gl_vidmem      %s\n","FIXME: Undocumented");
#ifdef OGL_RUNTIME_LOAD
	printf( "  -gl_library <l> %s\n","use alternate opengl library");
#endif
#ifdef WGL_VIDEO
	printf("  -gl_refresh <r> %s\n", "set refresh rate (in fullscreen mode)");
#endif
#endif
#ifdef SDL_VIDEO
	printf( "  -nosdlvidmodecheck %s\n", "Some X servers don't like checking vidmode first, so just switch");
	printf( "  -hwsurface      %s\n","FIXME: Undocumented");
#endif
#ifdef NETWORK
	printf("  -udp            %s\n", "Specify options for udp/ip:");
	printf("    @<shift>      %s\n", "  shift udp port base offset");
	printf("    =<HOST_LIST>  %s\n", "  broadcast both local and to HOST_LIST");
	printf("    +<HOST_LIST>  %s\n", "  broadcast only to HOST_LIST");
	printf("                  %s\n", "   HOSTS can be any IP or hostname")
		;
	printf("                  %s\n", "   HOSTS can also be in the form of <address>:<shift>");
	printf("                  %s\n", "   separate multiple HOSTS with a ,");
#endif
#ifdef __unix__
	printf( "  -serialdevice <s> %s\n", "Set serial/modem device to <s>");
	printf( "  -serialread <r> %s\n", "Set serial/modem to read from <r>");
#endif
	printf( "\n Help:\n\n");
	printf( "  -help, -h, -?, ? %s\n", "View this help screen");
	printf( "\n");
}

void do_joystick_init()
{

	if (!FindArg( "-nojoystick" ))	{
		con_printf(CON_VERBOSE, "\n%s", TXT_VERBOSE_6);
		joy_init();
		if ( FindArg( "-joyslow" ))	{
			con_printf(CON_VERBOSE, "\n%s", TXT_VERBOSE_7);
			joy_set_slow_reading(JOY_SLOW_READINGS);
		}
		if ( FindArg( "-joypolled" ))	{
			con_printf(CON_VERBOSE, "\n%s", TXT_VERBOSE_8);
			joy_set_slow_reading(JOY_POLLED_READINGS);
		}
		if ( FindArg( "-joybios" ))	{
			con_printf(CON_VERBOSE, "\n%s", TXT_VERBOSE_9);
			joy_set_slow_reading(JOY_BIOS_READINGS);
		}

	//	Added from Descent v1.5 by John.  Adapted by Samir.
	} else {
		con_printf(CON_VERBOSE, "\n%s", TXT_VERBOSE_10);
	}
}

//set this to force game to run in low res
int disable_high_res=0;

void do_register_player(ubyte *title_pal)
{
	strncpy( Players[Player_num].callsign, config_last_player.string, CALLSIGN_LEN );
	if (strlen(Players[Player_num].callsign))
		return;

	if (!Auto_demo) 	{

		key_flush();

		//now, before we bring up the register player menu, we need to
		//do some stuff to make sure the palette is ok.  First, we need to
		//get our current palette into the 2d's array, so the remapping will
		//work.  Second, we need to remap the fonts.  Third, we need to fill
		//in part of the fade tables so the darkening of the menu edges works

#if 0
		memcpy(gr_palette,title_pal,sizeof(gr_palette));
#endif
		remap_fonts_and_menus(1);
		RegisterPlayer();		//get player's name
	}

}

#define PROGNAME argv[0]

extern char Language[];

//can we do highres menus?
extern int MenuHiresAvailable;

int Inferno_verbose = 0;

//added on 11/18/98 by Victor Rachels to add -mission and -startgame
int start_net_immediately = 0;
//int start_with_mission = 0;
//char *start_with_mission_name;
//end this section addition

#define MENU_HIRES_MODE SM(640,480)

//	DESCENT II by Parallax Software
//		Descent Main

//extern ubyte gr_current_pal[];

#ifdef	EDITOR
int	Auto_exit = 0;
char	Auto_file[128] = "";
#endif

int main(int argc, char *argv[])
{
	int t;
	ubyte title_pal[768];

	mem_init();
	CON_Init();  // Initialise the console

	error_init(NULL, NULL);
	PHYSFSX_init(argc, argv);

	if (FindArg("-debug"))
		cvar_setint( &con_threshold, CON_DEBUG );
	else if (FindArg("-verbose"))
		cvar_setint( &con_threshold, CON_VERBOSE );

	if (! cfile_init("descent2.hog"))
		if (! cfile_init("d2demo.hog"))
			Warning("Could not find a valid hog file (descent2.hog or d2demo.hog)\nPossible locations are:\n"
#ifdef __unix__
			      "\t$HOME/.d2x\n"
			      "\t" SHAREPATH "\n"
#else
				  "\tCurrent directory\n"
#endif
				  "Or use the -hogdir option to specify an alternate location.");
	load_text();

	//print out the banner title
	con_printf(CON_NORMAL, "\nDESCENT 2 %s v%d.%d",VERSION_TYPE,Version_major,Version_minor);
	#ifdef VERSION_NAME
	con_printf(CON_NORMAL, "  %s", VERSION_NAME);
	#endif
	if (cfexist(MISSION_DIR "d2x.hog"))
		con_printf(CON_NORMAL, "  Vertigo Enhanced");

	con_printf(CON_NORMAL, "  %s %s\n", __DATE__,__TIME__);
	con_printf(CON_NORMAL, "%s\n%s\n",TXT_COPYRIGHT,TXT_TRADEMARK);
	con_printf(CON_NORMAL, "This is a MODIFIED version of Descent 2. Copyright (c) 1999 Peter Hawkins\n");
	con_printf(CON_NORMAL, "                                         Copyright (c) 2002 Bradley Bell\n");


	if (FindArg( "-?" ) || FindArg( "-help" ) || FindArg( "?" ) || FindArg( "-h" ) ) {
		print_commandline_help();
		set_exit_message("");
#ifdef __MINGW32__
		exit(0);  /* mingw hangs on this return.  dunno why */
#endif
		return(0);
	}

	con_printf(CON_NORMAL, "\n");
	con_printf(CON_NORMAL, TXT_HELP, PROGNAME);		//help message has %s for program name
	con_printf(CON_NORMAL, "\n");

	{
		char **i, **list;

		for (i = PHYSFS_getSearchPath(); *i != NULL; i++)
			con_printf(CON_VERBOSE, "PHYSFS: [%s] is in the search path.\n", *i);

		list = PHYSFS_getCdRomDirs();
		for (i = list; *i != NULL; i++)
			con_printf(CON_VERBOSE, "PHYSFS: cdrom dir [%s] is available.\n", *i);
		PHYSFS_freeList(list);

		list = PHYSFS_enumerateFiles("");
		for (i = list; *i != NULL; i++)
			con_printf(CON_DEBUG, "PHYSFS: * We've got [%s].\n", *i);
		PHYSFS_freeList(list);
	}

	//(re)added Mar 30, 2003 Micah Lieske - Allow use of 22K sound samples again.
	if(FindArg("-sound22k"))
	{
		digi_sample_rate = SAMPLE_RATE_22K;
	}

	if(FindArg("-sound11k"))
	{
		digi_sample_rate = SAMPLE_RATE_11K;
	}

	arch_init_start();

	arch_init();

	//con_printf(CON_VERBOSE, "\n%s...", "Checking for Descent 2 CD-ROM");

	if ((t = FindArg("-rearviewtime")))
	{
		float f = atof(Args[t + 1]);
		Rear_view_leave_time = f * f1_0;
	}
	con_printf(CON_VERBOSE, "Rear_view_leave_time=0x%x (%f sec)\n", Rear_view_leave_time, Rear_view_leave_time / (float)f1_0);

	//added/edited 8/18/98 by Victor Rachels to set maximum fps <= 100
	if ((t = FindArg( "-maxfps" ))) {
		t=atoi(Args[t+1]);
		if (t > 0 && t <= MAX_FPS)
			maxfps=t;
	}
	//end addition - Victor Rachels

#ifdef SUPPORTS_NICEFPS
	if (FindArg("-nicefps"))
		use_nice_fps = 1;
	if (FindArg("-niceautomap"))
		nice_automap = 1;
#endif

	if (FindArg("-renderstats"))
		gr_renderstats = 1;

	if ( FindArg( "-autodemo" ))
		Auto_demo = 1;

#ifndef RELEASE
	if ( FindArg( "-noscreens" ) )
		Skip_briefing_screens = 1;
#endif

	if ((t=FindArg("-tmap"))){
		select_tmap(Args[t+1]);
	}else
		select_tmap(NULL);

	Lighting_on = 1;

//	if (init_graphics()) return 1;

	#ifdef EDITOR
	if (gr_check_mode(SM(800, 600)) != 0)
	{
		con_printf(CON_NORMAL, "The editor will not be available, press any key to start game...\n" );
		Function_mode = FMODE_MENU;
	}
	#endif

	if (!WVIDEO_running)
		con_printf(CON_DEBUG,"WVIDEO_running = %d\n",WVIDEO_running);

	do_joystick_init();

	if ((t = gr_init()) != 0) // doesn't do much
		Error(TXT_CANT_INIT_GFX, t);

	con_printf (CON_VERBOSE, "%s", TXT_VERBOSE_1);
	ReadConfigFile();

	if (!VR_offscreen_buffer)	//if hasn't been initialied (by headset init)
		set_display_mode(Default_display_mode); //..then set default display mode

	{
		int screen_width = 640;
		int screen_height = 480;

		FindResArg("", &screen_width, &screen_height);

		// added 3/24/99 by Owen Evans for screen res changing
		Game_screen_mode = SM(screen_width, screen_height);
		// end added -OE
		set_display_mode(Game_screen_mode);

	}
	{
// added/edited on 12/14/98 by Matt Mueller - override res in d1x.ini with command line args
		int i, argnum = INT_MAX, w, h;
// added on 9/30/98 by Matt Mueller for selectable automap modes - edited 11/21/99 whee, more fun with defines. - edited 03/31/02 to use new FindResArg.
#define SMODE(V,VV,VG) if ((i=FindResArg(#V, &w, &h)) && (i < argnum)) { argnum = i; VV = SM(w, h); VG = 0; }
#define SMODE_GR(V,VG) if ((i=FindArg("-" #V "_gameres"))){if (i<argnum) VG=1;}
#define SMODE_PRINT(V,VV,VG) if (VG) con_printf(CON_VERBOSE, #V " using game resolution ...\n"); else con_printf(CON_VERBOSE, #V " using %ix%i ...\n",SM_W(VV),SM_H(VV) );
// aren't #defines great? :)
// end addition/edit -MM
#define S_MODE(V,VV,VG) argnum = INT_MAX; SMODE(V, VV, VG); SMODE_GR(V, VG); SMODE_PRINT(V, VV, VG);

		S_MODE(automap,automap_mode,automap_use_game_res);
//		S_MODE(menu,menu_screen_mode,menu_use_game_res);
	 }
//end addition -MM

	controls_init();

	con_printf(CON_VERBOSE, "\n%s\n\n", TXT_INITIALIZING_GRAPHICS);
	if (FindArg("-nofade"))
		grd_fades_disabled=1;

	//determine whether we're using high-res menus & movies
	if (FindArg("-nohires") || FindArg("-nohighres") || (gr_check_mode(MENU_HIRES_MODE) != 0) || disable_high_res)
		cvar_setint( &MovieHires, MenuHires = MenuHiresAvailable = 0 );
	else
		//NOTE LINK TO ABOVE!
		MenuHires = MenuHiresAvailable = 1;

	if (FindArg( "-lowresmovies" ))
		cvar_setint( &MovieHires, 0 );

	con_printf(CON_VERBOSE, "Going into graphics mode...\n");
	gr_set_mode(MovieHires.intval?SM(640,480):SM(320,200));

	// Load the palette stuff. Returns non-zero if error.
	con_printf(CON_DEBUG, "Initializing palette system...\n" );
	gr_use_palette_table(DEFAULT_PALETTE );

	con_printf(CON_DEBUG, "Initializing font system...\n" );
	gamefont_init();	// must load after palette data loaded.

	con_printf( CON_DEBUG, "Initializing movie libraries...\n" );
	init_movies();		//init movie libraries

	if ( FindArg( "-notitles" ) )
		songs_play_song( SONG_TITLE, 1);
	else
		show_titles();

	con_printf( CON_DEBUG, "\nShowing loading screen..." );
	show_loading_screen(title_pal);	// title_pal is needed (see below)

	CON_InitGFX(SWIDTH, SHEIGHT / 2);

	con_printf( CON_DEBUG , "\nDoing bm_init..." );
	#ifdef EDITOR
	if (!bm_init_use_tbl())
	#endif
		bm_init();

	#ifdef EDITOR
	if (FindArg("-hoarddata") != 0)
	{
		save_hoard_data();
		exit(1);
	}
	#endif

	//the bitmap loading code changes gr_palette, so restore it
	memcpy(gr_palette,title_pal,sizeof(gr_palette));

	if ( FindArg( "-norun" ) )
		return(0);

	con_printf( CON_DEBUG, "\nInitializing 3d system..." );
	g3_init();

	con_printf( CON_DEBUG, "\nInitializing texture caching system..." );
	texmerge_init( 10 );		// 10 cache bitmaps

	con_printf( CON_DEBUG, "\nRunning game...\n" );
	set_screen_mode(SCREEN_MENU);

	init_game();

	remap_fonts_and_menus(1);

	if ((t = FindArg("-autoexec")))
		cmd_appendf("exec %s", Args[t+1]);
	else if (cfexist("autoexec.cfg"))
		cmd_append("exec autoexec.cfg");

	cmd_queue_process();

	//	If built with editor, option to auto-load a level and quit game
	//	to write certain data.
	#ifdef	EDITOR
	{	int t;
	if ( (t = FindArg( "-autoload" )) ) {
		Auto_exit = 1;
		strcpy(Auto_file, Args[t+1]);
	}
		
	}

	if (Auto_exit) {
		strcpy(Players[0].callsign, "dummy");
	} else
	#endif
		if (!strlen(Players[0].callsign))
			do_register_player(title_pal);

	Game_mode = GM_GAME_OVER;

	if (Auto_demo)	{
		newdemo_start_playback("descent.dem");
		if (Newdemo_state == ND_STATE_PLAYBACK )
			Function_mode = FMODE_GAME;
	}

	//do this here because the demo code can do a longjmp when trying to
	//autostart a demo from the main menu, never having gone into the game
	setjmp(LeaveGame);

	remap_fonts_and_menus(1);

	while (Function_mode != FMODE_EXIT)
	{
		cmd_queue_process();

		switch( Function_mode )	{
		case FMODE_MENU:
			set_screen_mode(SCREEN_MENU);
			if ( Auto_demo )	{
				newdemo_start_playback(NULL);		// Randomly pick a file
				if (Newdemo_state != ND_STATE_PLAYBACK)	
					Error("No demo files were found for autodemo mode!");
			} else {
				#ifdef EDITOR
				if (Auto_exit) {
					strcpy((char *)&Level_names[0], Auto_file);
					LoadLevel(1, 1);
					Function_mode = FMODE_EXIT;
					break;
				}
				#endif

				check_joystick_calibration();
				gr_palette_clear();		//I'm not sure why we need this, but we do
				DoMenu();									 	
				#ifdef EDITOR
				if ( Function_mode == FMODE_EDITOR )	{
					create_new_mine();
					SetPlayerFromCurseg();
					load_palette(NULL,1,0);
				}
				#endif
			}
			break;
		case FMODE_GAME:
			#ifdef EDITOR
				keyd_editor_mode = 0;
			#endif

			game();

			if ( Function_mode == FMODE_MENU )
				songs_play_song( SONG_TITLE, 1 );
			break;
		#ifdef EDITOR
		case FMODE_EDITOR:
			keyd_editor_mode = 1;
			editor();
#ifdef __WATCOMC__
			_harderr( (void*)descent_critical_error_handler );		// Reinstall game error handler
#endif
			if ( Function_mode == FMODE_GAME ) {
				Game_mode = GM_EDITOR;
				editor_reset_stuff_on_level();
				N_players = 1;
			}
			break;
		#endif
		default:
			Error("Invalid function mode %d",Function_mode);
		}
	}

	WriteConfigFile();

	if (!FindArg( "-notitles" ))
		show_order_form();

	#ifndef NDEBUG
	if ( FindArg( "-showmeminfo" ) )
		show_mem_info = 1;		// Make memory statistics show
	#endif

	return(0);		//presumably successful exit
}


void check_joystick_calibration()	{
	int x1, y1, x2, y2, c;
	fix t1;

	if ( !Config_control_joystick.intval )
		return;

	joy_get_pos( &x1, &y1 );

	t1 = timer_get_fixed_seconds();
	while( timer_get_fixed_seconds() < t1 + F1_0/100 )
		;

	joy_get_pos( &x2, &y2 );

	// If joystick hasn't moved...
	if ( (abs(x2-x1)<30) &&  (abs(y2-y1)<30) )	{
		if ( (abs(x1)>30) || (abs(x2)>30) ||  (abs(y1)>30) || (abs(y2)>30) )	{
			c = nm_messagebox( NULL, 2, TXT_CALIBRATE, TXT_SKIP, TXT_JOYSTICK_NOT_CEN );
			if ( c==0 )	{
				joydefs_calibrate();
			}
		}
	}

}

void quit_request()
{
#ifdef NETWORK
//	void network_abort_game();
//	if(Network_status)
//		network_abort_game();
#endif
	exit(0);
}
