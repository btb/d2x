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
 * Routines to configure keyboard, joystick, etc..
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifdef WINDOWS
#include "desw.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include "error.h"
#include "pstypes.h"
#include "gr.h"
#include "mono.h"
#include "key.h"
#include "palette.h"
#include "inferno.h"
#include "gamefont.h"
#include "iff.h"
#include "u_mem.h"
#include "joy.h"
#include "mouse.h"
#include "kconfig.h"
#include "gauges.h"
#include "joydefs.h"
#include "songs.h"
#include "render.h"
#include "digi.h"
#include "newmenu.h"
#include "endlevel.h"
#include "multi.h"
#include "timer.h"
#include "text.h"
#include "player.h"
#include "menu.h"
#include "automap.h"
#include "args.h"
#include "lighting.h"
#include "ai.h"
#include "cntrlcen.h"
#if defined (TACTILE)
 #include "tactile.h"
#endif

#include "collide.h"

#ifdef USE_LINUX_JOY
#include "joystick.h"
#endif
#include "console.h"

ubyte ExtGameStatus=1;

vms_vector ExtForceVec;
vms_matrix ExtApplyForceMatrix;

int ExtJoltInfo[3]={0,0,0};
int ExtXVibrateInfo[2]={0,0};
int ExtYVibrateInfo[2]={0,0};
ubyte ExtXVibrateClear=0;
ubyte ExtYVibrateClear=0;

#define TABLE_CREATION 1

// Array used to 'blink' the cursor while waiting for a keypress.
sbyte fades[64] = { 1,1,1,2,2,3,4,4,5,6,8,9,10,12,13,15,16,17,19,20,22,23,24,26,27,28,28,29,30,30,31,31,31,31,31,30,30,29,28,28,27,26,24,23,22,20,19,17,16,15,13,12,10,9,8,6,5,4,4,3,2,2,1,1 };

//char * invert_text[2] = { "N", "Y" };
//char * joyaxis_text[4] = { "X1", "Y1", "X2", "Y2" };
//char * mouseaxis_text[2] = { "L/R", "F/B" };

int invert_text[2] = { TNUM_N, TNUM_Y };

#ifndef USE_LINUX_JOY
#if defined(SDL_INPUT)
char *joyaxis_text[JOY_MAX_AXES];
#else
	int joyaxis_text[7] = { TNUM_X1, TNUM_Y1, TNUM_Z1, TNUM_UN, TNUM_P1,TNUM_R1,TNUM_YA1 };
//	int joyaxis_text[4] = { TNUM_X1, TNUM_Y1, TNUM_X2, TNUM_Y2 };
#endif
#endif

int mouseaxis_text[3] = { TNUM_L_R, TNUM_F_B, TNUM_Z1 };

