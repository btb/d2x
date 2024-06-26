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

/*
 *
 * Stuff for rendering the HUD
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inferno.h"
#include "dxxerror.h"
#include "mono.h"
#include "gr.h"
#include "vid.h"
#include "maths.h"
#ifdef OGL
#include "ogl_init.h"
#endif


extern fix Cruise_speed;
extern int LinearSVGABuffer;
extern cvar_t r_framerate;


#ifndef NDEBUG
extern int Debug_pause; // John's debugging pause system
#endif

#ifndef RELEASE
extern int Saving_movie_frames;
#else
#define Saving_movie_frames 0
#endif


// Returns the length of the first 'n' characters of a string.
int string_width( char *s, int n )
{
	int w, h, aw;
	char p;

	p = s[n];
	s[n] = 0;
	gr_get_string_size( s, &w, &h, &aw );
	s[n] = p;
	return w;
}


// Draw string 's' centered on a canvas... if wider than
// canvas, then wrap it.
void draw_centered_text( int y, char *s )
{
	int i, l;
	char p;

	l = (int)strlen(s);

	if ( string_width( s, l ) < grd_curcanv->cv_bitmap.bm_w )
	{
		gr_string( 0x8000, y, s );
		return;
	}

	for ( i = 0; i < l; i++ )
	{
		if ( string_width(s, i) > (grd_curcanv->cv_bitmap.bm_w - 16) )
		{
			p = s[i];
			s[i] = 0;
			gr_string( 0x8000, y, s );
			s[i] = p;
			gr_string( 0x8000, y+grd_curcanv->cv_font->ft_h+1, &s[i] );
			return;
		}
	}
}


extern ubyte DefiningMarkerMessage;
extern char Marker_input[];

#define MAX_MARKER_MESSAGE_LEN 120
void game_draw_marker_message(void)
{
	char temp_string[MAX_MARKER_MESSAGE_LEN+25];

	if ( DefiningMarkerMessage)
	{
		gr_set_curfont( GAME_FONT );    //GAME_FONT
		gr_set_fontcolor( gr_getcolor(0,63,0), -1 );
		sprintf( temp_string, "Marker: %s_", Marker_input );
		draw_centered_text(grd_curcanv->cv_bitmap.bm_h/2 - 16, temp_string );
	}
}


#ifdef NETWORK
void game_draw_multi_message(void)
{
	char temp_string[MAX_MULTI_MESSAGE_LEN+25];

	if ( (Game_mode&GM_MULTI) && multi_sending_message )
	{
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor( gr_getcolor(0,63,0), -1 );
		sprintf( temp_string, "%s: %s_", TXT_MESSAGE, Network_message );
		draw_centered_text( grd_curcanv->cv_bitmap.bm_h/2-16, temp_string );
	}

	if ( (Game_mode&GM_MULTI) && multi_defining_message )
	{
		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor( gr_getcolor(0,63,0), -1 );
		sprintf( temp_string, "%s #%d: %s_", TXT_MACRO, multi_defining_message, Network_message );
		draw_centered_text( grd_curcanv->cv_bitmap.bm_h/2-16, temp_string );
	}
}
#endif


//these should be in gr.h
#define cv_w  cv_bitmap.bm_w
#define cv_h  cv_bitmap.bm_h

fix frame_time_list[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
fix frame_time_total = 0;
int frame_time_cntr = 0;


void ftoa(char *string, fix f)
{
	int decimal, fractional;

	decimal = f2i(f);
	fractional = ((f & 0xffff)*100)/65536;
	if (fractional < 0 )
		fractional *= -1;
	if (fractional > 99 )
		fractional = 99;
	sprintf( string, "%d.%02d", decimal, fractional );
}


void show_framerate(void)
{
	char temp[50];
//	static int q;
	fix rate;
	int x = 8, y = 5; // position measured from lower right corner

	frame_time_total += RealFrameTime - frame_time_list[frame_time_cntr];
	frame_time_list[frame_time_cntr] = RealFrameTime;
	frame_time_cntr = (frame_time_cntr+1) % 8;

	rate = fixdiv(f1_0*8, frame_time_total);

	gr_set_curfont( GAME_FONT );	
	gr_set_fontcolor( gr_getcolor(0,31,0), -1 );

	ftoa( temp, rate ); // Convert fixed to string
	if (Game_mode & GM_MULTI)
		y = 7;
	gr_printf(grd_curcanv->cv_w-(x*GAME_FONT->ft_w),grd_curcanv->cv_h-y*(GAME_FONT->ft_h+GAME_FONT->ft_h/4),"FPS: %s ", temp );
//	if ( !( q++ % 30 ) )
//		mprintf(( 0, "fps: %s\n", temp ));
}


#ifndef NDEBUG
fix Show_view_text_timer = -1;

void draw_window_label(void)
{
	if ( Show_view_text_timer > 0 )
	{
		char *viewer_name, *control_name;
		char *viewer_id;
		Show_view_text_timer -= FrameTime;
		gr_set_curfont( GAME_FONT );

		viewer_id = "";
		switch( Viewer->type )
		{
			case OBJ_FIREBALL:  viewer_name = "Fireball";       break;
			case OBJ_ROBOT:     viewer_name = "Robot";
			                    viewer_id = Robot_names[Viewer->id];
				break;
			case OBJ_HOSTAGE:   viewer_name = "Hostage";        break;
			case OBJ_PLAYER:    viewer_name = "Player";         break;
			case OBJ_WEAPON:    viewer_name = "Weapon";         break;
			case OBJ_CAMERA:    viewer_name = "Camera";         break;
			case OBJ_POWERUP:   viewer_name = "Powerup";
			                    viewer_id = Powerup_names[Viewer->id];
				break;
			case OBJ_DEBRIS:    viewer_name = "Debris";         break;
			case OBJ_CNTRLCEN:  viewer_name = "Reactor";        break;
			default:            viewer_name = "Unknown";        break;
		}

		switch ( Viewer->control_type)
		{
			case CT_NONE:       control_name = "Stopped";       break;
			case CT_AI:         control_name = "AI";            break;
			case CT_FLYING:     control_name = "Flying";        break;
			case CT_SLEW:       control_name = "Slew";          break;
			case CT_FLYTHROUGH: control_name = "Flythrough";    break;
			case CT_MORPH:      control_name = "Morphing";      break;
			default:            control_name = "Unknown";       break;
		}

		gr_set_fontcolor( gr_getcolor(31, 0, 0), -1 );
		gr_printf( 0x8000, 45, "%i: %s [%s] View - %s", OBJECT_NUMBER(Viewer), viewer_name, viewer_id, control_name );

	}
}
#endif


extern int Game_window_x;
extern int Game_window_y;
extern int max_window_w;
extern int max_window_h;


void render_countdown_gauge(void)
{
	if ( !Endlevel_sequence && Control_center_destroyed && (Countdown_seconds_left>-1)) // && (Countdown_seconds_left<127))
	{
		int	y;

		if (!is_D2_OEM && !is_MAC_SHARE && !is_SHAREWARE) // no countdown on registered only
		{
			//	On last level, we don't want a countdown.
			if (PLAYING_BUILTIN_MISSION && Current_level_num == Last_level)
			{
				if (!(Game_mode & GM_MULTI))
					return;
				if (Game_mode & GM_MULTI_ROBOTS)
					return;
			}
		}

		gr_set_curfont( SMALL_FONT );
		gr_set_fontcolor( gr_getcolor(0,63,0), -1 );
		y = SMALL_FONT->ft_h*4;
		if (Cockpit_mode.intval == CM_FULL_SCREEN)
			y += SMALL_FONT->ft_h*2;

		if (Player_is_dead)
			y += SMALL_FONT->ft_h*2;

		//if (!((Cockpit_mode.intval == CM_STATUS_BAR) && (Game_window_y >= 19)))
		//	y += 5;
		gr_printf( 0x8000, y, "T-%d s", Countdown_seconds_left );
	}
}


void game_draw_hud_stuff(void)
{
	//mprintf((0, "Linear is %d!\n", LinearSVGABuffer));

#ifndef NDEBUG
	if (Debug_pause)
	{
		gr_set_curfont( MEDIUM1_FONT );
		gr_set_fontcolor( gr_getcolor(31, 31, 31), -1 ); //gr_getcolor(31,0,0));
		gr_ustring( 0x8000, 85/2, "Debug Pause - Press P to exit" );
	}

	draw_window_label();
#endif

#ifdef NETWORK
	game_draw_multi_message();
#endif

	game_draw_marker_message();

//	if (Game_mode & GM_MULTI)
//	{
//		if (Netgame.PlayTimeAllowed)
//			game_draw_time_left ();
//	}

	if ( (Newdemo_state == ND_STATE_PLAYBACK) || (Newdemo_state == ND_STATE_RECORDING) )
	{
		char message[128];
		int h, w, aw;

		if (Newdemo_state == ND_STATE_PLAYBACK)
		{
			if (Newdemo_vcr_state != ND_STATE_PRINTSCREEN)
			  	sprintf(message, "%s (%d%%%% %s)", TXT_DEMO_PLAYBACK, newdemo_get_percent_done(), TXT_DONE);
			else
				sprintf(message, " ");
		}
		else
			sprintf(message, "%s", TXT_DEMO_RECORDING);

		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor( gr_getcolor(27,0,0), -1 );

		gr_get_string_size( message, &w, &h, &aw );
		if (Cockpit_mode.intval == CM_FULL_COCKPIT)
		{
			if (grd_curcanv->cv_bitmap.bm_h > 240)
				h += 40;
			else
				h += 15;
		}
		else if ( Cockpit_mode.intval == CM_LETTERBOX )
			h += 7;
		if (Cockpit_mode.intval != CM_REAR_VIEW && !Saving_movie_frames)
			gr_printf( (grd_curcanv->cv_bitmap.bm_w-w)/2, grd_curcanv->cv_bitmap.bm_h - h - 2, message );
	}

	render_countdown_gauge();

	if ( Player_num > -1 && Viewer->type==OBJ_PLAYER && Viewer->id==Player_num )
	{
		int	x = 3;
		int	y = grd_curcanv->cv_bitmap.bm_h;

		gr_set_curfont( GAME_FONT );
		gr_set_fontcolor( gr_getcolor(0, 31, 0), -1 );
		if (Cruise_speed > 0)
		{
			int line_spacing = GAME_FONT->ft_h + GAME_FONT->ft_h/4;

			mprintf((0, "line_spacing=%d ", line_spacing));

			if (Cockpit_mode.intval == CM_FULL_SCREEN)
			{
				if (Game_mode & GM_MULTI)
					y -= line_spacing * 11; //64
				else
					y -= line_spacing * 6;  //32
			}
			else if (Cockpit_mode.intval == CM_STATUS_BAR)
			{
				if (Game_mode & GM_MULTI)
					y -= line_spacing * 8;  //48
				else
					y -= line_spacing * 4;  //24
			}
			else
			{
				y = line_spacing * 2;       //12
				x = 20+2;
			}

			gr_printf( x, y, "%s %2d%%", TXT_CRUISE, f2i(Cruise_speed) );
		}
	}

	if (r_framerate.intval)
		show_framerate();

	if ( Newdemo_state == ND_STATE_PLAYBACK )
		Game_mode = Newdemo_game_mode;

	draw_hud();

	if ( Newdemo_state == ND_STATE_PLAYBACK )
		Game_mode = GM_NORMAL;

	if ( Player_is_dead )
		player_dead_message();
}


extern int gr_bitblt_dest_step_shift;
extern int gr_wait_for_retrace;
extern int gr_bitblt_double;


#if 0
void expand_row(ubyte *dest, ubyte *src, int num_src_pixels );
#pragma aux expand_row parm [edi] [esi] [ecx] modify exact [ecx esi edi eax ebx] = \
	"add	esi, ecx"   \
	"dec	esi"        \
	"add	edi, ecx"   \
	"add	edi, ecx"   \
	"dec	edi"        \
	"dec	edi"        \
"nextpixel:"	        \
	"mov	al,[esi]"   \
	"mov	ah, al"     \
	"dec	esi"        \
	"mov	[edi], ax"  \
	"dec	edi"        \
	"dec	edi"        \
	"dec	ecx"        \
	"jnz	nextpixel"  \
"done:"
#else
void expand_row( ubyte *dest, ubyte *src, int num_src_pixels )
{
	int i;

	for (i = 0; i < num_src_pixels; i++)
	{
		*dest++ = *src;
		*dest++ = *src++;
	}
}
#endif


// doubles the size in x or y of a bitmap in place.
void game_expand_bitmap( grs_bitmap *bmp, uint flags )
{
	int i;
	ubyte *dptr, *sptr;

	switch(flags & 3)
	{
	case 2: // expand x
		Assert( bmp->bm_rowsize == bmp->bm_w*2 );
		dptr = &bmp->bm_data[(bmp->bm_h-1)*bmp->bm_rowsize];
		for ( i = bmp->bm_h-1; i >= 0; i-- )
		{
			expand_row( dptr, dptr, bmp->bm_w );	
			dptr -= bmp->bm_rowsize;
		}
		bmp->bm_w *= 2;
		break;
	case 1: // expand y
		dptr = &bmp->bm_data[(2*(bmp->bm_h-1)+1)*bmp->bm_rowsize];
		sptr = &bmp->bm_data[(bmp->bm_h-1)*bmp->bm_rowsize];
		for ( i = bmp->bm_h-1; i >= 0; i-- )
		{
			memcpy( dptr, sptr, bmp->bm_w );	
			dptr -= bmp->bm_rowsize;
			memcpy( dptr, sptr, bmp->bm_w );	
			dptr -= bmp->bm_rowsize;
			sptr -= bmp->bm_rowsize;
		}
		bmp->bm_h *= 2;
		break;
	case 3: // expand x & y
		Assert( bmp->bm_rowsize == bmp->bm_w*2 );
		dptr = &bmp->bm_data[(2*(bmp->bm_h-1)+1)*bmp->bm_rowsize];
		sptr = &bmp->bm_data[(bmp->bm_h-1)*bmp->bm_rowsize];
		for ( i = bmp->bm_h-1; i >= 0; i-- )
		{
			expand_row( dptr, sptr, bmp->bm_w );	
			dptr -= bmp->bm_rowsize;
			expand_row( dptr, sptr, bmp->bm_w );	
			dptr -= bmp->bm_rowsize;
			sptr -= bmp->bm_rowsize;
		}
		bmp->bm_w *= 2;
		bmp->bm_h *= 2;
		break;
	}
}


extern int SW_drawn[2], SW_x[2], SW_y[2], SW_w[2], SW_h[2];


#if 0
//render a frame for the game in stereo
void game_render_frame_stereo(void)
{
	int dw, dh, sw, sh;
	fix save_aspect;
	fix actual_eye_width;
	int actual_eye_offset;
	grs_canvas RenderCanvas[2];
	int no_draw_hud = 0;

	save_aspect = grd_curscreen->sc_aspect;
	grd_curscreen->sc_aspect *= 2; // Muck with aspect ratio

	sw = dw = VR_render_buffer[0].cv_bitmap.bm_w;
	sh = dh = VR_render_buffer[0].cv_bitmap.bm_h;

	if (VR_low_res & 1)
	{
		sh /= 2;				
		grd_curscreen->sc_aspect *= 2; // Muck with aspect ratio
	}
	if (VR_low_res & 2)
	{
		sw /= 2;				
		grd_curscreen->sc_aspect /= 2; // Muck with aspect ratio
	}

	gr_init_sub_canvas( &RenderCanvas[0], &VR_render_buffer[0], 0, 0, sw, sh );
	gr_init_sub_canvas( &RenderCanvas[1], &VR_render_buffer[1], 0, 0, sw, sh );

	// Draw the left eye's view
	if (VR_eye_switch)
	{
		actual_eye_width = -VR_eye_width;
		actual_eye_offset = -VR_eye_offset;
	}
	else
	{
		actual_eye_width = VR_eye_width;
		actual_eye_offset = VR_eye_offset;
	}

	if (Guided_missile[Player_num] &&
		Guided_missile[Player_num]->type == OBJ_WEAPON &&
		Guided_missile[Player_num]->id == GUIDEDMISS_ID &&
		Guided_missile[Player_num]->signature == Guided_missile_sig[Player_num] &&
		Guided_in_big_window.intval)
		actual_eye_offset = 0;

	gr_set_current_canvas(&RenderCanvas[0]);

	if (Guided_missile[Player_num] &&
		Guided_missile[Player_num]->type == OBJ_WEAPON &&
		Guided_missile[Player_num]->id == GUIDEDMISS_ID &&
		Guided_missile[Player_num]->signature == Guided_missile_sig[Player_num] &&
		Guided_in_big_window.intval)
	{
		char *msg = "Guided Missile View";
		object *viewer_save = Viewer;
		int w, h, aw;

		Viewer = Guided_missile[Player_num];

  		{
			update_rendered_data(0, Viewer, 0, 0);
			render_frame(0, 0);

			wake_up_rendered_objects(Viewer, 0);
			Viewer = viewer_save;

			gr_set_curfont( GAME_FONT );
			gr_set_fontcolor( gr_getcolor(27,0,0), -1 );
			gr_get_string_size( msg, &w, &h, &aw );

			gr_printf( (grd_curcanv->cv_bitmap.bm_w-w)/2, 3, msg );

			draw_guided_crosshair();
		}

		HUD_render_message_frame();

		no_draw_hud = 1;
	}
	else if (Rear_view)
		render_frame(actual_eye_width, 0);  // switch eye positions for rear view
	else
		render_frame(-actual_eye_width, 0); // Left eye

	if ( VR_low_res )
		game_expand_bitmap( &RenderCanvas[0].cv_bitmap, VR_low_res );

	{	// render small window into left eye's canvas
		grs_canvas *save = grd_curcanv;
		fix save_aspect2 = grd_curscreen->sc_aspect;

		grd_curscreen->sc_aspect = save_aspect*2;
		SW_drawn[0] = SW_drawn[1] = 0;
		show_extra_views();
		gr_set_current_canvas(save);
		grd_curscreen->sc_aspect = save_aspect2;
	}

// NEWVR
	if (actual_eye_offset > 0 )
	{
		gr_setcolor( gr_getcolor(0,0,0) );
		gr_rect( grd_curcanv->cv_bitmap.bm_w-labs(actual_eye_offset)*2, 0, grd_curcanv->cv_bitmap.bm_w-1, grd_curcanv->cv_bitmap.bm_h );
	}
	else if (actual_eye_offset < 0 )
	{
		gr_setcolor( gr_getcolor(0,0,0) );
		gr_rect( 0, 0, labs(actual_eye_offset)*2-1, grd_curcanv->cv_bitmap.bm_h );
	}

	if ( VR_show_hud && !no_draw_hud )
	{
		grs_canvas tmp;

		if (actual_eye_offset < 0 )
		{
			gr_init_sub_canvas( &tmp, grd_curcanv, labs(actual_eye_offset*2), 0, grd_curcanv->cv_bitmap.bm_w-(labs(actual_eye_offset)*2), grd_curcanv->cv_bitmap.bm_h );
		}
		else
		{
			gr_init_sub_canvas( &tmp, grd_curcanv, 0, 0, grd_curcanv->cv_bitmap.bm_w-(labs(actual_eye_offset)*2), grd_curcanv->cv_bitmap.bm_h );
		}
		gr_set_current_canvas( &tmp );
		game_draw_hud_stuff();
	}

	// Draw the right eye's view
	gr_set_current_canvas(&RenderCanvas[1]);

	if (Guided_missile[Player_num] &&
		Guided_missile[Player_num]->type == OBJ_WEAPON &&
		Guided_missile[Player_num]->id == GUIDEDMISS_ID &&
		Guided_missile[Player_num]->signature == Guided_missile_sig[Player_num] &&
		Guided_in_big_window.intval)
		gr_bitmap(0, 0, &RenderCanvas[0].cv_bitmap);
	else
	{
		if (Rear_view)
			render_frame(-actual_eye_width, 0); // switch eye positions for rear view
		else
			render_frame(actual_eye_width, 0);  // Right eye

		if ( VR_low_res )
			game_expand_bitmap( &RenderCanvas[1].cv_bitmap, VR_low_res );
	}

	{	// copy small window from left eye
		grs_canvas temp;
		int w;

		for (w = 0; w < 2; w++)
		{
			if (SW_drawn[w])
			{
				gr_init_sub_canvas(&temp, &RenderCanvas[0], SW_x[w], SW_y[w], SW_w[w], SW_h[w]);
				gr_bitmap(SW_x[w]+actual_eye_offset*2, SW_y[w], &temp.cv_bitmap);
			}
		}
	}

// NEWVR
	if (actual_eye_offset > 0)
	{
		gr_setcolor( gr_getcolor(0,0,0) );
		gr_rect( 0, 0, labs(actual_eye_offset)*2-1, grd_curcanv->cv_bitmap.bm_h );
	}
	else if ( actual_eye_offset < 0 )
	{
		gr_setcolor( gr_getcolor(0,0,0) );
		gr_rect( grd_curcanv->cv_bitmap.bm_w-labs(actual_eye_offset)*2, 0, grd_curcanv->cv_bitmap.bm_w-1, grd_curcanv->cv_bitmap.bm_h );
	}

// NEWVR (Add the next 2 lines)
	if ( VR_show_hud && !no_draw_hud )
	{
		grs_canvas tmp;

		if ( actual_eye_offset > 0 )
		{
			gr_init_sub_canvas( &tmp, grd_curcanv, labs(actual_eye_offset*2), 0, grd_curcanv->cv_bitmap.bm_w-(labs(actual_eye_offset)*2), grd_curcanv->cv_bitmap.bm_h );
		}
		else
		{
			gr_init_sub_canvas( &tmp, grd_curcanv, 0, 0, grd_curcanv->cv_bitmap.bm_w-(labs(actual_eye_offset)*2), grd_curcanv->cv_bitmap.bm_h );
		}
		gr_set_current_canvas( &tmp );
		game_draw_hud_stuff();
	}

	// Draws white and black registration encoding lines
	// and Accounts for pixel-shift adjustment in upcoming bitblts
	if (VR_use_reg_code)
	{
		int width, height, quarter;

		width = RenderCanvas[0].cv_bitmap.bm_w;
		height = RenderCanvas[0].cv_bitmap.bm_h;
		quarter = width / 4;

		// black out left-hand side of left page

		// draw registration code for left eye
		if ( VR_eye_switch )
			gr_set_current_canvas( &RenderCanvas[1] );
		else
			gr_set_current_canvas( &RenderCanvas[0] );
		gr_setcolor( VR_WHITE_INDEX );
		gr_scanline( 0, quarter, height-1 );
		gr_setcolor( VR_BLACK_INDEX );
		gr_scanline( quarter, width-1, height-1 );

		if ( VR_eye_switch )
			gr_set_current_canvas( &RenderCanvas[0] );
		else
			gr_set_current_canvas( &RenderCanvas[1] );
		gr_setcolor( VR_WHITE_INDEX );
		gr_scanline( 0, quarter*3, height-1 );
		gr_setcolor( VR_BLACK_INDEX );
		gr_scanline( quarter*3, width-1, height-1 );
   }

	// Copy left eye, then right eye
	if ( VR_screen_flags&VRF_USE_PAGING )
		VR_current_page = !VR_current_page;
	else 
		VR_current_page = 0;
	gr_set_current_canvas( &VR_screen_pages[VR_current_page] );

// NEWVR

	if ( VR_eye_offset_changed > 0 )
	{
		VR_eye_offset_changed--;
		gr_clear_canvas(0);
	}

	sw = dw = VR_render_buffer[0].cv_bitmap.bm_w;
	sh = dh = VR_render_buffer[0].cv_bitmap.bm_h;

	// Copy left eye, then right eye
	gr_bitblt_dest_step_shift = 1; // Skip every other scanline.

	if ( VR_render_mode == VR_INTERLACED )
	{
		if ( actual_eye_offset > 0 )
		{
			int xoff = labs(actual_eye_offset);
			gr_bm_ubitblt( dw-xoff, dh, xoff, 0, 0, 0, &RenderCanvas[0].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
			gr_bm_ubitblt( dw-xoff, dh, 0, 1, xoff, 0, &RenderCanvas[1].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
		}
		else if ( actual_eye_offset < 0 )
		{
			int xoff = labs(actual_eye_offset);

			gr_bm_ubitblt( dw-xoff, dh, 0, 0, xoff, 0, &RenderCanvas[0].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
			gr_bm_ubitblt( dw-xoff, dh, xoff, 1, 0, 0, &RenderCanvas[1].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
		}
		else
		{
			gr_bm_ubitblt( dw, dh, 0, 0, 0, 0, &RenderCanvas[0].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
			gr_bm_ubitblt( dw, dh, 0, 1, 0, 0, &RenderCanvas[1].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
		}
	}
	else if (VR_render_mode == VR_AREA_DET)
	{
		// VFX copy
		gr_bm_ubitblt( dw, dh, 0,  VR_current_page, 0, 0, &RenderCanvas[0].cv_bitmap, &VR_screen_pages[0].cv_bitmap );
		gr_bm_ubitblt( dw, dh, dw, VR_current_page, 0, 0, &RenderCanvas[1].cv_bitmap, &VR_screen_pages[0].cv_bitmap );
	}
	else
	{
		Int3(); // Huh?
	}

	gr_bitblt_dest_step_shift = 0;

//	if ( Game_vfx_flag )
//		vfx_set_page(VR_current_page);		// 0 or 1
//	else
	if ( VR_screen_flags&VRF_USE_PAGING )
	{
		gr_wait_for_retrace = 0;

// Added by Samir from John's code
		if ( (VR_screen_pages[VR_current_page].cv_bitmap.bm_type == BM_MODEX) && (Game_3dmax_flag == 3) )
		{
			int old_x, old_y, new_x;

			old_x = VR_screen_pages[VR_current_page].cv_bitmap.bm_x;
			old_y = VR_screen_pages[VR_current_page].cv_bitmap.bm_y;
			new_x = old_y*VR_screen_pages[VR_current_page].cv_bitmap.bm_rowsize;
			new_x += old_x/4;
			VR_screen_pages[VR_current_page].cv_bitmap.bm_x = new_x;
			VR_screen_pages[VR_current_page].cv_bitmap.bm_y = 0;
			VR_screen_pages[VR_current_page].cv_bitmap.bm_type = BM_SVGA;
			gr_show_canvas( &VR_screen_pages[VR_current_page] );
			VR_screen_pages[VR_current_page].cv_bitmap.bm_type = BM_MODEX;
			VR_screen_pages[VR_current_page].cv_bitmap.bm_x = old_x;
			VR_screen_pages[VR_current_page].cv_bitmap.bm_y = old_y;
		}
		else
			gr_show_canvas( &VR_screen_pages[VR_current_page] );
		gr_wait_for_retrace = 1;
	}
	grd_curscreen->sc_aspect = save_aspect;
}
#endif


ubyte RenderingType = 0;
ubyte DemoDoingRight = 0, DemoDoingLeft = 0;
extern ubyte DemoDoRight, DemoDoLeft;
extern object DemoRightExtra, DemoLeftExtra;

char DemoWBUType[] = { 0, WBU_MISSILE, WBU_MISSILE, WBU_REAR, WBU_ESCORT, WBU_MARKER, WBU_MISSILE };
char DemoRearCheck[] = { 0, 0, 0, 1, 0, 0, 0 };
static const char *const DemoExtraMessage[] = { "PLAYER", "GUIDED", "MISSILE", "REAR", "GUIDE-BOT", "MARKER", "SHIP" };

extern char guidebot_name[];

void show_extra_views()
{
	int did_missile_view = 0;
	int save_newdemo_state = Newdemo_state;
	int w;

	if (Newdemo_state == ND_STATE_PLAYBACK)
	{
		if (DemoDoLeft)
		{
			DemoDoingLeft = DemoDoLeft;

			if (DemoDoLeft == 3)
				do_cockpit_window_view(0, ConsoleObject, 1, WBU_REAR, "REAR");
			else
				do_cockpit_window_view(0, &DemoLeftExtra, DemoRearCheck[DemoDoLeft], DemoWBUType[DemoDoLeft], DemoExtraMessage[DemoDoLeft]);
		}
		else
			do_cockpit_window_view(0, NULL, 0, WBU_WEAPON, NULL);

		if (DemoDoRight)
		{
			DemoDoingRight = DemoDoRight;

			if (DemoDoRight == 3)
				do_cockpit_window_view(1, ConsoleObject, 1, WBU_REAR, "REAR");
			else
				do_cockpit_window_view(1, &DemoRightExtra, DemoRearCheck[DemoDoRight], DemoWBUType[DemoDoRight], DemoExtraMessage[DemoDoRight]);
		}
		else
			do_cockpit_window_view(1, NULL, 0, WBU_WEAPON, NULL);

		DemoDoLeft = DemoDoRight = 0;
		DemoDoingLeft = DemoDoingRight = 0;

		return;
	}

	if (Guided_missile[Player_num] &&
		Guided_missile[Player_num]->type == OBJ_WEAPON &&
		Guided_missile[Player_num]->id == GUIDEDMISS_ID &&
		Guided_missile[Player_num]->signature == Guided_missile_sig[Player_num])
	{
		if (Guided_in_big_window.intval)
		{
			RenderingType = 6 + (1<<4);
			do_cockpit_window_view(1, Viewer, 0, WBU_MISSILE, "SHIP");
		}
		else
		{
			RenderingType = 1 + (1<<4);
			do_cockpit_window_view(1, Guided_missile[Player_num], 0, WBU_GUIDED, "GUIDED");
	    }

		did_missile_view = 1;
	}
	else
	{
		if (Guided_missile[Player_num]) // used to be active
		{
			if (!Guided_in_big_window.intval)
				do_cockpit_window_view(1, NULL, 0, WBU_STATIC, NULL);
			Guided_missile[Player_num] = NULL;
		}

		if (Missile_viewer) // do missile view
		{
			static int mv_sig = -1;

			if (mv_sig == -1)
				mv_sig = Missile_viewer->signature;
			if (Missile_view_enabled.intval &&
				Missile_viewer->type != OBJ_NONE &&
				Missile_viewer->signature == mv_sig)
			{
				RenderingType = 2 + (1<<4);
				do_cockpit_window_view(1, Missile_viewer, 0, WBU_MISSILE, "MISSILE");
				did_missile_view = 1;
			}
			else
			{
				Missile_viewer = NULL;
				mv_sig = -1;
				RenderingType = 255;
				do_cockpit_window_view(1, NULL, 0, WBU_STATIC, NULL);
			}
		}
	}

	for (w = 0; w < 2; w++)
	{
		if ( w == 1 && did_missile_view )
			continue; // if showing missile view in right window, can't show anything else

		// show special views if selected
		switch (Cockpit_3d_view[w].intval)
		{
			case CV_NONE:
				RenderingType = 255;
				do_cockpit_window_view(w, NULL, 0, WBU_WEAPON, NULL);
				break;
			case CV_REAR:
				if (Rear_view) // if big window is rear view, show front here
				{
					RenderingType = 3 + (w<<4);
					do_cockpit_window_view(w, ConsoleObject, 0, WBU_REAR, "FRONT");
				}
				else // show normal rear view
				{
					RenderingType = 3 + (w<<4);
					do_cockpit_window_view(w, ConsoleObject, 1, WBU_REAR, "REAR");
				}
			 	break;
			case CV_ESCORT:
			{
				object *buddy;
				buddy = find_escort();
				if (buddy == NULL)
				{
					do_cockpit_window_view(w, NULL, 0, WBU_WEAPON, NULL);
					cvar_setint(&Cockpit_3d_view[w], CV_NONE);
				}
				else
				{
					RenderingType = 4 + (w<<4);
					do_cockpit_window_view(w, buddy, 0, WBU_ESCORT, guidebot_name);
				}
				break;
			}
#ifdef NETWORK
			case CV_COOP:
			{
				int player = Coop_view_player[w];

				RenderingType = 255; // don't handle coop stuff

				if (player != -1 &&
					Players[player].connected &&
					((Game_mode & GM_MULTI_COOP) || ((Game_mode & GM_TEAM) && (get_team(player) == get_team(Player_num)))))
					do_cockpit_window_view(w, &Objects[Players[Coop_view_player[w]].objnum], 0, WBU_COOP, Players[Coop_view_player[w]].callsign);
				else
				{
					do_cockpit_window_view(w, NULL, 0, WBU_WEAPON, NULL);
					cvar_setint(&Cockpit_3d_view[w], CV_NONE);
				}
				break;
			}
#endif
			case CV_MARKER:
			{
				char label[10];

				RenderingType = 5 + (w<<4);
				if ( Marker_viewer_num[w] == -1 || MarkerObject[Marker_viewer_num[w]] == -1 )
				{
					cvar_setint(&Cockpit_3d_view[w], CV_NONE);
					break;
				}
				snprintf(label, sizeof(label)-1, "Marker %d", Marker_viewer_num[w] + 1);
				do_cockpit_window_view(w, &Objects[MarkerObject[Marker_viewer_num[w]]], 0, WBU_MARKER, label);
				break;
			}
			default:
				Int3(); // invalid window type
		}
	}
	RenderingType = 0;
	Newdemo_state = save_newdemo_state;
}


int BigWindowSwitch = 0;
extern int force_cockpit_redraw;

void draw_guided_crosshair(void);
void update_cockpits(int force_redraw);


// render a frame for the game
void game_render_frame_mono(void)
{
	grs_canvas Screen_3d_window;
	int no_draw_hud = 0;

	gr_init_sub_canvas( &Screen_3d_window, &VR_screen_pages[0],
		VR_render_sub_buffer[0].cv_bitmap.bm_x, 
		VR_render_sub_buffer[0].cv_bitmap.bm_y, 
		VR_render_sub_buffer[0].cv_bitmap.bm_w, 
		VR_render_sub_buffer[0].cv_bitmap.bm_h);

	if ( Game_double_buffer )
		gr_set_current_canvas(&VR_render_sub_buffer[0]);
	else
		gr_set_current_canvas(&Screen_3d_window);

	if (Guided_missile[Player_num] &&
		Guided_missile[Player_num]->type == OBJ_WEAPON &&
		Guided_missile[Player_num]->id == GUIDEDMISS_ID &&
		Guided_missile[Player_num]->signature == Guided_missile_sig[Player_num] &&
		Guided_in_big_window.intval)
	{
		char *msg = "Guided Missile View";
		object *viewer_save = Viewer;
		int w, h, aw;

		if (Cockpit_mode.intval == CM_FULL_COCKPIT)
		{
			BigWindowSwitch = 1;
			force_cockpit_redraw = 1;
			cvar_setint(&Cockpit_mode, CM_STATUS_BAR);
			return;
		}

		Viewer = Guided_missile[Player_num];

  		{
			update_rendered_data(0, Viewer, 0, 0);
			render_frame(0, 0);
  
			wake_up_rendered_objects(Viewer, 0);
			Viewer = viewer_save;

			gr_set_curfont( GAME_FONT );    //GAME_FONT );
			gr_set_fontcolor( gr_getcolor(27,0,0), -1 );
			gr_get_string_size( msg, &w, &h, &aw );

			gr_printf( (grd_curcanv->cv_bitmap.bm_w-w)/2, 3, msg );

			draw_guided_crosshair();
		}

		HUD_render_message_frame();

		no_draw_hud = 1;
	}
	else
	{
		if (BigWindowSwitch)
		{
			force_cockpit_redraw = 1;
			cvar_setint(&Cockpit_mode, CM_FULL_COCKPIT);
			BigWindowSwitch = 0;
			return;
		}
		update_rendered_data(0, Viewer, Rear_view, 0);
		render_frame(0, 0);
	}

	if ( Game_double_buffer )
		gr_set_current_canvas(&VR_render_sub_buffer[0]);
	else
		gr_set_current_canvas(&Screen_3d_window);

	if (!no_draw_hud)
		game_draw_hud_stuff();

	if (Game_paused) // render pause message over off-screen 3d (to minimize flicker)
	{
		extern char *Pause_msg;
		ubyte *save_data = VR_screen_pages[VR_current_page].cv_bitmap.bm_data;

		VR_screen_pages[VR_current_page].cv_bitmap.bm_data=VR_render_buffer[VR_current_page].cv_bitmap.bm_data;
		show_boxed_message(Pause_msg);
		VR_screen_pages[VR_current_page].cv_bitmap.bm_data=save_data;
	}

	if ( Game_double_buffer ) // copy to visible screen
	{
//		if ( !Game_cockpit_copy_code )
//		{
			if ( VR_screen_flags&VRF_USE_PAGING )
			{
				VR_current_page = !VR_current_page;
				gr_set_current_canvas( &VR_screen_pages[VR_current_page] );
				gr_bm_ubitblt( VR_render_sub_buffer[0].cv_w, VR_render_sub_buffer[0].cv_h, VR_render_sub_buffer[0].cv_bitmap.bm_x, VR_render_sub_buffer[0].cv_bitmap.bm_y, 0, 0, &VR_render_sub_buffer[0].cv_bitmap, &VR_screen_pages[VR_current_page].cv_bitmap );
				gr_wait_for_retrace = 0;
				gr_show_canvas( &VR_screen_pages[VR_current_page] );
				gr_wait_for_retrace = 1;
			}
			else
			{
				gr_bm_ubitblt( VR_render_sub_buffer[0].cv_w, 
						VR_render_sub_buffer[0].cv_h, 
						VR_render_sub_buffer[0].cv_bitmap.bm_x, 
						VR_render_sub_buffer[0].cv_bitmap.bm_y, 
						0, 0, 
						&VR_render_sub_buffer[0].cv_bitmap, 
						&VR_screen_pages[0].cv_bitmap );
			}
//		}
#if 0
		else
		{
#ifdef __MSDOS__
				gr_ibitblt( &VR_render_buffer[0].cv_bitmap, &VR_screen_pages[0].cv_bitmap, Game_cockpit_copy_code );
#else //def MACINTOSH
				gr_ibitblt( &VR_render_sub_buffer[0].cv_bitmap, &VR_screen_pages[0].cv_bitmap );
#endif
		}
#endif
	}

	update_cockpits(0);

	show_extra_views(); // missile view, buddy bot, etc.

	if ( Cockpit_mode.intval == CM_FULL_COCKPIT || Cockpit_mode.intval == CM_STATUS_BAR )
	{
		if ( Newdemo_state == ND_STATE_PLAYBACK )
			Game_mode = Newdemo_game_mode;

		render_gauges();

		if ( Newdemo_state == ND_STATE_PLAYBACK )
			Game_mode = GM_NORMAL;
	}

	con_draw();

	vid_update();
#ifdef OGL
	ogl_swap_buffers();
#endif
}


void toggle_cockpit(void)
{
	int new_mode;

	switch (Cockpit_mode.intval)
	{
		case CM_FULL_COCKPIT:
		{
			int max_h;
			if (max_window_w <= GameBitmaps[cockpit_bitmap[CM_STATUS_BAR+(SM_HIRES?(Num_cockpits/2):0)].index].bm_w)
				max_h = grd_curscreen->sc_h - GameBitmaps[cockpit_bitmap[CM_STATUS_BAR + (SM_HIRES?(Num_cockpits/2):0)].index].bm_h;
			else
				max_h = grd_curscreen->sc_h;

			if (Game_window_h.intval > max_h) //too big for statusbar
				new_mode = CM_FULL_SCREEN;
			else
				new_mode = CM_STATUS_BAR;
			break;
		}

		case CM_STATUS_BAR:
		case CM_FULL_SCREEN:
			if (Rear_view)
				return;
			new_mode = CM_FULL_COCKPIT;
			break;

		case CM_REAR_VIEW:
		case CM_LETTERBOX:
	        default:
			return; // do nothing
			break;

	}

	select_cockpit(new_mode);
	HUD_clear_messages();
	WriteConfigFile();
}


#ifndef MACINTOSH
#define WINDOW_W_DELTA  ((max_window_w / 16)&~1)   //24   //20
#define WINDOW_H_DELTA  ((max_window_h / 16)&~1)   //12   //10

#define WINDOW_MIN_W    ((max_window_w * 10) / 22)	  //160
#define WINDOW_MIN_H    ((max_window_h * 10) / 22)
#else
//#define WINDOW_W_DELTA    32
//#define WINDOW_H_DELTA    16

#define WINDOW_W_DELTA  ((max_window_w / 16) & ~15) // double word aligned
#define WINDOW_H_DELTA  ((max_window_h / 16) & ~15) // double word aligned

#define WINDOW_MIN_W    (max_window_w-(WINDOW_W_DELTA*11))
#define WINDOW_MIN_H    (max_window_h-(WINDOW_H_DELTA*11))
#endif


void grow_window(void)
{
	if (Cockpit_mode.intval == CM_FULL_COCKPIT)
	{
		cvar_setint(&Game_window_h, max_window_h);
		cvar_setint(&Game_window_w, max_window_w);
		toggle_cockpit();
		HUD_init_message("Press F3 to return to Cockpit mode");
		return;
	}

	if (Cockpit_mode.intval != CM_STATUS_BAR && (VR_screen_flags & VRF_ALLOW_COCKPIT))
		return;

	if (Game_window_h.intval >= max_window_h || Game_window_w.intval >= max_window_w)
	{
		//cvar_setint(&Game_window_w, max_window_w);
		//cvar_setint(&Game_window_h, max_window_h);
		select_cockpit(CM_FULL_SCREEN);
	}
	else
	{
		//int x,y;

		cvar_setint(&Game_window_w, Game_window_w.intval + WINDOW_W_DELTA);
		cvar_setint(&Game_window_h, Game_window_h.intval + WINDOW_H_DELTA);

		if (Game_window_h.intval > max_window_h)
			cvar_setint(&Game_window_h, max_window_h);

		if (Game_window_w.intval > max_window_w)
			cvar_setint(&Game_window_w, max_window_w);

		Game_window_x = (max_window_w - Game_window_w.intval) / 2;
		Game_window_y = (max_window_h - Game_window_h.intval) / 2;

		game_init_render_sub_buffers( Game_window_x, Game_window_y, Game_window_w.intval, Game_window_h.intval );
	}

	HUD_clear_messages(); // @mk, 11/11/94

	WriteConfigFile();
}


extern grs_bitmap background_bitmap;

void copy_background_rect(int left,int top,int right,int bot)
{
	grs_bitmap *bm = &background_bitmap;
	int x, y;
	int tile_left, tile_right, tile_top, tile_bot;
	int ofs_x, ofs_y;
	int dest_x, dest_y;

	if (right < left || bot < top)
		return;

	tile_left = left / bm->bm_w;
	tile_right = right / bm->bm_w;
	tile_top = top / bm->bm_h;
	tile_bot = bot / bm->bm_h;

	ofs_y = top % bm->bm_h;
	dest_y = top;

	for (y = tile_top; y <= tile_bot; y++)
	{
		int w, h;

		ofs_x = left % bm->bm_w;
		dest_x = left;

		//h = (bot < dest_y+bm->bm_h)?(bot-dest_y+1):(bm->bm_h-ofs_y);
		h = min(bot-dest_y+1,bm->bm_h-ofs_y);

		for (x = tile_left; x <= tile_right; x++)
		{
			//w = (right < dest_x+bm->bm_w)?(right-dest_x+1):(bm->bm_w-ofs_x);
			w = min(right-dest_x+1, bm->bm_w-ofs_x);

			gr_bm_ubitblt(w, h, dest_x, dest_y, ofs_x, ofs_y, &background_bitmap, &grd_curcanv->cv_bitmap);

			ofs_x = 0;
			dest_x += w;
		}

		ofs_y = 0;
		dest_y += h;
	}
}


// fills int the background surrounding the 3d window
void fill_background(void)
{
	int x, y, w, h, dx, dy;

	x = Game_window_x;
	y = Game_window_y;
	w = Game_window_w.intval;
	h = Game_window_h.intval;

	dx = x;
	dy = y;

	gr_set_current_canvas(&VR_screen_pages[VR_current_page]);
	copy_background_rect(x-dx, y-dy, x-1, y+h+dy-1);
	copy_background_rect(x+w, y-dy, grd_curcanv->cv_w-1, y+h+dy-1);
	copy_background_rect(x, y-dy, x+w-1, y-1);
	copy_background_rect(x, y+h, x+w-1, y+h+dy-1);

	if (VR_screen_flags & VRF_USE_PAGING)
	{
		gr_set_current_canvas(&VR_screen_pages[!VR_current_page]);
		copy_background_rect(x-dx, y-dy, x-1, y+h+dy-1);
		copy_background_rect(x+w, y-dy, x+w+dx-1, y+h+dy-1);
		copy_background_rect(x, y-dy, x+w-1, y-1);
		copy_background_rect(x, y+h, x+w-1, y+h+dy-1);
	}
}


void shrink_window(void)
{
	mprintf((0, "%d ", FrameCount));

//	mprintf((0, "W=%d H=%d\n", Game_window_w.intval, Game_window_h.intval));

	if ( Cockpit_mode.intval == CM_FULL_COCKPIT && (VR_screen_flags & VRF_ALLOW_COCKPIT) )
	{
		cvar_setint(&Game_window_h, max_window_h);
		cvar_setint(&Game_window_w, max_window_w);
		//!!toggle_cockpit();
		select_cockpit(CM_STATUS_BAR);
//		shrink_window();
//		shrink_window();
		HUD_init_message("Press F3 to return to Cockpit mode");
		WriteConfigFile();
		return;
	}

	if (Cockpit_mode.intval == CM_FULL_SCREEN)
	{
		//cvar_setint(&Game_window_w, max_window_w);
		//cvar_setint(&Game_window_h, max_window_h);
		select_cockpit(CM_STATUS_BAR);
		WriteConfigFile();
		return;
	}

	if (Cockpit_mode.intval != CM_STATUS_BAR && (VR_screen_flags & VRF_ALLOW_COCKPIT))
		return;

	mprintf((0, "Cockpit mode=%d\n", Cockpit_mode.intval));

	if (Game_window_w.intval > WINDOW_MIN_W) {
		//int x, y;

		cvar_setint(&Game_window_w, Game_window_w.intval - WINDOW_W_DELTA);
		cvar_setint(&Game_window_h, Game_window_h.intval - WINDOW_H_DELTA);

		mprintf((0, "NewW=%d NewH=%d VW=%d maxH=%d\n", Game_window_w.intval, Game_window_h.intval, max_window_w, max_window_h));
                  
		if ( Game_window_w.intval < WINDOW_MIN_W )
			cvar_setint(&Game_window_w, WINDOW_MIN_W);

		if ( Game_window_h.intval < WINDOW_MIN_H )
			cvar_setint(&Game_window_h, WINDOW_MIN_H);
			
		Game_window_x = (max_window_w - Game_window_w.intval) / 2;
		Game_window_y = (max_window_h - Game_window_h.intval) / 2;

		fill_background();

		game_init_render_sub_buffers( Game_window_x, Game_window_y, Game_window_w.intval, Game_window_h.intval );
		HUD_clear_messages();
		WriteConfigFile();
	}

}

int last_drawn_cockpit[2] = { -1, -1 };
extern void ogl_loadbmtexture(grs_bitmap *bm); 

// This actually renders the new cockpit onto the screen.
void update_cockpits(int force_redraw)
{
	int x, y; //w, h;
	bitmap_index bm_idx;
	grs_bitmap *bm;

	// Redraw the on-screen cockpit bitmaps
	if (VR_render_mode != VR_NONE )
		return;

	if (Cockpit_mode.intval != last_drawn_cockpit[VR_current_page])
		init_cockpit();

	bm_idx = cockpit_bitmap[Cockpit_mode.intval + (SM_HIRES?(Num_cockpits/2):0)];
	bm = &GameBitmaps[bm_idx.index];

	switch( Cockpit_mode.intval )
	{
	case CM_FULL_COCKPIT:
	case CM_REAR_VIEW:
		gr_set_current_canvas(&VR_screen_pages[VR_current_page]);
		PIGGY_PAGE_IN(bm_idx);
		gr_ubitmapm(0, 0, bm);
		break;

	case CM_FULL_SCREEN:
		Game_window_x = (max_window_w - Game_window_w.intval) / 2;
		Game_window_y = (max_window_h - Game_window_h.intval) / 2;
		fill_background();
		break;

	case CM_STATUS_BAR:

		gr_set_current_canvas(&VR_screen_pages[VR_current_page]);

		PIGGY_PAGE_IN(bm_idx);
		x = (max_window_w - bm->bm_w) / 2;
		if (max_window_w == bm->bm_w)
			y = max_window_h;
		else
			y = max_window_h - bm->bm_h;
		Game_window_x = (max_window_w - Game_window_w.intval) / 2;
		Game_window_y = (max_window_h - Game_window_h.intval) / 2;
		fill_background();
		gr_ubitmapm(x, y, bm);
		break;

	case CM_LETTERBOX:
		gr_set_current_canvas(&VR_screen_pages[VR_current_page]);

		// Clear the top and bottom of the screen
		gr_setcolor(BM_XRGB(0,0,0));
		gr_rect(0,
			VR_render_sub_buffer[VR_current_page].cv_bitmap.bm_y + VR_render_sub_buffer[VR_current_page].cv_bitmap.bm_h,
			GWIDTH-1,GHEIGHT-1);
		gr_rect(0, 0, GWIDTH-1, VR_render_sub_buffer[VR_current_page].cv_bitmap.bm_y);

		// In a modex mode, clear the other buffer.
		if (grd_curcanv->cv_bitmap.bm_type == BM_MODEX)
		{
			gr_set_current_canvas(&VR_screen_pages[VR_current_page^1]);
			gr_clear_canvas( BM_XRGB(0,0,0) );
			gr_set_current_canvas(&VR_screen_pages[VR_current_page]);
		}
		break;

	}

	gr_set_current_canvas(&VR_screen_pages[VR_current_page]);

	if (Cockpit_mode.intval != last_drawn_cockpit[VR_current_page] || force_redraw )
		last_drawn_cockpit[VR_current_page] = Cockpit_mode.intval;
	else
		return;

	if ( Cockpit_mode.intval == CM_FULL_COCKPIT || Cockpit_mode.intval == CM_STATUS_BAR )
		init_gauges();
}


void game_render_frame(void)
{
	set_screen_mode( SCREEN_GAME );

//	update_cockpits(0);

	play_homing_warning();

	if (VR_render_mode == VR_NONE )
		game_render_frame_mono();	 
/*	else
		game_render_frame_stereo();	 
		*/

	// Make sure palette is faded in
	stop_time();
	gr_palette_fade_in( gr_palette, 32, 0 );
	start_time();

	FrameCount++;
}


