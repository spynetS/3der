#include <stdio.h>
#include "../src/vector.h"
#include "../src/canvas.h"

int main() {

	Canvas *canvas = new_canvas(100,100);

	
	set_text(canvas, 2,2,10,"Hello World!");
	render(canvas);

	free_canvas(canvas);
	
	return 0;
}
