#include "operations.h"

void process_select(select_input_t* input) {
	log_i("kn select args: %s %u", input->table_name, (unsigned int)input->key);
	char* demo_str = string_duplicate("soy un kernel");
	do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, SELECT_IN, demo_str, 14, select_callback);
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

void select_callback(void* response) {
	log_i("Recibi respuesta del servidor: %s", (char*) response);
}
