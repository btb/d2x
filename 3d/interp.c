/* $Id: interp.c,v 1.9 2003-01-02 23:13:21 btb Exp $ */
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
 * Polygon object interpreter
 *
 * Old Log:
 * Revision 1.4  1995/10/10  22:20:09  allender
 * new morphing code from Matt
 *
 * Revision 1.3  1995/08/31  15:40:24  allender
 * swap color data correctly
 *
 * Revision 1.2  1995/05/11  13:06:38  allender
 * fix int --> short problem
 *
 * Revision 1.1  1995/05/05  08:51:41  allender
 * Initial revision
 *
 * Revision 1.1  1995/04/17  06:44:33  matt
 * Initial revision
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#ifdef RCS
static char rcsid[] = "$Id: interp.c,v 1.9 2003-01-02 23:13:21 btb Exp $";
#endif

#include <stdlib.h>
#include "error.h"

#include "interp.h"
#include "globvars.h"
#include "gr.h"
#include "byteswap.h"
#include "u_mem.h"

#define OP_EOF          0   //eof
#define OP_DEFPOINTS    1   //defpoints
#define OP_FLATPOLY     2   //flat-shaded polygon
#define OP_TMAPPOLY     3   //texture-mapped polygon
#define OP_SORTNORM     4   //sort by normal
#define OP_RODBM        5   //rod bitmap
#define OP_SUBCALL      6   //call a subobject
#define OP_DEFP_START   7   //defpoints with start
#define OP_GLOW         8   //glow value for next poly

//#define N_OPCODES (sizeof(opcode_table) / sizeof(*opcode_table))

#define MAX_POINTS_PER_POLY 25

short highest_texture_num;
int g3d_interp_outline;

g3s_point *Interp_point_list = NULL;

#define MAX_INTERP_COLORS 100

//this is a table of mappings from RGB15 to palette colors
struct {short pal_entry,rgb15;} interp_color_table[MAX_INTERP_COLORS];

int n_interp_colors=0;

//gives the interpreter an array of points to use
void g3_set_interp_points(g3s_point *pointlist)
{
	Interp_point_list = pointlist;
}

#define w(p)  (*((short *) (p)))
#define wp(p)  ((short *) (p))
#define fp(p)  ((fix *) (p))
#define vp(p)  ((vms_vector *) (p))

void rotate_point_list(g3s_point *dest,vms_vector *src,int n)
{
	while (n--)
		g3_rotate_point(dest++,src++);
}

vms_angvec zero_angles = {0,0,0};

g3s_point *point_list[MAX_POINTS_PER_POLY];

int glow_num = -1;

#ifdef WORDS_BIGENDIAN
void short_swap(short *s)
{
	*s = SWAPSHORT(*s);
}
void fix_swap(fix *f)
{
	*f = (fix)SWAPINT((int)*f);
}

void vms_vector_swap(vms_vector *v)
{
	fix_swap(fp(&v->x));
	fix_swap(fp(&v->y));
	fix_swap(fp(&v->z));
}

