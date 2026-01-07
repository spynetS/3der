#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
//#include "donut.c"
//#include "cube.c"
#include "renderer.h"
#include "canvas.h"

#define WIDTH  120
#define HEIGHT 60

// from loader.c
int load_obj(float *vertices, unsigned int *indices, int *size, const char* path);

int msleep(long msec);
int kbhit(void);

void init_cam(Camera *camera){
	
	camera->f = 0.75f;

	camera->camera_pos[0] = 0;
	camera->camera_pos[1] = 0;
	camera->camera_pos[2] = 0;

	camera->f = 1/(tan(60*(M_PI/180)));
	camera->zfar = 100;
	camera->znear = 0.1f;
	
	camera->camera_up[0] = 0;
	camera->camera_up[1] = 1;
	camera->camera_up[2] = 0;
}

void init_renderer(Renderer *renderer){
	// initilizes a depth buffer for each pixel
	renderer->width = WIDTH;
	renderer->height	= HEIGHT;
	
	renderer->depth_buffer = malloc(sizeof(float*) * HEIGHT);
	for(int y = 0; y < HEIGHT; y++) {
    renderer->depth_buffer[y] = malloc(sizeof(float) * WIDTH);
	}
	for(int y = 0; y < renderer->height; y++) {
		for(int x = 0; x < renderer->width; x++) {
			renderer->depth_buffer[y][x] = INFINITY;
		}
	}
}

int main(int args,char **argv) {



	
	if(args <= 1) printf("No model provided!\n");

	float *vertices = malloc(sizeof(float) * 4024*4);
	unsigned int *indices = malloc(sizeof(unsigned int) * 4024*4);

	int size = 0;
	load_obj(vertices,indices, &size, argv[1]);

	//	int size = sizeof(indices)/sizeof(indices[0])/3;
	//	system("clear");
	//	draw_point(0, 0, WIDTH, HEIGHT, "@");

	Renderer renderer = {0};
  init_renderer(&renderer);
	init_cam(&renderer.camera);

	Canvas *canvas = new_canvas(WIDTH, HEIGHT);
		
	while(1){
		setCursorPosition(0, 0);
		init_renderer(&renderer);
		//		system("clear");
		clear(canvas);
		
		for(int t = 0; t < size; t++) {
			Triangle triangle = {0};

			unsigned int i0 = indices[t*3 + 0];
			unsigned int i1 = indices[t*3 + 1];
			unsigned int i2 = indices[t*3 + 2];

			// Vertex 0
			triangle.v0[0] = vertices[i0*3 + 0];
			triangle.v0[1] = vertices[i0*3 + 1];
			triangle.v0[2] = vertices[i0*3 + 2];

			// Vertex 1
			triangle.v1[0] = vertices[i1*3 + 0];
			triangle.v1[1] = vertices[i1*3 + 1];
			triangle.v1[2] = vertices[i1*3 + 2];

			// Vertex 2
			triangle.v2[0] = vertices[i2*3 + 0];
			triangle.v2[1] = vertices[i2*3 + 1];
			triangle.v2[2] = vertices[i2*3 + 2];

			render_triangle(canvas, &renderer, &triangle);
		}

		if(kbhit()){
			switch(getchar()){
			case 'a':
				renderer.camera.camera_pos[0] -= 1;
				break;
			case 'd':
				renderer.camera.camera_pos[0] += 1;
				break;
			case 'w':
				renderer.camera.camera_pos[1] += 1;
				break;
			case 's':
				renderer.camera.camera_pos[1] -= 1;
				break;
			case '+':
			case '=':
				renderer.camera.camera_pos[2] -= 1;
				break;
			case '-':
				renderer.camera.camera_pos[2] += 1;
				break;
			}
		}
		render(canvas);
		msleep(16);
	}

	//	setCursorPosition(0, HEIGHT);

	/* for(int y = 0; y < HEIGHT; y++) { */
  /*   free(renderer.depth_buffer[y]); */
	/* } */
	/* free(renderer.depth_buffer); */

	return 0;
}
