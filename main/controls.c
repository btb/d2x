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
 * Code for controlling player movement
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "mono.h"
#include "key.h"
#include "joy.h"
#include "timer.h"
#include "error.h"
#include "inferno.h"
#include "args.h"
#include "gr.h"
#include "mouse.h"


//look at keyboard, mouse, joystick, CyberMan, whatever, and set 
//physics vars rotvel, velocity

fix Afterburner_charge=f1_0;

#define AFTERBURNER_USE_SECS	3				//use up in 3 seconds
#define DROP_DELTA_TIME			(f1_0/15)	//drop 3 per second

extern int Drop_afterburner_blob_flag;		//ugly hack

extern fix	Seismic_tremor_magnitude;

void read_flying_controls( object * obj )
{
	fix	forward_thrust_time;

	Assert(FrameTime > 0); 		//Get MATT if hit this!

// this section commented and moved to the bottom by WraithX
//	if (Player_is_dead) {
//		vm_vec_zero(&obj->mtype.phys_info.rotthrust);
//		vm_vec_zero(&obj->mtype.phys_info.thrust);
//		return;
//	}
// end of section to be moved.

	if ((obj->type!=OBJ_PLAYER) || (obj->id!=Player_num)) return;	//references to player_ship require that this obj be the player

	if (Guided_missile[Player_num] && Guided_missile[Player_num]->signature==Guided_missile_sig[Player_num]) {
		vms_angvec rotangs;
		vms_matrix rotmat,tempm;
		fix speed;

		//this is a horrible hack.  guided missile stuff should not be
		//handled in the middle of a routine that is dealing with the player

		vm_vec_zero(&obj->mtype.phys_info.rotthrust);

		rotangs.p = Controls.pitch_time / 2 + Seismic_tremor_magnitude/64;
		rotangs.b = Controls.bank_time / 2 + Seismic_tremor_magnitude/16;
		rotangs.h = Controls.heading_time / 2 + Seismic_tremor_magnitude/64;

		vm_angles_2_matrix(&rotmat,&rotangs);

		vm_matrix_x_matrix(&tempm,&Guided_missile[Player_num]->orient,&rotmat);

		Guided_missile[Player_num]->orient = tempm;

		speed = Weapon_info[Guided_missile[Player_num]->id].speed[Difficulty_level];

		vm_vec_copy_scale(&Guided_missile[Player_num]->mtype.phys_info.velocity,&Guided_missile[Player_num]->orient.fvec,speed);
#ifdef NETWORK
		if (Game_mode & GM_MULTI)
			multi_send_guided_info (Guided_missile[Player_num],0);
#endif

	}
	else {
		obj->mtype.phys_info.rotthrust.x = Controls.pitch_time;
		obj->mtype.phys_info.rotthrust.y = Controls.heading_time;
		obj->mtype.phys_info.rotthrust.z = Controls.bank_time;
	}

//	mprintf( (0, "Rot thrust = %.3f,%.3f,%.3f\n", f2fl(obj->mtype.phys_info.rotthrust.x),f2fl(obj->mtype.phys_info.rotthrust.y), f2fl(obj->mtype.phys_info.rotthrust.z) ));

	forward_thrust_time = Controls.forward_thrust_time;

	if (Players[Player_num].flags & PLAYER_FLAGS_AFTERBURNER)
	{
		if (Controls.state[afterburner]) { // player has key down
			//if (forward_thrust_time >= 0) { 		//..and isn't moving backward
			{
				fix afterburner_scale;
				int old_count,new_count;
	
				//add in value from 0..1
				afterburner_scale = f1_0 + min(f1_0/2,Afterburner_charge) * 2;
	
				forward_thrust_time = fixmul(FrameTime,afterburner_scale);	//based on full thrust
	
				old_count = (Afterburner_charge / (DROP_DELTA_TIME/AFTERBURNER_USE_SECS));

				Afterburner_charge -= FrameTime/AFTERBURNER_USE_SECS;

				if (Afterburner_charge < 0)
					Afterburner_charge = 0;

				new_count = (Afterburner_charge / (DROP_DELTA_TIME/AFTERBURNER_USE_SECS));

				if (old_count != new_count)
					Drop_afterburner_blob_flag = 1;	//drop blob (after physics called)
			}
		}
		else {
			fix cur_energy,charge_up;
	
			//charge up to full
			charge_up = min(FrameTime/8,f1_0 - Afterburner_charge);	//recharge over 8 seconds
	
			cur_energy = max(Players[Player_num].energy-i2f(10),0);	//don't drop below 10

			//maybe limit charge up by energy
			charge_up = min(charge_up,cur_energy/10);
	
			Afterburner_charge += charge_up;
	
			Players[Player_num].energy -= charge_up * 100 / 10;	//full charge uses 10% of energy
		}
	}

	// Set object's thrust vector for forward/backward
	vm_vec_copy_scale(&obj->mtype.phys_info.thrust,&obj->orient.fvec, forward_thrust_time );
	
	// slide left/right
	vm_vec_scale_add2(&obj->mtype.phys_info.thrust,&obj->orient.rvec, Controls.sideways_thrust_time );

	// slide up/down
	vm_vec_scale_add2(&obj->mtype.phys_info.thrust,&obj->orient.uvec, Controls.vertical_thrust_time );

	if (obj->mtype.phys_info.flags & PF_WIGGLE)
	{
		fix swiggle;
		fix_fastsincos(GameTime, &swiggle, NULL);
		if (FrameTime < F1_0) // Only scale wiggle if getting at least 1 FPS, to avoid causing the opposite problem.
			swiggle = fixmul(swiggle*20, FrameTime); //make wiggle fps-independent (based on pre-scaled amount of wiggle at 20 FPS)
		vm_vec_scale_add2(&obj->mtype.phys_info.velocity,&obj->orient.uvec,fixmul(swiggle,Player_ship->wiggle));
	}

	// As of now, obj->mtype.phys_info.thrust & obj->mtype.phys_info.rotthrust are 
	// in units of time... In other words, if thrust==FrameTime, that
	// means that the user was holding down the Max_thrust key for the
	// whole frame.  So we just scale them up by the max, and divide by
	// FrameTime to make them independant of framerate

	//	Prevent divide overflows on high frame rates.
	//	In a signed divide, you get an overflow if num >= div<<15
	{
		fix	ft = FrameTime;

		//	Note, you must check for ft < F1_0/2, else you can get an overflow  on the << 15.
		if ((ft < F1_0/2) && (ft << 15 <= Player_ship->max_thrust)) {
			mprintf((0, "Preventing divide overflow in controls.c for Max_thrust!\n"));
			ft = (Player_ship->max_thrust >> 15) + 1;
		}

		vm_vec_scale( &obj->mtype.phys_info.thrust, fixdiv(Player_ship->max_thrust,ft) );

		if ((ft < F1_0/2) && (ft << 15 <= Player_ship->max_rotthrust)) {
			mprintf((0, "Preventing divide overflow in controls.c for max_rotthrust!\n"));
			ft = (Player_ship->max_thrust >> 15) + 1;
		}

		vm_vec_scale( &obj->mtype.phys_info.rotthrust, fixdiv(Player_ship->max_rotthrust,ft) );
	}

	// moved here by WraithX
	if (Player_is_dead)
	{
		//vm_vec_zero(&obj->mtype.phys_info.rotthrust); // let dead players rotate, changed by WraithX
		vm_vec_zero(&obj->mtype.phys_info.thrust);  // don't let dead players move, changed by WraithX
		return;
	}// end if

}


