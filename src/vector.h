#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} vec2_t; 

typedef struct {
    float x;
    float y;
    float z;  
} vec3_t;


// TODO vector functions
vec2_t project (vec3_t* point);
void vec3_rotate_x(vec3_t* v, float angle);
void vec3_rotate_y(vec3_t* v, float angle);
void vec3_rotate_z(vec3_t* v, float angle);
#endif
