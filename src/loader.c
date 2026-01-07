#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int load_obj(float *vertices, unsigned int *indices, int *size, const char* path) {
	float *_vertices = malloc(sizeof(float) * 4024*4);
	unsigned int *_indices = malloc(sizeof(unsigned int) * 4024*4);

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
