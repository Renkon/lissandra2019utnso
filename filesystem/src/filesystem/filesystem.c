#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(),table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 is creo la carpeta y 1 si no lo hizo.
}

void create_partitions(int partitions, char* table_name){
	char buffer[100]={0}; // lo usaremos para guardar el nombre del fichero

	for (int i = 0; i < partitions; i++){
	   	sprintf(buffer, "%d.bin", i+1); // Ahora tenemos en buffer = "i+1.bin"
	   	FILE* arch = fopen(buffer, "w");
	   	fclose(arch);
	}
}
