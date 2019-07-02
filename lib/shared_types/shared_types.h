#ifndef SHARED_TYPES_SHARED_TYPES_H_
#define SHARED_TYPES_SHARED_TYPES_H_

#include "../utils/operation_types.h"

typedef struct {
	char* table_name;
	long long timestamp;
	int key;
	char* value;
} record_t;

typedef struct {
	char* table_name;
	consistency_t consistency;
	int partitions;
	long compaction_time;
} table_metadata_t;

#endif /* SHARED_TYPES_SHARED_TYPES_H_ */
