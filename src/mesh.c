#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mesh.h"
#include "array.h"
#include "colors.h"
#include "upng.h"

#define MESH_LIMIT 10



static mesh_t meshes[MESH_LIMIT];
static int meshCount = 0; 

static vec3_t cube_verts[N_CUBE_VERTS] = {
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
static face_t cube_faces[N_CUBE_FACES] = {
    // front
    { .a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // right
    { .a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // back
    { .a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // left
    { .a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // top
    { .a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
    // bottom
    { .a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFFFF },
    { .a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
};

void loadDefaultCube(void) {
    if(meshCount < MESH_LIMIT) {
        mesh_t cubeMesh = {
        .verts = NULL,
        .faces = NULL,
        .rotation = {0, 0, 0},
        .scale ={1.0, 1.0, 1.0},  // neutral scale factor  
        .translation = {0, 0, 0}
        };
        // FIX ME: Push default mesh into global mesh Array
        for (int i = 0; i < N_CUBE_VERTS; i++) {
            vec3_t cube_v = cube_verts[i];
            array_push(cubeMesh.verts, cube_v); 
        }

        for (int i = 0; i < N_CUBE_FACES; i++) {
            face_t cube_f = cube_faces[i];
            array_push(cubeMesh.faces, cube_f); 
        }
        cubeMesh.texture = (upng_t*) fetchDefaultTexture();
        meshes[meshCount++] = cubeMesh;
    }
    printf("Unable to load default cube. Mesh buffer is full\n"); 
}

bool loadObjMeshData(char* fname, mesh_t* mesh) {
	
	// store VT in an array
    FILE* fp;
    char str[1024];
    tex2_t* uvCoords = NULL; // init uv array to store tex data coords 

    if((fp = fopen(fname, "r")) == NULL) {
        printf("Error: couldn't open file\n");
        return false;
    }

    while(fgets(str, sizeof(str), fp) != NULL) {
        
        // Vertex data expects: "v  1.0000 2.000 3.000"
        if(!strncmp(str, "v ", 2)) {
            vec3_t vector;
            sscanf(str, "v %f %f %f", &vector.x, &vector.y, &vector.z);
            array_push(mesh->verts, vector);
        }
        // UV data expects: "vt 1.000000 0.000000"
        if(!strncmp(str, "vt ", 3)) {
			tex2_t texCoord; // v orientation is inverted relative to standard .obj
			sscanf(str, "vt %f %f", &texCoord.u, &texCoord.v);
			array_push(uvCoords, texCoord); 
		}
        // Face data expects: "f 1/2/3 1/2/3 1/2/3 "
        if(!strncmp(str, "f ", 2)) {
            int vert_i[3];
            int texture_i[3];
            int normal_i[3];

            sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                   &vert_i[0], &texture_i[0], &normal_i[0],
                   &vert_i[1], &texture_i[1], &normal_i[1],
                   &vert_i[2], &texture_i[2], &normal_i[2]
                  );
            
            face_t face = {
				// offset the .obj indexing to C conventions 
                .a = vert_i[0] - 1,
                .b = vert_i[1] - 1,
                .c = vert_i[2] - 1,
                // - 1 offset because .obj indexes from 1 
                .a_uv = uvCoords[texture_i[0] - 1],
                .b_uv = uvCoords[texture_i[1] - 1],
                .c_uv = uvCoords[texture_i[2] - 1]
            }; 
            array_push(mesh->faces, face); 
        }
    }
    array_free(uvCoords); 
    return true; 
}


bool loadMesh(char* objFileName, char* pngTextureFileName, vec3_t scale, vec3_t translation, vec3_t rotation) {
    if(meshCount < MESH_LIMIT) {
        if(loadObjMeshData(objFileName, &meshes[meshCount])) {
            if(!loadMeshTexture(pngTextureFileName, &meshes[meshCount])) {
                printf("Unable to load texture: %s << loading default texture\n", pngTextureFileName);
                upng_t* fallbackTexture = (upng_t*) fetchDefaultTexture();
                meshes[meshCount].texture = fallbackTexture;
            };
            meshes[meshCount].scale = scale;
            meshes[meshCount].translation = translation;
            meshes[meshCount].rotation = rotation;
            meshCount++;
            return true;
        }
        printf("Unable to load mesh: %s << Skipped\n", objFileName);
        return false; 
    }
    printf("Mesh limit exceeded\n%s is skipped.\n", objFileName);
    return false;    
}

bool loadMeshTexture(char* pngTexFileName, mesh_t* mesh) {	
	upng_t* png_texture = upng_new_from_file(pngTexFileName);
	
	if (png_texture != NULL) {
		upng_decode(png_texture);
		if(upng_get_error(png_texture) == UPNG_EOK) {
			mesh->texture = (upng_t*) upng_get_buffer(png_texture);
			return true; 
		}
	}
	return false; 
}

int getMeshCount(void) {
    return meshCount; 
}

mesh_t* getMeshPtr(int index) {
    return &meshes[index];
}

void freeMeshes(void) {
        for(int i = 0; i < meshCount; i++) {
            upng_free(meshes[i].texture);
            array_free(meshes[i].faces);
            array_free(meshes[i].verts); 
        }
}


int getTotalFaceCount(void) {
    int total = 0; 
    if(meshCount > 0) {
        for(int i = 0; i < meshCount; i++) {
            total += array_length(meshes[i].faces);
        }
    }
    return total; 
}
