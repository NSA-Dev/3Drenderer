#ifndef RENDERER_H
#define RENDERER_H
#include <stdint.h>
#include "matrix.h"
#include "triangle.h"
#include "mesh.h"
#include "camera.h"

#define MAX_TRIANGLES 10000  
#define FPS 60
#define FTT (1000 / FPS)
#define DEFAULT_CAM_DEPTH 5
#define MESH_ROTATION_FACTOR 1.2
#define MESH_SPIN_FACTOR 0.6
#define MESH_SCALE_FACTOR 1.2
#define CAM_POS_FACTOR 2.5
#define CAM_YAW_FACTOR 1.0 
#define CAM_VELOCITY_FACTOR 5.0 

typedef enum {
	MANUAL,
	SPIN
}ControlMode;

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

typedef struct {
	mat4_t viewMatrix; 
	mat4_t worldMatrix;
	triangle_t* renderQueue;
	uint32_t polyCount;
	uint32_t triangleCount;
	mat4_t projectionMatrix;
	RenderingMode renderingMode;
	CullMethod cullMethod;
	LightMethod lightMethod;
	ControlMode controlMode;
	bool isRunning;
	camera_t* cameraPtr;
	mesh_t* meshPtr;   
} Renderer; 



#endif
