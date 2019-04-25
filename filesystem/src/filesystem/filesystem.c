#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(),
			table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 is creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name) {
	char* table_directory = create_new_directory(get_table_directory(),table_name);

	for(int i = 0; i < partitions; i++) {
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

void create_metadata(consistency_t consistency, int partitions, long compaction_time, char* table_name){
	char* table_directory = create_new_directory(get_table_directory(),table_name);
	char* metadata_name = malloc(strlen("/metadata.txt") + 1);

	sprintf(metadata_name, "/metadata.txt");
	char* partition = malloc(strlen(table_directory) + strlen(metadata_name) + 1);
	strcpy(partition, table_directory);
	strcat(partition, metadata_name); //aca tengo toda la direccion y termina con metadata.txt

	FILE* arch = fopen(partition, "w");

	//Escribo logica para modificar txt:

	fputs("CONSISTENCY = ", arch);
	fprintf(arch, "%i\n", consistency);


	fputs("PARTITIONS = ", arch);
	fprintf(arch, "%d\n", partitions);


	fputs("COMPACTION_TIME = ", arch);
	fprintf(arch, "%d\n", compaction_time);

	fclose(arch);
	free(metadata_name);
	free(partition);
	free(table_directory);

}
