#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


#define FPS 60
#define FTT (1000 / FPS)

typedef enum {
	RENDER_WIRE,
	RENDER_WIRE_VERTEX,
	RENDER_SOLID,
	RENDER_SOLID_WIRE,
	RENDER_TEXTURED,
	RENDER_TEXTURED_WIRE
} RenderingMode;

typedef enum  {
	CULL_NONE,
	CULL_BACKFACE
} CullMethod;

typedef enum {
	LIGHT_NONE,
	LIGHT_BASIC
} LightMethod;


/* global declarations*/
extern RenderingMode g_renderingMode;
extern CullMethod g_cullMethod;
extern LightMethod g_lightMethod; 
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* framebuffer;
extern float* g_Zbuffer; 
extern SDL_Texture* framebuffer_texture;
extern int win_w;    // fallback value
extern int win_h;    // fallback value

/* signatures */
bool init_win(void);
void draw_grid(int spacing, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_bresLine(int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color); // uses Bresenheim's
void render_framebuffer(void);
void clear_framebuffer(uint32_t color);
void clear_Zbuffer(void);
void destroy_window(void);
#endif
