/*
 *
 * SDL video functions.
 *
 */


#include <SDL.h>

#include "vga.h"
#include "gr.h"
#include "error.h"
#include "palette.h"


// Global Variables -----------------------------------------------------------

int VGA_current_mode = SM_ORIGINAL;

static int sdl_video_flags = SDL_SWSURFACE;


// VGA Functions --------------------------------------------------------------

void vga_close(void)
{
   Int3();
   if (!SDL_WasInit(SDL_INIT_VIDEO))
      return;

   SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

short vga_init(void)
{
   Int3();
   if (SDL_WasInit(SDL_INIT_VIDEO))
      return 1;

   SDL_InitSubSystem(SDL_INIT_VIDEO);

   atexit(vga_close);

   return 0;
}

short vga_set_mode(short mode)
{
   Int3();
   unsigned int w, h;

   if (!SDL_WasInit(SDL_INIT_VIDEO))
      return 1;

   switch(mode)
   {
   case SM_ORIGINAL:   return 0;
   case SM_320x200C:   w = 320;    h = 200;    break;
   case SM_640x480V:   w = 640;    h = 480;    break;
   case SM_800x600V:   w = 800;    h = 600;    break;
   case SM_1024x768V:  w = 1024;   h = 768;    break;
   case SM_1280x1024V: w = 1280;   h = 1024;   break;
   default:       //unknown mode!!!  Very bad!!
      Error("Unknown mode %d in vga_set_mode()", mode);
   }

   VGA_current_mode = mode;

   gr_palette_clear();

// screen = SDL_SetVideoMode(w, h, 8, sdl_video_flags);

// return gr_init_screen(BM_LINEAR, w, h, 0, 0, w, screen->pixels);
   return 1;
}

short vga_check_mode(short mode)
{
   Int3();
   switch(mode) {
      case SM_320x200C:
      case SM_640x480V:
      case SM_800x600V:
      case SM_1024x768V:
      case SM_1280x1024V:
         return 0;
      default:
         return 11;
   }
}
