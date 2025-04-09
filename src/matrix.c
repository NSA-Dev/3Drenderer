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


mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
    
    float fov_factor = 1 / tan (fov /2);
    float z_factor = zfar / (zfar - znear); 

    mat4_t res = {{{ 0 }}}; // init to zero

    res.m[0][0] = aspect * fov_factor; 
    res.m[1][2] = fov_factor; 
    res.m[2][2] = z_factor;
    res.m[2][3] = -(z_factor) * znear;
    res.m[3][2] = 1.0; 

    return res; 
}

mat4_t mat4_make_scale(float sx, float sy, float sz) {
    mat4_t scale = mat4_make_identity();

    scale.m[0][0] = sx;
    scale.m[1][1] = sy;
    scale.m[2][2] = sz;

    return scale;
}


mat4_t mat4_make_translation(float tx, float ty, float tz) {
    // w unaccounted for
    mat4_t translation = mat4_make_identity();

    translation.m[0][3] = tx;
    translation.m[1][3] = ty;
    translation.m[2][3] = tz;

    return translation;
}

mat4_t mat4_make_rotation_x(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t res = mat4_make_identity();
    res.m[1][1] = c;
    res.m[1][2] = -s;
    res.m[2][1] = s;
    res.m[2][2] = c;

    return res;
}
mat4_t mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t res = mat4_make_identity();
    res.m[0][0] = c;
    res.m[0][2] = s;
    res.m[2][0] = -s;
    res.m[2][2] = c;

    return res;
}
mat4_t mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);

    mat4_t res = mat4_make_identity();
    res.m[0][0] = c;
    res.m[0][1] = -s;
    res.m[1][0] = s;
    res.m[1][1] = c;

    return res;
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

mat4_t  mat4_mult_mat4(mat4_t* a, mat4_t* b) {

    mat4_t res;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            res.m [i][j] = a->m[i][0] * b->m[0][j] 
                        +  a->m[i][1] * b->m[1][j]
                        +  a->m[i][2] * b->m[2][j]
                        +  a->m[i][3] * b->m[3][j];
        }
    }

   return res; 
}
