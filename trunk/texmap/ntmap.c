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
 * $Source: /cvs/cvsroot/d2x/texmap/ntmap.c,v $
 * $Revision: 1.6 $
 * $Author: bradleyb $
 * $Date: 2002-02-23 22:24:10 $
 * 
 * Start of conversion to new texture mapper.
 * 
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2001/10/25 09:12:16  bradleyb
 * Completed tmap selection code.
 *
 * Revision 1.4  2001/10/25 02:22:46  bradleyb
 * adding support for runtime selection of tmap funcs
 *
 * Revision 1.3  2001/01/31 15:18:04  bradleyb
 * Makefile and conf.h fixes
 *
 * Revision 1.2  2001/01/31 14:04:46  bradleyb
 * Fix compiler warnings
 *
 * Revision 1.1.1.1  2001/01/19 03:30:16  bradleyb
 * Import of d2x-0.0.8
 *
 * Revision 1.4  1999/10/18 00:31:55  donut
 * allow FP_TMAP to be used without NO_ASM
 *
 * Revision 1.3  1999/08/05 22:53:41  sekmu
 *
 * D3D patch(es) from ADB
 *
 * Revision 1.2  1999/07/07 21:31:09  donut
 * removed unused vars from ntmap_scanline_lighted and slightly optomized it
 *
 * Revision 1.1.1.1  1999/06/14 22:14:06  donut
 * Import of d1x 1.37 source.
 *
 * Revision 1.52  1995/03/14  15:13:06  john
 * Increased MAX_Y_Pointers to 480.
 * 
 * Revision 1.51  1995/02/23  14:25:09  john
 * Added editor tmap.
 * 
 * Revision 1.50  1995/02/20  18:22:58  john
 * Put all the externs in the assembly modules into tmap_inc.asm.
 * Also, moved all the C versions of the inner loops into a new module, 
 * scanline.c.
 * 
 * Revision 1.49  1995/02/20  17:09:11  john
 * Added code so that you can build the tmapper with no assembly!
 * 
 * Revision 1.48  1995/01/06  11:11:30  mike
 * even when not in editor, have 400 lines in texture map scanline table.
 * 
 * Revision 1.47  1994/12/15  16:43:25  matt
 * Took out code only needed by editor
 * 
 * Revision 1.46  1994/12/09  22:35:37  mike
 * fix bug in before call to asm_tmap_scanline_per causing write of pixel onto past right border onto left.
 * 
 * Revision 1.45  1994/12/06  16:31:06  mike
 * fix bug in asm_tmap_scanline_matt interface.
 * 
 * Revision 1.44  1994/12/04  20:37:18  mike
 * *** empty log message ***
 * 
 * Revision 1.43  1994/12/02  23:30:04  mike
 * optimizations.
 * 
 * Revision 1.42  1994/11/30  00:57:43  mike
 * optimizations.
 * 
 * Revision 1.41  1994/11/28  13:34:27  mike
 * optimizations.
 * 
 * Revision 1.40  1994/11/28  01:30:01  mike
 * kill warning.
 * 
 * Revision 1.39  1994/11/28  01:28:59  mike
 * optimizations.
 * 
 * Revision 1.38  1994/11/21  14:08:07  john
 * Took out all multiple instead of divide code.
 * 
 * Revision 1.37  1994/11/19  15:21:52  mike
 * rip out unused code.
 * 
 * Revision 1.36  1994/11/14  11:42:51  mike
 * optimization.
 * 
 * Revision 1.35  1994/11/12  16:41:36  mike
 * *** empty log message ***
 * 
 * Revision 1.34  1994/11/10  21:28:41  mike
 * remove call to init_interface_vars_to_assembler.
 * 
 * Revision 1.33  1994/11/10  11:08:59  mike
 * detail level stuff.
 * 
 * Revision 1.32  1994/11/09  22:55:52  matt
 * Added variable Current_seg_depth for detail level optimization
 * 
 * Revision 1.31  1994/11/09  19:57:31  john
 * Added texture rle caching.
 * 
 * Revision 1.30  1994/11/09  19:54:48  mike
 * Call flat shader if Tmap_flat_flag set.
 * 
 * Revision 1.29  1994/11/02  21:33:31  john
 * Added Burger Bill's optimization, ie.. 2 muls per 8 pixels.
 * 
 * Revision 1.28  1994/11/02  11:32:16  john
 * Added code for c callable inner loop and code to 
 * test dividing out z0.
 * 
 * Revision 1.27  1994/10/28  20:54:32  matt
 * Added error checking
 * 
 * Revision 1.26  1994/10/25  11:20:20  mike
 * fix bug in lighting overflow checking for one scanline tall linear texture maps.
 * 
 * Revision 1.25  1994/08/03  15:40:33  mike
 * Prevent divide overflows, decrease occurrence of precision-caused glitches.
 * 
 * Revision 1.24  1994/07/27  09:31:16  mike
 * Fix concave texture map problem, decrease occurrence of unimportant int 3.
 * 
 * Revision 1.23  1994/06/17  12:23:31  mike
 * Support non-lighted texture maps.
 * 
 * Revision 1.22  1994/06/11  08:10:24  mike
 * Fix mysterious hang bug, lighting value was out of range.
 * 
 * Revision 1.21  1994/06/09  16:10:16  mike
 * Change SC2000 from constant to variable.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifdef RCS
static char rcsid[] = "$Id: ntmap.c,v 1.6 2002-02-23 22:24:10 bradleyb Exp $";
#endif

