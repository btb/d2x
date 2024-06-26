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
 * Inferno gauge drivers
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "inferno.h"
#include "dxxerror.h"
#include "mono.h"
#ifdef OGL
#include "ogl_init.h"
#endif


void draw_ammo_info(int x,int y,int ammo_count,int primary);
extern void draw_guided_crosshair(void);

bitmap_index Gauges[MAX_GAUGE_BMS];   // Array of all gauge bitmaps.
bitmap_index Gauges_hires[MAX_GAUGE_BMS];   // hires gauges

grs_canvas *Canv_LeftEnergyGauge = NULL;
grs_canvas *Canv_AfterburnerGauge = NULL;
grs_canvas *Canv_SBEnergyGauge = NULL;
grs_canvas *Canv_SBAfterburnerGauge = NULL;
grs_canvas *Canv_RightEnergyGauge = NULL;
grs_canvas *Canv_NumericalGauge = NULL;

//Flags for gauges/hud stuff
cvar_t Reticle_on = { "crosshair", "1", CVAR_ARCHIVE };

//bitmap numbers for gauges

#define GAUGE_SHIELDS			0		//0..9, in decreasing order (100%,90%...0%)

#define GAUGE_INVULNERABLE		10		//10..19
#define N_INVULNERABLE_FRAMES	10

#define GAUGE_AFTERBURNER   	20
#define GAUGE_ENERGY_LEFT		21
#define GAUGE_ENERGY_RIGHT		22
#define GAUGE_NUMERICAL			23

#define GAUGE_BLUE_KEY			24
#define GAUGE_GOLD_KEY			25
#define GAUGE_RED_KEY			26
#define GAUGE_BLUE_KEY_OFF		27
#define GAUGE_GOLD_KEY_OFF		28
#define GAUGE_RED_KEY_OFF		29

#define SB_GAUGE_BLUE_KEY		30
#define SB_GAUGE_GOLD_KEY		31
#define SB_GAUGE_RED_KEY		32
#define SB_GAUGE_BLUE_KEY_OFF	33
#define SB_GAUGE_GOLD_KEY_OFF	34
#define SB_GAUGE_RED_KEY_OFF	35

#define SB_GAUGE_ENERGY			36

#define GAUGE_LIVES				37	

#define GAUGE_SHIPS				38
#define GAUGE_SHIPS_LAST		45

#define RETICLE_CROSS			46
#define RETICLE_PRIMARY			48
#define RETICLE_SECONDARY		51
#define RETICLE_LAST				55

#define GAUGE_HOMING_WARNING_ON	56
#define GAUGE_HOMING_WARNING_OFF	57

#define SML_RETICLE_CROSS		58
#define SML_RETICLE_PRIMARY	60
#define SML_RETICLE_SECONDARY	63
#define SML_RETICLE_LAST		67

#define KEY_ICON_BLUE			68
#define KEY_ICON_YELLOW			69
#define KEY_ICON_RED				70

#define SB_GAUGE_AFTERBURNER	71

#define FLAG_ICON_RED			72
#define FLAG_ICON_BLUE			73


/* Use static inline function under GCC to avoid CR/LF issues */
#ifdef __GNUC__
#define PAGE_IN_GAUGE(x) _page_in_gauge(x)
static inline void _page_in_gauge(int x)
{
    if (FontHires) {
        PIGGY_PAGE_IN(Gauges_hires[x]);
    } else {
	PIGGY_PAGE_IN(Gauges[x]);
    }
}

#else
#define PAGE_IN_GAUGE(x) \
	do {									 				\
		if (FontHires) {				\
			PIGGY_PAGE_IN(Gauges_hires[x]);	  	\
		} else {										  	\
			PIGGY_PAGE_IN(Gauges[x]);			  	\
		}													\
	} while (0)

#endif

#define GET_GAUGE_INDEX(x)	(FontHires?Gauges_hires[x].index:Gauges[x].index)

//change MAX_GAUGE_BMS when adding gauges

//Coordinats for gauges

// cockpit keys

#define GAUGE_BLUE_KEY_X_L		272
#define GAUGE_BLUE_KEY_Y_L		152
#define GAUGE_BLUE_KEY_X_H		535
#define GAUGE_BLUE_KEY_Y_H		374
#define GAUGE_BLUE_KEY_X        (SM_HIRES?GAUGE_BLUE_KEY_X_H:GAUGE_BLUE_KEY_X_L)
#define GAUGE_BLUE_KEY_Y        (SM_HIRES?GAUGE_BLUE_KEY_Y_H:GAUGE_BLUE_KEY_Y_L)

#define GAUGE_GOLD_KEY_X_L		273
#define GAUGE_GOLD_KEY_Y_L		162
#define GAUGE_GOLD_KEY_X_H		537
#define GAUGE_GOLD_KEY_Y_H		395
#define GAUGE_GOLD_KEY_X        (SM_HIRES?GAUGE_GOLD_KEY_X_H:GAUGE_GOLD_KEY_X_L)
#define GAUGE_GOLD_KEY_Y        (SM_HIRES?GAUGE_GOLD_KEY_Y_H:GAUGE_GOLD_KEY_Y_L)

#define GAUGE_RED_KEY_X_L		274
#define GAUGE_RED_KEY_Y_L		172
#define GAUGE_RED_KEY_X_H		539
#define GAUGE_RED_KEY_Y_H		416
#define GAUGE_RED_KEY_X         (SM_HIRES?GAUGE_RED_KEY_X_H:GAUGE_RED_KEY_X_L)
#define GAUGE_RED_KEY_Y         (SM_HIRES?GAUGE_RED_KEY_Y_H:GAUGE_RED_KEY_Y_L)

// status bar keys

#define SB_GAUGE_KEYS_X_L	   11
#define SB_GAUGE_KEYS_X_H		26
#define SB_GAUGE_KEYS_X         (SM_HIRES?SB_GAUGE_KEYS_X_H:SB_GAUGE_KEYS_X_L)

#define SB_GAUGE_BLUE_KEY_Y_L		153
#define SB_GAUGE_GOLD_KEY_Y_L		169
#define SB_GAUGE_RED_KEY_Y_L		185

#define SB_GAUGE_BLUE_KEY_Y_H		390
#define SB_GAUGE_GOLD_KEY_Y_H		422
#define SB_GAUGE_RED_KEY_Y_H		454

#define SB_GAUGE_BLUE_KEY_Y     (SM_HIRES?SB_GAUGE_BLUE_KEY_Y_H:SB_GAUGE_BLUE_KEY_Y_L)
#define SB_GAUGE_GOLD_KEY_Y     (SM_HIRES?SB_GAUGE_GOLD_KEY_Y_H:SB_GAUGE_GOLD_KEY_Y_L)
#define SB_GAUGE_RED_KEY_Y      (SM_HIRES?SB_GAUGE_RED_KEY_Y_H:SB_GAUGE_RED_KEY_Y_L)

// cockpit enery gauges

#define LEFT_ENERGY_GAUGE_X_L 	70
#define LEFT_ENERGY_GAUGE_Y_L 	131
#define LEFT_ENERGY_GAUGE_W_L 	64
#define LEFT_ENERGY_GAUGE_H_L 	8

#define LEFT_ENERGY_GAUGE_X_H 	137
#define LEFT_ENERGY_GAUGE_Y_H 	314
#define LEFT_ENERGY_GAUGE_W_H 	133
#define LEFT_ENERGY_GAUGE_H_H 	21

#define LEFT_ENERGY_GAUGE_X     (SM_HIRES?LEFT_ENERGY_GAUGE_X_H:LEFT_ENERGY_GAUGE_X_L)
#define LEFT_ENERGY_GAUGE_Y     (SM_HIRES?LEFT_ENERGY_GAUGE_Y_H:LEFT_ENERGY_GAUGE_Y_L)
#define LEFT_ENERGY_GAUGE_W     (SM_HIRES?LEFT_ENERGY_GAUGE_W_H:LEFT_ENERGY_GAUGE_W_L)
#define LEFT_ENERGY_GAUGE_H     (SM_HIRES?LEFT_ENERGY_GAUGE_H_H:LEFT_ENERGY_GAUGE_H_L)

#define RIGHT_ENERGY_GAUGE_X    (SM_HIRES?380:190)
#define RIGHT_ENERGY_GAUGE_Y    (SM_HIRES?314:131)
#define RIGHT_ENERGY_GAUGE_W    (SM_HIRES?133:64)
#define RIGHT_ENERGY_GAUGE_H    (SM_HIRES?21:8)

// cockpit afterburner gauge

#define AFTERBURNER_GAUGE_X_L	45-1
#define AFTERBURNER_GAUGE_Y_L	158
#define AFTERBURNER_GAUGE_W_L	12
#define AFTERBURNER_GAUGE_H_L	32

#define AFTERBURNER_GAUGE_X_H	88
#define AFTERBURNER_GAUGE_Y_H	377
#define AFTERBURNER_GAUGE_W_H	21
#define AFTERBURNER_GAUGE_H_H	65

#define AFTERBURNER_GAUGE_X     (SM_HIRES?AFTERBURNER_GAUGE_X_H:AFTERBURNER_GAUGE_X_L)
#define AFTERBURNER_GAUGE_Y     (SM_HIRES?AFTERBURNER_GAUGE_Y_H:AFTERBURNER_GAUGE_Y_L)
#define AFTERBURNER_GAUGE_W     (SM_HIRES?AFTERBURNER_GAUGE_W_H:AFTERBURNER_GAUGE_W_L)
#define AFTERBURNER_GAUGE_H     (SM_HIRES?AFTERBURNER_GAUGE_H_H:AFTERBURNER_GAUGE_H_L)

// sb energy gauge

#define SB_ENERGY_GAUGE_X       (SM_HIRES?196:98)
#define SB_ENERGY_GAUGE_Y       (SM_HIRES?382:(155-2))
#define SB_ENERGY_GAUGE_W       (SM_HIRES?32:16)
#define SB_ENERGY_GAUGE_H       (SM_HIRES?60:29)

// sb afterburner gauge

#define SB_AFTERBURNER_GAUGE_X  (SM_HIRES?196:98)
#define SB_AFTERBURNER_GAUGE_Y  (SM_HIRES?446:184)
#define SB_AFTERBURNER_GAUGE_W  (SM_HIRES?33:16)
#define SB_AFTERBURNER_GAUGE_H  (SM_HIRES?29:13)

#define SB_ENERGY_NUM_X         (SB_ENERGY_GAUGE_X+(SM_HIRES?4:2))
#define SB_ENERGY_NUM_Y         (SM_HIRES?457:175)

#define SHIELD_GAUGE_X          (SM_HIRES?292:146)
#define SHIELD_GAUGE_Y          (SM_HIRES?374:155)
#define SHIELD_GAUGE_W          (SM_HIRES?70:35)
#define SHIELD_GAUGE_H          (SM_HIRES?77:32)

#define SHIP_GAUGE_X            (SHIELD_GAUGE_X+(SM_HIRES?11:5))
#define SHIP_GAUGE_Y            (SHIELD_GAUGE_Y+(SM_HIRES?10:5))

#define SB_SHIELD_GAUGE_X       (SM_HIRES?247:123) //139
#define SB_SHIELD_GAUGE_Y       (SM_HIRES?395:163)

#define SB_SHIP_GAUGE_X         (SB_SHIELD_GAUGE_X+(SM_HIRES?11:5))
#define SB_SHIP_GAUGE_Y         (SB_SHIELD_GAUGE_Y+(SM_HIRES?10:5))

#define SB_SHIELD_NUM_X         (SB_SHIELD_GAUGE_X+(SM_HIRES?21:12)) //151
#define SB_SHIELD_NUM_Y         (SB_SHIELD_GAUGE_Y-(SM_HIRES?16:8))  //156 -- MWA used to be hard coded to 156

#define NUMERICAL_GAUGE_X       (SM_HIRES?308:154)
#define NUMERICAL_GAUGE_Y       (SM_HIRES?316:130)
#define NUMERICAL_GAUGE_W       (SM_HIRES?38:19)
#define NUMERICAL_GAUGE_H       (SM_HIRES?55:22)

#define PRIMARY_W_PIC_X         (SM_HIRES?(135-10):64)
#define PRIMARY_W_PIC_Y         (SM_HIRES?370:154)
#define PRIMARY_W_TEXT_X        (SM_HIRES?182:87)
#define PRIMARY_W_TEXT_Y        (SM_HIRES?400:157)
#define PRIMARY_AMMO_X          (SM_HIRES?186:(96-3))
#define PRIMARY_AMMO_Y          (SM_HIRES?420:171)

#define SECONDARY_W_PIC_X       (SM_HIRES?466:234)
#define SECONDARY_W_PIC_Y       (SM_HIRES?374:154)
#define SECONDARY_W_TEXT_X      (SM_HIRES?413:207)
#define SECONDARY_W_TEXT_Y      (SM_HIRES?378:157)
#define SECONDARY_AMMO_X        (SM_HIRES?428:213)
#define SECONDARY_AMMO_Y        (SM_HIRES?407:171)

#define SB_LIVES_X              (SM_HIRES?(550-10-3):266)
#define SB_LIVES_Y              (SM_HIRES?450-3:185)
#define SB_LIVES_LABEL_X        (SM_HIRES?475:237)
#define SB_LIVES_LABEL_Y		(SB_LIVES_Y+1)

#define SB_SCORE_RIGHT_L		301
#define SB_SCORE_RIGHT_H		(605+8)
#define SB_SCORE_RIGHT          (SM_HIRES?SB_SCORE_RIGHT_H:SB_SCORE_RIGHT_L)

#define SB_SCORE_Y              (SM_HIRES?398:158)
#define SB_SCORE_LABEL_X        (SM_HIRES?475:237)

#define SB_SCORE_ADDED_RIGHT    (SM_HIRES?SB_SCORE_RIGHT_H:SB_SCORE_RIGHT_L)
#define SB_SCORE_ADDED_Y        (SM_HIRES?413:165)

#define HOMING_WARNING_X        (SM_HIRES?14:7)
#define HOMING_WARNING_Y        (SM_HIRES?415:171)

#define BOMB_COUNT_X            (SM_HIRES?546:275)
#define BOMB_COUNT_Y            (SM_HIRES?445:186)

#define SB_BOMB_COUNT_X         (SM_HIRES?342:171)
#define SB_BOMB_COUNT_Y         (SM_HIRES?458:191)

#define LHX(x)		((x)*(MenuHires?2:1))
#define LHY(y)		((y)*(MenuHires?2.4:1))

static int score_display[2];
static fix score_time;

static int old_score[2]				= { -1, -1 };
static int old_energy[2]			= { -1, -1 };
static int old_shields[2]			= { -1, -1 };
static int old_flags[2]				= { -1, -1 };
static int old_weapon[2][2]		= {{ -1, -1 },{-1,-1}};
static int old_ammo_count[2][2]	= {{ -1, -1 },{-1,-1}};
static int Old_Omega_charge[2]	= { -1, -1 };
static int old_laser_level[2]		= { -1, -1 };
static int old_cloak[2]				= { 0, 0 };
static int old_lives[2]				= { -1, -1 };
static fix old_afterburner[2]		= { -1, -1 };
static int old_bombcount[2]		= { 0, 0 };

static int invulnerable_frame = 0;

static int cloak_fade_state;		//0=steady, -1 fading out, 1 fading in 

#define WS_SET				0		//in correct state
#define WS_FADING_OUT	1
#define WS_FADING_IN		2

int weapon_box_user[2]={WBU_WEAPON,WBU_WEAPON};		//see WBU_ constants in gauges.h
int weapon_box_states[2] = {WS_SET, WS_SET};
fix weapon_box_fade_values[2];

#define FADE_SCALE	(2*i2f(GR_FADE_LEVELS)/REARM_TIME)		// fade out and back in REARM_TIME, in fade levels per seconds (int)

typedef struct span {
	sbyte l,r;
} span;

