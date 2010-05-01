#include "SDL.h"
#include <cairo.h>
#include <math.h>

#define WIDTH   320
#define HEIGHT  240
#define FPS     30

#define SCALE  (HEIGHT / 4.0)

#define POLYGON_MAX_VERTICES  16
#define POLYGON_MAX            3

struct polygon {
    int n;
    float v[POLYGON_MAX_VERTICES][2];
  };

int ngon(struct polygon *polygon, int n, int odd, float scale) {
  float a, aoff;
  int i;

  if (odd) {
    aoff = 0;
  } else {
    aoff = 2*M_PI * (1.0 / n) / 2; // half a step
  }

  polygon->n = n;

  for (i = 0; i < n; i = i + 1) {
    a = 2*M_PI * (float) i / n + aoff;
    polygon->v[i][0] = scale * -sin(a);
    polygon->v[i][1] = scale *  cos(a);
  }

  return 0;
}

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  Uint8 *keystate;
  Uint32 next_frame, now;
  cairo_t *cr;

  int running;
  struct polygon polygons[POLYGON_MAX];

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_JOYSTICK);
  SDL_ShowCursor(0);
  SDL_SetVideoMode(WIDTH, HEIGHT, 32, 0);
  sdl_surface = SDL_GetVideoSurface();

  { /* Initialize Canvas */
    cairo_surface_t *cr_surface;
    cr_surface = cairo_image_surface_create_for_data(
      sdl_surface->pixels,
      CAIRO_FORMAT_RGB24,
      sdl_surface->w,
      sdl_surface->h,
      sdl_surface->pitch
      );
    cr = cairo_create(cr_surface);
    cairo_surface_destroy(cr_surface);
  }

  // Cartesian
  cairo_translate(cr, WIDTH/2.0, HEIGHT/2.0);
  cairo_scale(cr, 1, -1);

  // fixed scale
  cairo_scale(cr, SCALE, SCALE);

  /* Initialize Delay */
  next_frame = 1024.0 / FPS;

  { /* Game Logic */
    running = 1;

    ngon(&polygons[0], 8, 0, 1/2.0);
    ngon(&polygons[1], 4, 0, 1/4.0);
    ngon(&polygons[2], 3, 1, 1/8.0);
  }

  SDL_LockSurface(sdl_surface);
  while (running) {

    { /* Render Frame */
      int i, j;
      struct polygon *polygon;

      cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint(cr);
      cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

      for (j = 0; j < POLYGON_MAX; j = j + 1) {
        polygon = &polygons[j];

        cairo_new_path(cr);
        for (i = 0; i < polygon->n; i = i + 1) {
          cairo_line_to(cr, polygon->v[i][0] + j, polygon->v[i][1]);
        }
        cairo_close_path(cr);
        cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        cairo_fill(cr);
      }
    }

    /* Update Display */
    SDL_UnlockSurface(sdl_surface);
    SDL_Flip(sdl_surface);
    SDL_LockSurface(sdl_surface);

    { /* Delay */
      Uint32 now;

      now = SDL_GetTicks();
      if (now < next_frame) {
        SDL_Delay(next_frame - now);
      }
      next_frame = next_frame + 1024.0 / FPS;
    }

    { /* Game Logic */
      SDL_PumpEvents();
      keystate = SDL_GetKeyState(NULL);
      if (keystate[SDLK_q]) {
        running = 0;
      }
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
