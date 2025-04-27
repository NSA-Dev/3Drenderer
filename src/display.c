#include "display.h"

/* global init */
render_flags_t rendering_mode;
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


void initialize_rendering_mode(void) {
   rendering_mode.enable_wireframe = false;
   rendering_mode.enable_vertices = false;
   rendering_mode.enable_solid = true;
   rendering_mode.enable_culling = true;
   rendering_mode.enable_flat_shading = false;
   rendering_mode.enable_textured = false;
   rendering_mode.enable_textured_wire = false;   
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
    if(x >= 0 && x < win_w && y >= 0 && y < win_h) {
    framebuffer[(win_w*y)+x] = color;
    } 
}

void draw_bresLine(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx, dy;
    int step_x, step_y;
    int error;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    error = dx - dy;

    step_x = (x1 < x2) ? 1 : -1;
    step_y = (y1 < y2) ? 1 : -1;
   
    while(x1 != x2 || y1 != y2) {
        draw_pixel(x1, y1, color);
        int e2 = 2 * error; // used for balancing
        if(e2 > -dy) {
            error -= dy;
            x1 += step_x;
        }
        if (e2 < dx) {
            error += dx;
            y1 += step_y;
        }
    } 
 
    draw_pixel(x1, y1, color); 
}


void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for(int row = y; row < y + h; row++) {
        for(int col = x; col < x + w; col++) {
           draw_pixel(col, row, color);  
        }
    }
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    draw_bresLine(x1, y1, x2, y2, color);
    draw_bresLine(x2, y2, x3, y3, color);
    draw_bresLine(x3, y3, x1, y1, color);
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

void destroy_window(void) { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit(); 
}
