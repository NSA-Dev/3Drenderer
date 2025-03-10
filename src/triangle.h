#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vector.h"

// used for storing vertex indices
typedef struct {
    int a;
    int b;
    int c;
} face_t; 

// used for storing actual (x,y) coordinates for each vert
typedef struct {
    vec2_t points[3];
} triangle_t; 


#endif
