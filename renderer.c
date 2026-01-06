#include <stdio.h>
#include <math.h>
#include <stdlib.h>
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
    int x_pixel = (int)(x_proj * scale + width / 2);
    int y_pixel = (int)(y_proj * scale + height / 2);

    y_pixel = height - 1 - y_pixel; // flip Y for top-left origin

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

int inside_triangle(float px, float py, float tri[3][3]) {
    float x0 = tri[0][0], y0 = tri[0][1];
    float x1 = tri[1][0], y1 = tri[1][1];
    float x2 = tri[2][0], y2 = tri[2][1];

    float e0 = (px - x0) * (y1 - y0) - (py - y0) * (x1 - x0);
    float e1 = (px - x1) * (y2 - y1) - (py - y1) * (x2 - x1);
    float e2 = (px - x2) * (y0 - y2) - (py - y2) * (x0 - x2);

    return (e0 >= 0 && e1 >= 0 && e2 >= 0) || (e0 <= 0 && e1 <= 0 && e2 <= 0);
}

// Calculates the depth of a pixel using barycentric coordinates
float pixel_depth(float px, float py, float tri[3][3]) {
    float x0 = tri[0][0], y0 = tri[0][1], z0 = tri[0][2];
    float x1 = tri[1][0], y1 = tri[1][1], z1 = tri[1][2];
    float x2 = tri[2][0], y2 = tri[2][1], z2 = tri[2][2];

    float area = (x1 - x0)*(y2 - y0) - (y1 - y0)*(x2 - x0);
    if (fabs(area) < 1e-5) return INFINITY; // Avoid division by zero for degenerate triangles

    // Barycentric coordinates
    float w0 = ((x1 - px)*(y2 - py) - (y1 - py)*(x2 - px)) / area;
    float w1 = ((x2 - px)*(y0 - py) - (y2 - py)*(x0 - px)) / area;
    float w2 = 1.0f - w0 - w1;

    // Interpolate depth
    return w0*z0 + w1*z1 + w2*z2;
}

void render_triangle(Renderer *renderer, float* vertices, unsigned int *indices) {
	//TODO make bounding box insteaf of whole screen

	float zaxis[3], xaxis[3], yaxis[3];

	float pos_tar[3]; // cameraPos - cameraTarget
	vector_subtract(pos_tar, renderer->camera.camera_pos, renderer->camera.camera_target, 3);
	vector_normalize(zaxis,pos_tar, 3);

	float up_zaxis[3]; // cross(up,zaxis)
	vector_cross(up_zaxis, renderer->camera.camera_up, zaxis, 3);
	vector_normalize(xaxis,up_zaxis, 3);

	vector_cross(yaxis, zaxis, xaxis,3);

	float v0_world[3] = { vertices[indices[0]*3+0], vertices[indices[0]*3+1], vertices[indices[0]*3+2] };
	float v1_world[3] = { vertices[indices[1]*3+0], vertices[indices[1]*3+1], vertices[indices[1]*3+2] };
	float v2_world[3] = { vertices[indices[2]*3+0], vertices[indices[2]*3+1], vertices[indices[2]*3+2] };

	float tri_projected[3][3]; // Store projected (x, y, z_cam) for each vertex

	    float *v_list[3] = {v0_world, v1_world, v2_world};

    for (int i = 0; i < 3; i++) {
        float v_view[3];
        vector_subtract(v_view, v_list[i], renderer->camera.camera_pos, 3);

        float v_cam[3];
        v_cam[0] = vector_dot(v_view, xaxis, 3);
        v_cam[1] = vector_dot(v_view, yaxis, 3);
        v_cam[2] = vector_dot(v_view, zaxis, 3); // z is distance in camera space

        // Simple perspective projection
        tri_projected[i][0] = (v_cam[0] / v_cam[2]) * renderer->camera.f; // x_screen
        tri_projected[i][1] = (v_cam[1] / v_cam[2]) * renderer->camera.f; // y_screen
        tri_projected[i][2] = v_cam[2]; // Store z for depth buffering
    }

	
	for(int y = 0; y < renderer->height; y++) {
		for(int x = 0; x < renderer->width; x++) {

			// Convert pixel coordinate to screen coordinate (center is 0,0)
			/* float x_screen = (float)x - renderer->width / 2.0f; */
			/* float y_screen = (float)y - renderer->height / 2.0f; */
			float x_screen = (float)x - renderer->width / 2.0f;
			float y_screen = (renderer->height / 2.0f) - (float)y; // Flipped Y

			
			if(inside_triangle(x_screen,y, tri_projected)) {
				float z = pixel_depth(x_screen, y_screen, tri_projected);
				if(z < renderer->depth_buffer[y][x]){
					float z_min = -30.0f;             // nearest depth you care about
					float z_max = 50.0f;            // farthest depth you care about

					// normalize to 0..1
					float t = (z - z_min) / (z_max - z_min);

					if(t < 0) t = 0;
					if(t > 1) t = 1;

					int brightness = (int)((1.0f - t) * 255); // closer = brighter
					

					
					int color[3] = {brightness, brightness, brightness};
					draw_point_colored(x, y, renderer->width, renderer->height, "*",color);
					renderer->depth_buffer[y][x] = z;
				}
			}
		}
	}
}
