#include <math.h>
#include "vector.h"

// 2D vector functions
float vec2_mag(vec2_t* v) {
    float res = sqrtf(v->x * v->x + v->y * v->y);
    return res; 
}

vec2_t vec2_add(vec2_t* a, vec2_t* b) {
    vec2_t res = { .x = a->x + b->x,
                   .y = a->y + b->y
                 };
    return res;
}
vec2_t vec2_sub(vec2_t* a, vec2_t* b){
    vec2_t res = { .x = a->x - b->x,
                   .y = a->y - b->y
                 };
    return res;
}
// 3D vector functions
float vec3_mag(vec3_t* v) {
    float res = sqrtf(v->x * v->x 
                     + v->y * v->y
                     + v->z * v->z);
    return res; 
}

vec3_t vec3_add(vec3_t* a, vec3_t* b) {
    vec3_t res = { .x = a->x + b->x,
                   .y = a->y + b->y,
                   .z = a->z + b->z
                 };
    return res;
}

vec3_t vec3_sub(vec3_t* a, vec3_t* b){
    vec3_t res = { .x = a->x - b->x,
                   .y = a->y - b->y,
                   .z = a->z - b->z
                 };
    return res;
}

void vec3_rotate_x(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);
    
   float r_y = (v->y * r_cos) - (v->z * r_sin);
   float r_z = (v->z * r_cos) + (v->y * r_sin);
   
   v->y = r_y;
   v->z = r_z; 
}
void vec3_rotate_y(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);
    
   float r_x = (v->x * r_cos) + (v->z * r_sin);
   float r_z = (v->z * r_cos) - (v->x * r_sin);
   
   v->x = r_x;
   v->z = r_z;
}
void vec3_rotate_z(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);

   float r_x = (v->x * r_cos) - (v->y * r_sin);
   float r_y = (v->y * r_cos) + (v->x * r_sin);

   v->x = r_x;
   v->y = r_y;
}
