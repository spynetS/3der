#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"
//#include "donut.c"
//#include "cube.c"
#include "renderer.h"


#define WIDTH  40
#define HEIGHT 40

// from loader.c
int load_obj(float *vertices, unsigned int *indices, int *size, const char* path);

void init_cam(Camera *camera){
	
	camera->f = 50;
	// Position the camera in front of the cube
	camera->camera_pos[0] = 0;
	camera->camera_pos[1] = 3;
	camera->camera_pos[2] = -50; // A negative Z value

	// Point it at the center of the cube
	camera->camera_target[0] = 5;
	camera->camera_target[1] = 5;
	camera->camera_target[2] = 15;
	
	camera->camera_up[0] = 0;
	camera->camera_up[1] = 1;
	camera->camera_up[2] = 0;

}

void init_renderer(Renderer *renderer){
		// initilizes a depth buffer for each pixel
	renderer->depth_buffer = malloc(sizeof(float*) * HEIGHT);
	for(int y = 0; y < HEIGHT; y++) {
    renderer->depth_buffer[y] = malloc(sizeof(float) * WIDTH);
	}
	for(int y = 0; y < renderer->height; y++) {
		for(int x = 0; x < renderer->width; x++) {
			renderer->depth_buffer[y][x] = INFINITY;
		}
	}
	renderer->width = WIDTH;
	renderer->height	= HEIGHT;

}

int main(int args,char **argv) {

	if(args <= 1) printf("No model provided!\n");


	
	float *vertices = malloc(sizeof(float) * 4024*4);
	unsigned int *indices = malloc(sizeof(unsigned int) * 4024*4);

	int size = sizeof(indices)/sizeof(indices[0])/3;
	
	load_obj(vertices,indices, &size, argv[1]);

	
	
	printf("\033[2J");
	draw_point(0, 0, WIDTH, HEIGHT, "@");

	Renderer renderer = {0};
	init_cam(&renderer.camera);
	init_renderer(&renderer);

	for(int t = 0; t < size; t++) {
		render_triangle(&renderer, vertices, &indices[t*3]);
	}
	setCursorPosition(0, HEIGHT);


	for(int y = 0; y < HEIGHT; y++) {
    free(renderer.depth_buffer[y]);
	}
	free(renderer.depth_buffer);
	
	return 0;
}
