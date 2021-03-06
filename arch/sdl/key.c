/*
 *
 * SDL keyboard input support
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "inferno.h"
#include "game.h"
#include "event.h"
#include "dxxerror.h"
#include "key.h"
#include "timer.h"
#include "console.h"
#include "u_mem.h"
#include "mouse.h"


#define KEY_BUFFER_SIZE 16

static unsigned char Installed = 0;

//-------- Variable accessed by outside functions ---------
unsigned char           keyd_buffer_type; // 0=No buffer, 1=buffer ASCII, 2=buffer scans
unsigned char           keyd_repeat;
unsigned char           keyd_editor_mode;
volatile unsigned char  keyd_last_pressed;
volatile unsigned char  keyd_last_released;
volatile unsigned char  keyd_pressed[256];
volatile int            keyd_time_when_last_pressed;

typedef struct Key_info {
	ubyte   state;          // state of key 1 == down, 0 == up
	ubyte   last_state;     // previous state of key
	int     counter;        // incremented each time key is down in handler
	fix     timewentdown;   // simple counter incremented each time in interrupt and key is down
	fix     timehelddown;   // counter to tell how long key is down -- gets reset to 0 by key routines
	ubyte   downcount;      // number of key counts key was down
	ubyte   upcount;        // number of times key was released
} Key_info;

typedef struct keyboard {
	unsigned short  keybuffer[KEY_BUFFER_SIZE];
	Key_info        keys[256];
	fix             time_pressed[KEY_BUFFER_SIZE];
	unsigned int    keyhead, keytail;
} keyboard;

static keyboard key_data;

typedef struct key_props {
	char *key_text;
	unsigned char ascii_value;
	unsigned char shifted_ascii_value;
	SDLKey sym;
} key_props;

key_props key_properties[256] = {
{ "",       255,    255,    -1                 },
{ "ESC",    255,    255,    SDLK_ESCAPE        },
{ "1",      '1',    '!',    SDLK_1             },
{ "2",      '2',    '@',    SDLK_2             },
{ "3",      '3',    '#',    SDLK_3             },
{ "4",      '4',    '$',    SDLK_4             },
{ "5",      '5',    '%',    SDLK_5             },
{ "6",      '6',    '^',    SDLK_6             },
{ "7",      '7',    '&',    SDLK_7             },
{ "8",      '8',    '*',    SDLK_8             },
{ "9",      '9',    '(',    SDLK_9             },
{ "0",      '0',    ')',    SDLK_0             },
{ "-",      '-',    '_',    SDLK_MINUS         },
{ "=",      '=',    '+',    SDLK_EQUALS        },
{ "BSPC",   255,    255,    SDLK_BACKSPACE     },
{ "TAB",    255,    255,    SDLK_TAB           },
{ "Q",      'q',    'Q',    SDLK_q             },
{ "W",      'w',    'W',    SDLK_w             },
{ "E",      'e',    'E',    SDLK_e             },
{ "R",      'r',    'R',    SDLK_r             },
{ "T",      't',    'T',    SDLK_t             },
{ "Y",      'y',    'Y',    SDLK_y             },
{ "U",      'u',    'U',    SDLK_u             },
{ "I",      'i',    'I',    SDLK_i             },
{ "O",      'o',    'O',    SDLK_o             },
{ "P",      'p',    'P',    SDLK_p             },
{ "[",      '[',    '{',    SDLK_LEFTBRACKET   },
{ "]",      ']',    '}',    SDLK_RIGHTBRACKET  },
{ "RETURN", 255,    255,    SDLK_RETURN        },
{ "LCTRL",  255,    255,    SDLK_LCTRL         },
{ "A",      'a',    'A',    SDLK_a             },
{ "S",      's',    'S',    SDLK_s             },
{ "D",      'd',    'D',    SDLK_d             },
{ "F",      'f',    'F',    SDLK_f             },
{ "G",      'g',    'G',    SDLK_g             },
{ "H",      'h',    'H',    SDLK_h             },
{ "J",      'j',    'J',    SDLK_j             },
{ "K",      'k',    'K',    SDLK_k             },
{ "L",      'l',    'L',    SDLK_l             },
{ ";",      ';',    ':',    SDLK_SEMICOLON     },
{ "'",     '\'',    '"',    SDLK_QUOTE         },
{ "`",      '`',    '~',    SDLK_BACKQUOTE     },
{ "LSHFT",  255,    255,    SDLK_LSHIFT        },
{ "\\",    '\\',    '|',    SDLK_BACKSLASH     },
{ "Z",      'z',    'Z',    SDLK_z             },
{ "X",      'x',    'X',    SDLK_x             },
{ "C",      'c',    'C',    SDLK_c             },
{ "V",      'v',    'V',    SDLK_v             },
{ "B",      'b',    'B',    SDLK_b             },
{ "N",      'n',    'N',    SDLK_n             },
{ "M",      'm',    'M',    SDLK_m             },
{ ",",      ',',    '<',    SDLK_COMMA         },
{ ".",      '.',    '>',    SDLK_PERIOD        },
{ "/",      '/',    '?',    SDLK_SLASH         },
{ "RSHFT",  255,    255,    SDLK_RSHIFT        },
{ "PAD*",   '*',    255,    SDLK_KP_MULTIPLY   },
{ "LALT",   255,    255,    SDLK_LALT          },
{ "SPC",    ' ',    ' ',    SDLK_SPACE         },
{ "CPSLK",  255,    255,    SDLK_CAPSLOCK      },
{ "F1",     255,    255,    SDLK_F1            },
{ "F2",     255,    255,    SDLK_F2            },
{ "F3",     255,    255,    SDLK_F3            },
{ "F4",     255,    255,    SDLK_F4            },
{ "F5",     255,    255,    SDLK_F5            },
{ "F6",     255,    255,    SDLK_F6            },
{ "F7",     255,    255,    SDLK_F7            },
{ "F8",     255,    255,    SDLK_F8            },
{ "F9",     255,    255,    SDLK_F9            },
{ "F10",    255,    255,    SDLK_F10           },
{ "NMLCK",  255,    255,    SDLK_NUMLOCK       },
{ "SCLK",   255,    255,    SDLK_SCROLLOCK     },
{ "PAD7",   255,    255,    SDLK_KP7           },
{ "PAD8",   255,    255,    SDLK_KP8           },
{ "PAD9",   255,    255,    SDLK_KP9           },
{ "PAD-",   255,    255,    SDLK_KP_MINUS      },
{ "PAD4",   255,    255,    SDLK_KP4           },
{ "PAD5",   255,    255,    SDLK_KP5           },
{ "PAD6",   255,    255,    SDLK_KP6           },
{ "PAD+",   255,    255,    SDLK_KP_PLUS       },
{ "PAD1",   255,    255,    SDLK_KP1           },
{ "PAD2",   255,    255,    SDLK_KP2           },
{ "PAD3",   255,    255,    SDLK_KP3           },
{ "PAD0",   255,    255,    SDLK_KP0           },
{ "PAD.",   255,    255,    SDLK_KP_PERIOD     },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "F11",    255,    255,    SDLK_F11           },
{ "F12",    255,    255,    SDLK_F12           },
{ "F13",    255,    255,    SDLK_F13           },
{ "F14",    255,    255,    SDLK_F14           },
{ "F15",    255,    255,    SDLK_F15           },
#ifdef __APPLE__
{ "F16",    255,    255,    0x106A             },
{ "F17",    255,    255,    0x1040             },
{ "F18",    255,    255,    0x104F             },
{ "F19",    255,    255,    0x1050             },
#else
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
#endif
{ "",       255,    255,    -1                 },
{ "PAUSE",  255,    255,    SDLK_PAUSE         },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "PAD=",   255,    255,    SDLK_KP_EQUALS     },
{ "ENTER",  255,    255,    SDLK_KP_ENTER      },
{ "RCTRL",  255,    255,    SDLK_RCTRL         },
{ "LCMD",   255,    255,    SDLK_LMETA         },
{ "RCMD",   255,    255,    SDLK_RMETA         },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "PAD/",   255,    255,    SDLK_KP_DIVIDE     },
{ "",       255,    255,    -1                 },
{ "PRSCR",  255,    255,    SDLK_PRINT         },
{ "RALT",   255,    255,    SDLK_RALT          },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "HOME",   255,    255,    SDLK_HOME          },
{ "UP",     255,    255,    SDLK_UP            },
{ "PGUP",   255,    255,    SDLK_PAGEUP        },
{ "",       255,    255,    -1                 },
{ "LEFT",   255,    255,    SDLK_LEFT          },
{ "",       255,    255,    -1                 },
{ "RIGHT",  255,    255,    SDLK_RIGHT         },
{ "",       255,    255,    -1                 },
{ "END",    255,    255,    SDLK_END           },
{ "DOWN",   255,    255,    SDLK_DOWN          },
{ "PGDN",   255,    255,    SDLK_PAGEDOWN      },
{ "INS",    255,    255,    SDLK_INSERT        },
{ "DEL",    255,    255,    SDLK_DELETE        },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
{ "",       255,    255,    -1                 },
};

char *key_text[256];


unsigned char key_to_ascii(int keycode )
{
	int shifted;

	shifted = keycode & KEY_SHIFTED;
	keycode &= 0xFF;

	if (shifted)
		return key_properties[keycode].shifted_ascii_value;
	else
		return key_properties[keycode].ascii_value;
}


/* The list of keybindings */
static char *key_binding_list[256];


