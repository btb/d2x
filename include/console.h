/* Console */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_ 1

#include "cmd.h"
#include "cvar.h"

/* Priority levels */
#define CON_CRITICAL -2
#define CON_URGENT   -1
#define CON_NORMAL    0
#define CON_VERBOSE   1
#define CON_DEBUG     2

void con_init(void);
void con_init_gfx(void);
void con_resize(void);
void con_printf(int level, char *fmt, ...);

void con_show(void);
void con_draw(void);
void con_update(void);
int  con_events(int key);

extern int Console_open;

/* Console CVars */
/* How discriminating we are about which messages are displayed */
extern cvar_t con_threshold;


#endif /* _CONSOLE_H_ */
