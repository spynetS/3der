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


void draw_point_colored(float x_proj, float y_proj, int width, int height, char* c, int* rgb);
void draw_point(float x_proj, float y_proj, int width, int height, const char* c);
void render_triangle(Canvas *canvas, Renderer *renderer, Triangle *triangle);
	
#endif
