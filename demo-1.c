#include <math.h>

#include "core.h"

void game_init() {
  meteors[0].is_alive = 1;
  meteors[0].type = 0;
  meteors[0].x = 0;
  meteors[0].y = 0;
  meteors[0].a = 0;
  meteors[0].xv = 1;
  meteors[0].yv = 0;
  meteors[0].av = 2*M_PI;
}
