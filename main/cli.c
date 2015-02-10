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

#include <string.h>

#include "inferno.h"
#include "maths.h"
#include "gr.h"
#include "timer.h"
#include "u_mem.h"
#include "strutil.h"


#define get_msecs() approx_fsec_to_msec(timer_get_approx_seconds())

#define CLI_NUM_LINES           128
// Cut the buffer line if it becomes longer than this
#define CLI_CHARS_PER_LINE      128
// Cursor blink frequency in ms
#define CLI_BLINK_RATE          500
// Border in pixels from the most left to the first letter
#define CLI_CHAR_BORDER         4
// Default prompt used at the commandline
#define CLI_DEFAULT_PROMPT      "]"
// Cursor shown if we are in insert mode
#define CLI_INS_CURSOR          "_"
// Cursor shown if we are in overwrite mode
#define CLI_OVR_CURSOR          "|"

int CLI_insert_mode;            // Insert or Overwrite characters?

static char **CommandLines;     // List of all the past commands
static int TotalCommands;       // Number of commands in the Back Commands
static int LineBuffer;          // The number of visible lines in the console (autocalculated)
static char *Prompt;            // Prompt displayed in command line
static char  Command[CLI_CHARS_PER_LINE];   // current command in command line = lcommand + rcommand
static char LCommand[CLI_CHARS_PER_LINE];   // right hand side of cursor
static char RCommand[CLI_CHARS_PER_LINE];   // left hand side of cursor
static char VCommand[CLI_CHARS_PER_LINE];   // current visible command line
static int CursorPos;           // Current cursor position in CurrentCommand
static int Offset;              // CommandOffset (first visible char of command) - if command is too long to fit into console
static int CommandScrollBack;   // How much the users scrolled back in the command lines


/* Frees all the memory loaded by the cli */
static void cli_free(void);
/* shift command history (the one you can switch with the up/down keys) */
static void cli_newline(void);


/* Initializes the cli */
void cli_init()
{
	int loop;

	CommandLines = NULL;
	TotalCommands = 0;
	CLI_insert_mode = 1;
	CursorPos = 0;
	CommandScrollBack = 0;
	Prompt = d_strdup(CLI_DEFAULT_PROMPT);
	LineBuffer = CLI_NUM_LINES;

	CommandLines = (char **)d_malloc(sizeof(char *) * LineBuffer);
	for (loop = 0; loop <= LineBuffer - 1; loop++) {
		CommandLines[loop] = (char *)d_calloc(CLI_CHARS_PER_LINE, sizeof(char));
	}
	memset(Command, 0, CLI_CHARS_PER_LINE);
	memset(LCommand, 0, CLI_CHARS_PER_LINE);
	memset(RCommand, 0, CLI_CHARS_PER_LINE);
	memset(VCommand, 0, CLI_CHARS_PER_LINE);

	atexit(cli_free);
}


/* Frees all the memory loaded by the console */
static void cli_free(void)
{
	int i;

	for (i = 0; i <= LineBuffer - 1; i++) {
		d_free(CommandLines[i]);
	}
	d_free(CommandLines);

	CommandLines = NULL;

	d_free(Prompt);
}


/* Increments the command lines */
static void cli_newline(void)
{
	int loop;
	char *temp;

	temp  = CommandLines[LineBuffer - 1];

	for (loop = LineBuffer - 1; loop > 0; loop--)
		CommandLines[loop] = CommandLines[loop - 1];

	CommandLines[0] = temp;

	memset(CommandLines[0], 0, CLI_CHARS_PER_LINE);
	if (TotalCommands < LineBuffer - 1)
		TotalCommands++;
}


/* Draws the command line the user is typing in to the screen */
/* completely rewritten by C.Wacha */
void cli_draw(int y)
{
	int x, w, h, aw;
	float real_aw;
	int commandbuffer;
	static unsigned int LastBlinkTime = 0;  // Last time the consoles cursor blinked
	static int LastCursorPos = 0;           // Last Cursor Position
	static int Blink = 0;                   // Is the cursor currently blinking

	// Concatenate the left and right side to command
	strcpy(Command, LCommand);
	strncat(Command, RCommand, strlen(RCommand));

	gr_get_string_size(Command, &w, &h, &aw);
	if (w > 0 && strlen(Command))
		real_aw = (float)w/(float)strlen(Command);
	else
		real_aw = (float)aw;
	commandbuffer = (GWIDTH - 2*CLI_CHAR_BORDER)/real_aw - (int)strlen(Prompt) - 1; // -1 to make cursor visible

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
	gr_string(CLI_CHAR_BORDER, y-h, VCommand);

	// at last add the cursor
	// check if the blink period is over
	if (get_msecs() > LastBlinkTime) {
		LastBlinkTime = get_msecs() + CLI_BLINK_RATE;
		if(Blink)
			Blink = 0;
		else
			Blink = 1;
	}

	// check if cursor has moved - if yes display cursor anyway
	if (CursorPos != LastCursorPos) {
		LastCursorPos = CursorPos;
		LastBlinkTime = get_msecs() + CLI_BLINK_RATE;
		Blink = 1;
	}

	if (Blink) {
		int prompt_width, cmd_width, h, w;

		gr_get_string_size(Prompt, &prompt_width, &h, &w);
		gr_get_string_size(LCommand + Offset, &cmd_width, &h, &w);
		x = CLI_CHAR_BORDER + prompt_width + cmd_width;
		if (CLI_insert_mode)
			gr_string(x, y-h, CLI_INS_CURSOR);
		else
			gr_string(x, y-h, CLI_OVR_CURSOR);
	}
}