#define VESA 0
#define NUM_TMAPS 16

#define HEADLIGHT_LIGHTING 0

#define WIREFRAME 0
#define PERSPECTIVE 1

#include "pstypes.h"
#include "fix.h"
#include "vecmat.h"
#include "gr.h"
#include "3d.h"
#include "error.h"

#include "texmap.h"
#include "texmapl.h"
#include "rle.h"
#include "scanline.h"

//#include "../main/textures.h"

#ifdef EDITOR
#define EDITOR_TMAP 1       //if in, include extra stuff
#endif

#define F15_5 (F1_0*15 + F0_5)

// Temporary texture map, interface from Matt's 3d system to Mike's texture mapper.
g3ds_tmap Tmap1;

grs_bitmap Texmap_ptrs[NUM_TMAPS];
grs_bitmap Texmap4_ptrs[NUM_TMAPS];

fix Range_max=0; // debug, kill me

int	Interpolation_method=0;	// 0 = choose best method
int     Lighting_on=1;                  // initialize to no lighting
int	Tmap_flat_flag = 0;		//	1 = render texture maps as flat shaded polygons.
int	Current_seg_depth;		// HACK INTERFACE: how far away the current segment (& thus texture) is
int	Max_perspective_depth;
int	Max_linear_depth;
int	Max_flat_depth;

extern int Window_clip_left, Window_clip_bot, Window_clip_right, Window_clip_top;

// These variables are the interface to assembler.  They get set for each texture map, which is a real waste of time.
//	They should be set only when they change, which is generally when the window bounds change.  And, even still, it's
//	a pretty bad interface.
int	bytes_per_row=-1;
unsigned char *write_buffer;
int  	window_left;
int	window_right;
int	window_top;
int	window_bottom;
int  	window_width;
int  	window_height;

#define MAX_Y_POINTERS  1024

int	y_pointers[MAX_Y_POINTERS];

fix fix_recip[FIX_RECIP_TABLE_SIZE];

int	Lighting_enabled;
int	Fix_recip_table_computed=0;

fix fx_l, fx_u, fx_v, fx_z, fx_du_dx, fx_dv_dx, fx_dz_dx, fx_dl_dx;
int fx_xleft, fx_xright, fx_y;
unsigned char * pixptr;
int per2_flag = 0;
int Transparency_on = 0;
int dither_intensity_lighting = 0;

ubyte * tmap_flat_cthru_table;
ubyte tmap_flat_color;
ubyte tmap_flat_shade_value;



// -------------------------------------------------------------------------------------
void init_fix_recip_table(void)
{
	int	i;

	fix_recip[0] = F1_0;

	for (i=1; i<FIX_RECIP_TABLE_SIZE; i++)
		fix_recip[i] = F1_0/i;

	Fix_recip_table_computed = 1;
}

// -------------------------------------------------------------------------------------
//	Initialize interface variables to assembler.
//	These things used to be constants.  This routine is now (10/6/93) getting called for
//	every texture map.  It should get called whenever the window changes, or, preferably,
//	not at all.  I'm pretty sure these variables are only being used for range checking.
void init_interface_vars_to_assembler(void)
{
	grs_bitmap	*bp;

	bp = &grd_curcanv->cv_bitmap;

	Assert(bp!=NULL);
	Assert(bp->bm_data!=NULL);
	Assert(bp->bm_h <= MAX_Y_POINTERS);

	//	If bytes_per_row has changed, create new table of pointers.
	if (bytes_per_row != (int) bp->bm_rowsize) {
		int	y_val, i;

		bytes_per_row = (int) bp->bm_rowsize;

		y_val = 0;
		for (i=0; i<MAX_Y_POINTERS; i++) {
			y_pointers[i] = y_val;
			y_val += bytes_per_row;
		}
	}

        write_buffer = (unsigned char *) bp->bm_data;

	window_left = 0;
	window_right = (int) bp->bm_w-1;
	window_top = 0;
	window_bottom = (int) bp->bm_h-1;

	Window_clip_left = window_left;
	Window_clip_right = window_right;
	Window_clip_top = window_top;
	Window_clip_bot = window_bottom;

	window_width = bp->bm_w;
	window_height = bp->bm_h;

	if (!Fix_recip_table_computed)
		init_fix_recip_table();
}

// -------------------------------------------------------------------------------------
//                             VARIABLES
extern g3ds_tmap Tmap1;

// -------------------------------------------------------------------------------------
//	Returns number preceding val modulo modulus.
//	prevmod(3,4) = 2
//	prevmod(0,4) = 3
int prevmod(int val,int modulus)
{
	if (val > 0)
		return val-1;
	else
		return modulus-1;
//	return (val + modulus - 1) % modulus;
}


//	Returns number succeeding val modulo modulus.
//	succmod(3,4) = 0
//	succmod(0,4) = 1
int succmod(int val,int modulus)
{
	if (val < modulus-1)
		return val+1;
	else
		return 0;

//	return (val + 1) % modulus;
}

