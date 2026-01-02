#include <stdio.h>
#include <stdlib.h>
#include "vector.h"


int WIDTH = 40;
int HEIGHT = 40;
float camera[3] = {0,0,100};

// Cube vertices (x, y, z) all positive, larger scale
float vertices[] = {
    // Front face
     0.0f,  0.0f,  10.0f,  // 0
    10.0f,  0.0f,  10.0f,  // 1
    10.0f, 10.0f,  10.0f,  // 2
     0.0f, 10.0f,  10.0f,  // 3
    // Back face
     0.0f,  0.0f,  20.0f,  // 4
    10.0f,  0.0f,  20.0f,  // 5
    10.0f, 10.0f,  20.0f,  // 6
     0.0f, 10.0f,  20.0f   // 7
};

// Indices for the 12 triangles (2 per face)
unsigned int indices[] = {
    // Front
    0, 1, 2,
    2, 3, 0,
    // Right
    1, 5, 6,
    6, 2, 1,
    // Back
    5, 4, 7,
    7, 6, 5,
    // Left
    4, 0, 3,
    3, 7, 4,
    // Top
    3, 2, 6,
    6, 7, 3,
    // Bottom
    4, 5, 1,
    1, 0, 4
};

void setCursorPosition(int x, int y) {
    printf("\033[%d;%dH", y+1, x+1);
}

void setCharAt(int x, int y, char *c) {
    setCursorPosition(x, y);
    printf("%s",c);
}



void project_vertex(float *out, float *vertex, float *camera, float f) {
    float cam[3];
    vector_subtract(cam, vertex, camera, 3);  // cam = vertex - camera

    out[0] = (cam[0] / cam[2]) * f;          // x_screen
    out[1] = (cam[1] / cam[2]) * f;          // y_screen
}

void project_line(float *ap, float *bp, float *a, float *b, float *camera, float f) {
    project_vertex(ap, a, camera, f);
    project_vertex(bp, b, camera, f);
}


void draw_point(float x_proj, float y_proj, int width, int height, const char* c) {
    int scale = 1;
    int x_pixel = (int)(x_proj * scale + width / 2);
    int y_pixel = (int)(y_proj * scale + height / 2);

    y_pixel = height - 1 - y_pixel; // flip Y for top-left origin

    setCharAt(x_pixel, y_pixel, c);
}


void render_triangle(float vertex[9], int indices[3]) {

	for(int  i = 0; i < 3; i ++) {
		//printf("vector between %d and %d\n", i, (i+1) % 3);
		int vi = i;
		int vii = (i+1) % 3;
		
		float a[3];
		a[0] = vertex[indices[vi]*3 + 0];
		a[1] = vertex[indices[vi]*3 + 1];
		a[2] = vertex[indices[vi]*3 + 2];

		float b[3];
		b[0] = vertex[indices[vii]*3 + 0];
		b[1] = vertex[indices[vii]*3 + 1];
		b[2] = vertex[indices[vii]*3 + 2];
		

		float ap[2];
		float bp[2];
		project_line(ap,bp,a,b, camera, 50);
		
		draw_point(ap[0],ap[1],WIDTH,HEIGHT,"*");
		draw_point(bp[0],bp[1],WIDTH,HEIGHT,"*");
	}

}

int main() {

	printf("\033[2J");
	draw_point(0, 0, WIDTH, HEIGHT, "@");

	for(int t = 0; t < 12; t++) {
		render_triangle(vertices, &indices[t*3]);
	}
	setCursorPosition(0, HEIGHT);
	return 0;
}
