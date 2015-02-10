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

#define FG_COLOR    grd_curcanv->cv_font_fg_color
#define get_msecs() approx_fsec_to_msec(timer_get_approx_seconds())

#define CON_BG_HIRES (cfexist("scoresb.pcx")?"scoresb.pcx":"scores.pcx")
#define CON_BG_LORES (cfexist("scores.pcx")?"scores.pcx":"scoresb.pcx") // Mac datafiles only have scoresb.pcx
#define CON_BG ((SWIDTH>=640)?CON_BG_HIRES:CON_BG_LORES)

#define CON_NUM_LINES           128
// Cut the buffer line if it becomes longer than this
#define CON_CHARS_PER_LINE      128
// Cursor blink frequency in ms
#define CON_BLINK_RATE          500
// Border in pixels from the most left to the first letter
#define CON_CHAR_BORDER         4
// Spacing in pixels between lines
#define CON_LINE_SPACE          1
// Default prompt used at the commandline
#define CON_DEFAULT_PROMPT      "]"
// Scroll this many lines at a time (when pressing PGUP or PGDOWN)
#define CON_LINE_SCROLL         2
// Indicator showing that you scrolled up the history
#define CON_SCROLL_INDICATOR    "^"
// Cursor shown if we are in insert mode
#define CON_INS_CURSOR          "_"
// Cursor shown if we are in overwrite mode
#define CON_OVR_CURSOR          "|"
// Defines the default hide key (Hide() the console if pressed)
#define CON_DEFAULT_HIDEKEY	KEY_ESC
// Defines the opening/closing speed
#define CON_OPENCLOSE_SPEED 50


/* The console's data */
static int Visible;             // Enum that tells which visible state we are in CON_HIDE, CON_SHOW, CON_RAISE, CON_LOWER
static int RaiseOffset;         // Offset used when scrolling in the console
static int HideKey;             // The key that can hide the console
static char **ConsoleLines;     // List of all the past lines
static char **CommandLines;     // List of all the past commands
static int TotalConsoleLines;   // Total number of lines in the console
static int ConsoleScrollBack;   // How much the user scrolled back in the console
static int TotalCommands;       // Number of commands in the Back Commands
static int LineBuffer;          // The number of visible lines in the console (autocalculated)
static int VChars;              // The number of visible characters in one console line (autocalculated)
static char *Prompt;            // Prompt displayed in command line
static char  Command[CON_CHARS_PER_LINE];   // current command in command line = lcommand + rcommand
static char LCommand[CON_CHARS_PER_LINE];   // right hand side of cursor
static char RCommand[CON_CHARS_PER_LINE];   // left hand side of cursor
static char VCommand[CON_CHARS_PER_LINE];   // current visible command line
static int CursorPos;           // Current cursor position in CurrentCommand
static int Offset;              // CommandOffset (first visible char of command) - if command is too long to fit into console
static int InsMode;             // Insert or Overwrite characters?
static grs_canvas *ConsoleSurface;  // Canvas of the console
static grs_bitmap *BackgroundImage; // Background image for the console
static grs_bitmap *InputBackground; // Dirty rectangle to draw over behind the users background
#if 0
static unsigned char ConsoleAlpha;  // The consoles alpha level
#endif
static int CommandScrollBack;       // How much the users scrolled back in the command lines

/* console is ready to be written to */
static int con_initialized;


/* Internals */
void CON_UpdateOffset(void);
/* Frees all the memory loaded by the console */
void CON_Free(void);
#if 0
/* Sets the alpha channel of an SDL_Surface to the specified value (0 - transparent,
 255 - opaque). Use this function also for OpenGL. */
void CON_Alpha(unsigned char alpha);
/* Sets the alpha channel of an SDL_Surface to the specified value.
 Preconditions: the surface in question is RGBA. 0 <= a <= 255, where 0 is transparent and 255 opaque */
void CON_AlphaGL(SDL_Surface *s, int alpha);
/* Sets a background image for the console */
#endif
int CON_Background(grs_bitmap *image);
/* Sets font info for the console */
void CON_Font(grs_font *font, int fg, int bg);
/* Modify the prompt of the console */
void CON_SetPrompt(char *newprompt);
/* Set the key, that invokes a CON_Hide() after press. default is ESCAPE and you can always hide using
 ESCAPE and the HideKey. compared against event->key.keysym.sym !! */
