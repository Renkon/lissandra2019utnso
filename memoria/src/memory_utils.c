/*
 * memory_utils.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */
#include "memory_utils.h"

int memory_insert(long long timestamp, int key, char* value){
	char* str_key;
	char* str_tstamp;

	for (int i = 0; i < total_page_count; i++) {
		if (strcmp(main_memory[i], "null") == 0) {
			str_key = string_itoa(key);
			str_tstamp = string_from_format("%lld", timestamp);
			str_tstamp = realloc(str_tstamp, strlen(str_tstamp) + strlen(str_key) + strlen(value) + 3);

			strcat(str_tstamp, ";");
			strcat(str_tstamp, str_key);
			strcat(str_tstamp, ";");
			strcat(str_tstamp, value);

			strcpy(main_memory[i], str_tstamp);

			free(str_key);
			free(str_tstamp);
			return i;
		}
	}

	return -1;
}

bool memory_full() {
	for (int i = 0; i < total_page_count; i++) {
		if (strcmp(main_memory[i], "null") == 0) {
			return i >= total_page_count;
		}
	}

	return true;
}

/*char* main_memory_key(int index){ //TODO puedo hacerlo mejor con un enum
	char** our_array = string_split(main_memory[index], ";");
	char* value = our_array[1];

	free(our_array[0]);
	free(our_array[2]);
	free(our_array);

	return value;
}

char* main_memory_timestamp(int index){ //TODO puedo hacerlo mejor con un enum
	char** our_array = string_split(main_memory[index], ";");
	char* value = our_array[0];

	free(our_array[1]);
	free(our_array[2]);
	free(our_array);

	return value;
}

char* main_memory_value(int index){ //TODO puedo hacerlo mejor con un enum
	char** our_array = string_split(main_memory[index], ";");
	char* value = our_array[2];

	free(our_array[0]);
	free(our_array[1]);
	free(our_array);

	return value;
}*/

char* main_memory_values(int index,memory_var_t type){ //TODO puedo hacerlo mejor con un enum
	char** our_array = string_split(main_memory[index], ";");
	char* value = our_array[type];
	switch( type ){
	case 0:
		free(our_array[1]);
		free(our_array[2]);
		break;
	case 1:
		free(our_array[0]);
		free(our_array[2]);
		break;
	case 2:
		free(our_array[0]);
		free(our_array[1]);
		break;
	default:
		free(our_array[0]);
		free(our_array[1]);
		free(our_array[2]);
		break;
	}
		free(our_array);

	return value;
}

void modify_memory_by_index(int index, int key , char* value){
	char* str_key = string_itoa(key);
	char* str_tstamp = string_from_format("%lld", get_timestamp());

	str_tstamp = realloc(str_tstamp, strlen(str_tstamp) + strlen(str_key) + strlen(value) + 3);

	strcat(str_tstamp, ";");
	strcat(str_tstamp, str_key);
	strcat(str_tstamp, ";");
	strcat(str_tstamp, value);
	strcpy(main_memory[index], str_tstamp);
}
