/*
 *
 * OGL video functions. - Added 9/15/99 Matthew Mueller
 *
 *
 */

#ifdef HAVE_CONFIG_H
#include <conf.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _MSC_VER
#include <windows.h>
#endif

#if !defined(_MSC_VER) && !defined(macintosh)
#include <unistd.h>
#endif
#if !defined(macintosh)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include <errno.h>

#include "inferno.h"
#include "gr.h"
#include "vid.h"
#include "u_mem.h"
#include "error.h"
#include "timer.h"
#include "strutil.h"
#include "mono.h"
#include "args.h"
#include "key.h"
#include "physfsx.h"

#define DECLARE_VARS
#include "internal.h"
#if defined(__APPLE__) && defined(__MACH__)
#include <OpenGL/glu.h>
#undef GL_ARB_multitexture // hack!
#else
#include <GL/glu.h>
#endif


int ogl_voodoohack=0;

int vid_installed = 0;


void gr_palette_clear(); // Function prototype for gr_init;
int gl_initialized=0;
int gl_reticle = 0;

int ogl_fullscreen=0;


int vid_check_fullscreen(void)
{
	return ogl_fullscreen;
}


static void do_fullscreen(int f)
{
	if (ogl_voodoohack)
		ogl_fullscreen=1;//force fullscreen mode on voodoos.
	else
		ogl_fullscreen=f;
	if (gl_initialized){
		ogl_do_fullscreen_internal();
	}
}


int vid_toggle_fullscreen(void)
{
	do_fullscreen(!ogl_fullscreen);
	//	grd_curscreen->sc_mode=0;//hack to get it to reset screen mode
	return ogl_fullscreen;
}


int vid_toggle_fullscreen_menu(void)
{
	unsigned char *buf=NULL;

	if (ogl_readpixels_ok){
		MALLOC(buf,unsigned char,grd_curscreen->sc_w*grd_curscreen->sc_h*3);
		glReadBuffer(GL_FRONT);
		glReadPixels(0,0,grd_curscreen->sc_w,grd_curscreen->sc_h,GL_RGB,GL_UNSIGNED_BYTE,buf);
	}

	do_fullscreen(!ogl_fullscreen);

	if (ogl_readpixels_ok){
//		glWritePixels(0,0,grd_curscreen->sc_w,grd_curscreen->sc_h,GL_RGB,GL_UNSIGNED_BYTE,buf);
		glRasterPos2f(0,0);
		glDrawPixels(grd_curscreen->sc_w,grd_curscreen->sc_h,GL_RGB,GL_UNSIGNED_BYTE,buf);
		free(buf);
	}
	//	grd_curscreen->sc_mode=0;//hack to get it to reset screen mode

	return ogl_fullscreen;
}

void ogl_init_state(void){
	/* select clearing (background) color   */
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	/* initialize viewing values */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	glScalef(1.0, -1.0, 1.0);
	glTranslatef(0.0, -1.0, 0.0);
	gr_palette_step_up(0,0,0);//in case its left over from in game
}

int last_screen_mode=-1;