#if !defined OGL && !defined SDL_INPUT
char * key_text[256] = {         \
"","ESC","1","2","3","4","5","6","7","8","9","0","-", 			\
"=","BSPC","TAB","Q","W","E","R","T","Y","U","I","O",				\
"P","[","]","\x83","LCTRL","A","S","D","F",        \
"G","H","J","K","L",";","'","`",        \
"LSHFT","\\","Z","X","C","V","B","N","M",",",      \
".","/","RSHFT","PAD*","LALT","SPC",      \
"CPSLK","F1","F2","F3","F4","F5","F6","F7","F8","F9",        \
"F10","NMLCK","SCLK","PAD7","PAD8","PAD9","PAD-",   \
"PAD4","PAD5","PAD6","PAD+","PAD1","PAD2","PAD3","PAD0", \
"PAD.","","","","F11","F12","","","","","","","","","",         \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","",           \
"PAD\x83","RCTRL","","","","","","","","","","","","","", \
"","","","","","","","","","","PAD/","","","RALT","",      \
"","","","","","","","","","","","","","HOME","\x82","PGUP",     \
"","\x81","","\x7f","","END","\x80","PGDN","INS",       \
"DEL","","","","","","","","","","","","","","","","","",     \
"","","","","","","","","","","","","","","","","","","","",     \
"","","","","","","" };
#endif /* OGL */

ubyte system_keys[] = { KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_MINUS, KEY_EQUAL, KEY_PRINT_SCREEN };

//extern void GameLoop(int, int );

// macros for drawing lo/hi res kconfig screens (see scores.c as well)

#define LHX(x)		((x)*(MenuHires?2:1))
#define LHY(y)		((y)*(MenuHires?2.4:1))


#define BT_KEY 				0
//#define BT_MOUSE_BUTTON 	1
#define BT_MOUSE_AXIS		2
//#define BT_JOY_BUTTON 		3
#define BT_JOY_AXIS			4
#define BT_INVERT				5

char *btype_text[] = { "BT_KEY", "BT_MOUSE_BUTTON", "BT_MOUSE_AXIS", "BT_JOY_BUTTON", "BT_JOY_AXIS", "BT_INVERT" };

#define INFO_Y 28

typedef struct kc_item {
	short id;				// The id of this item
	short x, y;				
	short w1;
	short w2;
	short u,d,l,r;
        //short text_num1;
        char *text;
	ubyte type;
	ubyte value;		// what key,button,etc
} kc_item;

int Num_items=28;
kc_item *All_items;


//----------- WARNING!!!!!!! -------------------------------------------
// THESE NEXT FOUR BLOCKS OF DATA ARE GENERATED BY PRESSING DEL+F12 WHEN
// IN THE KEYBOARD CONFIG SCREEN.  BASICALLY, THAT PROCEDURE MODIFIES THE
// U,D,L,R FIELDS OF THE ARRAYS AND DUMPS THE NEW ARRAYS INTO KCONFIG.COD
//-------------------------------------------------------------------------

kc_item kc_keyboard[NUM_KEY_CONTROLS] = {
	{  0, 15, 49, 71, 26, 55,  2, 55,  1,"Pitch forward", BT_KEY, 255 },
	{  1, 15, 49,100, 26, 50,  3,  0, 24,"Pitch forward", BT_KEY, 255 },
	{  2, 15, 57, 71, 26,  0,  4, 25,  3,"Pitch backward", BT_KEY, 255 },
	{  3, 15, 57,100, 26,  1,  5,  2, 26,"Pitch backward", BT_KEY, 255 },
	{  4, 15, 65, 71, 26,  2,  6, 27,  5,"Turn left", BT_KEY, 255 },
	{  5, 15, 65,100, 26,  3,  7,  4, 28,"Turn left", BT_KEY, 255 },
	{  6, 15, 73, 71, 26,  4,  8, 29,  7,"Turn right", BT_KEY, 255 },
	{  7, 15, 73,100, 26,  5,  9,  6, 34,"Turn right", BT_KEY, 255 },
	{  8, 15, 85, 71, 26,  6, 10, 35,  9,"Slide on", BT_KEY, 255 },
	{  9, 15, 85,100, 26,  7, 11,  8, 36,"Slide on", BT_KEY, 255 },
	{ 10, 15, 93, 71, 26,  8, 12, 37, 11,"Slide left", BT_KEY, 255 },
	{ 11, 15, 93,100, 26,  9, 13, 10, 44,"Slide left", BT_KEY, 255 },
	{ 12, 15,101, 71, 26, 10, 14, 45, 13,"Slide right", BT_KEY, 255 },
	{ 13, 15,101,100, 26, 11, 15, 12, 30,"Slide right", BT_KEY, 255 },
	{ 14, 15,109, 71, 26, 12, 16, 31, 15,"Slide up", BT_KEY, 255 },
	{ 15, 15,109,100, 26, 13, 17, 14, 32,"Slide up", BT_KEY, 255 },
	{ 16, 15,117, 71, 26, 14, 18, 33, 17,"Slide down", BT_KEY, 255 },
	{ 17, 15,117,100, 26, 15, 19, 16, 46,"Slide down", BT_KEY, 255 },
	{ 18, 15,129, 71, 26, 16, 20, 47, 19,"Bank on", BT_KEY, 255 },
	{ 19, 15,129,100, 26, 17, 21, 18, 38,"Bank on", BT_KEY, 255 },
	{ 20, 15,137, 71, 26, 18, 22, 39, 21,"Bank left", BT_KEY, 255 },
	{ 21, 15,137,100, 26, 19, 23, 20, 40,"Bank left", BT_KEY, 255 },
	{ 22, 15,145, 71, 26, 20, 48, 41, 23,"Bank right", BT_KEY, 255 },
	{ 23, 15,145,100, 26, 21, 49, 22, 42,"Bank right", BT_KEY, 255 },
	{ 24,158, 49, 83, 26, 51, 26,  1, 25,"Fire primary", BT_KEY, 255 },
	{ 25,158, 49,112, 26, 54, 27, 24,  2,"Fire primary", BT_KEY, 255 },
	{ 26,158, 57, 83, 26, 24, 28,  3, 27,"Fire secondary", BT_KEY, 255 },
	{ 27,158, 57,112, 26, 25, 29, 26,  4,"Fire secondary", BT_KEY, 255 },
	{ 28,158, 65, 83, 26, 26, 34,  5, 29,"Fire flare", BT_KEY, 255 },
	{ 29,158, 65,112, 26, 27, 35, 28,  6,"Fire flare", BT_KEY, 255 },
	{ 30,158,105, 83, 26, 44, 32, 13, 31,"Accelerate", BT_KEY, 255 },
	{ 31,158,105,112, 26, 45, 33, 30, 14,"Accelerate", BT_KEY, 255 },
	{ 32,158,113, 83, 26, 30, 46, 15, 33,"reverse", BT_KEY, 255 },
	{ 33,158,113,112, 26, 31, 47, 32, 16,"reverse", BT_KEY, 255 },
	{ 34,158, 73, 83, 26, 28, 36,  7, 35,"Drop Bomb", BT_KEY, 255 },
	{ 35,158, 73,112, 26, 29, 37, 34,  8,"Drop Bomb", BT_KEY, 255 },
	{ 36,158, 85, 83, 26, 34, 44,  9, 37,"REAR VIEW", BT_KEY, 255 },
	{ 37,158, 85,112, 26, 35, 45, 36, 10,"REAR VIEW", BT_KEY, 255 },
	{ 38,158,133, 83, 26, 46, 40, 19, 39,"Cruise Faster", BT_KEY, 255 },
	{ 39,158,133,112, 26, 47, 41, 38, 20,"Cruise Faster", BT_KEY, 255 },
	{ 40,158,141, 83, 26, 38, 42, 21, 41,"Cruise Slower", BT_KEY, 255 },
	{ 41,158,141,112, 26, 39, 43, 40, 22,"Cruise Slower", BT_KEY, 255 },
	{ 42,158,149, 83, 26, 40, 52, 23, 43,"Cruise Off", BT_KEY, 255 },
	{ 43,158,149,112, 26, 41, 53, 42, 48,"Cruise Off", BT_KEY, 255 },
	{ 44,158, 93, 83, 26, 36, 30, 11, 45,"Automap", BT_KEY, 255 },
	{ 45,158, 93,112, 26, 37, 31, 44, 12,"Automap", BT_KEY, 255 },
	{ 46,158,121, 83, 26, 32, 38, 17, 47,"Afterburner", BT_KEY, 255 },
	{ 47,158,121,112, 26, 33, 39, 46, 18,"Afterburner", BT_KEY, 255 },
	{ 48, 15,161, 71, 26, 22, 50, 43, 49,"Cycle Primary", BT_KEY, 255 },
	{ 49, 15,161,100, 26, 23, 51, 48, 52,"Cycle Primary", BT_KEY, 255 },
	{ 50, 15,169, 71, 26, 48,  1, 53, 51,"Cycle Second", BT_KEY, 255 },
	{ 51, 15,169,100, 26, 49, 24, 50, 54,"Cycle Second", BT_KEY, 255 },
	{ 52,158,163, 83, 26, 42, 54, 49, 53,"Headlight", BT_KEY, 255 },
	{ 53,158,163,112, 26, 43, 55, 52, 50,"Headlight", BT_KEY, 255 },
	{ 54,158,171, 83, 26, 52, 56, 51, 55,"Energy->Shield", BT_KEY, 255 },
	{ 55,158,171,112, 26, 53,  0, 54,  0,"Energy->Shield", BT_KEY, 255 },
	{ 56,158,179, 83, 26, 54,  0,  0,  0,"Toggle Bomb", BT_KEY, 255 },
};

char *kc_key_bind_text[NUM_KEY_CONTROLS] = {
	"+lookdown",    "+lookdown",
	"+lookup",      "+lookup",
	"+left",        "+left",
	"+right",       "+right",
	"+strafe",      "+strafe",
	"+moveleft",    "+moveleft",
	"+moveright",   "+moveright",
	"+moveup",      "+moveup",
	"+movedown",    "+movedown",
	"+bank",        "+bank",
	"+bankleft",    "+bankleft",
	"+bankright",   "+bankright",
	"+attack",      "+attack",
	"+attack2",     "+attack2",
	"flare",        "flare",
	"+forward",     "+forward",
	"+back",        "+back",
	"bomb",         "bomb",
	"+rearview",    "+rearview",
	"+cruiseup",    "+cruiseup",
	"+cruisedown",  "+cruisedown",
	"+cruiseoff",   "+cruiseoff",
	"+automap",     "+automap",
	"+afterburner", "+afterburner",
	"cycle",        "cycle",
	"cycle2",       "cycle2",
	"headlight",    "headlight",
	"+nrgshield",   "+nrgshield",
	"togglebomb",
};

ubyte default_kc_keyboard_settings[MAX_CONTROLS] = {0x48,0xc8,0x50,0xd0,0x4b,0xcb,0x4d,0xcd,0x38,0xff,0x4f,0xff,0x51,0xff,0x4a,0xff,0x4e,0xff,0x2a,0xff,0x10,0x47,0x12,0x49,0x1d,0x80,0x39,0x81,0x21,0x24,0x1e,0xff,0x2c,0xff,0x30,0xff,0x13,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xf,0xff,0x1f,0xff,0x33,0xff,0x34,0xff,0x23,0xff,0x14,0xff,0xff,0x80,0x0,0x0};

kc_item kc_other[NUM_OTHER_CONTROLS] = {
	{  0, 22,138, 51, 40, 23,  2, 23,  1,"Pitch U/D", BT_JOY_AXIS, 255 },
	{  1, 22,138, 99,  8, 10,  3,  0, 12,"Pitch U/D", BT_INVERT, 255 },
	{  2, 22,146, 51, 40,  0,  4, 13,  3,"Turn L/R", BT_JOY_AXIS, 255 },
	{  3, 22,146, 99,  8,  1,  5,  2, 14,"Turn L/R", BT_INVERT, 255 },
	{  4, 22,154, 51, 40,  2,  6, 15,  5,"Slide L/R", BT_JOY_AXIS, 255 },
	{  5, 22,154, 99,  8,  3,  7,  4, 16,"Slide L/R", BT_INVERT, 255 },
	{  6, 22,162, 51, 40,  4,  8, 17,  7,"Slide U/D", BT_JOY_AXIS, 255 },
	{  7, 22,162, 99,  8,  5,  9,  6, 18,"Slide U/D", BT_INVERT, 255 },
	{  8, 22,170, 51, 40,  6, 10, 19,  9,"Bank L/R", BT_JOY_AXIS, 255 },
	{  9, 22,170, 99,  8,  7, 11,  8, 20,"Bank L/R", BT_INVERT, 255 },
	{ 10, 22,182, 51, 40,  8,  1, 21, 11,"throttle", BT_JOY_AXIS, 255 },
	{ 11, 22,182, 99,  8,  9, 12, 10, 22,"throttle", BT_INVERT, 255 },
	{ 12,182,138, 51, 40, 11, 14,  1, 13,"Pitch U/D", BT_MOUSE_AXIS, 255 },
	{ 13,182,138, 99,  8, 22, 15, 12,  2,"Pitch U/D", BT_INVERT, 255 },
	{ 14,182,146, 51, 40, 12, 16,  3, 15,"Turn L/R", BT_MOUSE_AXIS, 255 },
	{ 15,182,146, 99,  8, 13, 17, 14,  4,"Turn L/R", BT_INVERT, 255 },
	{ 16,182,154, 51, 40, 14, 18,  5, 17,"Slide L/R", BT_MOUSE_AXIS, 255 },
	{ 17,182,154, 99,  8, 15, 19, 16,  6,"Slide L/R", BT_INVERT, 255 },
	{ 18,182,162, 51, 40, 16, 20,  7, 19,"Slide U/D", BT_MOUSE_AXIS, 255 },
	{ 19,182,162, 99,  8, 17, 21, 18,  8,"Slide U/D", BT_INVERT, 255 },
	{ 20,182,170, 51, 40, 18, 22,  9, 21,"Bank L/R", BT_MOUSE_AXIS, 255 },
	{ 21,182,170, 99,  8, 19, 23, 20, 10,"Bank L/R", BT_INVERT, 255 },
	{ 22,182,182, 51, 40, 20, 13, 11, 23,"throttle", BT_MOUSE_AXIS, 255 },
	{ 23,182,182, 99,  8, 21,  0, 22,  0,"throttle", BT_INVERT, 255 },
};

kc_axis_map kc_other_axismap[NUM_OTHER_CONTROLS] = {
	AXIS_PITCH,     AXIS_NONE,
	AXIS_TURN,      AXIS_NONE,
	AXIS_LEFTRIGHT, AXIS_NONE,
	AXIS_UPDOWN,    AXIS_NONE,
	AXIS_BANK,      AXIS_NONE,
	AXIS_THROTTLE,  AXIS_NONE,
	AXIS_PITCH,     AXIS_NONE,
	AXIS_TURN,      AXIS_NONE,
	AXIS_LEFTRIGHT, AXIS_NONE,
	AXIS_UPDOWN,    AXIS_NONE,
	AXIS_BANK,      AXIS_NONE,
	AXIS_THROTTLE,  AXIS_NONE,
};

ubyte default_kc_other_settings[MAX_CONTROLS] = {0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0x1,0x0,0x0,0x0,0xff,0x0,0xff,0x0,0xff,0x0,0x2,0x0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0xff,0x0,0x3f};

kc_item kc_d2x[NUM_D2X_CONTROLS] = {
//        id,x,y,w1,w2,u,d,l,r,text_num1,type,value
	{  0, 15, 49, 71, 26, 19,  2, 27,  1,"WEAPON 1", BT_KEY, 255 },
	{  1, 15, 49,100, 26, 18,  3,  0,  2,"WEAPON 1", BT_KEY, 255 },
	{  2, 15, 57, 71, 26,  0,  4,  1,  3,"WEAPON 2", BT_KEY, 255 },
	{  3, 15, 57,100, 26,  1,  5,  2,  4,"WEAPON 2", BT_KEY, 255 },
	{  4, 15, 65, 71, 26,  2,  6,  3,  5,"WEAPON 3", BT_KEY, 255 },
	{  5, 15, 65,100, 26,  3,  7,  4,  6,"WEAPON 3", BT_KEY, 255 },
	{  6, 15, 73, 71, 26,  4,  8,  5,  7,"WEAPON 4", BT_KEY, 255 },
	{  7, 15, 73,100, 26,  5,  9,  6,  8,"WEAPON 4", BT_KEY, 255 },
	{  8, 15, 81, 71, 26,  6, 10,  7,  9,"WEAPON 5", BT_KEY, 255 },
	{  9, 15, 81,100, 26,  7, 11,  8, 10,"WEAPON 5", BT_KEY, 255 },
	{ 10, 15, 89, 71, 26,  8, 12,  9, 11,"WEAPON 6", BT_KEY, 255 },
	{ 11, 15, 89,100, 26,  9, 13, 10, 12,"WEAPON 6", BT_KEY, 255 },
	{ 12, 15, 97, 71, 26, 10, 14, 11, 13,"WEAPON 7", BT_KEY, 255 },
	{ 13, 15, 97,100, 26, 11, 15, 12, 14,"WEAPON 7", BT_KEY, 255 },
	{ 14, 15,105, 71, 26, 12, 16, 13, 15,"WEAPON 8", BT_KEY, 255 },
	{ 15, 15,105,100, 26, 13, 17, 14, 16,"WEAPON 8", BT_KEY, 255 },
	{ 16, 15,113, 71, 26, 14, 18, 15, 17,"WEAPON 9", BT_KEY, 255 },
	{ 17, 15,113,100, 26, 15, 19, 16, 18,"WEAPON 9", BT_KEY, 255 },
	{ 18, 15,121, 71, 26, 16,  1, 17, 19,"WEAPON 0", BT_KEY, 255 },
	{ 19, 15,121,100, 26, 17,  0, 18,  0,"WEAPON 0", BT_KEY, 255 },
	//{ 20, 15,131, 71, 26, 18, 22, 19, 21, "CYC PRIMARY", BT_KEY, 255},
	//{ 21, 15,131,100, 26, 19, 23, 20, 22, "CYC PRIMARY", BT_KEY, 255},
	//{ 22, 15,139, 71, 26, 20, 24, 21, 23, "CYC SECONDARY", BT_KEY, 255},
	//{ 23, 15,139,100, 26, 21, 25, 22, 24, "CYC SECONDARY", BT_KEY, 255},
	//{ 24,  8,147, 78, 26, 22, 26, 23, 25, "TOGGLE_PRIM AUTO", BT_KEY, 255},
	//{ 25,  8,147,107, 26, 23, 27, 24, 26, "TOGGLE_PRIM_AUTO", BT_KEY, 255},
	//{ 26,  8,155, 78, 26, 24,  1, 25, 27, "TOGGLE SEC AUTO", BT_KEY, 255},
	//{ 27,  8,155,107, 26, 25,  0, 26,  0, "TOGGLE SEC AUTO", BT_KEY, 255},
};

ubyte default_kc_d2x_settings[MAX_CONTROLS] = {0x2,0xff,0x3,0xff,0x4,0xff,0x5,0xff,0x6,0xff,0x7,0xff,0x8,0xff,0x9,0xff,0xa,0xff,0xb,0xff,0xff,0x0,0xff,0xff,0x12,0xf,0x80,0x80,0x80,0x0,0x4,0x4,0x0,0x0,0x0,0x10,0x0,0x0,0x0,0x20,0x57,0x52,0x0,0x12,0xa2,0xa2,0xa4,0xa4,0xa4,0xa4,0xa4,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0xff};


void kc_drawitem( kc_item *item, int is_current );
void kc_change_key( kc_item * item );
void kc_next_joyaxis(kc_item *item);  //added by WraithX on 11/22/00
void kc_change_joyaxis( kc_item * item );
void kc_change_mouseaxis( kc_item * item );
void kc_change_invert( kc_item * item );

int kconfig_is_axes_used(int axis)
{
	int i;
	for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
		if (( kc_other[i].type == BT_JOY_AXIS ) && (kc_other[i].value == axis ))
			return 1;
	}
	return 0;
}