/* get the action bound to a key, if any */
char *key_binding(ubyte keycode)
{
	return key_binding_list[keycode];
}


ubyte key_find_binding(char *text)
{
	int i;

	for (i = 0; i < 256; i++)
		if (key_binding_list[i] && !strnicmp(key_binding_list[i], text, CMD_MAX_LENGTH))
			return i;

	return 0;
}


/* Write key bindings to file */
void key_write_bindings(CFILE *file)
{
	int i;

	for (i = 0; i < 256; i++)
		if (key_binding_list[i])
			PHYSFSX_printf(file, "bind \"%s\" \"%s\"\n", key_text[i], key_binding_list[i]);
}


int key_get_keycode(const char *text)
{
	int i;

	if ( strlen(text) == 1 )
	{
		char c = text[0];

		for (i = 0; i < 256; i++)
			if (key_properties[i].ascii_value == c || key_properties[i].shifted_ascii_value == c)
				return i;
	}

	for (i = 0; i < 256; i++)
		if (!stricmp(text, key_text[i]))
			return i;

	return -1;
}


/* bind */
void key_cmd_bind(int argc, char **argv)
{
	char buf[CMD_MAX_LENGTH] = "";
	int key = -1;
	int i;

	if (argc < 2)
	{
		con_printf(CON_NORMAL, "key bindings:\n");
		for (i = 0; i < 256; i++) {
			if (!key_binding_list[i])
				continue;
			con_printf(CON_NORMAL, "%s: %s\n", key_text[i], key_binding_list[i]);
		}
		return;
	}

	key = key_get_keycode(argv[1]);

	if (key < 0) {
		con_printf(CON_CRITICAL, "bind: key %s not found\n", argv[1]);
		return;
	}

	if (argc < 3) {
		if (key_binding_list[key])
			con_printf(CON_NORMAL, "%s: %s\n", key_text[key], key_binding_list[key]);
		else
			con_printf(CON_NORMAL, "%s is unbound\n", key_text[key]);
		return;
	}

	for (i = 2; i < argc; i++) {
		if (i > 2)
			strncat(buf, " ", CMD_MAX_LENGTH-strlen(buf)-1);
		strncat(buf, argv[i], CMD_MAX_LENGTH-strlen(buf)-1);
	}

	if (key_binding_list[key])
		d_free(key_binding_list[key]);
	key_binding_list[key] = d_strdup(buf);
}


