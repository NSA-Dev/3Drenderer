#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"

// used for storing vertex indices
typedef struct {
    int a;
    int b;
    int c;
    tex2_t a_uv;
    tex2_t b_uv;
    tex2_t c_uv;
    uint32_t color;
} face_t; 

// used for storing actual (x,y) coordinates for each vert
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
// sorting
int partition(triangle_t* array, int low, int high); 
void qsort_depth(triangle_t* array, int low, int high);
// TODO add void draw_textured_triangle(); 
#endif
