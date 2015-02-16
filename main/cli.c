/*
 *  Command-line interface for the console
 *
 *  Based on an early version of SDL_Console
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


#define CLI_HISTORY_MAX         128
// Cut the buffer line if it becomes longer than this
#define CLI_CHARS_PER_LINE      128
// Cursor blink interval
#define CLI_BLINK_RATE          (F1_0/2)
// Border in pixels from the most left to the first letter
#define CLI_CHAR_BORDER         4
// Default prompt used at the commandline
#define CLI_DEFAULT_PROMPT      "]"
// Cursor shown if we are in insert mode
#define CLI_INS_CURSOR          "_"
// Cursor shown if we are in overwrite mode
#define CLI_OVR_CURSOR          "|"

int CLI_insert_mode;                        // Insert or Overwrite characters?

static char *CommandLines[CLI_HISTORY_MAX]; // List of all the past commands
static int TotalCommands;                   // Number of commands in the Back Commands
static char *Prompt;                        // Prompt displayed in command line
static char Command[CLI_CHARS_PER_LINE];    // current command in command line = lcommand + rcommand
static char LCommand[CLI_CHARS_PER_LINE];   // right hand side of cursor
static char RCommand[CLI_CHARS_PER_LINE];   // left hand side of cursor
static char VCommand[CLI_CHARS_PER_LINE];   // current visible command line
static size_t CursorPos;                    // Current cursor position in CurrentCommand
static size_t Offset;                       // CommandOffset (first visible char of command) - if command is too long to fit into console
static int CommandScrollBack;               // How much the users scrolled back in the command lines


/* Frees all the memory loaded by the cli */
static void cli_free(void)
{
	int i;

	for (i = 0; i <= CLI_HISTORY_MAX - 1; i++) {
		if (CommandLines[i])
			d_free(CommandLines[i]);
	}

	d_free(Prompt);
}


/* Initializes the cli */
void cli_init(void)
{
	TotalCommands = 0;
	CLI_insert_mode = 1;
	CursorPos = 0;
	CommandScrollBack = 0;
	Prompt = d_strdup(CLI_DEFAULT_PROMPT);

	memset(CommandLines, 0, sizeof(CommandLines));
	memset(Command, 0, sizeof(Command));
	memset(LCommand, 0, sizeof(LCommand));
	memset(RCommand, 0, sizeof(RCommand));
	memset(VCommand, 0, sizeof(VCommand));

	atexit(cli_free);
}


/* Increments the command lines */
static void cli_newline(void)
{
	int loop;

	if (CommandLines[CLI_HISTORY_MAX - 1])
		d_free(CommandLines[CLI_HISTORY_MAX - 1]);

	for (loop = CLI_HISTORY_MAX - 1; loop > 0; loop--)
		CommandLines[loop] = CommandLines[loop - 1];

	CommandLines[0] = NULL;

	if (TotalCommands < CLI_HISTORY_MAX - 1)
		TotalCommands++;
}


/* Draws the command line the user is typing in to the screen */
/* completely rewritten by C.Wacha */
void cli_draw(int y)
{
	int x, w, h, aw;
	float real_aw;
	size_t commandbuffer;
	fix cur_time = timer_get_fixed_seconds();
	static fix LastBlinkTime = 0;   // Last time the cursor blinked
	static size_t LastCursorPos = 0; // Last cursor position
	static int Blink = 0;           // Is the cursor currently blinking

	// Concatenate the left and right side to command
	strcpy(Command, LCommand);
	strncat(Command, RCommand, sizeof(Command) - strlen(Command) - 1);

	gr_get_string_size(Command, &w, &h, &aw);
	if (w > 0 && *Command)
		real_aw = (float)w/(float)strlen(Command);
	else
		real_aw = (float)aw;
	commandbuffer = (GWIDTH - 2*CLI_CHAR_BORDER)/real_aw - strlen(Prompt) - 1; // -1 to make cursor visible

	//calculate display offset from current cursor position
	if (CursorPos > commandbuffer && Offset < CursorPos - commandbuffer)
		Offset = CursorPos - commandbuffer;
	if(Offset > CursorPos)
		Offset = CursorPos;

	// first add prompt to visible part
	strcpy(VCommand, Prompt);

	// then add the visible part of the command
	strncat(VCommand, &Command[Offset], sizeof(VCommand) - strlen(VCommand) - 1);

	// now display the result
	gr_string(CLI_CHAR_BORDER, y-h, VCommand);

	// at last add the cursor
	// check if the blink period is over
	if (cur_time > LastBlinkTime) {
		LastBlinkTime = cur_time + CLI_BLINK_RATE;
		if(Blink)
			Blink = 0;
		else
			Blink = 1;
	}

	// check if cursor has moved - if yes display cursor anyway
	if (CursorPos != LastCursorPos) {
		LastCursorPos = CursorPos;
		LastBlinkTime = cur_time + CLI_BLINK_RATE;
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
		CommandLines[0] = d_strdup(Command);

		// display the command including the prompt
		con_printf(CON_NORMAL, "%s%s\n", Prompt, Command);

		cmd_append(Command);

		cli_clear();
		CommandScrollBack = -1;
	}
}


void cli_autocomplete(void)
{
	size_t i, j;
	const char *command;

	command = cmd_complete(LCommand);

	if (!command)
		return; // no tab completion took place so return silently

	j = strlen(command);
	if (j > CLI_CHARS_PER_LINE - 2)
		j = CLI_CHARS_PER_LINE - 1;

	memset(LCommand, 0, sizeof(LCommand));
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
	memset(LCommand, 0, sizeof(LCommand));
}


void cli_cursor_end(void)
{
	CursorPos = strlen(Command);
	strncat(LCommand, RCommand, strlen(RCommand));
	memset(RCommand, 0, sizeof(RCommand));
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
		if (Offset > 0)
			Offset--;
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
	memset(Command, 0, sizeof(Command));
	memset(LCommand, 0, sizeof(LCommand));
	memset(RCommand, 0, sizeof(RCommand));
	memset(VCommand, 0, sizeof(VCommand));
}


void cli_history_prev(void)
{
	if(CommandScrollBack < TotalCommands - 1) {
		/* move back a line in the command strings and copy the command to the current input string */
		CommandScrollBack++;
		memset(RCommand, 0, sizeof(RCommand));
		Offset = 0;
		strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = strlen(CommandLines[CommandScrollBack]);
	}
}


void cli_history_next(void)
{
	if(CommandScrollBack > -1) {
		/* move forward a line in the command strings and copy the command to the current input string */
		CommandScrollBack--;
		memset(RCommand, 0, sizeof(RCommand));
		memset(LCommand, 0, sizeof(LCommand));
		Offset = 0;
		if(CommandScrollBack > -1)
			strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = strlen(LCommand);
	}
}
