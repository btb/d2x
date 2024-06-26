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
 * Menu prototypes and variables
 *
 */

#ifndef _MENU_H
#define _MENU_H

// returns number of item chosen
extern int DoMenu(void);
extern void do_options_menu(void);
extern void d2x_options_menu(void);

// can we do highres menus?
extern int MenuHiresAvailable;
// are we currently in highres menus?
#define MenuHires (menu_use_game_res.intval?SM_HIRES2:MenuHiresAvailable)

#ifdef RELEASE  // read only from hog file
#define MENU_PCX_MAC_SHARE ("\x01menub.pcx")
#define MENU_PCX_SHAREWARE ("\x01menud.pcx")
#define MENU_PCX_OEM (MenuHires?"\x01menuob.pcx":"\x01menuo.pcx")
#define MENU_PCX_FULL (MenuHires?"\x01menub.pcx":"\x01menu.pcx")
#else
#define MENU_PCX_MAC_SHARE ("menub.pcx")
#define MENU_PCX_SHAREWARE ("menud.pcx")
#define MENU_PCX_OEM (MenuHires?"menuob.pcx":"menuo.pcx")
#define MENU_PCX_FULL (MenuHires?"menub.pcx":"menu.pcx")
#endif

// name of background bitmap
#define Menu_pcx_name (cfexist(MENU_PCX_FULL)?MENU_PCX_FULL:(cfexist(MENU_PCX_OEM)?MENU_PCX_OEM:cfexist(MENU_PCX_SHAREWARE)?MENU_PCX_SHAREWARE:MENU_PCX_MAC_SHARE))

extern void set_detail_level_parameters(int detail_level);

extern char *menu_difficulty_text[];
extern cvar_t Player_default_difficulty;
extern int Max_debris_objects;
extern cvar_t Auto_leveling_on;
extern cvar_t Guided_in_big_window;
extern cvar_t EscortHotKeys;
extern int Escort_view_enabled;
extern int Cockpit_rear_view;

#endif /* _MENU_H */