void swap_polygon_model_data(ubyte *data)
{
	int i;
	short n;
	g3s_uvl *uvl_val;
	ubyte *p = data;

	short_swap(wp(p));

	while (w(p) != OP_EOF) {
		switch (w(p)) {
			case OP_DEFPOINTS:
				short_swap(wp(p + 2));
				n = w(p+2);
				for (i = 0; i < n; i++)
					vms_vector_swap(vp((p + 4) + (i * sizeof(vms_vector))));
				p += n*sizeof(struct vms_vector) + 4;
				break;

			case OP_DEFP_START:
				short_swap(wp(p + 2));
				short_swap(wp(p + 4));
				n = w(p+2);
				for (i = 0; i < n; i++)
					vms_vector_swap(vp((p + 8) + (i * sizeof(vms_vector))));
				p += n*sizeof(struct vms_vector) + 8;
				break;

			case OP_FLATPOLY:
				short_swap(wp(p+2));
				n = w(p+2);
				vms_vector_swap(vp(p + 4));
				vms_vector_swap(vp(p + 16));
				short_swap(wp(p+28));
#ifdef MACINTOSH
				// swap the colors 0 and 255 here!!!!
				if (w(p+28) == 0)
					w(p+28) = 255;
				else if (w(p+28) == 255)
					w(p+28) = 0;
#endif
				for (i=0; i < n; i++)
					short_swap(wp(p + 30 + (i * 2)));
				p += 30 + ((n&~1)+1)*2;
				break;

			case OP_TMAPPOLY:
				short_swap(wp(p+2));
				n = w(p+2);
				vms_vector_swap(vp(p + 4));
				vms_vector_swap(vp(p + 16));
				for (i=0;i<n;i++) {
					uvl_val = (g3s_uvl *)((p+30+((n&~1)+1)*2) + (i * sizeof(g3s_uvl)));
					fix_swap(&uvl_val->u);
					fix_swap(&uvl_val->v);
				}
				short_swap(wp(p+28));
				for (i=0;i<n;i++)
					short_swap(wp(p + 30 + (i * 2)));
				p += 30 + ((n&~1)+1)*2 + n*12;
				break;

			case OP_SORTNORM:
				vms_vector_swap(vp(p + 4));
				vms_vector_swap(vp(p + 16));
				short_swap(wp(p + 28));
				short_swap(wp(p + 30));
				swap_polygon_model_data(p + w(p+28));
				swap_polygon_model_data(p + w(p+30));
				p += 32;
				break;

			case OP_RODBM:
				vms_vector_swap(vp(p + 20));
				vms_vector_swap(vp(p + 4));
				short_swap(wp(p+2));
				fix_swap(fp(p + 16));
				fix_swap(fp(p + 32));
				p+=36;
				break;

			case OP_SUBCALL:
				short_swap(wp(p+2));
				vms_vector_swap(vp(p+4));
				short_swap(wp(p+16));
				swap_polygon_model_data(p + w(p+16));
				p += 20;
				break;

			case OP_GLOW:
				short_swap(wp(p + 2));
				p += 4;
				break;

			default:
				Error("invalid polygon model\n"); //Int3();
		}
		short_swap(wp(p));
	}
}
#endif

#ifdef WORDS_NEED_ALIGNMENT

#include <string.h> // for memcpy

typedef struct chunk { // pointer-thing to next chunk of model_data
	ubyte *old_base; // where the offset sets off from (relative to beginning of model_data)
	ubyte *new_base; // where the base is in the aligned structure
	short offset; // how much to add to base to get the address of the offset
	short correction; // how much the value of the offset must be shifted for alignment
} chunk;

ubyte * old_dest(chunk o) // return where the chunk is (in unaligned structure)
{
	return o.old_base + INTEL_SHORT(w(o.old_base + o.offset));
}

ubyte * new_dest(chunk o) // return where the chunk is (in aligned structure)
{
	return o.new_base + INTEL_SHORT(w(o.old_base + o.offset)) + o.correction;
}

#define MAX_CHUNKS 100 // increase if insufficent

int no_chunks; // the number of chunks currently in array
chunk *chunk_list;

void add_chunk(ubyte *old_base, ubyte *new_base, int offset) {
	Assert(no_chunks + 1 < MAX_CHUNKS); //increase MAX_CHUNKS if you get this
	chunk_list[no_chunks].old_base = old_base;
	chunk_list[no_chunks].new_base = new_base;
	chunk_list[no_chunks].offset = offset;
	chunk_list[no_chunks].correction = 0;
	no_chunks++;
}

chunk get_first_chunk() // return chunk (from old structure) with smallest address, removing it from array
{
	int i, first_index = 0;
	chunk first;

	Assert(no_chunks >= 1);
	// find index of chunk with smallest address:
	for (i = 1; i < no_chunks; i++)
		if (old_dest(chunk_list[i]) < old_dest(chunk_list[first_index]))
			first_index = i;
	// assign it:
	first = chunk_list[first_index];
	// remove it from array:
	no_chunks--;
	for (i = first_index; i < no_chunks; i++)
		chunk_list[i] = chunk_list[i + 1];
	return first;
}

/*
 * find out what chunks the chunk "data" points to, return length of current chunk
 */
