#include <stdlib.h>

struct linked_list {
	struct node * head;
	struct node * tail;
	int size;
};

struct iterator {
	struct iterator* next;
	void * data;
};

struct node {
	struct node* next;
	void * data;
	struct node* prev;
};

void push_front(struct linked_list*, void*);

void push_back(struct linked_list*, void*);

void list_init(struct linked_list*);

void list_destroy(struct linked_list*);

struct iterator* iterator(struct linked_list*);

void* next(struct iterator**);

int has_next(struct iterator*);