#include "camera.h"


static camera_t camera = {
	.position = {0, 0, 0},
	.direction = {0, 0, 1},
	.forwardVelocity = {0, 0, 0},
	.yaw = 0.0
}; 

void initCamera(void) {
	camera.position = (vec3_t) {0, 0, 0};
	camera.direction = (vec3_t) {0, 0, 1}; 
	camera.forwardVelocity = (vec3_t) {0, 0, 0};
	camera.yaw = 0.0;
	camera.pitch = 0.0;   
}

void adjustCameraYaw(float angle) {
	camera.yaw += angle; 
}

void adjustCameraPitch(float angle) {
	camera.pitch += angle; 
}

void adjustCameraPositionX(float factor) {
	camera.position.x += factor;
}

void adjustCameraPositionY(float factor) {
	camera.position.y += factor;
}

void adjustCameraPositionZ(float factor) {
	camera.position.z += factor; 
}

void setCameraForwardVelocity(vec3_t v) {
	camera.forwardVelocity = v; 
}

void setCameraPosition(vec3_t v) {
	camera.position = v; 
}

void setCameraDirection(vec3_t v) {
	camera.direction = v; 
} 

void cameraMoveForward(float factor) {
    camera.forwardVelocity = vec3_mul(&camera.direction,factor);
    camera.position = vec3_add(&camera.position, &camera.forwardVelocity);
}

void cameraMoveBackward(float factor) {
    camera.forwardVelocity = vec3_mul(&camera.direction, factor);
    camera.position = vec3_sub(&camera.position, &camera.forwardVelocity);
}

float getCameraYaw(void) {
	return camera.yaw;
}

vec3_t getCameraDirection(void) {
	return camera.direction;
}
vec3_t getCameraPosition(void) {
	return camera.position;
}