// -------------------------------------------------------------------------------------
//	Select topmost vertex (minimum y coordinate) and bottommost (maximum y coordinate) in
//	texture map.  If either is part of a horizontal edge, then select leftmost vertex for
//	top, rightmost vertex for bottom.
//	Important: Vertex is selected with integer precision.  So, if there are vertices at
//	(0.0,0.7) and (0.5,0.3), the first vertex is selected, because they y coordinates are
//	considered the same, so the smaller x is favored.
//	Parameters:
//		nv		number of vertices
//		v3d	pointer to 3d vertices containing u,v,x2d,y2d coordinates
//	Results in:
//		*min_y_ind
//		*max_y_ind
// -------------------------------------------------------------------------------------
void compute_y_bounds(g3ds_tmap *t, int *vlt, int *vlb, int *vrt, int *vrb,int *bottom_y_ind)
{
	int	i;
	int	min_y,max_y;
	int	min_y_ind;
	int	original_vrt;
	fix	min_x;

	// Scan all vertices, set min_y_ind to vertex with smallest y coordinate.
	min_y = f2i(t->verts[0].y2d);
	max_y = min_y;
	min_y_ind = 0;
	min_x = f2i(t->verts[0].x2d);
	*bottom_y_ind = 0;

	for (i=1; i<t->nv; i++) {
		if (f2i(t->verts[i].y2d) < min_y) {
			min_y = f2i(t->verts[i].y2d);
			min_y_ind = i;
			min_x = f2i(t->verts[i].x2d);
		} else if (f2i(t->verts[i].y2d) == min_y) {
			if (f2i(t->verts[i].x2d) < min_x) {
				min_y_ind = i;
				min_x = f2i(t->verts[i].x2d);
			}
		}
		if (f2i(t->verts[i].y2d) > max_y) {
			max_y = f2i(t->verts[i].y2d);
			*bottom_y_ind = i;
		}
	}

//--removed mk, 11/27/94--	//	Check for a non-upright-hourglass polygon and fix, if necessary, by bashing a y coordinate.
//--removed mk, 11/27/94--	//	min_y_ind = index of minimum y coordinate, *bottom_y_ind = index of maximum y coordinate
//--removed mk, 11/27/94--{
//--removed mk, 11/27/94--	int	max_temp, min_temp;
//--removed mk, 11/27/94--
//--removed mk, 11/27/94--	max_temp = *bottom_y_ind;
//--removed mk, 11/27/94--	if (*bottom_y_ind < min_y_ind)
//--removed mk, 11/27/94--		max_temp += t->nv;
//--removed mk, 11/27/94--
//--removed mk, 11/27/94--	for (i=min_y_ind; i<max_temp; i++) {
//--removed mk, 11/27/94--		if (f2i(t->verts[i%t->nv].y2d) > f2i(t->verts[(i+1)%t->nv].y2d)) {
//--removed mk, 11/27/94--			Int3();
//--removed mk, 11/27/94--			t->verts[(i+1)%t->nv].y2d = t->verts[i%t->nv].y2d;
//--removed mk, 11/27/94--		}
//--removed mk, 11/27/94--	}
//--removed mk, 11/27/94--
//--removed mk, 11/27/94--	min_temp = min_y_ind;
//--removed mk, 11/27/94--	if (min_y_ind < *bottom_y_ind)
//--removed mk, 11/27/94--		min_temp += t->nv;
//--removed mk, 11/27/94--
//--removed mk, 11/27/94--	for (i=*bottom_y_ind; i<min_temp; i++) {
//--removed mk, 11/27/94--		if (f2i(t->verts[i%t->nv].y2d) < f2i(t->verts[(i+1)%t->nv].y2d)) {
//--removed mk, 11/27/94--			Int3();
//--removed mk, 11/27/94--			t->verts[(i+1)%t->nv].y2d = t->verts[i%t->nv].y2d;
//--removed mk, 11/27/94--		}
//--removed mk, 11/27/94--	}
//--removed mk, 11/27/94--}

	// Set "vertex left top", etc. based on vertex with topmost y coordinate
	*vlt = min_y_ind;
	*vrt = *vlt;
	*vlb = prevmod(*vlt,t->nv);
	*vrb = succmod(*vrt,t->nv);

	// If right edge is horizontal, then advance along polygon bound until it no longer is or until all
	// vertices have been examined.
	// (Left edge cannot be horizontal, because *vlt is set to leftmost point with highest y coordinate.)

	original_vrt = *vrt;

	while (f2i(t->verts[*vrt].y2d) == f2i(t->verts[*vrb].y2d)) {
		if (succmod(*vrt,t->nv) == original_vrt) {
			break;
		}
		*vrt = succmod(*vrt,t->nv);
		*vrb = succmod(*vrt,t->nv);
	}
}

// -------------------------------------------------------------------------------------
//	Returns dx/dy given two vertices.
//	If dy == 0, returns 0.0
// -------------------------------------------------------------------------------------
//--fix compute_dx_dy_lin(g3ds_tmap *t, int top_vertex,int bottom_vertex)
//--{
//--	int	dy;
//--
//--	// compute delta x with respect to y for any edge
//--	dy = f2i(t->verts[bottom_vertex].y2d - t->verts[top_vertex].y2d) + 1;
//--	if (dy)
//--		return (t->verts[bottom_vertex].x2d - t->verts[top_vertex].x2d) / dy;
//--	else
//--		return 0;
//--
//--}

fix compute_du_dy_lin(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(t->verts[bottom_vertex].u - t->verts[top_vertex].u, recip_dy);
}


fix compute_dv_dy_lin(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(t->verts[bottom_vertex].v - t->verts[top_vertex].v, recip_dy);
}

fix compute_dl_dy_lin(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(t->verts[bottom_vertex].l - t->verts[top_vertex].l, recip_dy);

}

