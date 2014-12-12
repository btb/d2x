/*
 * Code for controlling the console
 *  Based on an old version of SDL_Console
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

#include "console.h"

#include "u_mem.h"
#include "gr.h"
#include "timer.h"
#include "pstypes.h"
#include "error.h"
#include "cvar.h"
#include "gamefont.h"
#include "pcx.h"
#include "cfile.h"


#ifndef __MSDOS__
int text_console_enabled = 1;
#else
int isvga();
#define text_console_enabled (!isvga())
#endif


/* Console specific cvars */
/* How discriminating we are about which messages are displayed */
cvar_t con_threshold = {"con_threshold", "0",};

/* Private console stuff */
#define CON_NUM_LINES 40

#define FG_COLOR    grd_curcanv->cv_font_fg_color
#define get_msecs() approx_fsec_to_msec(timer_get_approx_seconds())

#define CON_BG_HIRES (cfexist("scoresb.pcx")?"scoresb.pcx":"scores.pcx")
#define CON_BG_LORES (cfexist("scores.pcx")?"scores.pcx":"scoresb.pcx") // Mac datafiles only have scoresb.pcx
#define CON_BG ((SWIDTH>=640)?CON_BG_HIRES:CON_BG_LORES)


/* our one console */
static ConsoleInformation Console;
#define console (&Console)

/* console is ready to be written to */
static int con_initialized;


/* Internals */
void CON_UpdateOffset(void);
/*! Frees all the memory loaded by the console */
void CON_Free(void);
#if 0
/*! Sets the alpha channel of an SDL_Surface to the specified value (0 - transparend,
 255 - opaque). Use this function also for OpenGL. */
void CON_Alpha(unsigned char alpha);
/*! Internal: Sets the alpha channel of an SDL_Surface to the specified value.
 Preconditions: the surface in question is RGBA. 0 <= a <= 255, where 0 is transparent and 255 opaque */
void CON_AlphaGL(SDL_Surface *s, int alpha);
/*! Sets a background image for the console */
#endif
int CON_Background(grs_bitmap *image);
/*! Sets font info for the console */
void CON_Font(grs_font *font, int fg, int bg);
/*! Beams a console to another screen surface. Needed if you want to make a Video restart in your program. This
 function first changes the OutputScreen Pointer then calls CON_Resize to adjust the new size. */
int CON_Transfer(grs_screen* new_outputscreen, int x, int y, int w, int h);
/*! Modify the prompt of the console */
void CON_SetPrompt(char* newprompt);
/*! Set the key, that invokes a CON_Hide() after press. default is ESCAPE and you can always hide using
 ESCAPE and the HideKey. compared against event->key.keysym.sym !! */
void CON_SetHideKey(int key);
/*! Internal: executes the command typed in at the console (called if you press ENTER)*/
void CON_Execute(char* command);
/*! Internal: Gets called when TAB was pressed */
void CON_TabCompletion(void);
/*! Internal: makes newline (same as printf("\n") or CON_Out("\n") ) */
void CON_NewLineConsole(void);
/*! Internal: shift command history (the one you can switch with the up/down keys) */
void CON_NewLineCommand(void);
/*! Internal: updates console after resize etc. */
void CON_UpdateConsole(void);


/*! Internal: draws the commandline the user is typing in to the screen. called by update? */
void DrawCommandLine();

/*! Internal: Gets called if you press the LEFT key (move cursor left) */
void Cursor_Left(void);
/*! Internal: Gets called if you press the RIGHT key (move cursor right) */
void Cursor_Right(void);
/*! Internal: Gets called if you press the HOME key (move cursor to the beginning
	of the line */
void Cursor_Home(void);
/*! Internal: Gets called if you press the END key (move cursor to the end of the line*/
void Cursor_End(void);
/*! Internal: Called if you press DELETE (deletes character under the cursor) */
void Cursor_Del(void);
/*! Internal: Called if you press BACKSPACE (deletes character left of cursor) */
void Cursor_BSpace(void);
/*! Internal: Called if you type in a character (add the char to the command) */
void Cursor_Add(int event);

