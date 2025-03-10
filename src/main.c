#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"
#include "display.h"
#include "vector.h"


#define  R_LIMIT (2 * 3.14159265) // rotation limit for view controls

  
bool is_running = false;
int previous_frame_time = 0; // ms
float fov_factor = 640;
/*  vector  declr  */
vec3_t camera_pos = {.x = 0, .y = 0, .z = 5};
vec3_t cube_rotation = {.x = 0, .y = 0, .z = 0}; 
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
            else if (event.key.keysym.sym == SDLK_LEFT)
                cube_rotation.y += 0.1;
            else if(event.key.keysym.sym == SDLK_RIGHT)
                cube_rotation.y -= 0.1;
            else if (event.key.keysym.sym == SDLK_UP)
                cube_rotation.x += 0.1;
            else if(event.key.keysym.sym == SDLK_DOWN)
                cube_rotation.x -= 0.1;
            break; 
    }
}

// remains in main.c for now
vec2_t project(vec3_t* p) {
    vec2_t projected = {
        .x = (fov_factor * p->x) / p->z,
        .y = (fov_factor * p->y) / p->z
    };
    return projected;
}

void update(void) {
    //While not enough ticks passed, lock the update until true
    int time_to_wait = FTT - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0) SDL_Delay(time_to_wait);     
    previous_frame_time = SDL_GetTicks(); 

    for (int i = 0; i < N_POINTS; i++) { 
        vec3_t temp = cube_points[i];
        if(cube_rotation.y != 0) vec3_rotate_y(&temp, cube_rotation.y);
        if(cube_rotation.x != 0) vec3_rotate_x(&temp, cube_rotation.x);
        // factor the camera in
       temp.z -= camera_pos.z;
       //project as normal
       vec2_t projected =  project(&temp);
       projected_points[i] = projected; 
    }
    // check rotations for overflow
    if(cube_rotation.x > R_LIMIT) cube_rotation.x -= R_LIMIT;
    if(cube_rotation.x < R_LIMIT) cube_rotation.x += R_LIMIT;
    if(cube_rotation.y > R_LIMIT) cube_rotation.y -= R_LIMIT;
    if(cube_rotation.y < R_LIMIT) cube_rotation.y += R_LIMIT;   
}

void render(void) {
    draw_grid(10, COLOR_LIGHT_GRAY);          // grid spacing
   /*
    for(int i = 0; i < N_POINTS; i++) {
        draw_rect(
        projected_points[i].x + (win_w/2), 
        projected_points[i].y + (win_h/2), 
        4, 
        4, 
        COLOR_ORANGE);
    } <<<<< Legacy cube rendering */
    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    SDL_RenderPresent(renderer); 
}

