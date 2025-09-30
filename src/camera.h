#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

#define CAM_POS_FACTOR 2.5
#define CAM_YAW_FACTOR 1.0 
#define CAM_VELOCITY_FACTOR 5.0

typedef struct {
	vec3_t position; 
	vec3_t direction;
	vec3_t forwardVelocity;
	float  yaw; // angle in radians representing rotation around Y
	float pitch;
} camera_t; 

void initCamera(void); 
void adjustCameraYaw(float angle);
void adjustCameraPitch(float angle);
void adjustCameraPositionX(float factor);
void adjustCameraPositionY(float factor);
void adjustCameraPositionZ(float factor);
void setCameraForwardVelocity(vec3_t v);
void setCameraPosition(vec3_t v);
void setCameraDirection(vec3_t v); 
void cameraMoveForward(float factor);
void cameraMoveBackward(float factor);
float getCameraYaw(void);
vec3_t getCameraDirection(void);
vec3_t getCameraPosition(void);   
#endif
