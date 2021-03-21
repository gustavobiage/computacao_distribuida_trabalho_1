#ifndef _MEMORY_UTILS
#define _MEMORY_UTILS

	struct mem_range {
		int start, length;
	};

	int intersect(struct mem_range, struct mem_range);

#endif

