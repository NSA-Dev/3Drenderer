#include "clipping.h"
#include <math.h>
#include "display.h"

plane_t g_viewPlanes[NUM_PLANES]; // contains view frustum planes responsible for clipping

void init_frustum_planes(float fovY, float zNear, float zFar) {
    // Get screen dimensions and calculate their halves 
    float half_h = win_h / 2.0;
    float half_w = win_w / 2.0; 
    
    // Compute individual fov angles 
    // compute top
    float fovT = fovY / 2;
    //compute bottom
    float fovB = atan(tan(fovT) * (half_h - 1) / half_h);
    //compute left
    float fovL = atan(tan(fovT) * (half_w / half_h));
    // compute right
    float fovR = atan(tan(fovL) * (half_w - 1) / half_w);
 // Left plane  
    g_viewPlanes[LEFT_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.x = cos(fovL);
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[LEFT_FRUSTUM_PLANE].normal.z = sin(fovL);
 // Right plane  
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.x = -cos(fovR);
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[RIGHT_FRUSTUM_PLANE].normal.z = sin(fovR);
 // Top plane  
    g_viewPlanes[TOP_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.y = -cos(fovT);
    g_viewPlanes[TOP_FRUSTUM_PLANE].normal.z = sin(fovT);
 // Bottom plane  
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].point = (vec3_t) {0, 0, 0};
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.y = cos(fovB);
    g_viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.z = sin(fovB);
 // Near plane  
    g_viewPlanes[NEAR_FRUSTUM_PLANE].point = (vec3_t) {0, 0, zNear};
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    g_viewPlanes[NEAR_FRUSTUM_PLANE].normal.z = 1;
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
            // and add it to  both lists
            // Calculate intersection point
            float t = previousDot / (previousDot - currentDot);
            vec3_t intersectionPoint = *currentVertex;
            intersectionPoint = vec3_sub(&intersectionPoint, previousVertex);            
            intersectionPoint = vec3_mul(&intersectionPoint, t);
            intersectionPoint = vec3_add(&intersectionPoint, previousVertex); 
            
            // insert the plane intersection point into the inside vertex list 
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

int getTriangleCount(polygon_t* polygon) {
    return (int) polygon->num_verts - 2; 
}


// slices the polygon into individual triangles (according to the count) 
// and writes the results into provided storage array
void slicePolygon(polygon_t* polygon, triangle_t storage[],  int* slicesCounter) { 
    for(int i = 0; i < polygon->num_verts - 2; i++) {
        int index0 = 0;
        int index1 = i + 1;
        int index2 = i + 2;
        
        storage[i].points[0] = vec4_from_vec3(&polygon->verts[index0]);
        storage[i].points[1] = vec4_from_vec3(&polygon->verts[index1]); 
        storage[i].points[2] = vec4_from_vec3(&polygon->verts[index2]);  
    }
    // side effect
    *slicesCounter = polygon->num_verts - 2;
}
