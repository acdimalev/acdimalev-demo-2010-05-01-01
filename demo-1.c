#include <math.h>

#include "SDL.h"
#include "core.h"

void generate_meteors() {
  int i, j, k;

  k = 0;
  for (j = 0; j < METEOR_TYPE_MAX; j = j + 1) {
    for (i = 0; i < meteor_types[j].density; i = i + 1) {
      float v = meteor_types[j].velocity;
      float av = meteor_types[j].angular_velocity;
      float a = 2*M_PI * frand();

      meteors[k].is_alive = 1;
      meteors[k].type = j;

      meteors[k].x = WIDTH/SCALE * frand();
      meteors[k].y = HEIGHT/SCALE * frand();
      meteors[k].a = 2*M_PI * frand();

      meteors[k].xv = v * -sin(a);
      meteors[k].yv = v *  cos(a);
      meteors[k].av = 2*M_PI * av * (frand() - 1/2.0);

      k = k + 1;
    }
  }
}

void game_init() {
  generate_meteors();
  bind_key(SDLK_SPACE, generate_meteors);
}
