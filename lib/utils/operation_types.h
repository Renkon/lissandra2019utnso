#ifndef OPERATION_TYPES_H_
#define OPERATION_TYPES_H_

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
	UNKNOWN,
	KERNEL,
	FILESYSTEM,
	MEMORY
} process_t;

typedef enum {
	STRONG_CONSISTENCY,
	STRONG_HASH_CONSISTENCY,
	EVENTUAL_CONSISTENCY
}__attribute__((packed)) consistency_t;

typedef struct {
	char* table_name;
	uint16_t key;
}__attribute__((packed)) select_input_t;

typedef struct {
	char* table_name;
	uint16_t key;
	char* value;
	long timestamp;
}__attribute__((packed)) insert_input_t;

typedef struct {
	char* table_name;
	consistency_t consistency;
	int partitions;
	long compaction_time;
}__attribute__((packed)) create_input_t;

typedef struct {
	char* table_name; // opcional
}__attribute__((packed)) describe_input_t;

typedef struct {
	char* table_name;
}__attribute__((packed)) drop_input_t;

// journal no requiere struct

typedef struct {
	int memory_number;
	consistency_t consistency;
}__attribute__((packed)) add_input_t;

typedef struct {
	char* path;
}__attribute__((packed)) run_input_t;

char* get_process_name(process_t process);

#endif /* OPERATION_TYPES_H_ */
