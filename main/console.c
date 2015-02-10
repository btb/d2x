/*
 *  Code for controlling the console
 *  Based on an early version of SDL_Console
 *
 *  Written By: Garrett Banuk <mongoose@mongeese.org>
 *  Code Cleanup and heavily extended by: Clemens Wacha <reflex-2000@gmx.net>
 *  Ported to use native Descent interfaces by: Bradley Bell <btb@icculus.org>
 *
 *  This is free, just be sure to give us credit when using it
 *  in any of your programs.
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#ifndef _WIN32_WCE
#include <fcntl.h>
#endif
#include <ctype.h>
#include <unistd.h>

#include "inferno.h"
#include "u_mem.h"
#include "gr.h"
#include "key.h"
#include "timer.h"
#include "pstypes.h"
#include "error.h"
#include "cfile.h"


#ifndef __MSDOS__
int text_console_enabled = 1;
#else
int isvga();
#define text_console_enabled (!isvga())
#endif


/* How discriminating we are about which messages are displayed */
cvar_t con_threshold = {"con_threshold", "0",};

#define CON_BG_HIRES (cfexist("scoresb.pcx")?"scoresb.pcx":"scores.pcx")
#define CON_BG_LORES (cfexist("scores.pcx")?"scores.pcx":"scoresb.pcx") // Mac datafiles only have scoresb.pcx
#define CON_BG ((SWIDTH>=640)?CON_BG_HIRES:CON_BG_LORES)

#define CON_NUM_LINES           128
// Cut the buffer line if it becomes longer than this
#define CON_CHARS_PER_LINE      128
// Border in pixels from the most left to the first letter
#define CON_CHAR_BORDER         4
// Spacing in pixels between lines
#define CON_LINE_SPACE          1
// Scroll this many lines at a time (when pressing PGUP or PGDOWN)
#define CON_LINE_SCROLL         2
// Indicator showing that you scrolled up the history
#define CON_SCROLL_INDICATOR    "^"
// Defines the default hide key (Hide() the console if pressed)
#define CON_DEFAULT_HIDEKEY	KEY_ESC
// Defines the opening/closing speed
#define CON_OPENCLOSE_SPEED 50


/* The console's data */
static int Visible;             // Enum that tells which visible state we are in CON_HIDE, CON_SHOW, CON_RAISE, CON_LOWER
static int RaiseOffset;         // Offset used when scrolling in the console
static int HideKey;             // The key that can hide the console
static char **ConsoleLines;     // List of all the past lines
static int TotalConsoleLines;   // Total number of lines in the console
static int ConsoleScrollBack;   // How much the user scrolled back in the console
static int LineBuffer;          // The number of visible lines in the console (autocalculated)
static int VChars;              // The number of visible characters in one console line (autocalculated)
static grs_canvas *ConsoleSurface;  // Canvas of the console
static grs_bitmap *BackgroundImage; // Background image for the console
static grs_bitmap *InputBackground; // Dirty rectangle to draw over behind the users background

/* console is ready to be written to */
static int con_initialized;


/* Internals */
void CON_UpdateOffset(void);
/* Frees all the memory loaded by the console */
static void CON_Free(void);
int CON_Background(grs_bitmap *image);
/* Sets font info for the console */
void CON_Font(grs_font *font, int fg, int bg);
/* Set the key, that invokes a CON_Hide() after press. default is ESCAPE and you can always hide using
 ESCAPE and the HideKey. compared against event->key.keysym.sym !! */
void CON_SetHideKey(int key);
/* makes newline (same as printf("\n") or CON_Out("\n") ) */
void CON_NewLineConsole(void);
/* updates console after resize etc. */
void CON_UpdateConsole(void);
/* Called if you press Ctrl-L (deletes the History) */
void Clear_History(void);


/* Takes keys from the keyboard and inputs them to the console
 * If the event was not handled (i.e. WM events or unknown ctrl-shift
 * sequences) the function returns the event for further processing. */
