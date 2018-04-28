

#include <SDL.h>

#include "key.h"
#include "timer.h"
#include "error.h"


#define KEY_BUFFER_SIZE 16

//-------- Variable accessed by outside functions ---------

char keyd_repeat;
volatile unsigned char keyd_pressed[256];
volatile int keyd_time_when_last_pressed;


typedef struct keyboard {
   unsigned short  keybuffer[KEY_BUFFER_SIZE];
   fix             time_pressed[KEY_BUFFER_SIZE];
   fix             TimeKeyWentDown[256];
   fix             TimeKeyHeldDown[256];
   unsigned int    NumDowns[256];
   unsigned int    NumUps[256];
   unsigned int    keyhead, keytail;
   unsigned char   E0Flag;
   unsigned char   E1Flag;
} keyboard;

static volatile keyboard key_data;

static unsigned char Installed = 0;


// Internal prototypes
void key_clear_bios_buffer_all(void);


// Initialization and Cleanup Routines

void key_init(void)
{
   keyd_time_when_last_pressed = timer_get_fixed_seconds();
   keyd_repeat = 1;

   if (Installed) return;
   Installed = 1;

   atexit(key_close);
}

void key_close()
{
   if (!Installed) return;
   Installed = 0;

   key_clear_bios_buffer_all();
}

char key_to_ascii(int keycode)
{
   Int3();
   return 255;
}

void key_clear_bios_buffer_all(void)
{
}

void key_clear_bios_buffer(void)
{
}

void key_flush()
{
   Int3();
}

// Returns 1 if character waiting... 0 otherwise
int key_checkch(void)
{
   int is_one_waiting = 0;

   key_clear_bios_buffer();

   if (key_data.keytail != key_data.keyhead)
      is_one_waiting = 1;

   return is_one_waiting;
}

int key_inkey(void)
{
   Int3();
   return 0;
}

int key_inkey_time(fix *time)
{
   Int3();
   return 0;
}

int key_peekkey()
{
   Int3();
   return 0;
}

int getch(void);

// If not installed, uses BIOS and returns getch();
// Else returns pending key (or waits for one if none waiting).
int key_getch(void)
{
   int dummy=0;

   if (!Installed)
      return getch();

   while (!key_checkch())
      dummy++;
   return key_inkey();
}

// Returns the number of seconds this key has been down since last call.
fix key_down_time(int scancode)
{
   Int3();
   return 0;
}

// Returns number of times key has went from up to down since last call.
unsigned int key_down_count(int scancode)
{
   Int3();
   return 0;
}
