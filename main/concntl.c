#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>

#include "console.h"
#include "timer.h"
#include "weapon.h"


typedef struct _console_control_info {
	ubyte state;
	fix   time_went_down;
	fix   time_held_down;
	uint  down_count;
	uint  up_count;
} console_control_info;

console_control_info Console_controls[CONCNTL_NUM_CONTROLS];

void concntl_cmd_lookdown_on(int argc, char **argv)     { Console_controls[CONCNTL_LOOKDOWN].state   = 1; }
void concntl_cmd_lookdown_off(int argc, char **argv)    { Console_controls[CONCNTL_LOOKDOWN].state   = 0; }
void concntl_cmd_lookup_on(int argc, char **argv)       { Console_controls[CONCNTL_LOOKUP].state     = 1; }
void concntl_cmd_lookup_off(int argc, char **argv)      { Console_controls[CONCNTL_LOOKUP].state     = 0; }
void concntl_cmd_left_on(int argc, char **argv)         { Console_controls[CONCNTL_LEFT].state       = 1; }
void concntl_cmd_left_off(int argc, char **argv)        { Console_controls[CONCNTL_LEFT].state       = 0; }
void concntl_cmd_right_on(int argc, char **argv)        { Console_controls[CONCNTL_RIGHT].state      = 1; }
void concntl_cmd_right_off(int argc, char **argv)       { Console_controls[CONCNTL_RIGHT].state      = 0; }
void concntl_cmd_strafe_on(int argc, char **argv)       { Console_controls[CONCNTL_STRAFE].state     = 1; }
void concntl_cmd_strafe_off(int argc, char **argv)      { Console_controls[CONCNTL_STRAFE].state     = 0; }
void concntl_cmd_moveleft_on(int argc, char **argv)     { Console_controls[CONCNTL_MOVELEFT].state   = 1; }
void concntl_cmd_moveleft_off(int argc, char **argv)    { Console_controls[CONCNTL_MOVELEFT].state   = 0; }
void concntl_cmd_moveright_on(int argc, char **argv)    { Console_controls[CONCNTL_MOVERIGHT].state  = 1; }
void concntl_cmd_moveright_off(int argc, char **argv)   { Console_controls[CONCNTL_MOVERIGHT].state  = 0; }
void concntl_cmd_moveup_on(int argc, char **argv)       { Console_controls[CONCNTL_MOVEUP].state     = 1; }
void concntl_cmd_moveup_off(int argc, char **argv)      { Console_controls[CONCNTL_MOVEUP].state     = 0; }
void concntl_cmd_movedown_on(int argc, char **argv)     { Console_controls[CONCNTL_MOVEDOWN].state   = 1; }
void concntl_cmd_movedown_off(int argc, char **argv)    { Console_controls[CONCNTL_MOVEDOWN].state   = 0; }
void concntl_cmd_bank_on(int argc, char **argv)         { Console_controls[CONCNTL_BANK].state       = 1; }
void concntl_cmd_bank_off(int argc, char **argv)        { Console_controls[CONCNTL_BANK].state       = 0; }
void concntl_cmd_bankleft_on(int argc, char **argv)     { Console_controls[CONCNTL_BANKLEFT].state   = 1; }
void concntl_cmd_bankleft_off(int argc, char **argv)    { Console_controls[CONCNTL_BANKLEFT].state   = 0; }
void concntl_cmd_bankright_on(int argc, char **argv)    { Console_controls[CONCNTL_BANKRIGHT].state  = 1; }
void concntl_cmd_bankright_off(int argc, char **argv)   { Console_controls[CONCNTL_BANKRIGHT].state  = 0; }
void concntl_cmd_attack_on(int argc, char **argv)       { Console_controls[CONCNTL_ATTACK].state     = 1; }
void concntl_cmd_attack_off(int argc, char **argv)      { Console_controls[CONCNTL_ATTACK].state     = 0; }
void concntl_cmd_attack2_on(int argc, char **argv)      { Console_controls[CONCNTL_ATTACK2].state    = 1; }
void concntl_cmd_attack2_off(int argc, char **argv)     { Console_controls[CONCNTL_ATTACK2].state    = 0; }
void concntl_cmd_flare_on(int argc, char **argv)        { Console_controls[CONCNTL_FLARE].state      = 1; }
void concntl_cmd_flare_off(int argc, char **argv)       { Console_controls[CONCNTL_FLARE].state      = 0; }
void concntl_cmd_forward_on(int argc, char **argv)      { Console_controls[CONCNTL_FORWARD].state    = 1; }
void concntl_cmd_forward_off(int argc, char **argv)     { Console_controls[CONCNTL_FORWARD].state    = 0; }
void concntl_cmd_back_on(int argc, char **argv)         { Console_controls[CONCNTL_BACK].state       = 1; }
void concntl_cmd_back_off(int argc, char **argv)        { Console_controls[CONCNTL_BACK].state       = 0; }
void concntl_cmd_bomb_on(int argc, char **argv)         { Console_controls[CONCNTL_BOMB].state       = 1; }
void concntl_cmd_bomb_off(int argc, char **argv)        { Console_controls[CONCNTL_BOMB].state       = 0; }
void concntl_cmd_rearview_on(int argc, char **argv)     { Console_controls[CONCNTL_REARVIEW].state   = 1; }
void concntl_cmd_rearview_off(int argc, char **argv)    { Console_controls[CONCNTL_REARVIEW].state   = 0; }
void concntl_cmd_cruiseup_on(int argc, char **argv)     { Console_controls[CONCNTL_CRUISEUP].state   = 1; }
void concntl_cmd_cruiseup_off(int argc, char **argv)    { Console_controls[CONCNTL_CRUISEUP].state   = 0; }
void concntl_cmd_cruisedown_on(int argc, char **argv)   { Console_controls[CONCNTL_CRUISEDOWN].state = 1; }
void concntl_cmd_cruisedown_off(int argc, char **argv)  { Console_controls[CONCNTL_CRUISEDOWN].state = 0; }
void concntl_cmd_cruiseoff_on(int argc, char **argv)    { Console_controls[CONCNTL_CRUISEOFF].state  = 1; }
void concntl_cmd_cruiseoff_off(int argc, char **argv)   { Console_controls[CONCNTL_CRUISEOFF].state  = 0; }
void concntl_cmd_automap_on(int argc, char **argv)      { Console_controls[CONCNTL_AUTOMAP].state    = 1; }
void concntl_cmd_automap_off(int argc, char **argv)     { Console_controls[CONCNTL_AUTOMAP].state    = 0; }
void concntl_cmd_afterburn_on(int argc, char **argv)    { Console_controls[CONCNTL_AFTERBURN].state  = 1; }
void concntl_cmd_afterburn_off(int argc, char **argv)   { Console_controls[CONCNTL_AFTERBURN].state  = 0; }
void concntl_cmd_cycle_on(int argc, char **argv)        { Console_controls[CONCNTL_CYCLE].state      = 1; }
void concntl_cmd_cycle_off(int argc, char **argv)       { Console_controls[CONCNTL_CYCLE].state      = 0; }
void concntl_cmd_cycle2_on(int argc, char **argv)       { Console_controls[CONCNTL_CYCLE2].state     = 1; }
void concntl_cmd_cycle2_off(int argc, char **argv)      { Console_controls[CONCNTL_CYCLE2].state     = 0; }
void concntl_cmd_headlight_on(int argc, char **argv)    { Console_controls[CONCNTL_HEADLIGHT].state  = 1; }
void concntl_cmd_headlight_off(int argc, char **argv)   { Console_controls[CONCNTL_HEADLIGHT].state  = 0; }
void concntl_cmd_nrgshield_on(int argc, char **argv)    { Console_controls[CONCNTL_NRGSHIELD].state  = 1; }
void concntl_cmd_nrgshield_off(int argc, char **argv)   { Console_controls[CONCNTL_NRGSHIELD].state  = 0; }
void concntl_cmd_togglebomb_on(int argc, char **argv)   { Console_controls[CONCNTL_TOGGLEBOMB].state = 1; }
void concntl_cmd_togglebomb_off(int argc, char **argv)  { Console_controls[CONCNTL_TOGGLEBOMB].state = 1; }

