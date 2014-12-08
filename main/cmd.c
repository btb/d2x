#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pstypes.h"
#include "cmd.h"
#include "console.h"
#include "error.h"
#include "u_mem.h"
#include "strutil.h"


typedef struct cmd_s
{
	char          *name;
	cmd_handler_t function;
	struct cmd_s  *next;
} cmd_t;

static cmd_t *cmd_list = NULL;


/* add a new console command */
void cmd_addcommand(char *cmd_name, cmd_handler_t cmd_func)
{
	cmd_t *cmd;

	Assert(cmd_name != NULL);

	for (cmd = cmd_list; cmd; cmd = cmd->next) {
		if (!stricmp(cmd_name, cmd->name))
		{
			Int3();
			con_printf(CON_NORMAL, "command %s already exists, not adding\n", cmd_name);
			return;
		}
	}

	/* create command, insert at front of list */
	MALLOC(cmd, cmd_t, 1);
	cmd->name = cmd_name;
	cmd->function = cmd_func;
	cmd->next = cmd_list;
	cmd_list = cmd;
}


/* execute a parsed command */
void cmd_execute(int argc, char **argv)
{
	cmd_t *cmd;
	for (cmd = cmd_list; cmd; cmd = cmd->next) {
		if (!stricmp(argv[0], cmd->name))
			return cmd->function(argc, argv);
	}

	/* Otherwise */
	if (argc > 1)  // set value of cvar
		cvar_set(argv[0], argv[1]);
	con_printf(CON_NORMAL, "%s: %f\n", argv[0], cvar(argv[0]));
}


/* Parse an input string */
void cmd_parse(char *input)
{
	char buffer[CMD_MAX_LENGTH];
	char *tokens[CMD_MAX_TOKENS];
	int num_tokens;
	int i, l;

	Assert(input != NULL);
	
	/* Strip leading spaces */
	for (i=0; isspace(input[i]); i++) ;
	strncpy( buffer, &input[i], CMD_MAX_LENGTH );

	printf("lead strip \"%s\"\n",buffer);
	l = strlen(buffer);
	/* If command is empty, give up */
	if (l==0) return;

	/* Strip trailing spaces */
	for (i=l-1; i>0 && isspace(buffer[i]); i--) ;
	buffer[i+1] = 0;
	printf("trail strip \"%s\"\n",buffer);

	/* Split into tokens */
	l = strlen(buffer);
	num_tokens = 1;

	tokens[0] = buffer;
	for (i=1; i<l; i++) {
        	if (isspace(buffer[i])) {
                	buffer[i] = 0;
			while (isspace(buffer[i+1]) && (i+1 < l)) i++;
			tokens[num_tokens++] = &buffer[i+1];
		}
	}

	/* Check for matching commands */
	cmd_execute(num_tokens, tokens);
}



/* +/- actions */

int Console_button_states[CMD_NUM_BUTTONS];

void cmd_attack_on(int argc, char **argv) { Console_button_states[CMD_ATTACK] = 1; }
void cmd_attack_off(int argc, char **argv) { Console_button_states[CMD_ATTACK] = 0; }
void cmd_attack2_on(int argc, char **argv) { Console_button_states[CMD_ATTACK2] = 1; }
void cmd_attack2_off(int argc, char **argv) { Console_button_states[CMD_ATTACK2] = 0; }


void cmd_init(void){
	memset(Console_button_states, 0, sizeof(int) * CMD_NUM_BUTTONS);

	cmd_addcommand("+attack", cmd_attack_on);
	cmd_addcommand("-attack", cmd_attack_off);
	cmd_addcommand("+attack2", cmd_attack2_on);
	cmd_addcommand("-attack2", cmd_attack2_off);
}
