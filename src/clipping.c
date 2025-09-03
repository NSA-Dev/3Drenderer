#include "clipping.h"
#include <math.h>
plane_t g_viewPlanes[NUM_PLANES]; // contains view frustum planes responsible for clipping

void init_frustum_planes(float fov, float zNear, float zFar) {
    float sin_half_fov = sin(fov/2.0);
    float cos_half_fov = cos(fov/2.0);  
    
 // Left plane  
    g_viewPlanes[LEFT_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov;
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov;
 // Right plane  
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov;
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov;
 // Top plane  
    g_viewPlanes[TOP_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov;
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov;
 // Bottom plane  
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov;
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov;
 // Near plane  
    g_viewPlanes[NEAR_FRUSTUM_PLANE].point = (vec3_t) {0, 0, zNear};
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.z = 1;;
 // Far plane  
    g_viewPlanes[FAR_FRUSTUM_PLANE].point = (vec3_t) {0, 0, zFar};
    g_viewPlanes[FAR_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[FAR_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[FAR_FRUSTUM_PLANE].normal.z = -1;
    

} 

polygon_t createPolygon(vec3_t* v0, vec3_t* v1, vec3_t* v2) {
    polygon_t res = {
        .verts = {*v0,*v1, *v2},
        .num_verts = 3 
    };
    return res; 
}

void clipPolygon(polygon_t* polygon) {
    // Clipping order left > right > top > bottom > near > far
    // See clipping.h for enum def  
    for(planeIndex_t i = LEFT_FRUSTUM_PLANE; i <= FAR_FRUSTUM_PLANE; i++) {
        clipAgainstPlane(polygon, i); 
    }  
}

void clipAgainstPlane(polygon_t* polygon, planeIndex_t i) {
    // TODO 
}
