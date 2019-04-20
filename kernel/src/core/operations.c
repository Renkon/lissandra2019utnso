#include "operations.h"

void process_select(select_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn select args: %s %u", input->table_name, (unsigned int)input->key);
	statement->operation = SELECT;
	statement->select_input = malloc(sizeof(select_input_t));
	memcpy(statement->select_input, input, sizeof(select_input_t));

	//char* demo_str = string_duplicate("soy un kernel");
	//do_simple_request(KERNEL, g_config.memory_ip, g_config.memory_port, SELECT_IN, demo_str, 14, select_callback);
	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_insert(insert_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn insert args: %s %u \"%s\" %ld", input->table_name, (unsigned int) input->key, input->value, input->timestamp);
	statement->operation = INSERT;
	statement->insert_input = malloc(sizeof(insert_input_t));
	memcpy(statement->insert_input, input, sizeof(insert_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_create(create_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn create args: %s %i %i %ld", input->table_name, input->consistency, input->partitions, input->compaction_time);
	statement->operation = CREATE;
	statement->create_input = malloc(sizeof(create_input_t));
	memcpy(statement->create_input, input, sizeof(create_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_describe(describe_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn describe args: %s", input->table_name);
	statement->operation = DESCRIBE;
	statement->describe_input = malloc(sizeof(describe_input_t));
	memcpy(statement->describe_input, input, sizeof(describe_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_drop(drop_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn drop args: %s", input->table_name);
	statement->operation = DROP;
	statement->drop_input = malloc(sizeof(drop_input_t));
	memcpy(statement->drop_input, input, sizeof(drop_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_journal() {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn journal args none");
	statement->operation = JOURNAL;

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_add(add_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn add %i %i", input->memory_number, input->consistency);
	statement->operation = ADD;
	statement->add_input = malloc(sizeof(add_input_t));
	memcpy(statement->add_input, input, sizeof(add_input_t));

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

void process_run(run_input_t* input) {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn run %s", input->path);
}

void process_metrics() {
	pcb_t* pcb = get_new_pcb();
	statement_t* statement = malloc(sizeof(statement_t));

	log_i("kn metrics args none");
	statement->operation = METRICS;

	list_add(pcb->statements, statement);
	list_add(g_scheduler_queues.new, pcb);
}

/*void select_callback(void* response) {
	log_i("Recibi respuesta del servidor: %s", (char*) response);
}*/

