/* prototypes for function calls between files within the OpenGL module */

#ifndef _INTERNAL_H_
#define _INTERNAL_H_

#include "ogl_init.h" // interface to OpenGL module

/* I assume this ought to be >= MAX_BITMAP_FILES in piggy.h? */
#define OGL_TEXTURE_LIST_SIZE 3000

extern ogl_texture ogl_texture_list[OGL_TEXTURE_LIST_SIZE];

extern int ogl_mem_target;

void ogl_init_texture_list_internal(void);
void ogl_smash_texture_list_internal(void);
void ogl_vivify_texture_list_internal(void);

extern int ogl_brightness_ok;
extern int ogl_brightness_r, ogl_brightness_g, ogl_brightness_b;
int ogl_setbrightness_internal(void);
extern int ogl_fullscreen;
void ogl_do_fullscreen_internal(void);

extern int ogl_voodoohack;

#ifdef GL_ARB_multitexture
extern int ogl_arb_multitexture_ok;
#else
#define ogl_arb_multitexture_ok 0
#endif
#ifdef GL_SGIS_multitexture
extern int ogl_sgis_multitexture_ok;
#else
#define ogl_sgis_multitexture_ok 0
#endif

extern int GL_TEXTURE_2D_enabled;
//extern int GL_texclamp_enabled;
//extern int GL_TEXTURE_ENV_MODE_state,GL_TEXTURE_MAG_FILTER_state,GL_TEXTURE_MIN_FILTER_state;
#define OGL_ENABLE2(a,f) {if (a ## _enabled!=1) {f;a ## _enabled=1;}}
#define OGL_DISABLE2(a,f) {if (a ## _enabled!=0) {f;a ## _enabled=0;}}

//#define OGL_ENABLE(a) OGL_ENABLE2(a,glEnable(a))
//#define OGL_DISABLE(a) OGL_DISABLE2(a,glDisable(a))
#define OGL_ENABLE(a) OGL_ENABLE2(GL_ ## a,glEnable(GL_ ## a))
#define OGL_DISABLE(a) OGL_DISABLE2(GL_ ## a,glDisable(GL_ ## a))

//#define OGL_TEXCLAMP() OGL_ENABLE2(GL_texclamp,glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_CLAMP);glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,    GL_CLAMP);)
//#define OGL_TEXREPEAT() OGL_DISABLE2(GL_texclamp,glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);)
//#define OGL_SETSTATE(a,s,f) {if (a ## _state!=s) {f;a ## _state=s;}}
//#define OGL_TEXENV(p,m) OGL_SETSTATE(p,m,glTexEnvi(GL_TEXTURE_ENV, p,m));
//#define OGL_TEXPARAM(p,m) OGL_SETSTATE(p,m,glTexParameteri(GL_TEXTURE_2D,p,m))

extern unsigned last_width,last_height;

static inline void OGL_VIEWPORT(const unsigned x, const unsigned y, const unsigned w, const unsigned h)
{
	if (w != last_width || h != last_height)
	{
		glViewport(x, grd_curscreen->sc_canvas.cv_bitmap.bm_h - y - h, w, h);
		last_width = w;
		last_height = h;
	}
}


//platform specific funcs
void ogl_swap_buffers_internal(void);

extern unsigned char *ogl_pal;

//whee
//#define PAL2Tr(c) ((gr_palette[c*3]+gr_palette_gamma.intval)/63.0)
//#define PAL2Tg(c) ((gr_palette[c*3+1]+gr_palette_gamma.intval)/63.0)
//#define PAL2Tb(c) ((gr_palette[c*3+2]+gr_palette_gamma.intval)/63.0)
//#define PAL2Tr(c) ((gr_palette[c*3])/63.0)
//#define PAL2Tg(c) ((gr_palette[c*3+1])/63.0)
//#define PAL2Tb(c) ((gr_palette[c*3+2])/63.0)
#define CPAL2Tr(c) ((gr_current_pal[c*3])/63.0)
#define CPAL2Tg(c) ((gr_current_pal[c*3+1])/63.0)
#define CPAL2Tb(c) ((gr_current_pal[c*3+2])/63.0)
#define PAL2Tr(c) ((ogl_pal[c*3])/63.0)
#define PAL2Tg(c) ((ogl_pal[c*3+1])/63.0)
#define PAL2Tb(c) ((ogl_pal[c*3+2])/63.0)
//inline GLfloat PAL2Tr(int c);
//inline GLfloat PAL2Tg(int c);
//inline GLfloat PAL2Tb(int c);

#endif // _INTERNAL_H_
