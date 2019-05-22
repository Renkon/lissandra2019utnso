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
	record_t* key_found = malloc(sizeof(record_t));
	partition_t* partition = read_fs_archive(fs_archive_path);

	for (int i = 0; i < partition->number_of_blocks; i++) {

		key_found = search_key_in_block(partition->blocks[i], key);

		if (key_found->timestamp != -1) {

			break;

		}

	}
	return key_found;
}

record_t* search_key_in_block(int block, int key) {
	char* block_directory = create_block_directory(block);

	record_t* key_found_in_block = malloc(sizeof(record_t));
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"
	key_found_in_block->timestamp = -1;

	FILE* arch = fopen(block_directory, "rb");

	record_t* readed_key = malloc(sizeof(record_t));
	while (!feof(arch)) {

		//Leo el bloque
		fread(&readed_key->key, 1, sizeof(readed_key->key), arch);
		fread(&readed_key->value_length, 1, sizeof(readed_key->value_length),arch);
		fread(&readed_key->timestamp, 1, sizeof(readed_key->timestamp), arch);
		readed_key->value = malloc((readed_key->value_length)+1);
		fread(readed_key->value, 1,readed_key->value_length +1, arch);


		if (readed_key->key == key) {
			//SI encuentro la key entonces paro el while y la devuelvo
			key_found_in_block = copy_key(readed_key);
			free(readed_key);
			break;
		}
		//Si no lo encuentro sigo buscando
	}
	fclose(arch);
	free(block_directory);
	return key_found_in_block;

}

record_t* copy_key(record_t* key_to_copy){
	record_t* copied_key = malloc(sizeof(record_t));
	copied_key->timestamp = key_to_copy->timestamp;
	if(copied_key->timestamp != -1){
	copied_key->value = malloc(key_to_copy->value_length+1);
	copied_key->key = key_to_copy->key;
	strcpy(copied_key->value, key_to_copy->value);
	copied_key->value_length = key_to_copy->value_length;

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