//store delta x values from left of box
span weapon_window_left[] = {		//first span 67,151
		{8,51},
		{6,53},
		{5,54},
		{4-1,53+2},
		{4-1,53+3},
		{4-1,53+3},
		{4-2,53+3},
		{4-2,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-1,53+3},
		{3-2,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{2-1,53+3},
		{1-1,53+3},
		{1-1,53+2},
		{1-1,53+2},
		{1-1,53+2},
		{1-1,53+2},
		{1-1,53+2},
		{1-1,53+2},
		{1-1,53+2},
		{0,53+2},
		{0,53+2},
		{0,53+2},
		{0,53+2},
		{0,52+3},
		{1-1,52+2},
		{2-2,51+3},
		{3-2,51+2},
		{4-2,50+2},
		{5-2,50},
		{5-2+2,50-2},
	};


//store delta x values from left of box
span weapon_window_right[] = {		//first span 207,154
		{208-202,255-202},
		{206-202,257-202},
		{205-202,258-202},
		{204-202,259-202},
		{203-202,260-202},
		{203-202,260-202},
		{203-202,260-202},
		{203-202,260-202},
		{203-202,260-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,261-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{203-202,262-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{204-202,263-202},
		{205-202,263-202},
		{206-202,262-202},
		{207-202,261-202},
		{208-202,260-202},
		{211-202,255-202},
	};

//store delta x values from left of box
span weapon_window_left_hires[] = {		//first span 67,154
	{20,110},
	{18,113},
	{16,114},
	{15,116},
	{14,117},
	{13,118},
	{12,119},
	{11,119},
	{10,120},
	{10,120},
	{9,121},
	{8,121},
	{8,121},
	{8,122},
	{7,122},
	{7,122},
	{7,122},
	{7,122},
	{7,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{6,122},
	{5,122},
	{5,122},
	{5,122},
	{5,122},
	{5,121},
	{5,121},
	{5,121},
	{5,121},
	{5,121},
	{5,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{4,121},
	{3,121},
	{3,121},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{3,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{2,120},
	{1,120},
	{1,120},
	{1,119},
	{1,119},
	{1,119},
	{1,119},
	{1,119},
	{1,119},
	{1,119},
	{1,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,118},
	{0,118},
	{0,118},
	{0,117},
	{0,117},
	{0,117},
	{1,116},
	{1,116},
	{2,115},
	{2,114},
	{3,113},
	{4,112},
	{5,111},
	{5,110},
	{7,109},
	{9,107},
	{10,105},
	{12,102},
};


//store delta x values from left of box
span weapon_window_right_hires[] = {		//first span 207,154
	{12,105},
	{9,107},
	{8,109},
	{6,110},
	{5,111},
	{4,112},
	{3,113},
	{3,114},
	{2,115},
	{2,115},
	{1,116},
	{1,117},
	{1,117},
	{0,117},
	{0,118},
	{0,118},
	{0,118},
	{0,118},
	{0,118},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,119},
	{0,120},
	{0,120},
	{0,120},
	{0,120},
	{1,120},
	{1,120},
	{1,120},
	{1,120},
	{1,120},
	{1,120},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,121},
	{1,122},
	{1,122},
	{2,122},
	{2,122},
	{2,122},
	{2,122},
	{2,122},
	{2,122},
	{2,122},
	{2,122},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,123},
	{2,124},
	{2,124},
	{3,124},
	{3,124},
	{3,124},
	{3,124},
	{3,124},
	{3,124},
	{3,124},
	{3,125},
	{3,125},
	{3,125},
	{3,125},
	{3,125},
	{3,125},
	{3,125},
	{3,125},
	{4,125},
	{4,125},
	{4,125},
	{5,125},
	{5,125},
	{5,125},
	{6,125},
	{6,124},
	{7,123},
	{8,123},
	{9,122},
	{10,121},
	{11,120},
	{12,120},
	{13,118},
	{15,117},
	{18,115},
	{20,114},
};

											
#define N_LEFT_WINDOW_SPANS  (sizeof(weapon_window_left)/sizeof(*weapon_window_left))
#define N_RIGHT_WINDOW_SPANS (sizeof(weapon_window_right)/sizeof(*weapon_window_right))

#define N_LEFT_WINDOW_SPANS_H  (sizeof(weapon_window_left_hires)/sizeof(*weapon_window_left_hires))
#define N_RIGHT_WINDOW_SPANS_H (sizeof(weapon_window_right_hires)/sizeof(*weapon_window_right_hires))

// defining box boundries for weapon pictures

#define PRIMARY_W_BOX_LEFT_L		63
#define PRIMARY_W_BOX_TOP_L		151		//154
#define PRIMARY_W_BOX_RIGHT_L		(PRIMARY_W_BOX_LEFT_L+58)
#define PRIMARY_W_BOX_BOT_L		(PRIMARY_W_BOX_TOP_L+N_LEFT_WINDOW_SPANS-1)
											
#define PRIMARY_W_BOX_LEFT_H		121
#define PRIMARY_W_BOX_TOP_H		364
#define PRIMARY_W_BOX_RIGHT_H		242
#define PRIMARY_W_BOX_BOT_H		(PRIMARY_W_BOX_TOP_H+N_LEFT_WINDOW_SPANS_H-1)		//470
											
#define PRIMARY_W_BOX_LEFT      (SM_HIRES?PRIMARY_W_BOX_LEFT_H:PRIMARY_W_BOX_LEFT_L)
#define PRIMARY_W_BOX_TOP       (SM_HIRES?PRIMARY_W_BOX_TOP_H:PRIMARY_W_BOX_TOP_L)
#define PRIMARY_W_BOX_RIGHT     (SM_HIRES?PRIMARY_W_BOX_RIGHT_H:PRIMARY_W_BOX_RIGHT_L)
#define PRIMARY_W_BOX_BOT       (SM_HIRES?PRIMARY_W_BOX_BOT_H:PRIMARY_W_BOX_BOT_L)

#define SECONDARY_W_BOX_LEFT_L	202	//207
#define SECONDARY_W_BOX_TOP_L		151
#define SECONDARY_W_BOX_RIGHT_L	263	//(SECONDARY_W_BOX_LEFT+54)
#define SECONDARY_W_BOX_BOT_L		(SECONDARY_W_BOX_TOP_L+N_RIGHT_WINDOW_SPANS-1)

#define SECONDARY_W_BOX_LEFT_H	404
#define SECONDARY_W_BOX_TOP_H		363
#define SECONDARY_W_BOX_RIGHT_H	529
#define SECONDARY_W_BOX_BOT_H		(SECONDARY_W_BOX_TOP_H+N_RIGHT_WINDOW_SPANS_H-1)		//470

#define SECONDARY_W_BOX_LEFT    (SM_HIRES?SECONDARY_W_BOX_LEFT_H:SECONDARY_W_BOX_LEFT_L)
#define SECONDARY_W_BOX_TOP     (SM_HIRES?SECONDARY_W_BOX_TOP_H:SECONDARY_W_BOX_TOP_L)
#define SECONDARY_W_BOX_RIGHT   (SM_HIRES?SECONDARY_W_BOX_RIGHT_H:SECONDARY_W_BOX_RIGHT_L)
#define SECONDARY_W_BOX_BOT     (SM_HIRES?SECONDARY_W_BOX_BOT_H:SECONDARY_W_BOX_BOT_L)

#define SB_PRIMARY_W_BOX_LEFT_L		34		//50
#define SB_PRIMARY_W_BOX_TOP_L		153
#define SB_PRIMARY_W_BOX_RIGHT_L		(SB_PRIMARY_W_BOX_LEFT_L+53+2)
#define SB_PRIMARY_W_BOX_BOT_L		(195+1)
											
#define SB_PRIMARY_W_BOX_LEFT_H		68
#define SB_PRIMARY_W_BOX_TOP_H		381
#define SB_PRIMARY_W_BOX_RIGHT_H		179
#define SB_PRIMARY_W_BOX_BOT_H		473
											
#define SB_PRIMARY_W_BOX_LEFT   (SM_HIRES?SB_PRIMARY_W_BOX_LEFT_H:SB_PRIMARY_W_BOX_LEFT_L)
#define SB_PRIMARY_W_BOX_TOP    (SM_HIRES?SB_PRIMARY_W_BOX_TOP_H:SB_PRIMARY_W_BOX_TOP_L)
#define SB_PRIMARY_W_BOX_RIGHT  (SM_HIRES?SB_PRIMARY_W_BOX_RIGHT_H:SB_PRIMARY_W_BOX_RIGHT_L)
#define SB_PRIMARY_W_BOX_BOT    (SM_HIRES?SB_PRIMARY_W_BOX_BOT_H:SB_PRIMARY_W_BOX_BOT_L)
											
#define SB_SECONDARY_W_BOX_LEFT_L	169
#define SB_SECONDARY_W_BOX_TOP_L		153
#define SB_SECONDARY_W_BOX_RIGHT_L	(SB_SECONDARY_W_BOX_LEFT_L+54+1)
#define SB_SECONDARY_W_BOX_BOT_L		(153+43)

#define SB_SECONDARY_W_BOX_LEFT_H	338
#define SB_SECONDARY_W_BOX_TOP_H		381
#define SB_SECONDARY_W_BOX_RIGHT_H	449
#define SB_SECONDARY_W_BOX_BOT_H		473

#define SB_SECONDARY_W_BOX_LEFT (SM_HIRES?SB_SECONDARY_W_BOX_LEFT_H:SB_SECONDARY_W_BOX_LEFT_L) //210
#define SB_SECONDARY_W_BOX_TOP  (SM_HIRES?SB_SECONDARY_W_BOX_TOP_H:SB_SECONDARY_W_BOX_TOP_L)
#define SB_SECONDARY_W_BOX_RIGHT (SM_HIRES?SB_SECONDARY_W_BOX_RIGHT_H:SB_SECONDARY_W_BOX_RIGHT_L)
#define SB_SECONDARY_W_BOX_BOT  (SM_HIRES?SB_SECONDARY_W_BOX_BOT_H:SB_SECONDARY_W_BOX_BOT_L)

#define SB_PRIMARY_W_PIC_X			(SB_PRIMARY_W_BOX_LEFT+1)	//51
#define SB_PRIMARY_W_PIC_Y      (SM_HIRES?382:154)
#define SB_PRIMARY_W_TEXT_X     (SB_PRIMARY_W_BOX_LEFT+(SM_HIRES?50:24)) //(51+23)
#define SB_PRIMARY_W_TEXT_Y     (SM_HIRES?390:157)
#define SB_PRIMARY_AMMO_X       (SB_PRIMARY_W_BOX_LEFT+(SM_HIRES?(38+20):30))	//((SB_PRIMARY_W_BOX_LEFT+33)-3) //(51+32)
#define SB_PRIMARY_AMMO_Y       (SM_HIRES?410:171)

#define SB_SECONDARY_W_PIC_X    (SM_HIRES?385:(SB_SECONDARY_W_BOX_LEFT+29))	//(212+27)
#define SB_SECONDARY_W_PIC_Y    (SM_HIRES?382:154)
#define SB_SECONDARY_W_TEXT_X		(SB_SECONDARY_W_BOX_LEFT+2)	//212
#define SB_SECONDARY_W_TEXT_Y   (SM_HIRES?389:157)
#define SB_SECONDARY_AMMO_X     (SB_SECONDARY_W_BOX_LEFT+(SM_HIRES?(14-4):11))	//(212+9)
#define SB_SECONDARY_AMMO_Y     (SM_HIRES?414:171)

typedef struct gauge_box {
	int left,top;
	int right,bot;		//maximal box
	span *spanlist;	//list of left,right spans for copy
} gauge_box;

gauge_box gauge_boxes[] = {

// primary left/right low res
		{PRIMARY_W_BOX_LEFT_L,PRIMARY_W_BOX_TOP_L,PRIMARY_W_BOX_RIGHT_L,PRIMARY_W_BOX_BOT_L,weapon_window_left},
		{SECONDARY_W_BOX_LEFT_L,SECONDARY_W_BOX_TOP_L,SECONDARY_W_BOX_RIGHT_L,SECONDARY_W_BOX_BOT_L,weapon_window_right},

//sb left/right low res
		{SB_PRIMARY_W_BOX_LEFT_L,SB_PRIMARY_W_BOX_TOP_L,SB_PRIMARY_W_BOX_RIGHT_L,SB_PRIMARY_W_BOX_BOT_L,NULL},
		{SB_SECONDARY_W_BOX_LEFT_L,SB_SECONDARY_W_BOX_TOP_L,SB_SECONDARY_W_BOX_RIGHT_L,SB_SECONDARY_W_BOX_BOT_L,NULL},

// primary left/right hires
		{PRIMARY_W_BOX_LEFT_H,PRIMARY_W_BOX_TOP_H,PRIMARY_W_BOX_RIGHT_H,PRIMARY_W_BOX_BOT_H,weapon_window_left_hires},
		{SECONDARY_W_BOX_LEFT_H,SECONDARY_W_BOX_TOP_H,SECONDARY_W_BOX_RIGHT_H,SECONDARY_W_BOX_BOT_H,weapon_window_right_hires},

// sb left/right hires
		{SB_PRIMARY_W_BOX_LEFT_H,SB_PRIMARY_W_BOX_TOP_H,SB_PRIMARY_W_BOX_RIGHT_H,SB_PRIMARY_W_BOX_BOT_H,NULL},
		{SB_SECONDARY_W_BOX_LEFT_H,SB_SECONDARY_W_BOX_TOP_H,SB_SECONDARY_W_BOX_RIGHT_H,SB_SECONDARY_W_BOX_BOT_H,NULL},
	};

// these macros refer to arrays above

#define COCKPIT_PRIMARY_BOX     (SM_HIRES?4:0)
#define COCKPIT_SECONDARY_BOX   (SM_HIRES?5:1)
#define SB_PRIMARY_BOX          (SM_HIRES?6:2)
#define SB_SECONDARY_BOX        (SM_HIRES?7:3)

// center status bar and move to bottom of screen
#define SBX(x)  (((SWIDTH - (SM_HIRES?640:320)) / 2) + (x))
#define SBY(y)  (SHEIGHT - (SM_HIRES?480:240) + (y))

int	Color_0_31_0 = -1;

//copy a box from the off-screen buffer to the visible page
void copy_gauge_box(gauge_box *box,grs_bitmap *bm)
{
	if (box->spanlist) {
		int n_spans = box->bot-box->top+1;
		int cnt,y;

//gr_setcolor(BM_XRGB(31,0,0));

		for (cnt=0,y=box->top;cnt<n_spans;cnt++,y++) {
			gr_bm_ubitblt(box->spanlist[cnt].r-box->spanlist[cnt].l+1,1,
						box->left+box->spanlist[cnt].l,y,box->left+box->spanlist[cnt].l,y,bm,&grd_curcanv->cv_bitmap);

			//gr_scanline(box->left+box->spanlist[cnt].l,box->left+box->spanlist[cnt].r,y);
	 	}
 	}
	else
	 {
		Assert(Cockpit_mode.intval == CM_STATUS_BAR);
		gr_bm_ubitblt(box->right-box->left+1,box->bot-box->top+1,
		              SBX(box->left), SBY(box->top), SBX(box->left), SBY(box->top),
						bm,&grd_curcanv->cv_bitmap);
	 }
}

//fills in the coords of the hostage video window
void get_hostage_window_coords(int *x,int *y,int *w,int *h)
{
	if (Cockpit_mode.intval == CM_STATUS_BAR) {
		*x = SB_SECONDARY_W_BOX_LEFT;
		*y = SB_SECONDARY_W_BOX_TOP;
		*w = SB_SECONDARY_W_BOX_RIGHT - SB_SECONDARY_W_BOX_LEFT + 1;
		*h = SB_SECONDARY_W_BOX_BOT - SB_SECONDARY_W_BOX_TOP + 1;
	}
	else {
		*x = SECONDARY_W_BOX_LEFT;
		*y = SECONDARY_W_BOX_TOP;
		*w = SECONDARY_W_BOX_RIGHT - SECONDARY_W_BOX_LEFT + 1;
		*h = SECONDARY_W_BOX_BOT - SECONDARY_W_BOX_TOP + 1;
	}

}

//these should be in gr.h 
#define cv_w  cv_bitmap.bm_w
#define cv_h  cv_bitmap.bm_h

extern int HUD_nmessages, hud_first; // From hud.c
extern char HUD_messages[HUD_MAX_NUM][HUD_MESSAGE_LENGTH+5]; 
extern fix ThisLevelTime;
extern fix Omega_charge;

void hud_show_score()
{
	char	score_str[20];
	int	w, h, aw;

	if ((HUD_nmessages > 0) && (strlen(HUD_messages[hud_first]) > 38))
		return;

	gr_set_curfont( GAME_FONT );

	if ( ((Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP)) ) {
		sprintf(score_str, "%s: %5d", TXT_KILLS, Players[Player_num].net_kills_total);
	} else {
		sprintf(score_str, "%s: %5d", TXT_SCORE, Players[Player_num].score);
  	}

	gr_get_string_size(score_str, &w, &h, &aw );

	if (Color_0_31_0 == -1)
		Color_0_31_0 = gr_getcolor(0,31,0);
	gr_set_fontcolor(Color_0_31_0, -1);

	gr_printf(grd_curcanv->cv_w-w-LHX(2), 3, score_str);
 }

void hud_show_timer_count()
 {
#ifdef NETWORK
	char	score_str[20];
	int	w, h, aw,i;
	fix timevar=0;
#endif

	if ((HUD_nmessages > 0) && (strlen(HUD_messages[hud_first]) > 38))
		return;

#ifdef NETWORK
   if ((Game_mode & GM_NETWORK) && Netgame.PlayTimeAllowed)
    {
     timevar=i2f (Netgame.PlayTimeAllowed*5*60);
     i=f2i(timevar-ThisLevelTime);
     i++;
    
     sprintf(score_str, "T - %5d", i);
																								 
     gr_get_string_size(score_str, &w, &h, &aw );

     if (Color_0_31_0 == -1)
		Color_0_31_0 = gr_getcolor(0,31,0);
     gr_set_fontcolor(Color_0_31_0, -1);

     if (i>-1 && !Control_center_destroyed)
	     gr_printf(grd_curcanv->cv_w-w-LHX(10), LHX(11), score_str);
    }
#endif
 }


//y offset between lines on HUD
int Line_spacing;

void hud_show_score_added()
{
	int	color;
	int	w, h, aw;
	char	score_str[20];

	if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
		return;

	if (score_display[0] == 0)
		return;

	gr_set_curfont( GAME_FONT );

	score_time -= FrameTime;
	if (score_time > 0) {
		color = f2i(score_time * 20) + 12;

		if (color < 10) color = 12;
		if (color > 31) color = 30;

		color = color - (color % 4);	//	Only allowing colors 12,16,20,24,28 speeds up gr_getcolor, improves caching

		if (Cheats_enabled.intval)
			sprintf(score_str, "%s", TXT_CHEATER);
		else
			sprintf(score_str, "%5d", score_display[0]);

		gr_get_string_size(score_str, &w, &h, &aw );
		gr_set_fontcolor(gr_getcolor(0, color, 0),-1 );
		gr_printf(grd_curcanv->cv_w-w-LHX(2+10), Line_spacing+4, score_str);
	} else {
		score_time = 0;
		score_display[0] = 0;
	}
	
}

void sb_show_score()
{	                                                                                                                                                                                                                                                             
	char	score_str[20];
	int x,y;
	int	w, h, aw;
	static int last_x[4]={SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_H,SB_SCORE_RIGHT_H};
	int redraw_score;

	if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
		redraw_score = -99;
	else
		redraw_score = -1;

	//if (old_score[VR_current_page] == redraw_score)
	{
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor(gr_getcolor(0,20,0),-1 );

		if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
			gr_printf(SBX(SB_SCORE_LABEL_X), SBY(SB_SCORE_Y), "%s:", TXT_KILLS);
		else
			gr_printf(SBX(SB_SCORE_LABEL_X), SBY(SB_SCORE_Y), "%s:", TXT_SCORE);
	}

	gr_set_curfont( GAME_FONT );
	if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
		sprintf(score_str, "%5d", Players[Player_num].net_kills_total);
	else
		sprintf(score_str, "%5d", Players[Player_num].score);
	gr_get_string_size(score_str, &w, &h, &aw );

	x = SB_SCORE_RIGHT-w-LHX(2);
	y = SB_SCORE_Y;
	
	//erase old score
	gr_setcolor(BM_XRGB(0,0,0));
	gr_rect(SBX(last_x[(SM_HIRES?2:0) + VR_current_page]), SBY(y), SBX(SB_SCORE_RIGHT), SBY(y + GAME_FONT->ft_h));

	if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
		gr_set_fontcolor(gr_getcolor(0,20,0),-1 );
	else
		gr_set_fontcolor(gr_getcolor(0,31,0),-1 );	

	gr_printf(SBX(x), SBY(y), score_str);

	last_x[(SM_HIRES?2:0) + VR_current_page] = x;
}

void sb_show_score_added()
{
	int	color;
	int w, h, aw;
	char	score_str[32];
	int x;
	static int last_x[4]={SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_H,SB_SCORE_RIGHT_H};
	static	int last_score_display[2] = { -1, -1};
	
	if ( (Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP) ) 
		return;

	if (score_display[VR_current_page] == 0)
		return;

	gr_set_curfont( GAME_FONT );

	score_time -= FrameTime;
	if (score_time > 0) {
		if (score_display[VR_current_page] != last_score_display[VR_current_page])
		{
			gr_setcolor(BM_XRGB(0,0,0));
			gr_rect(SBX(last_x[(SM_HIRES?2:0) + VR_current_page]), SBY(SB_SCORE_ADDED_Y),
			        SBX(SB_SCORE_ADDED_RIGHT), SBY(SB_SCORE_ADDED_Y + GAME_FONT->ft_h));

			last_score_display[VR_current_page] = score_display[VR_current_page];
		}

		color = f2i(score_time * 20) + 10;

		if (color < 10) color = 10;
		if (color > 31) color = 31;

		if (Cheats_enabled.intval)
			sprintf(score_str, "%s", TXT_CHEATER);
		else
			sprintf(score_str, "%5d", score_display[VR_current_page]);

		gr_get_string_size(score_str, &w, &h, &aw );

		x = SB_SCORE_ADDED_RIGHT-w-LHX(2);

		gr_set_fontcolor(gr_getcolor(0, color, 0),-1 );

		gr_printf(SBX(x), SBY(SB_SCORE_ADDED_Y), score_str);

		last_x[(SM_HIRES?2:0) + VR_current_page] = x;

	} else {
		//erase old score
		gr_setcolor(BM_XRGB(0,0,0));
		gr_rect(SBX(last_x[(SM_HIRES?2:0) + VR_current_page]), SBY(SB_SCORE_ADDED_Y),
		        SBX(SB_SCORE_ADDED_RIGHT), SBY(SB_SCORE_ADDED_Y + GAME_FONT->ft_h));

		score_time = 0;
		score_display[VR_current_page] = 0;

	}
}

fix	Last_warning_beep_time[2] = {0,0};		//	Time we last played homing missile warning beep.

//	-----------------------------------------------------------------------------
void play_homing_warning(void)
{
	fix	beep_delay;

	if (Endlevel_sequence || Player_is_dead)
		return;

	if (Players[Player_num].homing_object_dist >= 0) {
		beep_delay = Players[Player_num].homing_object_dist/128;
		if (beep_delay > F1_0)
			beep_delay = F1_0;
		else if (beep_delay < F1_0/8)
			beep_delay = F1_0/8;

		if (Last_warning_beep_time[VR_current_page] > GameTime)
			Last_warning_beep_time[VR_current_page] = 0;

		if (GameTime - Last_warning_beep_time[VR_current_page] > beep_delay/2) {
			digi_play_sample( SOUND_HOMING_WARNING, F1_0 );
			Last_warning_beep_time[VR_current_page] = GameTime;
		}
	}
}

int	Last_homing_warning_shown[2]={-1,-1};

//	-----------------------------------------------------------------------------
void show_homing_warning(void)
{
	if ((Cockpit_mode.intval == CM_STATUS_BAR) || (Endlevel_sequence)) {
		if (Last_homing_warning_shown[VR_current_page] == 1) {
			PAGE_IN_GAUGE( GAUGE_HOMING_WARNING_OFF );

			gr_ubitmapm( HOMING_WARNING_X, HOMING_WARNING_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_HOMING_WARNING_OFF) ] );

			Last_homing_warning_shown[VR_current_page] = 0;
		}
		return;
	}

	gr_set_current_canvas( get_current_game_screen() );


	if (Players[Player_num].homing_object_dist >= 0) {

		if (GameTime & 0x4000) {
			if (Last_homing_warning_shown[VR_current_page] != 1) {
				PAGE_IN_GAUGE( GAUGE_HOMING_WARNING_ON );
				gr_ubitmapm( HOMING_WARNING_X, HOMING_WARNING_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_HOMING_WARNING_ON) ] );
				Last_homing_warning_shown[VR_current_page] = 1;
			}
		} else {
			if (Last_homing_warning_shown[VR_current_page] != 0) {
				PAGE_IN_GAUGE( GAUGE_HOMING_WARNING_OFF );
				gr_ubitmapm( HOMING_WARNING_X, HOMING_WARNING_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_HOMING_WARNING_OFF) ] );
				Last_homing_warning_shown[VR_current_page] = 0;
			}
		}
	} else if (Last_homing_warning_shown[VR_current_page] != 0) {
		PAGE_IN_GAUGE( GAUGE_HOMING_WARNING_OFF );
		gr_ubitmapm( HOMING_WARNING_X, HOMING_WARNING_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_HOMING_WARNING_OFF) ] );
		Last_homing_warning_shown[VR_current_page] = 0;
	}
}

