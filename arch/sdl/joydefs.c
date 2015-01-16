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
	int oc_type = Config_control_type.intval;

	cvar_setint(&Config_control_type, items[0].value | (items[1].value << 1));

	if (oc_type != Config_control_type.intval) {
		if (Config_control_type.intval&CONTROL_USING_JOYSTICK)
				joydefs_calibrate_flag = 1;
		kc_set_controls();
	}
}

void joydefs_config()
{
	newmenu_item m[13];
	int i1 = 3;
	int nitems = 9;

	m[0].type = NM_TYPE_CHECK;  m[0].text = TXT_CONTROL_JOYSTICK; m[0].value = Config_control_type.intval&CONTROL_USING_JOYSTICK;
	m[1].type = NM_TYPE_CHECK;  m[1].text = TXT_CONTROL_MOUSE;    m[1].value = Config_control_type.intval&CONTROL_USING_MOUSE >> 1;
	m[2].type = NM_TYPE_TEXT;   m[2].text = "";
	m[3].type = NM_TYPE_MENU;   m[3].text = TXT_CUST_KEYBOARD;
	m[4].type = NM_TYPE_MENU;   m[4].text = "CUSTOMIZE ANALOG CONTROLS";
	m[5].type = NM_TYPE_MENU;   m[5].text = "CUSTOMIZE D2X KEYS";
	m[6].type = NM_TYPE_TEXT;   m[6].text = "";
	m[7].type = NM_TYPE_SLIDER; m[7].text = TXT_JOYS_SENSITIVITY; m[7].value = Config_joystick_sensitivity.intval; m[7].min_value = 0; m[7].max_value = 16;
	m[8].type = NM_TYPE_TEXT;   m[8].text = "";

	do {
		i1 = newmenu_do1(NULL, TXT_CONTROLS, nitems, m, joydef_menuset_1, i1);

		cvar_setint(&Config_control_type, m[0].value | (m[1].value << 1));
		cvar_setint(&Config_joystick_sensitivity, m[7].value);

		switch (i1) {
		case 3: kconfig(0, TXT_KEYBOARD); break;
		case 4: kconfig(1, TXT_AXES); break;
		case 5: kconfig(2, "D2X KEYS"); break;
		}

	} while (i1>-1);

}
