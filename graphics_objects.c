#include "graphics_objects.h"

const Light l1 = {
    .intensity = 0.1,
    .type = "ambient"
};

const Light l2 = {
    .intensity = 0.6,
    .position = {2, 1, 0},
    .type = "point"
};

const Light l3 = {
    .intensity = 0.2,
    .direction = {1, 4, 4},
    .type = "directional"
};

const Sphere s1 = {
    .center = {0, -1, 3},
    .radius = 1,
    .color = {1, 0, 0},
    .specular = 500,
    .reflective = 0.2
};

const Sphere s2 = {
    .center = {2, 0, 4},
    .radius = 1,
    .color = {0, 0, 1},
    .specular = 500,
    .reflective = 0.3
};

const Sphere s3 = {
    .center = {-2, 0, 4},
    .radius = 1,
    .color = {0, 1, 0},
    .specular = 10,
    .reflective = 0.4
};

const Sphere s4 = {
    .center = {0, -5001, 4},
    .radius = 5000,
    .color = {1, 0, 1},
    .specular = 10,
    .reflective = 0.2
};