#define MAX_SHOWN_LIVES 4

extern int Game_window_y;
extern int SW_y[2];

void hud_show_homing_warning(void)
{
	if (Players[Player_num].homing_object_dist >= 0) {
		if (GameTime & 0x4000) {
			int x=0x8000, y=grd_curcanv->cv_h-Line_spacing;

			if (weapon_box_user[0] != WBU_WEAPON || weapon_box_user[1] != WBU_WEAPON) {
				int wy = (weapon_box_user[0] != WBU_WEAPON)?SW_y[0]:SW_y[1];
				y = min(y,(wy - Line_spacing - Game_window_y));
			}

			gr_set_curfont( GAME_FONT );
			gr_set_fontcolor(gr_getcolor(0,31,0),-1 );
			gr_printf(x,y,TXT_LOCK);
		}
	}
}

void hud_show_keys(void)
{
	int y = 3*Line_spacing;
	int dx = GAME_FONT->ft_w+GAME_FONT->ft_w/2;

	if (Players[Player_num].flags & PLAYER_FLAGS_BLUE_KEY) {
		PAGE_IN_GAUGE( KEY_ICON_BLUE );
		gr_ubitmapm(2,y,&GameBitmaps[ GET_GAUGE_INDEX(KEY_ICON_BLUE) ] );

	}

	if (Players[Player_num].flags & PLAYER_FLAGS_GOLD_KEY) {
		PAGE_IN_GAUGE( KEY_ICON_YELLOW );
		gr_ubitmapm(2+dx,y,&GameBitmaps[ GET_GAUGE_INDEX(KEY_ICON_YELLOW) ] );
	}

	if (Players[Player_num].flags & PLAYER_FLAGS_RED_KEY) {
		PAGE_IN_GAUGE( KEY_ICON_RED );
		gr_ubitmapm(2+2*dx,y,&GameBitmaps[ GET_GAUGE_INDEX(KEY_ICON_RED) ] );
	}

}

#ifdef NETWORK
extern grs_bitmap Orb_icons[2];

void hud_show_orbs (void)
{
	if (Game_mode & GM_HOARD) {
		int x,y;
		grs_bitmap *bm;

		x=y=0;

		if (Cockpit_mode.intval == CM_FULL_COCKPIT) {
			y = 2*Line_spacing;
			x = 4*GAME_FONT->ft_w;
		}
		else if (Cockpit_mode.intval == CM_STATUS_BAR) {
			y = Line_spacing;
			x = GAME_FONT->ft_w;
		}
		else if (Cockpit_mode.intval == CM_FULL_SCREEN) {
			y = 5*Line_spacing;
			x = GAME_FONT->ft_w;
			if (FontHires)
				y += Line_spacing;
		}
		else
			Int3();		//what sort of cockpit?

		bm = &Orb_icons[FontHires];
		gr_ubitmapm(x,y,bm);

		gr_set_fontcolor(gr_getcolor(0,31,0),-1 );
		gr_printf(x+bm->bm_w+bm->bm_w/2, y+(FontHires?2:1), "x %d", Players[Player_num].secondary_ammo[PROXIMITY_INDEX]);

	}
}

void hud_show_flag(void)
{
	if ((Game_mode & GM_CAPTURE) && (Players[Player_num].flags & PLAYER_FLAGS_FLAG)) {
		int x,y,icon;

		x=y=0;

		if (Cockpit_mode.intval == CM_FULL_COCKPIT) {
			y = 2*Line_spacing;
			x = 4*GAME_FONT->ft_w;
		}
		else if (Cockpit_mode.intval == CM_STATUS_BAR) {
			y = Line_spacing;
			x = GAME_FONT->ft_w;
		}
		else if (Cockpit_mode.intval == CM_FULL_SCREEN) {
			y = 5*Line_spacing;
			x = GAME_FONT->ft_w;
			if (FontHires)
				y += Line_spacing;
		}
		else
			Int3();		//what sort of cockpit?


		icon = (get_team(Player_num) == TEAM_BLUE)?FLAG_ICON_RED:FLAG_ICON_BLUE;

		PAGE_IN_GAUGE( icon );
		gr_ubitmapm(x,y,&GameBitmaps[ GET_GAUGE_INDEX(icon) ] );

	}
}
#endif

void hud_show_energy(void)
{
	//gr_set_current_canvas(&VR_render_sub_buffer[0]);	//render off-screen
	gr_set_curfont( GAME_FONT );
	gr_set_fontcolor(gr_getcolor(0,31,0),-1 );
	if (Game_mode & GM_MULTI)
		gr_printf(2, grd_curcanv->cv_h-5*Line_spacing,"%s: %i", TXT_ENERGY, f2ir(Players[Player_num].energy));
	else
		gr_printf(2, grd_curcanv->cv_h-Line_spacing,"%s: %i", TXT_ENERGY, f2ir(Players[Player_num].energy));

	if (Newdemo_state==ND_STATE_RECORDING ) {
		int energy = f2ir(Players[Player_num].energy);

		if (energy != old_energy[VR_current_page]) {
			newdemo_record_player_energy(old_energy[VR_current_page], energy);
			old_energy[VR_current_page] = energy;
	 	}
	}
}

void hud_show_afterburner(void)
{
	int y;

	if (! (Players[Player_num].flags & PLAYER_FLAGS_AFTERBURNER))
		return;		//don't draw if don't have

	gr_set_curfont( GAME_FONT );
	gr_set_fontcolor(gr_getcolor(0,31,0),-1 );

	y = (Game_mode & GM_MULTI)?(-8*Line_spacing):(-3*Line_spacing);

	gr_printf(2, grd_curcanv->cv_h+y, "burn: %d%%" , fixmul(Afterburner_charge,100));

	if (Newdemo_state==ND_STATE_RECORDING ) {

		if (Afterburner_charge != old_afterburner[VR_current_page]) {
			newdemo_record_player_afterburner(old_afterburner[VR_current_page], Afterburner_charge);
			old_afterburner[VR_current_page] = Afterburner_charge;
	 	}
	}
}

char *d2_very_short_secondary_weapon_names[] =
		{"Flash","Guided","SmrtMine","Mercury","Shaker"};

#define SECONDARY_WEAPON_NAMES_VERY_SHORT(weapon_num) 					\
	((weapon_num <= MEGA_INDEX)?(*(&TXT_CONCUSSION + (weapon_num))):	\
	d2_very_short_secondary_weapon_names[weapon_num-SMISSILE1_INDEX])

//return which bomb will be dropped next time the bomb key is pressed
extern int which_bomb(void);

