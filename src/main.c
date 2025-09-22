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
#include "renderer.h"
#include "clipping.h"
#define  R_LIMIT (2 * 3.14159265) // rotation limit for view controls
#define  PI_CONST 3.14159265

mat4_t g_viewMatrix;
mat4_t g_worldMatrix; 
triangle_t* g_renderQueue = NULL; 
uint32_t g_polyCount  = 0;
uint32_t g_triangleCounter = 0; 
int previous_frame_time = 0; // ms
float g_deltaTime = 0; 
mat4_t proj_matrix;

 
bool setup(Renderer* renderer);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);

int main(void) {
    Renderer defaultRenderer;
    defaultRenderer.isRunning = false;   
    if(!disp_initWin()){
        fprintf(stderr, "Unable to initalize window. Shutting down.\n");
        return 1;
    }
    if(!setup(&defaultRenderer)) {
        fprintf(stderr, "Renderer setup failed. Shutting down.\n");
        return 1;
    };
  
    while(defaultRenderer.isRunning) {
       process_input();
       update();
       render();  
    }
   
    destroy_window();
    free_resources();

    return 0;
}


bool setup(Renderer* renderer) {
    char modelPath[] = "./assets/crab.obj"; 
    char texturePath[] = "./assets/crab.png"; 
   // if unable to load custom model, load default cube and print msg
    if(!load_mesh_data(modelPath)) {
        printf("Error: unable to read specified .obj file.\n");
        printf("Loading default model...\n");  
        load_cube_mesh();
    }   
    // .png Texture loading 
    if(!load_png_textureData(texturePath)) {
			printf("Error: unable to open provided texture.\n Loading default..."); 
			load_defaultTexture();  
            //mesh_texture = (uint32_t*) REDBRICK_TEXTURE; // Load test texture (hardcoded)
	}   

    // Allocate the draw list for all polys << Can eat all of the heap
    renderer->polyCount = array_length(getMesh()->faces);
    renderer->renderQueue = (triangle_t*)malloc(sizeof(triangle_t) * renderer->polyCount); 
    if(!renderer->renderQueue) {
		fprintf(stderr, "Error: not enough memory to process the model (The polycount is too high).\n");
		return false; 
	}

    float fov_y = 3.141592 / disp_getAspectY() ; // DEBUG 
    float fov_x = atan(tan(fov_y / 2) * disp_getAspectX()) * 2;
    float z_near = 1.0;
    float z_far = 20.0;
    renderer->projectionMatrix = mat4_make_perspective(fov_y, disp_getAspectY(), z_near, z_far);
    
    // Initialize frustum planes with a point and a normal
    initFrustumPlanes(fov_x, fov_y, z_near, z_far);  

    renderer->renderingMode = RENDER_WIRE_VERTEX;
    renderer->cullMethod = CULL_BACKFACE;
    renderer->LightMethod = LIGHT_NONE;
    renderer->controlMode = SPIN;
    renderer->isRunning = true;  
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
                mesh.translation.z += 1 * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_MINUS)
                mesh.translation.z -= 1 * g_deltaTime;
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
    // Synchronize frames before proceeding
    int elapsedTime = SDL_GetTicks() - previous_frame_time; 
    int waitTime = FTT - elapsedTime; 
    if (waitTime > 0 && waitTime <= FTT) {
        SDL_Delay(waitTime); 
    }
    g_deltaTime = (SDL_GetTicks() - previous_frame_time) / 1000.0; 
    previous_frame_time = SDL_GetTicks(); 

    // Reset the global triangle counter for this frame
    g_triangleCounter = 0; 

    // Adjust mesh by default values (e.g. if spin mode is active) 
    if(g_controlMode == SPIN) mesh.rotation.y += MESH_SPIN_FACTOR * g_deltaTime; 
    mesh.translation.z = DEFAULT_CAM_DEPTH; 

    // Set up default camera  target (looking at the positive z-axis)
    vec3_t targetPoint = { 0, 0, 1 }; 
    vec3_t upDirection = { 0, 1, 0 };  
    mat4_t cameraYawRotation = mat4_make_rotation_y(g_camera.yaw);
    vec4_t vec4_targetPoint = vec4_from_vec3(&targetPoint); 
    vec4_t mat4_product = mat4_mult_vec4(&cameraYawRotation, &vec4_targetPoint); 
    vec3_t currentDirection = vec3_from_vec4(&mat4_product); 
    g_camera.direction = currentDirection;
    targetPoint = vec3_add(&g_camera.position, &g_camera.direction); 
    // Create the view matrix
    g_viewMatrix = mat4_look_at(g_camera.position, targetPoint, upDirection); 
    
    // Create scale, rotation and translation matrices 
    mat4_t scaleMatrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translationMatrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotationMatrix_x = mat4_make_rotation_x(mesh.rotation.x); 
    mat4_t rotationMatrix_y = mat4_make_rotation_y(mesh.rotation.y); 
    mat4_t rotationMatrix_z = mat4_make_rotation_z(mesh.rotation.z); 

    // Main processing loop for all mesh faces
    int faceCount = array_length(mesh.faces);  
    for(int i = 0; i < faceCount; i++) {
        face_t currentFace = mesh.faces[i];
        // assign default color
        currentFace.color = COLOR_GRAY;

        // -1 offset already handled during .obj parsing 
        vec3_t faceVertices[3]; 
        faceVertices[0] = mesh.verts[currentFace.a]; 
        faceVertices[1] = mesh.verts[currentFace.b]; 
        faceVertices[2] = mesh.verts[currentFace.c]; 

        // perform vertex transformations
        vec4_t transformed[3];
        for(int j = 0; j < 3; j++) {
            vec4_t currentVertex = vec4_from_vec3(&faceVertices[j]);
            g_worldMatrix = mat4_make_identity(); 
            g_worldMatrix = mat4_mult_mat4(&scaleMatrix, &g_worldMatrix);
            g_worldMatrix = mat4_mult_mat4(&rotationMatrix_z, &g_worldMatrix);
            g_worldMatrix = mat4_mult_mat4(&rotationMatrix_y, &g_worldMatrix); 
            g_worldMatrix = mat4_mult_mat4(&rotationMatrix_x, &g_worldMatrix);
            g_worldMatrix = mat4_mult_mat4(&translationMatrix, &g_worldMatrix);
            
            currentVertex = mat4_mult_vec4(&g_worldMatrix, &currentVertex);
            currentVertex = mat4_mult_vec4(&g_viewMatrix, &currentVertex); // translate to Camera space 
            transformed[j] = currentVertex; 
        }  
        
        // Compute normal from resulting transformed face vertices 
        // Indexing: A - 0 ,  B - 1, C - 2
        vec3_t A, B, C, AB, AC, normal;
        A = vec3_from_vec4(&transformed[0]);
        B = vec3_from_vec4(&transformed[1]);
        C = vec3_from_vec4(&transformed[2]);
        AB = vec3_sub(&B, &A);
        AC = vec3_sub(&C, &A);
        vec3_norm(&AB);
        vec3_norm(&AC); 
        normal = vec3_cross(&AB, &AC);
        vec3_norm(&normal);
        
        // Perform Backface culling if needed
        if(g_cullMethod == CULL_BACKFACE) {
           vec3_t origin = { 0, 0, 0 };
           vec3_t cameraRay = vec3_sub(&origin, &A);
           float alignmentFactor = vec3_dot(&normal, &cameraRay);
           if(alignmentFactor < 0) continue;  
        }

        // Lighting
        if(g_lightMethod == LIGHT_BASIC) {
                float ray_alignment = -vec3_dot(&normal, &global_light.direction); 
                currentFace.color = light_apply_intensity(currentFace.color, ray_alignment); 
        }

        // Perform clipping, same vertex orientation as in normal calculation
        polygon_t polygon; 
        vec3_t pA, pB, pC;
        pA = vec3_from_vec4(&transformed[0]);
        pB = vec3_from_vec4(&transformed[1]); 
        pC = vec3_from_vec4(&transformed[2]);
        // FIX ME: pass mesh_face.*_uv
        polygon = createPolygon(&pA, &pB, &pC, currentFace.a_uv, currentFace.b_uv, currentFace.c_uv); 
        clipPolygon(&polygon);
        
        // Break the clipped part into individual triangles
        triangle_t clippedTriangles[CLIP_TRIANGLE_LIMIT];
        int clippedCount;
        
        clippedCount = 0;
        slicePolygon(&polygon, clippedTriangles, &clippedCount);  
        // Loop through all resulting triangles
        for(int t = 0; t < clippedCount; t++) {
            triangle_t currentTriangle = clippedTriangles[t]; 
            vec4_t projectedVertices[3];
            for(int j = 0; j < 3; j++) {
                projectedVertices[j] = mat4_mult_vec4_project(&proj_matrix, &currentTriangle.points[j]);
                // Flip verticaly respective to current coord system
                projectedVertices[j].y *= -1; // check texture allignment
                // Scale and translate into view
                projectedVertices[j].x *= (win_w / 2.0); 
                projectedVertices[j].y *= (win_h / 2.0); 
                projectedVertices[j].x += (win_w / 2.0);
                projectedVertices[j].y += (win_h / 2.0);  
            } 
            // Package the resulting data into triangle_t for rendering
            triangle_t result; 
            for(int i = 0; i < 3; i++) {
                result.points[i].x = projectedVertices[i].x;
                result.points[i].y = projectedVertices[i].y;
                result.points[i].z = projectedVertices[i].z;
                result.points[i].w = projectedVertices[i].w; 
            }
            result.texcoords[0].u = clippedTriangles[t].texcoords[0].u;
            result.texcoords[0].v = clippedTriangles[t].texcoords[0].v;
            result.texcoords[1].u = clippedTriangles[t].texcoords[1].u;
            result.texcoords[1].v = clippedTriangles[t].texcoords[1].v;
            result.texcoords[2].u = clippedTriangles[t].texcoords[2].u;
            result.texcoords[2].v = clippedTriangles[t].texcoords[2].v;  
            result.color = currentFace.color;
            
            // Send the result to the pipeline 
            if(g_triangleCounter < MAX_TRIANGLES) {
                g_renderQueue[g_triangleCounter++] = result; 
            }
        }
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