/* unbind */
void key_cmd_unbind(int argc, char **argv)
{
	unsigned int key;

	if (argc < 2 || argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}

	for (key = 0; key < 256; key++) {
		if (!stricmp(argv[1], key_text[key])) {
			break;
		}
	}

	if (key_binding_list[key])
		d_free(key_binding_list[key]);
	key_binding_list[key] = NULL;
}


void key_handle_binding(int keycode, int state)
{
	if (!key_binding_list[keycode])
		return;

	if (Game_paused)
		return;

	if (Function_mode != FMODE_GAME)
		return;

	if (con_is_visible())
		return;

	if (!state && key_binding_list[keycode][0] == '+')
		cmd_appendf("-%s", &key_binding_list[keycode][1]);
	else if (state)
		cmd_append(key_binding_list[keycode]);
}


void key_handler(SDL_KeyboardEvent *event)
{
	int i, event_key, key_state;

	if (event->keysym.sym != SDLK_UNKNOWN)
		event_key = event->keysym.sym;
	else
		event_key = 0x1000 | event->keysym.scancode; // hardware scancode, definitions in #ifdefs above

	key_state = (event->state == SDL_PRESSED);
	//=====================================================
	//Here a translation from win keycodes to mac keycodes!
	//=====================================================

	for (i = 255; i >= 0; i--) {
		if (key_properties[i].sym == event_key) {
			vkey_handler(i, key_state);
		}
	}
}