#ifdef TABLE_CREATION
int find_item_at( kc_item * items, int nitems, int x, int y )
{
	int i;
	
	for (i=0; i<nitems; i++ )	{
		if ( ((items[i].x+items[i].w1)==x) && (items[i].y==y))
			return i;
	}
	return -1;
}

int find_next_item_up( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		y--;
		if ( y < 0 ) {
			y = grd_curcanv->cv_bitmap.bm_h-1;
			x--;
			if ( x < 0 ) {
				x = grd_curcanv->cv_bitmap.bm_w-1;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_down( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		y++;
		if ( y > grd_curcanv->cv_bitmap.bm_h-1 ) {
			y = 0;
			x++;
			if ( x > grd_curcanv->cv_bitmap.bm_w-1 ) {
				x = 0;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_right( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		x++;
		if ( x > grd_curcanv->cv_bitmap.bm_w-1 ) {
			x = 0;
			y++;
			if ( y > grd_curcanv->cv_bitmap.bm_h-1 ) {
				y = 0;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}

int find_next_item_left( kc_item * items, int nitems, int citem )
{
	int x, y, i;

	y = items[citem].y;
	x = items[citem].x+items[citem].w1;
	
	do {	
		x--;
		if ( x < 0 ) {
			x = grd_curcanv->cv_bitmap.bm_w-1;
			y--;
			if ( y < 0 ) {
				y = grd_curcanv->cv_bitmap.bm_h-1;
			}
		}
		i = find_item_at( items, nitems, x, y );
	} while ( i < 0 );
	
	return i;
}
#endif

#ifdef NEWMENU_MOUSE
int get_item_height(kc_item *item)
{
	int w, h, aw;
	char btext[10];

	if (item->value==255) {
		strcpy(btext, "");
	} else {
		switch( item->type )	{
			case BT_KEY:
				strncpy( btext, key_text[item->value], 10 ); break;
			case BT_MOUSE_AXIS:
				strncpy( btext, Text_string[mouseaxis_text[item->value]], 10 ); break;
			case BT_JOY_AXIS:
#ifdef USE_LINUX_JOY
				sprintf( btext, "J%d A%d", j_axis[item->value].joydev, j_Get_joydev_axis_number (item->value) );
#elif defined(SDL_INPUT)
				if (joyaxis_text[item->value])
					strncpy(btext, joyaxis_text[item->value], 10);
				else
					sprintf(btext, "AXIS%2d", item->value + 1);
#else
				strncpy(btext, Text_string[joyaxis_text[item->value]], 10);
#endif
				break;
			case BT_INVERT:
				strncpy( btext, Text_string[invert_text[item->value]], 10 ); break;
		}
	}
	gr_get_string_size(btext, &w, &h, &aw  );

	return h;
}
#endif

void kconfig_sub(kc_item * items,int nitems, char * title)
{
	grs_canvas * save_canvas;
	grs_font * save_font;
	int old_keyd_repeat;
#ifdef NEWMENU_MOUSE
	int mouse_state, omouse_state, mx, my, x1, x2, y1, y2;
	int close_x, close_y, close_size;
#endif

	int i,k,ocitem,citem;
	int time_stopped = 0;

	All_items = items;
	Num_items = nitems;

	if (!((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence)) )
	{
		time_stopped = 1;
		stop_time();
	}

	save_canvas = grd_curcanv;


	gr_set_current_canvas(NULL);
	save_font = grd_curcanv->cv_font;

	game_flush_inputs();
	old_keyd_repeat = keyd_repeat;
	keyd_repeat = 1;

	//gr_clear_canvas( BM_XRGB(0,0,0) );

	nm_draw_background(0, 0, grd_curcanv->cv_bitmap.bm_w - 1, grd_curcanv->cv_bitmap.bm_h - 1);
   gr_palette_load (gr_palette);

	grd_curcanv->cv_font = MEDIUM3_FONT;

	{
		char * p;
		p = strchr( title, '\n' );
		if ( p ) *p = 32;
		gr_string( 0x8000, LHY(8), title );
		if ( p ) *p = '\n';
	}


//	if ( items == kc_keyboard )	{
//		gr_string( 0x8000, 8, "Keyboard" );
//	} else if ( items == kc_other )	{
//		gr_string( 0x8000, 8, "Others" );
//	}

#ifdef NEWMENU_MOUSE
	close_x = close_y = MenuHires?15:7;
	close_size = MenuHires?10:5;
	/*
	gr_setcolor( BM_XRGB(0, 0, 0) );
	gr_rect(close_x, close_y, close_x + close_size, close_y + close_size);
	gr_setcolor( BM_XRGB(21, 21, 21) );
	gr_rect(close_x + LHX(1), close_y + LHX(1), close_x + close_size - LHX(1), close_y + close_size - LHX(1));
	*/
#endif

	grd_curcanv->cv_font = GAME_FONT;
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

	gr_string( 0x8000, LHY(20), TXT_KCONFIG_STRING_1 );
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	if ( items == kc_keyboard )	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		
		gr_scanline( LHX(98), LHX(106), LHY(42) );
		gr_scanline( LHX(120), LHX(128), LHY(42) );
		gr_pixel( LHX(98), LHY(43) );						
		gr_pixel( LHX(98), LHY(44) );						
		gr_pixel( LHX(128), LHY(43) );						
		gr_pixel( LHX(128), LHY(44) );						
		
		gr_string( LHX(109), LHY(40), "OR" );

		gr_scanline( LHX(253), LHX(261), LHY(42) );
		gr_scanline( LHX(274), LHX(283), LHY(42) );
		gr_pixel( LHX(253), LHY(43) );						
		gr_pixel( LHX(253), LHY(44) );						
		gr_pixel( LHX(283), LHY(43) );						
		gr_pixel( LHX(283), LHY(44) );						

		gr_string( LHX(264), LHY(40), "OR" );

	} if ( items == kc_other )	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		gr_scanline( LHX(18), LHX(60), LHY(119+5) );
		gr_scanline( LHX(102), LHX(144), LHY(119+5) );
		gr_string( LHX(63), LHY(117+5), TXT_CONTROL_JOYSTICK );
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
		gr_string( LHX(84), LHY(129), TXT_AXIS );
		gr_string( LHX(110), LHY(129), TXT_INVERT );

		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );
		gr_scanline( LHX(178), LHX(226), LHY(119+5) );
		gr_scanline( LHX(256), LHX(304), LHY(119+5) );
		gr_string( LHX(229), LHY(117+5), TXT_CONTROL_MOUSE );
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
		gr_string( LHX(244), LHY(129), TXT_AXIS );
		gr_string( LHX(270), LHY(129), TXT_INVERT );
	}
	else if ( items == kc_d2x )
	{
		gr_set_fontcolor( BM_XRGB(31,27,6), -1 );
		gr_setcolor( BM_XRGB(31,27,6) );

		gr_scanline( LHX(98), LHX(106), LHY(42) );
		gr_scanline( LHX(120), LHX(128), LHY(42) );
		gr_pixel( LHX(98), LHY(43) );
		gr_pixel( LHX(98), LHY(44) );
		gr_pixel( LHX(128), LHY(43) );
		gr_pixel( LHX(128), LHY(44) );

		gr_string(LHX(109), LHY(40), "OR");
	}

	for (i=0; i<nitems; i++ )	{
		kc_drawitem( &items[i], 0 );
	}

	citem = 0;
	while(items[citem].id == -1)
		citem++;
	kc_drawitem( &items[citem], 1 );

	newmenu_show_cursor();

#ifdef NEWMENU_MOUSE
	mouse_state = omouse_state = 0;
#endif

	while(1)		{
	//	Windows addendum to allow for kconfig input.
		gr_update();

		//see if redbook song needs to be restarted
		songs_check_redbook_repeat();

		k = key_inkey();

#ifdef NEWMENU_MOUSE
		omouse_state = mouse_state;
		mouse_state = mouse_button_state(0);
#endif

		if ( !time_stopped ) {
			#ifdef NETWORK
			if (multi_menu_poll() == -1)
				k = -2;
			#endif
		}
		ocitem = citem;
		switch( k )	{
		case KEY_BACKSP:
			Int3();
			break;
		case KEY_COMMAND+KEY_SHIFTED+KEY_3:
		case KEY_PRINT_SCREEN:
			save_screen_shot(0);
			break;							
		case KEY_CTRLED+KEY_D:
			items[citem].value = 255;
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_CTRLED+KEY_R:	
			if ( items == kc_keyboard )
				for (i = 0; i < NUM_KEY_CONTROLS; i++) {
					items[i].value = default_kc_keyboard_settings[i];
					kc_drawitem( &items[i], 0 );
				}
			else if ( items == kc_other )
				for (i = 0; i < NUM_OTHER_CONTROLS; i++) {
					items[i].value = default_kc_other_settings[i];
					kc_drawitem( &items[i], 0 );
				}
			else if ( items == kc_d2x )
				for (i = 0; i < NUM_D2X_CONTROLS; i++)
				{
					items[i].value = default_kc_d2x_settings[i];
					kc_drawitem( &items[i], 0 );
				}
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_DELETE:
			items[citem].value=255;
			kc_drawitem( &items[citem], 1 );
			break;
		case KEY_UP: 		
		case KEY_PAD8:
#ifdef TABLE_CREATION
			if (items[citem].u==-1) items[citem].u=find_next_item_up( items,nitems, citem);
#endif
			citem = items[citem].u; 
			break;
		
		case KEY_DOWN: 	
		case KEY_PAD2:
#ifdef TABLE_CREATION
			if (items[citem].d==-1) items[citem].d=find_next_item_down( items,nitems, citem);
#endif
			citem = items[citem].d; 
			break;
		case KEY_LEFT: 	
		case KEY_PAD4:
#ifdef TABLE_CREATION
			if (items[citem].l==-1) items[citem].l=find_next_item_left( items,nitems, citem);
#endif
			citem = items[citem].l; 
			break;
		case KEY_RIGHT: 	
		case KEY_PAD6:
#ifdef TABLE_CREATION
			if (items[citem].r==-1) items[citem].r=find_next_item_right( items,nitems, citem);
#endif
			citem = items[citem].r; 
			break;
		case KEY_ENTER:	
		case KEY_PADENTER:	
			switch( items[citem].type )	{
			case BT_KEY:		kc_change_key( &items[citem] ); break;
			case BT_MOUSE_AXIS: 	kc_change_mouseaxis( &items[citem] ); break;
			case BT_JOY_AXIS: 	kc_change_joyaxis( &items[citem] ); break;
			case BT_INVERT: 	kc_change_invert( &items[citem] ); break;
			}
			break;
		//the following case added by WraithX on 11/22/00 to work around the weird joystick bug...
		case KEY_SPACEBAR:
			switch(items[citem].type)
			{
			case BT_JOY_AXIS:
				kc_next_joyaxis(&items[citem]);
				break;
			}
			break;
		//end addition by WraithX
		case -2:	
		case KEY_ESC:
			grd_curcanv->cv_font	= save_font;

			gr_set_current_canvas( save_canvas );
			keyd_repeat = old_keyd_repeat;
			game_flush_inputs();
			newmenu_hide_cursor();
			if (time_stopped)
				start_time();
			return;
#ifdef TABLE_CREATION
		case KEY_DEBUGGED+KEY_SHIFTED+KEY_2:
		case KEY_DEBUGGED+KEY_F12:	{
			FILE * fp;
			int j;

			for (i=0; i<NUM_KEY_CONTROLS; i++ )	{
				kc_keyboard[i].u = find_next_item_up( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].d = find_next_item_down( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].l = find_next_item_left( kc_keyboard,NUM_KEY_CONTROLS, i);
				kc_keyboard[i].r = find_next_item_right( kc_keyboard,NUM_KEY_CONTROLS, i);
			}
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				kc_other[i].u = find_next_item_up( kc_other,NUM_OTHER_CONTROLS, i);
				kc_other[i].d = find_next_item_down( kc_other,NUM_OTHER_CONTROLS, i);
				kc_other[i].l = find_next_item_left( kc_other,NUM_OTHER_CONTROLS, i);
				kc_other[i].r = find_next_item_right( kc_other,NUM_OTHER_CONTROLS, i);
			}
			fp = stderr; //fopen( "kconfig.cod", "wt" );

			fprintf( fp, "kc_item kc_keyboard[NUM_KEY_CONTROLS] = {\n" );
			for (i=0; i<NUM_KEY_CONTROLS; i++ )	{
				fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n", 
					kc_keyboard[i].id, kc_keyboard[i].x, kc_keyboard[i].y, kc_keyboard[i].w1, kc_keyboard[i].w2,
					kc_keyboard[i].u, kc_keyboard[i].d, kc_keyboard[i].l, kc_keyboard[i].r,
                                        34, kc_keyboard[i].text, 34, btype_text[kc_keyboard[i].type] );
			}
			fprintf( fp, "};\n\n" );
			fprintf( fp, "ubyte default_kc_keyboard_settings[MAX_CONTROLS] = " );
			fprintf( fp, "{0x%x", kc_keyboard[0].value );
			for (j = 1; j < MAX_CONTROLS; j++)
				fprintf( fp, ",0x%x", kc_keyboard[j].value );
			fprintf( fp, "};\n\n" );

			fprintf( fp, "kc_item kc_other[NUM_OTHER_CONTROLS] = {\n" );
			for (i=0; i<NUM_OTHER_CONTROLS; i++ )	{
				fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n",
					kc_other[i].id, kc_other[i].x, kc_other[i].y, kc_other[i].w1, kc_other[i].w2,
					kc_other[i].u, kc_other[i].d, kc_other[i].l, kc_other[i].r,
					34, kc_other[i].text, 34, btype_text[kc_other[i].type] );
			}
			fprintf( fp, "};\n\n" );
			fprintf( fp, "ubyte default_kc_other_settings[MAX_CONTROLS] = " );
			fprintf( fp, "{0x%x", kc_other[0].value );
			for (j = 1; j < MAX_CONTROLS; j++)
				fprintf( fp, ",0x%x", kc_other[j].value );
			fprintf( fp, "};\n" );

			fprintf( fp, "kc_item kc_d2x[NUM_D2X_CONTROLS] = {\n" );
			for (i=0; i<NUM_D2X_CONTROLS; i++ )	{
				fprintf( fp, "\t{ %2d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%c%s%c, %s, 255 },\n",
						kc_d2x[i].id, kc_d2x[i].x, kc_d2x[i].y, kc_d2x[i].w1, kc_d2x[i].w2,
						kc_d2x[i].u, kc_d2x[i].d, kc_d2x[i].l, kc_d2x[i].r,
						34, kc_d2x[i].text, 34, btype_text[kc_d2x[i].type] );
			}
			fprintf( fp, "};\n\n" );
			fprintf( fp, "ubyte default_kc_d2x_settings[MAX_CONTROLS] = " );
			fprintf( fp, "{0x%x", kc_d2x[0].value );
			for (j = 1; j < MAX_CONTROLS; j++)
				fprintf( fp, ",0x%x", kc_d2x[j].value );
			fprintf( fp, "};\n" );

			fclose(fp);

			}
			break;
#endif
		}

#ifdef NEWMENU_MOUSE
		if ( (mouse_state && !omouse_state) || (mouse_state && omouse_state) ) {
			int item_height;
			
			mouse_get_pos(&mx, &my);
			for (i=0; i<nitems; i++ )	{
				item_height = get_item_height( &items[i] );
				x1 = grd_curcanv->cv_bitmap.bm_x + LHX(items[i].x) + LHX(items[i].w1);
				x2 = x1 + LHX(items[i].w2);
				y1 = grd_curcanv->cv_bitmap.bm_y + LHY(items[i].y);
				y2 = y1 + LHX(item_height);
				if (((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2))) {
					citem = i;
					break;
				}
			}
		}
		else if ( !mouse_state && omouse_state ) {
			int item_height;
			
			mouse_get_pos(&mx, &my);
			item_height = get_item_height( &items[citem] );
			x1 = grd_curcanv->cv_bitmap.bm_x + LHX(items[citem].x) + LHX(items[citem].w1);
			x2 = x1 + LHX(items[citem].w2);
			y1 = grd_curcanv->cv_bitmap.bm_y + LHY(items[citem].y);
			y2 = y1 + LHY(item_height);
			if (((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2))) {
				newmenu_hide_cursor();
				switch( items[citem].type )	{
				case BT_KEY:				kc_change_key( &items[citem] ); break;
				case BT_MOUSE_AXIS: 		kc_change_mouseaxis( &items[citem] ); break;
				case BT_JOY_AXIS: 		kc_change_joyaxis( &items[citem] ); break;
				case BT_INVERT: 			kc_change_invert( &items[citem] ); break;
				}
				newmenu_show_cursor();
			} else {
				x1 = grd_curcanv->cv_bitmap.bm_x + close_x + LHX(1);
				x2 = x1 + close_size - LHX(1);
				y1 = grd_curcanv->cv_bitmap.bm_y + close_y + LHX(1);
				y2 = y1 + close_size - LHX(1);
				if ( ((mx > x1) && (mx < x2)) && ((my > y1) && (my < y2)) ) {
					grd_curcanv->cv_font	= save_font;
					gr_set_current_canvas( save_canvas );
					keyd_repeat = old_keyd_repeat;
					game_flush_inputs();
					newmenu_hide_cursor();
					if (time_stopped)
						start_time();
					return;
				}
			}

		}
#endif // NEWMENU_MOUSE

		if (ocitem!=citem)	{
			newmenu_hide_cursor();
			kc_drawitem( &items[ocitem], 0 );
			kc_drawitem( &items[citem], 1 );
			newmenu_show_cursor();
		}
	}
}


void kc_drawitem( kc_item *item, int is_current )
{
	int x, w, h, aw;
	char btext[16];

	if (is_current)
		gr_set_fontcolor( BM_XRGB(20,20,29), -1 );
	else
		gr_set_fontcolor( BM_XRGB(15,15,24), -1 );
   gr_string( LHX(item->x), LHY(item->y), item->text );

	if (item->value==255) {
		strcpy( btext, "" );
	} else {
		switch( item->type )	{
			case BT_KEY:
				strncpy( btext, key_text[item->value], 10 ); break;
			case BT_MOUSE_AXIS:
				strncpy( btext, Text_string[mouseaxis_text[item->value]], 10 ); break;
			case BT_JOY_AXIS:
#ifdef USE_LINUX_JOY
				sprintf(btext, "J%d A%d", j_axis[item->value].joydev, j_Get_joydev_axis_number(item->value));
#elif defined(SDL_INPUT)
				if (joyaxis_text[item->value])
					strncpy(btext, joyaxis_text[item->value], 10);
				else
					sprintf(btext, "AXIS%2d", item->value + 1);
#else
				strncpy(btext, Text_string[joyaxis_text[item->value]], 10);
#endif
				break;
			case BT_INVERT:
				strncpy( btext, Text_string[invert_text[item->value]], 10 ); break;
		}
	}
	if (item->w1) {
		gr_get_string_size(btext, &w, &h, &aw  );

		if (is_current)
			gr_setcolor( BM_XRGB(21,0,24) );
		else
			gr_setcolor( BM_XRGB(16,0,19) );
		gr_urect( LHX(item->w1+item->x), LHY(item->y-1), LHX(item->w1+item->x+item->w2), LHY(item->y)+h );
		
		gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

		x = LHX(item->w1+item->x)+((LHX(item->w2)-w)/2);
	
		gr_string( x, LHY(item->y), btext );
	}
}


static int looper=0;

void kc_drawquestion( kc_item *item )
{
	int c, x, w, h, aw;

	gr_get_string_size("?", &w, &h, &aw  );

	c = BM_XRGB(21,0,24);

	//@@gr_setcolor( gr_fade_table[fades[looper]*256+c] );
	gr_setcolor(BM_XRGB(21*fades[looper]/31,0,24*fades[looper]/31));
	looper++;
	if (looper>63) looper=0;

	gr_urect( LHX(item->w1+item->x), LHY(item->y-1), LHX(item->w1+item->x+item->w2), LHY(item->y)+h );
	
	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );

	x = LHX(item->w1+item->x)+((LHX(item->w2)-w)/2);
   
	gr_string( x, LHY(item->y), "?" );
gr_update();
}

void kc_change_key( kc_item * item )
{
	int i,n,f,k;
	ubyte keycode;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_PRESS_NEW_KEY );

	game_flush_inputs();
	keycode=255;
	k=255;
	
	while( (k!=KEY_ESC) && (keycode==255) )	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);
		kc_drawquestion( item );
	
		for (i=0; i<256; i++ )	{
			if (keyd_pressed[i] && (strlen(key_text[i])>0))	{
				f = 0;
				for (n=0; n<sizeof(system_keys); n++ )
					if ( system_keys[n] == i )
						f=1;
				if (!f)	
					keycode=i;
			}
		}
	}

	if (k!=KEY_ESC)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_KEY) && (All_items[i].value==keycode) )		{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = keycode;
	}
	kc_drawitem( item, 1 );

	gr_set_fontcolor( BM_XRGB(28,28,28), BM_XRGB(0,0,0) );

	nm_restore_background( 0, LHY(INFO_Y), LHX(310), grd_curcanv->cv_font->ft_h );

	game_flush_inputs();

}


