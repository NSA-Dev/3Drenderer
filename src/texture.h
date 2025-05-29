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

extern int texture_width;
extern int texture_height; 

extern const uint8_t REDBRICK_TEXTURE[]; // contains manually provided values for the default texture 
extern upng_t* png_texture; 
extern uint32_t* mesh_texture; 

 

bool load_png_textureData(char* filename); 
#endif 