int get_chunks(ubyte *data, ubyte *new_data)
{
	short n;
	ubyte *p = data;

	while (INTEL_SHORT(w(p)) != OP_EOF) {
		switch (INTEL_SHORT(w(p))) {
		case OP_DEFPOINTS:
			n = INTEL_SHORT(w(p+2));
			p += n*sizeof(struct vms_vector) + 4;
			break;
		case OP_DEFP_START:
			n = INTEL_SHORT(w(p+2));
			p += n*sizeof(struct vms_vector) + 8;
			break;
		case OP_FLATPOLY:
			n = INTEL_SHORT(w(p+2));
			p += 30 + ((n&~1)+1)*2;
			break;
		case OP_TMAPPOLY:
			n = INTEL_SHORT(w(p+2));
			p += 30 + ((n&~1)+1)*2 + n*12;
			break;
		case OP_SORTNORM:
			add_chunk(p, p - data + new_data, 28);
			add_chunk(p, p - data + new_data, 30);
			p += 32;
			break;
		case OP_RODBM:
			p+=36;
			break;
		case OP_SUBCALL:
			add_chunk(p, p - data + new_data, 16);
			p+=20;
			break;
		case OP_GLOW:
			p += 4;
			break;
		default:
			Error("invalid polygon model\n");
		}
	}
	return p + 2 - data;
}

#define SHIFT_SPACE 500 // increase if insufficent

void align_polygon_model_data(polymodel *pm)
{
	int i, chunk_len;
	int total_correction = 0;
	ubyte *cur_old, *cur_new;
	chunk cur_ch;
	chunk cl[MAX_CHUNKS]; // we need the chunk_list only in this function
	int tmp_size = pm->model_data_size + SHIFT_SPACE;
	ubyte *tmp = d_malloc(tmp_size); // where we build the aligned version of pm->model_data

	Assert(tmp != NULL);
	chunk_list = cl; // so other functions can access it
	no_chunks = 0;
	//start with first chunk (is always aligned!)
	cur_old = pm->model_data;
	cur_new = tmp;
	chunk_len = get_chunks(cur_old, cur_new);
	memcpy(cur_new, cur_old, chunk_len);
	while (no_chunks > 0) {
		cur_ch = get_first_chunk();
		if ((u_int32_t)new_dest(cur_ch) % 4L != 0) { // if (new) address unaligned
			short to_shift = 4 - (u_int32_t)new_dest(cur_ch) % 4L; // how much to align

			// correct chunks' addresses
			cur_ch.correction += to_shift;
			for (i = 0; i < no_chunks; i++)
				chunk_list[i].correction += to_shift;
			total_correction += to_shift;
			Assert((u_int32_t)new_dest(cur_ch) % 4L == 0);
			Assert(total_correction <= SHIFT_SPACE); // if you get this, increase SHIFT_SPACE
		}
		//write (corrected) chunk for current chunk:
		w(cur_ch.new_base + cur_ch.offset)
		  = INTEL_SHORT(cur_ch.correction
		  + INTEL_SHORT(w(cur_ch.old_base + cur_ch.offset)));
		//write (correctly aligned) chunk:
		cur_old = old_dest(cur_ch);
		cur_new = new_dest(cur_ch);
		chunk_len = get_chunks(cur_old, cur_new);
		memcpy(cur_new, cur_old, chunk_len);
		//correct submodel_ptr's for pm, too
		for (i = 0; i < MAX_SUBMODELS; i++)
			if (pm->model_data + pm->submodel_ptrs[i] >= cur_old
			    && pm->model_data + pm->submodel_ptrs[i] < cur_old + chunk_len)
				pm->submodel_ptrs[i] += (cur_new - tmp) - (cur_old - pm->model_data);
 	}
	d_free(pm->model_data);
	pm->model_data_size += total_correction;
	pm->model_data = d_malloc(pm->model_data_size);
	Assert(pm->model_data != NULL);
	memcpy(pm->model_data, tmp, pm->model_data_size);
	d_free(tmp);
}

#endif //def WORDS_NEED_ALIGNMENT

