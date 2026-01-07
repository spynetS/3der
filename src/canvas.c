#include "canvas.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

int msleep(long msec);

void setCursorPosition(int x, int y) {
	printf("\033[%d;%dH", y+1, x+1);
}

void setCharAt(int x, int y, const char *c) {
	setCursorPosition(x, y);
	printf("%s",c);
}


Canvas *new_canvas(unsigned int width, unsigned int height) {
    Canvas *canvas = malloc(sizeof(Canvas));
    if (!canvas) return NULL;

    canvas->width = width;
    canvas->height = height;
    canvas->length = (size_t)width * (size_t)height;

    canvas->buffer1 = malloc(sizeof(Pixel) * canvas->length);
    canvas->buffer2 = malloc(sizeof(Pixel) * canvas->length);
    if (!canvas->buffer1 || !canvas->buffer2) {
        free(canvas->buffer1);
        free(canvas->buffer2);
        free(canvas);
        return NULL;
    }

    for (size_t i = 0; i < canvas->length; i++) {
        canvas->buffer1[i] = (Pixel){0,0,0};
        canvas->buffer2[i] = (Pixel){0,0,0};
    }

    return canvas;
}

void set_pixel(Canvas* canvas, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    int index = y * canvas->width + x;
    if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) return; // bounds check
    canvas->buffer2[index].r = r;
    canvas->buffer2[index].g = g;
    canvas->buffer2[index].b = b;
}

void print_pixel(Pixel bottom) {
    printf("\x1b[48;2;%d;%d;%dm ",bottom.r, bottom.g, bottom.b);
}


void render(Canvas *canvas) {
	for(int y = 0; y < canvas->height; y ++) {
		for(int x = 0; x < canvas->width; x ++) {
			int index = y * canvas->width + x;
			Pixel pixel  = canvas->buffer1[index];
			Pixel pixel2 = canvas->buffer2[index];

			if(pixel. r != pixel2.r ||
				 pixel. g != pixel2.g ||
				 pixel. b != pixel2.b) {
				setCursorPosition(x,y);
				print_pixel(pixel2);
				/* fflush(stdout); */
				/* msleep(5); */
			}
		}
	}
	printf("\x1b[0m\n"); // reset
	memcpy(canvas->buffer1, canvas->buffer2, sizeof(Pixel) * canvas->width * canvas->height);
}

void clear(Canvas *canvas){
	for (size_t i = 0; i < canvas->length; i++) {
		canvas->buffer2[i].r = 0;
		canvas->buffer2[i].g = 0;
		canvas->buffer2[i].b = 0;
	}
}

void free_canvas(Canvas *canvas) {
	free(canvas->buffer1);
	free(canvas->buffer2);
	free(canvas);
}
