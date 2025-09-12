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
#include "light.h"
#include "texture.h"
#include "triangle.h"
#include "upng.h"
#include "camera.h"
#include "controls.h"
#include "clipping.h"
#define  R_LIMIT (2 * 3.14159265) // rotation limit for view controls
#define  PI_CONST 3.14159
mat4_t g_viewMatrix; 




triangle_t* g_renderQueue = NULL; 
uint32_t g_polyCount  = 0;
uint32_t g_triangleCounter = 0; 

bool is_running = false;
int previous_frame_time = 0; // ms
float g_deltaTime = 0; 
mat4_t proj_matrix;
char modelPath[] = "./assets/cube.obj"; // Usage: manually specify model.
char texturePath[] = "./assets/cube.png"; //

 
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
    // Allocate z-buffer
    g_Zbuffer = (float*)malloc(sizeof(float) * win_w * win_h); 
    if(!g_Zbuffer) {
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

   // if unable to load custom model, load default cube and print msg
    if(!load_mesh_data(modelPath)) {
        printf("Error: unable to read specified .obj file.\n");
        printf("Loading default model...\n");  
        load_cube_mesh(); 
        // load default cube texture
        mesh_texture = (uint32_t*)REDBRICK_TEXTURE; // mesh_texture (global) - defined in texture.h 
        texture_width = 64;
        texture_height = 64; 
    }   

    // Allocate the draw list for all polys << Can eat all of the heap
    g_polyCount = array_length(mesh.faces);
    g_renderQueue = (triangle_t*)malloc(sizeof(triangle_t) * g_polyCount); 
    if(!g_renderQueue) {
		fprintf(stderr, "Error: not enough memory to process the model (polycount is too high).\n");
		return false; 
	}
	
	
    // Renderer settings
    float aspectY = (float) win_h / (float) win_w; 
    float aspectX = (float) win_w / (float) win_h;  

    float fovY = (3.141592 / 3.0);   //PI_CONST / 3.0;
    float fovX = atan(tan(fovY / 2.0) * aspectX) * 2;
 
    float znear = 0.1;  // old znear  0.1
    float zfar = 100.0;  //old zfar 100.0
    

    init_frustum_planes(fovY, znear, zfar);  

    // Init projection matrix 
    proj_matrix =  mat4_make_perspective(fovY, aspectY, znear, zfar);
    mesh.translation.z = 5; // default "camera" depth


    // .png Texture loading 
    if(!load_png_textureData(texturePath)) {
			printf("Error: unable to open provided texture.\n Loading default..."); 
			mesh_texture = (uint32_t*) REDBRICK_TEXTURE; // Load test texture (hardcoded)
	}
     
    g_renderingMode = RENDER_TEXTURED;
    g_cullMethod = CULL_BACKFACE;  
    g_lightMethod = LIGHT_NONE; 
    g_controlMode = SPIN; 
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
            if(g_controlMode == MANUAL) {    
            // Manual camera control
             if (event.key.keysym.sym == SDLK_LEFT)
                mesh.rotation.y += MESH_ROTATION_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_RIGHT)
                mesh.rotation.y -= MESH_ROTATION_FACTOR * g_deltaTime;
             if (event.key.keysym.sym == SDLK_UP)
                mesh.rotation.x += MESH_ROTATION_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_DOWN)
                mesh.rotation.x -= MESH_ROTATION_FACTOR * g_deltaTime;
            // Zoom  TODO add camera control limits
             if(event.key.keysym.sym == SDLK_KP_PLUS)
                mesh.translation.z += 0.0001 * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_MINUS)
                mesh.translation.z -= 0.0001 * g_deltaTime;
		// cam controls << active only if g_controlMode == MANUAL   
		   // going too close to the obj 
		   //  or out of bounds crashes the program, due to perspective
		   // projection errors 
             if(event.key.keysym.sym == SDLK_a)
                g_camera.yaw += CAM_YAW_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_d)
                g_camera.yaw -= CAM_YAW_FACTOR * g_deltaTime;
                
             if(event.key.keysym.sym == SDLK_w) {
                g_camera.forwardVelocity = vec3_mul(&g_camera.direction, 
									 CAM_VELOCITY_FACTOR * g_deltaTime);
                g_camera.position = vec3_add(&g_camera.position, 
											&g_camera.forwardVelocity); 
             }
             if(event.key.keysym.sym == SDLK_s) {
                g_camera.forwardVelocity = vec3_mul(&g_camera.direction, 
									 CAM_VELOCITY_FACTOR * g_deltaTime);
                g_camera.position = vec3_sub(&g_camera.position, 
											&g_camera.forwardVelocity);  
             }
             if(event.key.keysym.sym == SDLK_KP_8)
                g_camera.position.z += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_2)
                g_camera.position.z -= CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_4)
                g_camera.position.x += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_6)
                g_camera.position.x -= CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_9)
                g_camera.position.y += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_3)
                g_camera.position.y -= CAM_POS_FACTOR * g_deltaTime;
           // stretch the mesh
             if(event.key.keysym.sym == SDLK_q)
                mesh.scale.x += MESH_SCALE_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_e)
                mesh.scale.x -= MESH_SCALE_FACTOR * g_deltaTime;
            }
            // Rendering modes
             if(event.key.keysym.sym == SDLK_F1) {
				 g_renderingMode = RENDER_WIRE;
            }    
             if (event.key.keysym.sym == SDLK_F2) {
				 g_renderingMode = RENDER_WIRE_VERTEX; 
            }
             if(event.key.keysym.sym == SDLK_F3) {
				 g_renderingMode = RENDER_SOLID; 
            }
             if (event.key.keysym.sym == SDLK_F4) {
				 g_renderingMode = RENDER_SOLID_WIRE; 
            }
             if(event.key.keysym.sym == SDLK_F5){
				g_cullMethod = CULL_BACKFACE;
			}
             if(event.key.keysym.sym == SDLK_F6) {
				 g_cullMethod = CULL_NONE;       
            }    
             if(event.key.keysym.sym == SDLK_F7) {
                if(mesh_texture != NULL) {
					g_renderingMode = RENDER_TEXTURED;   
                }
            }
             if(event.key.keysym.sym == SDLK_F8) {
               if(g_lightMethod == LIGHT_NONE) {
					g_lightMethod = LIGHT_BASIC;
				} else {
					g_lightMethod = LIGHT_NONE; 	
				}
            }
             if(event.key.keysym.sym == SDLK_F9) {
               if(g_controlMode == SPIN) {
					g_controlMode = MANUAL;
				} else {
					g_controlMode = SPIN; 	
				}
            }
                
            break; 
    }
}


