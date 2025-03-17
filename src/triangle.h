#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"

// used for storing vertex indices
typedef struct {
    int a;
    int b;
    int c;
} face_t; 

// used for storing actual (x,y) coordinates for each vert
typedef struct {
    vec2_t points[3];
} triangle_t; 

// move to display.c ?
void fill_upper_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); 
void fill_lower_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color); 
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);


#endif
