#define WIDTH   320
#define HEIGHT  240
#define FPS      30

#define ASPECT  ( (float) WIDTH / HEIGHT )
#define SCALE   ( sqrt(HEIGHT * WIDTH) )

#define POLYGON_VERTEX_MAX  16
#define POLYGON_MAX          3

#define METEOR_MAX       16
#define METEOR_TYPE_MAX   3

extern struct meteor {
    int is_alive, type;
    float x, y, a;
    float xv, yv, av;
  } meteors[METEOR_MAX];