// the following function added by WraithX on 11/22/00 to work around the weird joystick bug... - modified my Matt Mueller to skip already allocated axes
void kc_next_joyaxis(kc_item *item)
{
	int n, i, k, max, tries;
	ubyte code = 0;

	k = 255;
	n = 0;
	i = 0;

	// I modelled this ifdef after the code in the kc_change_joyaxis method.
	// So, if somethin's not workin here, it might not be workin there either.
	max = JOY_MAX_AXES;
	tries = 1;
	code = (item->value + 1) % max;

	if (code != 255)
	{
		for (i = 0; i < Num_items; i++)
		{
			n = item - All_items;
			if ((i != n) && (All_items[i].type == BT_JOY_AXIS) && (All_items[i].value == code))
			{
				if (tries > max)
					return; // all axes allocated already
				i = -1; // -1 so the i++ will push back to 0
				code = (item->value + ++tries) % max; // try next axis
			}//end if
		}//end for

		item->value = code;
	}//end if

	kc_drawitem(item, 1);
	nm_restore_background(0, LHY(INFO_Y), LHX(310), grd_curcanv->cv_font->ft_h);
	game_flush_inputs();

}//method kc_next_joyaxis
//end addition by WraithX


void kc_change_joyaxis( kc_item * item )
{
	int axis[JOY_MAX_AXES];
	int old_axis[JOY_MAX_AXES];
	int numaxis = joy_num_axes;
	int n,i,k;
	ubyte code;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_MOVE_NEW_JOY_AXIS );

	game_flush_inputs();
	code=255;
	k=255;

	joystick_read_raw_axis( JOY_ALL_AXIS, old_axis );

	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

		joystick_read_raw_axis( JOY_ALL_AXIS, axis );

		for (i=0; i<numaxis; i++ )	{
			if ( abs(axis[i]-old_axis[i])>100 )
			{
				code = i;
				con_printf(CON_DEBUG, "Axis Movement detected: Axis %i\n", i);
			}
			//old_axis[i] = axis[i];
		}
		for (i=0; i<Num_items; i++ )	
		 {
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_JOY_AXIS) && (All_items[i].value==code) )	
				code = 255;
		 }
	
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_JOY_AXIS) && (All_items[i].value==code) )	{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}

		item->value = code;					 
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), grd_curcanv->cv_font->ft_h );
	game_flush_inputs();

}

