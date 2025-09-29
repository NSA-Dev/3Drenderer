#ifndef LIGHT_H
#define LIGHT_H

#include <stdint.h>
#include "vector.h"

typedef struct {
    vec3_t direction;
} light_t;

void initLight(vec3_t direction); 
uint32_t light_apply_intensity(uint32_t original_color, float percentage);
uint32_t light_apply_intensity_bitwise(uint32_t original_color, float percentage);
void adjustLightPos(float x, float y, float z);
void setLightPos(float x, float y, float z);
vec3_t getLightDirection(void); 
#endif