void verify(ubyte *data)
{
	short n;
	ubyte *p = data;

	while (w(p) != OP_EOF) {
		switch (w(p)) {
		case OP_DEFPOINTS:
			n = (w(p+2));
			p += n*sizeof(struct vms_vector) + 4;
			break;
		case OP_DEFP_START:
			n = (w(p+2));
			p += n*sizeof(struct vms_vector) + 8;
			break;
		case OP_FLATPOLY:
			n = (w(p+2));
			p += 30 + ((n&~1)+1)*2;
			break;
		case OP_TMAPPOLY:
			n = (w(p+2));
			p += 30 + ((n&~1)+1)*2 + n*12;
			break;
		case OP_SORTNORM:
			verify(p + w(p + 28));
			verify(p + w(p + 30));
			p += 32;
			break;
		case OP_RODBM:
			p+=36;
			break;
		case OP_SUBCALL:
			verify(p + w(p + 16));
			p+=20;
			break;
		case OP_GLOW:
			p += 4;
			break;
		default:
			Error("invalid polygon model\n");
		}
	}
}


//calls the object interpreter to render an object.  The object renderer
//is really a seperate pipeline. returns true if drew
bool g3_draw_polygon_model(void *model_ptr,grs_bitmap **model_bitmaps,vms_angvec *anim_angles,fix model_light,fix *glow_values)
{
	ubyte *p = model_ptr;

	glow_num = -1;		//glow off by default

	while (w(p) != OP_EOF)

		switch (w(p)) {

			case OP_DEFPOINTS: {
				int n = w(p+2);

				rotate_point_list(Interp_point_list,vp(p+4),n);
				p += n*sizeof(struct vms_vector) + 4;

				break;
			}

			case OP_DEFP_START: {
				int n = w(p+2);
				int s = w(p+4);

				rotate_point_list(&Interp_point_list[s],vp(p+8),n);
				p += n*sizeof(struct vms_vector) + 8;

				break;
			}

			case OP_FLATPOLY: {
				int nv = w(p+2);

				Assert( nv < MAX_POINTS_PER_POLY );
				if (g3_check_normal_facing(vp(p+4),vp(p+16)) > 0) {
					int i;
					short c;
					unsigned char cc;
					int l;

//					DPH: Now we treat this color as 15bpp
//					gr_setcolor(w(p+28));
					
					//l = (32 * model_light) >> 16;
					l = f2i(fixmul(i2f(32), model_light));
					if (l<0) l = 0;
					else if (l>32) l = 32;
					cc = gr_find_closest_color_15bpp(w(p+28));
					c = gr_fade_table[(l<<8)|cc];
					gr_setcolor(c);

					for (i=0;i<nv;i++)
						point_list[i] = Interp_point_list + wp(p+30)[i];

					g3_draw_poly(nv,point_list);
				}

				p += 30 + ((nv&~1)+1)*2;
					
				break;
			}

			case OP_TMAPPOLY: {
				int nv = w(p+2);
				g3s_uvl *uvl_list;

				Assert( nv < MAX_POINTS_PER_POLY );
				if (g3_check_normal_facing(vp(p+4),vp(p+16)) > 0) {
					int i;
					fix light;

					//calculate light from surface normal

					if (glow_num < 0) {			//no glow

						light = -vm_vec_dot(&View_matrix.fvec,vp(p+16));
						light = f1_0/4 + (light*3)/4;
						light = fixmul(light,model_light);
					}
					else {				//yes glow
						light = glow_values[glow_num];
						glow_num = -1;
					}

					//now poke light into l values

					uvl_list = (g3s_uvl *) (p+30+((nv&~1)+1)*2);

					for (i=0;i<nv;i++)
						uvl_list[i].l = light;

					for (i=0;i<nv;i++)
						point_list[i] = Interp_point_list + wp(p+30)[i];

					g3_draw_tmap(nv,point_list,uvl_list,model_bitmaps[w(p+28)]);
				}

				p += 30 + ((nv&~1)+1)*2 + nv*12;
					
				break;
			}

			case OP_SORTNORM:

				if (g3_check_normal_facing(vp(p+16),vp(p+4)) > 0) {		//facing

					//draw back then front

					g3_draw_polygon_model(p+w(p+30),model_bitmaps,anim_angles,model_light,glow_values);
					g3_draw_polygon_model(p+w(p+28),model_bitmaps,anim_angles,model_light,glow_values);

				}
				else {			//not facing.  draw front then back

					g3_draw_polygon_model(p+w(p+28),model_bitmaps,anim_angles,model_light,glow_values);
					g3_draw_polygon_model(p+w(p+30),model_bitmaps,anim_angles,model_light,glow_values);
				}

				p += 32;

				break;


			case OP_RODBM: {
				g3s_point rod_bot_p,rod_top_p;

				g3_rotate_point(&rod_bot_p,vp(p+20));
				g3_rotate_point(&rod_top_p,vp(p+4));

				g3_draw_rod_tmap(model_bitmaps[w(p+2)],&rod_bot_p,w(p+16),&rod_top_p,w(p+32),f1_0);

				p+=36;
				break;
			}

			case OP_SUBCALL: {
				vms_angvec *a;

				if (anim_angles)
					a = &anim_angles[w(p+2)];
				else
					a = &zero_angles;

				g3_start_instance_angles(vp(p+4),a);

				g3_draw_polygon_model(p+w(p+16),model_bitmaps,anim_angles,model_light,glow_values);

				g3_done_instance();

				p += 20;

				break;

			}

			case OP_GLOW:

				if (glow_values)
					glow_num = w(p+2);
				p += 4;
				break;

			default:
				Error("invalid polygon model\n");
		}
	return 1;
}

