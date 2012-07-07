/*
 *
 * linux init.c - added Matt Mueller 9/6/98
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "pstypes.h"
#include "console.h"
#include "event.h"
#include "dxxerror.h"
#include "joy.h"
#include "args.h"

extern void arch_sdl_init(void);
extern void arch_svgalib_init(void);
extern void key_init(void);
extern int com_init(void);
extern void timer_init(void);

void arch_init_start()
{

}

void arch_init()
{
 // Initialise the library
	arch_sdl_init();
#ifdef SVGALIB_INPUT
	arch_svgalib_init();
#endif
	//added 06/09/99 Matt Mueller - fix nonetwork compile
#ifdef NETWORK
	//end addition -MM
//added on 10/19/98 by Victor Rachels to add serial support (from DPH)
    if(!(FindArg("-noserial")))
     com_init();
//end this section addition - Victor 
	//added 06/09/99 Matt Mueller - fix nonetwork compile
#endif
	//end addition -MM
    key_init();
}
