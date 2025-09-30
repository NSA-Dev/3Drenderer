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
mat4_t g_worldMatrix; 
triangle_t* g_renderQueue = NULL; 
uint32_t g_polyCount  = 0;
uint32_t g_triangleCounter = 0; 
bool is_running = false;
int previous_frame_time = 0; // ms
float g_deltaTime = 0; 
mat4_t proj_matrix;
char modelPath[] = "./assets/crab.obj"; 
char texturePath[] = "./assets/crab.png"; 

 
bool setup(void);
void process_input(void);
void update(void);
void render(void);
void free_resources(void);

int main(void) {
    is_running = init_window();
    
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
    
    // Init view settings 
    float aspect_y = (float)getWindowHeight() / (float)getWindowWidth();
    float aspect_x = (float)getWindowWidth() / (float)getWindowHeight();
    float fov_y = 3.141592 / 2.0; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float z_near = 1.0;
    float z_far = 20.0;
    proj_matrix = mat4_make_perspective(fov_y, aspect_y, z_near, z_far);
    
    // Initialize frustum planes with a point and a normal
    initFrustumPlanes(fov_x, fov_y, z_near, z_far);  
    
    // .png Texture loading 
    if(!load_png_textureData(texturePath)) {
			printf("Error: unable to open provided texture.\n Loading default..."); 
			mesh_texture = (uint32_t*) REDBRICK_TEXTURE; // Load test texture (hardcoded)
	}
    vec3_t lightOrigin = {0, 0, 1};
    vec3_t camOrigin = {0, 0, 0};
    vec3_t camDir = {0, 0, 1}; // look at positive Z
    initLight(lightOrigin);
    initCamera(camOrigin, camDir);  
    // Set default config here
    setRenderingMode(RENDER_WIRE_VERTEX);
    setCullMethod(CULL_BACKFACE);
    setLightMethod(LIGHT_NONE);
    setControlMode(SPIN);   
    return true; 
}

