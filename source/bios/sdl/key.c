

#include <SDL.h>

#include "key.h"


//-------- Variable accessed by outside functions ---------
char keyd_repeat;
volatile unsigned char keyd_pressed[256];
volatile int keyd_time_when_last_pressed;


//	Initialization and Cleanup Routines

void key_init(void)
{
}


char key_to_ascii(int keycode)
{
	return 255;
}

void key_flush()
{
}

int key_inkey(void)
{
	return 0;
}

int key_inkey_time(fix *time)
{
	return 0;
}

// If not installed, uses BIOS and returns getch();
//	Else returns pending key (or waits for one if none waiting).
int key_getch(void)
{
	return key_inkey();
}

// Returns the number of seconds this key has been down since last call.
fix key_down_time(int scancode)
{
	return 0;
}

// Returns number of times key has went from up to down since last call.
unsigned int key_down_count(int scancode)
{
	return 0;
}