fix compute_dx_dy(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(t->verts[bottom_vertex].x2d - t->verts[top_vertex].x2d, recip_dy);
}

fix compute_du_dy(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(fixmul(t->verts[bottom_vertex].u,t->verts[bottom_vertex].z) - fixmul(t->verts[top_vertex].u,t->verts[top_vertex].z), recip_dy);
}


fix compute_dv_dy(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(fixmul(t->verts[bottom_vertex].v,t->verts[bottom_vertex].z) - fixmul(t->verts[top_vertex].v,t->verts[top_vertex].z), recip_dy);

}

fix compute_dz_dy(g3ds_tmap *t, int top_vertex,int bottom_vertex, fix recip_dy)
{
	return fixmul(t->verts[bottom_vertex].z - t->verts[top_vertex].z, recip_dy);

}
int Skip_short_flag=0;

// -------------------------------------------------------------------------------------
//	Texture map current scanline in perspective.
// -------------------------------------------------------------------------------------
void ntmap_scanline_lighted(grs_bitmap *srcb, int y, fix xleft, fix xright, fix uleft, fix uright, fix vleft, fix vright, fix zleft, fix zright, fix lleft, fix lright)
{
	fix	dx,recip_dx;

	fx_xright = f2i(xright);
	//edited 06/27/99 Matt Mueller - moved these tests up from within the switch so as not to do a bunch of needless calculations when we are just gonna return anyway.  Slight fps boost?
	if (fx_xright < Window_clip_left)
		return;
	fx_xleft = f2i(xleft);
	if (fx_xleft > Window_clip_right)
		return;
	//end edit -MM

	dx = fx_xright - fx_xleft;
	if ((dx < 0) || (xright < 0) || (xleft > xright))		// the (xleft > xright) term is not redundant with (dx < 0) because dx is computed using integers
		return;

	// setup to call assembler scanline renderer
	if (dx < FIX_RECIP_TABLE_SIZE)
		recip_dx = fix_recip[dx];
	else
		recip_dx = F1_0/dx;

	fx_u = uleft;
	fx_v = vleft;
	fx_z = zleft;
	
	fx_du_dx = fixmul(uright - uleft,recip_dx);
	fx_dv_dx = fixmul(vright - vleft,recip_dx);
	fx_dz_dx = fixmul(zright - zleft,recip_dx);
	fx_y = y;
	pixptr = srcb->bm_data;

	switch (Lighting_enabled) {
		case 0:
			//added 05/17/99 Matt Mueller - prevent writing before the buffer
            if ((fx_y == 0) && (fx_xleft < 0))
				fx_xleft = 0;
			//end addition -MM
			if (fx_xright > Window_clip_right)
				fx_xright = Window_clip_right;
			
			cur_tmap_scanline_per();
			break;
		case 1: {
			fix	mul_thing;

			if (lleft < 0) lleft = 0;
			if (lright < 0) lright = 0;
			if (lleft > (NUM_LIGHTING_LEVELS*F1_0-F1_0/2)) lleft = (NUM_LIGHTING_LEVELS*F1_0-F1_0/2);
			if (lright > (NUM_LIGHTING_LEVELS*F1_0-F1_0/2)) lright = (NUM_LIGHTING_LEVELS*F1_0-F1_0/2);

			fx_l = lleft;
			fx_dl_dx = fixmul(lright - lleft,recip_dx);

			//	This is a pretty ugly hack to prevent lighting overflows.
			mul_thing = dx * fx_dl_dx;
			if (lleft + mul_thing < 0)
				fx_dl_dx += 12;
			else if (lleft + mul_thing > (NUM_LIGHTING_LEVELS*F1_0-F1_0/2))
				fx_dl_dx -= 12;

			//added 05/17/99 Matt Mueller - prevent writing before the buffer
            if ((fx_y == 0) && (fx_xleft < 0))
				fx_xleft = 0;
			//end addition -MM
			if (fx_xright > Window_clip_right)
				fx_xright = Window_clip_right;

			cur_tmap_scanline_per();
			break;
		}
		case 2:
#ifdef EDITOR_TMAP
			fx_xright = f2i(xright);
			fx_xleft = f2i(xleft);

			tmap_flat_color = 1;
			cur_tmap_scanline_flat();
#else
			Int3();	//	Illegal, called an editor only routine!
#endif
			break;
	}

}

int Do_vertical_scan=0;

int	Break_on_flat=0;

