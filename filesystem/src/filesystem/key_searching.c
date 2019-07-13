#include "key_searching.h"

table_metadata_t* read_table_metadata(char* table_directory) {

	table_metadata_t* table_metadata = malloc(sizeof(table_metadata_t));
	char* metadata_directory = create_metadata_directory(table_directory);
	FILE* arch = fopen(metadata_directory, "rb");
	fread(&table_metadata->compaction_time, 1, sizeof(table_metadata->compaction_time), arch);
	fread(&table_metadata->consistency, 1, sizeof(table_metadata->consistency), arch);
	fread(&table_metadata->partitions, 1, sizeof(table_metadata->partitions), arch);
	fclose(arch);
	free(metadata_directory);
	return table_metadata;
}

partition_t* read_fs_archive(char* partition_directory) {
	FILE* arch = fopen(partition_directory, "rb");
	partition_t* partition = malloc(sizeof(partition_t));
	fread(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks), arch);
	partition->blocks = malloc(sizeof(int) * partition->number_of_blocks);
	fread(partition->blocks, 1, sizeof(int) * partition->number_of_blocks, arch);
	fread(&partition->size, 1, sizeof(partition->size), arch);

	fclose(arch);

	return partition;
}

record_t* search_key_in_fs_archive(char* fs_archive_path, int key) {
	record_t* key_found_in_block = malloc(sizeof(record_t));
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"
	key_found_in_block->timestamp = -1;
	tkv_t* key_found;
	tkv_t* correct_key_found = malloc(sizeof(tkv_t));
	correct_key_found ->tkv = malloc(strlen("-1;-1;-1")+1);
	strcpy(correct_key_found->tkv,"-1;-1;-1");
	partition_t* partition = read_fs_archive(fs_archive_path);
	char* string_key = string_itoa(key);
	int index = 0;
	int incomplete_tkv_size = 0;

	for (int i = 0; i < partition->number_of_blocks; i++) {
		key_found = search_key_in_block(partition->blocks[i], string_key, index, incomplete_tkv_size,correct_key_found);
		index = 0;

		//Me fijo si encontro la key
	if (strcmp(key_found->tkv, "-1;-1;-1") != 0 && !key_found->incomplete) {
		char** tkv_found = string_split(key_found->tkv, ";");
		long long tkv_found_timestamp =string_to_long_long(tkv_found[0]);
		//Me fijo si esa key tiene timestamp mas grande qque la que encontre antes
		if(tkv_found_timestamp> key_found_in_block->timestamp){
			key_found_in_block->timestamp = tkv_found_timestamp;
			free(correct_key_found->tkv);
			correct_key_found->tkv = malloc(strlen(key_found->tkv)+1);
			strcpy(correct_key_found->tkv, key_found->tkv);

		}
			free(tkv_found[0]);
			free(tkv_found[1]);
			free(tkv_found[2]);
			free(tkv_found);

		}

		//Me fijo si encontro un tkv incompleto
		if (key_found->incomplete) {
			while (key_found->incomplete) {
				key_found->incomplete = false;
				char* continuation = read_first_tkv_in_block(partition->blocks[i + 1]);
				incomplete_tkv_size = strlen(continuation)+1;
				//Busco la siguiente parte y la concateno
				tkv_append(key_found,continuation);
				if (string_ends_with(key_found->tkv, "\n")) {
					char* substr = string_substring_until(key_found->tkv, strlen(key_found->tkv) - 1);
					strcpy(key_found->tkv, substr);
					key_found->incomplete = true;
					i++;
					free(substr);
				}

				free(continuation);
			}

			index = 1;
			char** tkv = string_split(key_found->tkv, ";");
			long long tkv_timestamp = string_to_long_long(tkv[0]);
			if (strcmp(tkv[1], string_key) == 0 && tkv_timestamp > key_found_in_block->timestamp) {
					key_found_in_block ->timestamp = tkv_timestamp;
					free(correct_key_found->tkv);
					correct_key_found->tkv = malloc(strlen(key_found->tkv)+1);
					strcpy(correct_key_found->tkv,key_found->tkv);
			}
			free(tkv[0]);
			free(tkv[1]);
			free(tkv[2]);
			free(tkv);

			//Seteo el tkv al default porque si la key que estaba cortada no es la que busco y se terminan los bloques
			//EStaria devolviendo esa key que no es y si de casualidad tiene el timestamp mas grande devolveria ese
			strcpy(key_found->tkv, "-1;-1;-1");

		}

		// limpiamos en cada iteracion
		if (i < partition->number_of_blocks - 1) {
			free(key_found->tkv);
			free(key_found);
		}
	}
	convert_to_record(key_found_in_block, correct_key_found);
	free(key_found->tkv);
	free(key_found);
	free(correct_key_found->tkv);
	free(correct_key_found);
	free(partition->blocks);
	free(partition);
	free(string_key);
	return key_found_in_block;
}

int tkv_size() {
	//Es la cantidad de digitos del numero maximo del uint_16 + la cantidad de digitos de la timestamp
	//+ la cantidad maxima de caracteres de un valor + 3 que es 2 porque tiene 2 ; y uno por el  \0
	return digits_in_a_number(USHRT_MAX) + digits_in_a_number(get_timestamp()) + g_config.max_value_size + 3;
}

