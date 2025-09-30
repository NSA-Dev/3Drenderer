#include "controls.h"

static ControlMode controlMode; 


void setControlMode(ControlMode mode) {
    controlMode = mode; 
}
ControlMode getControlMode(void) {
    return controlMode; 
}