int CON_Events(int event)
{
	if (!CON_isVisible())
		return event;

	if (event & KEY_CTRLED)
	{
		// CTRL pressed
		switch (event & ~KEY_CTRLED)
		{
			case KEY_A:
				Cursor_Home();
				break;
			case KEY_E:
				Cursor_End();
				break;
			case KEY_C:
				Clear_Command();
				break;
			case KEY_L:
				Clear_History();
				CON_UpdateConsole();
				break;
			default:
				return event;
		}
	}
	else if (event & KEY_ALTED)
	{
		// the console does not handle ALT combinations!
		return event;
	}
	else
	{
		// first of all, check if the console hide key was pressed
		if (event == HideKey)
		{
			CON_Hide();
			return 0;
		}
		switch (event & 0xff)
		{
			case KEY_LSHIFT:
			case KEY_RSHIFT:
				return event;
			case KEY_HOME:
				if(event & KEY_SHIFTED)
				{
					ConsoleScrollBack = LineBuffer-1;
					CON_UpdateConsole();
				} else {
					Cursor_Home();
				}
				break;
			case KEY_END:
				if(event & KEY_SHIFTED)
				{
					ConsoleScrollBack = 0;
					CON_UpdateConsole();
				} else {
					Cursor_End();
				}
				break;
			case KEY_PAGEUP:
				ConsoleScrollBack += CON_LINE_SCROLL;
				if(ConsoleScrollBack > LineBuffer-1)
					ConsoleScrollBack = LineBuffer-1;
				CON_UpdateConsole();
				break;
			case KEY_PAGEDOWN:
				ConsoleScrollBack -= CON_LINE_SCROLL;
				if(ConsoleScrollBack < 0)
					ConsoleScrollBack = 0;
				CON_UpdateConsole();
				break;
			case KEY_UP:
				Command_Up();
				break;
			case KEY_DOWN:
				Command_Down();
				break;
			case KEY_LEFT:
				Cursor_Left();
				break;
			case KEY_RIGHT:
				Cursor_Right();
				break;
			case KEY_BACKSP:
				Cursor_BSpace();
				break;
			case KEY_DELETE:
				Cursor_Del();
				break;
			case KEY_INSERT:
				InsMode = 1-InsMode;
				break;
			case KEY_TAB:
				CON_TabCompletion();
				break;
			case KEY_ENTER:
				CON_Execute();
				break;
			case KEY_LAPOSTRO:
				// deactivate Console
				CON_Hide();
				return 0;
			default:
			{
				unsigned char character = key_to_ascii(event);

				if (character == 255)
					break;
				if (InsMode)
					Cursor_Add(character);
				else {
					Cursor_Add(character);
					Cursor_Del();
				}
			}
		}
	}
	return 0;
}


/* Updates the console buffer */
void CON_UpdateConsole(void)
{
	int loop;
	int loop2;
	int Screenlines;
	grs_canvas *canv_save;

	/* Due to the Blits, the update is not very fast: So only update if it's worth it */
	if (!CON_isVisible())
		return;

	Screenlines = ConsoleSurface->cv_h / (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h);

	canv_save = grd_curcanv;
	gr_set_current_canvas(ConsoleSurface);

	/* draw the background image if there is one */
	if (BackgroundImage)
		gr_bitmap(0, 0, BackgroundImage);

	// now draw text from last but second line to top
	for (loop = 0; loop < Screenlines-1 && loop < LineBuffer - ConsoleScrollBack; loop++) {
		if (ConsoleScrollBack != 0 && loop == 0)
			for (loop2 = 0; loop2 < (VChars / 5) + 1; loop2++)
			{
				gr_string(CON_CHAR_BORDER + (loop2*5*ConsoleSurface->cv_font->ft_w), (Screenlines - loop - 2) * (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h), CON_SCROLL_INDICATOR);
			}
		else
		{
			gr_string(CON_CHAR_BORDER, (Screenlines - loop - 2) * (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h), ConsoleLines[ConsoleScrollBack + loop]);
		}
	}

	gr_set_current_canvas(canv_save);
}


void CON_UpdateOffset(void)
{
	switch (Visible) {
		case CON_CLOSING:
			RaiseOffset -= CON_OPENCLOSE_SPEED;
			if(RaiseOffset <= 0) {
				RaiseOffset = 0;
				Visible = CON_CLOSED;
			}
			break;
		case CON_OPENING:
			RaiseOffset += CON_OPENCLOSE_SPEED;
			if(RaiseOffset >= ConsoleSurface->cv_h) {
				RaiseOffset = ConsoleSurface->cv_h;
				Visible = CON_OPEN;
			}
			break;
		case CON_OPEN:
		case CON_CLOSED:
			break;
	}
}


