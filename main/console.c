/* $Id: console.c,v 1.18 2003-11-26 12:39:00 btb Exp $ */
/*
 *
 * Code for controlling the console
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifndef _WIN32_WCE
#include <fcntl.h>
#endif
#include <ctype.h>

#include <SDL.h>
#ifdef CONSOLE
#include "CON_console.h"
#endif

#include "pstypes.h"
#include "u_mem.h"
#include "error.h"
#include "console.h"
#include "cmd.h"
#include "cvar.h"
#include "gr.h"
#include "gamefont.h"
#include "pcx.h"
#include "cfile.h"

#ifndef __MSDOS__
int text_console_enabled = 1;
#else
int isvga();
#define text_console_enabled (!isvga())
#endif

int Console_open = 0;

/* Console specific cvars */
/* How discriminating we are about which messages are displayed */
cvar_t con_threshold = {"con_threshold", "0",};

/* Private console stuff */
#define CON_NUM_LINES 40
#if 0
#define CON_LINE_LEN 40
static char con_display[40][40];
static int  con_line; /* Current display line */
#endif

#ifdef CONSOLE
static int con_initialized;

ConsoleInformation *Console;

void con_parse(ConsoleInformation *console, char *command);
void con_hide();


/* Free the console */
void con_free(void)
{
	if (con_initialized)
		CON_Free(Console);
	con_initialized = 0;
}
#endif


/* Initialise the console */
void con_init(void)
{
	grs_screen fake_screen;
	grs_font   fake_font;

	fake_screen.sc_w = 320;
	fake_screen.sc_h = 200;
	fake_font.ft_w = 5;
	fake_font.ft_h = 5;

	Console = CON_Init(&fake_font, &fake_screen, CON_NUM_LINES, 0, 0, 320, 200);

	CON_SetExecuteFunction(Console, con_parse);
	CON_SetHideFunction(Console, con_hide);


	cmd_init();

	/* Initialise the cvars */
	cvar_registervariable (&con_threshold);

	con_initialized = 1;

	atexit(con_free);
}

#ifdef CONSOLE

#define CON_BG_HIRES (cfexist("scoresb.pcx")?"scoresb.pcx":"scores.pcx")
#define CON_BG_LORES (cfexist("scores.pcx")?"scores.pcx":"scoresb.pcx") // Mac datafiles only have scoresb.pcx
#define CON_BG ((SWIDTH>=640)?CON_BG_HIRES:CON_BG_LORES)

void con_background(char *filename)
{
	int pcx_error;
	grs_bitmap bmp;
	ubyte pal[256*3];

	gr_init_bitmap_data(&bmp);
	pcx_error = pcx_read_bitmap(filename, &bmp, BM_LINEAR, pal);
	Assert(pcx_error == PCX_ERROR_NONE);
	gr_remap_bitmap_good(&bmp, pal, -1, -1);
	CON_Background(Console, &bmp);
	gr_free_bitmap_data(&bmp);
}


void con_init_gfx(void)
{
	CON_Font(Console, SMALL_FONT, gr_getcolor(63, 63, 63), -1);
	CON_Transfer(Console, grd_curscreen, 0, 0, SWIDTH, SHEIGHT / 2);

	con_background(CON_BG);
}
#endif


void con_resize(void)
{
#ifdef CONSOLE
	CON_Font(Console, SMALL_FONT, gr_getcolor(63, 63, 63), -1);
	CON_Resize(Console, 0, 0, SWIDTH, SHEIGHT / 2);
	con_background(CON_BG);
#endif
}

/* Print a message to the console */
void con_printf(int priority, char *fmt, ...)
{
	va_list arglist;
	char buffer[2048];

	if (priority <= ((int)con_threshold.value))
	{
		va_start (arglist, fmt);
		vsprintf (buffer,  fmt, arglist);
		va_end (arglist);

#ifdef CONSOLE
		if (con_initialized)
			CON_Out(Console, buffer);
#endif

/*		for (i=0; i<l; i+=CON_LINE_LEN,con_line++)
		{
			memcpy(con_display, &buffer[i], min(80, l-i));
		}*/

		if (text_console_enabled)
		{
			/* Produce a sanitised version and send it to the standard output */
			char *p1, *p2;

			p1 = p2 = buffer;
			do
				switch (*p1)
				{
				case CC_COLOR:
				case CC_LSPACING:
					p1++;
				case CC_UNDERLINE:
					p1++;
					break;
				default:
					*p2++ = *p1++;
				}
			while (*p1);
			*p2 = 0;

			printf("%s", buffer);
		}
	}
}

/* Check for new console input. If it's there, use it */
void con_update(void)
{
#if 0
	char buffer[CMD_MAX_LENGTH], *t;

	/* Check for new input */
	t = fgets(buffer, sizeof(buffer), stdin);
	if (t == NULL) return;

	cmd_parse(buffer);
#endif
	con_draw();
}


int con_events(int key)
{
#ifdef CONSOLE
	return CON_Events(key);
#else
	return key;
#endif
}


/* Draw the console */
void con_draw(void)
{
#ifdef CONSOLE
	CON_DrawConsole(Console);
#else
#if 0
	char buffer[CON_LINE_LEN+1];
	int i,j;
	for (i = con_line, j=0; j < 20; i = (i+1) % CON_NUM_LINES, j++)
	{
		memcpy(buffer, con_display[i], CON_LINE_LEN);
		buffer[CON_LINE_LEN] = 0;
		gr_string(1,j*10,buffer);
	}
#endif
#endif
}

/* Show the console */
void con_show(void)
{
	Console_open = 1;
#ifdef CONSOLE
	CON_Show(Console);
	CON_Topmost(Console);
#endif
}

void con_hide(void)
{
	Console_open = 0;
}

#ifdef CONSOLE
void con_parse(ConsoleInformation *console, char *command)
{
	cmd_parse(command);
}
#endif
