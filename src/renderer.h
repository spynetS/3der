#ifndef RENDERER_H
#define RENDERER_H
#include "canvas.h"

typedef struct {
	float v0[3];
	float v1[3];
	float v2[3];
	float *indices;
} Triangle;

typedef struct {
	float camera_pos[3];
	float camera_up[3];
	float rot[3];
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
