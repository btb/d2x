/* Console variables */


#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <float.h>

#include "cvar.h"
#include "error.h"
#include "strutil.h"
#include "u_mem.h"


#define FLOAT_STRING_SIZE (3 + DBL_MANT_DIG - DBL_MIN_EXP + 1)


/* The list of cvars */
cvar_t *cvar_list = NULL;

int cvar_initialized = 0;


void cvar_free(void)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		d_free(ptr->string);
}


void cvar_init(void)
{
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
	d_free(cvar->string);
	cvar->string = d_strdup(value);
	cvar->value = strtod(cvar->string, (char **) NULL);
	cvar->intval = cvar_round(cvar->value);
}


void cvar_set_cvar_value(cvar_t *cvar, float value)
{
	char stringval[FLOAT_STRING_SIZE];

	snprintf(stringval, FLOAT_STRING_SIZE, "%f", value);

	cvar_set_cvar(cvar, stringval);
}


void cvar_set (char *cvar_name, char *value)
{
	cvar_set_cvar(cvar_find(cvar_name), value);
}


void cvar_set_value(char *cvar_name, float value)
{
	char stringval[FLOAT_STRING_SIZE];

	snprintf(stringval, FLOAT_STRING_SIZE, "%f", value);

	cvar_set_cvar(cvar_find(cvar_name), stringval);
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
