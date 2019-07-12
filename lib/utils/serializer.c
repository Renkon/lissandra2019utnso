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
	elements_network_t element_info = init_elements_info(4);
	int* iterator = element_info.elements_size;

	*iterator = strlen(record->table_name) + 1;
	iterator++;
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

elements_network_t elements_multiinsert_in_info(void* input) {
	t_list* inserts = (t_list*) input;
	insert_input_t* insert;
	elements_network_t element_info = init_elements_info(4 * list_size(inserts));
	int* iterator = element_info.elements_size;

	for (int i = 0; i < list_size(inserts); i++) {
		insert = (insert_input_t*) list_get(inserts, i);
		*iterator = strlen(insert->table_name) + 1;
		iterator++;
		*iterator = sizeof(uint16_t);
		iterator++;
		*iterator = strlen(insert->value) + 1;
		iterator++;
		*iterator = sizeof(long long);
		iterator++;
	}

	return element_info;
}

elements_network_t elements_multiinsert_out_info(void* input) {
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
	table_metadata_t* table_metadata;
	t_list* describe_list = (t_list*) input;
	elements_network_t element_info = init_elements_info(4 * list_size(describe_list));
	int* iterator = element_info.elements_size;

	for (int i = 0; i < list_size(describe_list); i++) {
		table_metadata = (table_metadata_t*) list_get(describe_list, i);
		*iterator = strlen(table_metadata->table_name) + 1;
		iterator++;
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

elements_network_t elements_value_in_info(void* input) {
	return init_elements_info(0);
}

elements_network_t elements_value_out_info(void* input) {
	elements_network_t element_info = init_elements_info(1);
	int* iterator = element_info.elements_size;

	*iterator = sizeof(int);

	return element_info;
}

elements_network_t elements_gossip_info(void* input) {
	memory_t* memory;
	t_list* memories = (t_list*) input;
	elements_network_t element_info = init_elements_info(5 * list_size(memories));
	int* iterator = element_info.elements_size;

	for (int i = 0; i < list_size(memories); i++) {
		memory = (memory_t*) list_get(memories, i);
		*iterator = sizeof(int);
		iterator++;
		*iterator = strlen(memory->ip) + 1;
		iterator++;
		*iterator = sizeof(int);
		iterator++;
		*iterator = sizeof(bool);
		iterator++;
		*iterator = sizeof(long long);
		iterator++;
	}

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

elements_network_t get_out_element_info(socket_operation_t operation, void* input) {
	switch(operation) {
		case SELECT_OUT:
			return elements_select_out_info(input);
		break;
		case INSERT_OUT:
			return elements_insert_out_info(input);
		break;
		case CREATE_OUT:
			return elements_create_out_info(input);
		break;
		case DESCRIBE_OUT:
			return elements_describe_out_info(input);
		break;
		case DROP_OUT:
			return elements_drop_out_info(input);
		break;
		case JOURNAL_OUT:
			return elements_journal_out_info(input);
		break;
		case VALUE_OUT:
			return elements_value_out_info(input);
		break;
		case GOSSIP_OUT:
			return elements_gossip_info(input);
		break;
		case MULTIINSERT_OUT:
			return elements_multiinsert_out_info(input);
		break;
		default:
			return init_elements_info(0);
		break;
	}
}

void serialize_content(void* to, socket_operation_t operation, void* from) {
	select_input_t* select;
	record_t* record_select;
	insert_input_t* insert;
	int* insert_response;
	int* multiinsert_response;
	t_list* inserts;
	create_input_t* create;
	int* create_response;
	describe_input_t* describe;
	t_list* metadata_describe;
	table_metadata_t* metadata;
	drop_input_t* drop;
	t_list* memories;
	memory_t* memory;
	int* drop_response;
	int* journal_response;
	int* value_response;
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
		case SELECT_OUT:
			record_select = (record_t*) from;

			elem_length = strlen(record_select->table_name) + 1;
			memcpy(to_ptr, record_select->table_name, elem_length);
			offset += elem_length;

			elem_length = sizeof(long long);
			memcpy(to_ptr + offset, &(record_select->timestamp), elem_length);
			offset += elem_length;

			elem_length = sizeof(int);
			memcpy(to_ptr + offset, &(record_select->key), elem_length);
			offset += elem_length;

			elem_length = strlen(record_select->value) + 1;
			memcpy(to_ptr + offset, record_select->value, elem_length);
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
		case INSERT_OUT:
			insert_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, insert_response, elem_length);
		break;
		case MULTIINSERT_IN:
			inserts = (t_list*) from;

			for (int i = 0; i < list_size(inserts); i++) {
				insert = (insert_input_t*) list_get(inserts, i);

				elem_length = strlen(insert->table_name) + 1;
				memcpy(to_ptr + offset, insert->table_name, elem_length);
				offset += elem_length;

				elem_length = sizeof(uint16_t);
				memcpy(to_ptr + offset, &(insert->key), elem_length);
				offset += elem_length;

				elem_length = strlen(insert->value) + 1;
				memcpy(to_ptr + offset, insert->value, elem_length);
				offset += elem_length;

				elem_length = sizeof(long long);
				memcpy(to_ptr + offset, &(insert->timestamp), elem_length);
				offset += elem_length;
			}
		break;
		case MULTIINSERT_OUT:
			multiinsert_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, multiinsert_response, elem_length);
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
		case CREATE_OUT:
			create_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, create_response, elem_length);
		break;
		case DESCRIBE_IN:
			describe = (describe_input_t*) from;

			if (describe->table_name != NULL) {
				elem_length = strlen(describe->table_name) + 1;
				memcpy(to_ptr, describe->table_name, elem_length);
			}
		break;
		case DESCRIBE_OUT:
			metadata_describe = (t_list*) from;
			for (int i = 0; i < list_size(metadata_describe); i++) {
				metadata = (table_metadata_t*) list_get(metadata_describe, i);

				elem_length = strlen(metadata->table_name) + 1;
				memcpy(to_ptr + offset, metadata->table_name, elem_length);
				offset += elem_length;

				elem_length = sizeof(consistency_t);
				memcpy(to_ptr + offset, &(metadata->consistency), elem_length);
				offset += elem_length;

				elem_length = sizeof(int);
				memcpy(to_ptr + offset, &(metadata->partitions), elem_length);
				offset += elem_length;

				elem_length = sizeof(long);
				memcpy(to_ptr + offset, &(metadata->compaction_time), elem_length);
				offset += elem_length;
			}
		break;
		case DROP_IN:
			drop = (drop_input_t*) from;
			elem_length = strlen(drop->table_name) + 1;
			memcpy(to_ptr, drop->table_name, elem_length);
		break;
		case DROP_OUT:
			drop_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, drop_response, elem_length);
		break;
		case JOURNAL_OUT:
			journal_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, journal_response, elem_length);
		break;
		case VALUE_OUT:
			value_response = (int*) from;

			elem_length = sizeof(int);
			memcpy(to_ptr, value_response, elem_length);
		break;
		case GOSSIP_IN:
		case GOSSIP_OUT:
			memories = (t_list*) from;
			for (int i = 0; i < list_size(memories); i++) {
				memory = (memory_t*) list_get(memories, i);

				elem_length = sizeof(int);
				memcpy(to_ptr + offset, &(memory->id), elem_length);
				offset += elem_length;

				elem_length = strlen(memory->ip) + 1;
				memcpy(to_ptr + offset, memory->ip, elem_length);
				offset += elem_length;

				elem_length = sizeof(int);
				memcpy(to_ptr + offset, &(memory->port), elem_length);
				offset += elem_length;

				elem_length = sizeof(bool);
				memcpy(to_ptr + offset, &(memory->alive), elem_length);
				offset += elem_length;

				elem_length = sizeof(long long);
				memcpy(to_ptr + offset, &(memory->timestamp), elem_length);
				offset += elem_length;
			}
		break;
		default:
		break;
	}
}