void process_input(void) {
    LightMethod currentLightMethod = getLightMethod();
    ControlMode currentControlMode = getControlMode(); 
    SDL_Event event; 
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                // Quit the program
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    is_running = false;
                    break;
                }
                // Mesh controls
                if(currentControlMode == MESH) {
                if (event.key.keysym.sym == SDLK_LEFT) {
                    mesh.rotation.y += MESH_ROTATION_FACTOR * g_deltaTime;
                    break;
                }
                if(event.key.keysym.sym == SDLK_RIGHT) {
                    mesh.rotation.y -= MESH_ROTATION_FACTOR * g_deltaTime;
                    break;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    mesh.rotation.x += MESH_ROTATION_FACTOR * g_deltaTime;
                    break;
                }
                if(event.key.keysym.sym == SDLK_DOWN) {
                    mesh.rotation.x -= MESH_ROTATION_FACTOR * g_deltaTime;
                    break;
                }
                // stretch the mesh
                if(event.key.keysym.sym == SDLK_q) {
                    mesh.scale.x += MESH_SCALE_FACTOR * g_deltaTime;
                    break;
                } 
                if(event.key.keysym.sym == SDLK_e) {
                    mesh.scale.x -= MESH_SCALE_FACTOR * g_deltaTime;
                    break;
                } 
                }
                // Camera controls 
                if(event.key.keysym.sym == SDLK_KP_PLUS) {
                    adjustCameraPitch(CAM_PITCH_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_MINUS) {
                    adjustCameraPitch(-CAM_PITCH_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_d) {
                    adjustCameraYaw(CAM_YAW_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_a) {
                    adjustCameraYaw(-CAM_YAW_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_w) {
                    cameraMoveForward(CAM_VELOCITY_FACTOR * g_deltaTime);
                    break;  
                }
                if(event.key.keysym.sym == SDLK_s) {
                    cameraMoveBackward(CAM_VELOCITY_FACTOR * g_deltaTime); 
                    break;   
                }
                if(event.key.keysym.sym == SDLK_KP_8) {
                    adjustCameraPositionZ(CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_2) {
                    adjustCameraPositionZ(-CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_6) {
                    adjustCameraPositionX(CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_4) {
                    adjustCameraPositionX(-CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_9) {
                    adjustCameraPositionY(CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                if(event.key.keysym.sym == SDLK_KP_3) {
                    adjustCameraPositionY(-CAM_POS_FACTOR * g_deltaTime);
                    break;
                } 
                // Rendering modes
                if(event.key.keysym.sym == SDLK_F1) {
                    setRenderingMode(RENDER_WIRE);
                    break;
                }    
                if (event.key.keysym.sym == SDLK_F2) {
                    setRenderingMode(RENDER_WIRE_VERTEX);
                    break; 
                }
                if(event.key.keysym.sym == SDLK_F3) {
                    setRenderingMode(RENDER_SOLID);
                    break; 
                }
                if (event.key.keysym.sym == SDLK_F4) {
                    setRenderingMode(RENDER_SOLID_WIRE);
                    break; 
                }
                if(event.key.keysym.sym == SDLK_F5){
                    setCullMethod(CULL_BACKFACE);
                    break;
                }
                if(event.key.keysym.sym == SDLK_F6) {
                    setCullMethod(CULL_NONE);
                    break;       
                }    
                if(event.key.keysym.sym == SDLK_F7) {
                    if(mesh_texture != NULL) {
                        setRenderingMode(RENDER_TEXTURED);
                        break;   
                    }
                }
                if(event.key.keysym.sym == SDLK_F8) { 
                if(currentLightMethod == LIGHT_NONE) {
                        setLightMethod(LIGHT_BASIC);
                        break;
                    } else {
                        setLightMethod(LIGHT_NONE);
                        break; 	
                    }
                }
                if(event.key.keysym.sym == SDLK_F9) {
                if(currentControlMode == SPIN) {
                        setControlMode(MESH);
                        break;
                    } else {
                        setControlMode(SPIN);
                        break; 	
                    }
                }
                    
                break; 
        }
    }
    
    
}

// TEST: if the camera math actually checks out (dir and pitch vector addition)
void update(void) { 
    // Synchronize frames before proceeding
    int elapsedTime = SDL_GetTicks() - previous_frame_time; 
    int waitTime = FTT - elapsedTime; 
    if (waitTime > 0 && waitTime <= FTT) {
        SDL_Delay(waitTime); 
    }
    g_deltaTime = (SDL_GetTicks() - previous_frame_time) / 1000.0; 
    previous_frame_time = SDL_GetTicks(); 
    // Poll current state
    CullMethod currentCulling = getCullMethod();
    LightMethod currentLight = getLightMethod();
    ControlMode currentControls = getControlMode();
    int currentWidth = getWindowWidth();
    int currentHeight = getWindowHeight();
    // Reset the global triangle counter for this frame
    g_triangleCounter = 0; 

    // Adjust mesh by default values (e.g. if spin mode is active) 
    if(currentControls == SPIN) mesh.rotation.y += MESH_SPIN_FACTOR * g_deltaTime; 
    mesh.translation.z = DEFAULT_CAM_DEPTH; 

    // Set up default camera  target (looking at the positive z-axis)
    vec3_t targetPoint = { 0, 0, 1 }; 
    vec3_t upDirection = { 0, 1, 0 };
    // Update camera direction with current pitch and yaw
    mat4_t cameraYawRotation = mat4_make_rotation_y(getCameraYaw());
    mat4_t cameraPitchRotation = mat4_make_rotation_x(getCameraPitch());
    vec4_t vec4_targetPoint = vec4_from_vec3(&targetPoint);
    vec4_t directionProduct = mat4_mult_vec4(&cameraYawRotation, &vec4_targetPoint);
    vec4_t pitchProduct = mat4_mult_vec4(&cameraPitchRotation, &vec4_targetPoint);
    vec3_t vec3_directionProduct = vec3_from_vec4(&directionProduct);
    vec3_t vec3_pitchProduct = vec3_from_vec4(&pitchProduct);
    vec3_t  currentDirection = vec3_add(&vec3_directionProduct, &vec3_pitchProduct); 
    setCameraDirection(currentDirection);
    vec3_t cameraPos, cameraDir;
    cameraPos = getCameraPosition();
    cameraDir = getCameraDirection();  
    targetPoint = vec3_add(&cameraPos, &cameraDir); 
    // Create the view matrix
    g_viewMatrix = mat4_look_at(cameraPos, targetPoint, upDirection); 
    
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
        if(currentCulling == CULL_BACKFACE) {
           vec3_t origin = { 0, 0, 0 };
           vec3_t cameraRay = vec3_sub(&origin, &A);
           float alignmentFactor = vec3_dot(&normal, &cameraRay);
           if(alignmentFactor < 0) continue;  
        }

        // Lighting
        if(currentLight == LIGHT_BASIC) {
                vec3_t lightDirection = getLightDirection();
                float ray_alignment = -vec3_dot(&normal, &lightDirection); 
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
                projectedVertices[j].x *= (currentWidth / 2.0); 
                projectedVertices[j].y *= (currentHeight / 2.0); 
                projectedVertices[j].x += (currentWidth / 2.0);
                projectedVertices[j].y += (currentHeight / 2.0);  
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
    clear_framebuffer(COLOR_BLACK);
    clear_Zbuffer();
    draw_grid(GRID_SPACING, COLOR_LIGHT_GRAY);         
    
    // draw all of the triangles residing in g_renderQueue based on the rendering mode selected  
    for(int i = 0; i < g_triangleCounter; i++) {
        triangle_t triangle = g_renderQueue[i];
        
       if(renderTexture()) {
           draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,triangle.texcoords[1].u, triangle.texcoords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,triangle.texcoords[2].u, triangle.texcoords[2].v,
                &triangle.color // dummy variable to comply with the signature                 
                );        
       }
        
        if(renderVerts()) {
            draw_rect(triangle.points[0].x - 3, triangle.points[0].y - 3, 4, 4, COLOR_RED); 
            draw_rect(triangle.points[1].x - 3, triangle.points[1].y - 3, 4, 4, COLOR_RED); 
            draw_rect(triangle.points[2].x - 3, triangle.points[2].y - 3, 4, 4, COLOR_RED);
        }

        if(renderSolids()) {
            draw_solid_triangle(&triangle); 
        }

        if(renderWire()) {
            draw_triangle(
                triangle.points[0].x, triangle.points[0].y,
                triangle.points[1].x, triangle.points[1].y,
                triangle.points[2].x, triangle.points[2].y,
                COLOR_GRAY                  
                );
        } 
    } 
    render_framebuffer(); 
}


void free_resources(void) {
    if(g_renderQueue != NULL) free(g_renderQueue);   
    array_free(mesh.faces);
    array_free(mesh.verts);
    upng_free(png_texture); 
}
