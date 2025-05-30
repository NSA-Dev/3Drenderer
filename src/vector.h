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

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4_t;

// 2D vector functions
float vec2_mag(vec2_t* v);
vec2_t vec2_add(vec2_t* a, vec2_t* b);
vec2_t vec2_sub(vec2_t* a, vec2_t* b);
vec2_t vec2_mul(vec2_t* v, float factor);
vec2_t vec2_div(vec2_t* v, float factor);
float vec2_dot(vec2_t* a, vec2_t* b);
void vec2_norm(vec2_t* v);
float vec2_cross(vec2_t* a, vec2_t* b); // fake cross product that returns a scalar 

// Vector 3D functions
float vec3_mag(vec3_t* v); 
vec3_t vec3_add(vec3_t* a, vec3_t* b);
vec3_t vec3_sub(vec3_t* a, vec3_t* b);
vec3_t vec3_mul(vec3_t* v, float factor);
vec3_t vec3_div(vec3_t* v, float factor);
vec3_t vec3_cross(vec3_t* a, vec3_t* b);
float vec3_dot(vec3_t* a, vec3_t* b);
void vec3_norm(vec3_t* v); 
void vec3_rotate_x(vec3_t* v, float angle);
void vec3_rotate_y(vec3_t* v, float angle);
void vec3_rotate_z(vec3_t* v, float angle);

// Vector conversion functions
vec4_t vec4_from_vec3(vec3_t* v);
vec3_t vec3_from_vec4(vec4_t* v);
vec2_t vec2_from_vec4(vec4_t* v); 
#endif
