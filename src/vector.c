#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "vector.h"

void print_vector(Vec3 *u) {
	printf("<");
	for(int i = 0; i < 3; i ++){
		printf("%f,",u->data[i]);
	}
	printf(">\n");
}

float vector4_dot(float *a, float *b, int size) {
	assert(a != NULL);
	assert(b != NULL);
	assert(size > 0);
	float result = 0;
	for(int i = 0; i < size; i ++) {
		result += (a[i]*b[i]);
	}
	return result;
}


float vector_dot(Vec3 *a, Vec3 *b) {
	assert(a != NULL);
	assert(b != NULL);
	assert(3 > 0);
	float result = 0;
	for(int i = 0; i < 3; i ++) {
		result += (a->data[i]*b->data[i]);
	}
	return result;
}

float vector_len_squared(Vec3 *a) {
	return vector_dot(a,a);
}

float vector_len(Vec3 *a) {
	return (float) sqrt(vector_len_squared(a));
}

void vector_lambda_mult(Vec3 *output, Vec3 *a, float lambda) {
	for(int i = 0 ; i < 3; i ++) {
		output->data[i] = a->data[i] * lambda;
	}
}

void vector_add(Vec3 *c, Vec3 *a, Vec3 *b) {
	for(int i = 0 ; i < 3; i ++) {
		c->data[i] = a->data[i]+b->data[i];
	}
}
void vector_subtract(Vec3 *c, Vec3 *a, Vec3 *b) {
	for(int i = 0 ; i < 3; i ++) {
		c->data[i] = a->data[i]-b->data[i];
	}
}

void vector_cross(Vec3 *c, Vec3 *u, Vec3 *v) {
	if(3 == 3){
		c->x = u->y * v->z - u->z * v->y;
		c->y = u->z * v->x - u->x * v->z;
		c->z = u->x * v->y - u->y * v->x;
	}	
}

void vector_normalize(Vec3 *c, Vec3 *a) {
	float length = vector_len(a);
	if (length == 0.0f) return; // Avoid division by zero
	for(int i = 0; i < 3; i ++){
		c->data[i] = a->data[i] / length;
	}
}


void vector_4x4_4(float *c, float matrix[4][4], float u[4]) {
	c[0] = vector4_dot(matrix[0], u, 4);
	c[1] = vector4_dot(matrix[1], u, 4);
	c[2] = vector4_dot(matrix[2], u, 4);
	c[3] = vector4_dot(matrix[3], u, 4);
}
