/*
 *
 * SVGALib video functions
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vga.h>
#include <vgagl.h>
#include "gr.h"
#include "grdef.h"
#include "palette.h"
#include "u_mem.h"
#include "error.h"
#ifdef SVGALIB_INPUT
#include <vgamouse.h>
#endif

#include "gamefont.h"


int vid_installed = 0;
int usebuffer;

extern void mouse_handler (int button, int dx, int dy, int dz, int drx, int dry, int drz);

GraphicsContext *physicalscreen, *screenbuffer;


void vid_update()
{
	if (usebuffer)
		gl_copyscreen(physicalscreen);
}


uint32_t Vid_current_mode;


int vid_set_mode(uint32_t mode)
{
	unsigned int w, h;
	char vgamode[16];
	vga_modeinfo *modeinfo;
	int modenum, rowsize;
	void *framebuffer;
	
#ifdef NOGRAPH
	return 0;
#endif
	if (mode<=0)
		return 0;

	w=SM_W(mode);
	h=SM_H(mode);
	Vid_current_mode = mode;

	gr_palette_clear();

	sprintf(vgamode, "G%dx%dx256", w, h);
	modenum = vga_getmodenumber(vgamode);	
	vga_setmode(modenum);
#ifdef SVGALIB_INPUT
	mouse_seteventhandler(mouse_handler);
#endif
	modeinfo = vga_getmodeinfo(modenum);

	if (modeinfo->flags & CAPABLE_LINEAR)
	{
		usebuffer = 0;

		vga_setlinearaddressing();

		// Set up physical screen only
		gl_setcontextvga(modenum);
		physicalscreen = gl_allocatecontext();
		gl_getcontext(physicalscreen);
		screenbuffer = physicalscreen;

		framebuffer = physicalscreen->vbuf;
		rowsize = physicalscreen->bytewidth;
	}
	else
	{
		usebuffer = 1;

		// Set up the physical screen
		gl_setcontextvga(modenum);
		physicalscreen = gl_allocatecontext();
		gl_getcontext(physicalscreen);

		// Set up the virtual screen
		gl_setcontextvgavirtual(modenum);
		screenbuffer = gl_allocatecontext();
		gl_getcontext(screenbuffer);

		framebuffer = screenbuffer->vbuf;
		rowsize = screenbuffer->bytewidth;
	}

	//gamefont_choose_game_font(w,h);

	return gr_init_screen(BM_LINEAR, w, h, 0, 0, rowsize, framebuffer);
}


int vid_init(void)
{
	int retcode;
	int mode = SM(320,200);

 	// Only do this function once!
	if (vid_installed == 1)
		return -1;
	
	vga_init();

	if ((retcode = gr_set_mode(mode)))
		return retcode;

	vid_installed = 1;
	atexit(vid_close);
	return 0;
}


void vid_close(void)
{
	if (vid_installed == 1) {
		vid_installed = 0;
		free(grd_curscreen);
		gl_freecontext(screenbuffer);
		gl_freecontext(physicalscreen);
	}
}

// Palette functions follow.

static int last_r=0, last_g=0, last_b=0;

void gr_palette_clear ()
{
	int colors[768];

	memset (colors, 0, 768 * sizeof(int));
	vga_setpalvec (0, 256, colors);

	gr_palette_faded_out = 1;
}


void gr_palette_step_up (int r, int g, int b)
{
	int i = 0;
	ubyte *p = gr_palette;
	int temp;

	int colors[768];

	if (gr_palette_faded_out) return;

	if ((r==last_r) && (g==last_g) && (b==last_b)) return;

	last_r = r;
	last_g = g;
	last_b = b;

	while (i < 768)
	{
		temp = (int)(*p++) + r + gr_palette_gamma.intval;
		if (temp<0) temp=0;
		else if (temp>63) temp=63;
		colors[i++] = temp;
		temp = (int)(*p++) + g + gr_palette_gamma.intval;
		if (temp<0) temp=0;
		else if (temp>63) temp=63;
		colors[i++] = temp;
		temp = (int)(*p++) + b + gr_palette_gamma.intval;
		if (temp<0) temp=0;
		else if (temp>63) temp=63;
		colors[i++] = temp;
	}
	vga_setpalvec (0, 256, colors);
}

//added on 980913 by adb to fix palette problems
// need a min without side effects...
#undef min
static inline int min(int x, int y) { return x < y ? x : y; }
//end changes by adb

void gr_palette_load (ubyte *pal)	
{
	int i;
	int colors[768];

	for (i = 0; i < 768; i++)
	{
		gr_current_pal[i] = pal[i];
		if (gr_current_pal[i] > 63) gr_current_pal[i] = 63;
		colors[i] = (min(gr_current_pal[i] + gr_palette_gamma.intval, 63));
	}

	vga_setpalvec (0, 256, colors);

	gr_palette_faded_out = 0;
	init_computed_colors();
}



int gr_palette_fade_out (ubyte *pal, int nsteps, int allow_keys)
{
	int i, j;
	ubyte c;
	fix fade_palette[768];
	fix fade_palette_delta[768];

	int fade_colors[768];

	if (gr_palette_faded_out) return 0;

	if (pal==NULL) pal=gr_current_pal;

	for (i=0; i<768; i++)
	{
		gr_current_pal[i] = pal[i];
		fade_palette[i] = i2f(pal[i]);
		fade_palette_delta[i] = fade_palette[i] / nsteps;
	}

	for (j=0; j<nsteps; j++)
	{
		for (i = 0; i < 768; i++)
		{
			fade_palette[i] -= fade_palette_delta[i];
			if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma.intval))
				fade_palette[i] = i2f(pal[i] + gr_palette_gamma.intval);
			c = f2i(fade_palette[i]);
			if (c > 63) c = 63;
			fade_colors[i] = c;
		}
		vga_setpalvec (0, 256, fade_colors);
	}

	gr_palette_faded_out = 1;
	return 0;
}



int gr_palette_fade_in (ubyte *pal, int nsteps, int allow_keys)
{
	int i, j;
	ubyte c;
	fix fade_palette[768];
	fix fade_palette_delta[768];

	int fade_colors[768];

	if (!gr_palette_faded_out) return 0;

	for (i=0; i<768; i++)
	{
		gr_current_pal[i] = pal[i];
		fade_palette[i] = 0;
		fade_palette_delta[i] = i2f(pal[i] + gr_palette_gamma.intval) / nsteps;
	}

 	for (j=0; j<nsteps; j++ )
	{
		for (i = 0; i < 768; i++ )
		{
			fade_palette[i] += fade_palette_delta[i];
			if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma.intval))
				fade_palette[i] = i2f(pal[i] + gr_palette_gamma.intval);
			c = f2i(fade_palette[i]);
			if (c > 63) c = 63;
			fade_colors[i] = c;
		}
		vga_setpalvec (0, 256, fade_colors);
	}

	gr_palette_faded_out = 0;
	return 0;
}



void gr_palette_read (ubyte *pal)
{
	int colors[768];
	int i;

	vga_getpalvec (0, 256, colors);

	for (i = 0; i < 768; i++)
		pal[i] = colors[i];
}
