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
COPYRIGHT 1993-1999 PARALLAX SOFTWARE CORPORATION.  ALL RIGHTS RESERVED.
*/

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include "gr.h"
#include "u_mem.h"
#include "cmd.h"


int gr_installed = 0;

cvar_t gr_aspectratio = { "r_aspectratio", "0", 0 };

//	Functions for GR.C

int gr_close_screen(void);

void gr_cmd_pixelaspect(int argc, char **argv);


void gr_close(void)
{
	gr_close_screen();
	gr_installed = 0;
}


int gr_init(void)
{
	// Only do this function once!
	if (gr_installed == 1)
		return 1;

	cvar_registervariable(&gr_palette_gamma);
	cvar_registervariable(&gr_aspectratio);

	cmd_addcommand("pixelaspect", gr_cmd_pixelaspect, "pixelaspect [n]\n" "    set the pixel aspect ratio to <n> (1.0 is square pixels)\n");

	// Set flags indicating that this is installed.
	gr_installed = 1;

	atexit(gr_close);

	return 0;
}


int gr_close_screen(void)
{
	if (grd_curscreen) {
		d_free(grd_curscreen);
		grd_curscreen = NULL;
	}

	return 0;
}


int gr_init_screen(int bitmap_type, int w, int h, int x, int y, int rowsize, ubyte *screen_addr)
{
	if (!gr_installed)
		return 1;

	if (grd_curscreen == NULL)
		MALLOC( grd_curscreen, grs_screen, 1 );

	memset(grd_curscreen, 0, sizeof(grs_screen));

	grd_curscreen->sc_mode = bitmap_type;
	grd_curscreen->sc_w = w;
	grd_curscreen->sc_h = h;
	switch (gr_aspectratio.intval) {
	default:
	case 0:
		grd_curscreen->sc_aspect = fixdiv(grd_curscreen->sc_w * 3, grd_curscreen->sc_h * 4);
		break;
	case 1:
		grd_curscreen->sc_aspect = fixdiv(grd_curscreen->sc_w * 9, grd_curscreen->sc_h * 16);
		break;
	case 2:
		grd_curscreen->sc_aspect = fixdiv(grd_curscreen->sc_w * 10, grd_curscreen->sc_h * 16);
		break;
	}
	grd_curscreen->sc_canvas.cv_bitmap.bm_x = x;
	grd_curscreen->sc_canvas.cv_bitmap.bm_y = y;
	grd_curscreen->sc_canvas.cv_bitmap.bm_w = w;
	grd_curscreen->sc_canvas.cv_bitmap.bm_h = h;
	grd_curscreen->sc_canvas.cv_bitmap.bm_rowsize = rowsize;
	grd_curscreen->sc_canvas.cv_bitmap.bm_type = bitmap_type;
	grd_curscreen->sc_canvas.cv_bitmap.bm_data = (bitmap_type == BM_LINEAR) ? screen_addr : NULL;

	grd_curscreen->sc_canvas.cv_color = 0;
	grd_curscreen->sc_canvas.cv_drawmode = 0;
	grd_curscreen->sc_canvas.cv_font = NULL;
	grd_curscreen->sc_canvas.cv_font_fg_color = 0;
	grd_curscreen->sc_canvas.cv_font_bg_color = 0;

	gr_set_current_canvas( &grd_curscreen->sc_canvas );

	return 0;
}


void gr_cmd_pixelaspect(int argc, char **argv)
{
	if (argc < 2 || argc > 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}

	grd_curscreen->sc_aspect = fl2f(strtof(argv[1], NULL));
}