void ogl_set_screen_mode(void){
	if (last_screen_mode==Screen_mode)
		return;
	OGL_VIEWPORT(0,0,grd_curscreen->sc_w,grd_curscreen->sc_h);
//	OGL_VIEWPORT(grd_curcanv->cv_bitmap.bm_x,grd_curcanv->cv_bitmap.bm_y,grd_curcanv->cv_bitmap.bm_w,grd_curcanv->cv_bitmap.bm_h);
	if (Screen_mode==SCREEN_GAME){
		glDrawBuffer(GL_BACK);
	}else{
		glClearColor(0.0, 0.0, 0.0, 0.0);
		glDrawBuffer(GL_FRONT);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();//clear matrix
		glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();//clear matrix
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	last_screen_mode=Screen_mode;
}

void vid_update()
{
	if (gl_initialized){

		if(Screen_mode != SCREEN_GAME){
			glFlush();
		}
	}
}

const char *gl_vendor, *gl_renderer, *gl_version, *gl_extensions;

void ogl_get_verinfo(void)
{
	long t, sgi_max_textures = -1;
	GLint arb_max_textures = -1;
#ifdef GL_NV_register_combiners
	long nv_register_combiners = -1;
#endif
	float anisotropic_max = 0;

	gl_vendor = (const char *) glGetString (GL_VENDOR);
	gl_renderer = (const char *) glGetString (GL_RENDERER);
	gl_version = (const char *) glGetString (GL_VERSION);
	gl_extensions = (const char *) glGetString (GL_EXTENSIONS);

	con_printf(CON_VERBOSE, "OpenGL: vendor: %s\nOpenGL: renderer: %s\nOpenGL: version: %s\n",gl_vendor,gl_renderer,gl_version);

	ogl_intensity4_ok = 1;
	ogl_luminance4_alpha4_ok = 1;
	ogl_rgba2_ok = 1;
	ogl_gettexlevelparam_ok = 1;
	ogl_setgammaramp_ok = 1;

#ifdef WGL_VIDEO
	dglMultiTexCoord2fARB = (glMultiTexCoord2fARB_fp)wglGetProcAddress("glMultiTexCoord2fARB");
	dglActiveTextureARB = (glActiveTextureARB_fp)wglGetProcAddress("glActiveTextureARB");
	dglMultiTexCoord2fSGIS = (glMultiTexCoord2fSGIS_fp)wglGetProcAddress("glMultiTexCoord2fSGIS");
	dglSelectTextureSGIS = (glSelectTextureSGIS_fp)wglGetProcAddress("glSelectTextureSGIS");
	dglColorTableEXT = (glColorTableEXT_fp)wglGetProcAddress("glColorTableEXT");
	dglCombinerParameteriNV = (glCombinerParameteriNV_fp)wglGetProcAddress("glCombinerParameteriNV");
	dglCombinerInputNV = (glCombinerInputNV_fp)wglGetProcAddress("glCombinerInputNV");
	dglCombinerOutputNV = (glCombinerOutputNV_fp)wglGetProcAddress("glCombinerOutputNV");
	dglFinalCombinerInputNV = (glFinalCombinerInputNV_fp)wglGetProcAddress("glFinalCombinerInputNV");
#endif

#ifdef GL_ARB_multitexture
	ogl_arb_multitexture_ok = (strstr(gl_extensions, "GL_ARB_multitexture") != 0 && glActiveTextureARB != 0);
	mprintf((0,"c:%p d:%p e:%p\n",strstr(gl_extensions,"GL_ARB_multitexture"),glActiveTextureARB,glBegin));
#endif
#ifdef GL_SGIS_multitexture
	ogl_sgis_multitexture_ok = (strstr(gl_extensions, "GL_SGIS_multitexture") != 0 && glSelectTextureSGIS != 0);
	mprintf((0,"a:%p b:%p\n",strstr(gl_extensions,"GL_SGIS_multitexture"),glSelectTextureSGIS));
#endif
	ogl_nv_texture_env_combine4_ok = (strstr(gl_extensions, "GL_NV_texture_env_combine4") != 0);
#ifdef GL_NV_register_combiners
	ogl_nv_register_combiners_ok=(strstr(gl_extensions,"GL_NV_register_combiners")!=0 && glCombinerOutputNV!=0);
#endif

	ogl_ext_texture_filter_anisotropic_ok = (strstr(gl_extensions, "GL_EXT_texture_filter_anisotropic") != 0);
	if (ogl_ext_texture_filter_anisotropic_ok)
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropic_max);

#ifdef GL_EXT_paletted_texture
	ogl_paletted_texture_ok = (strstr(gl_extensions, "GL_EXT_paletted_texture") != 0 && glColorTableEXT != 0);
	ogl_shared_palette_ok = (strstr(gl_extensions, "GL_EXT_shared_texture_palette") != 0 && ogl_paletted_texture_ok);
#endif
	//add driver specific hacks here.  whee.
	if ((stricmp(gl_renderer,"Mesa NVIDIA RIVA 1.0\n")==0 || stricmp(gl_renderer,"Mesa NVIDIA RIVA 1.2\n")==0) && stricmp(gl_version,"1.2 Mesa 3.0")==0){
		ogl_intensity4_ok=0;//ignores alpha, always black background instead of transparent.
		ogl_readpixels_ok=0;//either just returns all black, or kills the X server entirely
		ogl_gettexlevelparam_ok=0;//returns random data..
	}
	if (stricmp(gl_vendor,"Matrox Graphics Inc.")==0){
		//displays garbage. reported by
		//  redomen@crcwnet.com (render="Matrox G400" version="1.1.3 5.52.015")
		//  orulz (Matrox G200)
		ogl_intensity4_ok=0;
	}
#ifdef macintosh
	if (stricmp(gl_renderer,"3dfx Voodoo 3")==0){ // strangely, includes Voodoo 2
		ogl_gettexlevelparam_ok=0; // Always returns 0
	}
#endif

	//allow overriding of stuff.
#ifdef GL_ARB_multitexture
	if ((t=FindArg("-gl_arb_multitexture_ok"))){
		ogl_arb_multitexture_ok=atoi(Args[t+1]);
	}
	if (ogl_arb_multitexture_ok)
		glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &arb_max_textures);
