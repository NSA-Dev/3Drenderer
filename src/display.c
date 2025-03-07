#include "display.h"

/* global init */
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
uint32_t* framebuffer = NULL;
SDL_Texture* framebuffer_texture = NULL;
int win_w = 800;    // fallback value
int win_h = 600;    // fallback value

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

void draw_grid(int spacing, uint32_t color) {
    // draws a dot grid
    for(int row = 0; row <  win_h; row += spacing) {
        for(int col = 0; col < win_w; col += spacing) {
           draw_pixel(col, row, color); 
        }
    }
}
void draw_pixel(int x, int y, uint32_t color) {
    if(x < win_w && y < win_h) {
    framebuffer[(win_w*y)+x] = color;
    } 
}
void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for(int row = y; row < y + h; row++) {
        for(int col = x; col < x + w; col++) {
           draw_pixel(col, row, color);  
        }
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

void clear_framebuffer(uint32_t color) {
    for(int i = 0; i < (win_w * win_h); i++) {
        framebuffer[i] = color; 
    } 
}

void free_resources(void) {
    if(framebuffer != NULL) free(framebuffer); 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}
