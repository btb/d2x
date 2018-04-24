/*
 *
 * SDL mouse driver.
 *
 */


#include <SDL.h>

#include "fix.h"
#include "timer.h"
#include "mouse.h"
#include "key.h"


#define MOUSE_MAX_BUTTONS       11

typedef struct mouse_info {
   fix         ctime;
   ubyte       cyberman;
   int         num_buttons;
   ubyte       pressed[MOUSE_MAX_BUTTONS];
   fix         time_went_down[MOUSE_MAX_BUTTONS];
   fix         time_held_down[MOUSE_MAX_BUTTONS];
   uint        num_downs[MOUSE_MAX_BUTTONS];
   uint        num_ups[MOUSE_MAX_BUTTONS];
//   event_info  *x_info;
   ushort  button_status;
} mouse_info;

static mouse_info Mouse;

//--------------------------------------------------------
// returns 0 if no mouse
// else number of buttons
int mouse_init(int enable_cyberman)
{
   return 2;
}

void mouse_get_delta(int *dx, int *dy)
{
   *dx = 0;
   *dy = 0;
}

int mouse_get_btns(void)
{
   int i;
   uint flag = 1;
   int status = 0;

   for (i = 0; i < MOUSE_MAX_BUTTONS; i++) {
      if (Mouse.pressed[i])
         status |= flag;
      flag <<= 1;
   }
   return status;
}

void mouse_flush(void)
{
   int i;
   fix CurTime;

   //Clear the mouse data
   CurTime = timer_get_fixed_secondsX();
   for (i = 0; i < MOUSE_MAX_BUTTONS; i++) {
      Mouse.pressed[i] = 0;
      Mouse.time_went_down[i] = CurTime;
      Mouse.time_held_down[i] = 0;
      Mouse.num_downs[i] = 0;
      Mouse.num_ups[i] = 0;
   }
}

// Returns how many times this button has went down since last call.
int mouse_button_down_count(int button)
{
   int count;

   count = Mouse.num_downs[button];
   Mouse.num_downs[button] = 0;

   return count;
}

// Returns 1 if this button is currently down
int mouse_button_state(int button)
{
   int state;

   state = Mouse.pressed[button];

   return state;
}

// Returns how long this button has been down since last call.
fix mouse_button_down_time(int button)
{
   fix time_down, time;

   if (!Mouse.pressed[button]) {
      time_down = Mouse.time_held_down[button];
      Mouse.time_held_down[button] = 0;
   } else {
      time = timer_get_fixed_secondsX();
      time_down = time - Mouse.time_went_down[button];
      Mouse.time_went_down[button] = time;
   }

   return time_down;
}

void mouse_get_cyberman_pos(int *x, int *y)
{
   *x = 0;
   *y = 0;
}
