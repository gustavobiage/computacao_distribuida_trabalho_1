#include "../templates/linked_list.h"
#include <stdio.h>

void push_front(struct linked_list* list, void * data) {
	struct node* node = (struct node*) malloc(sizeof(struct node));
	node->data = data;
	if (!list->size) {
		list->head = list->tail = node;
	} else {
		node->next = list->head;
		list->head->prev = node;
		list->head = node;
	}
	list->size++;
}

void push_back(struct linked_list* list, void * data) {
	struct node* node = (struct node*) malloc(sizeof(struct node));
	node->data = data;
	if (!list->size) {
		list->head = list->tail = node;
	} else {
		node->prev = list->tail;
		list->tail->next = node;
		list->tail = node;
	}
	list->size++;
}

void list_init(struct linked_list* list) {
	list->size = 0;
	list->head = list->tail = NULL;
}

void list_destroy(struct linked_list* list) {
	struct node * node = list->head;
	struct node * aux;
	while (node != NULL) {
		aux = node->next;
		free(node);
		node = aux;
	}
}

struct iterator* iterator(struct linked_list* list) {
	return (struct iterator*) list->head;
}

void* next(struct iterator** it) {
	void* data = (*it)->data;
	*it = (*it)->next;
	return data;
}

int has_next(struct iterator* it) {
	return it != NULL;
}