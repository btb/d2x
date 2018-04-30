
#include <SDL.h>

#include "pstypes.h"
#include "error.h"
#include "fix.h"
#include "minmax.h"

#include "palette.h"


// Special --------------------------------------------------------------------
extern ubyte gr_current_pal[256*3];        // Current Valid Palette in RGB
extern ubyte gr_palette_gamma;
extern int grd_fades_disabled;
extern SDL_Surface *screen;

void init_computed_colors(void);


// Functions ------------------------------------------------------------------

void gr_palette_step_up(int r, int g, int b)
{
   Int3();
}

void gr_palette_clear(void)
{
   SDL_Palette *palette;
   SDL_Color colors[256];
   int ncolors;

   palette = screen->format->palette;

   if (palette == NULL)
      return; // Display is not palettised

   ncolors = palette->ncolors;
   memset(colors, 0, ncolors * sizeof(SDL_Color));

   SDL_SetPaletteColors(palette, colors, 0, 256);

   gr_palette_faded_out = 1;
}

void gr_palette_load(ubyte *pal)
{
   int i, j;
   SDL_Palette *palette;
   SDL_Color colors[256];

   for (i = 0; i < 768; i++) {
      gr_current_pal[i] = pal[i];
      if (gr_current_pal[i] > 63)
         gr_current_pal[i] = 63;
   }

   palette = screen->format->palette;

   if (palette == NULL)
      return; // Display is not palettised

   for (i = 0, j = 0; j < 256; j++) {
      colors[j].r = min(gr_current_pal[i] + gr_palette_gamma, 63) * 4; i++;
      colors[j].g = min(gr_current_pal[i] + gr_palette_gamma, 63) * 4; i++;
      colors[j].b = min(gr_current_pal[i] + gr_palette_gamma, 63) * 4; i++;
   }

   SDL_SetPaletteColors(palette, colors, 0, 256);

   gr_palette_faded_out = 0;
   init_computed_colors();
}

int gr_palette_fade_out(ubyte *pal, int nsteps, int allow_keys)
{
   int i, j, k;
   ubyte c;
   fix fade_palette[768];
   fix fade_palette_delta[768];

   SDL_Palette *palette;
   SDL_Color fade_colors[256];

   if (gr_palette_faded_out)
      return 0;

#ifndef NDEBUG
   if (grd_fades_disabled) {
      gr_palette_clear();
      return 0;
   }
#endif

   palette = screen->format->palette;
   if (palette == NULL)
      return -1; // Display is not palettised

   if (pal == NULL)
      pal = gr_current_pal;

   for (i = 0; i < 768; i++) {
      gr_current_pal[i] = pal[i];
      fade_palette[i] = i2f(pal[i]);
      fade_palette_delta[i] = fade_palette[i] / nsteps;
   }

   for (j = 0; j < nsteps; j++) {
      for (i = 0, k = 0; k < 256; k++) {
         fade_palette[i] -= fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].r = c * 4;
         i++;

         fade_palette[i] -= fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].g = c * 4;
         i++;

         fade_palette[i] -= fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].b = c * 4;
         i++;
      }

      SDL_SetPaletteColors(palette, fade_colors, 0, 256);
   }

   gr_palette_faded_out = 1;

   SDL_FillRect(screen, NULL, 0);

   return 0;
}

int gr_palette_fade_in(ubyte *pal, int nsteps, int allow_keys)
{
   int i, j, k;
   ubyte c;
   fix fade_palette[768];
   fix fade_palette_delta[768];
   SDL_Palette *palette;
   SDL_Color fade_colors[256];

   if (!gr_palette_faded_out) return 0;

#ifndef NDEBUG
   if (grd_fades_disabled) {
      gr_palette_load(pal);
      return 0;
   }
#endif

   palette = screen->format->palette;

   if (palette == NULL)
      return -1; // Display is not palettised

   if (pal == NULL)
      pal = gr_current_pal;

   for (i = 0; i < 768; i++) {
      gr_current_pal[i] = pal[i];
      fade_palette[i] = 0;
      fade_palette_delta[i] = i2f(pal[i]) / nsteps;
   }

   for (j = 0; j < nsteps; j++) {
      for (i = 0, k = 0; k < 256; k++) {
         fade_palette[i] += fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].r = c * 4;
         i++;

         fade_palette[i] += fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].g = c * 4;
         i++;

         fade_palette[i] += fade_palette_delta[i];
         if (fade_palette[i] > i2f(pal[i] + gr_palette_gamma))
            fade_palette[i] = i2f(pal[i] + gr_palette_gamma);
         c = f2i(fade_palette[i]);
         if (c > 63) c = 63;
         fade_colors[k].b = c * 4;
         i++;
      }

      SDL_SetPaletteColors(palette, fade_colors, 0, 256);
   }

   gr_palette_load(pal);

   gr_palette_faded_out = 0;

   return 0;
}

void gr_palette_read(ubyte *palette)
{
   Int3();
}
