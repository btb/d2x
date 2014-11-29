/* Console */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_ 1

#include "pstypes.h"

/* Priority levels */
#define CON_CRITICAL -2
#define CON_URGENT   -1
#define CON_NORMAL    0
#define CON_VERBOSE   1
#define CON_DEBUG     2

int  con_init(void);
void con_init_gfx(void);
void con_resize(void);
void con_printf(int level, char *fmt, ...);

void con_show(void);
void con_draw(void);
void con_update(void);
int  con_events(int key);

/* CVar stuff */
typedef struct cvar_s
{
	char *name;
	char *string;
	dboolean archive;
	float value;
	struct cvar_s *next;
} cvar_t;

extern cvar_t *cvar_vars;

/* Register a CVar with the name and string and optionally archive elements set */
void cvar_registervariable (cvar_t *cvar);

/* Equivalent to typing <var_name> <value> at the console */
void cvar_set(char *cvar_name, char *value);

/* Get a CVar's value */
float cvar(char *cvar_name);

/* Console CVars */
/* How discriminating we are about which messages are displayed */
extern cvar_t con_threshold;

#endif /* _CONSOLE_H_ */