extern void transfer_energy_to_shield(fix);

control_info Controls;

fix Cruise_speed = 0;

#define	PH_SCALE 8

#ifndef __MSDOS__
#define	JOYSTICK_READ_TIME (F1_0 / 40) // Read joystick at 40 Hz.
#else
#define	JOYSTICK_READ_TIME (F1_0 / 10) // Read joystick at 10 Hz.
#endif

fix	LastReadTime = 0;

fix	joy_axis[JOY_MAX_AXES];


void reset_cruise(void)
{
	Cruise_speed = 0;
}


static inline void button_down(control_button button)
{
	Controls.count[button]++;
	Controls.state[button] = 1;
	Controls.time_went_down[button] = timer_get_fixed_seconds();
}


static inline void button_up(control_button button)
{
	Controls.state[button] = 0;
	Controls.time_held_down[button] += timer_get_fixed_seconds() - Controls.time_went_down[button];
}


// Returns the number of seconds this 'button' has been down since last call.
static inline fix button_down_time(control_button button)
{
	fix time_down, time;

	if (!Controls.state[button]) {
		time_down = Controls.time_held_down[button];
		Controls.time_held_down[button] = 0;
	} else {
		time = timer_get_fixed_seconds();
		time_down = time - Controls.time_went_down[button];
		Controls.time_went_down[button] = time;
	}

	return time_down;
}


