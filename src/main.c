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


int previous_frame_time = 0; // ms
float g_deltaTime = 0;
triangle_t* g_drawList = NULL;  

bool setup(Renderer* renderer);
void process_input(Renderer* renderer);
void update(Renderer* renderer);
void draw(Renderer* renderer);
void free_resources(Renderer* renderer);

int main(void) {
    Renderer defaultRenderer;
    defaultRenderer.isRunning = false;
    defaultRenderer.renderQueue = NULL;

    if(!disp_initWin()){
        fprintf(stderr, "Unable to initalize window. Shutting down.\n");
        return 1;
    }
    if(!setup(&defaultRenderer)) {
        fprintf(stderr, "Renderer setup failed. Shutting down.\n");
        return 1;
    };
  
    while(defaultRenderer.isRunning) {
       process_input(&defaultRenderer);
       update(&defaultRenderer); 
       draw(&defaultRenderer);  
    }
   
    disp_destroyWindow();
    free_resources(&defaultRenderer);

    return 0;
}


bool setup(Renderer* renderer) {
    char modelPath[] = "./assets/cube.obj"; 
    char texturePath[] = "./assets/cube.png"; 
   // if unable to load custom model, load default cube and print msg
    if(!load_mesh_data(modelPath)) {
        printf("Error: unable to read specified .obj file.\n");
        printf("Loading default model...\n");  
        load_cube_mesh();
    }   

    renderer->renderQueue = NULL;  

    // Allocate the draw list for all polys << Can eat all of the heap
    int polyCount = array_length(getMeshPtr()->faces); // returns 12
    printf("Polys %d\n", polyCount);
    g_drawList = (triangle_t*)malloc(sizeof(triangle_t) * (polyCount));
    if(!g_drawList) {
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
    // .png Texture loading 
    if(!load_png_textureData(texturePath)) {
			printf("Error: unable to open provided texture.\n Loading default..."); 
			load_defaultTexture();  
            //mesh_texture = (uint32_t*) REDBRICK_TEXTURE; // Load test texture (hardcoded)
	} 
    renderer->renderingMode = RENDER_WIRE_VERTEX;
    renderer->renderQueue = g_drawList;
    renderer->polyCount = polyCount;
    printf("%d loaded polys\n", renderer->polyCount);  
    renderer->cullMethod = CULL_BACKFACE;
    renderer->lightMethod = LIGHT_NONE;
    renderer->controlMode = SPIN;
    renderer->isRunning = true;
    renderer->cameraPtr = getCameraPtr();
    renderer->meshPtr = getMeshPtr();   
    return true; 
}

void process_input(Renderer* renderer) {
    SDL_Event event; 
    SDL_PollEvent(&event);  // needs a pointer to the event
    
    switch(event.type) {
        case SDL_QUIT:
            renderer->isRunning = false;
            break;
        case SDL_KEYDOWN:
            // Quit the program
            if(event.key.keysym.sym == SDLK_ESCAPE)
                renderer->isRunning = false;
            if(renderer->controlMode == MANUAL) {    
            // Manual camera control
             if (event.key.keysym.sym == SDLK_LEFT)
                renderer->meshPtr->rotation.y += MESH_ROTATION_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_RIGHT)
                renderer->meshPtr->rotation.y -= MESH_ROTATION_FACTOR * g_deltaTime;
             if (event.key.keysym.sym == SDLK_UP)
                renderer->meshPtr->rotation.x += MESH_ROTATION_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_DOWN)
                renderer->meshPtr->rotation.x -= MESH_ROTATION_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_PLUS)
                renderer->meshPtr->translation.z += 1 * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_MINUS)
                renderer->meshPtr->translation.z -= 1 * g_deltaTime;
             if(event.key.keysym.sym == SDLK_a)
                renderer->cameraPtr->yaw += CAM_YAW_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_d)
                renderer->cameraPtr->yaw -= CAM_YAW_FACTOR * g_deltaTime;
                
             if(event.key.keysym.sym == SDLK_w) {
                renderer->cameraPtr->forwardVelocity = vec3_mul(&renderer->cameraPtr->direction, 
									 CAM_VELOCITY_FACTOR * g_deltaTime);
                renderer->cameraPtr->position = vec3_add(&renderer->cameraPtr->position, 
											&renderer->cameraPtr->forwardVelocity); 
             }
             if(event.key.keysym.sym == SDLK_s) {
                renderer->cameraPtr->forwardVelocity = vec3_mul(&renderer->cameraPtr->direction, 
									 CAM_VELOCITY_FACTOR * g_deltaTime);
                renderer->cameraPtr->position = vec3_sub(&renderer->cameraPtr->position, 
											&renderer->cameraPtr->forwardVelocity);  
             }
             if(event.key.keysym.sym == SDLK_KP_8)
                renderer->cameraPtr->position.z += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_2)
                renderer->cameraPtr->position.z -= CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_4)
                renderer->cameraPtr->position.x += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_6)
                renderer->cameraPtr->position.x -= CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_9)
                renderer->cameraPtr->position.y += CAM_POS_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_KP_3)
                renderer->cameraPtr->position.y -= CAM_POS_FACTOR * g_deltaTime;
           // stretch the mesh
             if(event.key.keysym.sym == SDLK_q)
                renderer->meshPtr->scale.x += MESH_SCALE_FACTOR * g_deltaTime;
             if(event.key.keysym.sym == SDLK_e)
                renderer->meshPtr->scale.x -= MESH_SCALE_FACTOR * g_deltaTime;
            }
            if(renderer->controlMode == SPIN) { 
                renderer->meshPtr->rotation.y += MESH_SPIN_FACTOR * g_deltaTime; 
            }
            // Rendering modes
             if(event.key.keysym.sym == SDLK_F1) {
				 renderer->renderingMode = RENDER_WIRE;
            }    
             if (event.key.keysym.sym == SDLK_F2) {
				 renderer->renderingMode = RENDER_WIRE_VERTEX; 
            }
             if(event.key.keysym.sym == SDLK_F3) {
				 renderer->renderingMode = RENDER_SOLID; 
            }
             if (event.key.keysym.sym == SDLK_F4) {
				 renderer->renderingMode = RENDER_SOLID_WIRE; 
            }
             if(event.key.keysym.sym == SDLK_F5){
				renderer->cullMethod = CULL_BACKFACE;
			}
             if(event.key.keysym.sym == SDLK_F6) {
				 renderer->cullMethod = CULL_NONE;       
            }    
             if(event.key.keysym.sym == SDLK_F7) {
                if(textureIsLoaded()) {
					renderer->renderingMode = RENDER_TEXTURED;   
                }
            }
             if(event.key.keysym.sym == SDLK_F8) {
               if(renderer->lightMethod == LIGHT_NONE) {
					renderer->lightMethod = LIGHT_BASIC;
				} else {
					renderer->lightMethod = LIGHT_NONE; 	
				}
            }
             if(event.key.keysym.sym == SDLK_F9) {
               if(renderer->controlMode == SPIN) {
					renderer->controlMode = MANUAL;
				} else {
					renderer->controlMode = SPIN; 	
				}
            }
                
            break; 
    }
}

