#ifndef _CMD_H_
#define _CMD_H_ 1


void cmd_init(void);

/* Maximum length for a single command */
#define CMD_MAX_LENGTH 2048
/* Maximum number of tokens per command */
#define CMD_MAX_TOKENS 64

/* Parse an input string */
void cmd_parse(char *input);  // FIXME: make this handle compound statements, add flag for insert/append?
void cmd_parsef(char *fmt, ...);
/* Add some commands to the queue to be executed */
void cmd_insert(char *input);
/* Add some commands to the queue to be executed */
void cmd_append(char *input);

/* Attempt to autocomplete an input string */
char *cmd_complete(char *input);

typedef void (*cmd_handler_t)(int argc, char *argv[]);

void cmd_addcommand(char *cmd_name, cmd_handler_t cmd_func);


/* +/- actions */

#define CMD_NUM_BUTTONS 2

typedef enum
{
	CMD_ATTACK,  // Fire primary weapon
	CMD_ATTACK2, // Fire secondary weapon
} cmd_button;

extern int Console_button_states[CMD_NUM_BUTTONS];


/* execute a bound key's command */
int cmd_handle_keybinding(unsigned char key);


#endif /* _CMD_H_ */
