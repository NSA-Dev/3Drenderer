#ifndef TEXTURE_H
#define TEXTURE_H
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "upng.h"

typedef struct {
    float u;
    float v;
} tex2_t;



 

bool load_png_textureData(char* filename);
uint8_t* fetchDefaultTexture(void); 
#endif 
