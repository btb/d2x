#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>

#include "console.h"
#include "timer.h"
#include "inferno.h"


typedef struct _console_control_info {
	ubyte state;
	fix   time_went_down;
	fix   time_held_down;
	uint  down_count;
	uint  up_count;
} console_control_info;

console_control_info Console_controls[CONCNTL_NUM_CONTROLS];

void concntl_cmd_lookdown_on(int argc, char **argv)     { console_control_set_state(CONCNTL_LOOKDOWN,   1); }
void concntl_cmd_lookdown_off(int argc, char **argv)    { console_control_set_state(CONCNTL_LOOKDOWN,   0); }
void concntl_cmd_lookup_on(int argc, char **argv)       { console_control_set_state(CONCNTL_LOOKUP,     1); }
void concntl_cmd_lookup_off(int argc, char **argv)      { console_control_set_state(CONCNTL_LOOKUP,     0); }
void concntl_cmd_left_on(int argc, char **argv)         { console_control_set_state(CONCNTL_LEFT,       1); }
void concntl_cmd_left_off(int argc, char **argv)        { console_control_set_state(CONCNTL_LEFT,       0); }
void concntl_cmd_right_on(int argc, char **argv)        { console_control_set_state(CONCNTL_RIGHT,      1); }
void concntl_cmd_right_off(int argc, char **argv)       { console_control_set_state(CONCNTL_RIGHT,      0); }
void concntl_cmd_strafe_on(int argc, char **argv)       { console_control_set_state(CONCNTL_STRAFE,     1); }
void concntl_cmd_strafe_off(int argc, char **argv)      { console_control_set_state(CONCNTL_STRAFE,     0); }
void concntl_cmd_moveleft_on(int argc, char **argv)     { console_control_set_state(CONCNTL_MOVELEFT,   1); }
void concntl_cmd_moveleft_off(int argc, char **argv)    { console_control_set_state(CONCNTL_MOVELEFT,   0); }
void concntl_cmd_moveright_on(int argc, char **argv)    { console_control_set_state(CONCNTL_MOVERIGHT,  1); }
void concntl_cmd_moveright_off(int argc, char **argv)   { console_control_set_state(CONCNTL_MOVERIGHT,  0); }
void concntl_cmd_moveup_on(int argc, char **argv)       { console_control_set_state(CONCNTL_MOVEUP,     1); }
void concntl_cmd_moveup_off(int argc, char **argv)      { console_control_set_state(CONCNTL_MOVEUP,     0); }
void concntl_cmd_movedown_on(int argc, char **argv)     { console_control_set_state(CONCNTL_MOVEDOWN,   1); }
void concntl_cmd_movedown_off(int argc, char **argv)    { console_control_set_state(CONCNTL_MOVEDOWN,   0); }
void concntl_cmd_bank_on(int argc, char **argv)         { console_control_set_state(CONCNTL_BANK,       1); }
void concntl_cmd_bank_off(int argc, char **argv)        { console_control_set_state(CONCNTL_BANK,       0); }
void concntl_cmd_bankleft_on(int argc, char **argv)     { console_control_set_state(CONCNTL_BANKLEFT,   1); }
void concntl_cmd_bankleft_off(int argc, char **argv)    { console_control_set_state(CONCNTL_BANKLEFT,   0); }
void concntl_cmd_bankright_on(int argc, char **argv)    { console_control_set_state(CONCNTL_BANKRIGHT,  1); }
void concntl_cmd_bankright_off(int argc, char **argv)   { console_control_set_state(CONCNTL_BANKRIGHT,  0); }
void concntl_cmd_attack_on(int argc, char **argv)       { console_control_set_state(CONCNTL_ATTACK,     1); }
void concntl_cmd_attack_off(int argc, char **argv)      { console_control_set_state(CONCNTL_ATTACK,     0); }
void concntl_cmd_attack2_on(int argc, char **argv)      { console_control_set_state(CONCNTL_ATTACK2,    1); }
void concntl_cmd_attack2_off(int argc, char **argv)     { console_control_set_state(CONCNTL_ATTACK2,    0); }
void concntl_cmd_flare_on(int argc, char **argv)        { console_control_set_state(CONCNTL_FLARE,      1); }
void concntl_cmd_flare_off(int argc, char **argv)       { console_control_set_state(CONCNTL_FLARE,      0); }
void concntl_cmd_forward_on(int argc, char **argv)      { console_control_set_state(CONCNTL_FORWARD,    1); }
void concntl_cmd_forward_off(int argc, char **argv)     { console_control_set_state(CONCNTL_FORWARD,    0); }
void concntl_cmd_back_on(int argc, char **argv)         { console_control_set_state(CONCNTL_BACK,       1); }
void concntl_cmd_back_off(int argc, char **argv)        { console_control_set_state(CONCNTL_BACK,       0); }
void concntl_cmd_bomb_on(int argc, char **argv)         { console_control_set_state(CONCNTL_BOMB,       1); }
void concntl_cmd_bomb_off(int argc, char **argv)        { console_control_set_state(CONCNTL_BOMB,       0); }
void concntl_cmd_rearview_on(int argc, char **argv)     { console_control_set_state(CONCNTL_REARVIEW,   1); }
void concntl_cmd_rearview_off(int argc, char **argv)    { console_control_set_state(CONCNTL_REARVIEW,   0); }
void concntl_cmd_cruiseup_on(int argc, char **argv)     { console_control_set_state(CONCNTL_CRUISEUP,   1); }
void concntl_cmd_cruiseup_off(int argc, char **argv)    { console_control_set_state(CONCNTL_CRUISEUP,   0); }
void concntl_cmd_cruisedown_on(int argc, char **argv)   { console_control_set_state(CONCNTL_CRUISEDOWN, 1); }
void concntl_cmd_cruisedown_off(int argc, char **argv)  { console_control_set_state(CONCNTL_CRUISEDOWN, 0); }
void concntl_cmd_cruiseoff_on(int argc, char **argv)    { console_control_set_state(CONCNTL_CRUISEOFF,  1); }
void concntl_cmd_cruiseoff_off(int argc, char **argv)   { console_control_set_state(CONCNTL_CRUISEOFF,  0); }
void concntl_cmd_automap_on(int argc, char **argv)      { console_control_set_state(CONCNTL_AUTOMAP,    1); }
void concntl_cmd_automap_off(int argc, char **argv)     { console_control_set_state(CONCNTL_AUTOMAP,    0); }
void concntl_cmd_afterburn_on(int argc, char **argv)    { console_control_set_state(CONCNTL_AFTERBURN,  1); }
void concntl_cmd_afterburn_off(int argc, char **argv)   { console_control_set_state(CONCNTL_AFTERBURN,  0); }
void concntl_cmd_cycle_on(int argc, char **argv)        { console_control_set_state(CONCNTL_CYCLE,      1); }
void concntl_cmd_cycle_off(int argc, char **argv)       { console_control_set_state(CONCNTL_CYCLE,      0); }
void concntl_cmd_cycle2_on(int argc, char **argv)       { console_control_set_state(CONCNTL_CYCLE2,     1); }
void concntl_cmd_cycle2_off(int argc, char **argv)      { console_control_set_state(CONCNTL_CYCLE2,     0); }
void concntl_cmd_headlight_on(int argc, char **argv)    { console_control_set_state(CONCNTL_HEADLIGHT,  1); }
void concntl_cmd_headlight_off(int argc, char **argv)   { console_control_set_state(CONCNTL_HEADLIGHT,  0); }
void concntl_cmd_nrgshield_on(int argc, char **argv)    { console_control_set_state(CONCNTL_NRGSHIELD,  1); }
void concntl_cmd_nrgshield_off(int argc, char **argv)   { console_control_set_state(CONCNTL_NRGSHIELD,  0); }
void concntl_cmd_togglebomb_on(int argc, char **argv)   { console_control_set_state(CONCNTL_TOGGLEBOMB, 1); }
void concntl_cmd_togglebomb_off(int argc, char **argv)  { console_control_set_state(CONCNTL_TOGGLEBOMB, 0); }

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


void console_control_set_state(console_control control, int state)
{
	console_control_info *ctl = &Console_controls[control];

	if (state) {
		ctl->down_count++;
		ctl->state = 1;
		ctl->time_went_down = timer_get_fixed_seconds();
	} else {
		ctl->up_count++;
		ctl->state = 0;
		ctl->time_held_down += timer_get_fixed_seconds() - ctl->time_went_down;
	}
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
