#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>
#include "vector.h"

typedef struct {
    float m[4][4];
} mat4_t;

/* mat4_t constructors */
mat4_t mat4_make_identity(void);
mat4_t mat4_make_scale(float sx, float sy, float sz);
mat4_t mat4_make_translation(float tx, float ty, float tz);
mat4_t mat4_make_rotation_x(float angle);
mat4_t mat4_make_rotation_y(float angle);
mat4_t mat4_make_rotation_z(float angle);
mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar);
/* operations*/
vec4_t mat4_mult_vec4(mat4_t* m, vec4_t* v);
mat4_t mat4_mult_mat4(mat4_t* a, mat4_t* b);
vec4_t mat4_mult_vec4_project(mat4_t* m, vec4_t* v);
mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up); 
#endif
