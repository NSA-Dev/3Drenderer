#include "triangle.h"
#include "display.h"

void int_swap(int* a, int* b) {
    int temp = *b;  
    *b = *a;
    *a = temp; 
}

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
        fill_upper_half(x0, y0, x1, y1, mX, mY, color); // WIP
    // draw lower half of the triangle
        fill_lower_half(x1, y1, mX, mY, x2, y2, color); // WIP
    }
}

