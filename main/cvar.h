/* Console variables */

#ifndef _CVAR_H
#define _CVAR_H 1

#include "pstypes.h"
#include "cfile.h"


typedef struct cvar_s
{
	char *name;
	char *string;
	bool archive;
	fix value;
	int intval;
	struct cvar_s *next;
} cvar_t;


/* Register a CVar with the name and string and optionally archive elements set */
void cvar_registervariable (cvar_t *cvar);

/* Set a CVar's value */
void cvar_set_cvar(cvar_t *cvar, char *value);
void cvar_set_cvarf(cvar_t *cvar, char *fmt, ...);
#define cvar_setint(cvar, x) cvar_set_cvarf((cvar), "%d", (x))
#define cvar_setfl(cvar, x) cvar_set_cvarf((cvar), "%f", (x))
#define cvar_toggle(cvar) cvar_setint((cvar), !(cvar)->intval)

/* Equivalent to typing <var_name> <value> at the console */
void cvar_set(char *cvar_name, char *value);

/* Get the pointer to a cvar by name */
cvar_t *cvar_find(char *cvar_name);

/* Try to autocomplete a cvar name */
char *cvar_complete(char *text);

/* Get a CVar's value */
fix cvar(char *cvar_name);

/* Write archive cvars to file */
void cvar_write(CFILE *file);


#endif /* _CVAR_H_ */
