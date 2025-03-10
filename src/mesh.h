#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

#define N_MESH_VERT 8
#define N_MESH_FACES (6 * 2) // each cube face (side) 2 triangles

extern vec3_t mesh_verts[N_MESH_VERT]; 
// This one holds vertex indexes to select from mesh_verts[]
extern face_t mesh_faces[N_MESH_FACES];



#endif
