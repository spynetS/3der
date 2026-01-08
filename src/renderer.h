#ifndef RENDERER_H
#define RENDERER_H
#include "canvas.h"
#include "vector.h"

typedef struct {
	Vec3 v0;
	Vec3 v1;
	Vec3 v2;
	float *indices;
} Triangle;

typedef struct {
	Vec3 camera_pos;
	Vec3 camera_up;
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


void render_triangle(Canvas *canvas, Renderer *renderer, Triangle *triangle);
	
#endif