extern int gr_find_closest_color_15bpp( int rgb );

#ifndef NDEBUG
int nest_count;
#endif

//alternate interpreter for morphing object
bool g3_draw_morphing_model(void *model_ptr,grs_bitmap **model_bitmaps,vms_angvec *anim_angles,fix model_light,vms_vector *new_points)
{
	ubyte *p = model_ptr;
	fix *glow_values = NULL;

	glow_num = -1;		//glow off by default

	while (w(p) != OP_EOF)

		switch (w(p)) {

			case OP_DEFPOINTS: {
				int n = w(p+2);

				rotate_point_list(Interp_point_list,new_points,n);
				p += n*sizeof(struct vms_vector) + 4;

				break;
			}

			case OP_DEFP_START: {
				int n = w(p+2);
				int s = w(p+4);

				rotate_point_list(&Interp_point_list[s],new_points,n);
				p += n*sizeof(struct vms_vector) + 8;

				break;
			}

			case OP_FLATPOLY: {
				int nv = w(p+2);
				int i,ntris;

				gr_setcolor(w(p+28));
				
				for (i=0;i<2;i++)
					point_list[i] = Interp_point_list + wp(p+30)[i];

				for (ntris=nv-2;ntris;ntris--) {

					point_list[2] = Interp_point_list + wp(p+30)[i++];

					g3_check_and_draw_poly(3,point_list,NULL,NULL);

					point_list[1] = point_list[2];

				}

				p += 30 + ((nv&~1)+1)*2;
					
				break;
			}

			case OP_TMAPPOLY: {
				int nv = w(p+2);
				g3s_uvl *uvl_list;
				g3s_uvl morph_uvls[3];
				int i,ntris;
				fix light;

				//calculate light from surface normal

				if (glow_num < 0) {			//no glow

					light = -vm_vec_dot(&View_matrix.fvec,vp(p+16));
					light = f1_0/4 + (light*3)/4;
					light = fixmul(light,model_light);
				}
				else {				//yes glow
					light = glow_values[glow_num];
					glow_num = -1;
				}

				//now poke light into l values

				uvl_list = (g3s_uvl *) (p+30+((nv&~1)+1)*2);

				for (i=0;i<3;i++)
					morph_uvls[i].l = light;

				for (i=0;i<2;i++) {
					point_list[i] = Interp_point_list + wp(p+30)[i];

					morph_uvls[i].u = uvl_list[i].u;
					morph_uvls[i].v = uvl_list[i].v;
				}

				for (ntris=nv-2;ntris;ntris--) {

					point_list[2] = Interp_point_list + wp(p+30)[i];
					morph_uvls[2].u = uvl_list[i].u;
					morph_uvls[2].v = uvl_list[i].v;
					i++;

					g3_check_and_draw_tmap(3,point_list,uvl_list,model_bitmaps[w(p+28)],NULL,NULL);

					point_list[1] = point_list[2];
					morph_uvls[1].u = morph_uvls[2].u;
					morph_uvls[1].v = morph_uvls[2].v;

				}

				p += 30 + ((nv&~1)+1)*2 + nv*12;

				break;
			}

			case OP_SORTNORM:

				if (g3_check_normal_facing(vp(p+16),vp(p+4)) > 0) {		//facing

					//draw back then front

					g3_draw_morphing_model(p+w(p+30),model_bitmaps,anim_angles,model_light,new_points);
					g3_draw_morphing_model(p+w(p+28),model_bitmaps,anim_angles,model_light,new_points);

				}
				else {			//not facing.  draw front then back

					g3_draw_morphing_model(p+w(p+28),model_bitmaps,anim_angles,model_light,new_points);
					g3_draw_morphing_model(p+w(p+30),model_bitmaps,anim_angles,model_light,new_points);
				}

				p += 32;

				break;


			case OP_RODBM: {
				g3s_point rod_bot_p,rod_top_p;

				g3_rotate_point(&rod_bot_p,vp(p+20));
				g3_rotate_point(&rod_top_p,vp(p+4));

				g3_draw_rod_tmap(model_bitmaps[w(p+2)],&rod_bot_p,w(p+16),&rod_top_p,w(p+32),f1_0);

				p+=36;
				break;
			}

			case OP_SUBCALL: {
				vms_angvec *a;

				if (anim_angles)
					a = &anim_angles[w(p+2)];
				else
					a = &zero_angles;

				g3_start_instance_angles(vp(p+4),a);

				g3_draw_polygon_model(p+w(p+16),model_bitmaps,anim_angles,model_light,glow_values);

				g3_done_instance();

				p += 20;

				break;

			}

			case OP_GLOW:

				if (glow_values)
					glow_num = w(p+2);
				p += 4;
				break;
		}

	return 1;
}