void kc_change_mouseaxis( kc_item * item )
{
	int i,n,k;
	ubyte code;
	int dx, dy, dz;

	gr_set_fontcolor( BM_XRGB(28,28,28), -1 );
	
	gr_string( 0x8000, LHY(INFO_Y), TXT_MOVE_NEW_MSE_AXIS );

	game_flush_inputs();
	code=255;
	k=255;

	mouse_get_delta( &dx, &dy, &dz );

	while( (k!=KEY_ESC) && (code==255))	
	{				
		#ifdef NETWORK
		if ((Game_mode & GM_MULTI) && (Function_mode == FMODE_GAME) && (!Endlevel_sequence))
			multi_menu_poll();
		#endif
//		if ( Game_mode & GM_MULTI )
//			GameLoop( 0, 0 );				// Continue
		k = key_inkey();
		timer_delay(f0_1/10);

		if (k == KEY_PRINT_SCREEN)
			save_screen_shot(0);

		kc_drawquestion( item );

		mouse_get_delta( &dx, &dy, &dz );
		if ( abs(dx)>20 ) code = 0;
		if ( abs(dy)>20 ) code = 1;
		if ( abs(dz)>20 ) code = 2;
	}
	if (code!=255)	{
		for (i=0; i<Num_items; i++ )	{
			n = item - All_items;
			if ( (i!=n) && (All_items[i].type==BT_MOUSE_AXIS) && (All_items[i].value==code) )		{
				All_items[i].value = 255;
				kc_drawitem( &All_items[i], 0 );
			}
		}
		item->value = code;
	}
	kc_drawitem( item, 1 );
	nm_restore_background( 0, LHY(INFO_Y), LHX(310), grd_curcanv->cv_font->ft_h );
	game_flush_inputs();

}


