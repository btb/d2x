#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"
#include "u_mem.h"
#include "strutil.h"
#include "inferno.h"


typedef struct cmd_s
{
	const char    *name;
	cmd_handler_t function;
	const char    *help_text;
	struct cmd_s  *next;
} cmd_t;

/* The list of cmds */
static cmd_t *cmd_list;


#define ALIAS_NAME_MAX 32
typedef struct cmd_alias_s
{
	char           name[ALIAS_NAME_MAX];
	char           *value;
	struct cmd_alias_s *next;
} cmd_alias_t;

/* The list of aliases */
static cmd_alias_t *cmd_alias_list;


typedef struct cmd_queue_s
{
	char *command_line;
	struct cmd_queue_s *next;
} cmd_queue_t;

/* The list of commands to be executed */
static cmd_queue_t *cmd_queue_head;
static cmd_queue_t *cmd_queue_tail;


/* number of cycles to wait before processing any commands */
static int cmd_queue_wait;


/* add a new console command */
void cmd_addcommand(const char *cmd_name, cmd_handler_t cmd_func, const char *cmd_help_text)
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
	cmd->help_text = cmd_help_text;
	cmd->next = cmd_list;
	con_printf(CON_DEBUG, "cmd_addcommand: added %s\n", cmd->name);
	cmd_list = cmd;
}


void cvar_cmd_set(int argc, char **argv);


