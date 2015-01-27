/*
 *
 * SDL joystick support
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>   // for memset
#include <SDL.h>

#include "joy.h"
#include "error.h"
#include "timer.h"
#include "console.h"
#include "event.h"
#include "text.h"
#include "u_mem.h"
#include "key.h"
#include "inferno.h"


#define MAX_JOYSTICKS 16

#define MAX_AXES_PER_JOYSTICK 8
#define MAX_BUTTONS_PER_JOYSTICK 16
#define MAX_HATS_PER_JOYSTICK 4

extern char *joyaxis_text[]; //from kconfig.c

char joy_present = 0;
int num_joysticks = 0;

int joy_deadzone = 0;

int joy_num_axes = 0;

struct joyaxis {
	int		value;
	int		min_val;
	int		center_val;
	int		max_val;
};

/* This struct is a "virtual" joystick, which includes all the axes
 * and buttons of every joystick found.
 */
static struct joyinfo {
	int n_axes;
	int n_buttons;
	struct joyaxis axes[JOY_MAX_AXES];
} Joystick;

/* This struct is an array, with one entry for each physical joystick
 * found.
 */
static struct {
	SDL_Joystick *handle;
	int n_axes;
	int n_buttons;
	int n_hats;
	int hat_map[MAX_HATS_PER_JOYSTICK];  //Note: Descent expects hats to be buttons, so these are indices into Joystick.buttons
	int axis_map[MAX_AXES_PER_JOYSTICK];
	int button_map[MAX_BUTTONS_PER_JOYSTICK];
} SDL_Joysticks[MAX_JOYSTICKS];


// Axis mapping cvars
// 0 = no action
// 1 = move forward/back
// 2 = look up/down
// 3 = move left/right
// 4 = look left/right
// 5 = move up/down
// 6 = bank left/right

cvar_t joy_advaxes[] = {
	{ "joy_advaxisx", "4", 1 },
	{ "joy_advaxisy", "2", 1 },
	{ "joy_advaxisz", "0", 1 },
	{ "joy_advaxisr", "0", 1 },
	{ "joy_advaxisu", "0", 1 },
	{ "joy_advaxisv", "0", 1 },
};

cvar_t joy_invert[] = {
	{ "joy_invertx", "0", 1 },
	{ "joy_inverty", "0", 1 },
	{ "joy_invertz", "0", 1 },
	{ "joy_invertr", "0", 1 },
	{ "joy_invertu", "0", 1 },
	{ "joy_invertv", "0", 1 },
};


void joy_button_handler(SDL_JoyButtonEvent *jbe)
{
	int button;

	button = SDL_Joysticks[jbe->which].button_map[jbe->button];

	vkey_handler(KEY_JB1 + button, jbe->state == SDL_PRESSED);
}

void joy_hat_handler(SDL_JoyHatEvent *jhe)
{
	int hat = SDL_Joysticks[jhe->which].hat_map[jhe->hat];
	int state_up    = (jhe->value & SDL_HAT_UP   ) != 0;
	int state_right = (jhe->value & SDL_HAT_RIGHT) != 0;
	int state_down  = (jhe->value & SDL_HAT_DOWN ) != 0;
	int state_left  = (jhe->value & SDL_HAT_LEFT ) != 0;
	int old_state_up    = keyd_pressed[KEY_JB1 + hat + 0];
	int old_state_right = keyd_pressed[KEY_JB1 + hat + 1];
	int old_state_down  = keyd_pressed[KEY_JB1 + hat + 2];
	int old_state_left  = keyd_pressed[KEY_JB1 + hat + 3];

	if (state_up    != old_state_up   ) vkey_handler(KEY_JB1 + hat + 0, state_up   );
	if (state_right != old_state_right) vkey_handler(KEY_JB1 + hat + 1, state_right);
	if (state_down  != old_state_down ) vkey_handler(KEY_JB1 + hat + 2, state_down );
	if (state_left  != old_state_left ) vkey_handler(KEY_JB1 + hat + 3, state_left );
}

void joy_axis_handler(SDL_JoyAxisEvent *jae)
{
	int axis;

	axis = SDL_Joysticks[jae->which].axis_map[jae->axis];
	
	Joystick.axes[axis].value = jae->value;
}


/* ----------------------------------------------- */

