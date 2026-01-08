
#ifndef VECTOR_H
#define VECTOR_H

typedef union {
    struct { float x, y, z; };
    float data[3];
} Vec3;


void print_vector(Vec3 *u);
float vector_dot(Vec3 *a, Vec3 *b);
float vector_len(Vec3 *a);
float vector_len_squared(Vec3 *a);
void vector_lambda_mult(Vec3 *output, Vec3 *a, float lambda);
void vector_add(Vec3 *c, Vec3 *a, Vec3 *b);
void vector_subtract(Vec3 *c, Vec3 *a, Vec3 *b);
void vector_cross(Vec3 *c, Vec3 *a, Vec3 *b);
void vector_normalize(Vec3 *c, Vec3 *a);
void vector_4x4_4(float *c, float matrix[4][4], float u[3]);
void vector_lookAt(Vec3 result[4][4], Vec3 eye[3], Vec3 target[3], Vec3 up[3]);

#endif
