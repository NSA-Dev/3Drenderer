#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"

#define  R_LIMIT (2 * 3.14159265) // rotation limit for view controls

/* .z rotations are disabled (they work, just did not add controls for them)*/


triangle_t* triangles_to_render = NULL;
bool is_running = false;
int previous_frame_time = 0; // ms
float fov_factor = 640;
char default_asset_dir[] = "./assets/f22.obj";
/*  vector  declr  */
vec3_t camera_pos = {.x = 0, .y = 0, .z = -5};

 
bool setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);

int main(void) {
    is_running = init_win();
    
    if(!setup()) {
        fprintf(stderr, "Setup failed. Shutting down.\n");
        return 1;
    };

    while(is_running) {
       process_input();
       update();
       render();  
    }
   
    destroy_window();
    free_resources();

    return 0;
}


bool setup(void) {
    // Allocate framebuffer
    framebuffer = (uint32_t*)malloc(sizeof(uint32_t) * win_w * win_h);
    if(!framebuffer) {
        fprintf(stderr, "Failed to allocate framebuffer memory.\n");
        return false;  
    }

   //framebuffer texture
   framebuffer_texture = SDL_CreateTexture(
        renderer,           // renderer responsible 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        win_w,
        win_h
   );
   if(!framebuffer_texture) { 
        fprintf(stderr, "Failed to allocate SDL_Texture.\n");
        return false;
   }

   //if unable to load custom model, load default cube and print msg
    if(!load_mesh_data(default_asset_dir)) {
        printf("Error: unable to read specified .obj file.\n");
        printf("Loading default model...\n");  
        load_cube_mesh(); 
    }
    return true; 
}

void process_input(void) {
    SDL_Event event; 
    SDL_PollEvent(&event);  // needs a pointer to the event
    
    switch(event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            else if (event.key.keysym.sym == SDLK_LEFT)
                mesh.rotation.y += 0.1;
            else if(event.key.keysym.sym == SDLK_RIGHT)
                mesh.rotation.y -= 0.1;
            else if (event.key.keysym.sym == SDLK_UP)
                mesh.rotation.x += 0.1;
            else if(event.key.keysym.sym == SDLK_DOWN)
                mesh.rotation.x -= 0.1;
            //TODO add camera control limits
            else if(event.key.keysym.sym == SDLK_KP_PLUS)
                camera_pos.z += 0.1;
            else if(event.key.keysym.sym == SDLK_KP_MINUS)
                camera_pos.z -= 0.1;
            break; 
    }
}

// TODO find a better place for this function, remains in main.c for now
vec2_t project(vec3_t* p) {
    vec2_t projected = {
        .x = (fov_factor * p->x) / p->z,
        .y = (fov_factor * p->y) / p->z
    };
    return projected;
}

void update(void) {
    // FPS sync.  While not enough ticks passed (see FTT in display.h), delay the update
    int time_to_wait = FTT - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0) SDL_Delay(time_to_wait);     
    previous_frame_time = SDL_GetTicks(); 

    triangles_to_render = NULL;
    int num_faces = array_length(mesh.faces);

    for(int i = 0; i < num_faces; i++) {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_verts[3]; 
        // look inside the mesh.faces[i].(a/b/c)
        // find index corresponding to a
        // grab data with the index from mesh_verts
        // -1 compensates for index  offset
        face_verts[0] = mesh.verts[mesh_face.a - 1];
        face_verts[1] = mesh.verts[mesh_face.b - 1];
        face_verts[2] = mesh.verts[mesh_face.c - 1];
        
        // prepare a temp triangle_t for passing into triangles_to_render[]
        // line 138
        triangle_t projected_triangle; 

        // transform recieved verts
        for(int j = 0; j < 3; j++) {
            vec3_t temp = face_verts[j]; 
        
            if(mesh.rotation.x != 0) vec3_rotate_x(&temp, mesh.rotation.x);
            if(mesh.rotation.y != 0) vec3_rotate_y(&temp, mesh.rotation.y);
            if(mesh.rotation.z != 0) vec3_rotate_z(&temp, mesh.rotation.z);
            
            // factor in camera depth
            temp.z += camera_pos.z;

            // project (perspective divide)
            vec2_t projected = project(&temp);
           
            // scale and translate to the middle of the screen
            projected.x += (win_w/2);
            projected.y += (win_h/2); 

            // pass into the array
            projected_triangle.points[j] = projected; 
       } 
        // save calculated data for rendering
        //triangles_to_render[i] = projected_triangle; 
        array_push(triangles_to_render, projected_triangle); 
    }


    // check rotations for overflow
    if(mesh.rotation.x > R_LIMIT) mesh.rotation.x -= R_LIMIT;
    if(mesh.rotation.x < R_LIMIT) mesh.rotation.x += R_LIMIT;
    if(mesh.rotation.y > R_LIMIT) mesh.rotation.y -= R_LIMIT;
    if(mesh.rotation.y < R_LIMIT) mesh.rotation.y += R_LIMIT;   
}

void render(void) {
    draw_grid(10, COLOR_LIGHT_GRAY);          // grid spacing
    int polycount = array_length(triangles_to_render);

    for(int i = 0; i < polycount; i++) {
        triangle_t triangle = triangles_to_render[i]; 
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, COLOR_GREEN); 
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, COLOR_GREEN); 
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, COLOR_GREEN);
        draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_GREEN                  
                ); 
    } 

    array_free(triangles_to_render); 
    /*  Not a great idea, memory management is  kinda bad 
     *  for now. Generally, I need to stay away from using malloc 
     *  and free inside update() and render() loops */

    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    SDL_RenderPresent(renderer); 
}
void free_resources(void) {
    if(framebuffer != NULL) free(framebuffer); 
    array_free(mesh.faces);
    array_free(mesh.verts);
    
}