void update(void) {
    // Frame sync. While not enough ticks passed (see FTT in display.h), delay the update
    int time_to_wait = FTT - (SDL_GetTicks() - previous_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FTT) SDL_Delay(time_to_wait);     
	// update delta time factor (to synchronize updates of scene objects)
	g_deltaTime = (SDL_GetTicks() - previous_frame_time) / 1000.0;
    previous_frame_time = SDL_GetTicks(); 
 
	// spin the mesh in case spin control mode is active
	if(g_controlMode == SPIN) {
		mesh.rotation.y += MESH_SPIN_FACTOR * g_deltaTime;
	}
    
    vec3_t up_dir = {0, 1, 0};
    // Find new target point for the FPS cam
    vec3_t targetPoint = {0, 0, 1}; // look at positive Z (LH coord sys)
    mat4_t rotation_cameraYaw = mat4_make_rotation_y(g_camera.yaw);
    
    // Jfc there has to be a better way
    // unfolding lecturer's code (have to pay for abusing pointers)
    vec4_t targetPoint_vec4 = vec4_from_vec3(&targetPoint); 
    vec4_t mat4_product = mat4_mult_vec4(&rotation_cameraYaw, &targetPoint_vec4); 
    vec3_t newDirection = vec3_from_vec4(&mat4_product);
    g_camera.direction = newDirection;  
 
    targetPoint = vec3_add(&g_camera.position, &g_camera.direction); 
    
     
    // prep finished, assign the view to g_viewMatrix
    g_viewMatrix = mat4_look_at(g_camera.position, targetPoint, up_dir); 
    

    
    // create transform  matrices to multiply mesh verts later
    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, 
                                          mesh.scale.y, 
                                          mesh.scale.z
                                          ); 
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x,
                                                      mesh.translation.y,
                                                      mesh.translation.z
                                                     );
    
    mat4_t rotation_x = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t rotation_y = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotation_z = mat4_make_rotation_z(mesh.rotation.z); 
    
	// reset triangle counter
    g_triangleCounter = 0; 
    
    
    int num_faces = g_polyCount;	// leftover

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
		// DEBUG if(i != 4) continue;

        face_t mesh_face = mesh.faces[i];
        mesh_face.color = 0xFFFFFFFF; // draw white mesh

        vec3_t face_verts[3]; 
        // look inside the mesh.faces[i].(a/b/c)
        // find index corresponding to a
        // grab data with the index from mesh_verts
        // -1 compensates for index  offset
        face_verts[0] = mesh.verts[mesh_face.a];
        face_verts[1] = mesh.verts[mesh_face.b];
        face_verts[2] = mesh.verts[mesh_face.c];
        
        // prepare a temp triangle_t for passing into g_renderQueue[]
        triangle_t projected_triangle;

        // add texture data from mesh_face to the temp variable before future transformations
        projected_triangle.texcoords[0].u = mesh_face.a_uv.u; 
        projected_triangle.texcoords[0].v = mesh_face.a_uv.v;
        projected_triangle.texcoords[1].u = mesh_face.b_uv.u;
        projected_triangle.texcoords[1].v = mesh_face.b_uv.v;
        projected_triangle.texcoords[2].u = mesh_face.c_uv.u;
        projected_triangle.texcoords[2].v = mesh_face.c_uv.v;  
         

        // prepare an array to store tranformations for intermediate culling check
        // changed to vec4_t
        vec4_t transformed_vertices[3];

        // Transformation step, note the type conversions (scale > rotate > translate)
        for(int j = 0; j < 3; j++) {
            // create a temp vector to apply transforms
            vec4_t temp = vec4_from_vec3(&face_verts[j]);
            
            //scale & translate
            temp = mat4_mult_vec4(&world_matrix, &temp);
            // apply the view matrix to the vertices to move into camera space  
            temp = mat4_mult_vec4(&g_viewMatrix, &temp);
            // store tranformed result into array 
            transformed_vertices[j] = temp; 
        }

        /* Precaclculate data for Culling & Shading */

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


       /* Rendering checks start here  */     
            
            // Culling  (clock wise orientation) 
            if(g_cullMethod == CULL_BACKFACE) {
                // Find camera ray by substracting origin from A
                vec3_t origin = {0, 0, 0}; // Not sure about this one
                vec3_t camera_ray = vec3_sub(&origin, &a);

                // check alignment between the normal and camera via dot prod
                float alignment_factor = vec3_dot(&normal, &camera_ray); 
        
                // was <= 0, if not aligned with camera (looking away) skip it
                if(alignment_factor < 0) continue;
            }

            // Lighting
            if(g_lightMethod == LIGHT_BASIC) {
                // Find light position relative to the face
                float ray_alignment = -vec3_dot(&normal, &global_light.direction); 
                mesh_face.color = light_apply_intensity(mesh_face.color, ray_alignment); 
            }
    
            // Clipping step
            // Create a temp polygon for clipping (from the original triangle)
            // The data for clipping is taken from  transformed_vertices[n],
            // but since its type is vec4_t instead of converting again,
            // the vec3_t versions are used from lines: 359-361  
            polygon_t polygon = createPolygon(&a, &b, &c);
            clipPolygon(&polygon); // pass the ref to the clipping sys  
            
            // stores results  of the clipping operation 
            triangle_t clippedTriangles[CLIP_TRIANGLE_LIMIT];
            
            int triangleCount = getTriangleCount(&polygon); // get number of slices 
            slicePolygon(&polygon, clippedTriangles, &triangleCount); // slice and write into clippedTriangles
            
            // DEBUG: triangleCount overwritten by slicePolygon call
            
            // loop assembled triangles after clipping
            for(int t = 0; t < triangleCount; t++) {
				triangle_t* clippedTriangle = &clippedTriangles[t]; 

				// After the poly is clipped, it needs to be broken into new triangles
				// in case the number of vertices goes up

			/* Rest of the update  */    
		
				// Projection step
				for(int j = 0; j < 3; j++) {
				
				   vec4_t projected = mat4_mult_vec4_project(&proj_matrix, &clippedTriangle->points[j]);
					
					//vec4_t projected = mat4_mult_vec4_project(&proj_matrix, &clippedTriangle->points[j]);             
				
					// Note: on widescreen x is scalled height and y by width
					// This is the opposite of what was shown in the materials.
					// scale projection into view
					projected.x *= (win_h / 2.0); // -1 here mirrored my textures -_-
					projected.y *= -(win_w / 2.0);
					// Inverting y & x is the necessary
					// (model values come bottom up, which is the opposite of screen space y) 
					// the same is  true for x. This has to do with their orientation in .obj files 

					// translate it to screen center
					projected.x += (win_w / 2.0); 
					projected.y += (win_h / 2.0);
				

					// Note to self:
					// An explicit typecast is required here, since a compiler has no clue 
					// about which type to use for this field
					projected_triangle.points[j] = (vec4_t) {
						.x = projected.x,
						.y = projected.y,
						// since updated struct field to vec4_t, we save z & w for correct rasterization later
						.z = projected.z, 
						.w = projected.w
					}; 
				}
				// add color data to the triangle
				projected_triangle.color = mesh_face.color;
				
			// save calculated data for rendering
			if(g_triangleCounter < g_polyCount) {
				g_renderQueue[g_triangleCounter] = projected_triangle;
				g_triangleCounter++; 
			}

    }


    // check rotations for overflow
    if(mesh.rotation.x > R_LIMIT) mesh.rotation.x -= R_LIMIT;
    if(mesh.rotation.x < R_LIMIT) mesh.rotation.x += R_LIMIT;
    if(mesh.rotation.y > R_LIMIT) mesh.rotation.y -= R_LIMIT;
    if(mesh.rotation.y < R_LIMIT) mesh.rotation.y += R_LIMIT; 
    }  
}

