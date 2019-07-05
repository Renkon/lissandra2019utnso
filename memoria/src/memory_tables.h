
#ifndef MEMORY_TABLES_H_
#define MEMORY_TABLES_H_

#include "memoria.h"
#include "memory_types.h"

#include <limits.h>
#include <commons/bitarray.h>
#include "core/operations.h"
#include "utils/operation_types.h"
#include "generic_client.h"



page_t* create_page(int index, bool modified );
segment_t* create_segment(char* table_name);
segment_t* get_segment_by_name(t_list* list, char* table_name);
page_t* get_page_by_key(segment_t* segment, t_list* index_list, int key);
int page_get_index(page_t* page);
page_t* get_page_by_index(segment_t* segment,int index);
t_list* get_pages_by_modified(bool modified);
void remove_segment(segment_t* segment);
void destroy_page(page_t* page);

#endif /* MEMORY_TABLES_H_ */