/* execute a parsed command */
void cmd_execute(int argc, char **argv)
{
	cmd_t *cmd;
	cmd_alias_t *alias;

	for (cmd = cmd_list; cmd; cmd = cmd->next) {
		if (!stricmp(argv[0], cmd->name)) {
			con_printf(CON_DEBUG, "cmd_execute: executing %s\n", argv[0]);
			cmd->function(argc, argv);
			return;
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
	{  // set value of cvar
		char *new_argv[argc+1];
		int i;

		new_argv[0] = "set";
		for (i = 0; i < argc; i++)
			new_argv[i+1] = argv[i];
		cvar_cmd_set(argc + 1, new_argv);
	}
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
	while( isspace(*input) ) { ++input; }
	strncpy( buffer, input, CMD_MAX_LENGTH );

	//printf("lead strip \"%s\"\n",buffer);
	l = (int)strlen(buffer);
	/* If command is empty, give up */
	if (l==0) return;

	/* Strip trailing spaces */
	for (i=l-1; i>0 && isspace(buffer[i]); i--) ;
	buffer[i+1] = 0;
	//printf("trail strip \"%s\"\n",buffer);

	/* Split into tokens */
	l = (int)strlen(buffer);
	num_tokens = 1;

	tokens[0] = buffer;
	for (i=1; i<l; i++) {
		if (buffer[i] == '"') {
			tokens[num_tokens - 1] = &buffer[++i];
			while (i < l && buffer[i] != '"')
				i++;
			buffer[i] = 0;
			continue;
		}
		if (isspace(buffer[i]) || buffer[i] == '=') {
			buffer[i] = 0;
			while (isspace(buffer[i+1]) && (i+1 < l)) i++;
			tokens[num_tokens++] = &buffer[i+1];
		}
	}

	/* Check for matching commands */
	cmd_execute(num_tokens, tokens);
}


int cmd_queue_process(void)
{
	cmd_queue_t *cmd;

	while (!cmd_queue_wait && cmd_queue_head) {
		cmd = cmd_queue_head;
		cmd_queue_head = cmd_queue_head->next;
		if (!cmd_queue_head)
			cmd_queue_tail = NULL;

		con_printf(CON_DEBUG, "cmd_queue_process: processing %s\n", cmd->command_line);
		cmd_parse(cmd->command_line);  // Note, this may change the queue

		d_free(cmd->command_line);
		d_free(cmd);
	}

	if (cmd_queue_wait > 0) {
		cmd_queue_wait--;
		if (Function_mode == FMODE_GAME) {
			con_printf(CON_DEBUG, "cmd_queue_process: waiting\n");
			return 1;
		}
	}

	return 0;
}


/* execute until there are no commands left */
void cmd_queue_flush(void)
{
	while (cmd_queue_process()) {
	}
}


/* Add some commands to the queue to be executed */
void cmd_enqueue(int insert, char *input)
{
	cmd_queue_t *new, *head, *tail;
	char output[CMD_MAX_LENGTH];
	char *optr;

	Assert(input != NULL);
	head = tail = NULL;

	while (*input) {
		optr = output;
		int quoted = 0;

		/* Strip leading spaces */
		while(isspace(*input) || *input == ';')
			input++;

		/* If command is empty, give up */
		if (! *input)
			continue;

		/* Find the end of this line (\n, ;, or nul) */
		do {
			if (!*input)
				break;
			if (*input == '"') {
				quoted = 1 - quoted;
				continue;
			} else if ( *input == '\n' || (!quoted && *input == ';') ) {
				input++;
				break;
			}
		} while ((*optr++ = *input++));
		*optr = 0;

		/* make a new queue item, add it to list */
		MALLOC(new, cmd_queue_t, 1);
		new->command_line = d_strdup(output);
		new->next = NULL;

		if (!head)
			head = new;
		if (tail)
			tail->next = new;
		tail = new;

		con_printf(CON_DEBUG, "cmd_enqueue: adding %s\n", output);
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
			cmd_queue_head = head;
		if (cmd_queue_tail)
			cmd_queue_tail->next = head;
		
		cmd_queue_tail = tail;
		con_printf(CON_DEBUG, "cmd_enqueue: added to back of list\n");
	}
}

void cmd_enqueuef(int insert, const char *fmt, ...)
{
	va_list arglist;
	char buf[CMD_MAX_LENGTH];
	
	va_start (arglist, fmt);
	vsnprintf (buf, CMD_MAX_LENGTH, fmt, arglist);
	va_end (arglist);
	
	cmd_enqueue(insert, buf);
}


/* Attempt to autocomplete an input string */
const char *cmd_complete(char *input)
{
	cmd_t *ptr;
	cmd_alias_t *aptr;

	int len = (int)strlen(input);

	if (!len)
		return NULL;

	for (ptr = cmd_list; ptr != NULL; ptr = ptr->next)
		if (!strnicmp(input, ptr->name, len))
			return ptr->name;

	for (aptr = cmd_alias_list; aptr != NULL; aptr = aptr->next)
		if (!strnicmp(input, aptr->name, len))
			return aptr->name;

	return cvar_complete(input);
}


/* alias */
void cmd_alias(int argc, char **argv)
{
	cmd_alias_t *alias;
	char buf[CMD_MAX_LENGTH] = "";
	int i;

	if (argc < 2) {
		con_printf(CON_NORMAL, "aliases:\n");
		for (alias = cmd_alias_list; alias; alias = alias->next)
			con_printf(CON_NORMAL, "%s: %s\n", alias->name, alias->value);
		return;
	}

	if (argc == 2) {
		for (alias = cmd_alias_list; alias; alias = alias->next)
			if (!stricmp(argv[1], alias->name)) {
				con_printf(CON_NORMAL, "%s: %s\n", alias->name, alias->value);
				return;
			}

		con_printf(CON_NORMAL, "alias: %s not found\n", argv[1]);
		return;
	}

	for (i = 2; i < argc; i++) {
		if (i > 2)
			strncat(buf, " ", CMD_MAX_LENGTH);
		strncat(buf, argv[i], CMD_MAX_LENGTH);
	}

	for (alias = cmd_alias_list; alias; alias = alias->next) {
		if (!stricmp(argv[1], alias->name)) {
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


/* unalias */
void cmd_unalias(int argc, char **argv)
{
	cmd_alias_t *alias, *prev_alias = NULL;

	if (argc < 2 || argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}
	
	for (alias = cmd_alias_list; alias ; alias = alias->next) {
		if (!stricmp(argv[1], alias->name))
			break;
		prev_alias = alias;
	}

	if (!alias) {
		con_printf(CON_NORMAL, "unalias: %s not found\n", argv[1]);
		return;
	}

	if (prev_alias)
		prev_alias->next = alias->next;
	else
		cmd_alias_list = alias->next;

	d_free(alias->value);
	d_free(alias);
}


/* echo to console */
void cmd_echo(int argc, char **argv)
{
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

	if (argc < 2 || argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}
	
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


/* get help */
void cmd_help(int argc, char **argv)
{
	cmd_t *cmd;

	if (argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}

	if (argc < 2) {
		con_printf(CON_NORMAL, "Available commands:\n");
		for (cmd = cmd_list; cmd; cmd = cmd->next) {
			con_printf(CON_NORMAL, "    %s\n", cmd->name);
		}

		return;
	}

	for (cmd = cmd_list; cmd != NULL; cmd = cmd->next)
		if (!stricmp(argv[1], cmd->name))
			break;

	if (!cmd) {
		con_printf(CON_URGENT, "Command %s not found\n", argv[1]);
		return;
	}

	if (!cmd->help_text) {
		con_printf(CON_NORMAL, "%s: no help found\n", argv[1]);
		return;
	}

	con_printf(CON_NORMAL, "%s\n", cmd->help_text);
}


/* execute script */
void cmd_wait(int argc, char **argv)
{
	if (argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}

	if (argc < 2)
		cmd_queue_wait = 1;
	else
		cmd_queue_wait = atoi(argv[1]);
}


void cmd_free(void)
{
	cmd_t *cmd_p;
	cmd_alias_t *alias_p;

	cmd_p = cmd_list;
	while (cmd_p) {
		cmd_t *temp = cmd_p;
		cmd_p = cmd_p->next;
		d_free(temp);
	}

	alias_p = cmd_alias_list;
	while (alias_p) {
		cmd_alias_t *temp = alias_p;
		d_free(alias_p->value);
		alias_p = alias_p->next;
		d_free(temp);
	}
}


void cmd_init(void)
{
	cmd_addcommand("alias",     cmd_alias,      "alias <name> <commands>\n" "    define <name> as an alias for <commands>\n"
	                                            "alias <name>\n"            "    show the current definition of <name>\n"
	                                            "alias\n"                   "    show all defined aliases");
	cmd_addcommand("unalias",   cmd_unalias,    "unalias <name>\n"          "    undefine the alias <name>");
	cmd_addcommand("echo",      cmd_echo,       "echo [text]\n"             "    write <text> to the console");
	cmd_addcommand("exec",      cmd_exec,       "exec <file>\n"             "    execute <file>");
	cmd_addcommand("help",      cmd_help,       "help [command]\n"          "    get help for <command>, or list all commands if not specified.");
	cmd_addcommand("wait",      cmd_wait,       "usage: wait [n]\n"         "    stop processing commands, resume in <n> cycles (default 1)");

	atexit(cmd_free);
}
