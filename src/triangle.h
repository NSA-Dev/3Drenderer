#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

// struct used for storing vertex indices, color and texture information for each face 
typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t; 

// struct used for storing actual (x,y) coordinates for each vert
typedef struct {
    vec2_t points[3];
    tex2_t texcoords[3]; 
    uint32_t color;
    float avg_depth;
} triangle_t; 

void fill_upper_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); 
void fill_lower_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); 
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void swap_triangle_t(triangle_t* a, triangle_t* b);
void draw_textured_triangle(
        int x0, int y0, float u0, float v0,  
        int x1, int y1, float u1, float v1,       
        int x2, int y2, float u2, float v2,
        uint32_t* texture 
        );
void draw_texel(
        int x, int y, uint32_t* texture,
        vec2_t* a, vec2_t* b, vec2_t* c,
        float*  u0, float* v0, float* u1, float* v1,
        float* u2, float* v2 
        ); // draw correct pixel color based on uv coords mapped to the triangle_t

vec3_t computeBarycentric2D(vec2_t* a, vec2_t* b, vec2_t* c, vec2_t* p) ; 
// sorting
int partition(triangle_t* array, int low, int high); 
void qsort_depth(triangle_t* array, int low, int high); 
#endif
