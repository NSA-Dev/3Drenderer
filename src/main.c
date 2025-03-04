#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"



/* Global scope */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* framebuffer_texture = NULL; 
uint32_t* framebuffer = NULL; 
bool is_running = false;
int win_w = 800;    // fallback value
int win_h = 600;    // fallback value
/* End of globals */


bool init_win(void); 
bool setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);
void draw_grid(int spacing);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void clear_framebuffer(uint32_t color); 
void render_framebuffer(void);

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

    /* Query display's video mode from main(0) display */
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    win_w = display_mode.w;
    win_h = display_mode.h;

    /* Handle window creation */
    window = SDL_CreateWindow(
            NULL,                   // winTitle
            SDL_WINDOWPOS_CENTERED, // pos X
            SDL_WINDOWPOS_CENTERED, // pos Y
            win_w,                  // width
            win_h,                  // height
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
    
    /* True fullscreen mode */
   // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN); 
    return true; 
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
    draw_grid(10);          // grid spacing
    draw_rect(800, 800, 200, 100, COLOR_ORANGE); // For testing
    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    SDL_RenderPresent(renderer); 
}

void free_resources(void) {
    if(framebuffer != NULL) free(framebuffer); 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}

void draw_grid(int spacing) {
    // draws a dot grid
    for(int row = 0; row <  win_h; row += spacing) {
        for(int col = 0; col < win_w; col += spacing) {
            framebuffer[(win_w*row)+col] = COLOR_LIGHT_GRAY;
        }
    }
}
void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for(int row = y; row < y + h; row++) {
        for(int col = x; col < x + w; col++) {
            framebuffer[(win_w*row)+col] = color; 
        }
    }
}
void clear_framebuffer(uint32_t color) {
    for(int i = 0; i < (win_w * win_h); i++) {
        framebuffer[i] = color; 
    } 
}

void render_framebuffer(void) {
    SDL_UpdateTexture(
        framebuffer_texture,
        NULL,
        framebuffer,
       (int)(win_w * sizeof(uint32_t))  // (int) typecast for SDL
    );

    SDL_RenderCopy(renderer, framebuffer_texture, NULL, NULL);
}
