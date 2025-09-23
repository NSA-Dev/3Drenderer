#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTS 8
#define N_CUBE_FACES (6 * 2) // each cube face (side) 2 triangles



typedef struct {
    vec3_t* verts;
    face_t* faces;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;  
} mesh_t;



// Functions
void load_cube_mesh(void);
bool load_mesh_data(char* fname); // parses provided .obj file, copie data into mesh_t array 
mesh_t* getMeshPtr(void);
void mesh_setRotationX(float f);
void mesh_setRotationY(float f);
void mesh_setRotationZ(float f);
void mesh_free(void);  
#endif
