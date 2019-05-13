#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(), table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 si creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name, int* blocks) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);

	for (int i = 0; i < partitions; i++) {
		//Pongo i+1 porque no existe la particion "0"
		char* bin_name = malloc(digits_in_a_number(i+1) + strlen("/.bin") + 1);
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
		partition->number_of_blocks = 0;

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
	char* metadata_directory = create_metadata_directory(table_directory);
	Table_metadata* table_metadata = malloc(sizeof(Table_metadata));
	//con esto ya tendria toda la direccion donde va a estar la metadata
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

Table_metadata* create_metadata(consistency_t consistency, int partitions,long compaction_time) {

	Table_metadata *metadata = malloc(sizeof(Table_metadata));
	metadata->compaction_time = compaction_time;
	metadata->consistency = consistency;
	metadata->partitions = partitions;
	return metadata;

}

int assign_free_blocks(t_bitarray* bitmap, int* blocks, int* partitions_amount) {
	int blocks_amount = 0;
	//Me fijo que la cantidad de particiones que me pedis sea menor a la cantidad de bloques totales
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

int find_free_block(t_bitarray* bitmap) {
	int i = 0;
	//Busco por todo el bitmap hasta encontrar un 0

	while (i < bitmap->size) {
		if (bitarray_test_bit(bitmap, i) == 0) {
			//Si lo encuentro seteo ese lugar en 1 y lo devuelvo
			bitarray_set_bit(bitmap, i);
			return i;
		}
		i++;
	}
	//Si no lo encuentro solo devuelvo -1
	return -1;
}

char* search_key (char* table_directory, int key){
	//Leo la metadata de la tabla
	Table_metadata* table_metadata = read_table_metadata(table_directory);
	 //Calculo la particion donde se supone que esta la key
	int partition_number= (key%table_metadata->partitions)+1;
	//Creo la direccion hasta la particion
	char* partition_name = create_partition_name(partition_number);
	char* partition_path = malloc(strlen(table_directory)+strlen(partition_name)+1);
	strcpy(partition_path, table_directory);
	strcat(partition_path, partition_name);

	//Buscar en esa particion
	 //Buscar en todos los tmp
	 //Buscar en el tmpc
	 //Encontrar el que tiene el timestamp mas grande
	 //DEvuelvo la key encontrada o no.
	free(partition_path);
	return "eo";

}
