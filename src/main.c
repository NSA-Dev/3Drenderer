#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"

#define WIN_W 1920
#define WIN_H 1080

/* Global scope */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* frame_buffer = NULL; 
bool is_running = false; 
/* End of globals */


bool init_win(void); 
void setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);

int main(void) {
    is_running = init_win();
    
    setup();

    while(is_running) {
       process_input();
       update();
       render();  
    }
    
    free_resources();
    return 0;
}



bool init_win(void) {

    /* Steps to create renderer window in SDL 
     * 1. initalize SDL (SDL_Init)
     * 2. create window instance
     * 3. attach renderer instance to the window
     * */
    
    /* Initialization */
    if(SDL_Init(SDL_INIT_EVERYTHING)!= 0) {
        fprintf(stderr, "Error intitalizing SDL.\n");
        return false; 
    }

    /* Handle window creation */
                                    // ARGS:
    window = SDL_CreateWindow(
            NULL,                   // winTitle
            SDL_WINDOWPOS_CENTERED, // pos X
            SDL_WINDOWPOS_CENTERED, // pos Y
            WIN_W,                  // width
            WIN_H,                  // height
            SDL_WINDOW_BORDERLESS   // winMode                    
    );
    
    if(!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false; 
    }   
    /* Handle renderer creation */
    renderer = SDL_CreateRenderer(
            window,                 // winInstance
            -1,                     // the index of a display device -1 default
            0                       // extra rend flags 
    );
    if(!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }   
    
    return true; 
}

void setup(void) {
    // Allocate framBuffer
    frame_buffer = (uint32_t*)malloc(sizeof(uint32_t) * WIN_W * WIN_H);
    if(frame_buffer == NULL) {
        fprintf(stderr, "Failed to allocate frame buffer memory.\n");
        return;  
    } 
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
    SDL_RenderPresent(renderer); 
}

void free_resources(void) {
    if(frame_buffer != NULL) free(frame_buffer); 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}
