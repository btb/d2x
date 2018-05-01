

#include <SDL.h>

#include "joy.h"
#include "error.h"


#define MAX_BUTTONS 20


// Data Structures ============================================================

typedef struct Button_info {
   ubyte ignore;
   ubyte state;
   ubyte last_state;
   unsigned int timedown;
   ubyte downcount;
   ubyte upcount;
} Button_info;

typedef struct Joy_info {
   SDL_Joystick *joy;
   ubyte present_mask;
   int   max_timer;
   int   read_count;
   ubyte last_value;
   Button_info buttons[MAX_BUTTONS];
   int   axis_min[4];
   int   axis_center[4];
   int   axis_max[4];
   int   has_pov;
   int   threshold;
   int   raw_x1, raw_y1, raw_z1, raw_r1, raw_u1, raw_v1, pov;
} Joy_info;


// Globals ====================================================================

Joy_info joystick;
char joy_installed = 0;
char joy_present;


// Functions ==================================================================

int joy_init(void)
{
   int i;

   atexit(joy_close);

   // Reset Joystick information.
   joy_flush();
   memset(&joystick, 0, sizeof(joystick));

   for (i = 0; i < MAX_BUTTONS; i++)
      joystick.buttons[i].last_state = 0;

   if (!joy_installed) {
      joy_present = 0;
      joy_installed = 1;
      joystick.max_timer = 65536;
      joystick.read_count = 0;
      joystick.last_value = 0;
   }

   joy_present = SDL_NumJoysticks();

   return joy_present;
}

void joy_close(void)
{
   if (joy_installed) {
      SDL_JoystickClose(joystick.joy);
      joy_installed = 0;
   }
}

void joy_get_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
   int i;

   for (i = 0; i < 4; i++) {
      axis_min[i] = joystick.axis_min[i];
      axis_center[i] = joystick.axis_center[i];
      axis_max[i] = joystick.axis_max[i];
   }
}

void joy_set_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
   int i;

   for (i = 0; i < 4; i++) {
      joystick.axis_min[i] = axis_min[i];
      joystick.axis_center[i] = axis_center[i];
      joystick.axis_max[i] = axis_max[i];
   }
}

ubyte joy_get_present_mask(void)
{
   Int3();
   return 0;
}

// resets joystick button parameters
void joy_flush(void)
{
   int i;

   for (i = 0; i < MAX_BUTTONS; i++ ) {
      joystick.buttons[i].ignore = 0;
      joystick.buttons[i].state = 0;
      joystick.buttons[i].timedown = 0;
      joystick.buttons[i].downcount = 0;
      joystick.buttons[i].upcount = 0;
   }
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
   int count;

   if ((!joy_installed) || (!joy_present))
      return 0;

   if (btn >= MAX_BUTTONS)
      return 0;

   count = joystick.buttons[btn].downcount;
   joystick.buttons[btn].downcount = 0;

   return count;
}

fix joy_get_button_down_time(int btn)
{
   Int3();
   return 0;
}

void joy_set_btn_values(int btn, int state, fix timedown, int downcount,
                        int upcount)
{
   Int3();
}

void joy_set_slow_reading(int flag)
{
   Int3();
}