void show_bomb_count(int x,int y,int bg_color,int always_show)
{
	int bomb,count,countx;
	char txt[5],*t;

	bomb = which_bomb();
	count = Players[Player_num].secondary_ammo[bomb];

	#ifndef RELEASE
	count = min(count,99);	//only have room for 2 digits - cheating give 200
	#endif

	countx = (bomb==PROXIMITY_INDEX)?count:-count;

	if (always_show && count == 0)		//no bombs, draw nothing on HUD
		return;

	//if (!always_show && countx == old_bombcount[VR_current_page])
	//	return;

	if (Cockpit_mode.intval == CM_STATUS_BAR) { //draw background
		gr_setcolor(bg_color);
		if (!SM_HIRES) {
			gr_rect(x-2, y-2, x+18, y+5);
			gr_setcolor(gr_find_closest_color(10,10,10));
			gr_scanline(x-3, x+18, y-2);
		} else {
			gr_rect(x-4, y-5, x+36, y+12);

			gr_setcolor(gr_find_closest_color(10,10,10));
			gr_scanline(x-6, x+36, y-5);
		}
	}

	if (count)
		gr_set_fontcolor((bomb==PROXIMITY_INDEX)?gr_find_closest_color(55,0,0):gr_getcolor(59,50,21),bg_color);
	else
		gr_set_fontcolor(bg_color,bg_color);	//erase by drawing in background color

	snprintf(txt, sizeof(txt)-1, "B:%02d", count);

	while ((t=strchr(txt,'1')) != NULL)
		*t = '\x84';	//convert to wide '1'

	gr_string(x,y,txt);

	old_bombcount[VR_current_page] = countx;
}

void draw_primary_ammo_info(int ammo_count)
{
	if (Cockpit_mode.intval == CM_STATUS_BAR)
		draw_ammo_info(SBX(SB_PRIMARY_AMMO_X), SBY(SB_PRIMARY_AMMO_Y), ammo_count, 1);
	else
		draw_ammo_info(PRIMARY_AMMO_X,PRIMARY_AMMO_Y,ammo_count,1);
}

//convert '1' characters to special wide ones
#define convert_1s(s) do {char *p=s; while ((p=strchr(p,'1')) != NULL) *p=(char)132;} while(0)

void hud_show_weapons(void)
{
	int	w, h, aw;
	int	y;
	char	*weapon_name;
	char	weapon_str[32];

//	gr_set_current_canvas(&VR_render_sub_buffer[0]);	//render off-screen
	gr_set_curfont( GAME_FONT );
	gr_set_fontcolor(gr_getcolor(0,31,0),-1 );

	y = grd_curcanv->cv_h;

	if (Game_mode & GM_MULTI)
		y -= 4*Line_spacing;

	weapon_name = PRIMARY_WEAPON_NAMES_SHORT(Primary_weapon);

	switch (Primary_weapon) {
		case LASER_INDEX:
			if (Players[Player_num].flags & PLAYER_FLAGS_QUAD_LASERS)
				sprintf(weapon_str, "%s %s %i", TXT_QUAD, weapon_name, Players[Player_num].laser_level+1);
			else
				sprintf(weapon_str, "%s %i", weapon_name, Players[Player_num].laser_level+1);
			break;

		case SUPER_LASER_INDEX:	Int3(); break;	//no such thing as super laser

		case VULCAN_INDEX:		
		case GAUSS_INDEX:			
			sprintf(weapon_str, "%s: %i", weapon_name, f2i((unsigned) Players[Player_num].primary_ammo[VULCAN_INDEX] * (unsigned) VULCAN_AMMO_SCALE)); 
			convert_1s(weapon_str);
			break;

		case SPREADFIRE_INDEX:
		case PLASMA_INDEX:
		case FUSION_INDEX:
		case HELIX_INDEX:
		case PHOENIX_INDEX:
			strcpy(weapon_str, weapon_name);
			break;
		case OMEGA_INDEX:
			sprintf(weapon_str, "%s: %03i", weapon_name, Omega_charge * 100/MAX_OMEGA_CHARGE);
			convert_1s(weapon_str);
			break;

		default:						Int3();	weapon_str[0] = 0;	break;
	}

	gr_get_string_size(weapon_str, &w, &h, &aw );
	gr_printf(grd_curcanv->cv_bitmap.bm_w-5-w, y-2*Line_spacing, weapon_str);

	if (Primary_weapon == VULCAN_INDEX) {
		if (Players[Player_num].primary_ammo[Primary_weapon] != old_ammo_count[0][VR_current_page]) {
			if (Newdemo_state == ND_STATE_RECORDING)
				newdemo_record_primary_ammo(old_ammo_count[0][VR_current_page], Players[Player_num].primary_ammo[Primary_weapon]);
			old_ammo_count[0][VR_current_page] = Players[Player_num].primary_ammo[Primary_weapon];
		}
	}

	if (Primary_weapon == OMEGA_INDEX) {
		if (Omega_charge != Old_Omega_charge[VR_current_page]) {
			if (Newdemo_state == ND_STATE_RECORDING)
				newdemo_record_primary_ammo(Old_Omega_charge[VR_current_page], Omega_charge);
			Old_Omega_charge[VR_current_page] = Omega_charge;
		}
	}

	weapon_name = SECONDARY_WEAPON_NAMES_VERY_SHORT(Secondary_weapon);

	sprintf(weapon_str, "%s %d",weapon_name,Players[Player_num].secondary_ammo[Secondary_weapon]);
	gr_get_string_size(weapon_str, &w, &h, &aw );
	gr_printf(grd_curcanv->cv_bitmap.bm_w-5-w, y-Line_spacing, weapon_str);

	if (Players[Player_num].secondary_ammo[Secondary_weapon] != old_ammo_count[1][VR_current_page]) {
		if (Newdemo_state == ND_STATE_RECORDING)
			newdemo_record_secondary_ammo(old_ammo_count[1][VR_current_page], Players[Player_num].secondary_ammo[Secondary_weapon]);
		old_ammo_count[1][VR_current_page] = Players[Player_num].secondary_ammo[Secondary_weapon];
	}

	show_bomb_count(grd_curcanv->cv_bitmap.bm_w-(3*GAME_FONT->ft_w+(FontHires?0:2)), y-3*Line_spacing,-1,1);
}

void hud_show_cloak_invuln(void)
{
	gr_set_fontcolor(gr_getcolor(0,31,0),-1 );

	if (Players[Player_num].flags & PLAYER_FLAGS_CLOAKED) {
		int	y = grd_curcanv->cv_h;

		if (Game_mode & GM_MULTI)
			y -= 7*Line_spacing;
		else
			y -= 4*Line_spacing;

		if ((Players[Player_num].cloak_time+CLOAK_TIME_MAX - GameTime > F1_0*3 ) || (GameTime & 0x8000))
			gr_printf(2, y, "%s", TXT_CLOAKED);
	}

	if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
		int	y = grd_curcanv->cv_h;

		if (Game_mode & GM_MULTI)
			y -= 10*Line_spacing;
		else
			y -= 5*Line_spacing;

		if (((Players[Player_num].invulnerable_time + INVULNERABLE_TIME_MAX - GameTime) > F1_0*4) || (GameTime & 0x8000))
			gr_printf(2, y, "%s", TXT_INVULNERABLE);
	}

}

void hud_show_shield(void)
{
//	gr_set_current_canvas(&VR_render_sub_buffer[0]);	//render off-screen
	gr_set_curfont( GAME_FONT );
	gr_set_fontcolor(gr_getcolor(0,31,0),-1 );

	if ( Players[Player_num].shields >= 0 )	{
		if (Game_mode & GM_MULTI)
			gr_printf(2, grd_curcanv->cv_h-6*Line_spacing,"%s: %i", TXT_SHIELD, f2ir(Players[Player_num].shields));
		else
			gr_printf(2, grd_curcanv->cv_h-2*Line_spacing,"%s: %i", TXT_SHIELD, f2ir(Players[Player_num].shields));
	} else {
		if (Game_mode & GM_MULTI)
			gr_printf(2, grd_curcanv->cv_h-6*Line_spacing,"%s: 0", TXT_SHIELD );
		else
			gr_printf(2, grd_curcanv->cv_h-2*Line_spacing,"%s: 0", TXT_SHIELD );
	}

	if (Newdemo_state==ND_STATE_RECORDING ) {
		int shields = f2ir(Players[Player_num].shields);

		if (shields != old_shields[VR_current_page]) {		// Draw the shield gauge
			newdemo_record_player_shields(old_shields[VR_current_page], shields);
			old_shields[VR_current_page] = shields;
		}
	}
}

//draw the icons for number of lives
void hud_show_lives()
{
	if ((HUD_nmessages > 0) && (strlen(HUD_messages[hud_first]) > 38))
		return;

	if (Game_mode & GM_MULTI) {
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor(gr_getcolor(0,31,0),-1 );
		gr_printf(10, 3, "%s: %d", TXT_DEATHS, Players[Player_num].net_killed_total);
	} 
	else if (Players[Player_num].lives > 1)  {
		grs_bitmap *bm;
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor(gr_getcolor(0,20,0),-1 );
		PAGE_IN_GAUGE( GAUGE_LIVES );
		bm = &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_LIVES) ];
		gr_ubitmapm(10,3,bm);
		gr_printf(10+bm->bm_w+bm->bm_w/2, 4, "x %d", Players[Player_num].lives-1);
	}

}

void sb_show_lives()
{
	int x,y;
	grs_bitmap * bm = &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_LIVES) ];
	x = SB_LIVES_X;
	y = SB_LIVES_Y;
  
	//if (old_lives[VR_current_page] == -1)
	{
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor(gr_getcolor(0,20,0),-1 );
		if (Game_mode & GM_MULTI)
			gr_printf(SBX(SB_LIVES_LABEL_X), SBY(SB_LIVES_LABEL_Y), "%s:", TXT_DEATHS);
		else
			gr_printf(SBX(SB_LIVES_LABEL_X), SBY(SB_LIVES_LABEL_Y), "%s:", TXT_LIVES);

	}

	if (Game_mode & GM_MULTI)
	{
		char killed_str[20];
		int w, h, aw;
		static int last_x[4] = {SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_L,SB_SCORE_RIGHT_H,SB_SCORE_RIGHT_H};
		int x;

		sprintf(killed_str, "%5d", Players[Player_num].net_killed_total);
		gr_get_string_size(killed_str, &w, &h, &aw);
		gr_setcolor(BM_XRGB(0,0,0));
		gr_rect(SBX(last_x[(SM_HIRES?2:0) + VR_current_page]), SBY(y + 1), SBX(SB_SCORE_RIGHT), SBY(y + GAME_FONT->ft_h));
		gr_set_fontcolor(gr_getcolor(0,20,0),-1);
		x = SB_SCORE_RIGHT-w-2;		
		gr_printf(SBX(x), SBY(y+1), killed_str);
		last_x[(SM_HIRES?2:0) + VR_current_page] = x;
		return;
	}

	//if (old_lives[VR_current_page]==-1 || Players[Player_num].lives != old_lives[VR_current_page])
	{

		//erase old icons

		gr_setcolor(BM_XRGB(0,0,0));
      
		gr_rect(SBX(x), SBY(y), SBX(SB_SCORE_RIGHT), SBY(y+bm->bm_h));

		if (Players[Player_num].lives-1 > 0) {
			gr_set_curfont( GAME_FONT );
			gr_set_fontcolor(gr_getcolor(0,20,0),-1 );
			PAGE_IN_GAUGE( GAUGE_LIVES );

			gr_ubitmapm(SBX(x), SBY(y), bm);
			gr_printf(SBX(x+bm->bm_w+GAME_FONT->ft_w), SBY(y), "x %d", Players[Player_num].lives-1);

//			gr_printf(SBX(x+12), SBY(y), "x %d", Players[Player_num].lives-1);
		}
	}

//	for (i=0;i<draw_count;i++,x+=bm->bm_w+2)
//		gr_ubitmapm(SBX(x), SBY(y), bm);

}

#ifndef RELEASE

#ifdef PIGGY_USE_PAGING
extern int Piggy_bitmap_cache_next;
#endif

void show_time()
{
	int secs = f2i(Players[Player_num].time_level) % 60;
	int mins = f2i(Players[Player_num].time_level) / 60;

	gr_set_curfont( GAME_FONT );

	if (Color_0_31_0 == -1)
		Color_0_31_0 = gr_getcolor(0,31,0);
	gr_set_fontcolor(Color_0_31_0, -1 );

	gr_printf(grd_curcanv->cv_w-4*GAME_FONT->ft_w,grd_curcanv->cv_h-4*Line_spacing,"%d:%02d", mins, secs);

//@@#ifdef PIGGY_USE_PAGING
//@@	{
//@@		char text[25];
//@@		int w,h,aw;
//@@		sprintf( text, "%d KB", Piggy_bitmap_cache_next/1024 );
//@@		gr_get_string_size( text, &w, &h, &aw );	
//@@		gr_printf(grd_curcanv->cv_w-10-w,grd_curcanv->cv_h/2, text );
//@@	}
//@@#endif

}
#endif

#define EXTRA_SHIP_SCORE	50000		//get new ship every this many points

void add_points_to_score(int points) 
{
	int prev_score;

	score_time += f1_0*2;
	score_display[0] += points;
	score_display[1] += points;
	if (score_time > f1_0*4) score_time = f1_0*4;

	if (points == 0 || Cheats_enabled.intval)
		return;

	if ((Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP))
		return;

	prev_score=Players[Player_num].score;

	Players[Player_num].score += points;

	if (Newdemo_state == ND_STATE_RECORDING)
		newdemo_record_player_score(points);

#ifdef NETWORK
	if (Game_mode & GM_MULTI_COOP)
		multi_send_score();

	if (Game_mode & GM_MULTI)
		return;
#endif

	if (Players[Player_num].score/EXTRA_SHIP_SCORE != prev_score/EXTRA_SHIP_SCORE) {
		int snd;
		Players[Player_num].lives += Players[Player_num].score/EXTRA_SHIP_SCORE - prev_score/EXTRA_SHIP_SCORE;
		powerup_basic(20, 20, 20, 0, TXT_EXTRA_LIFE);
		if ((snd=Powerup_info[POW_EXTRA_LIFE].hit_sound) > -1 )
			digi_play_sample( snd, F1_0 );
	}
}

void add_bonus_points_to_score(int points) 
{
	int prev_score;

	if (points == 0 || Cheats_enabled.intval)
		return;

	prev_score=Players[Player_num].score;

	Players[Player_num].score += points;


	if (Newdemo_state == ND_STATE_RECORDING)
		newdemo_record_player_score(points);

	if (Game_mode & GM_MULTI)
		return;

	if (Players[Player_num].score/EXTRA_SHIP_SCORE != prev_score/EXTRA_SHIP_SCORE) {
		int snd;
		Players[Player_num].lives += Players[Player_num].score/EXTRA_SHIP_SCORE - prev_score/EXTRA_SHIP_SCORE;
		if ((snd=Powerup_info[POW_EXTRA_LIFE].hit_sound) > -1 )
			digi_play_sample( snd, F1_0 );
	}
}

#include "key.h"

void init_gauge_canvases()
{
	grs_font *save_font = grd_curcanv->cv_font;

	gr_set_curfont(GAME_FONT);

	PAGE_IN_GAUGE( SB_GAUGE_ENERGY );
	PAGE_IN_GAUGE( GAUGE_AFTERBURNER );

	Canv_LeftEnergyGauge	= gr_create_sub_canvas( grd_curcanv, LEFT_ENERGY_GAUGE_X, LEFT_ENERGY_GAUGE_Y, LEFT_ENERGY_GAUGE_W, LEFT_ENERGY_GAUGE_H );
	Canv_SBEnergyGauge      = gr_create_sub_canvas( grd_curcanv,
	                                                SBX(SB_ENERGY_GAUGE_X),      SBY(SB_ENERGY_GAUGE_Y),
	                                                SB_ENERGY_GAUGE_W,           SB_ENERGY_GAUGE_H );
	Canv_SBAfterburnerGauge = gr_create_sub_canvas( grd_curcanv,
	                                                SBX(SB_AFTERBURNER_GAUGE_X), SBY(SB_AFTERBURNER_GAUGE_Y),
	                                                SB_AFTERBURNER_GAUGE_W,      SB_AFTERBURNER_GAUGE_H );
	Canv_RightEnergyGauge	= gr_create_sub_canvas( grd_curcanv, RIGHT_ENERGY_GAUGE_X, RIGHT_ENERGY_GAUGE_Y, RIGHT_ENERGY_GAUGE_W, RIGHT_ENERGY_GAUGE_H );
	Canv_NumericalGauge		= gr_create_sub_canvas( grd_curcanv, NUMERICAL_GAUGE_X, NUMERICAL_GAUGE_Y, NUMERICAL_GAUGE_W, NUMERICAL_GAUGE_H );
	Canv_AfterburnerGauge	= gr_create_sub_canvas( grd_curcanv, AFTERBURNER_GAUGE_X, AFTERBURNER_GAUGE_Y, AFTERBURNER_GAUGE_W, AFTERBURNER_GAUGE_H );

	gr_set_curfont(save_font);
}