void kc_change_invert( kc_item * item )
{
	game_flush_inputs();

	if (item->value)
		item->value = 0;
	else 
		item->value = 1;

	kc_drawitem( item, 1 );

}

#include "screens.h"

void kconfig(int n, char * title)
{
	int i, j;
	grs_bitmap *save_bm;

	set_screen_mode( SCREEN_MENU );

	kc_set_controls();

	//save screen
	save_bm = gr_create_bitmap( grd_curcanv->cv_bitmap.bm_w, grd_curcanv->cv_bitmap.bm_h );
	Assert( save_bm != NULL );
	
	gr_bm_bitblt(grd_curcanv->cv_bitmap.bm_w, grd_curcanv->cv_bitmap.bm_w, 
					0, 0, 0, 0, &grd_curcanv->cv_bitmap, save_bm );

	switch(n)	{
	case 0:kconfig_sub( kc_keyboard, NUM_KEY_CONTROLS, title );break;
	case 1:kconfig_sub( kc_other, NUM_OTHER_CONTROLS, title );break;
	case 2:kconfig_sub( kc_d2x, NUM_D2X_CONTROLS, title ); break;
 	default:
		Int3();
		return;
	}

	//restore screen
	gr_bitmap(0, 0, save_bm);
	gr_free_bitmap(save_bm);

#if 0 // set_screen_mode always calls this later... right?
	reset_cockpit();		//force cockpit redraw next time
#endif

	// Update save values...
	
	for (j=0; j<256; j++)
		if (key_binding(j)) {
			for (i = 0; i < NUM_KEY_CONTROLS; i++)
				if (!stricmp(key_binding(j), kc_key_bind_text[i])) {
					cmd_appendf("unbind %s", key_text[j]);
					break;
				}
			for (i = 0; i < NUM_D2X_CONTROLS; i++)
				if (kc_d2x[i].type == BT_KEY && !stricmp(key_binding(j), kc_d2x[i].text)) {
					cmd_appendf("unbind %s", key_text[j]);
					break;
				}
		}

	for (i=0; i<NUM_KEY_CONTROLS; i++ )
		if (kc_keyboard[i].value != 255)
			cmd_appendf("bind %s \"%s\"", key_text[kc_keyboard[i].value], kc_key_bind_text[i]);

	for (i = 0; i < NUM_D2X_CONTROLS; i++)
		if (kc_d2x[i].value != 255)
			cmd_appendf("bind %s \"%s\"", key_text[kc_d2x[i].value], kc_d2x[i].text);

	for (i = 0; i < 6; i++) {
		cvar_setint(&joy_advaxes[i], AXIS_NONE);
		cvar_setint(&joy_invert[i], 0);
	}
	for (i = 0; i < 3; i++) {
		cvar_setint(&mouse_axes[i], AXIS_NONE);
		cvar_setint(&mouse_invert[i], 0);
	}
	for (i = 0; i < NUM_OTHER_CONTROLS; i++) {
		if (kc_other[i].type == BT_JOY_AXIS && kc_other[i].value != 255) {
			cvar_setint(&joy_advaxes[kc_other[i].value], kc_other_axismap[i]);
			cvar_setint(&joy_invert[kc_other[i].value], kc_other[i+1].value);
		}
		if (kc_other[i].type == BT_MOUSE_AXIS && kc_other[i].value != 255) {
			cvar_setint(&mouse_axes[kc_other[i].value], kc_other_axismap[i]);
			cvar_setint(&mouse_invert[kc_other[i].value], kc_other[i+1].value);
		}
	}

	cmd_queue_process();
}


