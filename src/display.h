#ifndef DISPLAY_H
#define DISPLAY_H
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

/* global declarations*/
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern uint32_t* framebuffer;
extern SDL_Texture* framebuffer_texture;
extern int win_w;    // fallback value
extern int win_h;    // fallback value

/* signatures */
bool init_win(void);
void draw_grid(int spacing, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color); 
void render_framebuffer(void);
void clear_framebuffer(uint32_t color);
void free_resources(void);
#endif