void tkv_append(tkv_t* tkv,char* end){
	char* final_tkv = string_new();
	string_append(&final_tkv, tkv->tkv);
	string_append(&final_tkv, end);
	strcpy(tkv->tkv, final_tkv);
	free(final_tkv);
}

char* read_first_tkv_in_block(int block) {
	char* block_directory = create_block_directory(block);
	FILE* arch = fopen(block_directory, "rb");
	char* readed_key = malloc(tkv_size());
	fread(readed_key, 1, tkv_size(), arch);
	fclose(arch);
	free(block_directory);
	return readed_key;
}

tkv_t* search_key_in_block(int block, char* key, int index, int incomplete_tkv_size, tkv_t* previous_key_founded) {
	char* block_directory = create_block_directory(block);
	tkv_t* key_found_in_block = malloc(sizeof(tkv_t));
	key_found_in_block->tkv = malloc(tkv_size());
	//Le seteo -1 para que si no la encuentra, devuelva este "tkv invalido"
	strcpy(key_found_in_block->tkv, "-1;-1;-1");
	long long key_found_timestamp = -1;
	key_found_in_block->incomplete = false;
	FILE* arch = fopen(block_directory, "rb");

	char* readed_key = calloc(1, tkv_size());
	int pointer= 0;

	//SI mando index en 1 me salteo el primer read
	//Porque asi leo la parte del tkv anterior que ya lei
	if (index == 1) {
		fread(readed_key, 1, incomplete_tkv_size, arch);
		pointer += incomplete_tkv_size;
	}

	int i = 0;
	while (!feof(arch)) {
		size_t lecture = fread(readed_key, 1, tkv_size(), arch);
		pointer+= strlen(readed_key)+1;
		fseek(arch,pointer,SEEK_SET);
		if (readed_key[0] && string_ends_with(readed_key, "\n")) {
			//Si tiene \n entonces copio este string sin el \n y prengo el flag de incompleto
			char* substr = string_substring_until(readed_key, strlen(readed_key) - 1);

			if(!string_equals_ignore_case(key_found_in_block->tkv,"-1;-1;-1")){
				free(previous_key_founded->tkv);
				previous_key_founded->tkv = strdup(key_found_in_block->tkv);
			}
			strcpy(key_found_in_block->tkv, substr);
			key_found_in_block->incomplete = true;
			free(substr);
			break;
		}

		if (lecture == 0) {
			//Si el bloque no tiene nada entonces corto todorl.
			break;
		}

		char** tkv = string_split(readed_key, ";");
		long long tkv_timestamp =string_to_long_long(tkv[0]);
		if (strcmp(tkv[1], key) == 0 && tkv_timestamp > key_found_timestamp){
			//SI encuentro la key entonces paro el while y la devuelvo
			strcpy(key_found_in_block->tkv, readed_key);
			key_found_timestamp = tkv_timestamp;
		}

		free(tkv[0]);
		free(tkv[1]);
		free(tkv[2]);
		free(tkv);
		//La lectura se hace al final, porque? Porque si el archivo no tiene nada hago una lectura y se corta el while.
		//SI la hago primero leeria basura y podria romperse por los ifs de arriba (ya paso)

	}

	fclose(arch);
	free(readed_key);
	free(block_directory);
	return key_found_in_block;

}

void convert_to_record(record_t* record, tkv_t* tkv) {
	char** tkv_split = string_split(tkv->tkv, ";");
	record->timestamp = string_to_long_long(tkv_split[0]);
	record->key = string_to_int(tkv_split[1]);
	record->value = malloc(strlen(tkv_split[2]) + 1);

	strcpy(record->value, tkv_split[2]);

	free(tkv_split[0]);
	free(tkv_split[1]);
	free(tkv_split[2]);
	free(tkv_split);
}

record_t* copy_key2(record_t* key_to_copy){
	record_t* copied_key = malloc(sizeof(record_t));
	copied_key->timestamp = key_to_copy->timestamp;
	if (copied_key->timestamp != -1) {
		copied_key->value = malloc(strlen(key_to_copy->value) + 1);
		copied_key->key = key_to_copy->key;
		strcpy(copied_key->value, key_to_copy->value);
		copied_key->fs_archive_where_it_was_found = strdup(key_to_copy->fs_archive_where_it_was_found);
	}

	return copied_key;
}

record_t* copy_key(record_t* key_to_copy){
	record_t* copied_key = malloc(sizeof(record_t));
	copied_key->timestamp = key_to_copy->timestamp;
	if (copied_key->timestamp != -1) {
		copied_key->value = malloc(strlen(key_to_copy->value) + 1);
		copied_key->key = key_to_copy->key;
		strcpy(copied_key->value, key_to_copy->value);
	}

	return copied_key;
}

record_t* key_with_greater_timestamp(record_t* key_1, record_t* key_2) {
	if (key_1->timestamp > key_2->timestamp) {
		return key_1;
	} else {
		return key_2;
	}

}