/* select / toggle weapons */
void concntl_cmd_weapon(int argc, char **argv)
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


// Returns the number of seconds this 'button' has been down since last call.
fix console_control_down_time(console_control control)
{
	fix time_down, time;

	if (!Console_controls[control].state) {
		time_down = Console_controls[control].time_held_down;
		Console_controls[control].time_held_down = 0;
	} else {
		time = timer_get_fixed_seconds();
		time_down = time - Console_controls[control].time_went_down;
		Console_controls[control].time_went_down = time;
	}
	
	return time_down;
}

unsigned int console_control_down_count(console_control control)
{
	int n;

	n = Console_controls[control].down_count;
	Console_controls[control].down_count = 0;
	
	return n;
}

unsigned int console_control_up_count(console_control control)
{
	int n;

	n = Console_controls[control].up_count;
	Console_controls[control].up_count = 0;

	return n;
}


// Returns 1 if this control is currently down
unsigned int console_control_state(console_control control)
{
	return Console_controls[control].state;
}


void console_control_init(void)
{
	memset(Console_controls, 0, sizeof(console_control_info) * CONCNTL_NUM_CONTROLS);

	cmd_addcommand("+lookdown",     concntl_cmd_lookdown_on);
	cmd_addcommand("-lookdown",     concntl_cmd_lookdown_off);
	cmd_addcommand("+lookup",       concntl_cmd_lookup_on);
	cmd_addcommand("-lookup",       concntl_cmd_lookup_off);
	cmd_addcommand("+left",         concntl_cmd_left_on);
	cmd_addcommand("-left",         concntl_cmd_left_off);
	cmd_addcommand("+right",        concntl_cmd_right_on);
	cmd_addcommand("-right",        concntl_cmd_right_off);
	cmd_addcommand("+strafe",       concntl_cmd_strafe_on);
	cmd_addcommand("-strafe",       concntl_cmd_strafe_off);
	cmd_addcommand("+moveleft",     concntl_cmd_moveleft_on);
	cmd_addcommand("-moveleft",     concntl_cmd_moveleft_off);
	cmd_addcommand("+moveright",    concntl_cmd_moveright_on);
	cmd_addcommand("-moveright",    concntl_cmd_moveright_off);
	cmd_addcommand("+moveup",       concntl_cmd_moveup_on);
	cmd_addcommand("-moveup",       concntl_cmd_moveup_off);
	cmd_addcommand("+movedown",     concntl_cmd_movedown_on);
	cmd_addcommand("-movedown",     concntl_cmd_movedown_off);
	cmd_addcommand("+bank",         concntl_cmd_bank_on);
	cmd_addcommand("-bank",         concntl_cmd_bank_off);
	cmd_addcommand("+bankleft",     concntl_cmd_bankleft_on);
	cmd_addcommand("-bankleft",     concntl_cmd_bankleft_off);
	cmd_addcommand("+bankright",    concntl_cmd_bankright_on);
	cmd_addcommand("-bankright",    concntl_cmd_bankright_off);
	cmd_addcommand("+attack",       concntl_cmd_attack_on);
	cmd_addcommand("-attack",       concntl_cmd_attack_off);
	cmd_addcommand("+attack2",      concntl_cmd_attack2_on);
	cmd_addcommand("-attack2",      concntl_cmd_attack2_off);
	cmd_addcommand("+flare",        concntl_cmd_flare_on);
	cmd_addcommand("-flare",        concntl_cmd_flare_off);
	cmd_addcommand("+forward",      concntl_cmd_forward_on);
	cmd_addcommand("-forward",      concntl_cmd_forward_off);
	cmd_addcommand("+back",         concntl_cmd_back_on);
	cmd_addcommand("-back",         concntl_cmd_back_off);
	cmd_addcommand("+bomb",         concntl_cmd_bomb_on);
	cmd_addcommand("-bomb",         concntl_cmd_bomb_off);
	cmd_addcommand("+rearview",     concntl_cmd_rearview_on);
	cmd_addcommand("-rearview",     concntl_cmd_rearview_off);
	cmd_addcommand("+cruiseup",     concntl_cmd_cruiseup_on);
	cmd_addcommand("-cruiseup",     concntl_cmd_cruiseup_off);
	cmd_addcommand("+cruisedown",   concntl_cmd_cruisedown_on);
	cmd_addcommand("-cruisedown",   concntl_cmd_cruisedown_off);
	cmd_addcommand("+cruiseoff",    concntl_cmd_cruiseoff_on);
	cmd_addcommand("-cruiseoff",    concntl_cmd_cruiseoff_off);
	cmd_addcommand("+automap",      concntl_cmd_automap_on);
	cmd_addcommand("-automap",      concntl_cmd_automap_off);
	cmd_addcommand("+afterburn",    concntl_cmd_afterburn_on);
	cmd_addcommand("-afterburn",    concntl_cmd_afterburn_off);
	cmd_addcommand("+cycle",        concntl_cmd_cycle_on);
	cmd_addcommand("-cycle",        concntl_cmd_cycle_off);
	cmd_addcommand("+cycle2",       concntl_cmd_cycle2_on);
	cmd_addcommand("-cycle2",       concntl_cmd_cycle2_off);
	cmd_addcommand("+headlight",    concntl_cmd_headlight_on);
	cmd_addcommand("-headlight",    concntl_cmd_headlight_off);
	cmd_addcommand("+nrgshield",    concntl_cmd_nrgshield_on);
	cmd_addcommand("-nrgshield",    concntl_cmd_nrgshield_off);
	cmd_addcommand("+togglebomb",   concntl_cmd_togglebomb_on);
	cmd_addcommand("-togglebomb",   concntl_cmd_togglebomb_off);

	cmd_addcommand("weapon", concntl_cmd_weapon);
}
