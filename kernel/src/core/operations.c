#include "operations.h"

// TODO: eventualmente hay que hacer free de todos los input
void process_select(select_input_t* input) {
	log_i("kn select args: %s %u", input->table_name, (unsigned int)input->key);
}

void process_insert(insert_input_t* input) {
	log_i("kn insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
}

void process_create(create_input_t* input) {
	log_i("kn create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
}

void process_describe(describe_input_t* input) {
	log_i("kn describe args: %s", input->table_name);
}

void process_drop(drop_input_t* input) {
	log_i("kn drop args: %s", input->table_name);
}

void process_journal() {
	log_i("kn journal args none");
}

void process_add(add_input_t* input) {
	log_i("kn add %i %i", input->memory_number, input->consistency);
}

void process_run(run_input_t* input) {
	log_i("kn run %s", input->path);
}