void controls_cmd_lookdown_on(int argc, char **argv)   { button_down(pitch_forward); }
void controls_cmd_lookdown_off(int argc, char **argv)  { button_up(pitch_forward); }
void controls_cmd_lookup_on(int argc, char **argv)     { button_down(pitch_backward); }
void controls_cmd_lookup_off(int argc, char **argv)    { button_up(pitch_backward); }
void controls_cmd_left_on(int argc, char **argv)       { button_down(turn_left); }
void controls_cmd_left_off(int argc, char **argv)      { button_up(turn_left); }
void controls_cmd_right_on(int argc, char **argv)      { button_down(turn_right); }
void controls_cmd_right_off(int argc, char **argv)     { button_up(turn_right); }
void controls_cmd_strafe_on(int argc, char **argv)     { button_down(slide_on); }
void controls_cmd_strafe_off(int argc, char **argv)    { button_up(slide_on); }
void controls_cmd_moveleft_on(int argc, char **argv)   { button_down(slide_left); }
void controls_cmd_moveleft_off(int argc, char **argv)  { button_up(slide_left); }
void controls_cmd_moveright_on(int argc, char **argv)  { button_down(slide_right); }
void controls_cmd_moveright_off(int argc, char **argv) { button_up(slide_right); }
void controls_cmd_moveup_on(int argc, char **argv)     { button_down(slide_up); }
void controls_cmd_moveup_off(int argc, char **argv)    { button_up(slide_up); }
void controls_cmd_movedown_on(int argc, char **argv)   { button_down(slide_down); }
void controls_cmd_movedown_off(int argc, char **argv)  { button_up(slide_down); }
void controls_cmd_bank_on(int argc, char **argv)       { button_down(bank_on); }
void controls_cmd_bank_off(int argc, char **argv)      { button_up(bank_on); }
void controls_cmd_bankleft_on(int argc, char **argv)   { button_down(bank_left); }
void controls_cmd_bankleft_off(int argc, char **argv)  { button_up(bank_left); }
void controls_cmd_bankright_on(int argc, char **argv)  { button_down(bank_right); }
void controls_cmd_bankright_off(int argc, char **argv) { button_up(bank_right); }
void controls_cmd_forward_on(int argc, char **argv)    { button_down(accelerate); }
void controls_cmd_forward_off(int argc, char **argv)   { button_up(accelerate); }
void controls_cmd_back_on(int argc, char **argv)       { button_down(reverse); }
void controls_cmd_back_off(int argc, char **argv)      { button_up(reverse); }
void controls_cmd_cruiseup_on(int argc, char **argv)   { button_down(cruise_faster); }
void controls_cmd_cruiseup_off(int argc, char **argv)  { button_up(cruise_faster); }
void controls_cmd_cruisedown_on(int argc, char **argv) { button_down(cruise_slower); }
void controls_cmd_cruisedown_off(int argc, char **argv){ button_up(cruise_slower); }
void controls_cmd_cruiseoff_on(int argc, char **argv)  { button_down(cruise_off); }
void controls_cmd_cruiseoff_off(int argc, char **argv) { button_up(cruise_off); }
void controls_cmd_nrgshield_on(int argc, char **argv)  { button_down(energy_shield); }
void controls_cmd_nrgshield_off(int argc, char **argv) { button_up(energy_shield); }
void controls_cmd_attack_on(int argc, char **argv)     { button_down(fire_primary); }
void controls_cmd_attack_off(int argc, char **argv)    { button_up(fire_primary); }
void controls_cmd_attack2_on(int argc, char **argv)    { button_down(fire_secondary); }
void controls_cmd_attack2_off(int argc, char **argv)   { button_up(fire_secondary); }
void controls_cmd_rearview_on(int argc, char **argv)   { button_down(rear_view); }
void controls_cmd_rearview_off(int argc, char **argv)  { button_up(rear_view); }
void controls_cmd_automap_on(int argc, char **argv)    { button_down(automap); }
void controls_cmd_automap_off(int argc, char **argv)   { button_up(automap); }
void controls_cmd_afterburn_on(int argc, char **argv)  { button_down(afterburner); }
void controls_cmd_afterburn_off(int argc, char **argv) { button_up(afterburner); }
void controls_cmd_flare(int argc, char **argv)         { button_down(fire_flare); }
void controls_cmd_bomb(int argc, char **argv)          { button_up(drop_bomb); }
void controls_cmd_cycle(int argc, char **argv)         { button_down(cycle_primary); }
void controls_cmd_cycle2(int argc, char **argv)        { button_up(cycle_secondary); }
void controls_cmd_headlight(int argc, char **argv)     { button_down(headlight); }

