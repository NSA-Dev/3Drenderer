#ifndef CAMERA_H
#define CAMERA_H
#include "vector.h"

typedef struct {
	vec3_t position; 
	vec3_t direction;
	vec3_t forwardVelocity;
	float  yaw; // angle in radians representing rotation around Y
} camera_t; 

camera_t* getCameraPtr(void); 
#endif
