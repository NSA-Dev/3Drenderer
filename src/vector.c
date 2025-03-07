#include "vector.h"
#include <stdio.h>

vec2_t project (vec3_t* point) {
        return (vec2_t) {.x = point->x, .y = point->y}; 
}