void controls_cmd_togglebomb(int argc, char **argv)
{
	weapon_toggle_bomb();
}

void controls_cmd_weapon(int argc, char **argv)
{
	int n;

	if (argc < 2 || !stricmp(argv[1], "-h")) {
		con_printf(CON_NORMAL, "%s <num>\n", argv[0]);
		con_printf(CON_NORMAL, "    select or toggle weapon <num>\n");
		return;
	}

	n = atoi(argv[1]);
	if (n == 0)
		n = 10;
	if (n < 1 || n > 10)
		return;

	do_weapon_select((n-1) % 5, (n-1) / 5);
}


void controls_init(void)
{
	cmd_addcommand("+lookdown",    controls_cmd_lookdown_on, NULL);
	cmd_addcommand("-lookdown",    controls_cmd_lookdown_off, NULL);
	cmd_addcommand("+lookup",      controls_cmd_lookup_on, NULL);
	cmd_addcommand("-lookup",      controls_cmd_lookup_off, NULL);
	cmd_addcommand("+left",        controls_cmd_left_on, NULL);
	cmd_addcommand("-left",        controls_cmd_left_off, NULL);
	cmd_addcommand("+right",       controls_cmd_right_on, NULL);
	cmd_addcommand("-right",       controls_cmd_right_off, NULL);
	cmd_addcommand("+strafe",      controls_cmd_strafe_on, NULL);
	cmd_addcommand("-strafe",      controls_cmd_strafe_off, NULL);
	cmd_addcommand("+moveleft",    controls_cmd_moveleft_on, NULL);
	cmd_addcommand("-moveleft",    controls_cmd_moveleft_off, NULL);
	cmd_addcommand("+moveright",   controls_cmd_moveright_on, NULL);
	cmd_addcommand("-moveright",   controls_cmd_moveright_off, NULL);
	cmd_addcommand("+moveup",      controls_cmd_moveup_on, NULL);
	cmd_addcommand("-moveup",      controls_cmd_moveup_off, NULL);
	cmd_addcommand("+movedown",    controls_cmd_movedown_on, NULL);
	cmd_addcommand("-movedown",    controls_cmd_movedown_off, NULL);
	cmd_addcommand("+bank",        controls_cmd_bank_on, NULL);
	cmd_addcommand("-bank",        controls_cmd_bank_off, NULL);
	cmd_addcommand("+bankleft",    controls_cmd_bankleft_on, NULL);
	cmd_addcommand("-bankleft",    controls_cmd_bankleft_off, NULL);
	cmd_addcommand("+bankright",   controls_cmd_bankright_on, NULL);
	cmd_addcommand("-bankright",   controls_cmd_bankright_off, NULL);
	cmd_addcommand("+forward",     controls_cmd_forward_on, NULL);
	cmd_addcommand("-forward",     controls_cmd_forward_off, NULL);
	cmd_addcommand("+back",        controls_cmd_back_on, NULL);
	cmd_addcommand("-back",        controls_cmd_back_off, NULL);
	cmd_addcommand("+cruiseup",    controls_cmd_cruiseup_on, NULL);
	cmd_addcommand("-cruiseup",    controls_cmd_cruiseup_off, NULL);
	cmd_addcommand("+cruisedown",  controls_cmd_cruisedown_on, NULL);
	cmd_addcommand("-cruisedown",  controls_cmd_cruisedown_off, NULL);
	cmd_addcommand("+cruiseoff",   controls_cmd_cruiseoff_on, NULL);
	cmd_addcommand("-cruiseoff",   controls_cmd_cruiseoff_off, NULL);
	cmd_addcommand("+nrgshield",   controls_cmd_nrgshield_on, NULL);
	cmd_addcommand("-nrgshield",   controls_cmd_nrgshield_off, NULL);
	cmd_addcommand("+rearview",    controls_cmd_rearview_on, NULL);
	cmd_addcommand("-rearview",    controls_cmd_rearview_off, NULL);
	cmd_addcommand("+attack",      controls_cmd_attack_on, NULL);
	cmd_addcommand("-attack",      controls_cmd_attack_off, NULL);
	cmd_addcommand("+attack2",     controls_cmd_attack2_on, NULL);
	cmd_addcommand("-attack2",     controls_cmd_attack2_off, NULL);
	cmd_addcommand("+automap",     controls_cmd_automap_on, NULL);
	cmd_addcommand("-automap",     controls_cmd_automap_off, NULL);
	cmd_addcommand("+afterburner", controls_cmd_afterburn_on, NULL);
	cmd_addcommand("-afterburner", controls_cmd_afterburn_off, NULL);
	cmd_addcommand("flare",        controls_cmd_flare, NULL);
	cmd_addcommand("bomb",         controls_cmd_bomb, NULL);
	cmd_addcommand("cycle",        controls_cmd_cycle, NULL);
	cmd_addcommand("cycle2",       controls_cmd_cycle2, NULL);
	cmd_addcommand("headlight",    controls_cmd_headlight, NULL);
	cmd_addcommand("togglebomb",   controls_cmd_togglebomb, NULL);
	cmd_addcommand("weapon",       controls_cmd_weapon, NULL);
}


