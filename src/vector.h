#ifndef VECTOR_H
#define VECTOR_H

void print_vector(float *u, int size);
float vector_dot(float *a, float *b, int size);
float vector_len(float *a, int size);
float vector_len_squared(float *a, int size);
void vector_lambda_mult(float *output, float *a, float lambda, int size);
void vector_add(float *c, float *a, float *b, int size);
void vector_subtract(float *c, float *a, float *b, int size);
void vector_cross(float *c, float *a, float *b, int size);
void vector_normalize(float *c, float *a, int size);
void vector_4x4_4(float *c, float matrix[4][4], float u[3]);
	
#endif
