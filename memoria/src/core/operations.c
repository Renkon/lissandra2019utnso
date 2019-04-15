#include "operations.h"

void process_select(select_input_t* input) {
	log_i("mm select args: %s %u", input->table_name, (unsigned int)input->key);
	char* demo_str = string_duplicate("soy una memoria");
	do_simple_request(MEMORY, g_config.filesystem_ip, g_config.filesystem_port, SELECT_IN, demo_str, 16, select_callback);
}

void process_insert(insert_input_t* input) {
	log_i("mm insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
}

void process_create(create_input_t* input) {
	log_i("mm create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
}

void process_describe(describe_input_t* input) {
	log_i("mm describe args: %s", input->table_name);
}

void process_drop(drop_input_t* input) {
	log_i("mm drop args: %s", input->table_name);
}

void process_journal() {
	log_i("mm journal args none");
}

void select_callback(void* response) {
	log_i("Recibi respuesta del servidor: %s", (char*) response);
}
