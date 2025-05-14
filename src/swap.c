#include "swap.h"



void int_swap(int* a, int* b) {
    int temp = *b;  
    *b = *a;
    *a = temp;  
}


void float_swap(float* a, float* b) {
    float temp = *b;
    *b = *a;
    *a = temp;  
}
