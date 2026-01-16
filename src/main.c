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
#include "loader.h"


void init_cam(Camera *camera){
	
	camera->pos.x = 0;
	camera->pos.y = 0;
	camera->pos.z = -10;

	camera->rot.x = 0;
	camera->rot.y = 0;
	camera->rot.z = 0;

	set_fov(camera, 45);
	
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

	int width = termWidth();
	int height = termHeight();
	
	//init renderer
	Renderer renderer = {0};
  init_renderer(&renderer, width, height);
	init_cam(&renderer.camera);

	// init canvas
	Canvas *canvas = new_canvas(width, height);
	canvas->bg = (Pixel){0,0,100};
	canvas->force_rerender = 1;
	render(canvas);
	canvas->force_rerender = 0;
	
	float deg = 60;
	int run = 1;
	Object object = {0};
	load_object(&object, argv[1]);

	object.transform.pos.x = 0;
	object.transform.pos.y = 0;
	object.transform.pos.z = -2;

	
	object.transform.rot.x = 90*(M_PI/180);//(M_PI/180);
	object.transform.rot.y = 0;
	object.transform.rot.z = 0;

	object.transform.scale.x = 1;
	object.transform.scale.y = 1;
	object.transform.scale.z = 1;
	
	while(run){
		setCursorPosition(0, 0);
		init_renderer(&renderer, width, height);
		clear(canvas);
		
		check_input(&renderer, &deg, &run);
		
		for(size_t i = 0; i < object.triangle_length; i ++){

			float matrix[4][4];
			transform_matrix(matrix, object.transform);
			
			Triangle triangle = {
				object.triangles[i].v0,
				object.triangles[i].v1,
				object.triangles[i].v2
			};


			Vec3 transformed[3];
			vector_4x4_4((float*)&transformed[0], matrix, (float *)&triangle.v0);
			vector_4x4_4((float *)&transformed[1], matrix,(float *) &triangle.v1);
			vector_4x4_4((float *)&transformed[2], matrix,(float *) &triangle.v2);
			
			triangle.v0 = 	transformed[0];
			triangle.v1 = 	transformed[1];
			triangle.v2 = 	transformed[2];
			
			render_triangle(canvas, &renderer, triangle);
		}

		object.transform.rot.x += 0.02f;
		object.transform.rot.y += 0.02f;
		// object.transform.rot.y += 0.05f; 
		object.transform.rot.z += 0.02f;
		
		render(canvas);
		msleep(16);
	}

	// FREE A BUNCH
	free_canvas(canvas);

	for(int y = 0; y < height; y++) {
    free(renderer.depth_buffer[y]);
	}
	free(renderer.depth_buffer);
	
	return 0;
}


void check_input(Renderer *renderer, float *deg, int *run)
{
	if(kbhit()){
			switch(getchar()){
			case 'a':
				renderer->camera.pos.data[0] -= 0.1f;
				break;
			case 'd':
				renderer->camera.pos.data[0] += 0.1f;
				break;
			case 'w':
				renderer->camera.pos.data[1] += 0.1f;
				break;
			case 's':
				renderer->camera.pos.data[1] -= 0.1f;
				break;
			case '+':
			case '=':
				renderer->camera.pos.data[2] -= 0.1f;
				break;
			case '-':
				renderer->camera.pos.data[2] += 0.1f;
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
				renderer->camera.rot.data[0] += 0.1f;
				break;
			case '2':
				renderer->camera.rot.data[0] -= 0.1f;
				break;
			case '4':
				renderer->camera.rot.data[1] += 0.1f;
				break;
			case '6':
				renderer->camera.rot.data[1] -= 0.1f;
				break;
			case 'q':
				(*run) = 0;
				break;
			}
		}
}
