#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


#define FPS 60
#define FTT (1000 / FPS)

typedef struct {
    bool enable_wireframe;
    bool enable_vertices;
    bool enable_solid;
    bool enable_culling;
    bool enable_flat_shading;
} render_flags_t; 

/* global declarations*/
extern render_flags_t rendering_mode;
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* framebuffer;
extern SDL_Texture* framebuffer_texture;
extern int win_w;    // fallback value
extern int win_h;    // fallback value

/* signatures */
bool init_win(void);
void initialize_rendering_mode(void);
void draw_grid(int spacing, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_bresLine(int x1, int y1, int x2, int y2, uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color); // uses Bresenheim's
void render_framebuffer(void);
void clear_framebuffer(uint32_t color);
void destroy_window(void);
#endif
