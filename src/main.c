#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "colors.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"

#define  R_LIMIT (2 * 3.14159265) // rotation limit for view controls
#define  PI_CONST 3.14159
/* .z rotations are disabled (they work, just did not add controls for them)*/


triangle_t* triangles_to_render = NULL;
bool is_running = false;
int previous_frame_time = 0; // ms
mat4_t proj_matrix;
char default_asset_dir[] = "./assets/na"; // Usage: manually specify model.
                                        // shouldn't be used until I implement parsing
                                        // colors / textures from .obj files,
                                        // since face_t has been altered to include
                                        // color (uint32_t) field
/*  vector  declr  */


vec3_t camera_pos = { 0, 0, 0};
// replace with the one below to fix controls (breaks culling)
// vec3_t camera_pos = {.x = 0, .y = 0, .z = -5};

 
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

   // framebuffer texture
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

   // if unable to load custom model, load default cube and print msg
    if(!load_mesh_data(default_asset_dir)) {
        printf("Error: unable to read specified .obj file.\n");
        printf("Loading default model...\n");  
        load_cube_mesh(); 
    }

    // TODO fix 60 deg bug
    // Renderer settings
    float fov = PI_CONST / 2.0; // NOTE 60 deg fov (PI/3) is squashed on my monitor 
    float aspect = (float) win_h / (float) win_w;
    float znear = 0.1;
    float zfar = 100.0;
    
    // Init projection matrix 
    proj_matrix =  mat4_make_perspective(fov, aspect, znear, zfar);

    mesh.translation.z = 5; // default "camera" depth
    initialize_rendering_mode();
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
            // Quit the program
            if(event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            // Rotations
            else if (event.key.keysym.sym == SDLK_LEFT)
                mesh.rotation.y += 0.1;
            else if(event.key.keysym.sym == SDLK_RIGHT)
                mesh.rotation.y -= 0.1;
            else if (event.key.keysym.sym == SDLK_UP)
                mesh.rotation.x += 0.1;
            else if(event.key.keysym.sym == SDLK_DOWN)
                mesh.rotation.x -= 0.1;
            // Zoom  TODO fix this (broken because of culling), add camera control limits
            else if(event.key.keysym.sym == SDLK_KP_PLUS)
                mesh.translation.z += 0.1;
            else if(event.key.keysym.sym == SDLK_KP_MINUS)
                mesh.translation.z -= 0.1;
            else if(event.key.keysym.sym == SDLK_w)
                mesh.translation.y += 0.1;
            else if(event.key.keysym.sym == SDLK_s)
                mesh.translation.y -= 0.1;
            else if(event.key.keysym.sym == SDLK_a)
                mesh.translation.x += 0.1;
            else if(event.key.keysym.sym == SDLK_d)
                mesh.translation.x -= 0.1;
            else if(event.key.keysym.sym == SDLK_q)
                mesh.scale.x += 0.1;
            else if(event.key.keysym.sym == SDLK_e)
                mesh.scale.x -= 0.1;
            // Rendering modes
            else if(event.key.keysym.sym == SDLK_F1) {
                rendering_mode.enable_wireframe = true;
                rendering_mode.enable_vertices = true;
                rendering_mode.enable_solid = false;
            }    
            else if (event.key.keysym.sym == SDLK_F2) {
                rendering_mode.enable_wireframe = true;
                rendering_mode.enable_vertices = false;
                rendering_mode.enable_solid = false; 
            }
            else if(event.key.keysym.sym == SDLK_F3) {
                rendering_mode.enable_wireframe = false;
                rendering_mode.enable_vertices = false;
                rendering_mode.enable_solid = true; 
            }
            else if (event.key.keysym.sym == SDLK_F4) {
                rendering_mode.enable_wireframe = true;
                rendering_mode.enable_vertices = false;
                rendering_mode.enable_solid = true; 
            }
            else if(event.key.keysym.sym == SDLK_F5)
                rendering_mode.enable_culling = true;
            else if(event.key.keysym.sym == SDLK_F6)
                rendering_mode.enable_culling = false; 
            break; 
    }
}


