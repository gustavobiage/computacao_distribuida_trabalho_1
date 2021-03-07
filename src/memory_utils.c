#include "memory_utils.h"

int intersect(struct mem_range ma, struct mem_range mb) {
	// Caso 1
	//  |-----|-----|-----|
	// mab   mbb   mae   mbe
	// Caso 2
	//  |-----|-----|-----|
	// mbb   mab   mbe   mae
	return ma.begin <= mb.end && ma.end >= mb.begin ||
		mb.begin <= ma.end && mb.end >= ma.begin;
}