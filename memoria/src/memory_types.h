#ifndef MEMORY_TYPES_H_
#define MEMORY_TYPES_H_

#include <stdbool.h>
#include "commons/collections/list.h"

typedef struct {
	int index;
	bool modified;
} page_t;

typedef struct {
	char* name;
	t_list* page;
} segment_t;

#endif /* MEMORY_TYPES_H_ */
