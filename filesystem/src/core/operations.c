#include "operations.h"

void process_select(select_input_t* input) {
	log_i("fs select args: %s %u", input->table_name, (unsigned int)input->key);
}

void process_insert(insert_input_t* input) {
	log_i("fs insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
}

void process_create(create_input_t* input) {
	log_i("fs create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
}

void process_describe(describe_input_t* input) {
	log_i("fs describe args: %s", input->table_name);
}

void process_drop(drop_input_t* input) {
	log_i("fs drop args: %s", input->table_name);
}
