#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "mesh.h"
#include "array.h"
#include "colors.h"

mesh_t mesh = {
    .verts = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0},
    .scale ={1.0, 1.0, 1.0},  // neutral scale factor  
    .translation = {0, 0, 0}

};

vec3_t cube_verts[N_CUBE_VERTS] = {
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
face_t cube_faces[N_CUBE_FACES] = {
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

void load_cube_mesh(void) {
    for (int i = 0; i < N_CUBE_VERTS; i++) {
        vec3_t cube_v = cube_verts[i];
        array_push(mesh.verts, cube_v); 
    }

    for (int i = 0; i < N_CUBE_FACES; i++) {
        face_t cube_f = cube_faces[i];
        array_push(mesh.faces, cube_f); 
    }
}

bool load_mesh_data(char* fname) {
	
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
            array_push(mesh.verts, vector);
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

            array_push(mesh.faces, face); 
        }
    }
    array_free(uvCoords); 
    return true; 
}
