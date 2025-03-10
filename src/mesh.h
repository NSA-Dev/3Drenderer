#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

#define N_MESH_VERT 8
#define N_MESH_FACES (6 * 2) // each cube face (side) 2 triangles

vec3_t mesh_verts[N_MESH_VERT] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y =  1, .z = -1}, // 2
    {.x =  1, .y =  1, .z = -1}, // 3
    {.x =  1, .y = -1, .z = -1}, // 4
    {.x =  1, .y =  1, .z =  1}, // 5
    {.x =  1, .y = -1, .z =  1}, // 6
    {.x = -1, .y =  1, .z =  1}, // 7
    {.x = -1, .y = -1, .z =  1}  // 8
}; 

// This one holds vertex indexes to select from mesh_verts[]
face_t mesh_faces[N_MESH_FACES] = {
    // front 
    {.a = 1, .b = 2, .c = 3 },           
    {.a = 1, .b = 3, .c = 4 },
    // right
    {.a = 4, .b = 3, .c = 5 },
    {.a = 4, .b = 5, .c = 6 },
    // back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},
    // left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},
    // top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},
    // bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4},

};



#endif
