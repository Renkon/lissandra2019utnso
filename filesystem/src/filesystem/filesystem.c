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
		partition_t* partition = malloc(sizeof(partition_t));
		partition->number_of_blocks = 1;
		partition->blocks = malloc(sizeof(int));
		partition->blocks[0] = blocks[i]+1;
		partition->size = 69;

		fwrite(&partition->number_of_blocks, 1, sizeof(partition->number_of_blocks), arch);
		fwrite(partition->blocks, 1, sizeof(int), arch);
		fwrite(&partition->size, 1, sizeof(partition->size), arch);

		fclose(arch);
		free(bin_name);
		free(partition);
		free(partition_route);
	}
	free(table_directory);

}

void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);
	char* metadata_name = "/metadata.bin";
	char* metadata_directory = create_metadata_directory(table_directory);
	table_metadata_t* table_metadata = malloc(sizeof(table_metadata_t));
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

table_metadata_t* create_metadata(consistency_t consistency, int partitions, long compaction_time) {

	table_metadata_t* metadata = malloc(sizeof(table_metadata_t));
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

record_t* search_key (char* table_directory, int key){

	//Busco la key en el unico archivo tmp que puede haber
	record_t* key_found_in_tmpc = search_in_tmpc(table_directory,key);
	//Busco la key en todos los tmps que existan
	record_t* key_found_in_tmp =  search_in_all_tmps(table_directory,key);
	//Busco la key en la particion que deberia estar
	record_t* key_found_in_partition = search_in_partition(table_directory,key);
	//Comparo las 3 keys y por transitividad saco la que tiene la timestamp mas grande
	record_t* auxiliar_key = key_with_greater_timestamp(key_found_in_tmp,key_found_in_tmpc);

	record_t* most_current_key =  key_with_greater_timestamp(auxiliar_key,key_found_in_partition);

	//Devuelvo lo que encontre, si no esta la key entonces devuelvo una key con timestamp en -1
	free(key_found_in_tmpc);
	free(key_found_in_tmp);
	free(key_found_in_partition);
	//SI pongo este free pasa lo mismo, cuando hago un select va bien pero cuando hago otro seguido rompe
	//free(auxiliar_key);
	return most_current_key;

}

record_t* search_in_tmpc(char* table_directory, int key){
	record_t* key_found = malloc(sizeof(record_t));
	//Le seteo -1 para que si no la encuentra, devuelva esta "key invalida"
	key_found->timestamp = -1;
	if(exist_in_directory(get_tmpc_name(),table_directory)){
		key_found = search_key_in_fs_archive(get_tmpc_directory(table_directory), key);

	}
	//Al final devuelvo la key que encontre si es que habia o la key default con timestamp -1 si no estaba
	return key_found;
}


record_t* search_in_all_tmps(char* table_directory, int key){
	//ESta es la key que voy a devolver al final
	record_t* key_found_in_tmp = malloc(sizeof(record_t));
	key_found_in_tmp->timestamp = -1;
	//ESta key es para meter las keys que encuentro en cada tmp
	record_t* key_found = malloc(sizeof(record_t));
	//Los nombres de los tmp empiezan desde el 1
	int tmp_number=1;
	while( exist_in_directory(get_tmp_name(tmp_number),table_directory)){
		key_found = search_key_in_fs_archive(get_tmp_directory(table_directory,tmp_number), key);

		if(key_found->timestamp > key_found_in_tmp->timestamp){
			//Si la key que encontre tienen mas timestamp que la key que habia encontrado antes o la default
			//Entonces la guardo porque es la mas actual

			key_found_in_tmp = copy_key(key_found);
		}
		tmp_number++;
	}
	//Al final devuelvo la key con timestamp mas actual si la encontre o la key default con timestamp -1 si no
	free(key_found);
	return key_found_in_tmp;
}

record_t* search_in_partition(char* table_directory, int key) {
	//Leo la metadata de la tabla
	table_metadata_t* table_metadata = read_table_metadata(table_directory);
	int partition_number= (key%table_metadata->partitions)+1;
	//De nuevo la key default con timestamp en -1
	record_t* key_found = malloc(sizeof(record_t));
	key_found->timestamp = -1;
	key_found = search_key_in_fs_archive(create_partition_directory(table_directory,partition_number) , key);

	free(table_metadata);
	return key_found;
}



