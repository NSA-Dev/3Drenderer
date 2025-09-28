#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define MAX_TRIANGLES 10000  
#define FPS 60
#define FTT (1000 / FPS)
#define DEFAULT_CAM_DEPTH 5
#define GRID_SPACING 10

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

/* signatures */
bool init_window(void);
void draw_grid(int spacing, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_bresLine(int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color); // uses Bresenheim's
void render_framebuffer(void);
void clear_framebuffer(uint32_t color);
void clear_Zbuffer(void);
void destroy_window(void);
int getWindowWidth(void);
int getWindowHeight(void);
void setRenderingMode(RenderingMode mode);
void setLightMethod(LightMethod method); 
void setCullMethod(CullMethod method); 
LightMethod getLightMethod(void);
CullMethod getCullMethod(void);
RenderingMode getRenderingMode(void);
float* getZbufferPtr(void);
bool renderTexture(void);
bool renderWire(void);
bool renderVerts(void);
bool renderSolids(void);

#endif
