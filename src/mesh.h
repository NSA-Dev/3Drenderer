#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTS 8
#define N_CUBE_FACES (6 * 2) // each cube face (side) 2 triangles

void load_cube_mesh(void);

extern vec3_t cube_verts[N_CUBE_VERTS]; 
// This one holds vertex indexes to select from mesh_verts[]
extern face_t cube_faces[N_CUBE_FACES];

typedef struct {
    vec3_t* verts;
    face_t* faces;
    vec3_t rotation;  
} mesh_t;

// Global mesh
extern mesh_t mesh;

#endif
