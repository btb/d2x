/* Console, based on an old version of SDL_Console */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_ 1

/*! \mainpage

 \section intro Introduction
 SDL_Console is a console that can be added to any SDL application. It is similar to Quake and other games consoles.
 A console is meant to be a very simple way of interacting with a program and executing commands. You can also have
 more than one console at a time.

 \section docs Documentation
 For a detailed description of all functions see \ref CON_console.h. Remark that functions that have the mark "Internal"
 are only used internally. There's not much use of calling these functions.

 Have Fun!

 \author Garett Banuk <mongoose@mongeese.org> (Original Version)
 \author Clemens Wacha <reflex-2000@gmx.net> (Version 2.x, Documentation)
 \author Boris Lesner <talanthyr@tuxfamily.org> (Package Maintainer)
 \author Bradley Bell <btb@icculus.org> (Descent Version)
 */


#include "cmd.h"
#include "cvar.h"
#include "gr.h"
#include "key.h"

//! Cut the buffer line if it becomes longer than this
#define CON_CHARS_PER_LINE   128
//! Cursor blink frequency in ms
#define CON_BLINK_RATE       500
//! Border in pixels from the most left to the first letter
#define CON_CHAR_BORDER      4
//! Spacing in pixels between lines
#define CON_LINE_SPACE       1
//! Default prompt used at the commandline
#define CON_DEFAULT_PROMPT	"]"
//! Scroll this many lines at a time (when pressing PGUP or PGDOWN)
#define CON_LINE_SCROLL	2
//! Indicator showing that you scrolled up the history
#define CON_SCROLL_INDICATOR "^"
//! Cursor shown if we are in insert mode
#define CON_INS_CURSOR "_"
//! Cursor shown if we are in overwrite mode
#define CON_OVR_CURSOR "|"
//! Defines the default hide key (Hide() the console if pressed)
#define CON_DEFAULT_HIDEKEY	KEY_ESC
//! Defines the opening/closing speed
#define CON_OPENCLOSE_SPEED 50

enum {
	CON_CLOSED,	//! The console is closed (and not shown)
	CON_CLOSING,	//! The console is still open and visible but closing
	CON_OPENING,	//! The console is visible and opening but not yet fully open
	CON_OPEN	//! The console is open and visible
};

/*! This is a struct for each consoles data */
typedef struct console_information_td {
	int Visible;			//! enum that tells which visible state we are in CON_HIDE, CON_SHOW, CON_RAISE, CON_LOWER
	int RaiseOffset;			//! Offset used when scrolling in the console
	int HideKey;			//! the key that can hide the console
	char **ConsoleLines;		//! List of all the past lines
	char **CommandLines;		//! List of all the past commands
	int TotalConsoleLines;		//! Total number of lines in the console
	int ConsoleScrollBack;		//! How much the user scrolled back in the console
	int TotalCommands;		//! Number of commands in the Back Commands
	int LineBuffer;			//! The number of visible lines in the console (autocalculated)
	int VChars;			//! The number of visible characters in one console line (autocalculated)
	char* Prompt;			//! Prompt displayed in command line
	char Command[CON_CHARS_PER_LINE];	//! current command in command line = lcommand + rcommand
	char RCommand[CON_CHARS_PER_LINE];	//! left hand side of cursor
	char LCommand[CON_CHARS_PER_LINE];	//! right hand side of cursor
	char VCommand[CON_CHARS_PER_LINE];	//! current visible command line
	int CursorPos;			//! Current cursor position in CurrentCommand
	int Offset;			//! CommandOffset (first visible char of command) - if command is too long to fit into console
	int InsMode;			//! Insert or Overwrite characters?
	grs_canvas *ConsoleSurface;	//! Canvas of the console
	grs_screen *OutputScreen;	//! This is the screen to draw the console to
	grs_bitmap *BackgroundImage;	//! Background image for the console
	grs_bitmap *InputBackground;	//! Dirty rectangle to draw over behind the users background
	int DispX, DispY;		//! The top left x and y coords of the console on the display screen
#if 0
	unsigned char ConsoleAlpha;	//! The consoles alpha level
#endif
	int CommandScrollBack;		//! How much the users scrolled back in the command lines
	void(*CmdFunction)(struct console_information_td *console, char* command);	//! The Function that is executed if you press <Return> in the console
	char*(*TabFunction)(char* command);	//! The Function that is executed if you press <Tab> in the console
	void(*HideFunction)(void); //! The Function that is executed when the console is hidden
}
ConsoleInformation;

/*! Takes keys from the keyboard and inputs them to the console if the console isVisible().
	If the event was not handled (i.e. WM events or unknown ctrl- or alt-sequences)
	the function returns the event for further processing. */
int CON_Events(int event);
/*! Makes the console visible */
void CON_Show(ConsoleInformation *console);
/*! Hides the console */
void CON_Hide(ConsoleInformation *console);
/*! Returns 1 if the console is visible, 0 else */
int CON_isVisible(ConsoleInformation *console);
/*! Draws the console to the screen if it isVisible()*/
void CON_DrawConsole(ConsoleInformation *console);
/*! Initializes a new console */
ConsoleInformation *CON_Init(grs_font *Font, grs_screen *DisplayScreen, int lines, int x, int y, int w, int h);
/*! printf for the console */
void CON_Out(ConsoleInformation *console, const char *str, ...);
/*! Changes the size of the console */
int CON_Resize(ConsoleInformation *console, int x, int y, int w, int h);


/* Priority levels */
#define CON_CRITICAL -2
#define CON_URGENT   -1
#define CON_NORMAL    0
#define CON_VERBOSE   1
#define CON_DEBUG     2

void con_init(void);
void con_init_gfx(void);
void con_resize(void);
void con_printf(int level, char *fmt, ...);

void con_show(void);
void con_draw(void);
void con_update(void);
int  con_events(int key);

extern int Console_open;

/* Console CVars */
/* How discriminating we are about which messages are displayed */
extern cvar_t con_threshold;


#endif /* _CONSOLE_H_ */