void CON_SetHideKey(int key);
/* executes the command typed in at the console (called if you press ENTER)*/
void CON_Execute(char *command);
/* Gets called when TAB was pressed */
void CON_TabCompletion(void);
/* makes newline (same as printf("\n") or CON_Out("\n") ) */
void CON_NewLineConsole(void);
/* shift command history (the one you can switch with the up/down keys) */
void CON_NewLineCommand(void);
/* updates console after resize etc. */
void CON_UpdateConsole(void);

/* draws the commandline the user is typing in to the screen. called by update? */
void DrawCommandLine();

/* Gets called if you press the LEFT key (move cursor left) */
void Cursor_Left(void);
/* Gets called if you press the RIGHT key (move cursor right) */
void Cursor_Right(void);
/* Gets called if you press the HOME key (move cursor to the beginning of the line */
void Cursor_Home(void);
/* Gets called if you press the END key (move cursor to the end of the line*/
void Cursor_End(void);
/* Called if you press DELETE (deletes character under the cursor) */
void Cursor_Del(void);
/* Called if you press BACKSPACE (deletes character left of cursor) */
void Cursor_BSpace(void);
/* Called if you type in a character (add the char to the command) */
void Cursor_Add(int event);

/* Called if you press Ctrl-C (deletes the commandline) */
void Clear_Command(void);
/* Called if you press Ctrl-L (deletes the History) */
void Clear_History(void);

/* Called if you press UP key (switches through recent typed in commands */
void Command_Up(void);
/* Called if you press DOWN key (switches through recent typed in commands */
void Command_Down(void);


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
				if(strlen(Command) > 0) {
					CON_NewLineCommand();

					// copy the input into the past commands strings
					strcpy(CommandLines[0], Command);

					// display the command including the prompt
					CON_Out("%s%s\n", Prompt, Command);
					CON_UpdateConsole();

					CON_Execute(Command);

					Clear_Command();
					CommandScrollBack = -1;
				}
				break;
			case KEY_LAPOSTRO:
				// deactivate Console
				CON_Hide();
				return 0;
			default:
				if (key_to_ascii(event) == 255)
					break;
				if (InsMode)
					Cursor_Add(event);
				else {
					Cursor_Add(event);
					Cursor_Del();
				}
		}
	}
	return 0;
}


#if 0
/* CON_AlphaGL() -- sets the alpha channel of an SDL_Surface to the
 * specified value.  Preconditions: the surface in question is RGBA.
 * 0 <= a <= 255, where 0 is transparent and 255 is opaque. */
void CON_AlphaGL(SDL_Surface *s, int alpha)
{
	Uint8 val;
	int x, y, w, h;
	Uint32 pixel;
	Uint8 r, g, b, a;
	SDL_PixelFormat *format;
	static char errorPrinted = 0;

	/* debugging assertions -- these slow you down, but hey, crashing sucks */
	if (!s) {
		PRINT_ERROR("NULL Surface passed to CON_AlphaGL\n");
		return;
	}

	/* clamp alpha value to 0...255 */
	if (alpha < SDL_ALPHA_TRANSPARENT)
		val = SDL_ALPHA_TRANSPARENT;
	else if(alpha > SDL_ALPHA_OPAQUE)
		val = SDL_ALPHA_OPAQUE;
	else
		val = alpha;

	/* loop over alpha channels of each pixel, setting them appropriately. */
	w = s->w;
	h = s->h;
	format = s->format;
	switch (format->BytesPerPixel) {
		case 2:
			/* 16-bit surfaces don't seem to support alpha channels. */
			if (!errorPrinted) {
				errorPrinted = 1;
				PRINT_ERROR("16-bit SDL surfaces do not support alpha-blending under OpenGL.\n");
			}
			break;
		case 4: {
			/* we can do this very quickly in 32-bit mode.  24-bit is more
			 * difficult.  And since 24-bit mode is reall the same as 32-bit,
			 * so it usually ends up taking this route too.  Win!  Unroll loop
			 * and use pointer arithmetic for extra speed. */
			int numpixels = h * (w << 2);
			Uint8 *pix = (Uint8 *) (s->pixels);
			Uint8 *last = pix + numpixels;
			Uint8 *pixel;
			if((numpixels & 0x7) == 0)
				for(pixel = pix + 3; pixel < last; pixel += 32)
					*pixel = *(pixel + 4) = *(pixel + 8) = *(pixel + 12) = *(pixel + 16) = *(pixel + 20) = *(pixel + 24) = *(pixel + 28) = val;
			else
				for(pixel = pix + 3; pixel < last; pixel += 4)
					*pixel = val;
			break;
		}
		default:
			/* we have no choice but to do this slowly.  <sigh> */
			for(y = 0; y < h; ++y)
				for(x = 0; x < w; ++x) {
					char print = 0;
					/* Lock the surface for direct access to the pixels */
					if(SDL_MUSTLOCK(s) && SDL_LockSurface(s) < 0) {
						PRINT_ERROR("Can't lock surface: ");
						fprintf(stderr, "%s\n", SDL_GetError());
						return;
					}
					pixel = DT_GetPixel(s, x, y);
					if(x == 0 && y == 0)
						print = 1;
					SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
					pixel = SDL_MapRGBA(format, r, g, b, val);
					SDL_GetRGBA(pixel, format, &r, &g, &b, &a);
					DT_PutPixel(s, x, y, pixel);

					/* unlock surface again */
					if(SDL_MUSTLOCK(s))
						SDL_UnlockSurface(s);
				}
			break;
	}
}
#endif


