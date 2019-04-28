#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(),table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 si creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name, int* blocks) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);

	for (int i = 0; i < partitions; i++) {
		char* bin_name = malloc(digits_in_a_number(partitions) + strlen("/.bin") + 1);
		sprintf(bin_name, "/%d.bin", i + 1); //Esto transforma de int a string
		char* partition_route = malloc(strlen(table_directory) + strlen(bin_name) + 1);
		strcpy(partition_route, table_directory);
		strcat(partition_route, bin_name); //aca tengo toda la direccion y termina con n.bin n= nombre archivo

		FILE* arch = fopen(partition_route, "wb");
		//Creo el struct de la particion con sus datos
		Partition *partition = malloc(sizeof(Partition));
		partition->blocks = malloc(sizeof(int));
		partition->blocks[0] = blocks[i];
		partition->size = 0;

		fwrite(&partition->size, 1, sizeof(partition->size), arch);
		fwrite(partition->blocks, 1, sizeof(int), arch);

		fclose(arch);
		free(bin_name);
		free(partition);
		free(partition_route);
	}

	free(table_directory);

}

void create_table_metadata(consistency_t consistency, int partitions,
		long compaction_time, char* table_name) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);
	char* metadata_name = "/metadata.bin";
	char* metadata_directory = malloc(strlen(table_directory) + strlen(metadata_name) + 1);
	Table_metadata *table_metadata = malloc(sizeof(Table_metadata));
	//con esto ya tendria toda la direccion donde va a estar la metadata
	metadata_directory = create_new_directory(table_directory, metadata_name);
	//Creo el struct metadata a guardar con los datos del input
	table_metadata = create_metadata(consistency, partitions, compaction_time);
	//ABro el nuevo .bin y le guardo los datos correspondientes
	FILE* arch = fopen(metadata_directory, "wb");
	fwrite(&table_metadata->compaction_time, 1,sizeof(table_metadata->compaction_time), arch);
	fwrite(&table_metadata->consistency, 1, sizeof(table_metadata->consistency),arch);
	fwrite(&table_metadata->partitions, 1, sizeof(table_metadata->partitions),arch);
	fclose(arch);

	free(table_directory);
	free(metadata_directory);

}

Table_metadata *create_metadata(consistency_t consistency, int partitions,long compaction_time) {

	Table_metadata *metadata = malloc(sizeof(Table_metadata));
	metadata->compaction_time = compaction_time;
	metadata->consistency = consistency;
	metadata->partitions = partitions;
	return metadata;

}

int assign_free_blocks(t_bitarray *bitmap, int *blocks, int *partitions_amount) {

	int blocks_amount = 0;

//Si me pedis mas particiones que bloques existentes entonces ni me molesto en mirar el bitmap
	if (partitions_amount <= bitmap->size) {

		//Voy a buscar bloques tantas veces como particiones vaya a tener mi tabla
		for (int i = 0; i < partitions_amount; i++) {
			int block = find_free_block(bitmap);
			if (block == -1) {
				//SI no encontre un bloque libre se termina
				break;
			} else {
				//Si encontre un bloque lo guardo en mi array de bloques y aumento el contador de bloques
				blocks_amount++;

				blocks[i] = block;

			}
		}
	}
	//Siempre voy  a devolver la cantidad de bloques libres que encontre
	return blocks_amount;
}

int find_free_block(t_bitarray *bitmap) {
	int i = 0;
	//Busco por todo el bitmap hasta encontrar un 0

	while (i < bitmap->size) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			bitarray_set_bit(bitmap, i);
			//Si lo encuentro seteo ese lugar en 1 y lo devuelvo
			return i;

		}
		i++;
	}
	//Si no lo encuentro solo devuelvo -1
	return -1;
}