/* Executes the command entered */
void cli_execute(void)
{
	if(strlen(Command) > 0) {
		cli_newline();

		// copy the input into the past commands strings
		strcpy(CommandLines[0], Command);

		// display the command including the prompt
		con_printf(CON_NORMAL, "%s%s\n", Prompt, Command);

		cmd_append(Command);

		cli_clear();
		CommandScrollBack = -1;
	}
}


void cli_autocomplete(void)
{
	int i, j;
	char *command;

	command = cmd_complete(LCommand);

	if (!command)
		return; // no tab completion took place so return silently

	j = (int)strlen(command);
	if (j > CLI_CHARS_PER_LINE - 2)
		j = CLI_CHARS_PER_LINE-1;

	memset(LCommand, 0, CLI_CHARS_PER_LINE);
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


void cli_cursor_left(void)
{
	char temp[CLI_CHARS_PER_LINE];

	if (CursorPos > 0) {
		CursorPos--;
		strcpy(temp, RCommand);
		strcpy(RCommand, &LCommand[strlen(LCommand)-1]);
		strcat(RCommand, temp);
		LCommand[strlen(LCommand)-1] = '\0';
	}
}


void cli_cursor_right(void)
{
	char temp[CLI_CHARS_PER_LINE];

	if(CursorPos < strlen(Command)) {
		CursorPos++;
		strncat(LCommand, RCommand, 1);
		strcpy(temp, RCommand);
		strcpy(RCommand, &temp[1]);
	}
}


void cli_cursor_home(void)
{
	char temp[CLI_CHARS_PER_LINE];

	CursorPos = 0;
	strcpy(temp, RCommand);
	strcpy(RCommand, LCommand);
	strncat(RCommand, temp, strlen(temp));
	memset(LCommand, 0, CLI_CHARS_PER_LINE);
}


void cli_cursor_end(void)
{
	CursorPos = (int)strlen(Command);
	strncat(LCommand, RCommand, strlen(RCommand));
	memset(RCommand, 0, CLI_CHARS_PER_LINE);
}


void cli_cursor_del(void)
{
	char temp[CLI_CHARS_PER_LINE];

	if (strlen(RCommand) > 0) {
		strcpy(temp, RCommand);
		strcpy(RCommand, &temp[1]);
	}
}

void cli_cursor_backspace(void)
{
	if (CursorPos > 0) {
		CursorPos--;
		Offset--;
		if (Offset < 0)
			Offset = 0;
		LCommand[strlen(LCommand)-1] = '\0';
	}
}


void cli_add_character(char character)
{
	if (strlen(Command) < CLI_CHARS_PER_LINE - 1)
	{
		CursorPos++;
		LCommand[strlen(LCommand)] = character;
		LCommand[strlen(LCommand)] = '\0';
	}
	if (!CLI_insert_mode)
		cli_cursor_del();
}


void cli_clear(void)
{
	CursorPos = 0;
	memset( Command, 0, CLI_CHARS_PER_LINE);
	memset(LCommand, 0, CLI_CHARS_PER_LINE);
	memset(RCommand, 0, CLI_CHARS_PER_LINE);
	memset(VCommand, 0, CLI_CHARS_PER_LINE);
}


void cli_history_prev(void)
{
	if(CommandScrollBack < TotalCommands - 1) {
		/* move back a line in the command strings and copy the command to the current input string */
		CommandScrollBack++;
		memset(RCommand, 0, CLI_CHARS_PER_LINE);
		Offset = 0;
		strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = (int)strlen(CommandLines[CommandScrollBack]);
	}
}


void cli_history_next(void)
{
	if(CommandScrollBack > -1) {
		/* move forward a line in the command strings and copy the command to the current input string */
		CommandScrollBack--;
		memset(RCommand, 0, CLI_CHARS_PER_LINE);
		memset(LCommand, 0, CLI_CHARS_PER_LINE);
		Offset = 0;
		if(CommandScrollBack > -1)
			strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = (int)strlen(LCommand);
	}
}
