#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void print_vector(float *u, int size) {
	printf("<");
	for(int i = 0; i < size; i ++){
		printf("%f,",u[i]);
	}
	printf(">\n");
}


float vector_dot(float *a, float *b, int size) {
	assert(a != NULL);
	assert(b != NULL);
	assert(size > 0);
	float result = 0;
	for(int i = 0; i < size; i ++) {
		result += (a[i]*b[i]);
	}
	return result;
}

float vector_len_squared(float *a, int size) {
	return vector_dot(a,a,size);
}

float vector_len(float *a, int size) {
	return (float) sqrt(vector_len_squared(a,size));
}

void vector_lambda_mult(float *output, float *a, float lambda, int size) {
	for(int i = 0 ; i < size; i ++) {
		output[i] = a[i] * lambda;
	}
}

void vector_add(float *c, float *a, float *b, int size) {
	for(int i = 0 ; i < size; i ++) {
		c[i] = a[i]+b[i];
	}
}
void vector_subtract(float *c, float *a, float *b, int size) {
	for(int i = 0 ; i < size; i ++) {
		c[i] = a[i]-b[i];
	}
}

void vector_cross(float *c, float *u, float *v, int size) {
	if(size == 3){
		c[0] = u[1] * v[2] - u[2] * v[1];
		c[1] = u[2] * v[0] - u[0] * v[2];
		c[2] = u[0] * v[1] - u[1] * v[0];
	}	
}

void vector_normalize(float *c, float *a, int size) {
	float length = vector_len(a, size);
	if (length == 0.0f) return; // Avoid division by zero
	for(int i = 0; i < size; i ++){
		c[i] = a[i] / length;
	}
}