/* Draws the console buffer to the screen if the console is "visible" */
void CON_DrawConsole(void)
{
	grs_canvas *canv_save;
	grs_bitmap *clip;

	/* only draw if console is visible: here this means, that the console is not CON_CLOSED */
	if (Visible == CON_CLOSED)
		return;

	/* Update the scrolling offset */
	CON_UpdateOffset();

	canv_save = grd_curcanv;

	/* Update the command line since it has a blinking cursor */
	gr_set_current_canvas(ConsoleSurface);

	// restore InputBackground
	gr_bitmap(0, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, InputBackground);

	DrawCommandLine(ConsoleSurface->cv_h);

	gr_set_current_canvas(&grd_curscreen->sc_canvas);

	clip = gr_create_sub_bitmap(&ConsoleSurface->cv_bitmap, 0, ConsoleSurface->cv_h - RaiseOffset, ConsoleSurface->cv_w, RaiseOffset);

	gr_bitmap(0, 0, clip);
	gr_free_sub_bitmap(clip);

	gr_set_current_canvas(canv_save);
}


/* Initializes the console */
void CON_Init()
{
	int loop;

	Visible = CON_CLOSED;
	RaiseOffset = 0;
	ConsoleLines = NULL;
	TotalConsoleLines = 0;
	ConsoleScrollBack = 0;
	BackgroundImage = NULL;
	InsMode = 1;
	HideKey = CON_DEFAULT_HIDEKEY;

	/* load the console surface */
	ConsoleSurface = NULL;

	/* Load the dirty rectangle for user input */
	InputBackground = NULL;

	VChars = CON_CHARS_PER_LINE - 1;
	LineBuffer = CON_NUM_LINES;

	ConsoleLines = (char **)d_malloc(sizeof(char *) * LineBuffer);
	for (loop = 0; loop <= LineBuffer - 1; loop++) {
		ConsoleLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
	}

	cli_init();
	cmd_init();

	/* Initialise the cvars */
	cvar_registervariable (&con_threshold);

	con_initialized = 1;

	atexit(CON_Free);
}


void gr_init_bitmap_alloc( grs_bitmap *bm, int mode, int x, int y, int w, int h, int bytesperline);
void CON_InitGFX(int w, int h)
{
	int pcx_error;
	grs_bitmap bmp;
	ubyte pal[256*3];

	if (ConsoleSurface) {
		/* resize console surface */
		gr_free_bitmap_data(&ConsoleSurface->cv_bitmap);
		gr_init_bitmap_alloc(&ConsoleSurface->cv_bitmap, BM_LINEAR, 0, 0, w, h, w);
	} else {
		/* load the console surface */
		ConsoleSurface = gr_create_canvas(w, h);
	}

	/* Load the consoles font */
	CON_Font(SMALL_FONT, gr_find_closest_color(29,29,47), -1);

	/* make sure that the size of the console is valid */
	if (w > grd_curscreen->sc_w || w < ConsoleSurface->cv_font->ft_w * 32)
		w = grd_curscreen->sc_w;
	if (h > grd_curscreen->sc_h || h < ConsoleSurface->cv_font->ft_h)
		h = grd_curscreen->sc_h;

	/* Load the dirty rectangle for user input */
	if (InputBackground)
		gr_free_bitmap(InputBackground);
	InputBackground = gr_create_bitmap(w, ConsoleSurface->cv_font->ft_h);

	/* calculate the number of visible characters in the command line */
#if 0 // doesn't work because proportional font
	VChars = (w - CON_CHAR_BORDER) / ConsoleSurface->cv_font->ft_w;
	if (VChars >= CON_CHARS_PER_LINE)
		VChars = CON_CHARS_PER_LINE - 1;
#endif

	gr_init_bitmap_data(&bmp);
	pcx_error = pcx_read_bitmap(CON_BG, &bmp, BM_LINEAR, pal);
	Assert(pcx_error == PCX_ERROR_NONE);
	gr_remap_bitmap_good(&bmp, pal, -1, -1);
	CON_Background(&bmp);
	gr_free_bitmap_data(&bmp);
}


