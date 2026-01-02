#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "donut.c"
//#include "cube.c"


int WIDTH = 40;
int HEIGHT = 40;

int color[3] = {255,255,255};

//float camera[3] = {50,15,70};
float camera[3] = {0,0,-30};


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

		color[0] = 255;
		color[1] = 255;
		color[2] = 255;

		
		color[0] -= b[2]*10;
		color[1] -= b[2]*10;
		color[2] -= b[2]*10;
}


void draw_point_colored(float x_proj, float y_proj, int width, int height, const char* c, int* rgb) {
    int scale = 1;
    int x_pixel = (int)(x_proj * scale + width / 2);
    int y_pixel = (int)(y_proj * scale + height / 2);

    y_pixel = height - 1 - y_pixel; // flip Y for top-left origin

		if(rgb != NULL){
			char str[64];
			sprintf(str,"\33[48;2;%d;%d;%dm%s",rgb[0],rgb[1],rgb[2],c);
			setCharAt(x_pixel, y_pixel, str);
		} else {
			setCharAt(x_pixel, y_pixel, c);
		}

}

void draw_point(float x_proj, float y_proj, int width, int height, const char* c) {
	draw_point_colored(x_proj, y_proj, width, height, c, NULL);
}

void draw_line(int x0, int y0, int x1, int y1) {

	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);

	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;

	int err = dx - dy;

	while (1) {
		draw_point_colored(x0, y0, WIDTH, HEIGHT, " ", color);
		if (x0 == x1 && y0 == y1) break;
		int e2 = 2*err;
		if (e2 > -dy) { err -= dy; x0 += sx; }
		if (e2 < dx)  { err += dx; y0 += sy; }
	}
}


void render_triangle(float *vertex, int *indices) {

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
		
		/* draw_point(ap[0],ap[1],WIDTH,HEIGHT,"*"); */
		/* draw_point(bp[0],bp[1],WIDTH,HEIGHT,"*"); */
		draw_line(ap[0], ap[1], bp[0], bp[1]);
	}

}

int main() {

	printf("\033[2J");
	draw_point(0, 0, WIDTH, HEIGHT, "@");
	
	for(int t = 0; t < sizeof(indices)/sizeof(indices[0])/3; t++) {
			render_triangle(vertices, &indices[t*3]);
	}
	setCursorPosition(0, HEIGHT);
	return 0;
}