void init_model_sub(ubyte *p)
{
	Assert(++nest_count < 1000);

	while (w(p) != OP_EOF) {

		switch (w(p)) {

			case OP_DEFPOINTS: {
				int n = w(p+2);
				p += n*sizeof(struct vms_vector) + 4;
				break;
			}

			case OP_DEFP_START: {
				int n = w(p+2);
				p += n*sizeof(struct vms_vector) + 8;
				break;
			}

			case OP_FLATPOLY: {
				int nv = w(p+2);

				Assert(nv > 2);		//must have 3 or more points

//				*wp(p+28) = (short)gr_find_closest_color_15bpp(w(p+28));

				p += 30 + ((nv&~1)+1)*2;
					
				break;
			}

			case OP_TMAPPOLY: {
				int nv = w(p+2);

				Assert(nv > 2);		//must have 3 or more points

				if (w(p+28) > highest_texture_num)
					highest_texture_num = w(p+28);

				p += 30 + ((nv&~1)+1)*2 + nv*12;
					
				break;
			}

			case OP_SORTNORM:

				init_model_sub(p+w(p+28));
				init_model_sub(p+w(p+30));
				p += 32;

				break;


			case OP_RODBM:
				p += 36;
				break;


			case OP_SUBCALL: {
				init_model_sub(p+w(p+16));
				p += 20;
				break;

			}

			case OP_GLOW:
				p += 4;
				break;
			default:
				Error("invalid polygon model\n");
		}
	}
}

//init code for bitmap models
void g3_init_polygon_model(void *model_ptr)
{
	#ifndef NDEBUG
	nest_count = 0;
	#endif

	highest_texture_num = -1;

	init_model_sub((ubyte *) model_ptr);
}

// routine which allocates, reads, and inits a polymodel's model_data
void polygon_model_data_read(polymodel *pm, CFILE *fp)
{
	pm->model_data = d_malloc(pm->model_data_size);
	Assert(pm->model_data != NULL);
	cfread(pm->model_data, sizeof(ubyte), pm->model_data_size, fp );
#ifdef WORDS_NEED_ALIGNMENT
	align_polygon_model_data(pm);
#endif
#ifdef WORDS_BIGENDIAN
	swap_polygon_model_data(pm->model_data);
#endif
	//verify(pm->model_data);
	g3_init_polygon_model(pm->model_data);
}
