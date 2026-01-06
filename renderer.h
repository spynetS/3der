#ifndef RENDERER_H
#define RENDERER_H

typedef struct {
	float camera_pos[3];
	float camera_target[3];
	float camera_up[3];
	float f;
} Camera;


typedef struct {
	float **depth_buffer; // x y index for depth 
	int width;
	int height;
	Camera camera;
} Renderer;


void setCursorPosition(int x, int y);
void setCharAt(int x, int y, const char *c);
void draw_point_colored(float x_proj, float y_proj, int width, int height, char* c, int* rgb);
void draw_point(float x_proj, float y_proj, int width, int height, const char* c);
void project_vertex(float *out, float *vertex, float *camera, float f);
void project_line(float *ap, float *bp, float *a, float *b, float *camera, float f);
void draw_line(int x0, int y0, int x1, int y1);
void render_triangle(Renderer *renderer, float* vertices, unsigned int *indices);
	
#endif