fix Last_angles_p = 0;
fix Last_angles_b = 0;
fix Last_angles_h = 0;
ubyte Last_angles_read = 0;

extern int			VR_sensitivity;
						
int VR_sense_range[3] = { 25, 50, 75 };

#if 0 // unused
read_head_tracker()
{
	fix yaw, pitch, roll;
	int buttons;

//------ read vfx1 helmet --------
	if (vfx1_installed) {
		vfx_get_data(&yaw,&pitch,&roll,&buttons);
	} else if (iglasses_headset_installed)	{
		iglasses_read_headset( &yaw, &pitch, &roll );
	} else if (Victor_headset_installed)   {
		victor_read_headset_filtered( &yaw, &pitch, &roll );
	} else {
		return;
	}

	Use_player_head_angles = 0;
	if ( Last_angles_read )	{
		fix yaw1 = yaw;
		
		yaw1 = yaw;
		if ( (Last_angles_h < (F1_0/4) ) && (yaw > ((F1_0*3)/4) ) )	
			yaw1 -= F1_0;
		else if ( (yaw < (F1_0/4) ) && (Last_angles_h > ((F1_0*3)/4) ) )	
			yaw1 += F1_0;
	
		Controls.pitch_time	+= fixmul((pitch- Last_angles_p)*VR_sense_range[VR_sensitivity],FrameTime);
		Controls.heading_time+= fixmul((yaw1 -  Last_angles_h)*VR_sense_range[VR_sensitivity],FrameTime);
		Controls.bank_time	+= fixmul((roll - Last_angles_b)*VR_sense_range[VR_sensitivity],FrameTime);
	}
	Last_angles_read = 1;
	Last_angles_p = pitch;
	Last_angles_h = yaw;
	Last_angles_b = roll;
}
#endif


