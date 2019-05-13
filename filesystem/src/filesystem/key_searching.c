#include "key_searching.h"

Table_metadata* read_table_metadata(char* table_directory) {

	Table_metadata* table_metadata = malloc(sizeof(Table_metadata));
	char* metadata_directory = create_metadata_directory(table_directory);
	FILE* arch = fopen(metadata_directory, "rb");
	fread(&table_metadata->compaction_time, 1,sizeof(table_metadata->compaction_time), arch);
	fread(&table_metadata->consistency, 1, sizeof(table_metadata->consistency),arch);
	fread(&table_metadata->partitions, 1, sizeof(table_metadata->partitions),arch);
	fclose(arch);

	return table_metadata;
}

Partition* read_partition(char* partition_string) {
	FILE* arch2 = fopen(partition_string, "rb");
	Partition* partition = malloc(sizeof(Partition));
	fread(&partition->size, 1, sizeof(sizeof(int)), arch2);
	fread(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks),arch2);
	partition->blocks = malloc(sizeof(int) * partition->number_of_blocks);
	fread(partition->blocks, 1, sizeof(partition->blocks), arch2);

	return partition;
}

search_key_in_partition(char* partition_path, int key) {
	Key* key_found = malloc(sizeof(Key));
	Partition* partition = read_partition(partition_path);

	for (int i = 0; i < partition->number_of_blocks; i++) {

		key_found = search_key_in_block(i + 1, key);

		if (key_found->timestamp != -1) {

			break;

		}

	}
	return key_found;
}

Key search_key_in_block(int block, int key) {
	char* block_directory = create_block_directory(block);

	Key* key_found_in_block = malloc(sizeof(Key));
	//Le seteo -1 para que si no encuentra una key siga buscando
	key_found_in_block->timestamp = -1;
	FILE* arch = fopen(block_directory, "rb");
	while(!feof(arch)){

		Key* readed_key = malloc(sizeof(Key));

		//FALTA PONER LA LOGICA PARA LEER LOS BLOQUES (STRUCT KEY) Y DESPUES BUSCAR.

	}


	free(block_directory);

}

