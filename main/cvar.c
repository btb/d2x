/* Console variables */


#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>

#include "cvar.h"
#include "error.h"
#include "strutil.h"


/* The list of cvars */
cvar_t *cvar_list = NULL;


/* Register a cvar */
void cvar_registervariable (cvar_t *cvar)
{
	cvar_t *ptr;

	Assert(cvar != NULL);

	cvar->next = NULL;
	cvar->value = strtod(cvar->string, (char **) NULL);

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
void cvar_set (char *cvar_name, char *value)
{
	cvar_t *ptr;

	for (ptr = cvar_list; ptr != NULL; ptr = ptr->next)
		if (!stricmp(cvar_name, ptr->name)) break;

	if (ptr == NULL) return; // If we didn't find the cvar, give up

	ptr->value = strtod(value, (char **) NULL);
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
