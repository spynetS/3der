#pragma once

#include "canvas.h"
#include "vector.h"

typedef struct {
	Vec3 pos;
	Vec3 rot;
	Vec3 scale;
} Transform;

typedef struct {
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
} Triangle;

typedef struct {
	Transform transform;
	Triangle *triangles;
	size_t triangle_length; 
} Object;

typedef struct {
	Vec3 camera_up;
	Vec3 pos;
	Vec3 rot;
	float f;
	float zfar;
	float znear;
} Camera;

typedef struct {
	float **depth_buffer; // x y index for depth 
	int width;
	int height;
	Camera camera;
} Renderer;

void set_fov(Camera* camera, float deg);

void transform_matrix(float matrix[4][4], Transform t);

// we take triangle as value because we dont want to change its
// parameters
void render_triangle(Canvas *canvas, Renderer *renderer, Triangle triangle);
	