#endif
#ifdef GL_SGIS_multitexture
	if ((t=FindArg("-gl_sgis_multitexture_ok"))){
		ogl_sgis_multitexture_ok=atoi(Args[t+1]);
	}
	if (ogl_sgis_multitexture_ok)
		glGetIntegerv(GL_MAX_TEXTURES_SGIS, &sgi_max_textures);
#endif
#ifdef GL_NV_register_combiners
	if ((t = FindArg("-gl_nv_register_combiners_ok")))
	{
		ogl_nv_register_combiners_ok=atoi(Args[t + 1]);
	}
	if (ogl_nv_register_combiners_ok)
		glGetIntegerv(GL_MAX_GENERAL_COMBINERS_NV, &nv_register_combiners);
#endif
#ifdef GL_EXT_paletted_texture
	if ((t = FindArg("-gl_paletted_texture_ok")))
	{
		ogl_paletted_texture_ok = atoi(Args[t + 1]);
	}
	if ((t = FindArg("-gl_shared_palette_ok")))
	{
		ogl_shared_palette_ok = atoi(Args[t + 1]);
	}
	ogl_shared_palette_ok = ogl_shared_palette_ok && ogl_paletted_texture_ok; // shared palettes require palette support in the first place, obviously ;)
	printf("gl_paletted_texture: %i  gl_shared_palette: %i  (using paletted textures: %i)\n", ogl_paletted_texture_ok, ogl_shared_palette_ok, ogl_shared_palette_ok);
#endif
	if ((t=FindArg("-gl_intensity4_ok"))){
		ogl_intensity4_ok=atoi(Args[t+1]);
	}
	if ((t=FindArg("-gl_luminance4_alpha4_ok"))){
		ogl_luminance4_alpha4_ok=atoi(Args[t+1]);
	}
	if ((t=FindArg("-gl_rgba2_ok"))){
		ogl_rgba2_ok=atoi(Args[t+1]);
	}
	if ((t=FindArg("-gl_readpixels_ok"))){
		ogl_readpixels_ok=atoi(Args[t+1]);
	}
	if ((t=FindArg("-gl_gettexlevelparam_ok"))){
		ogl_gettexlevelparam_ok=atoi(Args[t+1]);
	}
	if ((t=FindArg("-gl_setgammaramp_ok")))
	{
		ogl_setgammaramp_ok = atoi(Args[t + 1]);
	}

	con_printf(CON_DEBUG, "gl_arb_multitexture:%i(%i units) gl_sgis_multitexture:%i(%i units) gl_nv_texture_env_combine4:%i\n", ogl_arb_multitexture_ok, arb_max_textures, ogl_sgis_multitexture_ok, sgi_max_textures, ogl_nv_texture_env_combine4_ok);
#ifdef GL_NV_register_combiners
	con_printf(CON_DEBUG, "gl_nv_register_combiners:%i(%i stages)\n", ogl_nv_register_combiners_ok, nv_register_combiners);
#endif
	con_printf(CON_DEBUG, "gl_intensity4:%i gl_luminance4_alpha4:%i gl_rgba2:%i gl_readpixels:%i gl_gettexlevelparam:%i gl_setgammaramp_ok:%i gl_ext_texture_filter_anisotropic:%i(%f max)\n", ogl_intensity4_ok, ogl_luminance4_alpha4_ok, ogl_rgba2_ok, ogl_readpixels_ok, ogl_gettexlevelparam_ok, ogl_setgammaramp_ok, ogl_ext_texture_filter_anisotropic_ok, anisotropic_max);
}