void vkey_handler(int keycode, int state)
{
	unsigned char temp;
	Key_info *key;

	key = &(key_data.keys[keycode]);

	if (state) {
		// Key going down
		keyd_last_pressed = keycode;
		keyd_time_when_last_pressed = timer_get_fixed_seconds();

		if (!keyd_pressed[keycode]) {
			// First time down
			key_handle_binding(keycode, 1);
			keyd_pressed[keycode] = 1;
			key->downcount += state;
			key->state = 1;
			key->timewentdown = keyd_time_when_last_pressed;
			key->counter++;
		}
	} else {
		// Key going up
		key_handle_binding(keycode, 0);
		keyd_pressed[keycode] = 0;
		keyd_last_released = keycode;
		key->upcount += key->state;
		key->state = 0;
		key->counter = 0;
		key->timehelddown += timer_get_fixed_seconds() - key->timewentdown;
	}

	if ( (state && !key->last_state) || (keyd_repeat && state && key->last_state) ) {
		if ( keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT])
			keycode |= KEY_SHIFTED;
		if ( keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT])
			keycode |= KEY_ALTED;
		if ( keyd_pressed[KEY_LCTRL] || keyd_pressed[KEY_RCTRL])
			keycode |= KEY_CTRLED;
		if ( keyd_pressed[KEY_LMETA] || keyd_pressed[KEY_RMETA])
			keycode |= KEY_METAED;
		if ( keyd_pressed[KEY_DELETE] )
			keycode |= KEY_DEBUGGED;

		temp = key_data.keytail + 1;
		if ( temp >= KEY_BUFFER_SIZE ) temp=0;
		if (temp!=key_data.keyhead)	{
			key_data.keybuffer[key_data.keytail] = keycode;
			key_data.time_pressed[key_data.keytail] = keyd_time_when_last_pressed;
			key_data.keytail = temp;
		}
	}
	key->last_state = state;
}


void key_close()
{
	int i;

	for (i = 0; i < 256; i++)
		if (key_binding_list[i])
			d_free(key_binding_list[i]);

	for (i = 0; i < MOUSE_MAX_BUTTONS; i++) {
		d_free(key_text[KEY_MB1 + i]);
	}

	Installed = 0;
}

void key_init()
{
	int i;

	if (Installed) return;

	Installed=1;

	keyd_time_when_last_pressed = timer_get_fixed_seconds();
	keyd_buffer_type = 1;
	keyd_repeat = 1;

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	for(i=0; i<256; i++)
		key_text[i] = key_properties[i].key_text;

	for (i = 0; i < MOUSE_MAX_BUTTONS; i++) {
		char temp[10];
		sprintf(temp, "MB%d", i + 1);
		key_text[KEY_MB1 + i] = d_strdup(temp);
	}

	cmd_addcommand("bind", key_cmd_bind,     "bind <key> <commands>\n" "    bind <commands> to <key>\n"
	                                         "bind <key>\n"            "    show the current binding for <key>\n"
	                                         "bind\n"                  "    show all key bindings");
	cmd_addcommand("unbind", key_cmd_unbind, "unbind <key>\n"          "    remove binding from <key>");

	// Clear the keyboard array
	key_flush();
	atexit(key_close);
}

