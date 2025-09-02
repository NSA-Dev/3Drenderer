#ifndef CLIPPING_H
#define CLIPPING_H

#define  NUM_PLANES 6
#include "vector.h"
 

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct  {
    vec3_t point;
    vec3_t normal;
} plane_t; 


extern plane_t g_viewPlanes[NUM_PLANES];

void init_frustum_planes(float fov, float zNear, float zFar); 

#endif
