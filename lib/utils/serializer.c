#include "serializer.h"

elements_network_t elements_select_in_info(void* input) {
	select_input_t* select_input = (select_input_t*) input;
	elements_network_t element_info = init_elements_info(2);
	int* iterator = element_info.elements_size;

	*iterator = strlen(select_input->table_name) + 1;
	iterator++;
	*iterator = sizeof(uint16_t);

	return element_info;
}

elements_network_t elements_select_out_info(void* input) {
	record_t* record = (record_t*) input;
	elements_network_t element_info = init_elements_info(3);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(long long);
	iterator++;
	*iterator = sizeof(int);
	iterator++;
	*iterator = strlen(record->value) + 1;

	return element_info;
}

elements_network_t elements_insert_in_info(void* input) {
	insert_input_t* insert_input = (insert_input_t*) input;
	elements_network_t element_info = init_elements_info(4);
	int* iterator = element_info.elements_size;

	*iterator = strlen(insert_input->table_name) + 1;
	iterator++;
	*iterator = sizeof(uint16_t);
	iterator++;
	*iterator = strlen(insert_input->value) + 1;
	iterator++;
	*iterator = sizeof(long long);

	return element_info;
}

elements_network_t elements_insert_out_info(void* input) {
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(int);

	return element_info;
}

elements_network_t elements_create_in_info(void* input) {
	create_input_t* create_input = (create_input_t*) input;
	elements_network_t element_info = init_elements_info(4);
	int* iterator = element_info.elements_size;

	*iterator = strlen(create_input->table_name) + 1;
	iterator++;
	*iterator = sizeof(consistency_t);
	iterator++;
	*iterator = sizeof(int);
	iterator++;
	*iterator = sizeof(long);

	return element_info;
}

elements_network_t elements_create_out_info(void* input) {
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(int);

	return element_info;
}

elements_network_t elements_describe_in_info(void* input) {
	describe_input_t* describe_input = (describe_input_t*) input;
	elements_network_t element_info;
	if (describe_input->table_name == NULL) {
		element_info = init_elements_info(0);
	} else {
		element_info = init_elements_info(1);
		int* iterator = element_info.elements_size;
		*iterator = strlen(describe_input->table_name) + 1;
	}

	return element_info;
}

elements_network_t elements_describe_out_info(void* input) {
	t_list* describe_list = (t_list*) input;
	elements_network_t element_info = init_elements_info(3 * list_size(describe_list));
	int* iterator = element_info.elements_size;

	for (int i = 0; i < list_size(describe_list); i++) {
		table_metadata_t* list_metadata = (table_metadata_t*) list_get(describe_list, i);
		*iterator = sizeof(consistency_t);
		iterator++;
		*iterator = sizeof(int);
		iterator++;
		*iterator = sizeof(long);
		iterator++;
	}

	return element_info;
}

elements_network_t elements_drop_in_info(void* input) {
	drop_input_t* drop_input = (drop_input_t*) input;
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = strlen(drop_input->table_name) + 1;

	return element_info;
}

elements_network_t elements_drop_out_info(void* input) {
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(int);

	return element_info;
}

elements_network_t elements_journal_in_info(void* input) {
	return init_elements_info(0);
}

elements_network_t elements_journal_out_info(void* input) {
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(int);

	return element_info;
}

elements_network_t init_elements_info(int elements) {
	elements_network_t element_info;
	element_info.elements = elements;

	if (elements > 0)
		element_info.elements_size = malloc(elements * sizeof(int));
	else
		element_info.elements_size = NULL;

	return element_info;
}

