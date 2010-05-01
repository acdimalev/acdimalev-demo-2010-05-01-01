#include <cairo.h>
#include <math.h>

#include "SDL.h"
#include "core.h"

struct meteor meteors[METEOR_MAX];

struct meteor_type meteor_types[METEOR_TYPE_MAX] = {
    0, 3, 1/32.0, 8, 1/2.0, 4/1.0,
    0, 4, 3/32.0, 3, 3/8.0, 1/2.0,
    0, 8, 3/16.0, 1, 1/8.0, 1/8.0
  };

struct polygon {
    int n;
    float v[POLYGON_VERTEX_MAX][2];
  } polygons[POLYGON_MAX];

struct key_binding {
    int key;
    void (*callback)();
  } key_bindings[KEY_BINDING_MAX];

int running;

void init_key_bindings() {
  int i;

  for (i = 0; i < KEY_BINDING_MAX; i = i + 1) {
    key_bindings[i].key = 0;
  }
}

int bind_key(int key, void (*callback)()) {
  int i;

  for (i = 0; i < KEY_BINDING_MAX; i = i + 1) {
    if (key_bindings[i].key == 0) {
      break;
    }
  }
  if (i == KEY_BINDING_MAX) { return -1; }

  key_bindings[i].key = key;
  key_bindings[i].callback = callback;

  return 0;
}

int ngon(struct polygon *polygon, int n, float scale) {
  float a, aoff;
  int i;

  polygon->n = n;

  for (i = 0; i < n; i = i + 1) {
    a = 2*M_PI * (float) i / n;
    polygon->v[i][0] = scale * -sin(a);
    polygon->v[i][1] = scale *  cos(a);
  }

  return 0;
}

int init_meteor_types() {
  int i;

  for (i = 0; i < METEOR_TYPE_MAX; i = i + 1) {
    struct polygon *polygon = &polygons[i];
    int   sides = meteor_types[i].sides;
    float scale = meteor_types[i].scale;

    if ( ngon(polygon, sides, scale) ) { return -1; }
    meteor_types[i].polygon = i;
  }

  return 0;
}

void init_meteors() {
  int i;

  for (i = 0; i < METEOR_MAX; i = i + 1) {
    meteors[i].is_alive = 0;
  }
}

void game_quit() {
  running = 0;
}

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  Uint8 *keystate;
  Uint32 next_frame, now;
  cairo_t *cr;
  cairo_matrix_t cairo_matrix_display;

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

  {
    cairo_matrix_t *m = &cairo_matrix_display;

    cairo_matrix_init_identity(m);

    // Cartesian
    cairo_matrix_translate(m, WIDTH/2.0, HEIGHT/2.0);
    cairo_matrix_scale(m, 1, -1);

    // fixed scale
    cairo_matrix_scale(m, SCALE, SCALE);
  }

  /* Initialize Delay */
  next_frame = 1024.0 / FPS;

  { /* Game Logic */
    running = 1;

    if ( init_meteor_types() ) {
      SDL_Quit();
      return -1;
    };
    init_meteors();

    init_key_bindings();
    bind_key(SDLK_q, game_quit);

    game_init();
  }

  SDL_LockSurface(sdl_surface);
  while (running) {

    { /* Render Frame */
      int i, j;

      cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint(cr);
      cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

      for (j = 0; j < METEOR_MAX; j = j + 1) {
        float a, x, y;
        struct polygon *polygon;

        if (!meteors[j].is_alive) { continue; }

        a = meteors[j].a;
        x = meteors[j].x;
        y = meteors[j].y;

        polygon = &polygons[meteor_types[meteors[j].type].polygon];

        cairo_set_matrix(cr, &cairo_matrix_display);
        cairo_translate(cr, x, y);
        cairo_rotate(cr, a);

        cairo_new_sub_path(cr);
        for (i = 0; i < polygon->n; i = i + 1) {
          cairo_line_to(cr, polygon->v[i][0], polygon->v[i][1]);
        }
        cairo_close_path(cr);

        cairo_set_matrix(cr, &cairo_matrix_display);
        cairo_translate(cr, x - WIDTH/SCALE, y);
        cairo_rotate(cr, a);

        cairo_new_sub_path(cr);
        for (i = 0; i < polygon->n; i = i + 1) {
          cairo_line_to(cr, polygon->v[i][0], polygon->v[i][1]);
        }
        cairo_close_path(cr);

        cairo_set_matrix(cr, &cairo_matrix_display);
        cairo_translate(cr, x, y - HEIGHT/SCALE);
        cairo_rotate(cr, a);

        cairo_new_sub_path(cr);
        for (i = 0; i < polygon->n; i = i + 1) {
          cairo_line_to(cr, polygon->v[i][0], polygon->v[i][1]);
        }
        cairo_close_path(cr);

        cairo_set_matrix(cr, &cairo_matrix_display);
        cairo_translate(cr, x - WIDTH/SCALE, y - HEIGHT/SCALE);
        cairo_rotate(cr, a);

        cairo_new_sub_path(cr);
        for (i = 0; i < polygon->n; i = i + 1) {
          cairo_line_to(cr, polygon->v[i][0], polygon->v[i][1]);
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
      SDL_Event event;
      int i;

      while ( SDL_PollEvent(&event) ) {
        if (event.type == SDL_KEYDOWN) {

      for (i = 0; i < KEY_BINDING_MAX; i = i + 1) {
        if (!key_bindings[i].key) { continue; }

        if (event.key.keysym.sym == key_bindings[i].key) {
          key_bindings[i].callback();
        }
      }

        }
      }

      for (i = 0; i < METEOR_MAX; i = i + 1) {
        if (!meteors[i].is_alive) { continue; }

        meteors[i].x = meteors[i].x + meteors[i].xv / FPS;
        meteors[i].y = meteors[i].y + meteors[i].yv / FPS;
        meteors[i].a = meteors[i].a + meteors[i].av / FPS;

        if (meteors[i].x < 0) {
          meteors[i].x = meteors[i].x + WIDTH/SCALE;
        }
        if (meteors[i].x > WIDTH/SCALE) {
          meteors[i].x = meteors[i].x - WIDTH/SCALE;
        }
        if (meteors[i].y < 0) {
          meteors[i].y = meteors[i].y + HEIGHT/SCALE;
        }
        if (meteors[i].y > HEIGHT/SCALE) {
          meteors[i].y = meteors[i].y - HEIGHT/SCALE;
        }
      }
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
