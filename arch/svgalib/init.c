/*
 *
 * SVGALib initialization
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include "args.h"

extern void d_mouse_init();

void arch_svgalib_init()
{
 if (!FindArg("-nomouse"))
	d_mouse_init();
}