int vid_check_mode(uint32_t mode)
{
	int w, h;

	w = SM_W(mode);
	h = SM_H(mode);
	return ogl_check_mode(w, h); // platform specific code
}


uint32_t Vid_current_mode;


int vid_set_mode(uint32_t mode)
{
	short w, h;

#ifdef NOGRAPH
return 0;
#endif
//	mode=0;
	if (mode<=0)
		return 0;

	if (mode == Vid_current_mode)
		return 0;

	w=SM_W(mode);
	h=SM_H(mode);
	Vid_current_mode = mode;

	//if (screen != NULL) gr_palette_clear();

//	ogl_init_state();

	gr_init_screen(BM_OGL, w, h, 0, 0, w, NULL);

	//gr_enable_default_palette_loading();
	
	ogl_init_window(w,h);//platform specific code

	ogl_get_verinfo();

	OGL_VIEWPORT(0,0,w,h);

	ogl_set_screen_mode();

//	gamefont_choose_game_font(w,h);
	
	return 0;
}

#define GLstrcmptestr(a,b) if (stricmp(a,#b)==0 || stricmp(a,"GL_" #b)==0)return GL_ ## b;
int ogl_atotexfilti(char *a,int min){
	GLstrcmptestr(a,NEAREST);
	GLstrcmptestr(a,LINEAR);
	if (min){//mipmaps are valid only for the min filter
		GLstrcmptestr(a,NEAREST_MIPMAP_NEAREST);
		GLstrcmptestr(a,NEAREST_MIPMAP_LINEAR);
		GLstrcmptestr(a,LINEAR_MIPMAP_NEAREST);
		GLstrcmptestr(a,LINEAR_MIPMAP_LINEAR);
	}
	Error("unknown/invalid texture filter %s\n",a);
//	return GL_NEAREST;
}
int ogl_testneedmipmaps(int i){
	switch (i){
		case GL_NEAREST:
		case GL_LINEAR:
			return 0;
		case GL_NEAREST_MIPMAP_NEAREST:
		case GL_NEAREST_MIPMAP_LINEAR:
		case GL_LINEAR_MIPMAP_NEAREST:
		case GL_LINEAR_MIPMAP_LINEAR:
			return 1;
	}
	Error("unknown texture filter %x\n",i);
//	return -1;
}


void ogl_cmd_texturemode(int argc, char **argv)
{
	if (argc < 2) {
		cmd_insertf("help %s", argv[0]);
		return;
	}

	if (!stricmp(argv[1], "GL_NEAREST")) {
		GL_texminfilt = GL_NEAREST;
		GL_texmagfilt = GL_NEAREST;
	} else if (!stricmp(argv[1], "GL_LINEAR")) {
		GL_texminfilt = GL_LINEAR;
		GL_texmagfilt = GL_LINEAR;
	} else if (!stricmp(argv[1], "GL_NEAREST_MIPMAP_NEAREST")) {
		GL_texminfilt = GL_NEAREST_MIPMAP_NEAREST;
		GL_texmagfilt = GL_NEAREST;
	} else if (!stricmp(argv[1], "GL_LINEAR_MIPMAP_NEAREST")) {
		GL_texminfilt = GL_LINEAR_MIPMAP_NEAREST;
		GL_texmagfilt = GL_LINEAR;
	} else if (!stricmp(argv[1], "GL_NEAREST_MIPMAP_LINEAR")) {
		GL_texminfilt = GL_NEAREST_MIPMAP_LINEAR;
		GL_texmagfilt = GL_NEAREST;
	} else if (!stricmp(argv[1], "GL_LINEAR_MIPMAP_LINEAR")) {
		GL_texminfilt = GL_LINEAR_MIPMAP_LINEAR;
		GL_texmagfilt = GL_LINEAR;
	} else
		return;

	GL_needmipmaps=ogl_testneedmipmaps(GL_texminfilt);

	ogl_smash_texture_list_internal();
}


#ifdef OGL_RUNTIME_LOAD
#ifdef _WIN32
char *OglLibPath="opengl32.dll";
#endif
#ifdef __unix__
char *OglLibPath="libGL.so";
#endif
#ifdef macintosh
char *OglLibPath = NULL;
#endif