void key_flush()
{
 	int i;
	fix curtime;

	if (!Installed)
		key_init();

	key_data.keyhead = key_data.keytail = 0;

	//Clear the keyboard buffer
	for (i=0; i<KEY_BUFFER_SIZE; i++ ) {
		key_data.keybuffer[i] = 0;
		key_data.time_pressed[i] = 0;
	}

//use gettimeofday here:
	curtime = timer_get_fixed_seconds();

	for (i=0; i<256; i++) {
		keyd_pressed[i] = 0;
		key_data.keys[i].state = 1;
		key_data.keys[i].last_state = 0;
		key_data.keys[i].timewentdown = curtime;
		key_data.keys[i].downcount = 0;
		key_data.keys[i].upcount = 0;
		key_data.keys[i].timehelddown = 0;
		key_data.keys[i].counter = 0;
	}
}

int add_one(int n)
{
	n++;
	if ( n >= KEY_BUFFER_SIZE ) n = 0;

	return n;
}

int key_checkch()
{
	int is_one_waiting = 0;

	event_poll();
	if (key_data.keytail != key_data.keyhead)
		is_one_waiting = 1;

	return is_one_waiting;
}

int key_inkey()
{
	int key = 0;

	if (!Installed)
		key_init();

	event_poll();
	if (key_data.keytail!=key_data.keyhead) {
		key = key_data.keybuffer[key_data.keyhead];
		key_data.keyhead = add_one(key_data.keyhead);
	}
	else
		timer_delay(1);

	return key;
}

int key_inkey_time(fix * time)
{
	int key = 0;

	if (!Installed)
		key_init();

	event_poll();
	if (key_data.keytail != key_data.keyhead) {
		key = key_data.keybuffer[key_data.keyhead];
		*time = key_data.time_pressed[key_data.keyhead];
		key_data.keyhead = add_one(key_data.keyhead);
	}

	return key;
}

int key_peekkey()
{
	int key = 0;

	event_poll();
	if (key_data.keytail != key_data.keyhead)
		key = key_data.keybuffer[key_data.keyhead];

	return key;
}

int key_getch()
{
	int dummy = 0;

	if (!Installed)
		return 0;
//		return getch();

	while (!key_checkch())
		dummy++;

	return key_inkey();
}

unsigned int key_get_shift_status()
{
	unsigned int shift_status = 0;

	if ( keyd_pressed[KEY_LSHIFT] || keyd_pressed[KEY_RSHIFT] )
		shift_status |= KEY_SHIFTED;

	if ( keyd_pressed[KEY_LALT] || keyd_pressed[KEY_RALT] )
		shift_status |= KEY_ALTED;

	if ( keyd_pressed[KEY_LCTRL] || keyd_pressed[KEY_RCTRL] )
		shift_status |= KEY_CTRLED;

	if ( keyd_pressed[KEY_LMETA] || keyd_pressed[KEY_RMETA] )
		shift_status |= KEY_METAED;

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

	event_poll();
	if ((scancode<0) || (scancode>255)) return 0;

	if (!keyd_pressed[scancode]) {
		time_down = key_data.keys[scancode].timehelddown;
		key_data.keys[scancode].timehelddown = 0;
	} else {
		time = timer_get_fixed_seconds();
		time_down = time - key_data.keys[scancode].timewentdown;
		key_data.keys[scancode].timewentdown = time;
	}

	return time_down;
}

unsigned int key_down_count(int scancode)
{
	int n;

	event_poll();
	if ((scancode<0) || (scancode>255)) return 0;

	n = key_data.keys[scancode].downcount;
	key_data.keys[scancode].downcount = 0;

	return n;
}

unsigned int key_up_count(int scancode)
{
	int n;

	event_poll();
	if ((scancode<0) || (scancode>255)) return 0;

	n = key_data.keys[scancode].upcount;
	key_data.keys[scancode].upcount = 0;

	return n;
}
