#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"
#include "display.h"
#include "vector.h"


/* Global scope */  
bool is_running = false; 
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
    // vector test
    vec2_t vector = {3.5, 4.5};
    printf("Vector created %.1f %.1f\n", vector.x, vector.y);

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
            break; 
    }
}

void update(void) {
    /* TODO frame update function */
}

void render(void) {
    SDL_SetRenderDrawColor(
            renderer,       // renderer instance
            255,            // R
            0,              // G
            0,              // B
            255             // A            
            );
    SDL_RenderClear(renderer);
    draw_grid(10, COLOR_LIGHT_GRAY);          // grid spacing
    draw_rect(300, 200, 300, 150, COLOR_ORANGE); // For testing
    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    SDL_RenderPresent(renderer); 
}

