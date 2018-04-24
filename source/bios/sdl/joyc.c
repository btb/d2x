

#include <SDL.h>

#include "joy.h"


// Data Structures ============================================================


// Globals ====================================================================

char joy_present;


// Functions ==================================================================

int joy_init(int joy, int spjoy)
{
   return 0;
}

void joy_get_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
}

void joy_set_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
}

ubyte joy_get_present_mask(void)
{
   return 0;
}

// resets joystick button parameters
void joy_flush(void)
{
}

// returns the status of the buttons at that moment.
ubyte joy_read_raw_buttons(void)
{
   return 0;
}

ubyte joystick_read_raw_axis(ubyte mask, int *axis)
{
   return 0;
}

void joy_set_cen(void)
{
}

int joy_get_scaled_reading(int raw, int axn)
{
   return 0;
}

void joy_get_pos(int *x, int *y)
{
}

int joy_get_button_state(int btn)
{
   return 0;
}

int joy_get_button_down_cnt(int btn)
{
   return 0;
}

fix joy_get_button_down_time(int btn)
{
   return 0;
}

void joy_set_btn_values(int btn, int state, fix timedown, int downcount,
                        int upcount)
{
}

void joy_set_slow_reading(int flag)
{
}
