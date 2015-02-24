/* Console variables */


#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <float.h>

#include "console.h"
#include "error.h"
#include "strutil.h"
#include "u_mem.h"
#include "hash.h"


#define CVAR_MAX_LENGTH 1024
#define CVAR_MAX_CVARS  1024

/* The list of cvars */
hashtable cvar_hash;
cvar_t *cvar_list[CVAR_MAX_CVARS];
int Num_cvars;


void cvar_free(void)
{
	while (Num_cvars--)
		d_free(cvar_list[Num_cvars]->string);

	hashtable_free(&cvar_hash);
}


void cvar_cmd_set(int argc, char **argv)
{
	char buf[CVAR_MAX_LENGTH];
	int ret, i;

	if (argc == 2) {
		cvar_t *ptr;

		if ((ptr = cvar_find(argv[1])))
			con_printf(CON_NORMAL, "%s: %s\n", ptr->name, ptr->string);
		else
			con_printf(CON_NORMAL, "set: variable %s not found\n", argv[1]);
		return;
	}

	if (argc == 1) {
		for (i = 0; i < Num_cvars; i++)
			con_printf(CON_NORMAL, "%s: %s\n", cvar_list[i]->name, cvar_list[i]->string);
		return;
	}

	ret = snprintf(buf, sizeof(buf), "%s", argv[2]);
	if (ret >= CVAR_MAX_LENGTH) {
		con_printf(CON_CRITICAL, "set: value too long (max %d characters)\n", CVAR_MAX_LENGTH);
		return;
	}

	for (i = 3; i < argc; i++) {
		ret = snprintf(buf, CVAR_MAX_LENGTH, "%s %s", buf, argv[i]);
		if (ret >= CVAR_MAX_LENGTH) {
			con_printf(CON_CRITICAL, "set: value too long (max %d characters)\n", CVAR_MAX_LENGTH);
			return;
		}
	}
	cvar_set(argv[1], buf);
}


void cvar_init(void)
{
	hashtable_init( &cvar_hash, CVAR_MAX_CVARS );

	cmd_addcommand("set", cvar_cmd_set, "set <name> <value>\n"  "    set variable <name> equal to <value>\n"
	                                    "set <name>\n"          "    show value of <name>\n"
	                                    "set\n"                 "    show value of all variables");

	atexit(cvar_free);
}


cvar_t *cvar_find(const char *cvar_name)
{
	int i;

	i = hashtable_search( &cvar_hash, cvar_name );

	if ( i < 0 )
		return NULL;

	return cvar_list[i];
}


const char *cvar_complete(char *text)
{
	int i;
	size_t len = strlen(text);

	if (!len)
		return NULL;

	for (i = 0; i < Num_cvars; i++)
		if (!strnicmp(text, cvar_list[i]->name, len))
			return cvar_list[i]->name;

	return NULL;
}


/* Register a cvar */
void cvar_registervariable(cvar_t *cvar)
{
	char *stringval;

	Assert(cvar != NULL);

	stringval = cvar->string;

	cvar->string = d_strdup(stringval);
	cvar->value = fl2f(strtod(cvar->string, NULL));
	cvar->intval = (int)strtol(cvar->string, NULL, 10);

	if (cvar_find(cvar->name))
	{
		Int3();
		con_printf(CON_URGENT, "cvar %s already exists!\n", cvar->name);
		return;
	}

	/* insert at end of list */
	hashtable_insert(&cvar_hash, cvar->name, Num_cvars);
	cvar_list[Num_cvars++] = cvar;
}


/* Set a CVar's value */
void cvar_set_cvar(cvar_t *cvar, char *value)
{
	if (!cvar)
		return;

	d_free(cvar->string);
	cvar->string = d_strdup(value);
	cvar->value = fl2f(strtod(cvar->string, NULL));
	cvar->intval = (int)strtol(cvar->string, NULL, 10);
	con_printf(CON_VERBOSE, "%s: %s\n", cvar->name, cvar->string);
}


void cvar_set_cvarf(cvar_t *cvar, const char *fmt, ...)
{
	va_list arglist;
	char buf[CVAR_MAX_LENGTH];
	int n;

	va_start (arglist, fmt);
	n = vsnprintf(buf, sizeof(buf), fmt, arglist);
	va_end (arglist);

	Assert(!(n < 0 || n > CVAR_MAX_LENGTH));

	cvar_set_cvar(cvar, buf);
}


void cvar_set(const char *cvar_name, char *value)
{
	cvar_t *cvar;
	extern cvar_t Cheats_enabled;

	cvar = cvar_find(cvar_name);
	if (!cvar) {
		Int3();
		con_printf(CON_NORMAL, "cvar %s not found\n", cvar_name);
		return;
	}

	if (cvar->flags & CVAR_CHEAT && !Cheats_enabled.intval)
	{
		con_printf(CON_NORMAL, "cvar %s is cheat protected.\n", cvar_name);
		return;
	}

	cvar_set_cvar(cvar, value);
}


/* Write archive cvars to file */
void cvar_write(CFILE *file)
{
	int i;

	for (i = 0; i < Num_cvars; i++)
		if (cvar_list[i]->flags & CVAR_ARCHIVE)
			PHYSFSX_printf(file, "%s=%s\n", cvar_list[i]->name, cvar_list[i]->string);
}
