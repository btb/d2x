

#include <SDL.h>

#include "joy.h"
#include "error.h"


// Data Structures ============================================================


// Globals ====================================================================

char joy_present;


// Functions ==================================================================

int joy_init(int joy, int spjoy)
{
	Int3();
	return 0;
}

void joy_get_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
	Int3();
}

void joy_set_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
	Int3();
}

ubyte joy_get_present_mask(void)
{
	Int3();
	return 0;
}

// resets joystick button parameters
void joy_flush(void)
{
	Int3();
}

// returns the status of the buttons at that moment.
ubyte joy_read_raw_buttons(void)
{
	Int3();
	return 0;
}

ubyte joystick_read_raw_axis(ubyte mask, int *axis)
{
	Int3();
	return 0;
}

void joy_set_cen(void)
{
	Int3();
}

int joy_get_scaled_reading(int raw, int axn)
{
	Int3();
	return 0;
}

void joy_get_pos(int *x, int *y)
{
	Int3();
}

int joy_get_btns(void)
{
	Int3();
	return 0;
}

int joy_get_button_state(int btn)
{
	Int3();
	return 0;
}

int joy_get_button_down_cnt(int btn)
{
	Int3();
	return 0;
}

fix joy_get_button_down_time(int btn)
{
	Int3();
	return 0;
}

void joy_set_btn_values(int btn, int state, fix timedown, int downcount, int upcount)
{
	Int3();
}

void joy_set_slow_reading(int flag)
{
	Int3();
}
