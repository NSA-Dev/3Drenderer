#ifndef CLIPPING_H
#define CLIPPING_H

#define  NUM_PLANES 6
#define  VERT_LIMIT 10  // arbitrary
#include "vector.h"
 

typedef enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
} planeIndex_t;

typedef struct  {
    vec3_t point;
    vec3_t normal;
} plane_t; 

// TODO find a way around this struct
typedef struct {
    vec3_t verts[VERT_LIMIT]; 
    int num_verts;
} polygon_t;


extern plane_t g_viewPlanes[NUM_PLANES];

void init_frustum_planes(float fov, float zNear, float zFar); 
polygon_t createPolygon(vec3_t* v0, vec3_t* v1, vec3_t* v2);
void clipPolygon(polygon_t* polygon); 
void clipAgainstPlane(polygon_t* polygon, planeIndex_t i); 
#endif