/*! Internal: Called if you press Ctrl-C (deletes the commandline) */
void Clear_Command(void);
/*! Internal: Called if you press Ctrl-L (deletes the History) */
void Clear_History(void);

/*! Internal: Called if you press UP key (switches through recent typed in commands */
void Command_Up(void);
/*! Internal: Called if you press DOWN key (switches through recent typed in commands */
void Command_Down(void);


/*  Takes keys from the keyboard and inputs them to the console
 If the event was not handled (i.e. WM events or unknown ctrl-shift
 sequences) the function returns the event for further processing. */
int CON_Events(int event)
{
	if(!CON_isVisible())
		return event;
	
	if(event & KEY_CTRLED)
	{
		//CTRL pressed
		switch(event & ~KEY_CTRLED)
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
	else if(event & KEY_ALTED)
	{
		//the console does not handle ALT combinations!
		return event;
	}
	else
	{
		//first of all, check if the console hide key was pressed
		if(event == console->HideKey)
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
					console->ConsoleScrollBack = console->LineBuffer-1;
					CON_UpdateConsole();
				} else {
					Cursor_Home();
				}
				break;
			case KEY_END:
				if(event & KEY_SHIFTED)
				{
					console->ConsoleScrollBack = 0;
					CON_UpdateConsole();
				} else {
					Cursor_End();
				}
				break;
			case KEY_PAGEUP:
				console->ConsoleScrollBack += CON_LINE_SCROLL;
				if(console->ConsoleScrollBack > console->LineBuffer-1)
					console->ConsoleScrollBack = console->LineBuffer-1;
				
				CON_UpdateConsole();
				break;
			case KEY_PAGEDOWN:
				console->ConsoleScrollBack -= CON_LINE_SCROLL;
				if(console->ConsoleScrollBack < 0)
					console->ConsoleScrollBack = 0;
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
				console->InsMode = 1-console->InsMode;
				break;
			case KEY_TAB:
				CON_TabCompletion();
				break;
			case KEY_ENTER:
				if(strlen(console->Command) > 0) {
					CON_NewLineCommand();
					
					// copy the input into the past commands strings
					strcpy(console->CommandLines[0], console->Command);
					
					// display the command including the prompt
					CON_Out("%s%s", console->Prompt, console->Command);
					CON_UpdateConsole();
					
					CON_Execute(console->Command);
					
					Clear_Command();
					console->CommandScrollBack = -1;
				}
				break;
			case KEY_LAPOSTRO:
				//deactivate Console
				CON_Hide();
				return 0;
			default:
				if (key_to_ascii(event) == 255)
					break;
				if(console->InsMode)
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
void CON_AlphaGL(SDL_Surface *s, int alpha) {
	Uint8 val;
	int x, y, w, h;
	Uint32 pixel;
	Uint8 r, g, b, a;
	SDL_PixelFormat *format;
	static char errorPrinted = 0;
	
	
	/* debugging assertions -- these slow you down, but hey, crashing sucks */
	if(!s) {
		PRINT_ERROR("NULL Surface passed to CON_AlphaGL\n");
		return;
	}
	
	/* clamp alpha value to 0...255 */
	if(alpha < SDL_ALPHA_TRANSPARENT)
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
			if(!errorPrinted) {
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
void CON_UpdateConsole(void) {
	int loop;
	int loop2;
	int Screenlines;
	grs_canvas *canv_save;
	short orig_color;
	
	/* Due to the Blits, the update is not very fast: So only update if it's worth it */
	if(!CON_isVisible())
		return;
	
	Screenlines = console->ConsoleSurface->cv_h / (CON_LINE_SPACE + console->ConsoleSurface->cv_font->ft_h);
	
	canv_save = grd_curcanv;
	gr_set_current_canvas(console->ConsoleSurface);
	
#if 0
	SDL_FillRect(console->ConsoleSurface, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, console->ConsoleAlpha));
#else
	//gr_rect(0,0,
#endif
	
#if 0
	if(console->OutputScreen->flags & SDL_OPENGLBLIT)
		SDL_SetAlpha(console->ConsoleSurface, 0, SDL_ALPHA_OPAQUE);
#endif
	
	/* draw the background image if there is one */
	if(console->BackgroundImage)
		gr_bitmap(0, 0, console->BackgroundImage);
	
	/* Draw the text from the back buffers, calculate in the scrollback from the user
	 * this is a normal SDL software-mode blit, so we need to temporarily set the ColorKey
	 * for the font, and then clear it when we're done.
	 */
#if 0
	if((console->OutputScreen->flags & SDL_OPENGLBLIT) && (console->OutputScreen->format->BytesPerPixel > 2)) {
		Uint32 *pix = (Uint32 *) (CurrentFont->FontSurface->pixels);
		SDL_SetColorKey(CurrentFont->FontSurface, SDL_SRCCOLORKEY, *pix);
	}
#endif
	
	//now draw text from last but second line to top
	for(loop = 0; loop < Screenlines-1 && loop < console->LineBuffer - console->ConsoleScrollBack; loop++) {
		if(console->ConsoleScrollBack != 0 && loop == 0)
			for(loop2 = 0; loop2 < (console->VChars / 5) + 1; loop2++)
			{
				orig_color = FG_COLOR;
				gr_string(CON_CHAR_BORDER + (loop2*5*console->ConsoleSurface->cv_font->ft_w), (Screenlines - loop - 2) * (CON_LINE_SPACE + console->ConsoleSurface->cv_font->ft_h), CON_SCROLL_INDICATOR);
				FG_COLOR = orig_color;
			}
		else
		{
			orig_color = FG_COLOR;
			gr_string(CON_CHAR_BORDER, (Screenlines - loop - 2) * (CON_LINE_SPACE + console->ConsoleSurface->cv_font->ft_h), console->ConsoleLines[console->ConsoleScrollBack + loop]);
			FG_COLOR = orig_color;
		}
	}
	
	gr_set_current_canvas(canv_save);
	
#if 0
	if(console->OutputScreen->flags & SDL_OPENGLBLIT)
		SDL_SetColorKey(CurrentFont->FontSurface, 0, 0);
#endif
}

void CON_UpdateOffset(void) {
	switch(console->Visible) {
		case CON_CLOSING:
			console->RaiseOffset -= CON_OPENCLOSE_SPEED;
			if(console->RaiseOffset <= 0) {
				console->RaiseOffset = 0;
				console->Visible = CON_CLOSED;
			}
			break;
		case CON_OPENING:
			console->RaiseOffset += CON_OPENCLOSE_SPEED;
			if(console->RaiseOffset >= console->ConsoleSurface->cv_h) {
				console->RaiseOffset = console->ConsoleSurface->cv_h;
				console->Visible = CON_OPEN;
			}
			break;
		case CON_OPEN:
		case CON_CLOSED:
			break;
	}
}

/* Draws the console buffer to the screen if the console is "visible" */
void CON_DrawConsole(void) {
	grs_canvas *canv_save;
	grs_bitmap *clip;
	
	/* only draw if console is visible: here this means, that the console is not CON_CLOSED */
	if(console->Visible == CON_CLOSED)
		return;
	
	/* Update the scrolling offset */
	CON_UpdateOffset();
	
	/* Update the command line since it has a blinking cursor */
	DrawCommandLine();
	
#if 0
	/* before drawing, make sure the alpha channel of the console surface is set
	 * properly.  (sigh) I wish we didn't have to do this every frame... */
	if(console->OutputScreen->flags & SDL_OPENGLBLIT)
		CON_AlphaGL(console->ConsoleSurface, console->ConsoleAlpha);
#endif
	
	canv_save = grd_curcanv;
	gr_set_current_canvas(&console->OutputScreen->sc_canvas);
	
	clip = gr_create_sub_bitmap(&console->ConsoleSurface->cv_bitmap, 0, console->ConsoleSurface->cv_h - console->RaiseOffset, console->ConsoleSurface->cv_w, console->RaiseOffset);
	
	gr_bitmap(0, 0, clip);
	gr_free_sub_bitmap(clip);
	
#if 0
	if(console->OutputScreen->flags & SDL_OPENGLBLIT)
		SDL_UpdateRects(console->OutputScreen, 1, &DestRect);
#endif
	
	gr_set_current_canvas(canv_save);
}


/* Initializes the console */
void CON_Init()
{
	int loop;

	console->Visible = CON_CLOSED;
	console->RaiseOffset = 0;
	console->ConsoleLines = NULL;
	console->CommandLines = NULL;
	console->TotalConsoleLines = 0;
	console->ConsoleScrollBack = 0;
	console->TotalCommands = 0;
	console->BackgroundImage = NULL;
#if 0
	console->ConsoleAlpha = SDL_ALPHA_OPAQUE;
#endif
	console->Offset = 0;
	console->InsMode = 1;
	console->CursorPos = 0;
	console->CommandScrollBack = 0;
	console->OutputScreen = NULL;
	console->Prompt = CON_DEFAULT_PROMPT;
	console->HideKey = CON_DEFAULT_HIDEKEY;

	/* load the console surface */
	console->ConsoleSurface = NULL;

	/* Load the dirty rectangle for user input */
	console->InputBackground = NULL;

	console->VChars = CON_CHARS_PER_LINE;
	console->LineBuffer = CON_NUM_LINES;

	console->ConsoleLines = (char **)d_malloc(sizeof(char *) * console->LineBuffer);
	console->CommandLines = (char **)d_malloc(sizeof(char *) * console->LineBuffer);
	for(loop = 0; loop <= console->LineBuffer - 1; loop++) {
		console->ConsoleLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
		console->CommandLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
	}
	memset(console->Command, 0, CON_CHARS_PER_LINE);
	memset(console->LCommand, 0, CON_CHARS_PER_LINE);
	memset(console->RCommand, 0, CON_CHARS_PER_LINE);
	memset(console->VCommand, 0, CON_CHARS_PER_LINE);

	cmd_init();

	/* Initialise the cvars */
	cvar_registervariable (&con_threshold);

	con_initialized = 1;

	atexit(CON_Free);
}


void CON_InitGFX(int w, int h)
{
	int pcx_error;
	grs_bitmap bmp;
	ubyte pal[256*3];

	console->OutputScreen = grd_curscreen;
	
	/* load the console surface */
	console->ConsoleSurface = gr_create_canvas(w, h);

	/* Load the consoles font */
	CON_Font(SMALL_FONT, gr_getcolor(63,63,63), -1);

	/* make sure that the size of the console is valid */
	if(w > console->OutputScreen->sc_w || w < console->ConsoleSurface->cv_font->ft_w * 32)
		w = console->OutputScreen->sc_w;
	if(h > console->OutputScreen->sc_h || h < console->ConsoleSurface->cv_font->ft_h)
		h = console->OutputScreen->sc_h;

	/* Load the dirty rectangle for user input */
	console->InputBackground = gr_create_bitmap(w, console->ConsoleSurface->cv_font->ft_h);
#if 0
	SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
	
	/* calculate the number of visible characters in the command line */
	console->VChars = (w - CON_CHAR_BORDER) / console->ConsoleSurface->cv_font->ft_w;
	if(console->VChars > CON_CHARS_PER_LINE)
		console->VChars = CON_CHARS_PER_LINE;
	
	CON_Transfer(grd_curscreen, 0, 0, SWIDTH, SHEIGHT / 2);
	
	gr_init_bitmap_data(&bmp);
	pcx_error = pcx_read_bitmap(CON_BG, &bmp, BM_LINEAR, pal);
	Assert(pcx_error == PCX_ERROR_NONE);
	gr_remap_bitmap_good(&bmp, pal, -1, -1);
	CON_Background(&bmp);
	gr_free_bitmap_data(&bmp);
}


/* Makes the console visible */
void CON_Show(void) {
	console->Visible = CON_OPENING;
	CON_UpdateConsole();
}

/* Hides the console (make it invisible) */
void CON_Hide(void) {
	console->Visible = CON_CLOSING;
}

/* tells wether the console is visible or not */
int CON_isVisible(void) {
	return((console->Visible == CON_OPEN) || (console->Visible == CON_OPENING));
}

/* Frees all the memory loaded by the console */
void CON_Free(void) {
	int i;
	
	for(i = 0; i <= console->LineBuffer - 1; i++) {
		d_free(console->ConsoleLines[i]);
		d_free(console->CommandLines[i]);
	}
	d_free(console->ConsoleLines);
	d_free(console->CommandLines);
	
	console->ConsoleLines = NULL;
	console->CommandLines = NULL;
	
	gr_free_canvas(console->ConsoleSurface);
	console->ConsoleSurface = NULL;
	
	if (console->BackgroundImage)
		gr_free_bitmap(console->BackgroundImage);
	console->BackgroundImage = NULL;
	
	gr_free_bitmap(console->InputBackground);
	console->InputBackground = NULL;

	con_initialized = 0;
}


/* Increments the console lines */
void CON_NewLineConsole(void) {
	int loop;
	char* temp;
	
	temp = console->ConsoleLines[console->LineBuffer - 1];
	
	for(loop = console->LineBuffer - 1; loop > 0; loop--)
		console->ConsoleLines[loop] = console->ConsoleLines[loop - 1];
	
	console->ConsoleLines[0] = temp;
	
	memset(console->ConsoleLines[0], 0, CON_CHARS_PER_LINE);
	if(console->TotalConsoleLines < console->LineBuffer - 1)
		console->TotalConsoleLines++;
	
	//Now adjust the ConsoleScrollBack
	//dont scroll if not at bottom
	if(console->ConsoleScrollBack != 0)
		console->ConsoleScrollBack++;
	//boundaries
	if(console->ConsoleScrollBack > console->LineBuffer-1)
		console->ConsoleScrollBack = console->LineBuffer-1;
	
}


/* Increments the command lines */
void CON_NewLineCommand(void) {
	int loop;
	char *temp;
	
	temp  = console->CommandLines[console->LineBuffer - 1];
	
	
	for(loop = console->LineBuffer - 1; loop > 0; loop--)
		console->CommandLines[loop] = console->CommandLines[loop - 1];
	
	console->CommandLines[0] = temp;
	
	memset(console->CommandLines[0], 0, CON_CHARS_PER_LINE);
	if(console->TotalCommands < console->LineBuffer - 1)
		console->TotalCommands++;
}

/* Draws the command line the user is typing in to the screen */
/* completely rewritten by C.Wacha */
void DrawCommandLine() {
	int x;
	int commandbuffer;
#if 0
	grs_font* CurrentFont;
#endif
	static unsigned int LastBlinkTime = 0;	/* Last time the consoles cursor blinked */
	static int LastCursorPos = 0;		// Last Cursor Position
	static int Blink = 0;			/* Is the cursor currently blinking */
	grs_canvas *canv_save;
	short orig_color;
	
	commandbuffer = console->VChars - strlen(console->Prompt)-1; // -1 to make cursor visible
	
#if 0
	CurrentFont = console->ConsoleSurface->cv_font;
#endif
	
	//Concatenate the left and right side to command
	strcpy(console->Command, console->LCommand);
	strncat(console->Command, console->RCommand, strlen(console->RCommand));
	
	//calculate display offset from current cursor position
	if(console->Offset < console->CursorPos - commandbuffer)
		console->Offset = console->CursorPos - commandbuffer;
	if(console->Offset > console->CursorPos)
		console->Offset = console->CursorPos;
	
	//first add prompt to visible part
	strcpy(console->VCommand, console->Prompt);
	
	//then add the visible part of the command
	strncat(console->VCommand, &console->Command[console->Offset], strlen(&console->Command[console->Offset]));
	
	//now display the result
	
#if 0
	//once again we're drawing text, so in OpenGL context we need to temporarily set up
	//software-mode transparency.
	if(console->OutputScreen->flags & SDL_OPENGLBLIT) {
		Uint32 *pix = (Uint32 *) (CurrentFont->FontSurface->pixels);
		SDL_SetColorKey(CurrentFont->FontSurface, SDL_SRCCOLORKEY, *pix);
	}
#endif
	
	canv_save = grd_curcanv;
	gr_set_current_canvas(console->ConsoleSurface);
	
	//first of all restore InputBackground
	gr_bitmap(0, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, console->InputBackground);
	
	//now add the text
	orig_color = FG_COLOR;
	gr_string(CON_CHAR_BORDER, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, console->VCommand);
	FG_COLOR = orig_color;
	
	//at last add the cursor
	//check if the blink period is over
	if(get_msecs() > LastBlinkTime) {
		LastBlinkTime = get_msecs() + CON_BLINK_RATE;
		if(Blink)
			Blink = 0;
		else
			Blink = 1;
	}
	
	//check if cursor has moved - if yes display cursor anyway
	if(console->CursorPos != LastCursorPos) {
		LastCursorPos = console->CursorPos;
		LastBlinkTime = get_msecs() + CON_BLINK_RATE;
		Blink = 1;
	}
	
	if(Blink) {
		int prompt_width, cmd_width, h, w;
		
		gr_get_string_size(console->Prompt, &prompt_width, &h, &w);
		gr_get_string_size(console->LCommand + console->Offset, &cmd_width, &h, &w);
		x = CON_CHAR_BORDER + prompt_width + cmd_width;
		orig_color = FG_COLOR;
		if(console->InsMode)
			gr_string(x, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, CON_INS_CURSOR);
		else
			gr_string(x, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, CON_OVR_CURSOR);
		FG_COLOR = orig_color;
	}
	
	gr_set_current_canvas(canv_save);
	
	
#if 0
	if(console->OutputScreen->flags & SDL_OPENGLBLIT) {
		SDL_SetColorKey(CurrentFont->FontSurface, 0, 0);
	}
#endif
}

#ifdef _MSC_VER
# define vsnprintf _vsnprintf
#endif

/* Outputs text to the console (in game), up to CON_CHARS_PER_LINE chars can be entered */
void CON_Out(const char *str, ...) {
	va_list marker;
	//keep some space free for stuff like CON_Out("blablabla %s", console->Command);
	char temp[CON_CHARS_PER_LINE + 128];
	char* ptemp;
	
	va_start(marker, str);
	vsnprintf(temp, CON_CHARS_PER_LINE + 127, str, marker);
	va_end(marker);
	
	ptemp = temp;
	
	//temp now contains the complete string we want to output
	// the only problem is that temp is maybe longer than the console
	// width so we have to cut it into several pieces
	
	if(console->ConsoleLines) {
		while(strlen(ptemp) > console->VChars) {
			CON_NewLineConsole();
			strncpy(console->ConsoleLines[0], ptemp, console->VChars);
			console->ConsoleLines[0][console->VChars] = '\0';
			ptemp = &ptemp[console->VChars];
		}
		CON_NewLineConsole();
		strncpy(console->ConsoleLines[0], ptemp, console->VChars);
		console->ConsoleLines[0][console->VChars] = '\0';
		CON_UpdateConsole();
	}
}


#if 0
/* Sets the alpha level of the console, 0 turns off alpha blending */
void CON_Alpha(unsigned char alpha) {
	/* store alpha as state! */
	console->ConsoleAlpha = alpha;
	
	if((console->OutputScreen->flags & SDL_OPENGLBLIT) == 0) {
		if(alpha == 0)
			SDL_SetAlpha(console->ConsoleSurface, 0, alpha);
		else
			SDL_SetAlpha(console->ConsoleSurface, SDL_SRCALPHA, alpha);
	}
	
	//	CON_UpdateConsole();
}
#endif


/* Adds  background image to the console, scaled to size of console*/
int CON_Background(grs_bitmap *image)
{
	/* Free the background from the console */
	if (image == NULL) {
		if (console->BackgroundImage)
			gr_free_bitmap(console->BackgroundImage);
		console->BackgroundImage = NULL;
#if 0
		SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
		return 0;
	}
	
	/* Load a new background */
	if (console->BackgroundImage)
		gr_free_bitmap(console->BackgroundImage);
	console->BackgroundImage = gr_create_bitmap(console->ConsoleSurface->cv_w, console->ConsoleSurface->cv_h);
	gr_bitmap_scale_to(image, console->BackgroundImage);
	
#if 0
	SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
	gr_bm_bitblt(console->BackgroundImage->bm_w, console->InputBackground->bm_h, 0, 0, 0, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, console->BackgroundImage, console->InputBackground);
	
	return 0;
}

/* Sets font info for the console */
void CON_Font(grs_font *font, int fg, int bg)
{
	grs_canvas *canv_save;
	
	canv_save = grd_curcanv;
	gr_set_current_canvas(console->ConsoleSurface);
	gr_set_curfont(font);
	gr_set_fontcolor(fg, bg);
	gr_set_current_canvas(canv_save);
}

void gr_init_bitmap_alloc( grs_bitmap *bm, int mode, int x, int y, int w, int h, int bytesperline);
/* resizes the console, has to reset alot of stuff
 * returns 1 on error */
int CON_Resize(int x, int y, int w, int h)
{
	/* make sure that the size of the console is valid */
	if(w > console->OutputScreen->sc_w || w < console->ConsoleSurface->cv_font->ft_w * 32)
		w = console->OutputScreen->sc_w;
	if(h > console->OutputScreen->sc_h || h < console->ConsoleSurface->cv_font->ft_h)
		h = console->OutputScreen->sc_h;
	
	/* resize console surface */
	gr_free_bitmap_data(&console->ConsoleSurface->cv_bitmap);
	gr_init_bitmap_alloc(&console->ConsoleSurface->cv_bitmap, BM_LINEAR, 0, 0, w, h, w);
	
	/* Load the dirty rectangle for user input */
	gr_free_bitmap(console->InputBackground);
	console->InputBackground = gr_create_bitmap(w, console->ConsoleSurface->cv_font->ft_h);
	
	/* Now reset some stuff dependent on the previous size */
	console->ConsoleScrollBack = 0;
	
	/* Reload the background image (for the input text area) in the console */
	if(console->BackgroundImage) {
#if 0
		SDL_FillRect(console->InputBackground, NULL, SDL_MapRGBA(console->ConsoleSurface->format, 0, 0, 0, SDL_ALPHA_OPAQUE));
#endif
		gr_bm_bitblt(console->BackgroundImage->bm_w, console->InputBackground->bm_h, 0, 0, 0, console->ConsoleSurface->cv_h - console->ConsoleSurface->cv_font->ft_h, console->BackgroundImage, console->InputBackground);
	}
	
#if 0
	/* restore the alpha level */
	CON_Alpha(console->ConsoleAlpha);
#endif
	return 0;
}

/* Transfers the console to another screen surface, and adjusts size */
int CON_Transfer(grs_screen *new_outputscreen, int x, int y, int w, int h)
{
	console->OutputScreen = new_outputscreen;
	
	return(CON_Resize(x, y, w, h));
}

/* Sets the Prompt for console */
void CON_SetPrompt(char* newprompt) {
	//check length so we can still see at least 1 char :-)
	if(strlen(newprompt) < console->VChars)
		console->Prompt = d_strdup(newprompt);
	else
		CON_Out("prompt too long. (max. %i chars)", console->VChars - 1);
}

/* Sets the key that deactivates (hides) the console. */
void CON_SetHideKey(int key) {
	console->HideKey = key;
}

/* Executes the command entered */
void CON_Execute(char* command) {
	cmd_parse(command);
}

void CON_TabCompletion(void) {
	int i,j;
	char* command;
	
	command = d_strdup(console->LCommand);
	command = cmd_complete(command);
	
	if(!command)
		return;	//no tab completion took place so return silently
	
	j = strlen(command);
	if(j > CON_CHARS_PER_LINE - 2)
		j = CON_CHARS_PER_LINE-1;
	
	memset(console->LCommand, 0, CON_CHARS_PER_LINE);
	console->CursorPos = 0;
	
	for(i = 0; i < j; i++) {
		console->CursorPos++;
		console->LCommand[i] = command[i];
	}
	//add a trailing space
	console->CursorPos++;
	console->LCommand[j] = ' ';
	console->LCommand[j+1] = '\0';
}

void Cursor_Left(void) {
	char temp[CON_CHARS_PER_LINE];
	
	if(console->CursorPos > 0) {
		console->CursorPos--;
		strcpy(temp, console->RCommand);
		strcpy(console->RCommand, &console->LCommand[strlen(console->LCommand)-1]);
		strcat(console->RCommand, temp);
		console->LCommand[strlen(console->LCommand)-1] = '\0';
		//CON_Out("L:%s, R:%s", console->LCommand, console->RCommand);
	}
}

void Cursor_Right(void) {
	char temp[CON_CHARS_PER_LINE];
	
	if(console->CursorPos < strlen(console->Command)) {
		console->CursorPos++;
		strncat(console->LCommand, console->RCommand, 1);
		strcpy(temp, console->RCommand);
		strcpy(console->RCommand, &temp[1]);
		//CON_Out("L:%s, R:%s", console->LCommand, console->RCommand);
	}
}

void Cursor_Home(void) {
	char temp[CON_CHARS_PER_LINE];
	
	console->CursorPos = 0;
	strcpy(temp, console->RCommand);
	strcpy(console->RCommand, console->LCommand);
	strncat(console->RCommand, temp, strlen(temp));
	memset(console->LCommand, 0, CON_CHARS_PER_LINE);
}

void Cursor_End(void) {
	console->CursorPos = strlen(console->Command);
	strncat(console->LCommand, console->RCommand, strlen(console->RCommand));
	memset(console->RCommand, 0, CON_CHARS_PER_LINE);
}

void Cursor_Del(void) {
	char temp[CON_CHARS_PER_LINE];
	
	if(strlen(console->RCommand) > 0) {
		strcpy(temp, console->RCommand);
		strcpy(console->RCommand, &temp[1]);
	}
}

void Cursor_BSpace(void) {
	if(console->CursorPos > 0) {
		console->CursorPos--;
		console->Offset--;
		if(console->Offset < 0)
			console->Offset = 0;
		console->LCommand[strlen(console->LCommand)-1] = '\0';
	}
}

void Cursor_Add(int event)
{
	if(strlen(console->Command) < CON_CHARS_PER_LINE - 1)
	{
		console->CursorPos++;
		console->LCommand[strlen(console->LCommand)] = key_to_ascii(event);
		console->LCommand[strlen(console->LCommand)] = '\0';
	}
}

void Clear_Command(void) {
	console->CursorPos = 0;
	memset(console->VCommand, 0, CON_CHARS_PER_LINE);
	memset(console->Command, 0, CON_CHARS_PER_LINE);
	memset(console->LCommand, 0, CON_CHARS_PER_LINE);
	memset(console->RCommand, 0, CON_CHARS_PER_LINE);
}

void Clear_History(void) {
	int loop;
	
	for(loop = 0; loop <= console->LineBuffer - 1; loop++)
		memset(console->ConsoleLines[loop], 0, CON_CHARS_PER_LINE);
}

void Command_Up(void) {
	if(console->CommandScrollBack < console->TotalCommands - 1) {
		/* move back a line in the command strings and copy the command to the current input string */
		console->CommandScrollBack++;
		memset(console->RCommand, 0, CON_CHARS_PER_LINE);
		console->Offset = 0;
		strcpy(console->LCommand, console->CommandLines[console->CommandScrollBack]);
		console->CursorPos = strlen(console->CommandLines[console->CommandScrollBack]);
		CON_UpdateConsole();
	}
}

void Command_Down(void) {
	if(console->CommandScrollBack > -1) {
		/* move forward a line in the command strings and copy the command to the current input string */
		console->CommandScrollBack--;
		memset(console->RCommand, 0, CON_CHARS_PER_LINE);
		memset(console->LCommand, 0, CON_CHARS_PER_LINE);
		console->Offset = 0;
		if(console->CommandScrollBack > -1)
			strcpy(console->LCommand, console->CommandLines[console->CommandScrollBack]);
		console->CursorPos = strlen(console->LCommand);
		CON_UpdateConsole();
	}
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

		if (con_initialized)
			CON_Out(buffer);

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
