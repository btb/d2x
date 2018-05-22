

#include <SDL.h>

#include "key.h"
#include "timer.h"
#include "error.h"


#define KEY_BUFFER_SIZE 16

//-------- Variable accessed by outside functions ---------
char keyd_buffer_type;    // 0=No buffer, 1=buffer ASCII, 2=buffer scans
char keyd_repeat;
unsigned char keyd_editor_mode;
volatile unsigned char keyd_last_pressed;
volatile unsigned char keyd_last_released;
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

unsigned char ascii_table[128] =
{ 255, 255, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',255,255,
   'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 255, 255,
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 39, '`',
   255, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 255,'*',
   255, ' ', 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,255,255,
   255, 255, 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255 };

unsigned char shifted_ascii_table[128] =
{ 255, 255, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',255,255,
   'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 255, 255,
   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
   255, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 255,255,
   255, ' ', 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,255,255,
   255, 255, 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255 };

unsigned char sdl_to_dos_scancode[] = {
   255, 255, 255, 255, KEY_A, KEY_B, KEY_C, KEY_D,
   KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L,
   KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T,
   KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z, KEY_1, KEY_2,
   KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
   KEY_ENTER, KEY_ESC, KEY_BACKSP, KEY_TAB,
   KEY_SPACEBAR, KEY_MINUS, KEY_EQUAL, KEY_LBRACKET,
   KEY_RBRACKET, KEY_SLASH, KEY_SLASH, KEY_SEMICOL,
   KEY_RAPOSTRO, KEY_LAPOSTRO, KEY_COMMA, KEY_PERIOD,
   KEY_DIVIDE, KEY_CAPSLOCK, KEY_F1, KEY_F2, KEY_F3, KEY_4, KEY_F5, KEY_F6,
   KEY_F7, KEY_F8, KEY_F9, KEY_F10,
   KEY_F11, KEY_F12, KEY_PRINT_SCREEN, KEY_SCROLLOCK,
   KEY_PAUSE, KEY_INSERT, KEY_HOME, KEY_PAGEUP,
   KEY_DELETE, KEY_END, KEY_PAGEDOWN, KEY_RIGHT,
   KEY_LEFT, KEY_DOWN, KEY_UP, KEY_NUMLOCK,
   KEY_PADDIVIDE, KEY_PADMULTIPLY, KEY_PADMINUS, KEY_PADPLUS,
   KEY_PADENTER, KEY_PAD1, KEY_PAD2, KEY_PAD3,
   KEY_PAD4, KEY_PAD5, KEY_PAD6, KEY_PAD7,
   KEY_PAD8, KEY_PAD9, KEY_PAD0, KEY_PADPERIOD,
   KEY_SLASH, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   KEY_LCTRL, KEY_LSHIFT, KEY_LALT, 255, KEY_RCTRL, KEY_RSHIFT, KEY_RALT, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255,
   255, 255, 255, 255, 255, 255, 255, 255
};

// Internal prototypes
void key_clear_bios_buffer_all(void);
void event_poll(void);


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
   int shifted;

   shifted = keycode & KEY_SHIFTED;
   keycode &= 0xFF;

   if (keycode >= 127)
      return 255;

   if (shifted)
      return shifted_ascii_table[keycode];
   else
      return ascii_table[keycode];
}

void key_clear_bios_buffer_all(void)
{
   event_poll();
}

void key_clear_bios_buffer(void)
{
   event_poll();
}

void key_flush()
{
   int i;
   fix CurTime;

   // Clear the BIOS buffer
   key_clear_bios_buffer();

   key_data.keyhead = key_data.keytail = 0;

   // Clear the keyboard buffer
   for (i = 0; i < KEY_BUFFER_SIZE; i++) {
      key_data.keybuffer[i] = 0;
      key_data.time_pressed[i] = 0;
   }

   // Clear the keyboard array

   CurTime = timer_get_fixed_secondsX();

   for (i = 0; i < 256; i++) {
      keyd_pressed[i] = 0;
      key_data.TimeKeyWentDown[i] = CurTime;
      key_data.TimeKeyHeldDown[i] = 0;
      key_data.NumDowns[i] = 0;
      key_data.NumUps[i] = 0;
   }
}

int add_one( int n )
{
   n++;
   if (n >= KEY_BUFFER_SIZE)
      n = 0;
   return n;
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
   int key = 0;

   key_clear_bios_buffer();

   if (key_data.keytail != key_data.keyhead) {
      key = key_data.keybuffer[key_data.keyhead];
      key_data.keyhead = add_one(key_data.keyhead);
   }

   return key;
}

int key_inkey_time(fix *time)
{
   int key = 0;

   key_clear_bios_buffer();

   if (key_data.keytail != key_data.keyhead) {
      key = key_data.keybuffer[key_data.keyhead];
      *time = key_data.time_pressed[key_data.keyhead];
      key_data.keyhead = add_one(key_data.keyhead);
   }

   return key;
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

unsigned int key_get_shift_status()
{
   unsigned int shift_status = 0;

   key_clear_bios_buffer();

   if ( keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT] )
      shift_status |= KEY_SHIFTED;

   if ( keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT] )
      shift_status |= KEY_ALTED;

   if ( keyd_pressed[KEY_LCTRL] || keyd_pressed[KEY_RCTRL] )
      shift_status |= KEY_CTRLED;