/* Makes the console visible */
void CON_Show(void)
{
	Visible = CON_OPENING;
	CON_UpdateConsole();
}


/* Hides the console (make it invisible) */
void CON_Hide(void)
{
	Visible = CON_CLOSING;
	key_flush();
}


/* tells wether the console is visible or not */
int CON_isVisible(void)
{
	return((Visible == CON_OPEN) || (Visible == CON_OPENING));
}


/* Frees all the memory loaded by the console */
static void CON_Free(void)
{
	int i;

	for (i = 0; i <= LineBuffer - 1; i++) {
		d_free(ConsoleLines[i]);
	}
	d_free(ConsoleLines);

	ConsoleLines = NULL;

	if (ConsoleSurface)
		gr_free_canvas(ConsoleSurface);
	ConsoleSurface = NULL;

	if (BackgroundImage)
		gr_free_bitmap(BackgroundImage);
	BackgroundImage = NULL;

	if (InputBackground)
		gr_free_bitmap(InputBackground);
	InputBackground = NULL;

	con_initialized = 0;
}


/* Increments the console lines */
void CON_NewLineConsole(void)
{
	int loop;
	char *temp;

	temp = ConsoleLines[LineBuffer - 1];

	for (loop = LineBuffer - 1; loop > 0; loop--)
		ConsoleLines[loop] = ConsoleLines[loop - 1];

	ConsoleLines[0] = temp;

	memset(ConsoleLines[0], 0, CON_CHARS_PER_LINE);
	if (TotalConsoleLines < LineBuffer - 1)
		TotalConsoleLines++;
	
	//Now adjust the ConsoleScrollBack
	//dont scroll if not at bottom
	if(ConsoleScrollBack != 0)
		ConsoleScrollBack++;
	//boundaries
	if(ConsoleScrollBack > LineBuffer-1)
		ConsoleScrollBack = LineBuffer-1;
}


static inline int con_get_width(void)
{
	if (!ConsoleSurface)
		return 0;

	return ConsoleSurface->cv_bitmap.bm_w - CON_CHAR_BORDER;
}


static inline int con_get_string_width(char *string)
{
	grs_canvas *canv_save;
	int w = 0, h, aw;

	if (!ConsoleSurface)
		return 0;

	canv_save = grd_curcanv;
	gr_set_current_canvas(ConsoleSurface);
	gr_get_string_size(string, &w, &h, &aw);
	gr_set_current_canvas(canv_save);

	return w;
}


#ifdef _MSC_VER
# define vsnprintf _vsnprintf
#endif

/* Outputs text to the console (in game), up to CON_CHARS_PER_LINE chars can be entered */
void CON_Out(const char *str, ...)
{
	va_list marker;
	//keep some space free for stuff like CON_Out("blablabla %s", Command);
	char temp[CON_CHARS_PER_LINE + 128];
	char* ptemp;

	va_start(marker, str);
	vsnprintf(temp, CON_CHARS_PER_LINE + 127, str, marker);
	va_end(marker);

	ptemp = temp;

	// temp now contains the complete string we want to output
	// the only problem is that temp is maybe longer than the console
	// width so we have to cut it into several pieces

	if (ConsoleLines) {
		char *p = ptemp;

		while (*p) {
			if (*p == '\n') {
				*p = '\0';
				CON_NewLineConsole();
				strcat(ConsoleLines[0], ptemp);
				ptemp = p+1;
			} else if (p - ptemp > VChars - strlen(ConsoleLines[0]) ||
					   con_get_string_width(ptemp) > con_get_width()) {
				CON_NewLineConsole();
				strncat(ConsoleLines[0], ptemp, VChars - strlen(ConsoleLines[0]));
				ConsoleLines[0][VChars] = '\0';
				ptemp = p;
			}
			p++;
		}
		if (strlen(ptemp)) {
			strncat(ConsoleLines[0], ptemp, VChars - strlen(ConsoleLines[0]));
			ConsoleLines[0][VChars] = '\0';
		}
		CON_UpdateConsole();
	}
}