/* Updates the console buffer */
void CON_UpdateConsole(void)
{
	int loop;
	int loop2;
	int Screenlines;
	grs_canvas *canv_save;
	short orig_color;

	/* Due to the Blits, the update is not very fast: So only update if it's worth it */
	if (!CON_isVisible())
		return;

	Screenlines = ConsoleSurface->cv_h / (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h);

	canv_save = grd_curcanv;
	gr_set_current_canvas(ConsoleSurface);

#if 0
	SDL_FillRect(ConsoleSurface, NULL, SDL_MapRGBA(ConsoleSurface->format, 0, 0, 0, ConsoleAlpha));
#else
	//gr_rect(0,0,
#endif

#if 0
	if (grd_curscreen->flags & SDL_OPENGLBLIT)
		SDL_SetAlpha(ConsoleSurface, 0, SDL_ALPHA_OPAQUE);
#endif

	/* draw the background image if there is one */
	if (BackgroundImage)
		gr_bitmap(0, 0, BackgroundImage);

	/* Draw the text from the back buffers, calculate in the scrollback from the user
	 * this is a normal SDL software-mode blit, so we need to temporarily set the ColorKey
	 * for the font, and then clear it when we're done.
	 */
#if 0
	if ((grd_curscreen->flags & SDL_OPENGLBLIT) && (grd_curscreen->format->BytesPerPixel > 2)) {
		Uint32 *pix = (Uint32 *) (CurrentFont->FontSurface->pixels);
		SDL_SetColorKey(CurrentFont->FontSurface, SDL_SRCCOLORKEY, *pix);
	}
#endif

	// now draw text from last but second line to top
	for (loop = 0; loop < Screenlines-1 && loop < LineBuffer - ConsoleScrollBack; loop++) {
		if (ConsoleScrollBack != 0 && loop == 0)
			for (loop2 = 0; loop2 < (VChars / 5) + 1; loop2++)
			{
				orig_color = FG_COLOR;
				gr_string(CON_CHAR_BORDER + (loop2*5*ConsoleSurface->cv_font->ft_w), (Screenlines - loop - 2) * (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h), CON_SCROLL_INDICATOR);
				FG_COLOR = orig_color;
			}
		else
		{
			orig_color = FG_COLOR;
			gr_string(CON_CHAR_BORDER, (Screenlines - loop - 2) * (CON_LINE_SPACE + ConsoleSurface->cv_font->ft_h), ConsoleLines[ConsoleScrollBack + loop]);
			FG_COLOR = orig_color;
		}
	}

	gr_set_current_canvas(canv_save);

#if 0
	if(grd_curscreen->flags & SDL_OPENGLBLIT)
		SDL_SetColorKey(CurrentFont->FontSurface, 0, 0);
#endif
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

	/* Update the command line since it has a blinking cursor */
	DrawCommandLine();

#if 0
	/* before drawing, make sure the alpha channel of the console surface is set
	 * properly.  (sigh) I wish we didn't have to do this every frame... */
	if (grd_curscreen->flags & SDL_OPENGLBLIT)
		CON_AlphaGL(ConsoleSurface, ConsoleAlpha);
#endif

	canv_save = grd_curcanv;
	gr_set_current_canvas(&grd_curscreen->sc_canvas);

	clip = gr_create_sub_bitmap(&ConsoleSurface->cv_bitmap, 0, ConsoleSurface->cv_h - RaiseOffset, ConsoleSurface->cv_w, RaiseOffset);

	gr_bitmap(0, 0, clip);
	gr_free_sub_bitmap(clip);

#if 0
	if (grd_curscreen->flags & SDL_OPENGLBLIT)
		SDL_UpdateRects(grd_curscreen, 1, &DestRect);
#endif

	gr_set_current_canvas(canv_save);
}