int joy_init()
{
	int i,j,n;
	char temp[10];

	if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
		con_printf(CON_VERBOSE, "sdl-joystick: initialisation failed: %s.",SDL_GetError());
		return 0;
	}

	memset(&Joystick,0,sizeof(Joystick));
	memset(joyaxis_text, 0, JOY_MAX_AXES * sizeof(char *));

	for (i = 0; i < 6; i++) {
		cvar_registervariable(&joy_advaxes[i]);
		cvar_registervariable(&joy_invert[i]);
	}

	n = SDL_NumJoysticks();

	con_printf(CON_VERBOSE, "sdl-joystick: found %d joysticks\n", n);
	for (i = 0; i < n; i++) {
		con_printf(CON_VERBOSE, "sdl-joystick %d: %s\n", i, SDL_JoystickName(i));
		SDL_Joysticks[num_joysticks].handle = SDL_JoystickOpen(i);
		if (SDL_Joysticks[num_joysticks].handle) {
			joy_present = 1;

			SDL_Joysticks[num_joysticks].n_axes
				= SDL_JoystickNumAxes(SDL_Joysticks[num_joysticks].handle);
			if(SDL_Joysticks[num_joysticks].n_axes > MAX_AXES_PER_JOYSTICK)
			{
				Warning("sdl-joystick: found %d axes, only %d supported.  Game may be unstable.\n", SDL_Joysticks[num_joysticks].n_axes, MAX_AXES_PER_JOYSTICK);
				SDL_Joysticks[num_joysticks].n_axes = MAX_AXES_PER_JOYSTICK;
			}

			SDL_Joysticks[num_joysticks].n_buttons
				= SDL_JoystickNumButtons(SDL_Joysticks[num_joysticks].handle);
			if(SDL_Joysticks[num_joysticks].n_buttons > MAX_BUTTONS_PER_JOYSTICK)
			{
				Warning("sdl-joystick: found %d buttons, only %d supported.  Game may be unstable.\n", SDL_Joysticks[num_joysticks].n_buttons, MAX_BUTTONS_PER_JOYSTICK);
				SDL_Joysticks[num_joysticks].n_buttons = MAX_BUTTONS_PER_JOYSTICK;
			}

			SDL_Joysticks[num_joysticks].n_hats
				= SDL_JoystickNumHats(SDL_Joysticks[num_joysticks].handle);
			if(SDL_Joysticks[num_joysticks].n_hats > MAX_HATS_PER_JOYSTICK)
			{
				Warning("sdl-joystick: found %d hats, only %d supported.  Game may be unstable.\n", SDL_Joysticks[num_joysticks].n_hats, MAX_HATS_PER_JOYSTICK);
				SDL_Joysticks[num_joysticks].n_hats = MAX_HATS_PER_JOYSTICK;
			}

			con_printf(CON_VERBOSE, "sdl-joystick: %d axes\n", SDL_Joysticks[num_joysticks].n_axes);
			con_printf(CON_VERBOSE, "sdl-joystick: %d buttons\n", SDL_Joysticks[num_joysticks].n_buttons);
			con_printf(CON_VERBOSE, "sdl-joystick: %d hats\n", SDL_Joysticks[num_joysticks].n_hats);

			for (j=0; j < SDL_Joysticks[num_joysticks].n_axes; j++)
			{
				sprintf(temp, "J%d A%d", i + 1, j + 1);
				joyaxis_text[Joystick.n_axes] = d_strdup(temp);
				SDL_Joysticks[num_joysticks].axis_map[j] = Joystick.n_axes++;
			}
			for (j=0; j < SDL_Joysticks[num_joysticks].n_buttons; j++)
			{
				sprintf(temp, "J%dB%d", i + 1, j + 1);
				key_text[KEY_JB1 + Joystick.n_buttons] = d_strdup(temp);
				SDL_Joysticks[num_joysticks].button_map[j] = Joystick.n_buttons++;
			}
			for (j=0; j < SDL_Joysticks[num_joysticks].n_hats; j++)
			{
				SDL_Joysticks[num_joysticks].hat_map[j] = Joystick.n_buttons;
				//a hat counts as four buttons

				sprintf(temp, "J%dH%dUP", i + 1, j + 1);
				key_text[KEY_JB1 + Joystick.n_buttons] = d_strdup(temp);
				Joystick.n_buttons++;

				sprintf(temp, "J%dH%dRIGHT", i + 1, j + 1);
				key_text[KEY_JB1 + Joystick.n_buttons] = d_strdup(temp);
				Joystick.n_buttons++;

				sprintf(temp, "J%dH%dDOWN", i + 1, j + 1);
				key_text[KEY_JB1 + Joystick.n_buttons] = d_strdup(temp);
				Joystick.n_buttons++;

				sprintf(temp, "J%dH%dLEFT", i + 1, j + 1);
				key_text[KEY_JB1 + Joystick.n_buttons] = d_strdup(temp);
				Joystick.n_buttons++;
			}

			num_joysticks++;
		}
		else
			con_printf(CON_VERBOSE, "sdl-joystick: initialization failed!\n");

		con_printf(CON_VERBOSE, "sdl-joystick: %d axes (total)\n", Joystick.n_axes);
		con_printf(CON_VERBOSE, "sdl-joystick: %d buttons (total)\n", Joystick.n_buttons);
	}

	joy_num_axes = Joystick.n_axes;
	atexit(joy_close);

	return joy_present;
}

