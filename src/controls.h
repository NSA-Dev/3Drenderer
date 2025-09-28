#ifndef CONTROLS_H
#define CONTROLS_H

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
 
void setControlMode(ControlMode mode);
ControlMode getControlMode(void); 

#endif
