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
    vec3_t* planePoint = &g_viewPlanes[i].point; 
    vec3_t* planeNormal = &g_viewPlanes[i].normal;

    // declare the array corresponding to the inside vert list
    vec3_t insideVerts[VERT_LIMIT];
    int insideCounter = 0;
    vec3_t substracted; // temp variable

    // set current & last vertex pointers (On init: current - first prev - last)
    vec3_t* currentVertex = &polygon->verts[0];
    vec3_t* previousVertex = &polygon->verts[polygon->num_verts - 1]; 
    
    // pre-calculate dot products for current & previous (last)
    // i.e. determine if curr & prev are inside the plane
    substracted = vec3_sub(previousVertex, planePoint); 
    float previousDot = vec3_dot(&substracted, planeNormal); 
    float currentDot = 0; 
    // The loop interval is alw limited by polygon->num_verts, which can go out of bounds?
    while(currentVertex != &polygon->verts[polygon->num_verts]) { 
        substracted = vec3_sub(currentVertex, planePoint);
        currentDot = vec3_dot(&substracted, planeNormal);
        // test for plane intersection between the current & prev vertices
        if(currentDot * previousDot < 0) {
            // if result above is negative we need to determine the intersection
            // and add it to the both lists  
            vec3_t intersectionPoint = calculateIntersection(currentVertex, 
                                                             previousVertex,
                                                             previousDot, currentDot);
            insideVerts[insideCounter] = intersectionPoint;
            insideCounter++; 
        }
        
        if(currentDot > 0) {
            insideVerts[insideCounter] = *currentVertex;
            insideCounter++;   
        }
        previousDot = currentDot; 
        previousVertex = currentVertex; 
        currentVertex++;
    } 


    // make a polygon out of resulting insideVertices and write to the polygon pased as a ref
    for(int i = 0; i < insideCounter; i++) {
        polygon->verts[i] = insideVerts[i];
    } 
    polygon->num_verts = insideCounter; 
}

vec3_t calculateIntersection(vec3_t* v0, vec3_t* v1, float d0, float d1) {
    // calculate intersection point I via I = V0 + t(V1 - V0)
    // where t is the interpolation factor and equal to:
    // t = d0 / (d0 - d1) with d0, d1 being respective dot products with a plane normal

    float t = d0 / (d0 - d1);
    vec3_t temp;

    temp = vec3_sub(v1, v0);
    temp = vec3_mul(&temp, t); 
    temp = vec3_add(&temp, v0);

    return temp;
};
