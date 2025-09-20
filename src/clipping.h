#ifndef CLIPPING_H
#define CLIPPING_H

#define  NUM_PLANES 6
#define  VERT_LIMIT 10  // arbitrary
#define  CLIP_TRIANGLE_LIMIT 10
#include "vector.h"
#include "triangle.h"

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


void initFrustumPlanes(float fov_x, float fov_y, float z_near, float z_far);
polygon_t createPolygon(vec3_t* v0, vec3_t* v1, vec3_t* v2);
void clipPolygon(polygon_t* polygon); 
void clipAgainstPlane(polygon_t* polygon, planeIndex_t i);
vec3_t calculateIntersection(vec3_t* v0, vec3_t* v1, float d0, float d1);
int getTriangleCount(polygon_t* polygon); 
void slicePolygon(polygon_t* polygon, triangle_t storage[], int* slicesCounter); 
/*
****    DEBUG   ****
polygon_t debug_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2);
void debug_triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);
void debug_clip_polygon_against_plane(polygon_t* polygon, int plane);
void debug_clip_polygon(polygon_t* polygon); 
****    END OF DEBUG    ****
*/

#endif