int ogl_rt_loaded=0;
int ogl_init_load_library(void)
{
	int retcode=0;
	if (!ogl_rt_loaded){
		int t;
		if ((t=FindArg("-gl_library")))
			OglLibPath=Args[t+1];

		retcode = OpenGL_LoadLibrary(true);
		if(retcode)
		{
			mprintf((0,"Opengl loaded ok\n"));
	
			if(!glEnd)
			{
				Error("Opengl: Functions not imported\n");
			}
		}else{
			Error("Opengl: error loading %s\n", OglLibPath? OglLibPath : SDL_GetError());
		}
		ogl_rt_loaded=1;
	}
	return retcode;
}
#endif


int vid_init(void)
{
	int t, glt = 0;

 	// Only do this function once!
	if (vid_installed == 1)
		return -1;

#ifdef OGL_RUNTIME_LOAD
	ogl_init_load_library();
#endif

#ifdef VID_SUPPORTS_FULLSCREEN_TOGGLE
	if (FindArg("-gl_voodoo")){
		ogl_voodoohack=1;
		vid_toggle_fullscreen();
	}
	if (FindArg("-fullscreen"))
		vid_toggle_fullscreen();
#endif
	if ((glt=FindArg("-gl_alttexmerge")))
		ogl_alttexmerge=1;
	if ((t=FindArg("-gl_stdtexmerge")))
		if (t>=glt)//allow overriding of earlier args
			ogl_alttexmerge=0;

	if ((glt = FindArg("-gl_16bittextures")))
	{
		ogl_rgba_internalformat = GL_RGB5_A1;
		ogl_rgb_internalformat = GL_RGB5;
	}

	if ((glt=FindArg("-gl_mipmap"))){
		GL_texmagfilt=GL_LINEAR;
		GL_texminfilt=GL_LINEAR_MIPMAP_NEAREST;
	}
	if ((glt=FindArg("-gl_trilinear")))
	{
		GL_texmagfilt = GL_LINEAR;
		GL_texminfilt = GL_LINEAR_MIPMAP_LINEAR;
	}
	if ((t=FindArg("-gl_simple"))){
		if (t>=glt){//allow overriding of earlier args
			glt=t;
			GL_texmagfilt=GL_NEAREST;
			GL_texminfilt=GL_NEAREST;
		}
	}
	if ((t=FindArg("-gl_texmagfilt")) || (t=FindArg("-gl_texmagfilter"))){
		if (t>=glt)//allow overriding of earlier args
			GL_texmagfilt=ogl_atotexfilti(Args[t+1],0);
	}
	if ((t=FindArg("-gl_texminfilt")) || (t=FindArg("-gl_texminfilter"))){
		if (t>=glt)//allow overriding of earlier args
			GL_texminfilt=ogl_atotexfilti(Args[t+1],1);
	}
	GL_needmipmaps=ogl_testneedmipmaps(GL_texminfilt);

	if ((t = FindArg("-gl_anisotropy")) || (t = FindArg("-gl_anisotropic")))
	{
		GL_texanisofilt=atof(Args[t + 1]);
	}

	mprintf((0,"gr_init: texmagfilt:%x texminfilt:%x needmipmaps=%i anisotropic:%f\n",GL_texmagfilt,GL_texminfilt,GL_needmipmaps,GL_texanisofilt));

	
	if ((t=FindArg("-gl_vidmem"))){
		ogl_mem_target=atoi(Args[t+1])*1024*1024;
	}
	if ((t=FindArg("-gl_reticle"))){
		gl_reticle=atoi(Args[t+1]);
	}
	//printf("ogl_mem_target=%i\n",ogl_mem_target);
	
	ogl_init();//platform specific initialization

	ogl_init_texture_list_internal();
		
	cmd_addcommand("gl_texturemode", ogl_cmd_texturemode, "gl_texturemode <x>\n" "    use OpenGL texture mode <x>. Available mode are\n"
	                                                                             "        GL_NEAREST\n"
	                                                                             "        GL_LINEAR\n"
	                                                                             "        GL_NEAREST_MIPMAP_NEAREST\n"
	                                                                             "        GL_LINEAR_MIPMAP_NEAREST\n"
	                                                                             "        GL_NEAREST_MIPMAP_LINEAR\n"
	                                                                             "        GL_LINEAR_MIPMAP_LINEAR");

	vid_installed = 1;
	
	atexit(vid_close);

	return 0;
}


