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
 * Header for controls.c
 *
 */


#ifndef _CONTROLS_H
#define _CONTROLS_H


typedef enum {
	slide_on,
	bank_on,
	rear_view,
	fire_primary,
	fire_secondary,
	fire_flare,
	drop_bomb,
	automap,
	afterburner,
	cycle_primary,
	cycle_secondary,
	headlight,
	CONTROL_NUM_BUTTONS
} control_button;


typedef struct _control_info {
	fix pitch_time;
	fix vertical_thrust_time;
	fix heading_time;
	fix sideways_thrust_time;
	fix bank_time;
	fix forward_thrust_time;

	fix time_held_down[CONTROL_NUM_BUTTONS];
	fix time_went_down[CONTROL_NUM_BUTTONS];
	ubyte count[CONTROL_NUM_BUTTONS];
	ubyte state[CONTROL_NUM_BUTTONS];
} control_info;


extern control_info Controls;
extern void controls_read_all();
extern void controls_init(void);

//set the cruise speed to zero
extern void reset_cruise(void);

void read_flying_controls( object * obj );

extern ubyte Controls_stopped;
extern ubyte Controls_always_move;

extern fix Afterburner_charge;

#endif
