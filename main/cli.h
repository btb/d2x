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

#ifndef _CLI_H_
#define _CLI_H_ 1

// Insert or Overwrite characters?
extern int InsMode;

void cli_init(void);
/* executes the command typed in at the console (called if you press ENTER)*/
void CON_Execute();
/* Gets called when TAB was pressed */
void CON_TabCompletion(void);
/* draws the commandline the user is typing in to the screen. called by update? */
void DrawCommandLine(int y);
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
void Cursor_Add(char event);
/* Called if you press Ctrl-C (deletes the commandline) */
void Clear_Command(void);
/* Called if you press UP key (switches through recent typed in commands */
void Command_Up(void);
/* Called if you press DOWN key (switches through recent typed in commands */
void Command_Down(void);

#endif /* _CLI_H_ */
