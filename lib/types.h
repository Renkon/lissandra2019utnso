#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

typedef enum {
	INVALID,
	SELECT,
	INSERT,
	CREATE,
	DESCRIBE,
	DROP, // hasta aca puede el fs
	JOURNAL, // hasta aca puede la memoria
	ADD,
	RUN // y hasta aca puede el kernel
} operation_t;

typedef enum {
	KERNEL,
	FILESYSTEM,
	MEMORY
} process_t;

typedef enum {
	STRONG_CONSISTENCY,
	STRONG_HASH_CONSISTENCY,
	EVENTUAL_CONSISTENCY
} consistency_t;

typedef struct {
	char* table_name;
	uint16_t key;
} select_input_t;

typedef struct {
	char* table_name;
	uint16_t key;
	char* value;
	long timestamp;
} insert_input_t;

typedef struct {
	char* table_name;
	consistency_t consistency;
	int partitions;
	long compaction_time;
} create_input_t;

typedef struct {
	char* table_name; // opcional
} describe_input_t;

typedef struct {
	char* table_name;
} drop_input_t;

// journal no requiere struct

typedef struct {
	int memory_number;
	consistency_t consistency;
} add_input_t;

typedef struct {
	char* path;
} run_input_t;

#endif /* TYPES_H_ */
