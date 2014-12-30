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


#define CVAR_MAX_LENGTH 1024


/* The list of cvars */
cvar_t *cvar_list = NULL;

int cvar_initialized = 0;


void cvar_free(void)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		d_free(ptr->string);
}


void cvar_cmd_set(int argc, char **argv)
{
	char buf[CVAR_MAX_LENGTH];
	int ret, i;

	if (argc < 3)
		return;

	ret = snprintf(buf, CVAR_MAX_LENGTH, "%s", argv[2]);
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
	cmd_addcommand("set", cvar_cmd_set);

	atexit(cvar_free);
	cvar_initialized = 1;
}


cvar_t *cvar_find(char *cvar_name)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (!stricmp(cvar_name, ptr->name))
			return ptr;

	return NULL;
}


char *cvar_complete(char *text)
{
	cvar_t *ptr;
	int len = strlen(text);

	if (!len)
		return NULL;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (!strnicmp(text, ptr->name, len))
			return ptr->name;

	return NULL;
}


#define cvar_round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

/* Register a cvar */
void cvar_registervariable (cvar_t *cvar)
{
	char *stringval;
	cvar_t *ptr;

	if (!cvar_initialized)
		cvar_init();

	Assert(cvar != NULL);

	stringval = cvar->string;

	cvar->string = d_strdup(stringval);
	cvar->value = strtod(cvar->string, (char **) NULL);
	cvar->intval = cvar_round(cvar->value);
	cvar->next = NULL;

	if (cvar_list == NULL) {
		cvar_list = cvar;
		return;
	}

	/* insert at end of list */
	for (ptr = cvar_list; ptr->next != NULL; ptr = ptr->next)
		Assert(stricmp(cvar->name, ptr->name));
	ptr->next = cvar;
}


/* Set a CVar's value */
void cvar_set_cvar(cvar_t *cvar, char *value)
{
	if (!cvar)
		return;

	d_free(cvar->string);
	cvar->string = d_strdup(value);
	cvar->value = strtod(cvar->string, (char **) NULL);
	cvar->intval = cvar_round(cvar->value);
}


void cvar_set_cvarf(cvar_t *cvar, char *fmt, ...)
{
	va_list arglist;
	char buf[CVAR_MAX_LENGTH];
	int n;

	va_start (arglist, fmt);
	n = vsnprintf (buf, CVAR_MAX_LENGTH, fmt, arglist);
	va_end (arglist);

	Assert(!(n < 0 || n > CVAR_MAX_LENGTH));

	cvar_set_cvar(cvar, buf);
}


void cvar_set (char *cvar_name, char *value)
{
	cvar_set_cvar(cvar_find(cvar_name), value);
}


/* Get a CVar's value */
float cvar (char *cvar_name)
{
	cvar_t *cvar;

	cvar = cvar_find(cvar_name);

	if (!cvar)
		return 0.0; // If we didn't find the cvar, give up

	return cvar->value;
}


/* Write archive cvars to file */
void cvar_write(CFILE *file)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (ptr->archive)
			PHYSFSX_printf(file, "%s=%s\n", ptr->name, ptr->string);
}
