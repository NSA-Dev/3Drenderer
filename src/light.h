#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"


typedef struct {
    vec3_t direction; 
} light_t;

extern light_t global_light;

uint32_t light_apply_intensity(uint32_t original_color, float percentage);
uint32_t light_apply_intensity_bitwise(uint32_t original_color, float percentage);
void adjust_global_ight_pos(float x, float y, float z);
void set_global_light_pos(float x, float y, float z); 
#endif