void serialize_content(void* to, socket_operation_t operation, void* from) {
	select_input_t* select;
	insert_input_t* insert;
	create_input_t* create;
	describe_input_t* describe;
	drop_input_t* drop;
	char* to_ptr = (char*) to;
	int offset = 0;
	int elem_length;

	switch (operation) {
		case SELECT_IN:
			select = (select_input_t*) from;

			elem_length = strlen(select->table_name) + 1;
			memcpy(to_ptr, select->table_name, elem_length);
			offset += elem_length;

			elem_length = sizeof(uint16_t);
			memcpy(to_ptr + offset, &(select->key), elem_length);
		break;
		case INSERT_IN:
			insert = (insert_input_t*) from;

			elem_length = strlen(insert->table_name) + 1;
			memcpy(to_ptr, insert->table_name, elem_length);
			offset += elem_length;

			elem_length = sizeof(uint16_t);
			memcpy(to_ptr + offset, &(insert->key), elem_length);
			offset += elem_length;

			elem_length = strlen(insert->value) + 1;
			memcpy(to_ptr + offset, insert->value, elem_length);
			offset += elem_length;

			elem_length = sizeof(long long);
			memcpy(to_ptr + offset, &(insert->timestamp), elem_length);
		break;
		case CREATE_IN:
			create = (create_input_t*) from;

			elem_length = strlen(create->table_name) + 1;
			memcpy(to_ptr, create->table_name, elem_length);
			offset += elem_length;

			elem_length = sizeof(consistency_t);
			memcpy(to_ptr + offset, &(create->consistency), elem_length);
			offset += elem_length;

			elem_length = sizeof(int);
			memcpy(to_ptr + offset, &(create->partitions), elem_length);
			offset += elem_length;

			elem_length = sizeof(long);
			memcpy(to_ptr + offset, &(create->compaction_time), elem_length);
		break;
		case DESCRIBE_IN:
			describe = (describe_input_t*) from;

			if (describe->table_name != NULL) {
				elem_length = strlen(describe->table_name) + 1;
				memcpy(to_ptr, describe->table_name, elem_length);
			}
		break;
		case DROP_IN:
			drop = (drop_input_t*) from;
			elem_length = strlen(drop->table_name) + 1;
			memcpy(to_ptr, drop->table_name, elem_length);
		break;
		default:
		break;
	}
}

void* deserialize_content(void* from, socket_operation_t operation, int elements, int* elements_size) {
	select_input_t* select;
	insert_input_t* insert;
	create_input_t* create;
	describe_input_t* describe;
	drop_input_t* drop;
	int offset = 0;
	int length;

	switch (operation) {
		case SELECT_IN:
			select = malloc(sizeof(select_input_t));
			length = elements_size[0];
			select->table_name = malloc(length);
			memcpy(select->table_name, from, length);
			offset += length;
			length = elements_size[1];
			select->key = *((uint16_t*)(from + offset));
			return select;
		break;
		case INSERT_IN:
			insert = malloc(sizeof(insert_input_t));
			length = elements_size[0];
			insert->table_name = malloc(length);
			memcpy(insert->table_name, from + offset, length);
			offset += length;
			length = elements_size[1];
			insert->key = *((uint16_t*)(from + offset));
			offset += length;
			length = elements_size[2];
			insert->value = malloc(length);
			memcpy(insert->value, from + offset, length);
			offset += length;
			length = elements_size[3];
			insert->timestamp = *((long long*)(from + offset));
			return insert;
		break;
		case CREATE_IN:
			create = malloc(sizeof(create_input_t));
			length = elements_size[0];
			create->table_name = malloc(length);
			memcpy(create->table_name, from, length);
			offset += length;
			length = elements_size[1];
			create->consistency = *((consistency_t*)(from + offset));
			offset += length;
			length = elements_size[2];
			create->partitions = *((int*)(from + offset));
			offset += length;
			length = elements_size[3];
			create->compaction_time = *((long*)(from + offset));
			return create;
		break;
		case DESCRIBE_IN:
			describe = malloc(sizeof(describe_input_t));
			if (elements == 0) {
				describe->table_name = NULL;
			} else {
				describe->table_name = malloc(elements_size[0]);
				memcpy(describe->table_name, from, elements_size[0]);
			}
			return describe;
		break;
		case DROP_IN:
			drop = malloc(sizeof(drop_input_t));
			drop->table_name = malloc(elements_size[0]);
			memcpy(drop->table_name, from, elements_size[0]);
			return drop;
		break;
		default:
		break;
	}

	return NULL;
}

void free_deserialized_content(void* content, socket_operation_t operation) {
	switch (operation) {
		case SELECT_IN:
			free(((select_input_t*) content)->table_name);
			free(content);
		break;
		case INSERT_IN:
			free(((insert_input_t*) content)->table_name);
			free(((insert_input_t*) content)->value);
			free(content);
		break;
		case CREATE_IN:
			free(((create_input_t*) content)->table_name);
			free(content);
		break;
		case DESCRIBE_IN:
			free(((describe_input_t*) content)->table_name);
			free(content);
		break;
		case DROP_IN:
			free(((drop_input_t*) content)->table_name);
			free(content);
		break;
		default:
		break;
	}
}

