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


vec4_t mat4_mult_vec4(mat4_t* m, vec4_t* v) {
    vec4_t res = {
        .x = m->m[0][0] * v->x + m->m[0][1] * v->y + m->m[0][2] * v->z + m->m[0][3] * v->w,
        .y = m->m[1][0] * v->x + m->m[1][1] * v->y + m->m[1][2] * v->z + m->m[1][3] * v->w,
        .z = m->m[2][0] * v->x + m->m[2][1] * v->y + m->m[2][2] * v->z + m->m[2][3] * v->w,
        .w = m->m[3][0] * v->x + m->m[3][1] * v->y + m->m[3][2] * v->z + m->m[3][3] * v->w,
    };

    return res;
}
