/*
 *
 * SDL mouse driver.
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <string.h>

#include <SDL.h>

#include "fix.h"
#include "timer.h"
#include "event.h"
#include "mouse.h"
#include "key.h"

#ifdef _WIN32_WCE
# define LANDSCAPE
#endif

#define Z_SENSITIVITY 100

static struct mouseinfo {
	int delta_x, delta_y, delta_z;
	int x,y,z;
} Mouse;


cvar_t mouse_axes[3] = {
	{ "mouse_axisx", "4", 1 },
	{ "mouse_axisy", "2", 1 },
	{ "mouse_axisz", "0", 1 },
};

cvar_t mouse_invert[] = {
	{ "mouse_invertx", "0", 1 },
	{ "mouse_inverty", "0", 1 },
	{ "mouse_invertz", "0", 1 },
};


void d_mouse_init(void)
{
	int i;

	memset(&Mouse,0,sizeof(Mouse));

	for (i = 0; i < 3; i++) {
		cvar_registervariable(&mouse_axes[i]);
		cvar_registervariable(&mouse_invert[i]);
	}
}

void mouse_button_handler(SDL_MouseButtonEvent *mbe)
{
	// to bad, SDL buttons use a different mapping as descent expects,
	// this is at least true and tested for the first three buttons 
	int button_remap[11] = {
		MB_LEFT,
		MB_MIDDLE,
		MB_RIGHT,
		MB_Z_UP,
		MB_Z_DOWN,
		MB_PITCH_BACKWARD,
		MB_PITCH_FORWARD,
		MB_BANK_LEFT,
		MB_BANK_RIGHT,
		MB_HEAD_LEFT,
		MB_HEAD_RIGHT
	};

	int button = button_remap[mbe->button - 1]; // -1 since SDL seems to start counting at 1

	vkey_handler(KEY_MB1 + button, mbe->state == SDL_PRESSED);

	if (mbe->state == SDL_PRESSED) {
		if (button == MB_Z_UP) {
			Mouse.delta_z += Z_SENSITIVITY;
			Mouse.z += Z_SENSITIVITY;
		} else if (button == MB_Z_DOWN) {
			Mouse.delta_z -= Z_SENSITIVITY;
			Mouse.z -= Z_SENSITIVITY;
		}
	}
}

void mouse_motion_handler(SDL_MouseMotionEvent *mme)
{
#ifdef LANDSCAPE
	Mouse.delta_y += mme->xrel;
	Mouse.delta_x += mme->yrel;
	Mouse.y += mme->xrel;
	Mouse.x += mme->yrel;
#else
	Mouse.delta_x += mme->xrel;
	Mouse.delta_y += mme->yrel;
	Mouse.x += mme->xrel;
	Mouse.y += mme->yrel;
#endif
}

void mouse_flush()	// clears all mice events...
{
	event_poll();

	Mouse.delta_x = 0;
	Mouse.delta_y = 0;
	Mouse.delta_z = 0;
	Mouse.x = 0;
	Mouse.y = 0;
	Mouse.z = 0;
	SDL_GetMouseState(&Mouse.x, &Mouse.y); // necessary because polling only gives us the delta.
}

//========================================================================
void mouse_get_pos( int *x, int *y )
{
	event_poll();
#ifdef _WIN32_WCE // needed here only for touchpens?
# ifdef LANDSCAPE
	SDL_GetMouseState(&Mouse.y, &Mouse.x);
# else
	SDL_GetMouseState(&Mouse.x, &Mouse.y);
# endif
#endif
	*x=Mouse.x;
	*y=Mouse.y;
}

void mouse_get_delta( int *dx, int *dy, int *dz )
{
	event_poll();
	*dx = Mouse.delta_x;
	*dy = Mouse.delta_y;
	*dz = Mouse.delta_z;
	Mouse.delta_x = 0;
	Mouse.delta_y = 0;
	Mouse.delta_z = 0;
}

int mouse_get_btns()
{
	int i;
	uint flag=1;
	int status = 0;

	event_poll();

	for (i=0; i<MOUSE_MAX_BUTTONS; i++ ) {
		if (keyd_pressed[KEY_MB1 + i])
			status |= flag;
		flag <<= 1;
	}

	return status;
}

void mouse_get_cyberman_pos( int *x, int *y )
{
}


// Returns 1 if this button is currently down
int mouse_button_state(int button)
{
	event_poll();
	return keyd_pressed[KEY_MB1 + button];
}


int mouse_set_mode(int i)
{
	SDL_GrabMode old;

	old = SDL_WM_GrabInput(SDL_GRAB_QUERY);
	if (i)
		SDL_WM_GrabInput(SDL_GRAB_ON);
	else
		SDL_WM_GrabInput(SDL_GRAB_OFF);

	return (old == SDL_GRAB_ON);
}
