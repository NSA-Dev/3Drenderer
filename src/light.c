#include "light.h"

uint32_t light_apply_intensity(uint32_t original_color, float percentage) {
    
    uint32_t alpha = (original_color & 0xFF000000);
    uint32_t red   = (original_color & 0x00FF0000) * percentage;
    uint32_t green = (original_color & 0x0000FF00) * percentage;
    uint32_t blue  = (original_color & 0x000000FF) * percentage;

    uint32_t res = alpha | (red & 0x00FF0000) | (green & 0x0000FF00) | (blue & 0x000000FF); 

    return res;
}
