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
	int x_pixel = x_proj;//+width/2;//(int)((x_proj + 1.0f) * 0.5f * (width - 1));
	int y_pixel = y_proj;//+height/2;//(int)((1.0f - (y_proj + 1.0f) * 0.5f) * (height - 1)); // flip Y


	if(rgb != NULL){
		char str[64];
		if(rgb[0] > 255) rgb[0] = 255;
		if(rgb[1] > 255) rgb[1] = 255;
		if(rgb[2] > 255) rgb[2] = 255;
			
		sprintf(str,"\33[48;2;%d;%d;%dm%s",rgb[0],rgb[1],rgb[2],c);
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

int to_screen_x(float x, int width) {
	return (int)((x + 1.0f) * 0.5f * (width - 1));
}

int to_screen_y(float y, int height) {

	return (int)((1.0f -(y + 1.0f) * 0.5f) * (height - 1)); // flip Y
}

void render_triangle(Renderer *renderer, Triangle *triangle) {

	float f = 1/(tan(60*(M_PI/180)));
	float tri[3][4];

	float zfar = 100;
	float znear = 0.1f;
	
	float p[4][4] = {
		{f/(renderer->width/renderer->height), 0, 0,0},
		{0, f, 0,0},
		{0, (zfar+znear)/(znear-zfar), (2*zfar*znear)/(znear-zfar),0},
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
	
	
	tri[0][0] = to_screen_x(tri[0][0], renderer->width);
	tri[0][1] = to_screen_y(tri[0][1], renderer->height);

	tri[1][0] = to_screen_x(tri[1][0], renderer->width);
	tri[1][1] = to_screen_y(tri[1][1], renderer->height);

	tri[2][0] = to_screen_x(tri[2][0], renderer->width);
	tri[2][1] = to_screen_y(tri[2][1], renderer->height);

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
							/* int r = (int)((1.0f - z) * 255); */
							/* int g = (int)((1.0f - z) * 255); */
							/* int b = (int)((1.0f - z) * 255); */

							int r = (int)(z * 255);       // far = red
							int g = 0;
							int b = (int)((1.0f - z) * 255); // near = blue


							int color[3] = {r ,g ,b};
							draw_point_colored(x, y, renderer->width, renderer->height, " ", color);
            }
        }
    }
}


	/* draw_point(tri[0][0], tri[0][1], renderer->width,renderer->height,"*"); */
	/* draw_point(tri[1][0], tri[1][1], renderer->width,renderer->height,"*"); */
	/* draw_point(tri[2][0], tri[2][1], renderer->width,renderer->height,"*"); */
	
}