void kc_set_controls()
{
	int i, j;

	for (i=0; i<NUM_KEY_CONTROLS; i++ )
		kc_keyboard[i].value = 255;

	for (i=0; i<NUM_OTHER_CONTROLS; i++ ) {
		if (kc_other[i].type == BT_INVERT)
			kc_other[i].value = 0;
		else
			kc_other[i].value = 255;
	}

	for (i=0; i<NUM_D2X_CONTROLS; i++ )
		kc_d2x[i].value = 255;

	for (j = 0; j < 256; j++)
		if (key_binding(j)) {
			for (i = 0; i < NUM_KEY_CONTROLS; i++)
				if (kc_keyboard[i].value == 255
					&& !stricmp(key_binding(j), kc_key_bind_text[i])) {
					kc_keyboard[i].value = j;
					break;
				}
		}

	for(j = 0; j < 256; j++)
		if (key_binding(j)) {
			for (i = 0; i < NUM_D2X_CONTROLS; i++)
				if (kc_d2x[i].value == 255
					&& !stricmp(key_binding(j), kc_d2x[i].text)) {
					kc_d2x[i].value = j;
					break;
				}
		}

	for (i = 0; i < 6; i++) {
		int inv = joy_invert[i].intval;
		switch (joy_advaxes[i].intval) {
			case AXIS_PITCH:     kc_other[ 0].value = i; kc_other[ 1].value = inv; break;
			case AXIS_TURN:      kc_other[ 2].value = i; kc_other[ 3].value = inv; break;
			case AXIS_LEFTRIGHT: kc_other[ 4].value = i; kc_other[ 5].value = inv; break;
			case AXIS_UPDOWN:    kc_other[ 6].value = i; kc_other[ 7].value = inv; break;
			case AXIS_BANK:      kc_other[ 8].value = i; kc_other[ 9].value = inv; break;
			case AXIS_THROTTLE:  kc_other[10].value = i; kc_other[11].value = inv; break;
			case AXIS_NONE:      break;
			default:
				Int3();
				break;
		}
	}

	for (i = 0; i < 3; i++) {
		int inv = mouse_invert[i].intval;
		switch (mouse_axes[i].intval) {
			case AXIS_PITCH:     kc_other[12].value = i; kc_other[13].value = inv; break;
			case AXIS_TURN:      kc_other[14].value = i; kc_other[15].value = inv; break;
			case AXIS_LEFTRIGHT: kc_other[16].value = i; kc_other[17].value = inv; break;
			case AXIS_UPDOWN:    kc_other[18].value = i; kc_other[19].value = inv; break;
			case AXIS_BANK:      kc_other[20].value = i; kc_other[21].value = inv; break;
			case AXIS_THROTTLE:  kc_other[22].value = i; kc_other[23].value = inv; break;
			case AXIS_NONE:      break;
			default:
				Int3();
				break;
		}
	}
}

#if 0 // no mac support for vr headset

void kconfig_center_headset()
{
	if (vfx1_installed)
		vfx_center_headset();
//	} else if (iglasses_headset_installed)	{
//	} else if (Victor_headset_installed)   {
//	} else {
//	}

}

#endif // end of #if for kconfig_center_headset
