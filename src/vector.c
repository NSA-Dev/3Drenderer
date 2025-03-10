#include <math.h>
#include "vector.h"



void vec3_rotate_x(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);
    
   float r_y = (v->y * r_cos) - (v->z * r_sin);
   float r_z = (v->z * r_cos) + (v->y * r_sin);
   
   v->y = r_y;
   v->z = r_z; 
}
void vec3_rotate_y(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);
    
   float r_x = (v->x * r_cos) + (v->z * r_sin);
   float r_z = (v->z * r_cos) - (v->x * r_sin);
   
   v->x = r_x;
   v->z = r_z;
}
void vec3_rotate_z(vec3_t* v, float angle) {
   float r_cos = cos(angle);
   float r_sin = sin(angle);

   float r_x = (v->x * r_cos) - (v->y * r_sin);
   float r_y = (v->y * r_cos) + (v->x * r_sin);

   v->x = r_x;
   v->y = r_y;
}