/* Preserves pitch, heading, and states */
void controls_reset(void)
{
	Controls.forward_thrust_time = 0;
	Controls.sideways_thrust_time = 0;
	Controls.vertical_thrust_time = 0;
	Controls.bank_time = 0;
	memset(Controls.count, 0, CONTROL_NUM_BUTTONS);
}


void controls_read_all()
{
	int i;
	int dx, dy, dz;
	fix ctime;
	int raw_joy_axis[JOY_MAX_AXES];
	fix kp, kh;
	ubyte channel_masks;
	fix analog_control[7]; // indexed on control_analog

	memset(analog_control, 0, sizeof(analog_control));

	controls_reset();

	cmd_queue_process();

	ctime = timer_get_fixed_seconds();

	//---------  Read Joystick -----------
	if ( (LastReadTime + JOYSTICK_READ_TIME > ctime) ) {
#ifndef __MSDOS__
		if ((ctime < 0) && (LastReadTime >= 0))
#else
			if ((ctime < 0) && (LastReadTime > 0))
#endif
				LastReadTime = ctime;
	} else if (Config_control_joystick.intval) {
		LastReadTime = ctime;
		channel_masks = joystick_read_raw_axis( JOY_ALL_AXIS, raw_joy_axis );

		Assert(joy_num_axes <= 6); // don't have cvar mapping above 6 yet
		for (i = 0; i < joy_num_axes; i++) {
#ifndef SDL_INPUT
			if (channel_masks&(1<<i)) {
#endif
				int joy_null_value = f2i(Config_joystick_deadzone[joy_advaxes[i].intval - 1].intval * 128);

				raw_joy_axis[i] = joy_get_scaled_reading( raw_joy_axis[i], i );

				if (raw_joy_axis[i] > joy_null_value)
					raw_joy_axis[i] = ((raw_joy_axis[i] - joy_null_value) * 128) / (128 - joy_null_value);
				else if (raw_joy_axis[i] < -joy_null_value)
					raw_joy_axis[i] = ((raw_joy_axis[i] + joy_null_value) * 128) / (128 - joy_null_value);
				else
					raw_joy_axis[i] = 0;
				joy_axis[i]	= (raw_joy_axis[i] * FrameTime) / 128;
#ifndef SDL_INPUT
			} else {
				joy_axis[i] = 0;
			}
#endif
		}
	} else {
		for (i = 0; i < joy_num_axes; i++)
			joy_axis[i] = 0;
	}

	if (Config_control_joystick.intval)
		for (i = 0; i < 6; i++)
			analog_control[joy_advaxes[i].intval] += joy_axis[i] * (joy_invert[i].intval ? -1 : 1) * Config_joystick_sensitivity[joy_advaxes[i].intval-1].value;

	if (Config_control_mouse.intval) {
		//---------  Read Mouse -----------
		mouse_get_delta( &dx, &dy, &dz );

		analog_control[mouse_axes[0].intval] += dx * FrameTime / 35 * (mouse_invert[0].intval ? -1 : 1) * Config_mouse_sensitivity[mouse_axes[0].intval-1].value;
		analog_control[mouse_axes[1].intval] += dy * FrameTime / 25 * (mouse_invert[1].intval ? -1 : 1) * Config_mouse_sensitivity[mouse_axes[1].intval-1].value;
		analog_control[mouse_axes[2].intval] += dz * FrameTime      * (mouse_invert[2].intval ? -1 : 1) * Config_mouse_sensitivity[mouse_axes[2].intval-1].value;
	}

	//------------ Read pitch_time -----------
	if ( !Controls.state[slide_on] ) {
		// mprintf((0, "pitch: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kp = 0;

		kp += button_down_time(pitch_forward) / (PH_SCALE * 2);
		kp -= button_down_time(pitch_backward) / (PH_SCALE * 2);

		if (kp == 0)
			Controls.pitch_time = 0;
		else if (kp > 0) {
			if (Controls.pitch_time < 0)
				Controls.pitch_time = 0;
		} else // kp < 0
			if (Controls.pitch_time > 0)
				Controls.pitch_time = 0;
		Controls.pitch_time += kp;

		Controls.pitch_time -= analog_control[AXIS_PITCH];

	} else
		Controls.pitch_time = 0;

	if (!Player_is_dead) {

		//----------- Read vertical_thrust_time -----------------

		if ( Controls.state[slide_on] ) {
			Controls.vertical_thrust_time += button_down_time(pitch_forward);
			Controls.vertical_thrust_time -= button_down_time(pitch_backward);
			Controls.vertical_thrust_time += analog_control[AXIS_PITCH];
		}

		Controls.vertical_thrust_time += button_down_time(slide_up);
		Controls.vertical_thrust_time -= button_down_time(slide_down);
		Controls.vertical_thrust_time += analog_control[AXIS_UPDOWN];

	}

	//---------- Read heading_time -----------

	if ( !Controls.state[slide_on] && !Controls.state[bank_on] ) {
		//mprintf((0, "heading: %7.3f %7.3f: %7.3f\n", f2fl(k4), f2fl(k6), f2fl(Controls.heading_time)));
		kh = 0;

		kh -= button_down_time(turn_left) / PH_SCALE;
		kh += button_down_time(turn_right) / PH_SCALE;

		if (kh == 0)
			Controls.heading_time = 0;
		else if (kh > 0) {
			if (Controls.heading_time < 0)
				Controls.heading_time = 0;
		} else // kh < 0
			if (Controls.heading_time > 0)
				Controls.heading_time = 0;
		Controls.heading_time += kh;

		Controls.heading_time += analog_control[AXIS_TURN];

	} else
		Controls.heading_time = 0;

	if (!Player_is_dead) {

		//----------- Read sideways_thrust_time -----------------

		if ( Controls.state[slide_on] ) {
			Controls.sideways_thrust_time -= button_down_time(turn_left);
			Controls.sideways_thrust_time += button_down_time(turn_right);
			Controls.sideways_thrust_time += analog_control[AXIS_TURN];
		}

		Controls.sideways_thrust_time -= button_down_time(slide_left);
		Controls.sideways_thrust_time += button_down_time(slide_right);
		Controls.sideways_thrust_time += analog_control[AXIS_LEFTRIGHT];

	}

	//----------- Read bank_time -----------------

	if ( Controls.state[bank_on] ) {
		Controls.bank_time += button_down_time(turn_left);
		Controls.bank_time -= button_down_time(turn_right);
		Controls.bank_time -= analog_control[AXIS_TURN];
	}

	Controls.bank_time += button_down_time(bank_left);
	Controls.bank_time -= button_down_time(bank_right);
	Controls.bank_time -= analog_control[AXIS_BANK];

	// the following "if" added by WraithX, 4/14/00
	// done so that dead players can't move
	if (!Player_is_dead) {

		//----------- Read forward_thrust_time -------------

		Controls.forward_thrust_time += button_down_time(accelerate);
		Controls.forward_thrust_time -= button_down_time(reverse);
		Controls.forward_thrust_time -= analog_control[AXIS_THROTTLE];

		//---------Read Energy->Shield key----------

		if ((Players[Player_num].flags & PLAYER_FLAGS_CONVERTER) && Controls.state[energy_shield])
			transfer_energy_to_shield(button_down_time(energy_shield));

	}

	//----------- Read stupid-cruise-control-type of throttle.

	Cruise_speed += button_down_time(cruise_faster);
	Cruise_speed -= button_down_time(cruise_slower);

	if (Controls.count[cruise_off])
		Cruise_speed = 0;

	if (Cruise_speed > i2f(100))
		Cruise_speed = i2f(100);
	if (Cruise_speed < 0)
		Cruise_speed = 0;

	if (Controls.forward_thrust_time == 0)
		Controls.forward_thrust_time = fixmul(Cruise_speed,FrameTime) / 100;

#if 0
	read_head_tracker();
#endif

	//----------- Clamp values between -FrameTime and FrameTime
	if (FrameTime > F1_0 )
		mprintf( (1, "Bogus frame time of %.2f seconds\n", f2fl(FrameTime) ));

	if (Controls.pitch_time         > FrameTime/2 ) Controls.pitch_time         = FrameTime/2;
	if (Controls.vertical_thrust_time > FrameTime ) Controls.vertical_thrust_time = FrameTime;
	if (Controls.heading_time         > FrameTime ) Controls.heading_time         = FrameTime;
	if (Controls.sideways_thrust_time > FrameTime ) Controls.sideways_thrust_time = FrameTime;
	if (Controls.bank_time            > FrameTime ) Controls.bank_time            = FrameTime;
	if (Controls.forward_thrust_time  > FrameTime ) Controls.forward_thrust_time  = FrameTime;
	//if (Controls.afterburner_time   > FrameTime ) Controls.afterburner_time     = FrameTime;

	if (Controls.pitch_time         < -FrameTime/2 ) Controls.pitch_time         = -FrameTime/2;
	if (Controls.vertical_thrust_time < -FrameTime ) Controls.vertical_thrust_time = -FrameTime;
	if (Controls.heading_time         < -FrameTime ) Controls.heading_time         = -FrameTime;
	if (Controls.sideways_thrust_time < -FrameTime ) Controls.sideways_thrust_time = -FrameTime;
	if (Controls.bank_time            < -FrameTime ) Controls.bank_time            = -FrameTime;
	if (Controls.forward_thrust_time  < -FrameTime ) Controls.forward_thrust_time  = -FrameTime;
	//if (Controls.afterburner_time   < -FrameTime ) Controls.afterburner_time     = -FrameTime;

	//--------- Don't do anything if in debug mode
#ifndef RELEASE
	if ( keyd_pressed[KEY_DELETE] )	{
		memset( &Controls, 0, sizeof(control_info) );
	}
#endif
}
