#ifndef MESH_H
#define MESH_H
#include "vector.h"
#include "triangle.h"
#include "upng.h"

// These declarations correspond to the fallback default model of the cube
#define N_CUBE_VERTS 8
#define N_CUBE_FACES (6 * 2) // each cube face (side) 2 triangles

typedef struct {
    vec3_t* verts;
    face_t* faces;
    upng_t* texture;
    vec3_t rotation;
    vec3_t scale;
    vec3_t translation;  
} mesh_t;

// Functions
void loadDefaultCube(void);
bool loadObjMeshData(char* fname, mesh_t* mesh);
bool loadMesh(char* objFileName, char* pngTextureFileName, vec3_t scale, vec3_t translation, vec3_t rotation); 
bool loadMeshTexture(char* pngTexFileName, mesh_t* mesh);
int getMeshCount(void);
mesh_t* getMeshPtr(int index);
void freeMeshes(void);
int getTotalFaceCount(void);  
#endif
