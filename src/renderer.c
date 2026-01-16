#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"
#include "vector.h"

void set_fov(Camera* camera, float deg) {
	camera->f = 1/(tan(deg*(M_PI/180)));
}


double clamp(double d, double min, double max) {
  const double t = d < min ? min : d;
  return t > max ? max : t;
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

void to_view(Triangle *triangle, Camera *camera) {
	vector_subtract(&triangle->v0, &triangle->v0,&camera->pos);
	vector_subtract(&triangle->v1, &triangle->v1,&camera->pos);
	vector_subtract(&triangle->v2, &triangle->v2,&camera->pos);
}

void mat4_identity(float m[4][4]) {
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			m[i][j] = (i==j) ? 1.0f : 0.0f;
}

void mat4_mul(float out[4][4], float a[4][4], float b[4][4]) {
	float tmp[4][4];
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++) {
			tmp[i][j] = 0;
			for(int k=0;k<4;k++)
				tmp[i][j] += a[i][k] * b[k][j];
		}
	// copy tmp to out
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++)
			out[i][j] = tmp[i][j];
}

void transform_matrix(float matrix[4][4], Transform t) {
	float sx = t.scale.x, sy = t.scale.y, sz = t.scale.z;
	float cx = cosf(t.rot.x), sxr = sinf(t.rot.x);
	float cy = cosf(t.rot.y), syr = sinf(t.rot.y);
	float cz = cosf(t.rot.z), szr = sinf(t.rot.z);

	// Scale matrix
	float S[4][4]; mat4_identity(S);
	S[0][0] = sx; S[1][1] = sy; S[2][2] = sz;

	// Rotation X
	float Rx[4][4]; mat4_identity(Rx);
	Rx[1][1] = cx; Rx[1][2] = -sxr;
	Rx[2][1] = sxr; Rx[2][2] = cx;

	// Rotation Y
	float Ry[4][4]; mat4_identity(Ry);
	Ry[0][0] = cy; Ry[0][2] = syr;
	Ry[2][0] = -syr; Ry[2][2] = cy;

	// Rotation Z
	float Rz[4][4]; mat4_identity(Rz);
	Rz[0][0] = cz; Rz[0][1] = -szr;
	Rz[1][0] = szr; Rz[1][1] = cz;

	// Combine rotations R = Rz * Ry * Rx
	float R[4][4]; mat4_identity(R);
	mat4_mul(R, Rz, Ry);
	mat4_mul(R, R, Rx);

	mat4_mul(R, R, S);
	
	// Translation matrix
	float T[4][4]; mat4_identity(T);
	T[0][3] = t.pos.x; T[1][3] = t.pos.y; T[2][3] = t.pos.z;


	// Final matrix M = T * R * S
	mat4_mul(matrix, T, R);

}


void render_triangle(Canvas *canvas, Renderer *renderer, Triangle triangle) {

	float tri[3][4];
	to_view(&triangle, &renderer->camera);
	
	// perspective matrix
	float p[4][4] = {
		{renderer->camera.f/(renderer->width/renderer->height), 0, 0,0},
		{0, renderer->camera.f, 0,0},
		{0, (renderer->camera.zfar+renderer->camera.znear)/(renderer->camera.znear-renderer->camera.zfar), (2*renderer->camera.zfar*renderer->camera.znear)/(renderer->camera.znear-renderer->camera.zfar),0},
		{0, 0,-1,0},
	};

	// apply the matrix on the vertexes
	vector_4x4_4(tri[0], p, triangle.v0.data);
	vector_4x4_4(tri[1], p, triangle.v1.data);
	vector_4x4_4(tri[2], p, triangle.v2.data);

	// divide by w
	for(int i=0; i<3; i++) {
    tri[i][0] /= tri[i][3];
    tri[i][1] /= tri[i][3];
    tri[i][2] /= tri[i][3];
    tri[i][3] = 1.0f;

		float z_ndc = tri[i][2]; // after perspective divide
		float z_screen = (z_ndc + 1.0f) * 0.5f; // now in [0,1]
		tri[i][2] = z_screen;
	}

	// convert to screen cordinates
	to_screen(tri[0],renderer->width, renderer->height);
	to_screen(tri[1],renderer->width, renderer->height);
	to_screen(tri[2],renderer->width, renderer->height);

	int minX = floor(fminf(fminf(tri[0][0], tri[1][0]), tri[2][0]));
	int maxX = ceil(fmaxf(fmaxf(tri[0][0], tri[1][0]), tri[2][0]));
	int minY = floor(fminf(fminf(tri[0][1], tri[1][1]), tri[2][1]));
	int maxY = ceil(fmaxf(fmaxf(tri[0][1], tri[1][1]), tri[2][1]));

	// bounding box
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

					int color[3];
					color[0] =(int)((1.0f - z) * 255);
					color[1] = (int)((1.0f - z) * 255);
					color[2] =(int)((1.0f - z) * 255);
					
					//					draw_point_colored(x, y, renderer->width, renderer->height, " ", color);
					set_pixel(canvas, x, y, color[0], color[1], color[2]);
				}
			}
    }
	}

	set_pixel(canvas, tri[0][0], tri[0][1], 255,255,255);
	set_pixel(canvas, tri[1][0], tri[1][1], 255,255,255);
	set_pixel(canvas, tri[2][0], tri[2][1], 255,255,255);
}
