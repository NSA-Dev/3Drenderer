#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"
#include "display.h"
#include "vector.h"

#define  N_POINTS (9*9*9)  // Number of cube coordinates


/* Global scope */  
bool is_running = false;
float fov_factor = 128;
/*  vector array declr  */
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS]; 
/* End of globals */

 
bool setup(void);
void process_input(void);
void update(void);
void render(void);

int main(void) {
    is_running = init_win();
    
    if(!setup()) {
        fprintf(stderr, "Setup failed. Shutting down.\n");
        return 1;
    };

    while(is_running) {
       process_input();
       update();
       render();  
    }
    
    free_resources();

    return 0;
}


bool setup(void) {
    // Allocate framebuffer
    framebuffer = (uint32_t*)malloc(sizeof(uint32_t) * win_w * win_h);
    if(!framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer memory.\n");
        return false;  
    }

   //framebuffer texture
   framebuffer_texture = SDL_CreateTexture(
        renderer,           // renderer responsible 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        win_w,
        win_h
   );
   if(!framebuffer_texture) { 
        fprintf(stderr, "Failed to allocate SDL_Texture.\n");
        return false;
   }

   // 9x9x9 cube from -1 to 1
   int  current_point = 0;  
   for(float x = -1; x <= 1; x += 0.25) {
        for(float y = -1; y <= 1; y += 0.25) {
            for(float z = -1; z <= 1; z += 0.25) {
                vec3_t new_point = {.x = x*fov_factor, 
                                    .y = y*fov_factor, 
                                    .z = z*fov_factor
                                   };
                cube_points[current_point++] = new_point;
            }
        }
   }

    return true; 
}

void process_input(void) {
    SDL_Event event; 
    SDL_PollEvent(&event);  // needs a pointer to the event
    
    switch(event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break; 
    }
}

void update(void) {
    for (int i = 0; i < N_POINTS; i++) {
       vec2_t projected =  project(&cube_points[i]);
       projected_points[i] = projected; 
    }
}

void render(void) {
    draw_grid(10, COLOR_LIGHT_GRAY);          // grid spacing
    for(int i = 0; i < N_POINTS; i++) {
        draw_rect(projected_points[i].x + (win_w/2), 
        projected_points[i].y + (win_h/2), 
        4, 
        4, 
        COLOR_ORANGE);
    }
    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    SDL_RenderPresent(renderer); 
}

