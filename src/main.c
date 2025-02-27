#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define WIN_W 1920
#define WIN_H 1080

/* Global scope */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool isRunning = false; 
/* End of globals */


bool init_win(void); 
void setup(void);
void process_input(void);
void update(void);
void render(void); 

int main(void) {

    isRunning = init_win();
    
    setup();

    while(isRunning) {
       process_input();
       update();
       render();  
    }
    return 0;
}



bool init_win(void) {

    /* Steps to create renderer window in SDL 
     * 1. initalize SDL (SDL_Init)
     * 2. create window instance
     * 3. attach renderer instance to the window
     * */
    

    if(SDL_Init(SDL_INIT_EVERYTHING)!= 0) {
        fprintf(stderr, "Error intitalizing SDL.\n");
        return false; 
    }
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
        fprintf(stderr, "Error creating SDL window\n");
        return false; 
    }   
   
    renderer = SDL_CreateRenderer(
            window,                 // winInstance
            -1,                     // the index of display device -1 default
            0                       // extra rend flags 
            );

    if(!renderer) {
        fprintf(stderr, "Error creating SDL renderer.\n");
    }   
    
    return true; 
}

void setup(void) {
    ;
}
void process_input(void) {
    SDL_Event event; 
    
    SDL_PollEvent(&event);  // needs pointer to the event
    switch(event.type) {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                isRunning = false;
            break; 
    }


}
void update(void) {
    ;
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
