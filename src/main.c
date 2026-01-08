#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector.h"
//#include "donut.c"
//#include "cube.c"
#include "renderer.h"
#include "canvas.h"
#include "msc.h"

// from loader.c
int load_obj(float *vertices, unsigned int *indices, int *size, const char* path);

void init_cam(Camera *camera){
	
	camera->camera_pos.x = 0;
	camera->camera_pos.y = 0;
	camera->camera_pos.z = 0;

	camera->rot.x = 0;
	camera->rot.y = 0;
	camera->rot.z = 0;

	set_fov(camera, 60);
	
	camera->zfar = 100;
	camera->znear = 0.1f;
	
	camera->camera_up.x = 0;
	camera->camera_up.y = 1;
	camera->camera_up.z = 0;
}

void init_renderer(Renderer *renderer, int width, int height){
	// initilizes a depth buffer for each pixel
	renderer->width = width;
	renderer->height	= height;
	if(renderer->depth_buffer != NULL)
	{
		for(int y = 0; y < height; y++) {
				if(renderer->depth_buffer[y] != NULL)
					free(renderer->depth_buffer[y]);
		}
		free(renderer->depth_buffer);
	}

	renderer->depth_buffer = malloc(sizeof(float*) * height);
	for(int y = 0; y < height; y++) {
    renderer->depth_buffer[y] = malloc(sizeof(float) * width);
	}
	for(int y = 0; y < renderer->height; y++) {
		for(int x = 0; x < renderer->width; x++) {
			renderer->depth_buffer[y][x] = INFINITY;
		}
	}
}

void check_input(Renderer *renderer, float *deg, int *run);

int main(int args,char **argv) {

	if(args <= 1) printf("No model provided!\n");

	// load obj
	float *vertices = malloc(sizeof(float) * 4024*4);
	unsigned int *indices = malloc(sizeof(unsigned int) * 4024*4);
	int size = 0;
	load_obj(vertices,indices, &size, argv[1]);

	int width = termWidth();
	int height = termHeight();
	
	//init renderer
	Renderer renderer = {0};
  init_renderer(&renderer, width, height);
	init_cam(&renderer.camera);

	// init canvas
	Canvas *canvas = new_canvas(width, height);
	for(int i = 0 ; i < canvas->width * canvas->height; i ++){
		canvas->buffer2[i].r = 0;
		canvas->buffer2[i].g = 0;
		canvas->buffer2[i].b = 0;
	}
	canvas->force_rerender = 1;
	render(canvas);
	canvas->force_rerender = 0;
	
	float deg = 60;
	int run = 1;
	
	while(run){
		setCursorPosition(0, 0);
		init_renderer(&renderer, width, height);
		clear(canvas);
		//		system("clear");

		check_input(&renderer, &deg, &run);
		
		for(int t = 0; t < size; t++) {
			Triangle triangle = {0};

			unsigned int i0 = indices[t*3 + 0];
			unsigned int i1 = indices[t*3 + 1];
			unsigned int i2 = indices[t*3 + 2];

			// Vertex 0
			triangle.v0.x = vertices[i0*3 + 0];
			triangle.v0.y = vertices[i0*3 + 1];
			triangle.v0.z = vertices[i0*3 + 2];

			// Vertex 1
			triangle.v1.x = vertices[i1*3 + 0];
			triangle.v1.y = vertices[i1*3 + 1];
			triangle.v1.z = vertices[i1*3 + 2];

			// Vertex 2
			triangle.v2.x = vertices[i2*3 + 0];
			triangle.v2.y = vertices[i2*3 + 1];
			triangle.v2.z = vertices[i2*3 + 2];

			render_triangle(canvas, &renderer, &triangle);
		}

		render(canvas);
		msleep(16);
	}

	// FREE A BUNCH
	free_canvas(canvas);

	for(int y = 0; y < height; y++) {
    free(renderer.depth_buffer[y]);
	}
	free(renderer.depth_buffer);

	free(vertices);
	free(indices);
	
	return 0;
}


void check_input(Renderer *renderer, float *deg, int *run)
{
	if(kbhit()){
			switch(getchar()){
			case 'a':
				renderer->camera.camera_pos.data[0] -= 1;
				break;
			case 'd':
				renderer->camera.camera_pos.data[0] += 1;
				break;
			case 'w':
				renderer->camera.camera_pos.data[1] += 1;
				break;
			case 's':
				renderer->camera.camera_pos.data[1] -= 1;
				break;
			case '+':
			case '=':
				renderer->camera.camera_pos.data[2] -= 1;
				break;
			case '-':
				renderer->camera.camera_pos.data[2] += 1;
				break;
			case 'f':
				(*deg) += 1;
				set_fov(&renderer->camera, (*deg));
				break;
			case 'F':
				(*deg) -= 1;
				set_fov(&renderer->camera, (*deg));
				break;
			case '8':
				renderer->camera.rot.data[0] += 1;
				break;
			case '2':
				renderer->camera.rot.data[0] -= 1;
				break;
			case '4':
				renderer->camera.rot.data[1] += 1;
				break;
			case '6':
				renderer->camera.rot.data[1] -= 1;
				break;
			case 'q':
				(*run) = 0;
				break;
			}
		}
}
