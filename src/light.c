#include "light.h"

static light_t light;


void initLight(vec3_t direction) {
        light.direction = direction;
}

uint32_t light_apply_intensity(uint32_t original_color, float percentage) {
   
    // lock between 0 & 1.0
   if(percentage < 0) percentage = 0;
   if(percentage > 1) percentage = 1;

    uint32_t alpha = (original_color & 0xFF000000);
    uint32_t red   = (original_color & 0x00FF0000) * percentage;
    uint32_t green = (original_color & 0x0000FF00) * percentage;
    uint32_t blue  = (original_color & 0x000000FF) * percentage;

    uint32_t res = alpha | (red & 0x00FF0000) | (green & 0x0000FF00) | (blue & 0x000000FF); 

    return res;
}

uint32_t light_apply_intensity_bitwise(uint32_t original_color, float percentage) {   
    // lock between 0 & 1.0
   if(percentage < 0) percentage = 0;
   if(percentage > 1) percentage = 1;

   // >> 16 || >> 8 and so on,  extracts the numeric value of a hexnum 
    uint32_t alpha = (original_color & 0xFF000000);
    uint32_t red   = (uint32_t)(((original_color >> 16) & 0xFF) * percentage);
    uint32_t green = (uint32_t)((original_color >> 8) & 0xFF) * percentage;
    uint32_t blue  = (uint32_t)((original_color & 0xFF) * percentage);

    uint32_t res = alpha | (red << 16) | (green  << 8) | (blue); 

    return res;   
}

// shift the original position by x, y, z units 
void adjustLightPos(float x, float y, float z) {
     light.direction.x += x;
     light.direction.y += y;
     light.direction.z += z; 
};

// overwrite position of the global light source
void setLightPos(float x, float y, float z) {
     light.direction.x = x;
     light.direction.y = y;
     light.direction.z = z; 
};

vec3_t getLightDirection(void) {
    return light.direction; 
}