void* deserialize_content(void* from, socket_operation_t operation, int elements, int* elements_size) {
	select_input_t* select;
	record_t* record_select;
	insert_input_t* insert;
	int* insert_response;
	t_list* inserts;
	int* multiinsert_response;
	create_input_t* create;
	int* create_response;
	describe_input_t* describe;
	t_list* metadata_describe;
	table_metadata_t* metadata;
	drop_input_t* drop;
	t_list* memories;
	memory_t* memory;
	int* drop_response;
	int* journal_response;
	int* value_response;
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
		case SELECT_OUT:
			record_select = malloc(sizeof(record_t));
			length = elements_size[0];
			record_select->table_name = malloc(length);
			memcpy(record_select->table_name, from + offset, length);
			offset += length;
			length = elements_size[1];
			record_select->timestamp = *((long long*)(from + offset));
			offset += length;
			length = elements_size[2];
			record_select->key = *((int*)(from + offset));
			offset += length;
			length = elements_size[3];
			record_select->value = malloc(length);
			memcpy(record_select->value, from + offset, length);
			return record_select;
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
		case INSERT_OUT:
			insert_response = malloc(sizeof(int));
			memcpy(insert_response, from, elements_size[0]);
			return insert_response;
		break;
		case MULTIINSERT_IN:
			inserts = list_create();
			for (int i = 0; i < elements; i = i + 4) {
				insert = malloc(sizeof(insert_input_t));

				length = elements_size[i];
				insert->table_name = malloc(length);
				memcpy(insert->table_name, from + offset, length);
				offset += length;
				length = elements_size[i + 1];
				insert->key = *((uint16_t*)(from + offset));
				offset += length;
				length = elements_size[i + 2];
				insert->value = malloc(length);
				memcpy(insert->value, from + offset, length);
				offset += length;
				length = elements_size[i + 3];
				insert->timestamp = *((long long*)(from + offset));
				offset += length;
				list_add(inserts, insert);
			}
			return inserts;
		break;
		case MULTIINSERT_OUT:
			multiinsert_response = malloc(sizeof(int));
			memcpy(multiinsert_response, from, elements_size[0]);
			return multiinsert_response;
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
		case CREATE_OUT:
			create_response = malloc(sizeof(int));
			memcpy(create_response, from, elements_size[0]);
			return create_response;
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
		case DESCRIBE_OUT:
			metadata_describe = list_create();
			for (int i = 0; i < elements; i = i + 4) {
				metadata = malloc(sizeof(table_metadata_t));
				length = elements_size[i];
				metadata->table_name = malloc(length);
				memcpy(metadata->table_name, from + offset, length);
				offset += length;
				length = elements_size[i + 1];
				metadata->consistency = *((consistency_t*)(from + offset));
				offset += length;
				length = elements_size[i + 2];
				metadata->partitions = *((int*)(from + offset));
				offset += length;
				length = elements_size[i + 3];
				metadata->compaction_time = *((long*)(from + offset));
				offset += length;
				list_add(metadata_describe, metadata);
			}
			return metadata_describe;
		break;
		case DROP_IN:
			drop = malloc(sizeof(drop_input_t));
			drop->table_name = malloc(elements_size[0]);
			memcpy(drop->table_name, from, elements_size[0]);
			return drop;
		break;
		case DROP_OUT:
			drop_response = malloc(sizeof(int));
			memcpy(drop_response, from, elements_size[0]);
			return drop_response;
		break;
		case JOURNAL_OUT:
			journal_response = malloc(sizeof(int));
			memcpy(journal_response, from, elements_size[0]);
			return journal_response;
		break;
		case VALUE_OUT:
			value_response = malloc(sizeof(int));
			memcpy(value_response, from, elements_size[0]);
			return value_response;
		break;
		case GOSSIP_IN:
		case GOSSIP_OUT:
			memories = list_create();
			for (int i = 0; i < elements; i = i + 5) {
				memory = malloc(sizeof(memory_t));
				length = elements_size[i];
				memory->id = *((int*)(from + offset));
				offset += length;
				length = elements_size[i + 1];
				memory->ip = malloc(length);
				memcpy(memory->ip, from + offset, length);
				offset += length;
				length = elements_size[i + 2];
				memory->port = *((int*)(from + offset));
				offset += length;
				length = elements_size[i + 3];
				memory->alive = *((bool*)(from + offset));
				offset += length;
				length = elements_size[i + 4];
				memory->timestamp = *((long long*)(from + offset));
				offset += length;
				list_add(memories, memory);
			}
			return memories;
		break;
		default:
		break;
	}

	return NULL;
}

