#ifndef CANVAS_H
#define CANVAS_H

#define RENDER

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} Pixel;

typedef struct {
	Pixel *buffer1;
	Pixel *buffer2;
	int length;
	unsigned int width;
	unsigned int height;
	Pixel bg;
	int force_rerender;
} Canvas;

void setCursorPosition(int x, int y);
void setCharAt(int x, int y, const char *c);

Canvas *new_canvas(unsigned int width, unsigned int height);

void render_text(unsigned int x, unsigned int y, const char*);
void set_text(Canvas* canvas, unsigned int x, unsigned int y, unsigned int height, const char*);

void set_pixel(Canvas* canvas, unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b);
void render(Canvas *canvas);
void free_canvas(Canvas *canvas);
void clear(Canvas *canvas);

#endif
