/*
 *
 * SDL joystick support
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include "joydefs.h"
#include "pstypes.h"
#include "newmenu.h"
#include "inferno.h"
#include "text.h"
#include "kconfig.h"

extern int num_joysticks;

int joydefs_calibrate_flag = 0;

void joydefs_calibrate()
{
	joydefs_calibrate_flag = 0;

	if (!num_joysticks) {
		nm_messagebox( NULL, 1, TXT_OK, TXT_NO_JOYSTICK );
		return;
	}

	//Actual calibration if necessary

}

void joydef_menuset_1(int nitems, newmenu_item * items, int *last_key, int citem )
{
	int oc_joy = Config_control_joystick.intval;
	int oc_mouse = Config_control_mouse.intval;

	cvar_setint(&Config_control_joystick, items[0].value);
	cvar_setint(&Config_control_mouse, items[1].value);

	if (!oc_joy && Config_control_joystick.intval)
		joydefs_calibrate_flag = 1;

	if (oc_joy != Config_control_joystick.intval || oc_mouse != Config_control_mouse.intval)
		kc_set_controls();
}


#define ADD_SLIDER(txt,v,minv,maxv) do { m[nitems].type=NM_TYPE_SLIDER; m[nitems].text=txt; m[nitems].value=v; m[nitems].min_value=minv; m[nitems].max_value=maxv; nitems++; } while (0)

void joydefs_sensitivity(void)
{
	newmenu_item m[23];
	int i1 = 0;
	int nitems = 0;

	m[nitems].type = NM_TYPE_TEXT; m[nitems].text = "Joystick Sensitivity"; nitems++;
	ADD_SLIDER(TXT_TURN_LR,  f2i(8 * Config_joystick_sensitivity[AXIS_TURN-1].value), 0, 16);
	ADD_SLIDER(TXT_PITCH_UD, f2i(8 * Config_joystick_sensitivity[AXIS_PITCH-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_LR, f2i(8 * Config_joystick_sensitivity[AXIS_LEFTRIGHT-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_UD, f2i(8 * Config_joystick_sensitivity[AXIS_UPDOWN-1].value), 0, 16);
	ADD_SLIDER(TXT_BANK_LR,  f2i(8 * Config_joystick_sensitivity[AXIS_BANK-1].value), 0, 16);
	ADD_SLIDER(TXT_THROTTLE, f2i(8 * Config_joystick_sensitivity[AXIS_THROTTLE-1].value), 0, 16);

	m[nitems].type = NM_TYPE_TEXT; m[nitems].text = ""; nitems++;
	m[nitems].type = NM_TYPE_TEXT; m[nitems].text = "Joystick Deadzone"; nitems++;
	ADD_SLIDER(TXT_TURN_LR,  f2i(8 * Config_joystick_deadzone[AXIS_TURN-1].value), 0, 16);
	ADD_SLIDER(TXT_PITCH_UD, f2i(8 * Config_joystick_deadzone[AXIS_PITCH-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_LR, f2i(8 * Config_joystick_deadzone[AXIS_LEFTRIGHT-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_UD, f2i(8 * Config_joystick_deadzone[AXIS_UPDOWN-1].value), 0, 16);
	ADD_SLIDER(TXT_BANK_LR,  f2i(8 * Config_joystick_deadzone[AXIS_BANK-1].value), 0, 16);
	ADD_SLIDER(TXT_THROTTLE, f2i(8 * Config_joystick_deadzone[AXIS_THROTTLE-1].value), 0, 16);

	m[nitems].type = NM_TYPE_TEXT; m[nitems].text = ""; nitems++;
	m[nitems].type = NM_TYPE_TEXT; m[nitems].text = "Mouse Sensitivity"; nitems++;
	ADD_SLIDER(TXT_TURN_LR,  f2i(8 * Config_mouse_sensitivity[AXIS_TURN-1].value), 0, 16);
	ADD_SLIDER(TXT_PITCH_UD, f2i(8 * Config_mouse_sensitivity[AXIS_PITCH-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_LR, f2i(8 * Config_mouse_sensitivity[AXIS_LEFTRIGHT-1].value), 0, 16);
	ADD_SLIDER(TXT_SLIDE_UD, f2i(8 * Config_mouse_sensitivity[AXIS_UPDOWN-1].value), 0, 16);
	ADD_SLIDER(TXT_BANK_LR,  f2i(8 * Config_mouse_sensitivity[AXIS_BANK-1].value), 0, 16);
	ADD_SLIDER(TXT_THROTTLE, f2i(8 * Config_mouse_sensitivity[AXIS_THROTTLE-1].value), 0, 16);

	do
		i1 = newmenu_do1(NULL, "Sensitivity", nitems, m, NULL, i1);
	while ( i1 > -1 );

	cvar_setfl(&Config_joystick_sensitivity[AXIS_TURN-1],      m[1].value / 8.0);
	cvar_setfl(&Config_joystick_sensitivity[AXIS_PITCH-1],     m[2].value / 8.0);
	cvar_setfl(&Config_joystick_sensitivity[AXIS_LEFTRIGHT-1], m[3].value / 8.0);
	cvar_setfl(&Config_joystick_sensitivity[AXIS_UPDOWN-1],    m[4].value / 8.0);
	cvar_setfl(&Config_joystick_sensitivity[AXIS_BANK-1],      m[5].value / 8.0);
	cvar_setfl(&Config_joystick_sensitivity[AXIS_THROTTLE-1],  m[6].value / 8.0);

	cvar_setfl(&Config_joystick_deadzone[AXIS_TURN-1],      m[ 9].value / 16.0);
	cvar_setfl(&Config_joystick_deadzone[AXIS_PITCH-1],     m[10].value / 16.0);
	cvar_setfl(&Config_joystick_deadzone[AXIS_LEFTRIGHT-1], m[11].value / 16.0);
	cvar_setfl(&Config_joystick_deadzone[AXIS_UPDOWN-1],    m[12].value / 16.0);
	cvar_setfl(&Config_joystick_deadzone[AXIS_BANK-1],      m[13].value / 16.0);
	cvar_setfl(&Config_joystick_deadzone[AXIS_THROTTLE-1],  m[14].value / 16.0);

	cvar_setfl(&Config_mouse_sensitivity[AXIS_TURN-1],      m[17].value / 8.0);
	cvar_setfl(&Config_mouse_sensitivity[AXIS_PITCH-1],     m[18].value / 8.0);
	cvar_setfl(&Config_mouse_sensitivity[AXIS_LEFTRIGHT-1], m[19].value / 8.0);
	cvar_setfl(&Config_mouse_sensitivity[AXIS_UPDOWN-1],    m[20].value / 8.0);
	cvar_setfl(&Config_mouse_sensitivity[AXIS_BANK-1],      m[21].value / 8.0);
	cvar_setfl(&Config_mouse_sensitivity[AXIS_THROTTLE-1],  m[22].value / 8.0);
}


void joydefs_config()
{
	newmenu_item m[9];
	int i1 = 3;
	int nitems = 9;

	m[0].type = NM_TYPE_CHECK;  m[0].text = TXT_CONTROL_JOYSTICK; m[0].value = Config_control_joystick.intval;
	m[1].type = NM_TYPE_CHECK;  m[1].text = TXT_CONTROL_MOUSE;    m[1].value = Config_control_mouse.intval;
	m[2].type = NM_TYPE_TEXT;   m[2].text = "";
	m[3].type = NM_TYPE_MENU;   m[3].text = TXT_CUST_KEYBOARD;
	m[4].type = NM_TYPE_MENU;   m[4].text = "CUSTOMIZE ANALOG CONTROLS";
	m[5].type = NM_TYPE_MENU;   m[5].text = "CUSTOMIZE D2X KEYS";
	m[6].type = NM_TYPE_TEXT;   m[6].text = "";
	m[7].type = NM_TYPE_MENU;   m[7].text = TXT_JOYS_SENSITIVITY;
	m[8].type = NM_TYPE_TEXT;   m[8].text = "";

	do {
		i1 = newmenu_do1(NULL, TXT_CONTROLS, nitems, m, joydef_menuset_1, i1);

		cvar_setint(&Config_control_joystick, m[0].value);
		cvar_setint(&Config_control_mouse, m[1].value);

		switch (i1) {
		case 3: kconfig(0, TXT_KEYBOARD); break;
		case 4: kconfig(1, TXT_AXES); break;
		case 5: kconfig(2, "D2X KEYS"); break;
		case 7: joydefs_sensitivity();
		}

	} while (i1>-1);

}