void close_gauge_canvases()
{
	if (Canv_LeftEnergyGauge)
	{
		gr_free_sub_canvas( Canv_LeftEnergyGauge );
		gr_free_sub_canvas( Canv_SBEnergyGauge );
		gr_free_sub_canvas( Canv_SBAfterburnerGauge );
		gr_free_sub_canvas( Canv_RightEnergyGauge );
		gr_free_sub_canvas( Canv_NumericalGauge );
		gr_free_sub_canvas( Canv_AfterburnerGauge );
	}
}

void init_gauges()
{
	int i;

	//draw_gauges_on 	= 1;

	for (i=0; i<2; i++ )	{
		if ( ((Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP)) || ((Newdemo_state == ND_STATE_PLAYBACK) && (Newdemo_game_mode & GM_MULTI) && !(Newdemo_game_mode & GM_MULTI_COOP)) ) 
			old_score[i] = -99;
		else
			old_score[i]			= -1;
		old_energy[i]			= -1;
		old_shields[i]			= -1;
		old_flags[i]			= -1;
		old_cloak[i]			= -1;
		old_lives[i]			= -1;
		old_afterburner[i]	= -1;
		old_bombcount[i]		= 0;
		old_laser_level[i]	= 0;
	
		old_weapon[0][i] = old_weapon[1][i] = -1;
		old_ammo_count[0][i] = old_ammo_count[1][i] = -1;
		Old_Omega_charge[i] = -1;
	}

	cloak_fade_state = 0;

	weapon_box_user[0] = weapon_box_user[1] = WBU_WEAPON;
}

void draw_energy_bar(int energy)
{
	int not_energy;
	int x1, x2, y;

	// Draw left energy bar
	gr_set_current_canvas( Canv_LeftEnergyGauge );
	PAGE_IN_GAUGE( GAUGE_ENERGY_LEFT );
	gr_ubitmapm( 0, 0, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_ENERGY_LEFT)] );
	gr_setcolor( BM_XRGB(0,0,0) );

	if (!SM_HIRES)
		not_energy = 61 - (energy*61)/100;
	else
		not_energy = 125 - (energy*125)/100;

	if (energy < 100)
		for (y=0; y < LEFT_ENERGY_GAUGE_H; y++) {
			x1 = LEFT_ENERGY_GAUGE_H - 1 - y;
			x2 = LEFT_ENERGY_GAUGE_H - 1 - y + not_energy;

			if (x2 > LEFT_ENERGY_GAUGE_W - y/3)
				x2 = LEFT_ENERGY_GAUGE_W - y/3;
			
			if (x2 > x1) gr_uline( i2f(x1), i2f(y), i2f(x2), i2f(y) ); 
		}
	
	gr_set_current_canvas( get_current_game_screen() );

	// Draw right energy bar
	gr_set_current_canvas( Canv_RightEnergyGauge );
	PAGE_IN_GAUGE( GAUGE_ENERGY_RIGHT );
	gr_ubitmapm( 0, 0, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_ENERGY_RIGHT) ] );
	gr_setcolor( BM_XRGB(0,0,0) );

	if (energy < 100)
		for (y=0; y < RIGHT_ENERGY_GAUGE_H; y++) {
			x1 = RIGHT_ENERGY_GAUGE_W - RIGHT_ENERGY_GAUGE_H + 1 + y - not_energy;
			x2 = RIGHT_ENERGY_GAUGE_W - RIGHT_ENERGY_GAUGE_H + 1 + y;

			if (x1 < y/3)
				x1 = y/3;

			if (x2 > x1) gr_uline( i2f(x1), i2f(y), i2f(x2), i2f(y) );  
		}

	gr_set_current_canvas( get_current_game_screen() );
}

ubyte afterburner_bar_table[AFTERBURNER_GAUGE_H_L*2] = {
			3,11,
			3,11,
			3,11,
			3,11,
			3,11,
			3,11,
			2,11,
			2,10,
			2,10,
			2,10,
			2,10,
			2,10,
			2,10,
			1,10,
			1,10,
			1,10,
			1,9,
			1,9,
			1,9,
			1,9,
			0,9,
			0,9,
			0,8,
			0,8,
			0,8,
			0,8,
			1,8,
			2,8,
			3,8,
			4,8,
			5,8,
			6,7,
};

ubyte afterburner_bar_table_hires[AFTERBURNER_GAUGE_H_H*2] = {
	5,20,
	5,20,
	5,19,
	5,19,
	5,19,
	5,19,
	4,19,
	4,19,
	4,19,
	4,19,

	4,19,
	4,18,
	4,18,
	4,18,
	4,18,
	3,18,
	3,18,
	3,18,
	3,18,
	3,18,

	3,18,
	3,17,
	3,17,
	2,17,
	2,17,
	2,17,
	2,17,
	2,17,
	2,17,
	2,17,

	2,17,
	2,16,
	2,16,
	1,16,
	1,16,
	1,16,
	1,16,
	1,16,
	1,16,
	1,16,

	1,16,
	1,15,
	1,15,
	1,15,
	0,15,
	0,15,
	0,15,
	0,15,
	0,15,
	0,15,

	0,14,
	0,14,
	0,14,
	1,14,
	2,14,
	3,14,
	4,14,
	5,14,
	6,13,
	7,13,

	8,13,
	9,13,
	10,13,
	11,13,
	12,13
};


void draw_afterburner_bar(int afterburner)
{
	int not_afterburner;
	int y;

	// Draw afterburner bar
	gr_set_current_canvas( Canv_AfterburnerGauge );
	PAGE_IN_GAUGE( GAUGE_AFTERBURNER );
	gr_ubitmapm( 0, 0, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_AFTERBURNER) ] );
	gr_setcolor( BM_XRGB(0,0,0) );

	not_afterburner = fixmul(f1_0 - afterburner,AFTERBURNER_GAUGE_H);

	for (y=0;y<not_afterburner;y++) {

		gr_uline( i2f(SM_HIRES ? afterburner_bar_table_hires[y*2] : afterburner_bar_table[y*2]), i2f(y),
						i2f((SM_HIRES ? afterburner_bar_table_hires[y*2 + 1] : afterburner_bar_table[y*2 + 1]) + 1), i2f(y) );
	}

	gr_set_current_canvas( get_current_game_screen() );
}

void draw_shield_bar(int shield)
{
	int bm_num = shield>=100?9:(shield / 10);

	PAGE_IN_GAUGE( GAUGE_SHIELDS+9-bm_num	);
	gr_ubitmapm( SHIELD_GAUGE_X, SHIELD_GAUGE_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_SHIELDS+9-bm_num) ] );
}

#define CLOAK_FADE_WAIT_TIME  0x400

void draw_player_ship(int cloak_state,int old_cloak_state,int x, int y)
{
	static fix cloak_fade_timer=0;
	static int cloak_fade_value=GR_FADE_LEVELS-1;
	static int refade = 0;
	grs_bitmap *bm = NULL;

	if (Game_mode & GM_TEAM)	{
		#ifdef NETWORK
		PAGE_IN_GAUGE( GAUGE_SHIPS+get_team(Player_num) );
		bm = &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_SHIPS+get_team(Player_num)) ];
		#endif
	} else {
		PAGE_IN_GAUGE( GAUGE_SHIPS+Player_num );
		bm = &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_SHIPS+Player_num) ];
	}
	

	if (old_cloak_state==-1 && cloak_state)
			cloak_fade_value=0;

//	mprintf((0, "cloak/oldcloak %d/%d", cloak_state, old_cloak_state));

	if (!cloak_state) {
		cloak_fade_value=GR_FADE_LEVELS-1;
		cloak_fade_state = 0;
	}

	if (cloak_state==1 && old_cloak_state==0)
		cloak_fade_state = -1;
	//else if (cloak_state==0 && old_cloak_state==1)
	//	cloak_fade_state = 1;

	if (cloak_state && GameTime > Players[Player_num].cloak_time + CLOAK_TIME_MAX - i2f(3))		//doing "about-to-uncloak" effect
		if (cloak_fade_state==0)
			cloak_fade_state = 2;
	

	if (cloak_fade_state)
		cloak_fade_timer -= FrameTime;

	while (cloak_fade_state && cloak_fade_timer < 0) {

		cloak_fade_timer += CLOAK_FADE_WAIT_TIME;

		cloak_fade_value += cloak_fade_state;

		if (cloak_fade_value >= GR_FADE_LEVELS-1) {
			cloak_fade_value = GR_FADE_LEVELS-1;
			if (cloak_fade_state == 2 && cloak_state)
				cloak_fade_state = -2;
			else
				cloak_fade_state = 0;
		}
		else if (cloak_fade_value <= 0) {
			cloak_fade_value = 0;
			if (cloak_fade_state == -2)
				cloak_fade_state = 2;
			else
				cloak_fade_state = 0;
		}
	}

//	To fade out both pages in a paged mode.
	if (refade) refade = 0;
	else if (cloak_state && old_cloak_state && !cloak_fade_state && !refade) {
		cloak_fade_state = -1;
		refade = 1;
	}

	gr_set_current_canvas(&VR_render_buffer[0]);

	gr_ubitmap( x, y, bm);

	Gr_scanline_darkening_level = cloak_fade_value;
	gr_rect(x, y, x+bm->bm_w-1, y+bm->bm_h-1);
	Gr_scanline_darkening_level = GR_FADE_LEVELS;

	gr_set_current_canvas( get_current_game_screen() );

	gr_bm_ubitbltm( bm->bm_w, bm->bm_h, x, y, x, y, &VR_render_buffer[0].cv_bitmap, &grd_curcanv->cv_bitmap);
}

#define INV_FRAME_TIME	(f1_0/10)		//how long for each frame

void draw_numerical_display(int shield, int energy)
{
	gr_set_current_canvas( Canv_NumericalGauge );
	gr_set_curfont( GAME_FONT );
	PAGE_IN_GAUGE( GAUGE_NUMERICAL );
	gr_ubitmap( 0, 0, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_NUMERICAL) ] );

	gr_set_fontcolor(gr_getcolor(14,14,23),-1 );

	if (!SM_HIRES) {
		gr_printf((shield>99)?3:((shield>9)?5:7),15,"%d",shield);
		gr_set_fontcolor(gr_getcolor(25,18,6),-1 );
		gr_printf((energy>99)?3:((energy>9)?5:7),2,"%d",energy);
	} else {
		gr_printf((shield>99)?7:((shield>9)?11:15),33,"%d",shield);
		gr_set_fontcolor(gr_getcolor(25,18,6),-1 );
		gr_printf((energy>99)?7:((energy>9)?11:15),4,"%d",energy);
	}
	
	gr_set_current_canvas( get_current_game_screen() );
}


void draw_keys()
{
	gr_set_current_canvas( get_current_game_screen() );

	if (Players[Player_num].flags & PLAYER_FLAGS_BLUE_KEY )	{
		PAGE_IN_GAUGE( GAUGE_BLUE_KEY );
		gr_ubitmapm( GAUGE_BLUE_KEY_X, GAUGE_BLUE_KEY_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_BLUE_KEY) ] );
	} else {
		PAGE_IN_GAUGE( GAUGE_BLUE_KEY_OFF );
		gr_ubitmapm( GAUGE_BLUE_KEY_X, GAUGE_BLUE_KEY_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_BLUE_KEY_OFF) ] );
	}

	if (Players[Player_num].flags & PLAYER_FLAGS_GOLD_KEY)	{
		PAGE_IN_GAUGE( GAUGE_GOLD_KEY );
		gr_ubitmapm( GAUGE_GOLD_KEY_X, GAUGE_GOLD_KEY_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_GOLD_KEY) ] );
	} else {
		PAGE_IN_GAUGE( GAUGE_GOLD_KEY_OFF );
		gr_ubitmapm( GAUGE_GOLD_KEY_X, GAUGE_GOLD_KEY_Y, &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_GOLD_KEY_OFF) ] );
	}

	if (Players[Player_num].flags & PLAYER_FLAGS_RED_KEY)	{
		PAGE_IN_GAUGE( GAUGE_RED_KEY );
		gr_ubitmapm( GAUGE_RED_KEY_X,  GAUGE_RED_KEY_Y,  &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_RED_KEY) ] );
	} else {
		PAGE_IN_GAUGE( GAUGE_RED_KEY_OFF );
		gr_ubitmapm( GAUGE_RED_KEY_X,  GAUGE_RED_KEY_Y,  &GameBitmaps[ GET_GAUGE_INDEX(GAUGE_RED_KEY_OFF) ] );
	}
}


void draw_weapon_info_sub(int info_index,gauge_box *box,int pic_x,int pic_y,char *name,int text_x,int text_y)
{
	grs_bitmap *bm;
	char *p;

	//clear the window
	gr_setcolor(BM_XRGB(0,0,0));

	if (Cockpit_mode.intval == CM_STATUS_BAR)
		gr_rect(SBX(box->left), SBY(box->top), SBX(box->right), SBY(box->bot));
	else
		gr_rect(box->left, box->top, box->right, box->bot);

	if (Piggy_hamfile_version >= 3 // !SHAREWARE
		&& SM_HIRES)
	{
		bm=&GameBitmaps[Weapon_info[info_index].hires_picture.index];
		PIGGY_PAGE_IN( Weapon_info[info_index].hires_picture );
	} else {
		bm=&GameBitmaps[Weapon_info[info_index].picture.index];
		PIGGY_PAGE_IN( Weapon_info[info_index].picture );
	}

	Assert(bm != NULL);

	gr_ubitmapm(pic_x,pic_y,bm);
	
	gr_set_fontcolor(gr_getcolor(0,20,0),-1 );

	if ((p=strchr(name,'\n'))!=NULL) {
		*p=0;
		gr_printf(text_x,text_y,name);
		gr_printf(text_x,text_y+grd_curcanv->cv_font->ft_h+1,p+1);
		*p='\n';
	} else
		gr_printf(text_x,text_y,name);

	//	For laser, show level and quadness
	if (info_index == LASER_ID || info_index == SUPER_LASER_ID) {
		char	temp_str[7];

		sprintf(temp_str, "%s: 0", TXT_LVL);

		temp_str[5] = Players[Player_num].laser_level+1 + '0';

		gr_printf(text_x,text_y+Line_spacing, temp_str);

		if (Players[Player_num].flags & PLAYER_FLAGS_QUAD_LASERS) {
			strcpy(temp_str, TXT_QUAD);
			gr_printf(text_x,text_y+2*Line_spacing, temp_str);

		}

	}
}


void draw_weapon_info(int weapon_type,int weapon_num,int laser_level)
{
	int info_index;

	if (weapon_type == 0) {
		info_index = Primary_weapon_to_weapon_info[weapon_num];

		if (info_index == LASER_ID && laser_level > MAX_LASER_LEVEL)
			info_index = SUPER_LASER_ID;

		if (Cockpit_mode.intval == CM_STATUS_BAR)
			draw_weapon_info_sub(info_index,
				&gauge_boxes[SB_PRIMARY_BOX],
				SBX(SB_PRIMARY_W_PIC_X), SBY(SB_PRIMARY_W_PIC_Y),
				PRIMARY_WEAPON_NAMES_SHORT(weapon_num),
				SBX(SB_PRIMARY_W_TEXT_X), SBY(SB_PRIMARY_W_TEXT_Y));
		else
			draw_weapon_info_sub(info_index,
				&gauge_boxes[COCKPIT_PRIMARY_BOX],
				PRIMARY_W_PIC_X,PRIMARY_W_PIC_Y,
				PRIMARY_WEAPON_NAMES_SHORT(weapon_num),
				PRIMARY_W_TEXT_X,PRIMARY_W_TEXT_Y);

	}
	else {
		info_index = Secondary_weapon_to_weapon_info[weapon_num];

		if (Cockpit_mode.intval == CM_STATUS_BAR)
			draw_weapon_info_sub(info_index,
				&gauge_boxes[SB_SECONDARY_BOX],
				SBX(SB_SECONDARY_W_PIC_X), SBY(SB_SECONDARY_W_PIC_Y),
				SECONDARY_WEAPON_NAMES_SHORT(weapon_num),
				SBX(SB_SECONDARY_W_TEXT_X), SBY(SB_SECONDARY_W_TEXT_Y));
		else
			draw_weapon_info_sub(info_index,
				&gauge_boxes[COCKPIT_SECONDARY_BOX],
				SECONDARY_W_PIC_X,SECONDARY_W_PIC_Y,
				SECONDARY_WEAPON_NAMES_SHORT(weapon_num),
				SECONDARY_W_TEXT_X,SECONDARY_W_TEXT_Y);
	}
}

