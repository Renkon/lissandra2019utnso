#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(),table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 si creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);

	for (int i = 0; i < partitions; i++) {
		char* bin_name = malloc(digits_in_a_number(partitions) + strlen("/.bin") + 1);
		sprintf(bin_name, "/%d.bin", i + 1); //Esto transforma de int a string
		char* partition = malloc(strlen(table_directory) + strlen(bin_name) + 1);
		strcpy(partition, table_directory);
		strcat(partition, bin_name); //aca tengo toda la direccion y termina con n.bin n= nombre archivo

		FILE* arch = fopen(partition, "wb");
		//insertar logica para asignar bloques

		fclose(arch);
		free(bin_name);
		free(partition);
	}

	free(table_directory);

}

void create_table_metadata(consistency_t consistency, int partitions,long compaction_time, char* table_name) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);
	char* metadata_name = "/metadata.bin";
	char* metadata_directory = malloc(strlen(table_directory) + strlen(metadata_name) + 1);
	Table_metadata *table_metadata = malloc(sizeof(Table_metadata));
	//con esto ya tendria toda la direccion donde va a estar la metadata
	metadata_directory =create_new_directory(table_directory,metadata_name);
	//Creo el struct metadata a guardar con los datos del input
	printf(metadata_directory);
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