extern int Color_0_31_0;


// draw a crosshair for the guided missile
void draw_guided_crosshair(void)
{
	int x, y, w, h;

	gr_setcolor(Color_0_31_0);

	w = grd_curcanv->cv_w>>5;
	if (w < 5)
		w = 5;

	h = i2f(w) / grd_curscreen->sc_aspect;

	x = grd_curcanv->cv_w / 2;
	y = grd_curcanv->cv_h / 2;

	gr_scanline(x-w/2, x+w/2, y);
	gr_uline(i2f(x), i2f(y-h/2), i2f(x), i2f(y+h/2));

}


typedef struct bkg {
	short x, y, w, h;   // The location of the menu.
	grs_bitmap * bmp;   // The background under the menu.
} bkg;

bkg bg = { 0, 0, 0, 0, NULL };

#define BOX_BORDER (MenuHires?60:30)


// show a message in a nice little box
void show_boxed_message(char *msg)
{
	int w, h, aw;
	int x, y;

	gr_set_current_canvas(&VR_screen_pages[VR_current_page]);
	gr_set_curfont( MEDIUM1_FONT );

	gr_get_string_size(msg, &w, &h, &aw);

	x = (grd_curscreen->sc_w-w)/2;
	y = (grd_curscreen->sc_h-h)/2;

	if (bg.bmp)
	{
		gr_free_bitmap(bg.bmp);
		bg.bmp = NULL;
	}

	// Save the background of the display
	bg.x=x; bg.y=y; bg.w=w; bg.h=h;

	bg.bmp = gr_create_bitmap( w+BOX_BORDER, h+BOX_BORDER );

	gr_bm_ubitblt( w+BOX_BORDER, h+BOX_BORDER, 0, 0, x-BOX_BORDER/2, y-BOX_BORDER/2, &(grd_curcanv->cv_bitmap), bg.bmp );

	nm_draw_background(x-BOX_BORDER/2, y-BOX_BORDER/2, x+w+BOX_BORDER/2-1, y+h+BOX_BORDER/2-1);

	gr_set_fontcolor( gr_getcolor(31, 31, 31), -1 );

	gr_ustring( 0x8000, y, msg );
}


void clear_boxed_message(void)
{
	if (bg.bmp)
	{
		gr_bitmap(bg.x-BOX_BORDER/2, bg.y-BOX_BORDER/2, bg.bmp);

		gr_free_bitmap(bg.bmp);
		bg.bmp = NULL;
	}
}
