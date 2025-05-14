#include <stdlib.h>
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
        int mX = (x2-x0) * (y1-y0) / (float)(y2 - y0) + x0;

    // draw upper half of the triangle
        fill_upper_half(x0, y0, x1, y1, mX, mY, color); 
    // draw lower half of the triangle
        fill_lower_half(x1, y1, mX, mY, x2, y2, color); 
    }
}


void draw_textured_triangle(
        int x0, int y0, float u0, float v0,  
        int x1, int y1, float u1, float v1,       
        int x2, int y2, float u2, float v2,
        uint32_t* texture) {

    // only difference from regular draw_filledTriangle is going pixel by pixel 
    // instead of connecting by drawLine()
    
    // sort by y (y0 < y1 < y2) to ensure correct vertex orientation
    if(y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap (&v0, &v1); 
    }

    if(y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2); 
    }
   // extra check to avoid incorrect sorting  
    if (y0 > y1) { 
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap (&v0, &v1); 
    }
    

  // We begin by finding slopes for left & right leg to determine how far we need to go
  // in Y direction, and then calculate start & end of the current scanline (xStart, Xend)
  // After we have correct coordinates, we loop the line pixel by pixel while grabbing texture colors
  // and assigning them to current pixel. 

  // determine inverse slope starting at y0 (topmost vertex) 
  float invSlope_L = 0;
  float invSlope_R = 0;

  if(y1-y0 != 0) invSlope_L = (float) (x1 - x0) / abs(y1 - y0); // left leg slope 
  if(y2-y0 != 0) invSlope_R = (float)(x2 - x0) / abs(y2 - y0); // right leg slope 
  
  
  // draw only in case if we vertical distance != 0
  if(y0 != y1) {

    // I'd argue this interval should be inclusive   
    for(int y = y0;  y <= y1; y++) { 
        int xStart = x1 + (y - y1) * invSlope_L;
        int xEnd = x0 + (y - y0) * invSlope_R;  
    
        if(xEnd < xStart) int_swap(&xStart, &xEnd); // check if we are going L -> R swap otherwise
    
        for(int x = xStart; x <= xEnd; x++) {
            // grab texture data pixel by pixel
            draw_pixel(x, y, 0xFFFF00FF /*texture data color*/);  
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

