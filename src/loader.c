#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "renderer.h"
#include "loader.h"

int load_object(Object *object, const char* path) {
	float *vertices = malloc(sizeof(float) * 4024*256);
	unsigned int *indices = malloc(sizeof(unsigned int) * 4024*256);
	int size = 0;
	load_obj(vertices,indices, &size, path);

	object->triangles = malloc(sizeof(Triangle) * size);
	
	for(int t = 0; t < size; t++) {
		Triangle triangle = {0};

		unsigned int i0 = indices[t*3 + 0];
		unsigned int i1 = indices[t*3 + 1];
		unsigned int i2 = indices[t*3 + 2];

		// Vertex 0
		triangle.v0.x = vertices[i0*3 + 0];
		triangle.v0.y = vertices[i0*3 + 1];
		triangle.v0.z = vertices[i0*3 + 2];
		triangle.v0.w = 1;

		// Vertex 1
		triangle.v1.x = vertices[i1*3 + 0];
		triangle.v1.y = vertices[i1*3 + 1];
		triangle.v1.z = vertices[i1*3 + 2];
		triangle.v1.w = 1;

		// Vertex 2
		triangle.v2.x = vertices[i2*3 + 0];
		triangle.v2.y = vertices[i2*3 + 1];
		triangle.v2.z = vertices[i2*3 + 2];
		triangle.v2.w = 1;

		object->triangles[t] = triangle;
		object->triangle_length++;
	}

	free(vertices);
	free(indices);
}

int load_obj(float *vertices, unsigned int *indices, int *size, const char* path) {
	float *_vertices = malloc(sizeof(float) * 4024*256);
	unsigned int *_indices = malloc(sizeof(unsigned int) * 4024*256);

	FILE* obj;
	obj = fopen(path,"r");
	if(obj == NULL) exit(1);

	int v_count = 0;
	int i_count = 0;
	
	char line[256]; // buffer to store each line
	while (fgets(line, sizeof(line), obj)) {
		if(line[0] == 'v' && line[1] != 'n'){
			float v1, v2, v3;
			sscanf(line+2,"%f %f %f", &v1, &v2, &v3);
			_vertices[v_count++] = v1;
			_vertices[v_count++] = v2;
			_vertices[v_count++] = v3;
		}

		if(line[0] == 'f'){
			int i1, i2, i3;
			sscanf(line+2,"%d %d %d", &i1, &i2, &i3);
			_indices[i_count++] = i1-1;
			_indices[i_count++] = i2-1;
			_indices[i_count++] = i3-1;
		}
	}


	memcpy(vertices, _vertices, sizeof(float)*v_count);
	memcpy(indices, _indices, sizeof(unsigned int)*i_count);

	free(_indices);
	free(_vertices);

	*size = i_count/3;
	
	fclose(obj);
	return 0;
}