void vid_close(void)
{
//	mprintf((0,"ogl init: %s %s %s - %s\n",glGetString(GL_VENDOR),glGetString(GL_RENDERER),glGetString(GL_VERSION),glGetString,(GL_EXTENSIONS)));
	ogl_brightness_r = ogl_brightness_g = ogl_brightness_b = 0;
	ogl_setbrightness_internal();

	ogl_close();//platform specific code

#ifdef OGL_RUNTIME_LOAD
	if (ogl_rt_loaded)
		OpenGL_LoadLibrary(false);
#endif
}
extern int r_upixelc;
void ogl_upixelc(int x, int y, int c){
	r_upixelc++;
//	printf("gr_upixelc(%i,%i,%i)%i\n",x,y,c,Function_mode==FMODE_GAME);
//	if(Function_mode != FMODE_GAME){
//		grd_curcanv->cv_bitmap.bm_data[y*grd_curscreen->sc_canvas.cv_bitmap.bm_w+x]=c;
//	}else{
		OGL_DISABLE(TEXTURE_2D);
		glPointSize(1.0);
		glBegin(GL_POINTS);
//		glBegin(GL_LINES);
//	ogl_pal=gr_current_pal;
		glColor3f(CPAL2Tr(c),CPAL2Tg(c),CPAL2Tb(c));
//	ogl_pal=gr_palette;
		glVertex2f((x + grd_curcanv->cv_bitmap.bm_x + 0.5) / (float)last_width, 1.0 - (y + grd_curcanv->cv_bitmap.bm_y + 0.5) / (float)last_height);
//		glVertex2f(x/((float)last_width+1),1.0-y/((float)last_height+1));
		glEnd();
//	}
}
void ogl_urect(int left,int top,int right,int bot){
	GLfloat xo,yo,xf,yf;
	int c=COLOR;
	
	xo=(left+grd_curcanv->cv_bitmap.bm_x)/(float)last_width;
	xf = (right + 1 + grd_curcanv->cv_bitmap.bm_x) / (float)last_width;
	yo=1.0-(top+grd_curcanv->cv_bitmap.bm_y)/(float)last_height;
	yf = 1.0 - (bot + 1 + grd_curcanv->cv_bitmap.bm_y) / (float)last_height;

	OGL_DISABLE(TEXTURE_2D);
	if (Gr_scanline_darkening_level >= GR_FADE_LEVELS)
		glColor3f(CPAL2Tr(c), CPAL2Tg(c), CPAL2Tb(c));
	else
		glColor4f(CPAL2Tr(c), CPAL2Tg(c), CPAL2Tb(c), 1.0 - (float)Gr_scanline_darkening_level / ((float)GR_FADE_LEVELS - 1.0));
	glBegin(GL_QUADS);
	glVertex2f(xo,yo);
	glVertex2f(xo,yf);
	glVertex2f(xf,yf);
	glVertex2f(xf,yo);
	glEnd();
}
void ogl_ulinec(int left,int top,int right,int bot,int c){
	GLfloat xo,yo,xf,yf;

	xo = (left + grd_curcanv->cv_bitmap.bm_x + 0.5) / (float)last_width;
	xf = (right + grd_curcanv->cv_bitmap.bm_x + 0.5) / (float)last_width;
	yo = 1.0 - (top + grd_curcanv->cv_bitmap.bm_y + 0.5) / (float)last_height;
	yf = 1.0 - (bot + grd_curcanv->cv_bitmap.bm_y + 0.5) / (float)last_height;

	OGL_DISABLE(TEXTURE_2D);
	glColor3f(CPAL2Tr(c),CPAL2Tg(c),CPAL2Tb(c));
	glBegin(GL_LINES);
	glVertex2f(xo,yo);
	glVertex2f(xf,yf);
	glEnd();
}
	