void draw_ammo_info(int x,int y,int ammo_count,int primary)
{
	int w;
	char str[16];

	if (primary)
		w = (grd_curcanv->cv_font->ft_w*7)/2;
	else
		w = (grd_curcanv->cv_font->ft_w*5)/2;

	gr_setcolor(BM_XRGB(0,0,0));
	gr_rect(x,y,x+w,y+grd_curcanv->cv_font->ft_h);
	gr_set_fontcolor(gr_getcolor(20,0,0),-1 );
	sprintf(str,"%03d",ammo_count);
	convert_1s(str);
	gr_printf(x,y,str);
}

void draw_secondary_ammo_info(int ammo_count)
{
	if (Cockpit_mode.intval == CM_STATUS_BAR)
		draw_ammo_info(SBX(SB_SECONDARY_AMMO_X), SBY(SB_SECONDARY_AMMO_Y), ammo_count, 0);
	else
		draw_ammo_info(SECONDARY_AMMO_X,SECONDARY_AMMO_Y,ammo_count,0);
}

//returns true if drew picture
int draw_weapon_box(int weapon_type,int weapon_num)
{
	int drew_flag=0;
	int laser_level_changed;

	gr_set_current_canvas(&VR_render_buffer[0]);

	gr_set_curfont( GAME_FONT );

	laser_level_changed = (weapon_type==0 && weapon_num==LASER_INDEX && (Players[Player_num].laser_level != old_laser_level[VR_current_page]));

	if ((weapon_num != old_weapon[weapon_type][VR_current_page] || laser_level_changed) && weapon_box_states[weapon_type] == WS_SET && (old_weapon[weapon_type][VR_current_page] != -1))
	{
		weapon_box_states[weapon_type] = WS_FADING_OUT;
		weapon_box_fade_values[weapon_type]=i2f(GR_FADE_LEVELS-1);
	}
		
	if (weapon_box_states[weapon_type] == WS_FADING_OUT) {
		draw_weapon_info(weapon_type,old_weapon[weapon_type][VR_current_page],old_laser_level[VR_current_page]);
		old_ammo_count[weapon_type][VR_current_page]=-1;
		Old_Omega_charge[VR_current_page]=-1;
		drew_flag=1;
		weapon_box_fade_values[weapon_type] -= FrameTime * FADE_SCALE;
		if (weapon_box_fade_values[weapon_type] <= 0) {
			weapon_box_states[weapon_type] = WS_FADING_IN;
			old_weapon[weapon_type][VR_current_page] = weapon_num;
			old_weapon[weapon_type][!VR_current_page] = weapon_num;
			old_laser_level[VR_current_page] = Players[Player_num].laser_level;
			old_laser_level[!VR_current_page] = Players[Player_num].laser_level;
			weapon_box_fade_values[weapon_type] = 0;
		}
	}
	else if (weapon_box_states[weapon_type] == WS_FADING_IN) {
		if (weapon_num != old_weapon[weapon_type][VR_current_page]) {
			weapon_box_states[weapon_type] = WS_FADING_OUT;
		}
		else {
			draw_weapon_info(weapon_type,weapon_num,Players[Player_num].laser_level);
			old_ammo_count[weapon_type][VR_current_page]=-1;
			Old_Omega_charge[VR_current_page]=-1;
			drew_flag=1;
			weapon_box_fade_values[weapon_type] += FrameTime * FADE_SCALE;
			if (weapon_box_fade_values[weapon_type] >= i2f(GR_FADE_LEVELS-1)) {
				weapon_box_states[weapon_type] = WS_SET;
				old_weapon[weapon_type][!VR_current_page] = -1;		//force redraw (at full fade-in) of other page
			}
		}
	} else
	//	if (old_weapon[weapon_type][VR_current_page] == -1)
			{
			//@@if (laser_level_changed)
			//@@	old_weapon[weapon_type][VR_current_page] = LASER_INDEX;
			//@@else 
			{
				draw_weapon_info(weapon_type, weapon_num, Players[Player_num].laser_level);
				old_weapon[weapon_type][VR_current_page] = weapon_num;
				old_ammo_count[weapon_type][VR_current_page] = -1;
				Old_Omega_charge[VR_current_page] = -1;
				old_laser_level[VR_current_page] = Players[Player_num].laser_level;
				drew_flag = 1;
			}
		}

	if (weapon_box_states[weapon_type] != WS_SET) {		//fade gauge
		Gr_scanline_darkening_level = f2i(weapon_box_fade_values[weapon_type]);
		if (Cockpit_mode.intval == CM_STATUS_BAR)
			gr_rect(SBX(gauge_boxes[SB_PRIMARY_BOX+weapon_type].left),  SBY(gauge_boxes[SB_PRIMARY_BOX+weapon_type].top),
			        SBX(gauge_boxes[SB_PRIMARY_BOX+weapon_type].right), SBY(gauge_boxes[SB_PRIMARY_BOX+weapon_type].bot));

		else
			gr_rect(gauge_boxes[COCKPIT_PRIMARY_BOX+weapon_type].left,  gauge_boxes[COCKPIT_PRIMARY_BOX+weapon_type].top,
			        gauge_boxes[COCKPIT_PRIMARY_BOX+weapon_type].right, gauge_boxes[COCKPIT_PRIMARY_BOX+weapon_type].bot);

		Gr_scanline_darkening_level = GR_FADE_LEVELS;
	}

	gr_set_current_canvas(get_current_game_screen());
	return drew_flag;
}

fix static_time[2];

void draw_static(int win)
{
	vclip *vc = &Vclip[VCLIP_MONITOR_STATIC];
	grs_bitmap *bmp;
	int framenum;
	int boxofs = (Cockpit_mode.intval == CM_STATUS_BAR)?SB_PRIMARY_BOX:COCKPIT_PRIMARY_BOX;
	int x,y;

	static_time[win] += FrameTime;
	if (static_time[win] >= vc->play_time) {
		weapon_box_user[win] = WBU_WEAPON;
		return;
	}

	framenum = static_time[win] * vc->num_frames / vc->play_time;

	PIGGY_PAGE_IN(vc->frames[framenum]);

	bmp = &GameBitmaps[vc->frames[framenum].index];

	gr_set_current_canvas(&VR_render_buffer[0]);
   
	for (x=gauge_boxes[boxofs+win].left;x<gauge_boxes[boxofs+win].right;x+=bmp->bm_w)
		for (y=gauge_boxes[boxofs+win].top;y<gauge_boxes[boxofs+win].bot;y+=bmp->bm_h)
			if (Cockpit_mode.intval == CM_STATUS_BAR)
				gr_bitmap(SBX(x), SBY(y), bmp);
			else
				gr_bitmap(x, y, bmp);

	gr_set_current_canvas(get_current_game_screen());

	copy_gauge_box(&gauge_boxes[boxofs+win],&VR_render_buffer[0].cv_bitmap);
}

void draw_weapon_boxes()
{
	int boxofs = (Cockpit_mode.intval == CM_STATUS_BAR)?SB_PRIMARY_BOX:COCKPIT_PRIMARY_BOX;
	int drew;

	if (weapon_box_user[0] == WBU_WEAPON) {
		drew = draw_weapon_box(0,Primary_weapon);
		if (drew) 
			copy_gauge_box(&gauge_boxes[boxofs+0],&VR_render_buffer[0].cv_bitmap);

		if (weapon_box_states[0] == WS_SET) {
			if ((Primary_weapon == VULCAN_INDEX) || (Primary_weapon == GAUSS_INDEX))
			{
				if (Newdemo_state == ND_STATE_RECORDING &&
		Players[Player_num].primary_ammo[VULCAN_INDEX] != old_ammo_count[0][VR_current_page])
					newdemo_record_primary_ammo(old_ammo_count[0][VR_current_page], Players[Player_num].primary_ammo[VULCAN_INDEX]);
				draw_primary_ammo_info(f2i((unsigned) VULCAN_AMMO_SCALE * (unsigned) Players[Player_num].primary_ammo[VULCAN_INDEX]));
				old_ammo_count[0][VR_current_page] = Players[Player_num].primary_ammo[VULCAN_INDEX];
			}

			if (Primary_weapon == OMEGA_INDEX)
			{
				if (Newdemo_state == ND_STATE_RECORDING &&
		Omega_charge != Old_Omega_charge[VR_current_page])
					newdemo_record_primary_ammo(Old_Omega_charge[VR_current_page], Omega_charge);
				draw_primary_ammo_info(Omega_charge * 100/MAX_OMEGA_CHARGE);
				Old_Omega_charge[VR_current_page] = Omega_charge;
			}
		}
	}
	else if (weapon_box_user[0] == WBU_STATIC)
		draw_static(0);

	if (weapon_box_user[1] == WBU_WEAPON) {
		drew = draw_weapon_box(1,Secondary_weapon);
		if (drew)
			copy_gauge_box(&gauge_boxes[boxofs+1],&VR_render_buffer[0].cv_bitmap);

		if (weapon_box_states[1] == WS_SET)
		{
			old_bombcount[VR_current_page] = 0x7fff;	//force redraw
			if (Newdemo_state == ND_STATE_RECORDING &&
	Players[Player_num].secondary_ammo[Secondary_weapon] != old_ammo_count[1][VR_current_page])
				newdemo_record_secondary_ammo(old_ammo_count[1][VR_current_page], Players[Player_num].secondary_ammo[Secondary_weapon]);
			draw_secondary_ammo_info(Players[Player_num].secondary_ammo[Secondary_weapon]);
			old_ammo_count[1][VR_current_page] = Players[Player_num].secondary_ammo[Secondary_weapon];

			if (Cockpit_mode.intval == CM_STATUS_BAR)
				show_bomb_count(SBX(SB_BOMB_COUNT_X), SBY(SB_BOMB_COUNT_Y), gr_find_closest_color(0, 0, 0), 0);
			else
				show_bomb_count(BOMB_COUNT_X, BOMB_COUNT_Y, gr_find_closest_color(0, 0, 0), 0);
		}
	}
	else if (weapon_box_user[1] == WBU_STATIC)
		draw_static(1);
}


void sb_draw_energy_bar(int energy)
{
	int erase_height, w, h, aw;
	char energy_str[20];

	gr_set_current_canvas( Canv_SBEnergyGauge );

	PAGE_IN_GAUGE( SB_GAUGE_ENERGY );
	gr_ubitmapm(0, 0, &GameBitmaps[GET_GAUGE_INDEX(SB_GAUGE_ENERGY)]);

	erase_height = (100 - energy) * SB_ENERGY_GAUGE_H / 100;

	if (erase_height > 0) {
		gr_setcolor( BM_XRGB(0,0,0) );
		gr_rect(0, 0, SB_ENERGY_GAUGE_W - 1, erase_height - 1);
	}
	
	//draw numbers
	sprintf(energy_str, "%d", energy);
	gr_get_string_size(energy_str, &w, &h, &aw );
	gr_set_fontcolor(gr_getcolor(25,18,6),-1 );
	gr_printf(((SB_ENERGY_GAUGE_W - w)/2), SB_ENERGY_GAUGE_H - GAME_FONT->ft_h - (GAME_FONT->ft_h / 4), "%d", energy);

	gr_set_current_canvas(get_current_game_screen());
}

void sb_draw_afterburner()
{
	int erase_height, w, h, aw;
	char ab_str[3] = "AB";

	gr_set_current_canvas( Canv_SBAfterburnerGauge );
	PAGE_IN_GAUGE( SB_GAUGE_AFTERBURNER );
	gr_ubitmapm(0, 0, &GameBitmaps[GET_GAUGE_INDEX(SB_GAUGE_AFTERBURNER)]);

	erase_height = fixmul((f1_0 - Afterburner_charge),SB_AFTERBURNER_GAUGE_H);

	if (erase_height > 0) {
		gr_setcolor( BM_XRGB(0,0,0) );
		gr_rect(0, 0, SB_AFTERBURNER_GAUGE_W - 1, erase_height - 1);
	}

	//draw legend
	if (Players[Player_num].flags & PLAYER_FLAGS_AFTERBURNER)
		gr_set_fontcolor(gr_getcolor(45,0,0),-1 );
	else 
		gr_set_fontcolor(gr_getcolor(12,12,12),-1 );

	gr_get_string_size(ab_str, &w, &h, &aw );
	gr_printf(((SB_AFTERBURNER_GAUGE_W - w)/2), SB_AFTERBURNER_GAUGE_H - GAME_FONT->ft_h - (GAME_FONT->ft_h / 4), "AB");
	gr_set_current_canvas(get_current_game_screen());
}

void sb_draw_shield_num(int shield)
{
	//draw numbers

	gr_set_curfont( GAME_FONT );
	gr_set_fontcolor(gr_getcolor(14,14,23),-1 );

	//erase old one
//	PIGGY_PAGE_IN(cockpit_bitmap[Cockpit_mode.intval + (SM_HIRES ? (Num_cockpits/2) : 0)]);

//	gr_setcolor(gr_gpixel(&grd_curcanv->cv_bitmap, SB_SHIELD_NUM_X - 1, SB_SHIELD_NUM_Y - 1));

//	gr_rect(SBX(SB_SHIELD_NUM_X), SBY(SB_SHIELD_NUM_Y), SBX(SB_SHIELD_NUM_X + (SM_HIRES ? 27 : 13)), SBY(SB_SHIELD_NUM_Y + GAME_FONT->ft_h));
	gr_printf(SBX((shield>99)?SB_SHIELD_NUM_X:((shield>9)?SB_SHIELD_NUM_X+2:SB_SHIELD_NUM_X+4)), SBY(SB_SHIELD_NUM_Y), "%d", shield);
}

void sb_draw_shield_bar(int shield)
{
	int bm_num = shield>=100?9:(shield / 10);

	gr_set_current_canvas(get_current_game_screen());
	PAGE_IN_GAUGE( GAUGE_SHIELDS+9-bm_num );
	gr_ubitmapm(SBX(SB_SHIELD_GAUGE_X), SBY(SB_SHIELD_GAUGE_Y), &GameBitmaps[GET_GAUGE_INDEX(GAUGE_SHIELDS+9-bm_num)]);
}

void sb_draw_keys()
{
	grs_bitmap * bm;
	int flags = Players[Player_num].flags;

	gr_set_current_canvas(get_current_game_screen());

	bm = &GameBitmaps[ GET_GAUGE_INDEX((flags&PLAYER_FLAGS_BLUE_KEY)?SB_GAUGE_BLUE_KEY:SB_GAUGE_BLUE_KEY_OFF) ];
	PAGE_IN_GAUGE( (flags&PLAYER_FLAGS_BLUE_KEY)?SB_GAUGE_BLUE_KEY:SB_GAUGE_BLUE_KEY_OFF );
	gr_ubitmapm(SBX(SB_GAUGE_KEYS_X), SBY(SB_GAUGE_BLUE_KEY_Y), bm);

	bm = &GameBitmaps[ GET_GAUGE_INDEX((flags&PLAYER_FLAGS_GOLD_KEY)?SB_GAUGE_GOLD_KEY:SB_GAUGE_GOLD_KEY_OFF) ];
	PAGE_IN_GAUGE( (flags&PLAYER_FLAGS_GOLD_KEY)?SB_GAUGE_GOLD_KEY:SB_GAUGE_GOLD_KEY_OFF );
	gr_ubitmapm(SBX(SB_GAUGE_KEYS_X), SBY(SB_GAUGE_GOLD_KEY_Y), bm);

	bm = &GameBitmaps[ GET_GAUGE_INDEX((flags&PLAYER_FLAGS_RED_KEY)?SB_GAUGE_RED_KEY:SB_GAUGE_RED_KEY_OFF) ];
	PAGE_IN_GAUGE( (flags&PLAYER_FLAGS_RED_KEY)?SB_GAUGE_RED_KEY:SB_GAUGE_RED_KEY_OFF );
	gr_ubitmapm(SBX(SB_GAUGE_KEYS_X), SBY(SB_GAUGE_RED_KEY_Y), bm);
}

