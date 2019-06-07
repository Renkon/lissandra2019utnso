#include "key_searching.h"

table_metadata_t* read_table_metadata(char* table_directory) {

	table_metadata_t* table_metadata = malloc(sizeof(table_metadata_t));
	char* metadata_directory = create_metadata_directory(table_directory);
	FILE* arch = fopen(metadata_directory, "rb");
	fread(&table_metadata->compaction_time, 1,sizeof(table_metadata->compaction_time), arch);
	fread(&table_metadata->consistency, 1, sizeof(table_metadata->consistency),arch);
	fread(&table_metadata->partitions, 1, sizeof(table_metadata->partitions),arch);
	fclose(arch);

	return table_metadata;
}

partition_t* read_fs_archive(char* partition_directory) {
	FILE* arch = fopen(partition_directory, "rb");
	partition_t* partition = malloc(sizeof(partition_t));
	fread(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks),arch);
	partition->blocks = malloc(sizeof(int) * partition->number_of_blocks);
	fread(partition->blocks, 1, sizeof(int)*partition->number_of_blocks, arch);
	fread(&partition->size, 1, sizeof(partition->size), arch);

	fclose(arch);

	return partition;
}

record_t* search_key_in_fs_archive(char* fs_archive_path, int key) {
	record_t* key_found_in_block = malloc(sizeof(record_t));
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"
	key_found_in_block->timestamp = -1;
	tkv_t* key_found= malloc(sizeof(tkv_t));
	key_found->incomplete = false;
	partition_t* partition = read_fs_archive(fs_archive_path);
	char* string_key = malloc(digits_in_a_number(key)+1);
	string_key = string_itoa(key);
	int index = 0;
	int incomplete_tkv_size = 0;
	for (int i = 0; i < partition->number_of_blocks; i++) {
		key_found = search_key_in_block(partition->blocks[i], string_key,index,incomplete_tkv_size);
		index = 0;
		//Me fijo si la funcion encontro esa key
		if (strcmp(key_found->tkv, "-1;-1;-1") != 0 && !key_found->incomplete) {
			break;
		}
		//Me fijo si encontro un tkv incompleto
		if (key_found->incomplete) {
			while (key_found->incomplete) {
				key_found->incomplete = false;
				int aver = partition->blocks[i+1];
				char* continuation = read_first_tkv_in_block(partition->blocks[i+1]);
				incomplete_tkv_size = strlen(continuation)+1;
				//Busco la siguiente parte y la concateno
				tkv_append(key_found,continuation);
				if (string_ends_with(key_found->tkv, "\n")) {
					strcpy(key_found->tkv, string_substring_until(key_found->tkv,strlen(key_found->tkv)-1));
					key_found->incomplete = true;
					i++;
				}

			free(continuation);
			}
			index = 1;
			char** tkv = string_split(key_found->tkv,";");// Le tengo que hacer free a esto? x2 TODO
				if (strcmp(tkv[1],string_key) == 0 ){
				break;
			}
		//Seteo el tkv al default porque si la key que estaba cortada no es la que busco y se terminan los bloques
		//EStaria devolviendo esa key que no es y si de casualidad tiene el timestamp mas grande devolveria ese
		strcpy(key_found->tkv, "-1;-1;-1");
		}

	}
 convert_to_record(	key_found_in_block,key_found);
 free(key_found);// SI le hago free a un struct que tiene un puntero vasta con esto o primero hago free al puntero del struct? TODO
	return key_found_in_block;
}

int tkv_size(){
	//Es la cantidad de digitos del numero maximo del uint_16 + la cantidad de digitos de la timestamp
	//+ la cantidad maxima de caracteres de un valor + 3 que es 2 porque tiene 2 ; y uno por el  \0
	return  digits_in_a_number(USHRT_MAX)+digits_in_a_number(get_timestamp())+g_config_keys_size+3;

}

void tkv_append(tkv_t* tkv,char* end){
	char* final_tkv=  string_new(); //Le hago free a esto? TODO
	string_append(&final_tkv, tkv->tkv);
	string_append(&final_tkv, end);
	strcpy(tkv->tkv,final_tkv);
}

char* read_first_tkv_in_block(int block){
	char* block_directory = create_block_directory(block);
	FILE* arch = fopen(block_directory, "rb");
	char* readed_key = malloc(tkv_size());
	fread(readed_key, 1,tkv_size(), arch);
	return readed_key;
}

tkv_t* search_key_in_block(int block, char* key, int index, int incomplete_tkv_size) {
	char* block_directory = create_block_directory(block);
	tkv_t* key_found_in_block = malloc(sizeof(tkv_t));
	key_found_in_block->tkv = malloc(tkv_size());
	//Le seteo -1 para que si no la encuentra, devuelva este "tkv invalido"
	strcpy(key_found_in_block->tkv, "-1;-1;-1");
	key_found_in_block->incomplete = false;
	FILE* arch = fopen(block_directory, "rb");

	char* readed_key = malloc(tkv_size());
	//SI mando index en 1 me salteo el primer read
	//Porque asi leo la parte del tkv anterior que ya lei
	if(index == 1){
	fread(readed_key, 1,incomplete_tkv_size, arch);
	}
	while (!feof(arch)) {
		size_t lecture =fread(readed_key, 1,tkv_size(), arch);
		 if(string_ends_with(readed_key,"\n")){
			 //Si tiene \n entonces copio este string sin el \n y prengo el flag de incompleto
			strcpy(key_found_in_block->tkv, string_substring_until(readed_key,strlen(readed_key)-1));
			key_found_in_block->incomplete = true;
			break;
		 }
		 if(lecture == 0){
			//Si el bloque no tiene nada entonces corto todorl.
			 break;
		 }

		char** tkv = string_split(readed_key,";");// Le tengo que hacer free a esto? TODO
		if (strcmp(tkv[1],key) == 0 ){
			//SI encuentro la key entonces paro el while y la devuelvo
			strcpy(key_found_in_block->tkv, readed_key);
			break;
		}
		//La lectura se hace al final, porque? Porque si el archivo no tiene nada hago una lectura y se corta el while.
		//SI la hago primero leeria basura y podria romperse por los ifs de arriba (ya paso)

	}
	fclose(arch);
	free(readed_key);
	free(block_directory);
	return key_found_in_block;

}

void convert_to_record(record_t* record,tkv_t* tkv){
	char** tkv_split = string_split(tkv->tkv,";");// Le tengo que hacer free a esto? x3 TODO
	record->timestamp = string_to_long_long(tkv_split[0]);
	record->key = string_to_int(tkv_split[1]);
	record->value = malloc(strlen(tkv_split[2])+1);
	strcpy(record->value, tkv_split[2]);
	}


record_t* copy_key(record_t* key_to_copy){
	record_t* copied_key = malloc(sizeof(record_t));
	copied_key->timestamp = key_to_copy->timestamp;
	if(copied_key->timestamp != -1){
	copied_key->value = malloc(strlen(key_to_copy->value)+1);
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


