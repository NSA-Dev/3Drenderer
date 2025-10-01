#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"
#include "upng.h"

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
    vec4_t points[3];
    tex2_t texcoords[3]; 
    uint32_t color;
    upng_t* texture; 
} triangle_t; 

void fill_upper_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); // legacy helper for draw_filled_triangle()
void fill_lower_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); // legacy helper for draw_filled_triangle()
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); // legacy function
void draw_solid_triangle(triangle_t* triangle); // draws solid triangle, using z-buffer  
void swap_triangle_t(triangle_t* a, triangle_t* b);
void draw_textured_triangle(
        int x0, int y0, float z0, float w0, float u0, float v0,  
        int x1, int y1, float z1, float w1, float u1, float v1,       
        int x2, int y2, float z2, float w2, float u2, float v2,
        upng_t* texture 
        );
        
// draw correct pixel color based on uv coords mapped to the triangle_t
void draw_texel(
        int x, int y, upng_t* texture, vec4_t* a, vec4_t* b, vec4_t* c,
        float*  u0, float* v0, float* u1, float* v1, float* u2, float* v2 ); 

void draw_triangle_pixel(int x, int y, uint32_t color, vec4_t* vert_a, vec4_t* vert_b, vec4_t* vert_c); // helper function for draw_solid_triangle()
vec3_t computeBarycentric2D(vec2_t* a, vec2_t* b, vec2_t* c, vec2_t* p) ; 

#endif