// -------------------------------------------------------------------------------------
//	Render a texture map with lighting using perspective interpolation in inner and outer loops.
// -------------------------------------------------------------------------------------
void ntexture_map_lighted(grs_bitmap *srcb, g3ds_tmap *t)
{
	int	vlt,vrt,vlb,vrb;	// vertex left top, vertex right top, vertex left bottom, vertex right bottom
	int	topy,boty,y, dy;
	fix	dx_dy_left,dx_dy_right;
	fix	du_dy_left,du_dy_right;
	fix	dv_dy_left,dv_dy_right;
	fix	dz_dy_left,dz_dy_right;
	fix	dl_dy_left,dl_dy_right;
	fix	recip_dyl, recip_dyr;
	int	max_y_vertex;
	fix	xleft,xright,uleft,vleft,uright,vright,zleft,zright,lleft,lright;
	int	next_break_left, next_break_right;

     	g3ds_vertex *v3d;

        //remove stupid warnings in compile
        dl_dy_left = F1_0;
        dl_dy_right = F1_0;
        lleft = F1_0;
        lright = F1_0;

	v3d = t->verts;

	// Determine top and bottom y coords.
	compute_y_bounds(t,&vlt,&vlb,&vrt,&vrb,&max_y_vertex);

	// Set top and bottom (of entire texture map) y coordinates.
	topy = f2i(v3d[vlt].y2d);
	boty = f2i(v3d[max_y_vertex].y2d);
	if (topy > Window_clip_bot)
		return;
	if (boty > Window_clip_bot)
		boty = Window_clip_bot;

	// Set amount to change x coordinate for each advance to next scanline.
	dy = f2i(t->verts[vlb].y2d) - f2i(t->verts[vlt].y2d);
	if (dy < FIX_RECIP_TABLE_SIZE)
		recip_dyl = fix_recip[dy];
	else
		recip_dyl = F1_0/dy;

	dx_dy_left = compute_dx_dy(t,vlt,vlb, recip_dyl);
	du_dy_left = compute_du_dy(t,vlt,vlb, recip_dyl);
	dv_dy_left = compute_dv_dy(t,vlt,vlb, recip_dyl);
	dz_dy_left = compute_dz_dy(t,vlt,vlb, recip_dyl);

	dy = f2i(t->verts[vrb].y2d) - f2i(t->verts[vrt].y2d);
	if (dy < FIX_RECIP_TABLE_SIZE)
		recip_dyr = fix_recip[dy];
	else
		recip_dyr = F1_0/dy;

	du_dy_right = compute_du_dy(t,vrt,vrb, recip_dyr);
	dx_dy_right = compute_dx_dy(t,vrt,vrb, recip_dyr);
	dv_dy_right = compute_dv_dy(t,vrt,vrb, recip_dyr);
	dz_dy_right = compute_dz_dy(t,vrt,vrb, recip_dyr);

	if (Lighting_enabled) {
		dl_dy_left = compute_dl_dy_lin(t,vlt,vlb, recip_dyl);
		dl_dy_right = compute_dl_dy_lin(t,vrt,vrb, recip_dyr);

		lleft = v3d[vlt].l;
		lright = v3d[vrt].l;
	}

 	// Set initial values for x, u, v
	xleft = v3d[vlt].x2d;
	xright = v3d[vrt].x2d;

	zleft = v3d[vlt].z;
	zright = v3d[vrt].z;

	uleft = fixmul(v3d[vlt].u,zleft);
	uright = fixmul(v3d[vrt].u,zright);
	vleft = fixmul(v3d[vlt].v,zleft);
	vright = fixmul(v3d[vrt].v,zright);

	// scan all rows in texture map from top through first break.
	next_break_left = f2i(v3d[vlb].y2d);
	next_break_right = f2i(v3d[vrb].y2d);

	for (y = topy; y < boty; y++) {

		// See if we have reached the end of the current left edge, and if so, set
		// new values for dx_dy and x,u,v
		if (y == next_break_left) {
			fix	recip_dy;

			// Handle problem of double points.  Search until y coord is different.  Cannot get
			// hung in an infinite loop because we know there is a vertex with a lower y coordinate
			// because in the for loop, we don't scan all spanlines.
			while (y == f2i(v3d[vlb].y2d)) {
				vlt = vlb;
				vlb = prevmod(vlb,t->nv);
			}
			next_break_left = f2i(v3d[vlb].y2d);

			dy = f2i(t->verts[vlb].y2d) - f2i(t->verts[vlt].y2d);
			if (dy < FIX_RECIP_TABLE_SIZE)
				recip_dy = fix_recip[dy];
			else
				recip_dy = F1_0/dy;

			dx_dy_left = compute_dx_dy(t,vlt,vlb, recip_dy);

			xleft = v3d[vlt].x2d;
			zleft = v3d[vlt].z;
			uleft = fixmul(v3d[vlt].u,zleft);
			vleft = fixmul(v3d[vlt].v,zleft);
			lleft = v3d[vlt].l;

			du_dy_left = compute_du_dy(t,vlt,vlb, recip_dy);
			dv_dy_left = compute_dv_dy(t,vlt,vlb, recip_dy);
			dz_dy_left = compute_dz_dy(t,vlt,vlb, recip_dy);

			if (Lighting_enabled) {
				dl_dy_left = compute_dl_dy_lin(t,vlt,vlb, recip_dy);
				lleft = v3d[vlt].l;
			}
		}

		// See if we have reached the end of the current left edge, and if so, set
		// new values for dx_dy and x.  Not necessary to set new values for u,v.
		if (y == next_break_right) {
			fix	recip_dy;

			while (y == f2i(v3d[vrb].y2d)) {
				vrt = vrb;
				vrb = succmod(vrb,t->nv);
			}

			next_break_right = f2i(v3d[vrb].y2d);

			dy = f2i(t->verts[vrb].y2d) - f2i(t->verts[vrt].y2d);
			if (dy < FIX_RECIP_TABLE_SIZE)
				recip_dy = fix_recip[dy];
			else
				recip_dy = F1_0/dy;

			dx_dy_right = compute_dx_dy(t,vrt,vrb, recip_dy);

			xright = v3d[vrt].x2d;
			zright = v3d[vrt].z;
			uright = fixmul(v3d[vrt].u,zright);
			vright = fixmul(v3d[vrt].v,zright);

			du_dy_right = compute_du_dy(t,vrt,vrb, recip_dy);
			dv_dy_right = compute_dv_dy(t,vrt,vrb, recip_dy);
			dz_dy_right = compute_dz_dy(t,vrt,vrb, recip_dy);

			if (Lighting_enabled) {
				dl_dy_right = compute_dl_dy_lin(t,vrt,vrb, recip_dy);
				lright = v3d[vrt].l;
			}
		}

		if (Lighting_enabled) {
			if (y >= Window_clip_top)
				ntmap_scanline_lighted(srcb,y,xleft,xright,uleft,uright,vleft,vright,zleft,zright,lleft,lright);
			lleft += dl_dy_left;
			lright += dl_dy_right;
		} else
			if (y >= Window_clip_top)
				ntmap_scanline_lighted(srcb,y,xleft,xright,uleft,uright,vleft,vright,zleft,zright,lleft,lright);

		uleft += du_dy_left;
		vleft += dv_dy_left;

		uright += du_dy_right;
		vright += dv_dy_right;

		xleft += dx_dy_left;
		xright += dx_dy_right;

		zleft += dz_dy_left;
		zright += dz_dy_right;

	}

	// We can get lleft or lright out of bounds here because we compute dl_dy using fixed point values,
	//	but we plot an integer number of scanlines, therefore doing an integer number of additions of the delta.

//if (Break_on_flat)
//	mprintf(0, "[%i %i %i] ", y, f2i(xleft), f2i(xright));

	ntmap_scanline_lighted(srcb,y,xleft,xright,uleft,uright,vleft,vright,zleft,zright,lleft,lright);
}


