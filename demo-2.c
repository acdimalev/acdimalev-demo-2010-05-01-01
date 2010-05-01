#include <math.h>

#include "SDL.h"
#include "core.h"

void generate_meteor() {
  int type = METEOR_TYPE_MAX - 1;
  float v = meteor_types[type].velocity;
  float av = meteor_types[type].angular_velocity;
  float a = 2*M_PI * frand();

  meteors[0].is_alive = 1;
  meteors[0].type = type;

  meteors[0].x = WIDTH/SCALE * frand();
  meteors[0].y = HEIGHT/SCALE * frand();
  meteors[0].a = 2*M_PI * frand();

  meteors[0].xv = v * -sin(a);
  meteors[0].yv = v *  cos(a);
  meteors[0].av = 2*M_PI * av * (frand() - 1/2.0);
}

void explode() {
  struct meteor *meteor, *tagged_meteors[METEOR_MAX];
  int i;

  for (i = 0; i < METEOR_MAX; i = i + 1) {
    if (meteors[i].is_alive) { break; }
  }
  if (i == METEOR_MAX) {
    generate_meteor();
    return;
  }

  for (i = 0; i < METEOR_MAX; i = i + 1) {
    if (meteors[i].is_alive) {
      tagged_meteors[i] = &meteors[i];
    } else {
      tagged_meteors[i] = 0;
    }
  }

  for (i = 0; i < METEOR_MAX; i = i + 1) {
    if (tagged_meteors[i]) {
      meteor_explode(tagged_meteors[i]);
    }
  }
}

void game_init() {
  generate_meteor();
  bind_key(SDLK_SPACE, explode);
}
