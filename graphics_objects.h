typedef struct {
    float r;
    float g;
    float b;
} Color;


typedef struct {
    float closest_t;
    Sphere* closest_sphere;
} Intersection;

typedef struct {
    int center[3];
    int radius;
    int color[3];
    int specular;
    float reflective;
} Sphere;

typedef struct {
    char type[16];
    float intensity;
    int position[3];
    int direction[3];
} Light;

