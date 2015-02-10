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

#define CON_NUM_LINES           128
// Cut the buffer line if it becomes longer than this
#define CON_CHARS_PER_LINE      128
// Cursor blink frequency in ms
#define CON_BLINK_RATE          500
// Border in pixels from the most left to the first letter
#define CON_CHAR_BORDER         4
// Default prompt used at the commandline
#define CON_DEFAULT_PROMPT      "]"
// Cursor shown if we are in insert mode
#define CON_INS_CURSOR          "_"
// Cursor shown if we are in overwrite mode
#define CON_OVR_CURSOR          "|"


/* The console's data */
static char **CommandLines;     // List of all the past commands
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
int InsMode;                    // Insert or Overwrite characters?
static int CommandScrollBack;   // How much the users scrolled back in the command lines


/* Frees all the memory loaded by the console */
static void CON_Free(void);
/* shift command history (the one you can switch with the up/down keys) */
static void CON_NewLineCommand(void);


/* Initializes the console */
void cli_init()
{
	int loop;

	CommandLines = NULL;
	TotalCommands = 0;
	InsMode = 1;
	CursorPos = 0;
	CommandScrollBack = 0;
	Prompt = d_strdup(CON_DEFAULT_PROMPT);

	VChars = CON_CHARS_PER_LINE - 1;
	LineBuffer = CON_NUM_LINES;

	CommandLines = (char **)d_malloc(sizeof(char *) * LineBuffer);
	for (loop = 0; loop <= LineBuffer - 1; loop++) {
		CommandLines[loop] = (char *)d_calloc(CON_CHARS_PER_LINE, sizeof(char));
	}
	memset(Command, 0, CON_CHARS_PER_LINE);
	memset(LCommand, 0, CON_CHARS_PER_LINE);
	memset(RCommand, 0, CON_CHARS_PER_LINE);
	memset(VCommand, 0, CON_CHARS_PER_LINE);

	atexit(CON_Free);
}


/* Frees all the memory loaded by the console */
static void CON_Free(void)
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
static void CON_NewLineCommand(void)
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
void DrawCommandLine(int y)
{
	int x;
	int commandbuffer;
	static unsigned int LastBlinkTime = 0;  // Last time the consoles cursor blinked
	static int LastCursorPos = 0;           // Last Cursor Position
	static int Blink = 0;                   // Is the cursor currently blinking

	commandbuffer = VChars - (int)strlen(Prompt) - 1; // -1 to make cursor visible

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
	gr_string(CON_CHAR_BORDER, y - grd_curfont->ft_h, VCommand);

	// at last add the cursor
	// check if the blink period is over
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
		if (InsMode)
			gr_string(x, y - grd_curfont->ft_h, CON_INS_CURSOR);
		else
			gr_string(x, y - grd_curfont->ft_h, CON_OVR_CURSOR);
	}
}


/* Sets the Prompt for console */
void CON_SetPrompt(char* newprompt) {
	// check length so we can still see at least 1 char :-)
	if (strlen(newprompt) < VChars) {
		d_free(Prompt);
		Prompt = d_strdup(newprompt);
	} else
		CON_Out("prompt too long. (max. %i chars)\n", VChars - 1);
}


/* Executes the command entered */
void CON_Execute(void)
{
	if(strlen(Command) > 0) {
		CON_NewLineCommand();

		// copy the input into the past commands strings
		strcpy(CommandLines[0], Command);

		// display the command including the prompt
		CON_Out("%s%s\n", Prompt, Command);

		cmd_append(Command);

		Clear_Command();
		CommandScrollBack = -1;
	}
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


void Cursor_Add(char character)
{
	if (strlen(Command) < CON_CHARS_PER_LINE - 1)
	{
		CursorPos++;
		LCommand[strlen(LCommand)] = character;
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


void Command_Up(void)
{
	if(CommandScrollBack < TotalCommands - 1) {
		/* move back a line in the command strings and copy the command to the current input string */
		CommandScrollBack++;
		memset(RCommand, 0, CON_CHARS_PER_LINE);
		Offset = 0;
		strcpy(LCommand, CommandLines[CommandScrollBack]);
		CursorPos = (int)strlen(CommandLines[CommandScrollBack]);
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
	}
}