void free_deserialized_content(void* content, socket_operation_t operation) {
	t_list* describe_list;
	t_list* inserts;
	t_list* memories;

	if (content == NULL)
		return;

	switch (operation) {
		case SELECT_IN:
			free(((select_input_t*) content)->table_name);
			free(content);
		break;
		case SELECT_OUT:
			free(((record_t*) content)->table_name);
			free(((record_t*) content)->value);
			free(content);
		break;
		case INSERT_IN:
			free(((insert_input_t*) content)->table_name);
			free(((insert_input_t*) content)->value);
			free(content);
		break;
		case INSERT_OUT:
			free(content);
		break;
		case MULTIINSERT_IN:
			inserts = (t_list*) content;
			for (int i = 0; i < list_size(inserts); i++) {
				free(((insert_input_t*) list_get(inserts, i))->table_name);
				free(((insert_input_t*) list_get(inserts, i))->value);
				free(list_get(inserts, i));
			}
			list_destroy(inserts);
		break;
		case MULTIINSERT_OUT:
			free(content);
		break;
		case CREATE_IN:
			free(((create_input_t*) content)->table_name);
			free(content);
		break;
		case CREATE_OUT:
			free(content);
		break;
		case DESCRIBE_IN:
			if (((describe_input_t*) content)->table_name != NULL)
				free(((describe_input_t*) content)->table_name);
			free(content);
		break;
		case DESCRIBE_OUT:
			describe_list = (t_list*) content;
			for (int i = 0; i < list_size(describe_list); i++) {
				free(((table_metadata_t*) list_get(describe_list, i))->table_name);
				free(list_get(describe_list, i));
			}
			list_destroy(describe_list);
		break;
		case DROP_IN:
			free(((drop_input_t*) content)->table_name);
			free(content);
		break;
		case DROP_OUT:
			free(content);
		break;
		case JOURNAL_OUT:
			free(content);
		break;
		case VALUE_OUT:
			free(content);
		break;
		case GOSSIP_IN:
		case GOSSIP_OUT:
			memories = (t_list*) content;
			for (int i = 0; i < list_size(memories); i++) {
				free(((memory_t*) list_get(memories, i))->ip);
				free(list_get(memories, i));
			}
			list_destroy(memories);
		break;
		default:
		break;
	}
}