void render(void) {
    draw_grid(10, COLOR_LIGHT_GRAY);          // grid spacing
    
    // draw all of the triangles residing in g_renderQueue based on the rendering mode selected  
    for(int i = 0; i < g_triangleCounter; i++) {
        triangle_t triangle = g_renderQueue[i];
        
       if(g_renderingMode == RENDER_TEXTURED || g_renderingMode == RENDER_TEXTURED_WIRE) {
           draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,triangle.texcoords[2].u, triangle.texcoords[2].v,
                &triangle.color // dummy variable to comply with the signature                 
                );        
       }
        
        if(g_renderingMode == RENDER_WIRE_VERTEX) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 4, 4, COLOR_RED); 
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 4, 4, COLOR_RED); 
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 4, 4, COLOR_RED);
        }

        if(g_renderingMode == RENDER_SOLID) {
            draw_solid_triangle(&triangle); 
        }

        if(g_renderingMode == RENDER_WIRE || g_renderingMode == RENDER_WIRE_VERTEX || g_renderingMode == RENDER_SOLID_WIRE || g_renderingMode == RENDER_TEXTURED_WIRE) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_GRAY                  
                );
        } 
    } 



    render_framebuffer();
    clear_framebuffer(COLOR_BLACK);
    clear_Zbuffer(); 
    SDL_RenderPresent(renderer); 
}
void free_resources(void) {
    if(framebuffer != NULL) free(framebuffer);
    if(g_Zbuffer != NULL) free(g_Zbuffer);
    if(g_renderQueue != NULL) free(g_renderQueue);   
    array_free(mesh.faces);
    array_free(mesh.verts);
    upng_free(png_texture); 
}
