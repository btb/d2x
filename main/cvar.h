/* Console variables */

#ifndef _CVAR_H
#define _CVAR_H 1

#include "pstypes.h"


typedef struct cvar_s
{
	char *name;
	char *string;
	dboolean archive;
	float value;
	struct cvar_s *next;
} cvar_t;


/* Register a CVar with the name and string and optionally archive elements set */
void cvar_registervariable (cvar_t *cvar);

/* Equivalent to typing <var_name> <value> at the console */
void cvar_set(char *cvar_name, char *value);

/* Get a CVar's value */
float cvar(char *cvar_name);


#endif /* _CVAR_H_ */
