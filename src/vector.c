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


vec2_t vec2_mul(vec2_t* v, float factor) {
    vec2_t res = {
        .x = v->x * factor,
        .y = v->y * factor
        };
    return res; 
}
vec2_t vec2_div(vec2_t* v, float factor) {
    vec2_t res = {
        .x = v->x / factor,
        .y = v->y / factor
        };
    return res; 
}


float vec2_dot(vec2_t* a, vec2_t* b) {
    float res = (a->x * b->x) + (a->y * b->y);
    return res; 
}


void vec2_norm(vec2_t* v) {
    // v is already a pointer inside this function hence no &
    // cause you will pass the address of the ptr itself this way
    float z = vec2_mag(v);
    *v = vec2_div(v, z); 
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


vec3_t vec3_mul(vec3_t* v, float factor) {
    vec3_t res = {
        .x = v->x * factor,
        .y = v->y * factor,
        .z = v->z * factor
        };
    return res; 
}
vec3_t vec3_div(vec3_t* v, float factor) {
    vec3_t res = {
        .x = v->x / factor,
        .y = v->y / factor,
        .z = v->z / factor
        };
    return res; 
}


vec3_t vec3_cross(vec3_t* a, vec3_t* b) {
    vec3_t res = {
        .x = (a->y * b->z) - (a->z * b->y),
        .y = (a->z * b->x) - (a->x * b->z), 
        .z = (a->x * b->y) - (a->y * b->x) 
    };
    return res; 
}

float vec3_dot(vec3_t* a, vec3_t* b) {
    float res = (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
    return res; 
}

void vec3_norm(vec3_t* v) {
    // refer to vec2 version of this if confused on ptrs 
    float z = vec3_mag(v);
    *v = vec3_div(v, z); 
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
