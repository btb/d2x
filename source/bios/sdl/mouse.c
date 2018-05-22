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
#include "error.h"


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

static int Mouse_installed = 0;
static int Mouse_center = 0;

int mouse_set_mode(int i)
{
   int old;
   old = Mouse_center;
   if (i) Mouse_center = 1;
   else Mouse_center = 0;

   if (Mouse_center)
      SDL_SetRelativeMouseMode(SDL_TRUE);
   else
      SDL_SetRelativeMouseMode(SDL_FALSE);

   return old;
}

//--------------------------------------------------------
// returns 0 if no mouse
// else number of buttons
int mouse_init(int enable_cyberman)
{
   if (Mouse_installed)
      return 2;

   Mouse_installed = 1;

   atexit(mouse_close);

   mouse_flush();

    return 2;
}

void mouse_close()
{
   if (Mouse_installed) {
      Mouse_installed = 0;
   }
}

void mouse_get_delta(int *dx, int *dy)
{
   if (!Mouse_installed) {
      *dx = *dy = 0;
      return;
   }

   SDL_GetRelativeMouseState(dx, dy);
}

void event_poll(void);

int mouse_get_btns(void)
{
   int i;
   uint flag = 1;
   int status = 0;

   event_poll();

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

   event_poll();

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

   if (!Mouse_installed)
      return 0;

   event_poll();

   count = Mouse.num_downs[button];
   Mouse.num_downs[button] = 0;

   return count;
}

// Returns 1 if this button is currently down
int mouse_button_state(int button)
{
   int state;

   event_poll();

   state = Mouse.pressed[button];

   return state;
}

// Returns how long this button has been down since last call.
fix mouse_button_down_time(int button)
{
   fix time_down, time;

   event_poll();

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

void mouse_button_handler(SDL_MouseButtonEvent *event)
{
   Mouse.ctime = timer_get_fixed_secondsX();

   if (event->button == SDL_BUTTON_LEFT &&
       event->type == SDL_MOUSEBUTTONDOWN) {
      if (!Mouse.pressed[MB_LEFT]) {
         Mouse.pressed[MB_LEFT] = 1;
         Mouse.time_went_down[MB_LEFT] = Mouse.ctime;
      }
      Mouse.num_downs[MB_LEFT]++;

   } else if (event->button == SDL_BUTTON_LEFT &&
              event->type == SDL_MOUSEBUTTONUP) {
      if (Mouse.pressed[MB_LEFT]) {
         Mouse.pressed[MB_LEFT] = 0;
         Mouse.time_held_down[MB_LEFT] +=
            Mouse.ctime - Mouse.time_went_down[MB_LEFT];
      }
      Mouse.num_ups[MB_LEFT]++;

   } else if (event->button == SDL_BUTTON_RIGHT &&
              event->type == SDL_MOUSEBUTTONDOWN) {
      if (!Mouse.pressed[MB_RIGHT]) {
         Mouse.pressed[MB_RIGHT] = 1;
         Mouse.time_went_down[MB_RIGHT] = Mouse.ctime;
      }
      Mouse.num_downs[MB_RIGHT]++;

   } else if (event->button == SDL_BUTTON_RIGHT &&
              event->type == SDL_MOUSEBUTTONUP) {
      if (Mouse.pressed[MB_RIGHT])  {
         Mouse.pressed[MB_RIGHT] = 0;
         Mouse.time_held_down[MB_RIGHT] +=
            Mouse.ctime - Mouse.time_went_down[MB_RIGHT];
      }
      Mouse.num_ups[MB_RIGHT]++;

   } else if (event->button == SDL_BUTTON_MIDDLE &&
              event->type == SDL_MOUSEBUTTONDOWN) {
      if (!Mouse.pressed[MB_MIDDLE])   {
         Mouse.pressed[MB_MIDDLE] = 1;
         Mouse.time_went_down[MB_MIDDLE] = Mouse.ctime;
      }
      Mouse.num_downs[MB_MIDDLE]++;

   } else if (event->button == SDL_BUTTON_MIDDLE &&
              event->type == SDL_MOUSEBUTTONUP) {
      if (Mouse.pressed[MB_MIDDLE]) {
         Mouse.pressed[MB_MIDDLE] = 0;
         Mouse.time_held_down[MB_MIDDLE] +=
            Mouse.ctime - Mouse.time_went_down[MB_MIDDLE];
      }
      Mouse.num_ups[MB_MIDDLE]++;

   }
}
