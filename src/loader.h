#ifndef LOADER_H
#define LOADER_H

int load_object(Object *object, const char* path);
int load_obj(float *vertices, unsigned int *indices, int *size, const char* path);

#endif
