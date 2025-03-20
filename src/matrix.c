#include "matrix.h"
 
mat4_t mat4_make_identity(void) {

/* When the 2D array is part of a struct (or another nested structure), 
 you need an additional set of braces to initialize the struct itself. */
    mat4_t m = {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    }};

    return m; 
}


mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t scale = mat4_make_identity();

    scale.m[0][0] = sx;
    scale.m[1][1] = sy;
    scale.m[2][2] = sz;

    return scale;
}
