#ifndef TKVS_UTILS_H_
#define TKVS_UTILS_H_

#include <commons/collections/list.h>
#include "filesystem/filesystem.h"

typedef struct {
	int	partition;
	t_list*	tkvs;

}tkvs_per_partition_t;

int necessary_blocks_for_tkvs(t_list* tkvs);
int size_of_all_tkvs(t_list* tkvs);
int blocks_needed_for_memtable();
int size_of_all_tkvs_from_table();
t_list* create_partition_tkv_list(char* table_directory,table_metadata_t* table_metadata);
void free_tkvs_per_partition_list(t_list* list);
void free_tkvs_per_partition(tkvs_per_partition_t* tkvs);
t_list*  create_tkv_list(partition_t* partition);


#endif /* TKVS_UTILS_H_ */