void update(Renderer* renderer) {
    // Synchronize frames before proceeding
    int elapsedTime = SDL_GetTicks() - previous_frame_time; 
    int waitTime = FTT - elapsedTime; 
    if (waitTime > 0 && waitTime <= FTT) {
        SDL_Delay(waitTime); 
    }
    g_deltaTime = (SDL_GetTicks() - previous_frame_time) / 1000.0; 
    previous_frame_time = SDL_GetTicks(); 
    // Reset the global triangle counter for this frame
    renderer->triangleCount = 0; 
    // scale the mesh into view
    renderer->meshPtr->translation.z = DEFAULT_CAM_DEPTH; 

    // Set up default camera  target (looking at the positive z-axis)
    vec3_t targetPoint = { 0, 0, 1 }; 
    vec3_t upDirection = { 0, 1, 0 };  
    mat4_t cameraYawRotation = mat4_make_rotation_y(renderer->cameraPtr->yaw);
    vec4_t vec4_targetPoint = vec4_from_vec3(&targetPoint); 
    vec4_t mat4_product = mat4_mult_vec4(&cameraYawRotation, &vec4_targetPoint); 
    vec3_t currentDirection = vec3_from_vec4(&mat4_product); 
    renderer->cameraPtr->direction= currentDirection;
    targetPoint = vec3_add(&renderer->cameraPtr->position, &renderer->cameraPtr->direction); 
    // Create the view matrix
    renderer->viewMatrix = mat4_look_at(renderer->cameraPtr->position, targetPoint, upDirection); 
    
    // Create scale, rotation and translation matrices 
    mat4_t scaleMatrix = mat4_make_scale(renderer->meshPtr->scale.x, renderer->meshPtr->scale.y, renderer->meshPtr->scale.z);
    mat4_t translationMatrix = mat4_make_translation(renderer->meshPtr->translation.x, renderer->meshPtr->translation.y, renderer->meshPtr->translation.z);
    mat4_t rotationMatrix_x = mat4_make_rotation_x(renderer->meshPtr->rotation.x); 
    mat4_t rotationMatrix_y = mat4_make_rotation_y(renderer->meshPtr->rotation.y); 
    mat4_t rotationMatrix_z = mat4_make_rotation_z(renderer->meshPtr->rotation.z); 

    // Main processing loop for all mesh faces
    //!!!!!!!!!!!!!!!!!
    int faceCount = renderer->polyCount;
    printf("%d << to update\n", faceCount);
    //////////////////////////////////////////
    //int faceCount = array_length(renderer->meshPtr->faces);  
    for(int i = 0; i < faceCount; i++) {
        printf("Hello from main update() loop\n"); 
        face_t currentFace = renderer->meshPtr->faces[i];
        // assign default color
        currentFace.color = COLOR_GRAY;

        // -1 offset already handled during .obj parsing 
        vec3_t faceVertices[3]; 
        faceVertices[0] = renderer->meshPtr->verts[currentFace.a]; 
        faceVertices[1] = renderer->meshPtr->verts[currentFace.b]; 
        faceVertices[2] = renderer->meshPtr->verts[currentFace.c];
        printf("FACE:%f\n", faceVertices[0].x); 

        // perform vertex transformations
        vec4_t transformed[3];
        for(int j = 0; j < 3; j++) {
            vec4_t currentVertex = vec4_from_vec3(&faceVertices[j]);
            renderer->worldMatrix = mat4_make_identity(); 
            renderer->worldMatrix = mat4_mult_mat4(&scaleMatrix, &renderer->worldMatrix);
            renderer->worldMatrix = mat4_mult_mat4(&rotationMatrix_z, &renderer->worldMatrix);
            renderer->worldMatrix = mat4_mult_mat4(&rotationMatrix_y, &renderer->worldMatrix); 
            renderer->worldMatrix = mat4_mult_mat4(&rotationMatrix_x, &renderer->worldMatrix);
            renderer->worldMatrix = mat4_mult_mat4(&translationMatrix, &renderer->worldMatrix);
            
            currentVertex = mat4_mult_vec4(&renderer->worldMatrix, &currentVertex);
            currentVertex = mat4_mult_vec4(&renderer->viewMatrix, &currentVertex); // translate to Camera space 
            transformed[j] = currentVertex;
            printf("TRANSFORMED FACE %f\n", transformed[j].x); 
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
        printf("Got Normal %f \n", normal.x); 
        // Perform Backface culling if needed
        if(renderer->cullMethod == CULL_BACKFACE) {
           vec3_t origin = { 0, 0, 0 };
           vec3_t cameraRay = vec3_sub(&origin, &A);
           float alignmentFactor = vec3_dot(&normal, &cameraRay);
           if(alignmentFactor < 0) continue;
           
            printf("Hello from CULL\n");   
        }

        // Lighting
        if(renderer->lightMethod == LIGHT_BASIC) {
                float ray_alignment = -vec3_dot(&normal, &global_light.direction); 
                currentFace.color = light_apply_intensity(currentFace.color, ray_alignment);
                printf("Hello from LIGHT\n");   
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
        printf("Clipped before slicing:%d\n", clippedCount);
        slicePolygon(&polygon, clippedTriangles, &clippedCount);
        printf("Clipped after slicing:%d\n", clippedCount);
        // fetch current display dimensions for future calculations
        int win_w, win_h;
        win_w = disp_getWindowWidth();
        win_h = disp_getWindowHeight();  
        // Loop through all resulting triangles via clippedCount
        for(int t = 0; t < clippedCount; t++) {
             printf("Hello from CLIP\n"); 
            triangle_t currentTriangle = clippedTriangles[t]; 
            vec4_t projectedVertices[3];
            for(int j = 0; j < 3; j++) {
                projectedVertices[j] = mat4_mult_vec4_project(&renderer->projectionMatrix, &currentTriangle.points[j]);
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
            if(renderer->triangleCount < MAX_TRIANGLES) {
                renderer->renderQueue[renderer->triangleCount++] = result; 
            }
        }
    }   
    printf("Triangles:%d\nTriangle queue: %d\nFace counter: %d\n",  renderer->triangleCount, 
        array_length(renderer->renderQueue), faceCount);
}



void draw(Renderer* renderer) {
    // Needs only triangleCount & rendering mode for context
    disp_drawGrid(10, COLOR_LIGHT_GRAY);          // grid spacing
    
    // draw all of the triangles residing in g_renderQueue based on the rendering mode selected  
    for(int i = 0; i < renderer->triangleCount; i++) {
        triangle_t triangle = renderer->renderQueue[i];
        printf("Drawing: X %f\n", triangle.points->x); 
        RenderingMode currentRenderingMode = renderer->renderingMode; 
       if(currentRenderingMode == RENDER_TEXTURED || currentRenderingMode == RENDER_TEXTURED_WIRE) {
           draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,triangle.texcoords[2].u, triangle.texcoords[2].v,
                &triangle.color // dummy variable to comply with the signature                 
                );        
       }
        
        if(currentRenderingMode == RENDER_WIRE_VERTEX) {
            disp_drawRect(triangle.points[0].x - 3, triangle.points[0].y - 3, 4, 4, COLOR_RED); 
            disp_drawRect(triangle.points[1].x - 3, triangle.points[1].y - 3, 4, 4, COLOR_RED); 
            disp_drawRect(triangle.points[2].x - 3, triangle.points[2].y - 3, 4, 4, COLOR_RED);
        }

        if(currentRenderingMode == RENDER_SOLID) {
            draw_solid_triangle(&triangle); 
        }

        if(currentRenderingMode == RENDER_WIRE || currentRenderingMode == RENDER_WIRE_VERTEX || currentRenderingMode == RENDER_SOLID_WIRE || currentRenderingMode == RENDER_TEXTURED_WIRE) {
            disp_drawTriangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_GRAY                  
                );
        } 
    } 
    disp_renderFramebuffer();
    disp_clearFramebuffer(COLOR_BLACK);
    disp_clearZbuffer();
    disp_updateSDL(); 
}


void free_resources(Renderer* renderer) {
    disp_free();
    if(renderer->renderQueue) free(renderer->renderQueue);
    mesh_free();
    texture_free();
}