void joy_close()
{
	while (num_joysticks)
		SDL_JoystickClose(SDL_Joysticks[--num_joysticks].handle);
	while (Joystick.n_axes--)
		d_free(joyaxis_text[Joystick.n_axes]);
	while (Joystick.n_buttons--)
		d_free(key_text[KEY_JB1 + Joystick.n_buttons]);
}

void joy_get_pos(int *x, int *y)
{
	int axis[JOY_MAX_AXES];

	if (!num_joysticks) {
		*x=*y=0;
		return;
	}

	joystick_read_raw_axis (JOY_ALL_AXIS, axis);

	*x = joy_get_scaled_reading( axis[0], 0 );
	*y = joy_get_scaled_reading( axis[1], 1 );
}

int joy_get_btns()
{
#if 0 // This is never used?
	int i, buttons = 0;
	for (i=0; i++; i<buttons) {
		switch (keyd_pressed[KEY_JB1 + i]) {
		case 1:
			buttons |= 1<<i;
			break;
		case 0:
			break;
		}
	}
	return buttons;
#else
	return 0;
#endif
}


ubyte joystick_read_raw_axis( ubyte mask, int * axis )
{
	int i;
	ubyte channel_masks = 0;
	
	if (!num_joysticks)
		return 0;

	event_poll();

	for (i = 0; i < Joystick.n_axes; i++)
	{
		if ((axis[i] = Joystick.axes[i].value))
			channel_masks |= 1 << i;
	}

	return channel_masks;
}

void joy_flush()
{
	if (!num_joysticks)
		return;
}

int joy_get_button_state( int btn )
{
	if (!num_joysticks)
		return 0;

	if(btn >= Joystick.n_buttons)
		return 0;

	event_poll();

	return keyd_pressed[KEY_JB1 + btn];
}

void joy_get_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
	int i;

	for (i = 0; i < Joystick.n_axes; i++)
	{
		axis_center[i] = Joystick.axes[i].center_val;
		axis_min[i] = Joystick.axes[i].min_val;
		axis_max[i] = Joystick.axes[i].max_val;
	}
}

void joy_set_cal_vals(int *axis_min, int *axis_center, int *axis_max)
{
	int i;

	for (i = 0; i < Joystick.n_axes; i++)
	{
		Joystick.axes[i].center_val = axis_center[i];
		Joystick.axes[i].min_val = axis_min[i];
		Joystick.axes[i].max_val = axis_max[i];
	}
}

int joy_get_scaled_reading( int raw, int axis_num )
{
#if 1
	return raw/256;
#else
	int d, x;

	raw -= Joystick.axes[axis_num].center_val;
	
	if (raw < 0)
		d = Joystick.axes[axis_num].center_val - Joystick.axes[axis_num].min_val;
	else if (raw > 0)
		d = Joystick.axes[axis_num].max_val - Joystick.axes[axis_num].center_val;
	else
		d = 0;
	
	if (d)
		x = ((raw << 7) / d);
	else
		x = 0;
	
	if ( x < -128 )
		x = -128;
	if ( x > 127 )
		x = 127;
	
	d =  (joy_deadzone) * 6;
	if ((x > (-1*d)) && (x < d))
		x = 0;
	
	return x;
#endif
}

void joy_set_slow_reading( int flag )
{
}
