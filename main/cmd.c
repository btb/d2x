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
#include "weapon.h"
#include "key.h"


typedef struct cmd_s
{
	char          *name;
	cmd_handler_t function;
	struct cmd_s  *next;
} cmd_t;

/* The list of cmds */
static cmd_t *cmd_list = NULL;


#define ALIAS_NAME_MAX 32
typedef struct cmd_alias_s
{
	char           name[ALIAS_NAME_MAX];
	char           *value;
	struct cmd_alias_s *next;
} cmd_alias_t;

/* The list of aliases */
static cmd_alias_t *cmd_alias_list = NULL;


/* The list of keybindings */
static char *cmd_keybinding_list[256];


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
	con_printf(CON_DEBUG, "cmd_addcommand: added %s\n", cmd->name);
	cmd_list = cmd;
}


typedef struct cmd_queue_s
{
	char *command_line;
	struct cmd_queue_s *next;
} cmd_queue_t;

/* The list of commands to be executed */
static cmd_queue_t *cmd_queue_head = NULL;
static cmd_queue_t *cmd_queue_tail = NULL;


/* execute a parsed command */
void cmd_execute(int argc, char **argv)
{
	cmd_t *cmd;
	cmd_alias_t *alias;

	for (cmd = cmd_list; cmd; cmd = cmd->next) {
		if (!stricmp(argv[0], cmd->name)) {
			con_printf(CON_DEBUG, "cmd_execute: executing %s\n", argv[0]);
			return cmd->function(argc, argv);
		}
	}

	for (alias = cmd_alias_list; alias; alias = alias->next) {
		if (!stricmp(argv[0], alias->name)) {
			con_printf(CON_DEBUG, "cmd_execute: pushing alias \"%s\": %s\n", alias->name, alias->value);
			cmd_insert(alias->value);
			return;
		}
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
	int quoted = 0;

	Assert(input != NULL);
	
	/* Strip leading spaces */
	for (i=0; isspace(input[i]); i++) ;
	strncpy( buffer, &input[i], CMD_MAX_LENGTH );

	//printf("lead strip \"%s\"\n",buffer);
	l = strlen(buffer);
	/* If command is empty, give up */
	if (l==0) return;

	/* Strip trailing spaces */
	for (i=l-1; i>0 && isspace(buffer[i]); i--) ;
	buffer[i+1] = 0;
	//printf("trail strip \"%s\"\n",buffer);

	/* Split into tokens */
	l = strlen(buffer);
	num_tokens = 1;

	tokens[0] = buffer;
	for (i=1; i<l; i++) {
		if (buffer[i] == '"') {
			quoted = 1 - quoted;
			continue;
		}
		if ((isspace(buffer[i]) || buffer[i] == '=') && !quoted) {
			buffer[i] = 0;
			while (isspace(buffer[i+1]) && (i+1 < l)) i++;
			tokens[num_tokens++] = &buffer[i+1];
		}
	}

	/* Check for matching commands */
	cmd_execute(num_tokens, tokens);
}

void cmd_queue_process()
{
	cmd_queue_t *cmd;

	while (cmd_queue_head) {
		cmd = cmd_queue_head;
		cmd_queue_head = cmd_queue_head->next;
		if (!cmd_queue_head)
			cmd_queue_tail = NULL;

		con_printf(CON_DEBUG, "cmd_queue_process: processing %s\n", cmd->command_line);
		cmd_parse(cmd->command_line);  // Note, this may change the queue

		d_free(cmd->command_line);
		d_free(cmd);
	}
}


/* Add some commands to the queue to be executed */
void cmd_enqueue(int insert, char *input)
{
	cmd_queue_t *new, *head, *tail;
	char *line;

	Assert(input != NULL);
	head = tail = NULL;

	while (*input) {
		int quoted = 0;

		/* Strip leading spaces */
		while(isspace(*input) || *input == ';')
			input++;

		/* If command is empty, give up */
		if (! *input)
			continue;

		/* Now at start of a command line */
		line = input;

		/* Find the end of this line (\n, ;, or nul) */
		do {
			if (!*input)
				break;
			if (*input == '"') {
				quoted = 1 - quoted;
				continue;
			} else if ( *input == '\n' || (!quoted && *input == ';') ) {
				*input = 0;
				break;
			}
		} while (*input++);

		/* make a new queue item, add it to list */
		MALLOC(new, cmd_queue_t, 1);
		new->command_line = d_strdup(line);
		new->next = NULL;

		if (!head)
			head = new;
		if (tail)
			tail->next = new;
		tail = new;

		con_printf(CON_DEBUG, "cmd_enqueue: adding %s\n", line);
	}

	if (insert) {
		 /* add our list to the head of the main list */
		if (cmd_queue_head)
			tail->next = cmd_queue_head;
		if (!cmd_queue_tail)
			cmd_queue_tail = tail;
		
		cmd_queue_head = head;
		con_printf(CON_DEBUG, "cmd_enqueue: added to front of list\n");
	} else {
		/* add our list to the tail of the main list */
		if (!cmd_queue_head)
			cmd_queue_head = new;
		if (cmd_queue_tail)
			cmd_queue_tail->next = head;
		
		cmd_queue_tail = tail;
		con_printf(CON_DEBUG, "cmd_enqueue: added to back of list\n");
	}
}

void cmd_enqueuef(int insert, char *fmt, ...){
	va_list arglist;
	char buf[CMD_MAX_LENGTH];
	
	va_start (arglist, fmt);
	vsnprintf (buf, CMD_MAX_LENGTH, fmt, arglist);
	va_end (arglist);
	
	cmd_enqueue(insert, buf);
}


/* Attempt to autocomplete an input string */
char *cmd_complete(char *input)
{
	CON_Out("     No autocomplete yet");
	return NULL;
}


int cmd_handle_keybinding(unsigned char key)
{
	if (cmd_keybinding_list[key]) {
		cmd_insert(cmd_keybinding_list[key]);
		return 1;
	}
	return 0;
}



/* alias */
void cmd_alias(int argc, char **argv)
{
	cmd_alias_t *alias;
	char buf[CMD_MAX_LENGTH] = "";
	int i;

	if (argc < 2)
	{
		con_printf(CON_NORMAL, "aliases:\n");
		for (alias = cmd_alias_list; alias; alias = alias->next)
			con_printf(CON_NORMAL, "%s: %s\n", alias->name, alias->value);
		return;
	}

	for (i = 2; i < argc; i++) {
		if (i > 2)
			strncat(buf, " ", CMD_MAX_LENGTH);
		strncat(buf, argv[i], CMD_MAX_LENGTH);
	}

	for (alias = cmd_alias_list; alias; alias = alias->next) {
		if (!stricmp(argv[1], alias->name))
		{
			d_free(alias->value);
			alias->value = d_strdup(buf);
			return;
		}
	}

	MALLOC(alias, cmd_alias_t, 1);
	strncpy(alias->name, argv[1], ALIAS_NAME_MAX);
	alias->value = d_strdup(buf);
	alias->next = cmd_alias_list;
	cmd_alias_list = alias;
}

/* bind */
/* FIXME: key_text is not really adequate for this */
void cmd_bind(int argc, char **argv)
{
	char buf[CMD_MAX_LENGTH] = "";
	unsigned char key = 0;
	int i;

	if (argc < 2)
	{
		con_printf(CON_NORMAL, "key bindings:\n");
		for (i = 0; i < 256; i++) {
			if (!cmd_keybinding_list[i])
				continue;
			con_printf(CON_NORMAL, "%s: %s\n", key_text[i], cmd_keybinding_list[i]);
		}
		return;
	}

	for (i = 2; i < argc; i++) {
		if (i > 2)
			strncat(buf, " ", CMD_MAX_LENGTH);
		strncat(buf, argv[i], CMD_MAX_LENGTH);
	}

	for (i = 0; i < 256; i++) {
		if (!stricmp(argv[1], key_text[i])) {
			key = i;
			break;
		}
	}

	if (!key) {
		con_printf(CON_CRITICAL, "bind: key %s not found\n", argv[1]);
		return;
	}

	if (cmd_keybinding_list[key])
		d_free(cmd_keybinding_list[key]);
	cmd_keybinding_list[key] = d_strdup(buf);
}

/* +/- actions */
int Console_button_states[CMD_NUM_BUTTONS];
void cmd_attack_on(int argc, char **argv) { Console_button_states[CMD_ATTACK] = 1; }
void cmd_attack_off(int argc, char **argv) { Console_button_states[CMD_ATTACK] = 0; }
void cmd_attack2_on(int argc, char **argv) { Console_button_states[CMD_ATTACK2] = 1; }
void cmd_attack2_off(int argc, char **argv) { Console_button_states[CMD_ATTACK2] = 0; }

/* weapon select */
void cmd_impulse(int argc, char**argv) {
	if (argc < 2)
		return;
	int n = atoi(argv[1]);
	if (n >= 1 && n <= 20) {
		select_weapon((n-1) % 10, (n-1) / 10, 0, 1);
	}
}

/* echo to console */
void cmd_echo(int argc, char **argv) {
	char buf[CMD_MAX_LENGTH] = "";
	int i;
	for (i = 1; i < argc; i++) {
		if (i > 1)
			strncat(buf, " ", CMD_MAX_LENGTH);
		strncat(buf, argv[i], CMD_MAX_LENGTH);
	}
	con_printf(CON_NORMAL, "%s\n", buf);
}

/* execute script */
void cmd_exec(int argc, char **argv) {
	cmd_queue_t *new, *head, *tail;
	PHYSFS_File *f;
	char line[CMD_MAX_LENGTH] = "";

	if (argc < 2)
		return;

	head = tail = NULL;

	f = PHYSFSX_openReadBuffered(argv[1]);
	if (!f) {
		con_printf(CON_CRITICAL, "exec: %s not found\n", argv[1]);
		return;
	}
	while (PHYSFSX_gets(f, line)) {
		/* make a new queue item, add it to list */
		MALLOC(new, cmd_queue_t, 1);
		new->command_line = d_strdup(line);
		new->next = NULL;

		if (!head)
			head = new;
		if (tail)
			tail->next = new;
		tail = new;

		con_printf(CON_DEBUG, "cmd_exec: adding %s\n", line);
	}
	PHYSFS_close(f);

	/* add our list to the head of the main list */
	if (cmd_queue_head)
		tail->next = cmd_queue_head;
	if (!cmd_queue_tail)
		cmd_queue_tail = tail;

	cmd_queue_head = head;
	con_printf(CON_DEBUG, "cmd_exec: added to front of list\n");
}


void cmd_free(void)
{
	int i;
	void *p, *temp;

	p = cmd_list;
	while (p) {
		temp = p;
		p = ((cmd_t *)p)->next;
		d_free(temp);
	}

	p = cmd_alias_list;
	while (p) {
		d_free(((cmd_alias_t *)p)->value);
		temp = p;
		p = ((cmd_alias_t *)p)->next;
		d_free(temp);
	}

	for (i = 0; i < 256; i++)
		if (cmd_keybinding_list[i])
			d_free(cmd_keybinding_list[i]);
}


void cmd_init(void){
	memset(Console_button_states, 0, sizeof(int) * CMD_NUM_BUTTONS);

	cmd_addcommand("alias", cmd_alias);
	cmd_addcommand("bind", cmd_bind);

	cmd_addcommand("+attack", cmd_attack_on);
	cmd_addcommand("-attack", cmd_attack_off);
	cmd_addcommand("+attack2", cmd_attack2_on);
	cmd_addcommand("-attack2", cmd_attack2_off);

	cmd_addcommand("impulse", cmd_impulse);

	cmd_addcommand("echo", cmd_echo);

	cmd_addcommand("exec", cmd_exec);

	atexit(cmd_free);
}
