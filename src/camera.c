#include "camera.h"


static camera_t camera = {
	.position = {0, 0, 0},
	.direction = {0, 0, 1},
	.forwardVelocity = {0, 0, 0},
	.yaw = 0.0
}; 

camera_t* getCameraPtr(void) {
	return &camera; 
}
