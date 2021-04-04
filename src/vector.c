#include "../templates/vector.h"

void vector_push_back(struct vector* vector, void* content) {
	if (vector->max_size == vector->size) {
		vector_resize(vector);
	}
	vector->vec[vector->size++] = content;
}

void* vector_get(struct vector* vector, int index) {
	if (index < 0 || index >= vector->max_size) {
		return NULL;
	}
	return vector->vec[index];
}

void vector_put(struct vector* vector, int index, void* content) {
	if (index < 0 || index >= vector->max_size) {
		return;
	}
	vector->vec[index] = content;
}

void vector_resize(struct vector* vector) {
	vector->max_size *= MULTIPLIER;
	vector->vec = realloc(vector->vec, sizeof(void*)*vector->max_size);
}

void vector_init(struct vector* vector) {
	vector->max_size = INITIAL_MAX_SIZE;
	vector->size = 0;
	vector->vec = (void**) malloc(sizeof(void*)*vector->max_size);
}

void vector_destroy(struct vector* vector) {
	free(vector->vec);
}