// -------------------------------------------------------------------------------------
//	Texture map current scanline using linear interpolation.
// -------------------------------------------------------------------------------------
void ntmap_scanline_lighted_linear(grs_bitmap *srcb, int y, fix xleft, fix xright, fix uleft, fix uright, fix vleft, fix vright, fix lleft, fix lright)
{
	fix	u,v,l;
	fix	dx,recip_dx;

	fix	du_dx,dv_dx,dl_dx;

	u = uleft;
	v = vleft;
	l = lleft;

	dx = f2i(xright) - f2i(xleft);
	if ((dx < 0) || (xright < 0) || (xleft > xright))		// the (xleft > xright) term is not redundant with (dx < 0) because dx is computed using integers
		return;

		// setup to call assembler scanline renderer
		if (dx < FIX_RECIP_TABLE_SIZE)
			recip_dx = fix_recip[dx];
		else
			recip_dx = F1_0/dx;

		du_dx = fixmul(uright - uleft,recip_dx);
		dv_dx = fixmul(vright - vleft,recip_dx);

		fx_u = uleft;
		fx_v = vleft;
		fx_du_dx = du_dx;
		fx_dv_dx = dv_dx;
		fx_y = y;
		fx_xright = f2i(xright);
		fx_xleft = f2i(xleft);
		pixptr = srcb->bm_data;

		switch (Lighting_enabled) {
			case 0:
				//added 07/11/99 adb - prevent writing before the buffer
				if (fx_xleft < 0)
					fx_xleft = 0;
				//end addition -adb
				
				cur_tmap_scanline_lin_nolight();
				break;
			case 1:
				if (lleft < F1_0/2)
					lleft = F1_0/2;
				if (lright < F1_0/2)
					lright = F1_0/2;

				if (lleft > MAX_LIGHTING_VALUE*NUM_LIGHTING_LEVELS)
					lleft = MAX_LIGHTING_VALUE*NUM_LIGHTING_LEVELS;
				if (lright > MAX_LIGHTING_VALUE*NUM_LIGHTING_LEVELS)
					lright = MAX_LIGHTING_VALUE*NUM_LIGHTING_LEVELS;

				//added 07/11/99 adb - prevent writing before the buffer
				if (fx_xleft < 0)
					fx_xleft = 0;
				//end addition -adb

{
			fix mul_thing;

			fx_l = lleft;
			fx_dl_dx = fixmul(lright - lleft,recip_dx);

			//	This is a pretty ugly hack to prevent lighting overflows.
			mul_thing = dx * fx_dl_dx;
			if (lleft + mul_thing < 0)
				fx_dl_dx += 12;
			else if (lleft + mul_thing > (NUM_LIGHTING_LEVELS*F1_0-F1_0/2))
				fx_dl_dx -= 12;
}

				fx_l = lleft;
				dl_dx = fixmul(lright - lleft,recip_dx);
				fx_dl_dx = dl_dx;
				cur_tmap_scanline_lin();
				break;
			case 2:
#ifdef EDITOR_TMAP
				fx_xright = f2i(xright);
				fx_xleft = f2i(xleft);
				tmap_flat_color = 1;
				cur_tmap_scanline_flat();
#else
				Int3();	//	Illegal, called an editor only routine!
#endif
				break;
		}
}

