#include "display.h"

#define DEFAULT_WINDOW_W 800
#define DEFAULT_WINDOW_H 600


static Display display; 
bool disp_initWin(void) {

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
    if(SDL_GetCurrentDisplayMode(0, &display_mode)){
        display.windowWidth = DEFAULT_WINDOW_W;
        display.windowHeight = DEFAULT_WINDOW_H;
        display.aspectRatio_Y = (float) DEFAULT_WINDOW_H / DEFAULT_WINDOW_W;
        display.aspectRatio_X =(float) DEFAULT_WINDOW_W / DEFAULT_WINDOW_H; 
    } else {
        display.windowWidth = display_mode.w;
        display.windowHeight = display_mode.h;
        display.aspectRatio_Y = (float) display_mode.h / display_mode.w;
        display.aspectRatio_X =(float) display_mode.w / display_mode.h;
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
    display.SDLrendererInstance = SDL_CreateRenderer(
            display.windowInstance,                 
            -1,                     // the index of a display device -1 default
            0                       // extra rend flags 
    );
    if(!display.SDLrendererInstance) {
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
        display.SDLrendererInstance,           // renderer responsible
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

void disp_drawGrid(int spacing, uint32_t color) {
    // draws a dot grid
    for(int row = 0; row <  display.windowHeight; row += spacing) {
        for(int col = 0; col < display.windowWidth; col += spacing) {
           disp_drawPixel(col, row, color); 
        }
    }
}
void disp_drawPixel(int x, int y, uint32_t color) {
    if(x >= 0 && x <  display.windowWidth && y >= 0 && y <  display.windowHeight) {
    display.framebuffer[(display.windowWidth*y)+x] = color;
    } 
}

void disp_drawBresLine(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx, dy;
    int step_x, step_y;
    int error;

    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

    error = dx - dy;

    step_x = (x1 < x2) ? 1 : -1;
    step_y = (y1 < y2) ? 1 : -1;
   
    while(x1 != x2 || y1 != y2) {
        disp_drawPixel(x1, y1, color);
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
 
    disp_drawPixel(x1, y1, color); 
}


void disp_drawRect(int x, int y, int w, int h, uint32_t color) {
    for(int row = y; row < y + h; row++) {
        for(int col = x; col < x + w; col++) {
           disp_drawPixel(col, row, color);  
        }
    }
}

void disp_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color) {
    disp_drawBresLine(x1, y1, x2, y2, color);
    disp_drawBresLine(x2, y2, x3, y3, color);
    disp_drawBresLine(x3, y3, x1, y1, color);
} 

void disp_renderFramebuffer(void) {
    SDL_UpdateTexture(
        display.framebufferTexture,
        NULL,
        display.framebuffer,
       (int)(display.windowWidth * sizeof(uint32_t))  // (int) typecast for SDL
    );

    SDL_RenderCopy(display.SDLrendererInstance, display.framebufferTexture, NULL, NULL);
}

void disp_clearFramebuffer(uint32_t color) {
    for(int i = 0; i < (display.windowWidth * display.windowHeight); i++) {
        display.framebuffer[i] = color; 
    } 
}

void disp_clearZbuffer(void) {
	for(int i = 0; i < (display.windowWidth * display.windowHeight); i++) {
		display.zBuffer[i] = 1.0; // Z-buffer starts with 1.0 in left handed coord sys by convention   
	}

}

void disp_destroyWindow(void) { 
    SDL_DestroyRenderer(display.SDLrendererInstance);
    SDL_DestroyWindow(display.windowInstance);
    SDL_Quit(); 
}

int disp_getWindowWidth(void) {
    return display.windowWidth;
}

int disp_getWindowHeight(void) {
    return display.windowHeight;
}

float disp_getAspectY(void) {
    return display.aspectRatio_Y;
}

float disp_getAspectX(void) {
    return display.aspectRatio_X; 
}


void disp_updateSDL(void) {
    SDL_RenderPresent(display.SDLrendererInstance); 
}

float* disp_getZbufferPtr(void) {
    return display.zBuffer; 
}

void disp_free(void) {
    if(display.framebuffer != NULL) free(display.framebuffer);
    if(display.zBuffer != NULL) free(display.zBuffer);
}    