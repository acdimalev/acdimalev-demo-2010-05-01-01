#define WIDTH   320
#define HEIGHT  240
#define FPS      30

#define ASPECT  ( (float) WIDTH / HEIGHT )
#define SCALE   ( sqrt(HEIGHT * WIDTH) )

#define POLYGON_VERTEX_MAX  16
#define POLYGON_MAX          3

#define METEOR_MAX       32
#define METEOR_TYPE_MAX   3

#define KEY_BINDING_MAX   8

extern struct meteor {
    int is_alive, type;
    float x, y, a;
    float xv, yv, av;
  } meteors[METEOR_MAX];

extern struct meteor_type {
    int polygon;
    int sides;
    float scale;
    int density;
    float velocity, angular_velocity;
  } meteor_types[METEOR_TYPE_MAX];

extern float frand();
extern void meteor_explode(struct meteor *meteor);