// -------------------------------------------------------------------------------------
//	Render a texture map with lighting using perspective interpolation in inner and outer loops.
// -------------------------------------------------------------------------------------
void ntexture_map_lighted_linear(grs_bitmap *srcb, g3ds_tmap *t)
{
	int	vlt,vrt,vlb,vrb;	// vertex left top, vertex right top, vertex left bottom, vertex right bottom
	int	topy,boty,y, dy;
	fix	dx_dy_left,dx_dy_right;
	fix	du_dy_left,du_dy_right;
	fix	dv_dy_left,dv_dy_right;
	fix	dl_dy_left,dl_dy_right;
	int	max_y_vertex;
	fix	xleft,xright,uleft,vleft,uright,vright,lleft,lright;
	int	next_break_left, next_break_right;
	fix	recip_dyl, recip_dyr;

	g3ds_vertex *v3d;

        //remove stupid warnings in compile
        dl_dy_left = F1_0;
        dl_dy_right = F1_0;
        lleft = F1_0;
        lright = F1_0;

	v3d = t->verts;

	// Determine top and bottom y coords.
	compute_y_bounds(t,&vlt,&vlb,&vrt,&vrb,&max_y_vertex);

	// Set top and bottom (of entire texture map) y coordinates.
	topy = f2i(v3d[vlt].y2d);
	boty = f2i(v3d[max_y_vertex].y2d);

	if (topy > Window_clip_bot)
		return;
	if (boty > Window_clip_bot)
		boty = Window_clip_bot;

	dy = f2i(t->verts[vlb].y2d) - f2i(t->verts[vlt].y2d);
	if (dy < FIX_RECIP_TABLE_SIZE)
		recip_dyl = fix_recip[dy];
	else
		recip_dyl = F1_0/dy;

	dy = f2i(t->verts[vrb].y2d) - f2i(t->verts[vrt].y2d);
	if (dy < FIX_RECIP_TABLE_SIZE)
		recip_dyr = fix_recip[dy];
	else
		recip_dyr = F1_0/dy;

	// Set amount to change x coordinate for each advance to next scanline.
	dx_dy_left = compute_dx_dy(t,vlt,vlb, recip_dyl);
	dx_dy_right = compute_dx_dy(t,vrt,vrb, recip_dyr);

	du_dy_left = compute_du_dy_lin(t,vlt,vlb, recip_dyl);
	du_dy_right = compute_du_dy_lin(t,vrt,vrb, recip_dyr);

	dv_dy_left = compute_dv_dy_lin(t,vlt,vlb, recip_dyl);
	dv_dy_right = compute_dv_dy_lin(t,vrt,vrb, recip_dyr);

	if (Lighting_enabled) {
		dl_dy_left = compute_dl_dy_lin(t,vlt,vlb, recip_dyl);
		dl_dy_right = compute_dl_dy_lin(t,vrt,vrb, recip_dyr);

		lleft = v3d[vlt].l;
		lright = v3d[vrt].l;
	}

 	// Set initial values for x, u, v
	xleft = v3d[vlt].x2d;
	xright = v3d[vrt].x2d;

	uleft = v3d[vlt].u;
	uright = v3d[vrt].u;
	vleft = v3d[vlt].v;
	vright = v3d[vrt].v;

	// scan all rows in texture map from top through first break.
	next_break_left = f2i(v3d[vlb].y2d);
	next_break_right = f2i(v3d[vrb].y2d);

	for (y = topy; y < boty; y++) {

		// See if we have reached the end of the current left edge, and if so, set
		// new values for dx_dy and x,u,v
		if (y == next_break_left) {
			fix	recip_dy;

			// Handle problem of double points.  Search until y coord is different.  Cannot get
			// hung in an infinite loop because we know there is a vertex with a lower y coordinate
			// because in the for loop, we don't scan all spanlines.
			while (y == f2i(v3d[vlb].y2d)) {
				vlt = vlb;
				vlb = prevmod(vlb,t->nv);
			}
			next_break_left = f2i(v3d[vlb].y2d);

			dy = f2i(t->verts[vlb].y2d) - f2i(t->verts[vlt].y2d);
			if (dy < FIX_RECIP_TABLE_SIZE)
				recip_dy = fix_recip[dy];
			else
				recip_dy = F1_0/dy;

			dx_dy_left = compute_dx_dy(t,vlt,vlb, recip_dy);

			xleft = v3d[vlt].x2d;
			uleft = v3d[vlt].u;
			vleft = v3d[vlt].v;
			lleft = v3d[vlt].l;

			du_dy_left = compute_du_dy_lin(t,vlt,vlb, recip_dy);
			dv_dy_left = compute_dv_dy_lin(t,vlt,vlb, recip_dy);

			if (Lighting_enabled) {
				dl_dy_left = compute_dl_dy_lin(t,vlt,vlb, recip_dy);
				lleft = v3d[vlt].l;
			}
		}

		// See if we have reached the end of the current left edge, and if so, set
		// new values for dx_dy and x.  Not necessary to set new values for u,v.
		if (y == next_break_right) {
			fix	recip_dy;

			while (y == f2i(v3d[vrb].y2d)) {
				vrt = vrb;
				vrb = succmod(vrb,t->nv);
			}

			dy = f2i(t->verts[vrb].y2d) - f2i(t->verts[vrt].y2d);
			if (dy < FIX_RECIP_TABLE_SIZE)
				recip_dy = fix_recip[dy];
			else
				recip_dy = F1_0/dy;

			next_break_right = f2i(v3d[vrb].y2d);
			dx_dy_right = compute_dx_dy(t,vrt,vrb, recip_dy);

			xright = v3d[vrt].x2d;
			uright = v3d[vrt].u;
			vright = v3d[vrt].v;

			du_dy_right = compute_du_dy_lin(t,vrt,vrb, recip_dy);
			dv_dy_right = compute_dv_dy_lin(t,vrt,vrb, recip_dy);

			if (Lighting_enabled) {
				dl_dy_right = compute_dl_dy_lin(t,vrt,vrb, recip_dy);
				lright = v3d[vrt].l;
			}
		}

		if (Lighting_enabled) {
			ntmap_scanline_lighted_linear(srcb,y,xleft,xright,uleft,uright,vleft,vright,lleft,lright);
			lleft += dl_dy_left;
			lright += dl_dy_right;
		} else
			ntmap_scanline_lighted_linear(srcb,y,xleft,xright,uleft,uright,vleft,vright,lleft,lright);

		uleft += du_dy_left;
		vleft += dv_dy_left;

		uright += du_dy_right;
		vright += dv_dy_right;

		xleft += dx_dy_left;
		xright += dx_dy_right;

	}

	// We can get lleft or lright out of bounds here because we compute dl_dy using fixed point values,
	//	but we plot an integer number of scanlines, therefore doing an integer number of additions of the delta.

	ntmap_scanline_lighted_linear(srcb,y,xleft,xright,uleft,uright,vleft,vright,lleft,lright);
}