GLfloat last_r=0, last_g=0, last_b=0;
int do_pal_step=0;
void ogl_do_palfx(void){
//	GLfloat r,g,b,a;
	OGL_DISABLE(TEXTURE_2D);
	if (gr_palette_faded_out){
/*		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
		glColor3f(0,0,0);
//		r=g=b=0.0;a=1.0;
	}else{
		if (do_pal_step){
			//glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_ONE);
			glColor3f(last_r,last_g,last_b);
//			r=f2fl(last_r);g=f2fl(last_g);b=f2fl(last_b);a=0.5;
		}else
			return;
	}
	
	
	glBegin(GL_QUADS);
	glVertex2f(0,0);
	glVertex2f(0,1);
	glVertex2f(1,1);
	glVertex2f(1,0);
	glEnd();
	
	glEnable(GL_BLEND);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void gr_palette_clear()
{
	gr_palette_faded_out=1;
}


int ogl_brightness_ok = 0;
int ogl_setgammaramp_ok = 1;
int ogl_brightness_r = 0, ogl_brightness_g = 0, ogl_brightness_b = 0;
static int old_b_r = 0, old_b_g = 0, old_b_b = 0;

void gr_palette_step_up(int r, int g, int b)
{
	if (gr_palette_faded_out)
		return;

	old_b_r = ogl_brightness_r;
	old_b_g = ogl_brightness_g;
	old_b_b = ogl_brightness_b;

	ogl_brightness_r = max(r + gr_palette_gamma.intval, 0);
	ogl_brightness_g = max(g + gr_palette_gamma.intval, 0);
	ogl_brightness_b = max(b + gr_palette_gamma.intval, 0);

	if (ogl_setgammaramp_ok &&
	    (old_b_r != ogl_brightness_r ||
	     old_b_g != ogl_brightness_g ||
	     old_b_b != ogl_brightness_b))
		ogl_brightness_ok = !ogl_setbrightness_internal();

	if (!ogl_setgammaramp_ok || !ogl_brightness_ok)
	{
		last_r = ogl_brightness_r / 63.0;
		last_g = ogl_brightness_g / 63.0;
		last_b = ogl_brightness_b / 63.0;

		do_pal_step = (r || g || b || gr_palette_gamma.intval);
	}
	else
	{
		do_pal_step = 0;
	}
}


void gr_palette_load( ubyte *pal )	
{
 int i;//, j;

 for (i=0; i<768; i++ ) {
     gr_current_pal[i] = pal[i];
     if (gr_current_pal[i] > 63) gr_current_pal[i] = 63;
 }
 //palette = screen->format->palette;

 gr_palette_faded_out=0;

	gr_palette_step_up(0, 0, 0); // make ogl_setbrightness_internal get run so that menus get brightened too.

 init_computed_colors();

	ogl_init_shared_palette();
}



int gr_palette_fade_out(ubyte *pal, int nsteps, int allow_keys)
{
	int j;
	unsigned char *buf;

	if (gr_palette_faded_out) return 0;

	if (grd_fades_disabled || !ogl_readpixels_ok) {
		gr_palette_faded_out = 1;
		return 0;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	MALLOC(buf, unsigned char, grd_curscreen->sc_w * grd_curscreen->sc_h * 3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, grd_curscreen->sc_w, grd_curscreen->sc_h, GL_RGB, GL_UNSIGNED_BYTE, buf);

	for (j = 0; j < nsteps; j++) {
		glRasterPos2f(0,0);
		glDrawPixels(grd_curscreen->sc_w, grd_curscreen->sc_h, GL_RGB, GL_UNSIGNED_BYTE, buf);

		glColor4f(0.0, 0.0, 0.0, (float)(j) / (nsteps - 1)); // go from 0 to 1
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
		glEnd();

		glFlush();

		timer_delay(f0_1 / 10);
	}
	d_free(buf);

	gr_palette_faded_out=1;
	return 0;
}



int gr_palette_fade_in(ubyte *pal, int nsteps, int allow_keys)
{
	int j;
	unsigned char *buf;

	if (!gr_palette_faded_out) return 0;

	if (grd_fades_disabled || !ogl_readpixels_ok) {
		gr_palette_faded_out = 0;
		return 0;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	MALLOC(buf, unsigned char, grd_curscreen->sc_w * grd_curscreen->sc_h * 3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0, 0, grd_curscreen->sc_w, grd_curscreen->sc_h, GL_RGB, GL_UNSIGNED_BYTE, buf);

	for (j = 0; j < nsteps; j++) {
		glRasterPos2f(0,0);
		glDrawPixels(grd_curscreen->sc_w, grd_curscreen->sc_h, GL_RGB, GL_UNSIGNED_BYTE, buf);

		glColor4f(0.0, 0.0, 0.0, (float)(nsteps - 1 - j) / (nsteps - 1)); // go from 1 to 0
		glBegin(GL_QUADS);
		glVertex2i(0, 0);
		glVertex2i(0, 1);
		glVertex2i(1, 1);
		glVertex2i(1, 0);
		glEnd();

		glFlush();

		timer_delay(f0_1 / 10);
	}
	d_free(buf);

	gr_palette_faded_out=0;
	return 0;
}



void gr_palette_read(ubyte * pal)
{
	int i;
	for (i=0; i<768; i++ ) {
		pal[i]=gr_current_pal[i];
		if (pal[i] > 63) pal[i] = 63;
	}
}

//writes out an uncompressed RGB .tga file
//if we got really spiffy, we could optionally link in libpng or something, and use that.
void write_bmp(char *savename,int w,int h,unsigned char *buf){
	PHYSFS_file *f;

	f = PHYSFSX_openWriteBuffered(savename);
		
	if (f) {
		GLubyte    targaMagic[12] = { 0, //no identification field
			 0,//no colormap
			 2,//RGB image (well, BGR, actually)
			 0, 0, 0, 0, 0, 0, 0, 0, 0 };//no colormap or image origin stuff.
		GLubyte blah;
		int r;
		GLubyte *s;
		int x,y;
		
		//write .TGA header.
		PHYSFS_write(f, targaMagic, sizeof(targaMagic), 1);
		PHYSFS_writeSLE16(f, w);
		PHYSFS_writeSLE16(f, h);
		PHYSFSX_writeU8(f, 24); // 24 bpp
		PHYSFSX_writeU8(f, 0);  // no attribute bits, origin is lowerleft, no interleave
		
		s=buf;
		for (y=0;y<h;y++){//TGAs use BGR ordering of data.
			for (x=0;x<w;x++){
				blah=s[0];
				s[0]=s[2];
				s[2]=blah;
				s+=3;				
			}
		}
		x=0;y=w*h*3;
		while (y > 0)
		{
			r = (int)PHYSFS_write(f, buf + x, 1, y);
			if (r<=0){
				mprintf((0,"screenshot error, couldn't write to %s (err %i)\n",savename,errno));
				break;
			}
			x+=r;y-=r;
		}
		PHYSFS_close(f);
	}else{
		mprintf((0,"screenshot error, couldn't open %s (err %i)\n",savename,errno));
	}
}
void save_screen_shot(int automap_flag)
{
//	fix t1;
	char message[100];
	static int savenum=0;
	char savename[13];
	unsigned char *buf;
	
	if (!ogl_readpixels_ok){
		if (!automap_flag)
			hud_message(MSGC_GAME_FEEDBACK,"glReadPixels not supported on your configuration");
		return;
	}

	stop_time();

//added/changed on 10/31/98 by Victor Rachels to fix overwrite each new game
	do
	{
		if (savenum == 9999)
			savenum = 0;
		sprintf(savename, "scrn%04d.tga", savenum++);
	} while (PHYSFS_exists(savename));

	sprintf( message, "%s '%s'", TXT_DUMPING_SCREEN, savename );
//end this section addition/change - Victor Rachels

	if (automap_flag) {
//	save_font = grd_curcanv->cv_font;
//	gr_set_curfont(GAME_FONT);
//	gr_set_fontcolor(gr_find_closest_color_current(0,31,0),-1);
//	gr_get_string_size(message,&w,&h,&aw);
//		modex_print_message(32, 2, message);
	} else {
		hud_message(MSGC_GAME_FEEDBACK, "%s", message);
	}
	
	buf = d_malloc(grd_curscreen->sc_w*grd_curscreen->sc_h*3);
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,grd_curscreen->sc_w,grd_curscreen->sc_h,GL_RGB,GL_UNSIGNED_BYTE,buf);
	write_bmp(savename,grd_curscreen->sc_w,grd_curscreen->sc_h,buf);
	d_free(buf);

	key_flush();
	start_time();
}
