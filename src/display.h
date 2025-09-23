#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct {
	int	windowWidth;
	int windowHeight;
	SDL_Window* windowInstance;
	SDL_Renderer* SDLrendererInstance;
	uint32_t* framebuffer;
	float* zBuffer;
	SDL_Texture* framebufferTexture;
	float aspectRatio_Y;
	float aspectRatio_X; 
} Display;


/* signatures */
bool disp_initWin(void); 
void disp_drawGrid(int spacing, uint32_t color);
void disp_drawPixel(int x, int y, uint32_t color);
void disp_drawBresLine(int x1, int y1, int x2, int y2, uint32_t color);
void disp_drawRect(int x, int y, int w, int h, uint32_t color);
void disp_drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color); // uses Bresenheim's
void disp_renderFramebuffer(void);
void disp_clearFramebuffer(uint32_t color);
void disp_clearZbuffer(void);
void disp_destroyWindow(void);
int disp_getWindowWidth(void);
int disp_getWindowHeight(void);
float disp_getAspectY(void);
float disp_getAspectX(void);
void disp_updateSDL(void);
float* disp_getZbufferPtr(void);
void disp_free(void);  
#endif
