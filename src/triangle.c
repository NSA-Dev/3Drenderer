#include "triangle.h"

void int_swap(int* a, int* b) {
    int temp = *b; 
    *b = *a;
    a* = temp; 
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
    
    // Calculate median point (based on triangle similarity property)
    int mY = y1;
    int mX = (x2-x0) * (y1-y0) / (float)(y2 - y0) + x0;

    // draw upper half of the triangle
    draw_flat_bottom(x0, y0, x1,y1, mX, mY, color); // WIP
    // draw lower half of the triangle
    draw_flat_top(x1, y1, mX, mY, x2, y2, color); // WIP

}

