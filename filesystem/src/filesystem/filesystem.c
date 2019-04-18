#include "filesystem.h"

int create_table_folder(char* table_name) {
	//Creo el nuevo directorio donde va a estar la nueva tabla
	char *table_directory = create_new_directory(get_table_directory(),table_name);
	//Esta funcion es la que crea la carpeta
	return mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	//mkdir devuelve 0 is creo la carpeta y 1 si no lo hizo.
}

