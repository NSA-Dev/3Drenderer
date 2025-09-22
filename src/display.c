#include "display.h"

#define DEFAULT_WINDOW_W 800
#define DEFAULT_WINDOW_H 600


static Display display; 
static RenderingMode display_renderingMode;
static CullMethod display_cullMethod;
static LightMethod display_lightMethod; 
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
    if(!SDL_GetCurrentDisplayMode(0, &display_mode)){
        display.windowWidth = DEFAULT_WINDOW_W;
        display.windowHeight = DEFAULT_WINDOW_H; 
    } else {
        display.windowWidth = display_mode.w;
        display.windowHeight = display_mode.h;
    }
    /* Handle window creation */
    display.windowInstance = SDL_CreateWindow(
            NULL,                   // winTitle
            SDL_WINDOWPOS_CENTERED, // pos X
            SDL_WINDOWPOS_CENTERED, // pos Y
            display.windowWidth,                  // width
            display.windowHeight,                  // height
            SDL_WINDOW_BORDERLESS   // winMode                    
    );
    
    if(!display.windowInstance) {
        fprintf(stderr, "Error creating SDL window.\n");
        return false; 
    }   
    /* Handle renderer creation */
    display.rendererInstance = SDL_CreateRenderer(
            display.windowInstance,                 
            -1,                     // the index of a display device -1 default
            0                       // extra rend flags 
    );
    if(!display.rendererInstance) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    } 
    // Allocate framebuffer
    display.framebuffer = (uint32_t*)malloc(sizeof(uint32_t) * display.windowWidth 
                                                             * display.windowHeight);
    if(!display.framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer memory.\n");
        return false;  
    }
    // Allocate z-buffer
    display.zBuffer = (float*)malloc(sizeof(float) * display.windowWidth 
                                                   * display.windowHeight); 
    if(!display.zBuffer) {
		fprintf(stderr, "Failed to allocate z-buffer memory.\n");
		return false; 
	}
   // framebuffer texture
   display.framebufferTexture = SDL_CreateTexture(
        display.rendererInstance,           // renderer responsible
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        display.windowWidth,
        display.windowHeight
   );
   if(!display.framebufferTexture) { 
        fprintf(stderr, "Failed to allocate SDL_Texture.\n");
        return false;
   }  
    
    return true; 
}

void draw_grid(int spacing, uint32_t color) {
    // draws a dot grid
    for(int row = 0; row <  display.windowHeight; row += spacing) {
        for(int col = 0; col < display.windowWidth; col += spacing) {
           draw_pixel(col, row, color); 
        }
    }
}
void draw_pixel(int x, int y, uint32_t color) {
    if(x >= 0 && x <  display.windowWidth && y >= 0 && y <  display.windowHeight) {
    display.framebuffer[(display.windowWidth*y)+x] = color;
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
        display.framebufferTexture,
        NULL,
        display.framebuffer,
       (int)(display.windowWidth * sizeof(uint32_t))  // (int) typecast for SDL
    );

    SDL_RenderCopy(display.rendererInstance, display.framebufferTexture, NULL, NULL);
}

void clear_framebuffer(uint32_t color) {
    for(int i = 0; i < (display.windowWidth * display.windowHeight); i++) {
        display.framebuffer[i] = color; 
    } 
}

void clear_Zbuffer(void) {
	for(int i = 0; i < (display.windowWidth * display.windowHeight); i++) {
		display.zBuffer[i] = 1.0; // Z-buffer starts with 1.0 in left handed coord sys by convention   
	}

}

void destroy_window(void) { 
    SDL_DestroyRenderer(display.rendererInstance);
    SDL_DestroyWindow(display.windowInstance);
    SDL_Quit(); 
}


int getWindowWidth(void) {
    return display.windowWidth;
}
int getWindowHeight(void) {
    return display.windowHeight;
}