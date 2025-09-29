#include "display.h"

/* global init */
static RenderingMode renderingMode;
static CullMethod cullMethod;
static LightMethod lightMethod; 
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* framebuffer = NULL;
static float* Zbuffer = NULL; 
static SDL_Texture* framebuffer_texture = NULL;
static int win_w = 800;    // fallback value
static int win_h = 600;    // fallback value

bool init_window(void) {

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
       // Allocate framebuffer
    framebuffer = (uint32_t*)malloc(sizeof(uint32_t) * win_w * win_h);
    if(!framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer memory.\n");
        return false;  
    }
    // Allocate z-buffer
    Zbuffer = (float*)malloc(sizeof(float) * win_w * win_h); 
    if(!Zbuffer) {
		fprintf(stderr, "Failed to allocate z-buffer memory.\n");
		return false; 
	}
   // framebuffer texture
   framebuffer_texture = SDL_CreateTexture(
        renderer,           // renderer responsible
        SDL_PIXELFORMAT_RGBA32,
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


void draw_grid(int spacing, uint32_t color) {
    // draws a dot grid
    for(int row = 0; row <  win_h; row += spacing) {
        for(int col = 0; col < win_w; col += spacing) {
           draw_pixel(col, row, color); 
        }
    }
}
void draw_pixel(int x, int y, uint32_t color) {
    if(x < 0 || x >= win_w || y < 0 || y >= win_h) {
        return;
    } 
    framebuffer[(win_w*y)+x] = color;
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
    SDL_RenderPresent(renderer);
}

void clear_framebuffer(uint32_t color) {
    for(int i = 0; i < (win_w * win_h); i++) {
        framebuffer[i] = color; 
    } 
}

void clear_Zbuffer(void) {
	for(int i = 0; i < (win_w * win_h); i++) {
		Zbuffer[i] = 1.0; // Z-buffer starts with 1.0 in left handed coord sys by convention   
	}

}

void destroy_window(void) { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    if(framebuffer != NULL) free(framebuffer);
    if(Zbuffer != NULL) free(Zbuffer); 
}

int getWindowWidth(void) {
    return win_w;
}
int getWindowHeight(void) {
    return win_h;
}

void setRenderingMode(RenderingMode mode) {
    renderingMode = mode; 
}
void setLightMethod(LightMethod method) {
    lightMethod = method;
} 
void setCullMethod(CullMethod method) {
    cullMethod = method; 
}
 
LightMethod getLightMethod(void) {
    return lightMethod;
}

CullMethod getCullMethod(void) {
    return cullMethod;
}
RenderingMode getRenderingMode(void) {
    return renderingMode; 
}

float* getZbufferPtr(void) {
    return Zbuffer; // raw Zbuffer access, here for ease of access but should be avoided 
}

float getZbufferAt(int x, int y) {
    if(x < 0 || x >= win_w || y < 0 || y >= win_h) {
        return 1.0; 
    }
    return Zbuffer[(win_w * y) + x]; 
}
void updateZbufferAt(int x, int y, float val) {
    if(x < 0 || x >= win_w || y < 0 || y >= win_h) {
        return; 
    }
    Zbuffer[(win_w * y) + x] = val; 
}

bool renderTexture(void) {
    return (renderingMode == RENDER_TEXTURED || renderingMode == RENDER_TEXTURED_WIRE); 
}
bool renderWire(void) {
    return (renderingMode == RENDER_WIRE || renderingMode == RENDER_WIRE_VERTEX 
            || renderingMode == RENDER_SOLID_WIRE || renderingMode == RENDER_TEXTURED_WIRE);
}
bool renderVerts(void) {
   return (renderingMode == RENDER_WIRE_VERTEX); 
}
bool renderSolids(void) {
    return (renderingMode == RENDER_SOLID); 
}