// fix	DivNum = F1_0*12;

extern void draw_tmap_flat(grs_bitmap *bp,int nverts,g3s_point **vertbuf);

// -------------------------------------------------------------------------------------
// Interface from Matt's data structures to Mike's texture mapper.
// -------------------------------------------------------------------------------------
void draw_tmap(grs_bitmap *bp,int nverts,g3s_point **vertbuf)
{
	int	i;

	//	These variables are used in system which renders texture maps which lie on one scanline as a line.
	// fix	div_numerator;
	int	lighting_on_save = Lighting_on;

	Assert(nverts <= MAX_TMAP_VERTS);


#ifdef USE_MULT_CODE
	if ( !divide_table_filled ) fill_divide_table();
#endif

	// -- now called from g3_start_frame -- init_interface_vars_to_assembler();

	//	If no transparency and seg depth is large, render as flat shaded.
	if ((Current_seg_depth > Max_linear_depth) && ((bp->bm_flags & 3) == 0)) {
		draw_tmap_flat(bp, nverts, vertbuf);
		return;
	}

	if ( bp->bm_flags & BM_FLAG_RLE )
		bp = rle_expand_texture( bp );		// Expand if rle'd

	Transparency_on = bp->bm_flags & BM_FLAG_TRANSPARENT;
	if (bp->bm_flags & BM_FLAG_NO_LIGHTING)
		Lighting_on = 0;


	// Setup texture map in Tmap1
	Tmap1.nv = nverts;						// Initialize number of vertices

// 	div_numerator = DivNum;	//f1_0*3;

	for (i=0; i<nverts; i++) {
		g3ds_vertex	*tvp = &Tmap1.verts[i];
		g3s_point	*vp = vertbuf[i];

		tvp->x2d = vp->p3_sx;
		tvp->y2d = vp->p3_sy;

		//	Check for overflow on fixdiv.  Will overflow on vp->z <= something small.  Allow only as low as 256.
		if (vp->p3_z < 256) {
			vp->p3_z = 256;
			// Int3();		// we would overflow if we divided!
		}

		tvp->z = fixdiv(F1_0*12, vp->p3_z);
		tvp->u = vp->p3_u << 6; //* bp->bm_w;
		tvp->v = vp->p3_v << 6; //* bp->bm_h;

		Assert(Lighting_on < 3);

		if (Lighting_on)
			tvp->l = vp->p3_l * NUM_LIGHTING_LEVELS;
	}


	Lighting_enabled = Lighting_on;

	// Now, call my texture mapper.
	if (Lighting_on) {
		switch (Interpolation_method) {	// 0 = choose, 1 = linear, 2 = /8 perspective, 3 = full perspective
			case 0:								// choose best interpolation
				per2_flag = 1;
				if (Current_seg_depth > Max_perspective_depth)
					ntexture_map_lighted_linear(bp, &Tmap1);
				else
					ntexture_map_lighted(bp, &Tmap1);
				break;
			case 1:								// linear interpolation
				per2_flag = 1;
				ntexture_map_lighted_linear(bp, &Tmap1);
				break;
			case 2:								// perspective every 8th pixel interpolation
				per2_flag = 1;
				ntexture_map_lighted(bp, &Tmap1);
				break;
			case 3:								// perspective every pixel interpolation
				per2_flag = 0;					// this hack means do divide every pixel
				ntexture_map_lighted(bp, &Tmap1);
				break;
			default:
				Assert(0);				// Illegal value for Interpolation_method, must be 0,1,2,3
		}
	} else {
		switch (Interpolation_method) {	// 0 = choose, 1 = linear, 2 = /8 perspective, 3 = full perspective
			case 0:								// choose best interpolation
				per2_flag = 1;
				if (Current_seg_depth > Max_perspective_depth)
					ntexture_map_lighted_linear(bp, &Tmap1);
				else
					ntexture_map_lighted(bp, &Tmap1);
				break;
			case 1:								// linear interpolation
				per2_flag = 1;
				ntexture_map_lighted_linear(bp, &Tmap1);
				break;
			case 2:								// perspective every 8th pixel interpolation
				per2_flag = 1;
				ntexture_map_lighted(bp, &Tmap1);
				break;
			case 3:								// perspective every pixel interpolation
				per2_flag = 0;					// this hack means do divide every pixel
				ntexture_map_lighted(bp, &Tmap1);
				break;
			default:
				Assert(0);				// Illegal value for Interpolation_method, must be 0,1,2,3
		}
	}

	Lighting_on = lighting_on_save;

}
