#ifndef CANVAS_H
#define CANVAS_H

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
} Canvas;

void setCursorPosition(int x, int y);
void setCharAt(int x, int y, const char *c);

Canvas *new_canvas(unsigned int width, unsigned int height);
void set_pixel(Canvas* canvas, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void render(Canvas *canvas);
void free_canvas(Canvas *canvas);
void clear(Canvas *canvas);

#endif