/* Initializes the console */
void CON_Init()
{
	int loop;

	Visible = CON_CLOSED;
	RaiseOffset = 0;
	ConsoleLines = NULL;
	CommandLines = NULL;
	TotalConsoleLines = 0;
	ConsoleScrollBack = 0;
	TotalCommands = 0;
	BackgroundImage = NULL;
#if 0
	ConsoleAlpha = SDL_ALPHA_OPAQUE;
#endif
	Offset = 0;
	InsMode = 1;
	CursorPos = 0;
	CommandScrollBack = 0;
	Prompt = d_strdup(CON_DEFAULT_PROMPT);
	HideKey = CON_DEFAULT_HIDEKEY;

	/* load the console surface */
	ConsoleSurface = NULL;

	/* Load the dirty rectangle for user input */
	InputBackground = NULL;

	VChars = CON_CHARS_PER_LINE - 1;
	LineBuffer = CON_NUM_LINES;

	ConsoleLines = (char **)d_malloc(sizeof(char *) * LineBuffer);
	CommandLines = (char **)d_malloc(sizeof(char *) * LineBuffer);
	for (loop = 0; loop <= LineBuffer - 1; loop++) {
		ConsoleLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
		CommandLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
	}
	memset(Command, 0, CON_CHARS_PER_LINE);
	memset(LCommand, 0, CON_CHARS_PER_LINE);
	memset(RCommand, 0, CON_CHARS_PER_LINE);
	memset(VCommand, 0, CON_CHARS_PER_LINE);

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
#if 0
	SDL_FillRect(InputBackground, NULL, SDL_MapRGBA(ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif

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
void CON_Free(void)
{
	int i;

	for (i = 0; i <= LineBuffer - 1; i++) {
		d_free(ConsoleLines[i]);
		d_free(CommandLines[i]);
	}
	d_free(ConsoleLines);
	d_free(CommandLines);

	ConsoleLines = NULL;
	CommandLines = NULL;

	if (ConsoleSurface)
		gr_free_canvas(ConsoleSurface);
	ConsoleSurface = NULL;

	if (BackgroundImage)
		gr_free_bitmap(BackgroundImage);
	BackgroundImage = NULL;

	if (InputBackground)
		gr_free_bitmap(InputBackground);
	InputBackground = NULL;

	d_free(Prompt);

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


/* Increments the command lines */
void CON_NewLineCommand(void)
{
	int loop;
	char *temp;

	temp  = CommandLines[LineBuffer - 1];

	for (loop = LineBuffer - 1; loop > 0; loop--)
		CommandLines[loop] = CommandLines[loop - 1];

	CommandLines[0] = temp;

	memset(CommandLines[0], 0, CON_CHARS_PER_LINE);
	if (TotalCommands < LineBuffer - 1)
		TotalCommands++;
}


/* Draws the command line the user is typing in to the screen */
/* completely rewritten by C.Wacha */
void DrawCommandLine()
{
	int x;
	int commandbuffer;
#if 0
	grs_font *CurrentFont;
#endif
	static unsigned int LastBlinkTime = 0;  // Last time the consoles cursor blinked
	static int LastCursorPos = 0;           // Last Cursor Position
	static int Blink = 0;                   // Is the cursor currently blinking
	grs_canvas *canv_save;
	short orig_color;

	commandbuffer = VChars - (int)strlen(Prompt) - 1; // -1 to make cursor visible

#if 0
	CurrentFont = ConsoleSurface->cv_font;
#endif

	// Concatenate the left and right side to command
	strcpy(Command, LCommand);
	strncat(Command, RCommand, strlen(RCommand));

	//calculate display offset from current cursor position
	if (Offset < CursorPos - commandbuffer)
		Offset = CursorPos - commandbuffer;
	if(Offset > CursorPos)
		Offset = CursorPos;

	// first add prompt to visible part
	strcpy(VCommand, Prompt);

	// then add the visible part of the command
	strncat(VCommand, &Command[Offset], strlen(&Command[Offset]));

	// now display the result

#if 0
	// once again we're drawing text, so in OpenGL context we need to temporarily set up
	// software-mode transparency.
	if (grd_curscreen->flags & SDL_OPENGLBLIT) {
		Uint32 *pix = (Uint32 *) (CurrentFont->FontSurface->pixels);
		SDL_SetColorKey(CurrentFont->FontSurface, SDL_SRCCOLORKEY, *pix);
	}
#endif

	canv_save = grd_curcanv;
	gr_set_current_canvas(ConsoleSurface);

	// first of all restore InputBackground
	gr_bitmap(0, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, InputBackground);

	// now add the text
	orig_color = FG_COLOR;
	gr_string(CON_CHAR_BORDER, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, VCommand);
	FG_COLOR = orig_color;

	//at last add the cursor
	//check if the blink period is over
	if (get_msecs() > LastBlinkTime) {
		LastBlinkTime = get_msecs() + CON_BLINK_RATE;
		if(Blink)
			Blink = 0;
		else
			Blink = 1;
	}

	// check if cursor has moved - if yes display cursor anyway
	if (CursorPos != LastCursorPos) {
		LastCursorPos = CursorPos;
		LastBlinkTime = get_msecs() + CON_BLINK_RATE;
		Blink = 1;
	}

	if (Blink) {
		int prompt_width, cmd_width, h, w;

		gr_get_string_size(Prompt, &prompt_width, &h, &w);
		gr_get_string_size(LCommand + Offset, &cmd_width, &h, &w);
		x = CON_CHAR_BORDER + prompt_width + cmd_width;
		orig_color = FG_COLOR;
		if (InsMode)
			gr_string(x, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, CON_INS_CURSOR);
		else
			gr_string(x, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, CON_OVR_CURSOR);
		FG_COLOR = orig_color;
	}

	gr_set_current_canvas(canv_save);
	
#if 0
	if (grd_curscreen->flags & SDL_OPENGLBLIT) {
		SDL_SetColorKey(CurrentFont->FontSurface, 0, 0);
	}
#endif
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


#if 0
/* Sets the alpha level of the console, 0 turns off alpha blending */
void CON_Alpha(unsigned char alpha)
{
	/* store alpha as state! */
	ConsoleAlpha = alpha;

	if ((grd_curscreen->flags & SDL_OPENGLBLIT) == 0) {
		if (alpha == 0)
			SDL_SetAlpha(ConsoleSurface, 0, alpha);
		else
			SDL_SetAlpha(ConsoleSurface, SDL_SRCALPHA, alpha);
	}

	//CON_UpdateConsole();
}
#endif


/* Adds background image to the console, scaled to size of console*/
int CON_Background(grs_bitmap *image)
{
	/* Free the background from the console */
	if (image == NULL) {
		if (BackgroundImage)
			gr_free_bitmap(BackgroundImage);
		BackgroundImage = NULL;
#if 0
		SDL_FillRect(InputBackground, NULL, SDL_MapRGBA(ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
		return 0;
	}

	/* Load a new background */
	if (BackgroundImage)
		gr_free_bitmap(BackgroundImage);
	BackgroundImage = gr_create_bitmap(ConsoleSurface->cv_w, ConsoleSurface->cv_h);
	gr_bitmap_scale_to(image, BackgroundImage);

#if 0
	SDL_FillRect(InputBackground, NULL, SDL_MapRGBA(ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
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
#if 0
		SDL_FillRect(InputBackground, NULL, SDL_MapRGBA(ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
		gr_bm_bitblt(BackgroundImage->bm_w, InputBackground->bm_h, 0, 0, 0, ConsoleSurface->cv_h - ConsoleSurface->cv_font->ft_h, BackgroundImage, InputBackground);
	}

#if 0
	/* restore the alpha level */
	CON_Alpha(ConsoleAlpha);
#endif
}


/* Sets the Prompt for console */
void CON_SetPrompt(char *newprompt) {
	// check length so we can still see at least 1 char :-)
	if (strlen(newprompt) < VChars) {
		d_free(Prompt);
		Prompt = d_strdup(newprompt);
	} else
		CON_Out("prompt too long. (max. %i chars)\n", VChars - 1);
}


/* Sets the key that deactivates (hides) the console. */
void CON_SetHideKey(int key)
{
	HideKey = key;
}


/* Executes the command entered */
void CON_Execute(char *command)
{
	cmd_append(command);
}


void CON_TabCompletion(void)
{
	int i, j;
	char *command;

	command = cmd_complete(LCommand);

	if (!command)
		return; // no tab completion took place so return silently

	j = (int)strlen(command);
	if (j > CON_CHARS_PER_LINE - 2)
		j = CON_CHARS_PER_LINE-1;

	memset(LCommand, 0, CON_CHARS_PER_LINE);
	CursorPos = 0;

	for (i = 0; i < j; i++) {
		CursorPos++;
		LCommand[i] = command[i];
	}
	// add a trailing space
	CursorPos++;
	LCommand[j] = ' ';
	LCommand[j+1] = '\0';
}


void Cursor_Left(void)
{
	char temp[CON_CHARS_PER_LINE];

	if (CursorPos > 0) {
		CursorPos--;
		strcpy(temp, RCommand);
		strcpy(RCommand, &LCommand[strlen(LCommand)-1]);
		strcat(RCommand, temp);
		LCommand[strlen(LCommand)-1] = '\0';
	}
}


void Cursor_Right(void)
{
	char temp[CON_CHARS_PER_LINE];
	
	if(CursorPos < strlen(Command)) {
		CursorPos++;
		strncat(LCommand, RCommand, 1);
		strcpy(temp, RCommand);
		strcpy(RCommand, &temp[1]);
	}
}


void Cursor_Home(void)
{
	char temp[CON_CHARS_PER_LINE];

	CursorPos = 0;
	strcpy(temp, RCommand);
	strcpy(RCommand, LCommand);
	strncat(RCommand, temp, strlen(temp));
	memset(LCommand, 0, CON_CHARS_PER_LINE);
}


void Cursor_End(void)
{
	CursorPos = (int)strlen(Command);
	strncat(LCommand, RCommand, strlen(RCommand));
	memset(RCommand, 0, CON_CHARS_PER_LINE);
}


void Cursor_Del(void)
{
	char temp[CON_CHARS_PER_LINE];

	if (strlen(RCommand) > 0) {
		strcpy(temp, RCommand);
		strcpy(RCommand, &temp[1]);
	}
}

void Cursor_BSpace(void)
{
	if (CursorPos > 0) {
		CursorPos--;
		Offset--;
		if (Offset < 0)
			Offset = 0;
		LCommand[strlen(LCommand)-1] = '\0';
	}
}


void Cursor_Add(int event)
{
	if (strlen(Command) < CON_CHARS_PER_LINE - 1)
	{
		CursorPos++;
		LCommand[strlen(LCommand)] = key_to_ascii(event);
		LCommand[strlen(LCommand)] = '\0';
	}
}


void Clear_Command(void)
{
	CursorPos = 0;
	memset( Command, 0, CON_CHARS_PER_LINE);
	memset(LCommand, 0, CON_CHARS_PER_LINE);
	memset(RCommand, 0, CON_CHARS_PER_LINE);
	memset(VCommand, 0, CON_CHARS_PER_LINE);
}


void Clear_History(void)
{
	int loop;

	for (loop = 0; loop <= LineBuffer - 1; loop++)
		memset(ConsoleLines[loop], 0, CON_CHARS_PER_LINE);
}


void Command_Up(void)
{
	if(CommandScrollBack < TotalCommands - 1) {
		/* move back a line in the command strings and copy the command to the current input string */
		CommandScrollBack++;
		memset(RCommand, 0, CON_CHARS_PER_LINE);
		Offset = 0;
		strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = (int)strlen(CommandLines[CommandScrollBack]);
		CON_UpdateConsole();
	}
}


void Command_Down(void)
{
	if(CommandScrollBack > -1) {
		/* move forward a line in the command strings and copy the command to the current input string */
		CommandScrollBack--;
		memset(RCommand, 0, CON_CHARS_PER_LINE);
		memset(LCommand, 0, CON_CHARS_PER_LINE);
		Offset = 0;
		if(CommandScrollBack > -1)
			strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = (int)strlen(LCommand);
		CON_UpdateConsole();
	}
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
