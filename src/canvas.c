#include "canvas.h"
#include "msc.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "./font8x8_basic.h"



void setCursorPosition(int x, int y) {
	printf("\033[%d;%dH", y+1, x+1);
}

void setCharAt(int x, int y, const char *c) {
	setCursorPosition(x, y);
	printf("%s",c);
}

Canvas *new_canvas(unsigned int width, unsigned int height) {

	disableEcho();
	hideCursor();
	
	Canvas *canvas = malloc(sizeof(Canvas));
	if (!canvas) return NULL;

	canvas->bg = (Pixel){0,0,0};
	canvas->force_rerender = 0;
	canvas->width = width;
	canvas->height = height;
	canvas->length = (int)width * (int)height;

	canvas->buffer1 = malloc(sizeof(Pixel) * canvas->length);
	canvas->buffer2 = malloc(sizeof(Pixel) * canvas->length);
	if (!canvas->buffer1 || !canvas->buffer2) {
		free(canvas->buffer1);
		free(canvas->buffer2);
		free(canvas);
		return NULL;
	}

	for (int i = 0; i < canvas->length; i++) {
		canvas->buffer1[i] = (Pixel){0,0,0};
		canvas->buffer2[i] = (Pixel){0,0,0};
	}

	return canvas;
}

void set_pixel(Canvas* canvas, unsigned int x, unsigned int y, unsigned char r, unsigned char g, unsigned char b) {
    int index = y * canvas->width + x;
    if (x >= canvas->width || y >= canvas->height) return; // bounds check
    canvas->buffer2[index].r = r;
    canvas->buffer2[index].g = g;
    canvas->buffer2[index].b = b;
}

void print_pixel(Pixel bottom) {
    printf("\x1b[48;2;%d;%d;%dm ",bottom.r, bottom.g, bottom.b);
}

void render_text(unsigned int x, unsigned int y, const char* str){
	setCharAt(x, y, str);
}

void set_text(Canvas* canvas, unsigned int x, unsigned int y, unsigned int height ,const char* str) {
	while(*str){
		setCursorPosition(x, y);
		int local_y = 0;
		for(int i = 0; i < 8; i ++){
			int chari = font8x8_basic[*str][i];
			int local_x = 0;

			for (int i = 0; i <= 7; i++) {
				//        printf("%c", ((chari >> i) & 1) ? '#' : '-');
				if(((chari >> i) & 1)){
					set_pixel(canvas, x+local_x,y+local_y,255,255,255);
					fflush(stdout);
				}
				local_x++;
			}
			local_y++;
		}
		x += 8;
		str++;
		
	}
}


void render(Canvas *canvas) {
	for(unsigned int y = 0; y < canvas->height; y ++) {
		for(unsigned int x = 0; x < canvas->width; x ++) {
			int index = y * canvas->width + x;
			Pixel pixel  = canvas->buffer1[index];
			Pixel pixel2 = canvas->buffer2[index];

			if(canvas->force_rerender || 
				 pixel. r != pixel2.r ||
				 pixel. g != pixel2.g ||
				 pixel. b != pixel2.b) {
#ifdef RENDER				
				setCursorPosition(x,y);
				print_pixel(pixel2);
#endif				
			}
		}
	}
#ifdef RENDER	
	printf("\x1b[0m\n"); // reset
#endif	
	memcpy(canvas->buffer1, canvas->buffer2, sizeof(Pixel) * canvas->width * canvas->height);
	fflush(stdout);
}

void clear(Canvas *canvas){
	for (int i = 0; i < canvas->length; i++) {
		canvas->buffer2[i] = canvas->bg;
	}
	//canvas->force_rerender = 1;
}

void free_canvas(Canvas *canvas) {
	free(canvas->buffer1);
	free(canvas->buffer2);
	free(canvas);
}
