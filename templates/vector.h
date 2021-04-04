#include <stdlib.h>
#include <stdio.h>

#define INITIAL_MAX_SIZE 100
#define MULTIPLIER 4

struct vector {
	int max_size;
	int size;
	void ** vec;
};

void vector_push_back(struct vector*, void* content);

void* vector_get(struct vector*, int index);

void vector_put(struct vector*, int index, void* content);

void vector_resize(struct vector*);

void vector_init(struct vector*);

void vector_destroy(struct vector*);