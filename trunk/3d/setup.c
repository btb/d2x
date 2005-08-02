/* $Id: setup.c,v 1.7 2005-08-02 06:14:48 chris Exp $ */
/*
THE COMPUTER CODE CONTAINED HEREIN IS THE SOLE PROPERTY OF PARALLAX
SOFTWARE CORPORATION ("PARALLAX").  PARALLAX, IN DISTRIBUTING THE CODE TO
END-USERS, AND SUBJECT TO ALL OF THE TERMS AND CONDITIONS HEREIN, GRANTS A
ROYALTY-FREE, PERPETUAL LICENSE TO SUCH END-USERS FOR USE BY SUCH END-USERS
IN USING, DISPLAYING,  AND CREATING DERIVATIVE WORKS THEREOF, SO LONG AS
SUCH USE, DISPLAY OR CREATION IS FOR NON-COMMERCIAL, ROYALTY OR REVENUE
FREE PURPOSES.  IN NO EVENT SHALL THE END-USER USE THE COMPUTER CODE
CONTAINED HEREIN FOR REVENUE-BEARING PURPOSES.  THE END-USER UNDERSTANDS
AND AGREES TO THE TERMS HEREIN AND ACCEPTS THE SAME BY USE OF THIS FILE.  
COPYRIGHT 1993-1998 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/
/*
 * 
 * Setup for 3d library
 * 
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifdef RCS
static char rcsid[] = "$Id: setup.c,v 1.7 2005-08-02 06:14:48 chris Exp $";
#endif

#include <stdlib.h>

#include "error.h"

#include "3d.h"
#include "globvars.h"
#include "clipper.h"
//#include "div0.h"

#ifdef OGL
#include "ogl_init.h"
#else
#include "texmap.h"  // for init_interface_vars_to_assembler()
#endif

//initialize the 3d system
void g3_init(void)
{
//	div0_init(DM_ERROR);
	atexit(g3_close);
}

//close down the 3d system
void g3_close(void) {}

//start the frame
void g3_start_frame(void)
{
	fix s;

	//set int w,h & fixed-point w,h/2
	Canv_w2 = (Canvas_width  = grd_curcanv->cv_bitmap.bm_w)<<15;
	Canv_h2 = (Canvas_height = grd_curcanv->cv_bitmap.bm_h)<<15;
#ifdef __powerc
	fCanv_w2 = f2fl((Canvas_width  = grd_curcanv->cv_bitmap.bm_w)<<15);
	fCanv_h2 = f2fl((Canvas_height = grd_curcanv->cv_bitmap.bm_h)<<15);
#endif

	//compute aspect ratio for this canvas

	s = fixmuldiv(grd_curscreen->sc_aspect,Canvas_height,Canvas_width);

	if (s <= f1_0) {	   //scale x
		Window_scale.x = s;
		Window_scale.y = f1_0;
	}
	else {
		Window_scale.y = fixdiv(f1_0,s);
		Window_scale.x = f1_0;
	}
	
	Window_scale.z = f1_0;		//always 1

	init_free_points();

#ifdef OGL
	ogl_start_frame();
#else
	init_interface_vars_to_assembler();		//for the texture-mapper
#endif
}

//this doesn't do anything, but is here for completeness
void g3_end_frame(void)
{
#ifdef OGL
	ogl_end_frame();
#endif

//	Assert(free_point_num==0);
	free_point_num = 0;

}


