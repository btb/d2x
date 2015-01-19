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

enum {
	CON_CLOSED,	//! The console is closed (and not shown)
	CON_CLOSING,	//! The console is still open and visible but closing
	CON_OPENING,	//! The console is visible and opening but not yet fully open
	CON_OPEN	//! The console is open and visible
};

/*! Takes keys from the keyboard and inputs them to the console if the console isVisible().
	If the event was not handled (i.e. WM events or unknown ctrl- or alt-sequences)
	the function returns the event for further processing. */
int CON_Events(int event);
/*! Makes the console visible */
void CON_Show(void);
/*! Hides the console */
void CON_Hide(void);
/*! Returns 1 if the console is visible, 0 else */
int CON_isVisible(void);
/*! Draws the console to the screen if it isVisible()*/
void CON_DrawConsole(void);
/*! Initializes the console */
void CON_Init(void);
/*! Initializes the graphical console */
void CON_InitGFX(int w, int h);
/*! printf for the console */
void CON_Out(const char *str, ...);
/*! Changes the size of the console */
void CON_Resize(int w, int h);


/* Priority levels */
#define CON_CRITICAL -2
#define CON_URGENT   -1
#define CON_NORMAL    0
#define CON_VERBOSE   1
#define CON_DEBUG     2

void con_printf(int level, char *fmt, ...);

/* Console CVars */
/* How discriminating we are about which messages are displayed */
extern cvar_t con_threshold;


#endif /* _CONSOLE_H_ */
