#include "clipping.h"
#include <math.h>
#include "display.h"

static plane_t viewPlanes[NUM_PLANES]; // contains view frustum planes responsible for clipping


void initFrustumPlanes(float fov_x, float fov_y, float z_near, float z_far) {
    float cos_half_fov_x = cos(fov_x / 2.0); 
    float sin_half_fov_x = sin(fov_x / 2.0);
    float cos_half_fov_y = cos(fov_y / 2.0);
    float sin_half_fov_y = sin(fov_y / 2.0);

    viewPlanes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    viewPlanes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
	viewPlanes[LEFT_FRUSTUM_PLANE].normal.y = 0;
	viewPlanes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	viewPlanes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	viewPlanes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
	viewPlanes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
	viewPlanes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

	viewPlanes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	viewPlanes[TOP_FRUSTUM_PLANE].normal.x = 0;
	viewPlanes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
	viewPlanes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	viewPlanes[BOTTOM_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
	viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
	viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
	viewPlanes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

	viewPlanes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
	viewPlanes[NEAR_FRUSTUM_PLANE].normal.x = 0;
	viewPlanes[NEAR_FRUSTUM_PLANE].normal.y = 0;
	viewPlanes[NEAR_FRUSTUM_PLANE].normal.z = 1;

	viewPlanes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
	viewPlanes[FAR_FRUSTUM_PLANE].normal.x = 0;
	viewPlanes[FAR_FRUSTUM_PLANE].normal.y = 0;
	viewPlanes[FAR_FRUSTUM_PLANE].normal.z = -1;
    
}


polygon_t createPolygon(vec3_t* v0, vec3_t* v1, vec3_t* v2, tex2_t t0, tex2_t t1, tex2_t t2) {
    polygon_t res = {
        .verts = {*v0,*v1, *v2},
        .num_verts = 3,
        .texcoords = { t0, t1, t2} 
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
    printf("Clipping....\n");
    vec3_t* planePoint = &viewPlanes[i].point; 
    vec3_t* planeNormal = &viewPlanes[i].normal;
    // Declare the array corresponding to the inside vert & tex list
    vec3_t insideVerts[VERT_LIMIT];
    tex2_t insideTexcoords[VERT_LIMIT]; 
    int insideCounter = 0;
    vec3_t substracted; // temp variable

    // Set current & last vertex & texcoord pointers (On init: current - first prev - last)
    vec3_t* currentVertex = &polygon->verts[0];
    tex2_t* currentTexcoord = &polygon->texcoords[0]; 
    vec3_t* previousVertex = &polygon->verts[polygon->num_verts - 1]; 
    tex2_t* previousTexcoord = &polygon->texcoords[polygon->num_verts - 1]; 
    
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
            // Update vert and tex coords via linear interpolation, pass into the inside list
            float t = previousDot / (previousDot - currentDot);
            
            vec3_t intersectionPoint;
            intersectionPoint.x = float_lerp(previousVertex->x, currentVertex->x, t);
            intersectionPoint.y = float_lerp(previousVertex->y, currentVertex->y, t);
            intersectionPoint.z = float_lerp(previousVertex->z, currentVertex->z, t); 
            tex2_t interpolatedTexcoord = {
                    .u = float_lerp(previousTexcoord->u, currentTexcoord->u, t),
                    .v = float_lerp(previousTexcoord->v, currentTexcoord->v, t)  
            };
            
            insideTexcoords[insideCounter] = interpolatedTexcoord;    
            insideVerts[insideCounter] = intersectionPoint;
            insideCounter++; 
        }        
        if(currentDot > 0) {
            insideVerts[insideCounter] = *currentVertex;
            insideTexcoords[insideCounter] = *currentTexcoord; 
            insideCounter++;   
        }
        previousDot = currentDot; 
        previousVertex = currentVertex;
        previousTexcoord = currentTexcoord;  
        currentVertex++;
        currentTexcoord++;
    } 
    // Make a polygon out of resulting insideVertices and write to the polygon pased as a ref
    for(int i = 0; i < insideCounter; i++) {
        polygon->verts[i] = insideVerts[i];
        polygon->texcoords[i] = insideTexcoords[i]; 
    } 
    polygon->num_verts = insideCounter; 
}

float float_lerp(float a, float b, float factor) {
    return a + factor * (b - a); 
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
        storage[i].texcoords[0] = polygon->texcoords[index0];
        storage[i].texcoords[1] = polygon->texcoords[index1];
        storage[i].texcoords[2] = polygon->texcoords[index2];   
    }
    *slicesCounter = polygon->num_verts - 2;
    printf("SLICED: %d\n", polygon->num_verts - 2);
}
