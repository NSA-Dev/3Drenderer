#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "triangle.h"
#include "display.h"
#include "swap.h"
 

void fill_upper_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // y = mx + b - but inverse slope is needed
    // calculate slopes
    float inv_m1 = (float) (x1 - x0) / (y1 - y0); 
    float inv_m2 = (float) (x2 - x0) / (y2 - y0);

    //designate starting point
    float x_start = x0; 
    float x_end = x0; 

    // loop all rows
    for(int y = y0; y <= y2; y++) {
        draw_bresLine(x_start, y, x_end, y, color); 
        x_start += inv_m1;
        x_end += inv_m2;
    }
} 

void fill_lower_half(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    // y = mx + b same idea as above except x2 y2 is origin & going up
    
    // calculate slopes
    float inv_m1 = (float) (x2 - x0) / (y2 - y0); 
    float inv_m2 = (float) (x2 - x1) / (y2 - y1);

    //designate starting point (x2, y2)
    float x_start = x2;
    float x_end = x2;

    for(int y = y2; y >= y1; y--) {
        draw_bresLine(x_start, y, x_end, y, color);
        x_start -= inv_m1;
        x_end -= inv_m2;
    }
}
void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    //  sort by y (y0 < y1 < y2)
    if(y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if(y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    
    if (y0 > y1) { 
        int_swap(&y0, &y1);
        int_swap(&x0, &x1); 
    }

    // drawing
    if(y1 == y2) {
        fill_upper_half(x0, y0, x1, y1, x2, y2, color);      
    } else if (y0 == y1) {
        fill_lower_half(x0, y0, x1, y1, x2, y2, color); 
    } else {

    // Calculate median point (based on triangle similarity property)
        int mY = y1;
        int mX = (((x2-x0) * (y1-y0)) / (float)(y2 - y0)) + x0;

    // draw upper half of the triangle
        fill_upper_half(x0, y0, x1, y1, mX, mY, color); 
    // draw lower half of the triangle
        fill_lower_half(x1, y1, mX, mY, x2, y2, color); 
    }
}


///////////////////////////////////////////////////////////////////////////////
// Return the barycentric weights alpha, beta, and gamma for point p
///////////////////////////////////////////////////////////////////////////////
//
//         (B)
//         /|\
//        / | \
//       /  |  \
//      /  (P)  \
//     /  /   \  \
//    / /       \ \
//   //           \\
//  (A)------------(C)
//
///////////////////////////////////////////////////////////////////////////////

vec3_t computeBarycentric2D(vec2_t* a, vec2_t* b, vec2_t* c, vec2_t* p) {
   // find vectors in between the vertices & p
   vec2_t ac = vec2_sub(c, a);
   vec2_t ab = vec2_sub(b, a);
   vec2_t ap = vec2_sub(p, a);
   vec2_t pc = vec2_sub(c, p);
   vec2_t pb = vec2_sub(b, p); 

   float area_ABC = vec2_cross(&ac, &ab);
   
   //if(area_ABC <=0) return (vec3_t) {0, 0, 0};
   
   float alpha = vec2_cross(&pc, &pb) / area_ABC;
   float beta = vec2_cross(&ac, &ap) / area_ABC;
   float gamma = 1 - (alpha + beta); 

   // package everything into vec3
   vec3_t weights = {alpha, beta, gamma};
 
   return weights;     
}


void draw_texel(
        int x, int y, uint32_t* texture,
        vec4_t* a, vec4_t* b, vec4_t* c,
        float*  u0, float* v0, float* u1, float* v1,
        float* u2, float* v2 
) {
	// package xy compononets for barycentric weights computation
	vec2_t a_xy = vec2_from_vec4(a);
	vec2_t b_xy = vec2_from_vec4(b);
	vec2_t c_xy = vec2_from_vec4(c); 
	
	
	// compute weights relative to the point P (desired x & y screen pos)
    vec2_t pointP = {x, y};
    vec3_t weights = computeBarycentric2D(&a_xy, &b_xy, &c_xy, &pointP); 

    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z; 
 
	//values for interpolated uv and 1/w (containing z) per current pixel
    float interU, interV, interW_inverted; 
    
    // get interpolated uv coords, by weighing them with alpha, beta & gamma
    interU = (*u0 / a->w) * alpha + (*u1 / b->w) * beta + (*u2 / c->w) * gamma; 
    interV = (*v0 / a->w) * alpha + (*v1 / b->w) * beta + (*v2 / c->w) * gamma; 
    
	// find interpolated 1/w
	interW_inverted = (1 / a->w) * alpha + (1 / b->w) * beta + (1 / c->w) * gamma; 
	
	// finally divide this back to undo the perspective distortion
	interU /= interW_inverted;
	interV /= interW_inverted; 

    // scale uv to texture H x W 
    int textureX = abs((int)(interU * texture_width));
    int textureY = abs((int)(interV * texture_height)); 


	// clamp the array index before passing to the draw_pixel
	int texIndex = ((texture_width * textureY) + textureX) % (texture_width * texture_height); 
	draw_pixel(x, y, mesh_texture[texIndex]);
	
     
}

// I am using round() in contrast to the lecture materials, might cause issues down the line
void draw_textured_triangle(
        int x0, int y0, float z0, float w0, float u0, float v0,  
        int x1, int y1, float z1, float w1, float u1, float v1,       
        int x2, int y2, float z2, float w2, float u2, float v2,
        uint32_t* texture 
        ) {

    // only difference from regular draw_filledTriangle is going pixel by pixel 
    // instead of connecting by drawLine()
    
    // sort by y (y0 < y1 < y2) to ensure correct vertex orientation
    if(y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap (&v0, &v1); 
    }

    if(y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2); 
    }
   // extra check to avoid incorrect sorting  
    if (y0 > y1) { 
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap (&v0, &v1); 
    }
    
  // Flip the v-s (v grows downwards in .obj)
  v0 = 1.0 - v0;
  v1 = 1.0 - v1;
  v2 = 1.0 - v2;   

  // put vertex data in vec2_t for later usage
  vec4_t a = {x0, y0, z0, w0};
  vec4_t b = {x1, y1, z1, w1};
  vec4_t c = {x2, y2, z2, w2};   

    
  // UPPER HALF
  // We begin by finding slopes for left & right leg to determine how far we need to go
  // in Y direction, and then calculate start & end of the current scanline (xStart, Xend)
  // After we have correct coordinates, we loop the line pixel by pixel while grabbing texture colors
  // and assigning them to current pixel. 


  // determine inverse slope starting at y0 (topmost vertex) 
  float invSlope_L = 0;
  float invSlope_R = 0;

  if(y1-y0 != 0) invSlope_L = (float) (x1 - x0) / abs(y1 - y0); // left leg slope 
  if(y2-y0 != 0) invSlope_R = (float)(x2 - x0) / abs(y2 - y0); // right leg slope 
  
  
  // draw only in case if vertical distance != 0
  if(y1 - y0 != 0) {
   
    for(int y = y0;  y <= y1; y++) { 
        int xStart = (x1 + (y - y1) * invSlope_L); // added rounding for consistency (same for bot)
        int xEnd = (x0 + (y - y0) * invSlope_R);  
    
        if(xEnd < xStart) int_swap(&xStart, &xEnd); // check if we are going L -> R swap otherwise
    
    // Note x <= xEnd
        for(int x = xStart; x <= xEnd; x++) {
            // grab texture data pixel by pixel
             draw_texel(x, y, texture, &a, &b, &c, &u0, &v0, &u1, &v1, &u2, &v2);  
        }
    }  
  }
  
  // LOWER HALF 
  invSlope_L = 0;
  invSlope_R = 0;

  if(y2-y1 != 0) invSlope_L = (float) (x2 - x1) / abs(y2 - y1); // left leg slope 
  if(y2-y0 != 0) invSlope_R = (float)(x2 - x0) / abs(y2 - y0); // right leg slope 
  
  
  // draw only in case if vertical distance != 0
  if(y2 - y1 != 0) {

    // I'd argue this interval should be inclusive   
    for(int y = y1;  y <= y2; y++) { 
        int xStart = x1 + (y - y1) * invSlope_L;
        int xEnd = (x0 + (y - y0) * invSlope_R);  
    
        if(xEnd < xStart) int_swap(&xStart, &xEnd); // check if we are going L -> R swap otherwise
    
        for(int x = xStart; x <= xEnd; x++) {
            // grab texture data pixel by pixel
            draw_texel(x, y, texture, &a, &b, &c, &u0, &v0, &u1, &v1, &u2, &v2);  
        }
    }  
  }

}

void swap_triangle_t(triangle_t* a, triangle_t* b) {
    triangle_t temp = *a;
    *a = *b;
    *b = temp; 
}


int partition(triangle_t* array, int low, int high) {
    float pivot = array[high].avg_depth;
    int i = low;


    for(int j = low; j < high; j++) {
        if(array[j].avg_depth >= pivot) {
            swap_triangle_t(&array[i], &array[j]);
            i++;
        }
    }

    swap_triangle_t(&array[i], &array[high]);

    return i; 
} 

void qsort_depth(triangle_t* array, int low, int high) {
    if (low < high) {
        int pivot = partition(array, low, high); 
        qsort_depth(array, low, pivot - 1);
        qsort_depth(array, pivot + 1, high);
    } 
}