//	Draws invulnerable ship, or maybe the flashing ship, depending on invulnerability time left.
void draw_invulnerable_ship()
{
	static fix time=0;

	gr_set_current_canvas(get_current_game_screen());

	if (((Players[Player_num].invulnerable_time + INVULNERABLE_TIME_MAX - GameTime) > F1_0*4) || (GameTime & 0x8000)) {

		if (Cockpit_mode.intval == CM_STATUS_BAR) {
			PAGE_IN_GAUGE( GAUGE_INVULNERABLE+invulnerable_frame );
			gr_ubitmapm(SBX(SB_SHIELD_GAUGE_X), SBY(SB_SHIELD_GAUGE_Y),
			            &GameBitmaps[GET_GAUGE_INDEX(GAUGE_INVULNERABLE+invulnerable_frame) ] );
		} else {
			PAGE_IN_GAUGE( GAUGE_INVULNERABLE+invulnerable_frame );
			gr_ubitmapm(SBX(SHIELD_GAUGE_X), SBY(SHIELD_GAUGE_Y),
			            &GameBitmaps[GET_GAUGE_INDEX(GAUGE_INVULNERABLE+invulnerable_frame)] );
		}

		time += FrameTime;

		while (time > INV_FRAME_TIME) {
			time -= INV_FRAME_TIME;
			if (++invulnerable_frame == N_INVULNERABLE_FRAMES)
				invulnerable_frame=0;
		}
	} else if (Cockpit_mode.intval == CM_STATUS_BAR)
		sb_draw_shield_bar(f2ir(Players[Player_num].shields));
	else
		draw_shield_bar(f2ir(Players[Player_num].shields));
}

extern int Missile_gun;
extern int allowed_to_fire_laser(void);
extern int allowed_to_fire_missile(void);

rgb player_rgb[] = {
							{15,15,23},
							{27,0,0},
							{0,23,0},
							{30,11,31},
							{31,16,0},
							{24,17,6},
							{14,21,12},
							{29,29,0},
						};

extern ubyte Newdemo_flying_guided;
extern int max_window_w;

typedef struct {
	sbyte x, y;
} xy;

//offsets for reticle parts: high-big  high-sml  low-big  low-sml
xy cross_offsets[4] = 		{ {-8,-5},	{-4,-2},	{-4,-2}, {-2,-1} };
xy primary_offsets[4] = 	{ {-30,14}, {-16,6},	{-15,6}, {-8, 2} };
xy secondary_offsets[4] =	{ {-24,2},	{-12,0}, {-12,1}, {-6,-2} };

//draw the reticle
void show_reticle(int force_big_one)
{
	int x,y;
	int laser_ready,missile_ready,laser_ammo,missile_ammo;
	int cross_bm_num,primary_bm_num,secondary_bm_num;
	int use_hires_reticle,small_reticle,ofs,gauge_index;

	if (Newdemo_state==ND_STATE_PLAYBACK && Newdemo_flying_guided)
	{
		 draw_guided_crosshair();
		 return;
	}

	x = grd_curcanv->cv_w/2;
	y = grd_curcanv->cv_h/2;

	laser_ready = allowed_to_fire_laser();
	missile_ready = allowed_to_fire_missile();

	laser_ammo = player_has_weapon(Primary_weapon,0);
	missile_ammo = player_has_weapon(Secondary_weapon,1);

	primary_bm_num = (laser_ready && laser_ammo==HAS_ALL);
	secondary_bm_num = (missile_ready && missile_ammo==HAS_ALL);

	if (primary_bm_num && Primary_weapon==LASER_INDEX && (Players[Player_num].flags & PLAYER_FLAGS_QUAD_LASERS))
		primary_bm_num++;

	if (Secondary_weapon_to_gun_num[Secondary_weapon]==7)
		secondary_bm_num += 3;		//now value is 0,1 or 3,4
	else if (secondary_bm_num && !(Missile_gun&1))
			secondary_bm_num++;

	cross_bm_num = ((primary_bm_num > 0) || (secondary_bm_num > 0));

	Assert(primary_bm_num <= 2);
	Assert(secondary_bm_num <= 4);
	Assert(cross_bm_num <= 1);
#ifdef OGL
	if (gl_reticle==2 || (gl_reticle && grd_curcanv->cv_bitmap.bm_w > 320))
	{
		ogl_draw_reticle(cross_bm_num,primary_bm_num,secondary_bm_num);
	}
	else
	{
#endif


		use_hires_reticle = (FontHires != 0);

	small_reticle = !(grd_curcanv->cv_bitmap.bm_w*3 > max_window_w*2 || force_big_one);
	ofs = (use_hires_reticle?0:2) + small_reticle;

	gauge_index = (small_reticle?SML_RETICLE_CROSS:RETICLE_CROSS) + cross_bm_num;
	PAGE_IN_GAUGE( gauge_index );
	gr_ubitmapm(x+cross_offsets[ofs].x,y+cross_offsets[ofs].y,&GameBitmaps[GET_GAUGE_INDEX(gauge_index)] );

	gauge_index = (small_reticle?SML_RETICLE_PRIMARY:RETICLE_PRIMARY) + primary_bm_num;
	PAGE_IN_GAUGE( gauge_index );
	gr_ubitmapm(x+primary_offsets[ofs].x,y+primary_offsets[ofs].y,&GameBitmaps[GET_GAUGE_INDEX(gauge_index)] );

	gauge_index = (small_reticle?SML_RETICLE_SECONDARY:RETICLE_SECONDARY) + secondary_bm_num;
	PAGE_IN_GAUGE( gauge_index );
	gr_ubitmapm(x+secondary_offsets[ofs].x,y+secondary_offsets[ofs].y,&GameBitmaps[GET_GAUGE_INDEX(gauge_index)] );

#ifdef OGL
	}
#endif
}

#ifdef NETWORK
void hud_show_kill_list()
{
	int n_players,player_list[MAX_NUM_NET_PLAYERS];
	int n_left,i,x0,x1,y,save_y,fth;

// ugly hack since placement of netgame players and kills is based off of
// menuhires (which is always 1 for mac).  This throws off placement of
// players in pixel double mode.

	if (Show_kill_list_timer > 0)
	{
		Show_kill_list_timer -= FrameTime;
		if (Show_kill_list_timer < 0)
			Show_kill_list = 0;
	}
	
	gr_set_curfont( GAME_FONT );

	n_players = multi_get_kill_list(player_list);

	if (Show_kill_list == 3)
		n_players = 2;

	if (n_players <= 4)
		n_left = n_players;
	else
		n_left = (n_players+1)/2;

	//If font size changes, this code might not work right anymore 
	//Assert(GAME_FONT->ft_h==5 && GAME_FONT->ft_w==7);

	fth = GAME_FONT->ft_h;

	x0 = LHX(1); x1 = LHX(43);

	if (Game_mode & GM_MULTI_COOP)
		x1 = LHX(31);

	save_y = y = grd_curcanv->cv_h - n_left*(fth+1);

	if (Cockpit_mode.intval == CM_FULL_COCKPIT) {
		save_y = y -= LHX(6);
		if (Game_mode & GM_MULTI_COOP)
			x1 = LHX(33);
		else
			x1 = LHX(43);
	}

	for (i=0;i<n_players;i++) {
		int player_num;
		char name[9];
		int sw,sh,aw;

		if (i>=n_left) {
			if (Cockpit_mode.intval == CM_FULL_COCKPIT)
				x0 = grd_curcanv->cv_w - LHX(53);
			else
				x0 = grd_curcanv->cv_w - LHX(60);
			if (Game_mode & GM_MULTI_COOP)
				x1 = grd_curcanv->cv_w - LHX(27);
			else
				x1 = grd_curcanv->cv_w - LHX(15);  // Right edge of name, change this for width problems
			if (i==n_left)
				y = save_y;

        if (Netgame.KillGoal || Netgame.PlayTimeAllowed)
           {
             x1-=LHX(18);
            // x0-=LHX(18);
           }
		}
     else  if (Netgame.KillGoal || Netgame.PlayTimeAllowed)
           {
				 x1 = LHX(43);
             x1-=LHX(18);
            // x0-=LHX(18);
           }

	
		if (Show_kill_list == 3)
			player_num = i;
		else
			player_num = player_list[i];

		if (Show_kill_list == 1 || Show_kill_list==2)
		{
			int color;

			if (Players[player_num].connected != 1)
				gr_set_fontcolor(gr_getcolor(12, 12, 12), -1);
			else if (Game_mode & GM_TEAM) {
				color = get_team(player_num);
				gr_set_fontcolor(gr_getcolor(player_rgb[color].r,player_rgb[color].g,player_rgb[color].b),-1 );
			}
			else {
				color = player_num;
				gr_set_fontcolor(gr_getcolor(player_rgb[color].r,player_rgb[color].g,player_rgb[color].b),-1 );
			}
		}	

		else 
		{
			gr_set_fontcolor(gr_getcolor(player_rgb[player_num].r,player_rgb[player_num].g,player_rgb[player_num].b),-1 );
		}

		if (Show_kill_list == 3)
			strcpy(name, Netgame.team_name[i]);
		else
			strcpy(name,Players[player_num].callsign);	// Note link to above if!!
		gr_get_string_size(name,&sw,&sh,&aw);
		while (sw > (x1-x0-LHX(2))) {
			name[strlen(name)-1]=0;
			gr_get_string_size(name,&sw,&sh,&aw);
		}
		gr_printf(x0,y,"%s",name);

		if (Show_kill_list==2)
		 {
		  if (Players[player_num].net_killed_total+Players[player_num].net_kills_total==0)
			gr_printf (x1,y,"NA");
		  else
		   gr_printf (x1,y,"%d%%",(int)((float)((float)Players[player_num].net_kills_total/((float)Players[player_num].net_killed_total+(float)Players[player_num].net_kills_total))*100.0));		
		 }
		else if (Show_kill_list == 3)	
			gr_printf(x1,y,"%3d",team_kills[i]);
		else if (Game_mode & GM_MULTI_COOP)
			gr_printf(x1,y,"%-6d",Players[player_num].score);
      else if (Netgame.PlayTimeAllowed || Netgame.KillGoal)
         gr_printf(x1,y,"%3d(%d)",Players[player_num].net_kills_total,Players[player_num].KillGoalCount);
      else
			gr_printf(x1,y,"%3d",Players[player_num].net_kills_total);
                        
		y += fth+1;

	}
}
#endif

#ifndef RELEASE
extern int Saving_movie_frames;
#else
#define Saving_movie_frames 0
#endif

//returns true if viewer can see object
int see_object(int objnum)
{
	fvi_query fq;
	int hit_type;
	fvi_info hit_data;

	//see if we can see this player

	fq.p0 					= &Viewer->pos;
	fq.p1 					= &Objects[objnum].pos;
	fq.rad 					= 0;
	fq.thisobjnum       = OBJECT_NUMBER(Viewer);
	fq.flags 				= FQ_TRANSWALL | FQ_CHECK_OBJS;
	fq.startseg				= Viewer->segnum;
	fq.ignore_obj_list	= NULL;

	hit_type = find_vector_intersection(&fq, &hit_data);

	return (hit_type == HIT_OBJECT && hit_data.hit_object == objnum);
}

#ifdef NETWORK
//show names of teammates & players carrying flags
void show_HUD_names()
{
	int show_team_names,show_all_names,show_flags,player_team;
	int p;

	show_all_names = ((Newdemo_state == ND_STATE_PLAYBACK) || (Netgame.ShowAllNames && Show_reticle_name));
	show_team_names = (((Game_mode & GM_MULTI_COOP) || (Game_mode & GM_TEAM)) && Show_reticle_name);
	show_flags = (Game_mode & GM_CAPTURE) | (Game_mode & GM_HOARD);

	if (! (show_all_names || show_team_names || show_flags))
		return;

	player_team = get_team(Player_num);

	for (p=0;p<N_players;p++) {	//check all players
		int objnum;
		int show_name,has_flag;

		show_name = ((show_all_names && !(Players[p].flags & PLAYER_FLAGS_CLOAKED)) || (show_team_names && get_team(p)==player_team));
		has_flag = (Players[p].connected && Players[p].flags & PLAYER_FLAGS_FLAG);

		if (Newdemo_state == ND_STATE_PLAYBACK) {
			//if this is a demo, the objnum in the player struct is wrong,
			//so we search the object list for the objnum

			for (objnum=0;objnum<=Highest_object_index;objnum++)
				if (Objects[objnum].type==OBJ_PLAYER && Objects[objnum].id == p)
					break;
			if (objnum > Highest_object_index)		//not in list, thus not visible
				show_name = has_flag = 0;				//..so don't show name
		}
		else
			objnum = Players[p].objnum;

		if ((show_name || has_flag) && see_object(objnum)) {
			g3s_point player_point;

			g3_rotate_point(&player_point,&Objects[objnum].pos);

			if (player_point.p3_codes == 0) {	//on screen

				g3_project_point(&player_point);

				if (! (player_point.p3_flags & PF_OVERFLOW)) {
					fix x,y;
			
					x = player_point.p3_sx;
					y = player_point.p3_sy;
			
					if (show_name) {				// Draw callsign on HUD
						char s[CALLSIGN_LEN+1];
						int w, h, aw;
						int x1, y1;
						int color_num;
			
						color_num = (Game_mode & GM_TEAM)?get_team(p):p;

						sprintf(s, "%.7s", Players[p].callsign);
						gr_get_string_size(s, &w, &h, &aw);
						gr_set_fontcolor(gr_getcolor(player_rgb[color_num].r,player_rgb[color_num].g,player_rgb[color_num].b),-1 );
						x1 = f2i(x)-w/2;
						y1 = f2i(y)-h/2;
						gr_string (x1, y1, s);
					}
		
					if (has_flag) {				// Draw box on HUD
						fix dx,dy,w,h;
			
						dy = -fixmuldiv(fixmul(Objects[objnum].size,Matrix_scale.y),i2f(grd_curcanv->cv_h)/2,player_point.p3_z);
						dx = fixmul(dy,grd_curscreen->sc_aspect);
	
						w = dx/4;
						h = dy/4;
	
						if (Game_mode & GM_CAPTURE)
							gr_setcolor((get_team(p) == TEAM_BLUE)?BM_XRGB(31,0,0):BM_XRGB(0,0,31));
						else if (Game_mode & GM_HOARD)
						{
							if (Game_mode & GM_TEAM)
								gr_setcolor((get_team(p) == TEAM_RED)?BM_XRGB(31,0,0):BM_XRGB(0,0,31));
							else
								gr_setcolor(BM_XRGB(0,31,0));
						}

						gr_line(x+dx-w,y-dy,x+dx,y-dy);
						gr_line(x+dx,y-dy,x+dx,y-dy+h);
	
						gr_line(x-dx,y-dy,x-dx+w,y-dy);
						gr_line(x-dx,y-dy,x-dx,y-dy+h);
	
						gr_line(x+dx-w,y+dy,x+dx,y+dy);
						gr_line(x+dx,y+dy,x+dx,y+dy-h);
	
						gr_line(x-dx,y+dy,x-dx+w,y+dy);
						gr_line(x-dx,y+dy,x-dx,y+dy-h);
					}
				}
			}
		}
	}
}
#endif


extern int last_drawn_cockpit[2];

