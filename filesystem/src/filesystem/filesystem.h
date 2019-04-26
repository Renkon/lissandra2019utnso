#ifndef CREATE_CREATE_UTILS_H_
#define CREATE_CREATE_UTILS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "../lissandra/lissandra.h"
#include "../utils.h"

typedef struct table_metadata {
	consistency_t consistency;
	int partitions;
	long compaction_time;

} Table_metadata;

int create_table_folder(char* table_name);
void create_partitions(int partitions, char* table_name);
char* create_bin_name(int name);
void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name);
Table_metadata *create_metadata(consistency_t consistency, int partitions,long compaction_time);

#endif /* CREATE_CREATE_UTILS_H_ */
