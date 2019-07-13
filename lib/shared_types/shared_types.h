#ifndef SHARED_TYPES_SHARED_TYPES_H_
#define SHARED_TYPES_SHARED_TYPES_H_

#include "../utils/operation_types.h"

typedef struct {
	char* table_name;
	long long timestamp;
	int key;
	char* value;
	char* fs_archive_where_it_was_found;
}__attribute__((packed)) record_t;

typedef struct {
	char* table_name;
	consistency_t consistency;
	int partitions;
	long compaction_time;
}__attribute__((packed)) table_metadata_t;

typedef struct {
	int id;
	char* ip;
	int port;
	bool alive;
	long long timestamp;
}__attribute__((packed)) memory_t;

#endif /* SHARED_TYPES_SHARED_TYPES_H_ */