void update(void) {
    // FPS sync.  While not enough ticks passed (see FTT in display.h), delay the update
    int time_to_wait = FTT - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0) SDL_Delay(time_to_wait);     
    previous_frame_time = SDL_GetTicks(); 
 

    // create transform  matrices to multiply mesh verts later
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, 
                                          mesh.scale.y, 
                                          mesh.scale.z
                                          ); 
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x,
                                                      mesh.translation.y,
                                                      mesh.translation.z
                                                     );
    
    // could stand some revision I'd say
    mat4_t rotation_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_z = mat4_make_rotation_z(mesh.rotation.z); 
    
    // reset triangle array (leaks mem?)
    triangles_to_render = NULL;
    int num_faces = array_length(mesh.faces);

    // precalc the world matrix before applying transoformations to the vertices
    // (scale > rotate > translate) since translation changes origin of the object
    mat4_t world_matrix = mat4_make_identity();
    world_matrix = mat4_mult_mat4(&scale_matrix, &world_matrix);
    world_matrix = mat4_mult_mat4(&rotation_x, &world_matrix);
    world_matrix = mat4_mult_mat4(&rotation_y, &world_matrix);
    world_matrix = mat4_mult_mat4(&rotation_z, &world_matrix);
    world_matrix = mat4_mult_mat4(&translation_matrix, &world_matrix);


    /* Main transformation loop, goes through all of the faces and applies
     * transformation data */
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
        triangle_t projected_triangle;

        // prepare an array to store tranformations for intermediate culling check
        // changed to vec4_t
        vec4_t transformed_vertices[3];

        // Transformation step, note the type conversions (scale > rotate > translate)
        for(int j = 0; j < 3; j++) {
            // create a temp vector to apply transforms
            vec4_t temp = vec4_from_vec3(&face_verts[j]);
            
            //scale & translate
            temp = mat4_mult_vec4(&world_matrix, &temp);
            // store tranformed result into array 
            transformed_vertices[j] = temp; 
        }

        /* Culling check (clock wise orientation) */
        if(rendering_mode.enable_culling) {
        //TODO account for vec4. 
        //Grabing vertices (temp solution is to convert back to vec3_t)
            vec3_t a = vec3_from_vec4(&transformed_vertices[0]); /*   A  */
            vec3_t b = vec3_from_vec4(&transformed_vertices[1]); /*  / \  */
            vec3_t c = vec3_from_vec4(&transformed_vertices[2]); /* C---B */
        
        // calculate (CA & BA) lengths
            vec3_t c_a = vec3_sub(&c, &a);
            vec3_t b_a = vec3_sub(&b, &a);
            vec3_norm(&c_a);
            vec3_norm(&b_a); 
        
        // Find their normal via cross product
            vec3_t normal = vec3_cross(&b_a, &c_a);
        
        // normalize it since only direction is relevant
            vec3_norm(&normal);
        
        // Find camera ray by substracting camera pos from A
            vec3_t camera_ray = vec3_sub(&camera_pos, &a);

        // check alignment between the normal and camera via dot prod
            float alignment_factor = vec3_dot(&normal, &camera_ray); 
        
        // was <= 0, if not aligned with camera (looking away) skip it
            if(alignment_factor < 0) continue;
        }  
        
        //avg depth calculation for z sorting
        float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;  
        projected_triangle.avg_depth = avg_depth;
    
        // Projection step
        for(int j = 0; j < 3; j++) {
            
            vec4_t projected = mat4_mult_vec4_project(&proj_matrix, &transformed_vertices[j]);
                        
            // scale projection into view
            projected.x *= (win_w / 2.0);
            projected.y *= (win_h / 2.0);
    
            // translate it to screen center
            projected.x += (win_w / 2.0); 
            projected.y += (win_h / 2.0);
            

            // Note to self:
            // An explicit typecast is required here, since a compiler has no clue 
            // about which type to use for this field
            projected_triangle.points[j] = (vec2_t) {
                .x = projected.x,
                .y = projected.y
            }; 
       }
        // add color data to the triangle
        projected_triangle.color = mesh_face.color;
        
        // save calculated data for rendering 
        array_push(triangles_to_render, projected_triangle);
        
    }

    // sort triangles by average depth for painters algorithm
    int length = array_length(triangles_to_render);
    qsort_depth(triangles_to_render, 0, length - 1);
   
   // DEBUG 
   // for(int i = 0; i < length; i++) {
   //     printf("Depth after sort[%d]: %.3f\n", i, triangles_to_render[i].avg_depth);
   // }

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
        if(rendering_mode.enable_vertices) { 
        draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 4, 4, COLOR_RED); 
        draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 4, 4, COLOR_RED); 
        draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 4, 4, COLOR_RED);
        }

        if(rendering_mode.enable_solid) {
        draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                triangle.color                  
                );
        }

        if(rendering_mode.enable_wireframe) {
        draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_GRAY                  
                );
        } 
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