#ifndef NDEBUG
   if (keyd_pressed[KEY_DELETE])
      shift_status |=KEY_DEBUGGED;
#endif

   return shift_status;
}

// Returns the number of seconds this key has been down since last call.
fix key_down_time(int scancode)
{
   fix time_down, time;

   if ((scancode<0)|| (scancode>255))
      return 0;

#ifndef NDEBUG
   if (keyd_editor_mode && key_get_shift_status() )
      return 0;
#endif

   if ( !keyd_pressed[scancode] )   {
      time_down = key_data.TimeKeyHeldDown[scancode];
      key_data.TimeKeyHeldDown[scancode] = 0;
   } else   {
      time = timer_get_fixed_secondsX();
      time_down =  time - key_data.TimeKeyWentDown[scancode];
      key_data.TimeKeyWentDown[scancode] = time;
   }

   return time_down;
}

// Returns number of times key has went from up to down since last call.
unsigned int key_down_count(int scancode)
{
   int n;

   if ((scancode < 0) || (scancode > 255))
      return 0;

   n = key_data.NumDowns[scancode];
   key_data.NumDowns[scancode] = 0;

   return n;
}

void key_handler(SDL_KeyboardEvent *event)
{
   unsigned char scancode, breakbit, temp;
   unsigned short keycode;

   scancode = sdl_to_dos_scancode[event->keysym.scancode];
   breakbit = (event->state == SDL_RELEASED);

   if (breakbit) {
      // Key going up
      keyd_last_released = scancode;
      keyd_pressed[scancode] = 0;
      key_data.NumUps[scancode]++;
      temp = 0;
      temp |= keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT];
      temp |= keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT];
      temp |= keyd_pressed[KEY_LCTRL] || keyd_pressed[KEY_RCTRL];
#ifndef NDEBUG
      temp |= keyd_pressed[KEY_DELETE];
      if ( !(keyd_editor_mode && temp) )
#endif      // NOTICE LINK TO ABOVE IF!!!!
         key_data.TimeKeyHeldDown[scancode] += timer_get_fixed_secondsX() - key_data.TimeKeyWentDown[scancode];
   } else {
      // Key going down
      keyd_last_pressed = scancode;
      keyd_time_when_last_pressed = timer_get_fixed_secondsX();
      if (!keyd_pressed[scancode]) {
         // First time down
         key_data.TimeKeyWentDown[scancode] = timer_get_fixed_secondsX();
         keyd_pressed[scancode] = 1;
         key_data.NumDowns[scancode]++;
#if 0 //ndef NDEBUG
         if ( (keyd_pressed[KEY_LSHIFT]) && (scancode == KEY_BACKSP) ) {
            keyd_pressed[KEY_LSHIFT] = 0;
            Int5();
         }
#endif
      } else if (!keyd_repeat) {
         // Don't buffer repeating key if repeat mode is off
         scancode = 0xAA;
      }

      if ( scancode!=0xAA ) {
         keycode = scancode;

         if ( keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT] )
            keycode |= KEY_SHIFTED;

         if ( keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT] )
            keycode |= KEY_ALTED;

         if ( keyd_pressed[KEY_LCTRL] || keyd_pressed[KEY_RCTRL] )
            keycode |= KEY_CTRLED;

#ifndef NDEBUG
         if ( keyd_pressed[KEY_DELETE] )
            keycode |= KEY_DEBUGGED;
#endif

         temp = key_data.keytail+1;
         if ( temp >= KEY_BUFFER_SIZE ) temp=0;

         if (temp!=key_data.keyhead)   {
            key_data.keybuffer[key_data.keytail] = keycode;
            key_data.time_pressed[key_data.keytail] = keyd_time_when_last_pressed;
            key_data.keytail = temp;
         }
      }
   }
}

void mouse_button_handler(SDL_MouseButtonEvent *event);

void event_poll(void)
{
   SDL_Event event;

   while (SDL_PollEvent(&event)) {
      switch(event.type) {
         case SDL_KEYDOWN:
         case SDL_KEYUP:
            key_handler((SDL_KeyboardEvent *)&event);
            break;
         case SDL_MOUSEBUTTONDOWN:
         case SDL_MOUSEBUTTONUP:
            mouse_button_handler((SDL_MouseButtonEvent *)&event);
            break;
         case SDL_MOUSEMOTION:
            //            mouse_motion_handler((SDL_MouseMotionEvent *)&event);
            break;
         case SDL_JOYBUTTONDOWN:
         case SDL_JOYBUTTONUP:
            Int3();
            //            joy_button_handler((SDL_JoyButtonEvent *)&event);
            break;
         case SDL_JOYAXISMOTION:
            Int3();
            //            joy_axis_handler((SDL_JoyAxisEvent *)&event);
            break;
         case SDL_JOYHATMOTION:
            Int3();
            //            joy_hat_handler((SDL_JoyHatEvent *)&event);
            break;
         default:
//            Int3();
         case SDL_JOYBALLMOTION:
         case SDL_WINDOWEVENT:
            break;
//         case SDL_QUIT: {
//            void quit_request(void);
//            quit_request();
//         } break;
      }
   }
}
