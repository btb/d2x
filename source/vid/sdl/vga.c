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

static int sdl_window_flags = SDL_WINDOW_SHOWN;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *screen, *windowSurface;
SDL_Texture *texture;


// VGA Functions --------------------------------------------------------------

void vga_close(void)
{
   if (!SDL_WasInit(SDL_INIT_VIDEO))
      return;

   SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

short vga_init(void)
{
   if (SDL_WasInit(SDL_INIT_VIDEO))
      return 1;

   if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
      Error("Could not initialize SDL video: %s\n", SDL_GetError());

   atexit(vga_close);

   return 0;
}

short vga_set_mode(short mode)
{
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

   if (screen != NULL)
      gr_palette_clear();

   window = SDL_CreateWindow("Descent II",
                             SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                             w, h, sdl_window_flags);
   renderer = SDL_CreateRenderer(window, -1, 0);

   if (window == NULL || renderer == NULL)
      Error("Could not create SDL window: %s\n", SDL_GetError());

   screen = SDL_CreateRGBSurfaceWithFormat(0, w, h, 8, SDL_PIXELFORMAT_INDEX8);

   windowSurface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

   texture = SDL_CreateTextureFromSurface(renderer, windowSurface);

   return gr_init_screen(BM_LINEAR, w, h, 0, 0, w, screen->pixels);
}

short vga_check_mode(short mode)
{
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
