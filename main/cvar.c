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


#define cvar_round(x) ((x)>=0?(int)((x)+0.5):(int)((x)-0.5))

/* Register a cvar */
void cvar_registervariable (cvar_t *cvar)
{
	cvar_t *ptr;
	char *stringval;

	if (!cvar_initialized)
		cvar_init();

	Assert(cvar != NULL);

	stringval = cvar->string;

	cvar->next = NULL;
	cvar->string = d_strdup(stringval);
	cvar->value = strtod(cvar->string, (char **) NULL);
	cvar->intval = cvar_round(cvar->value);

	if (cvar_list == NULL)
	{
		cvar_list = cvar;
	} else
	{
		for (ptr = cvar_list; ptr->next != NULL; ptr = ptr->next) ;
		ptr->next = cvar;
	}
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
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (!stricmp(cvar_name, ptr->name)) break;

	if (ptr == NULL) return; // If we didn't find the cvar, give up

	cvar_set_cvar(ptr, value);
}


void cvar_set_value(char *cvar_name, float value)
{
	char stringval[FLOAT_STRING_SIZE];

	snprintf(stringval, FLOAT_STRING_SIZE, "%f", value);

	cvar_set(cvar_name, stringval);
}


/* Get a CVar's value */
float cvar (char *cvar_name)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (!strcmp(cvar_name, ptr->name)) break;

	if (ptr == NULL) return 0.0; // If we didn't find the cvar, give up

	return ptr->value;
}