/* Adds background image to the console, scaled to size of console*/
int CON_Background(grs_bitmap *image)
{
	/* Free the background from the console */
	if (image == NULL) {
		if (BackgroundImage)
			gr_free_bitmap(BackgroundImage);
		BackgroundImage = NULL;
		return 0;
	}

	/* Load a new background */
	if (BackgroundImage)
		gr_free_bitmap(BackgroundImage);
	BackgroundImage = gr_create_bitmap(ConsoleSurface->cv_w, ConsoleSurface->cv_h);
	gr_bitmap_scale_to(image, BackgroundImage);

	gr_bm_bitblt(BackgroundImage->bm_w, InputBackground->bm_h, 0, 0, 0, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, BackgroundImage, InputBackground);

	return 0;
}


/* Sets font info for the console */
void CON_Font(grs_font *font, int fg, int bg)
{
	grs_canvas *canv_save;

	canv_save = grd_curcanv;
	gr_set_current_canvas(ConsoleSurface);
	gr_set_curfont(font);
	gr_set_fontcolor(fg, bg);
	gr_set_current_canvas(canv_save);
}


/* resizes the console, has to reset alot of stuff
 * returns 1 on error */
void CON_Resize(int w, int h)
{
	/* make sure that the size of the console is valid */
	if(w > grd_curscreen->sc_w || w < ConsoleSurface->cv_font->ft_w * 32)
		w = grd_curscreen->sc_w;
	if(h > grd_curscreen->sc_h || h < ConsoleSurface->cv_font->ft_h)
		h = grd_curscreen->sc_h;

	/* resize console surface */
	gr_free_bitmap_data(&ConsoleSurface->cv_bitmap);
	gr_init_bitmap_alloc(&ConsoleSurface->cv_bitmap, BM_LINEAR, 0, 0, w, h, w);

	/* Load the dirty rectangle for user input */
	gr_free_bitmap(InputBackground);
	InputBackground = gr_create_bitmap(w, ConsoleSurface->cv_font->ft_h);

	/* Now reset some stuff dependent on the previous size */
	ConsoleScrollBack = 0;

	/* Reload the background image (for the input text area) in the console */
	if (BackgroundImage) {
		gr_bm_bitblt(BackgroundImage->bm_w, InputBackground->bm_h, 0, 0, 0, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, BackgroundImage, InputBackground);
	}
}


/* Sets the key that deactivates (hides) the console. */
void CON_SetHideKey(int key)
{
	HideKey = key;
}


void Clear_History(void)
{
	int loop;
	
	for (loop = 0; loop <= LineBuffer - 1; loop++)
		memset(ConsoleLines[loop], 0, CON_CHARS_PER_LINE);
}


/* convert to ansi rgb colors 17-231 */
#define PAL2ANSI(x) ((36*gr_palette[(x)*3]/11) + (6*gr_palette[(x)*3+1]/11) + (gr_palette[(x)*3+2]/11) + 16)

/* Print a message to the console */
void con_printf(int priority, char *fmt, ...)
{
	va_list arglist;
	char buffer[2048];

	if (priority <= (con_threshold.intval))
	{
		va_start (arglist, fmt);
		vsprintf (buffer,  fmt, arglist);
		va_end (arglist);

		if (con_initialized)
			CON_Out(buffer);

		if (!text_console_enabled)
			return;

		if (isatty(fileno(stdout))) {
			char *buf, *p;
			unsigned char color, spacing, underline;

			p = buf = buffer;
			do
				switch (*p)
				{
				case CC_COLOR:
					*p++ = 0;
					printf("%s", buf);
					color = *p++;
					printf("\x1B[38;5;%dm", PAL2ANSI(color));
					buf = p;
					break;
				case CC_LSPACING:
					*p++ = 0;
					printf("%s", buf);
					spacing = *p++;
					//printf("<SPACING %d>", color);
					buf = p;
					break;
				case CC_UNDERLINE:
					*p++ = 0;
					printf("%s", buf);
					underline = 1;
					//printf("<UNDERLINE>");
					buf = p;
					break;
				default:
					p++;
				}
			while (*p);

			printf("%s", buf);

		} else {
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
