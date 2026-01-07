#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"
#include "vector.h"

double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
}

void setCursorPosition(int x, int y) {
	printf("\033[%d;%dH", y+1, x+1);
}

void setCharAt(int x, int y, const char *c) {
	setCursorPosition(x, y);
	printf("%s",c);
}


void draw_point_colored(float x_proj, float y_proj, int width, int height, char* c, int* rgb) {
	int scale = 1;
	int x_pixel = x_proj;
	int y_pixel = y_proj;


	if(rgb != NULL){
		char str[64];
		if(rgb[0] > 255) rgb[0] = 255;
		if(rgb[1] > 255) rgb[1] = 255;
		if(rgb[2] > 255) rgb[2] = 255;
			
		sprintf(str,"\33[48;2;%d;%d;%dm%s\33[0m",rgb[0],rgb[1],rgb[2],c);
		setCharAt(x_pixel, y_pixel, str);
	} else {
		setCharAt(x_pixel, y_pixel, c);
	}

}

void draw_point(float x_proj, float y_proj, int width, int height, const char* c) {
	draw_point_colored(x_proj, y_proj, width, height, c, NULL);
}

int is_inside(int px, int py, float tri[3][4])
{
    float x0 = tri[0][0], y0 = tri[0][1];
    float x1 = tri[1][0], y1 = tri[1][1];
    float x2 = tri[2][0], y2 = tri[2][1];

    float w0 = (x1 - x0)*(py - y0) - (y1 - y0)*(px - x0);
    float w1 = (x2 - x1)*(py - y1) - (y2 - y1)*(px - x1);
    float w2 = (x0 - x2)*(py - y2) - (y0 - y2)*(px - x2);

    // Check if all weights have the same sign (triangle is counter-clockwise)
    return (w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0);
}

float edge_function(float x0, float y0, float x1, float y1, float x, float y) {
    return (y0 - y1)*x + (x1 - x0)*y + x0*y1 - x1*y0;
}
// Calculates the depth of a pixel using barycentric coordinates
float pixel_depth(float x, float y, float tri[3][4]) {
	float area = edge_function(tri[0][0], tri[0][1], tri[1][0], tri[1][1], tri[2][0], tri[2][1]);

	float alpha = edge_function(tri[1][0], tri[1][1], tri[2][0], tri[2][1], x, y) / area;
	float beta  = edge_function(tri[2][0], tri[2][1], tri[0][0], tri[0][1], x, y) / area;
	float gamma = edge_function(tri[0][0], tri[0][1], tri[1][0], tri[1][1], x, y) / area;

	float z = alpha*tri[0][2] + beta*tri[1][2] + gamma*tri[2][2];
	return z;
}

void to_screen(float x[4], int width, int height) {
	x[0] = (int)((x[0] + 1.0f) * 0.5f * (width - 1));
	x[1] = (int)((1.0f -(x[1] + 1.0f) * 0.5f) * (height - 1));
}

void normalize(float *x, float *y, float *z) {
    float length = sqrt((*x)*(*x) + (*y)*(*y) + (*z)*(*z));
    if(length == 0) return; // avoid division by zero
    *x /= length;
    *y /= length;
    *z /= length;
}

double max (double a, double b) {
	return a > b ? a : b;
}

void to_view(float *c, float *v, Camera *camera) {
	vector_subtract(c, v, camera->camera_pos,3);
}

void render_triangle(Renderer *renderer, Triangle *triangle) {

	float tri[3][4];
	
	
	to_view(triangle->v0, triangle->v0, &renderer->camera);
	to_view(triangle->v1, triangle->v1, &renderer->camera);
	to_view(triangle->v2, triangle->v2, &renderer->camera);
	
	
	float p[4][4] = {
		{renderer->camera.f/(renderer->width/renderer->height), 0, 0,0},
		{0, renderer->camera.f, 0,0},
		{0, (renderer->camera.zfar+renderer->camera.znear)/(renderer->camera.znear-renderer->camera.zfar), (2*renderer->camera.zfar*renderer->camera.znear)/(renderer->camera.znear-renderer->camera.zfar),0},
		{0, 0,-1,0},
	};
	
	vector_4x4_4(tri[0], p, triangle->v0);
	vector_4x4_4(tri[1], p, triangle->v1);
	vector_4x4_4(tri[2], p, triangle->v2);

	for(int i=0; i<3; i++) {
    tri[i][0] /= tri[i][3];
    tri[i][1] /= tri[i][3];
    tri[i][2] /= tri[i][3];
    tri[i][3] = 1.0f;

		float z_ndc = tri[i][2]; // after perspective divide
		float z_screen = (z_ndc + 1.0f) * 0.5f; // now in [0,1]
		tri[i][2] = z_screen;

	}
	
	to_screen(tri[0],renderer->width, renderer->height);
	to_screen(tri[1],renderer->width, renderer->height);
	to_screen(tri[2],renderer->width, renderer->height);

	int minX = floor(fminf(fminf(tri[0][0], tri[1][0]), tri[2][0]));
	int maxX = ceil(fmaxf(fmaxf(tri[0][0], tri[1][0]), tri[2][0]));
	int minY = floor(fminf(fminf(tri[0][1], tri[1][1]), tri[2][1]));
	int maxY = ceil(fmaxf(fmaxf(tri[0][1], tri[1][1]), tri[2][1]));

	minX = fmaxf(minX, 0);
	maxX = fminf(maxX, renderer->width - 1);
	minY = fmaxf(minY, 0);
	maxY = fminf(maxY, renderer->height - 1);

	
	for(int y = minY; y <= maxY; y++){
    for(int x = minX; x <= maxX; x++){
			if(is_inside(x, y, tri)){
				float z = pixel_depth(x, y, tri);
				if(z < renderer->depth_buffer[y][x]){
					renderer->depth_buffer[y][x] = z;

					/* float nx = (triangle->v1[1] - triangle->v0[1])*(triangle->v2[2] - triangle->v0[2]) - (triangle->v1[2] - triangle->v0[2])*(triangle->v2[1] - triangle->v0[1]); */
					/* float ny = (triangle->v1[2] - triangle->v0[2])*(triangle->v2[0] - triangle->v0[0]) - (triangle->v1[0] - triangle->v0[0])*(triangle->v2[2] - triangle->v0[2]); */
					/* float nz = (triangle->v1[0] - triangle->v0[0])*(triangle->v2[1] - triangle->v0[1]) - (triangle->v1[1] - triangle->v0[1])*(triangle->v2[0] - triangle->v0[0]); */
					/* normalize(&nx, &ny, &nz); */

					/* float light_dir[3] = {0, 0, -1}; // light coming from camera */
					/* float intensity = max(0.0f, nx*light_dir[0] + ny*light_dir[1] + nz*light_dir[2]); */
					/* int color_val = (int)(intensity * 255); */
					/* int color[3] = {color_val, 244, color_val}; */


					int color[3];
					color[0] =(int)((1.0f - z) * 255);
					color[1] = (int)((1.0f - z) * 255);
					color[2] =(int)((1.0f - z) * 255);
					
					draw_point_colored(x, y, renderer->width, renderer->height, " ", color);
				}
			}
    }
	}

	/* draw_point(tri[0][0], tri[0][1], renderer->width,renderer->height,"*"); */
	/* draw_point(tri[1][0], tri[1][1], renderer->width,renderer->height,"*"); */
	/* draw_point(tri[2][0], tri[2][1], renderer->width,renderer->height,"*"); */
	
}