//draw all the things on the HUD
void draw_hud()
{

#if 0 //def OGL
        if (Cockpit_mode.intval == CM_STATUS_BAR) {
                //ogl needs to redraw every frame, at least currently.
                //              init_cockpit();
			last_drawn_cockpit[0]=-1;
			last_drawn_cockpit[1]=-1;
                                  init_gauges();
                
                               //              vr_reset_display();
        }
#endif
                                          

	Line_spacing = GAME_FONT->ft_h + GAME_FONT->ft_h/4;

	//	Show score so long as not in rearview
	if ( !Rear_view && Cockpit_mode.intval != CM_REAR_VIEW && Cockpit_mode.intval != CM_STATUS_BAR && !Saving_movie_frames) {
		hud_show_score();
		if (score_time)
			hud_show_score_added();
	}

	if ( !Rear_view && Cockpit_mode.intval != CM_REAR_VIEW && !Saving_movie_frames)
	 hud_show_timer_count();

	//	Show other stuff if not in rearview or letterbox.
	if (!Rear_view && Cockpit_mode.intval != CM_REAR_VIEW) { // && Cockpit_mode.intval!=CM_LETTERBOX) {
		if (Cockpit_mode.intval == CM_STATUS_BAR || Cockpit_mode.intval == CM_FULL_SCREEN)
			hud_show_homing_warning();

		if (Cockpit_mode.intval == CM_FULL_SCREEN) {
			hud_show_energy();
			hud_show_shield();
			hud_show_afterburner();
			hud_show_weapons();
			if (!Saving_movie_frames)
				hud_show_keys();
			hud_show_cloak_invuln();

			if ( ( Newdemo_state == ND_STATE_RECORDING ) && ( Players[Player_num].flags != old_flags[VR_current_page] )) {
				newdemo_record_player_flags(old_flags[VR_current_page], Players[Player_num].flags);
				old_flags[VR_current_page] = Players[Player_num].flags;
			}
		}

		#ifdef NETWORK
		#ifndef RELEASE
		if (!(Game_mode&GM_MULTI && Show_kill_list) && !Saving_movie_frames)
			show_time();
		#endif
		#endif
		if (Reticle_on.intval && Cockpit_mode.intval != CM_LETTERBOX && (!Use_player_head_angles))
			show_reticle(0);

#ifdef NETWORK
		show_HUD_names();

		if (Cockpit_mode.intval != CM_LETTERBOX && Cockpit_mode.intval != CM_REAR_VIEW)
			hud_show_flag();

		if (Cockpit_mode.intval != CM_LETTERBOX && Cockpit_mode.intval != CM_REAR_VIEW)
			hud_show_orbs();

#endif
		if (!Saving_movie_frames)
			HUD_render_message_frame();

		if (Cockpit_mode.intval != CM_STATUS_BAR && !Saving_movie_frames)
			hud_show_lives();

		#ifdef NETWORK
		if (Game_mode&GM_MULTI && Show_kill_list)
			hud_show_kill_list();
		#endif
	}

	if (Rear_view && Cockpit_mode.intval != CM_REAR_VIEW) {
		HUD_render_message_frame();
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor(gr_getcolor(0,31,0),-1 );
		if (Newdemo_state == ND_STATE_PLAYBACK)
			gr_printf(0x8000,grd_curcanv->cv_h-14,TXT_REAR_VIEW);
		else
			gr_printf(0x8000,grd_curcanv->cv_h-10,TXT_REAR_VIEW);
	}
}

extern short *BackBuffer;

//print out some player statistics
void render_gauges()
{
	static uint32_t old_display_mode = 0xfffffffe;
	int energy = f2ir(Players[Player_num].energy);
	int shields = f2ir(Players[Player_num].shields);
	int cloak = ((Players[Player_num].flags&PLAYER_FLAGS_CLOAKED) != 0);
 
	Assert(Cockpit_mode.intval == CM_FULL_COCKPIT || Cockpit_mode.intval == CM_STATUS_BAR);

// check to see if our display mode has changed since last render time --
// if so, then we need to make new gauge canvases.
// Also make the first gauge canvases here, in the game,
// because they are [now] sub-canvases.

  
	if (old_display_mode != Current_display_mode)
	{
		close_gauge_canvases();
		init_gauge_canvases();
		old_display_mode = Current_display_mode;
	}

	if (shields < 0 ) shields = 0;

	gr_set_current_canvas(get_current_game_screen());
	gr_set_curfont( GAME_FONT );

	if (Newdemo_state == ND_STATE_RECORDING)
		if (Players[Player_num].homing_object_dist >= 0)
			newdemo_record_homing_distance(Players[Player_num].homing_object_dist);

	if (Cockpit_mode.intval == CM_FULL_COCKPIT)
		draw_player_ship(cloak, old_cloak[VR_current_page], SHIP_GAUGE_X, SHIP_GAUGE_Y);
	else
		draw_player_ship(cloak, old_cloak[VR_current_page], SBX(SB_SHIP_GAUGE_X), SBY(SB_SHIP_GAUGE_Y));

	old_cloak[VR_current_page] = cloak;

	if (Cockpit_mode.intval == CM_FULL_COCKPIT) {
		if (Newdemo_state == ND_STATE_RECORDING && (energy != old_energy[VR_current_page]))
		{
			newdemo_record_player_energy(old_energy[VR_current_page], energy);
			old_energy[VR_current_page] = energy;
		}
		draw_energy_bar(energy);
		draw_numerical_display(shields, energy);

		if (Newdemo_state == ND_STATE_RECORDING && (Afterburner_charge != old_afterburner[VR_current_page]))
		{
			newdemo_record_player_afterburner(old_afterburner[VR_current_page], Afterburner_charge);
			old_afterburner[VR_current_page] = Afterburner_charge;
		}
		draw_afterburner_bar(Afterburner_charge);

		if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE) {
			draw_invulnerable_ship();
			old_shields[VR_current_page] = shields ^ 1;
		} else {		// Draw the shield gauge
			if (Newdemo_state == ND_STATE_RECORDING && (shields != old_shields[VR_current_page]))
			{
				newdemo_record_player_shields(old_shields[VR_current_page], shields);
				old_shields[VR_current_page] = shields;
			}
			draw_shield_bar(shields);
		}
		draw_numerical_display(shields, energy);
	
		if (Newdemo_state == ND_STATE_RECORDING && (Players[Player_num].flags != old_flags[VR_current_page]))
		{
			newdemo_record_player_flags(old_flags[VR_current_page], Players[Player_num].flags);
			old_flags[VR_current_page] = Players[Player_num].flags;
		}
		draw_keys();

		show_homing_warning();

	} else if (Cockpit_mode.intval == CM_STATUS_BAR) {

		if (Newdemo_state == ND_STATE_RECORDING && (energy != old_energy[VR_current_page]))
		{
			newdemo_record_player_energy(old_energy[VR_current_page], energy);
			old_energy[VR_current_page] = energy;
		}
		sb_draw_energy_bar(energy);

		if (Newdemo_state == ND_STATE_RECORDING && (Afterburner_charge != old_afterburner[VR_current_page]))
		{
			newdemo_record_player_afterburner(old_afterburner[VR_current_page], Afterburner_charge);
			old_afterburner[VR_current_page] = Afterburner_charge;
		}
		sb_draw_afterburner();

		if (Players[Player_num].flags & PLAYER_FLAGS_INVULNERABLE)
		{
			draw_invulnerable_ship();
			old_shields[VR_current_page] = shields ^ 1;
		} 
		else
		{		// Draw the shield gauge
			if (Newdemo_state == ND_STATE_RECORDING && (shields != old_shields[VR_current_page]))
			{
				newdemo_record_player_shields(old_shields[VR_current_page], shields);
				old_shields[VR_current_page] = shields;
			}
			sb_draw_shield_bar(shields);
		}
		sb_draw_shield_num(shields);

		if (Newdemo_state == ND_STATE_RECORDING && (Players[Player_num].flags != old_flags[VR_current_page]))
		{
			newdemo_record_player_flags(old_flags[VR_current_page], Players[Player_num].flags);
			old_flags[VR_current_page] = Players[Player_num].flags;
		}
		sb_draw_keys();
	

		// May want to record this in a demo...
		if ((Game_mode & GM_MULTI) && !(Game_mode & GM_MULTI_COOP))
		{
			sb_show_lives();
			old_lives[VR_current_page] = Players[Player_num].net_killed_total;
		}
		else
		{
			sb_show_lives();
			old_lives[VR_current_page] = Players[Player_num].lives;
		}

		if ((Game_mode&GM_MULTI) && !(Game_mode & GM_MULTI_COOP))
		{
			sb_show_score();
			old_score[VR_current_page] = Players[Player_num].net_kills_total;
		}
		else
		{
			sb_show_score();
			old_score[VR_current_page] = Players[Player_num].score;

			//if (score_time)
				sb_show_score_added();
		}
	}


	draw_weapon_boxes();

}

//	---------------------------------------------------------------------------------------------------------
//	Call when picked up a laser powerup.
//	If laser is active, set old_weapon[0] to -1 to force redraw.
void update_laser_weapon_info(void)
{
	if (old_weapon[0][VR_current_page] == 0)
		if (! (Players[Player_num].laser_level > MAX_LASER_LEVEL && old_laser_level[VR_current_page] <= MAX_LASER_LEVEL))
			old_weapon[0][VR_current_page] = -1;
}

extern int Game_window_y;
void fill_background(void);

int SW_drawn[2], SW_x[2], SW_y[2], SW_w[2], SW_h[2];

//draws a 3d view into one of the cockpit windows.  win is 0 for left,
//1 for right.  viewer is object.  NULL object means give up window
//user is one of the WBU_ constants.  If rear_view_flag is set, show a
//rear view.  If label is non-NULL, print the label at the top of the
//window.
void do_cockpit_window_view(int win,object *viewer,int rear_view_flag,int user,const char *label)
{
	grs_canvas window_canv;
	static grs_canvas overlap_canv;
	object *viewer_save = Viewer;
	static int overlap_dirty[2]={0,0};
	static int window_x,window_y;
	gauge_box *box;
	int rear_view_save = Rear_view;
	int w,h,dx;

	box = NULL;

	if (viewer == NULL) {								//this user is done

		Assert(user == WBU_WEAPON || user == WBU_STATIC);

		if (user == WBU_STATIC && weapon_box_user[win] != WBU_STATIC)
			static_time[win] = 0;

		if (weapon_box_user[win] == WBU_WEAPON || weapon_box_user[win] == WBU_STATIC)
			return;		//already set

		weapon_box_user[win] = user;

		if (overlap_dirty[win]) {
			gr_set_current_canvas(&VR_screen_pages[VR_current_page]);
			fill_background();
			overlap_dirty[win] = 0;
		}

		return;
	}

	update_rendered_data(win+1, viewer, rear_view_flag, user);

	weapon_box_user[win] = user;						//say who's using window
		
	Viewer = viewer;
	Rear_view = rear_view_flag;

	if (Cockpit_mode.intval == CM_FULL_SCREEN)
	{

		w = VR_render_buffer[0].cv_bitmap.bm_w/6;			// hmm.  I could probably do the sub_buffer assigment for all macines, but I aint gonna chance it

		h = i2f(w) / grd_curscreen->sc_aspect;

		dx = (win==0)?-(w+(w/10)):(w/10);

		window_x = VR_render_buffer[0].cv_bitmap.bm_w/2+dx;
		window_y = VR_render_buffer[0].cv_bitmap.bm_h-h-(h/10);

		//copy these vars so stereo code can get at them
		SW_drawn[win]=1; SW_x[win] = window_x; SW_y[win] = window_y; SW_w[win] = w; SW_h[win] = h; 

		gr_init_sub_canvas(&window_canv,&VR_render_buffer[0],window_x,window_y,w,h);
	}
	else {
		if (Cockpit_mode.intval == CM_FULL_COCKPIT)
		{
			box = &gauge_boxes[COCKPIT_PRIMARY_BOX+win];
			gr_init_sub_canvas(&window_canv, &VR_render_buffer[0],
			                   box->left, box->top,
			                   box->right-box->left+1, box->bot-box->top+1);
		}
		else if (Cockpit_mode.intval == CM_STATUS_BAR)
		{
			box = &gauge_boxes[SB_PRIMARY_BOX+win];
			gr_init_sub_canvas(&window_canv, &VR_render_buffer[0],
			                   SBX(box->left), SBY(box->top),
			                   box->right-box->left+1, box->bot-box->top+1);
		}
		else
			goto abort;
	}

	gr_set_current_canvas(&window_canv);
	
	render_frame(0, win+1);
		
	//	HACK! If guided missile, wake up robots as necessary.
	if (viewer->type == OBJ_WEAPON) {
		// -- Used to require to be GUIDED -- if (viewer->id == GUIDEDMISS_ID)
		wake_up_rendered_objects(viewer, win+1);
	}

	if (label) {
		gr_set_curfont( GAME_FONT );
		if (Color_0_31_0 == -1)
			Color_0_31_0 = gr_getcolor(0,31,0);
		gr_set_fontcolor(Color_0_31_0, -1);
		gr_printf(0x8000,2,label);
	}

	if (user == WBU_GUIDED)
		draw_guided_crosshair();

	if (Cockpit_mode.intval == CM_FULL_SCREEN) {
		int small_window_bottom,big_window_bottom,extra_part_h;
		
		{
			gr_setcolor(BM_XRGB(0,0,32));
			gr_ubox(0,0,grd_curcanv->cv_bitmap.bm_w-1,grd_curcanv->cv_bitmap.bm_h-1);
		}

		//if the window only partially overlaps the big 3d window, copy
		//the extra part to the visible screen

		big_window_bottom = Game_window_y + Game_window_h.intval - 1;

		if (window_y > big_window_bottom) {

			//the small window is completely outside the big 3d window, so
			//copy it to the visible screen

			if (VR_screen_flags & VRF_USE_PAGING)
				gr_set_current_canvas(&VR_screen_pages[!VR_current_page]);
			else
				gr_set_current_canvas(get_current_game_screen());

			gr_bitmap(window_x,window_y,&window_canv.cv_bitmap);

			overlap_dirty[win] = 1;
		}
		else {

			small_window_bottom = window_y + window_canv.cv_bitmap.bm_h - 1;
			
			extra_part_h = small_window_bottom - big_window_bottom;

			if (extra_part_h > 0) {
			
				gr_init_sub_canvas(&overlap_canv,&window_canv,0,window_canv.cv_bitmap.bm_h-extra_part_h,window_canv.cv_bitmap.bm_w,extra_part_h);

				if (VR_screen_flags & VRF_USE_PAGING)
					gr_set_current_canvas(&VR_screen_pages[!VR_current_page]);
				else
					gr_set_current_canvas(get_current_game_screen());

				gr_bitmap(window_x,big_window_bottom+1,&overlap_canv.cv_bitmap);
				
				overlap_dirty[win] = 1;
			}
		}
	}
	else {
	
		gr_set_current_canvas(get_current_game_screen());
		copy_gauge_box(box,&VR_render_buffer[0].cv_bitmap);
	}

	//force redraw when done
	old_weapon[win][VR_current_page] = old_ammo_count[win][VR_current_page] = -1;

abort:;

	Viewer = viewer_save;

	Rear_view = rear_view_save;
}

#ifdef MACINTOSH
	void calculate_sub_view_window_bounds(int inSubWindowNum, TQARect* outBoundsRect)
	{
		int 		boxNumber 		= 0;
		gauge_box*	currentGaugeBox	= NULL;
		int w 	= 0;
		int h 	= 0;
		int dx 	= 0;
		int window_x = 0;
		int window_y = 0;

		Assert(outBoundsRect);
		Assert((inSubWindowNum == 0) || (inSubWindowNum == 1));
		
		switch (Cockpit_mode.intval)
		{
			case CM_FULL_SCREEN:
				// note: this calculation is taken from do_cockpit_window_view for the full
				// screen mode case
		
				w = (VR_render_buffer[0].cv_bitmap.bm_w) / 6;
				h = (i2f(w)) / (grd_curscreen->sc_aspect);
		
				dx = (inSubWindowNum==0)?-(w+(w/10)):(w/10);
		
				window_x = ((VR_render_buffer[0].cv_bitmap.bm_w) / 2) + dx;
				window_y = (VR_render_buffer[0].cv_bitmap.bm_h) - h - (h/10);
				
				outBoundsRect->top 		= window_x;
				outBoundsRect->left		= window_y;
				outBoundsRect->bottom	= window_x + w;
				outBoundsRect->right 	= window_y + h;
				break;
	
			case CM_FULL_COCKPIT:
			case CM_STATUS_BAR:
				if (inSubWindowNum == 0)
				{
					boxNumber = SB_PRIMARY_BOX;
				}
				else
				{
					boxNumber = SB_SECONDARY_BOX;
				}
				
				//boxNumber = (Current_display_mode * 4) + (Cockpit_mode.intval * 2) + inSubWindowNum;
				currentGaugeBox = &gauge_boxes[boxNumber];
				Assert(currentGaugeBox);
				
				outBoundsRect->top 		= currentGaugeBox->top;
				outBoundsRect->left		= currentGaugeBox->left;
				outBoundsRect->bottom	= currentGaugeBox->bot + 1;
				outBoundsRect->right 	= currentGaugeBox->right + 1;
				
				break;
	
			default:
				Int3();
				return;
		}
	}

#endif


