#include "create_utils.h"

void create_table_folder(char* table_name) {
	char *table_directory = create_table_path(table_name);
	//Esta funcion es la que crea la carpeta
	mkdir(table_directory, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	for (int i = 0; i < sizeof(table_directory); i++) {
			free(table_directory[i]);
		}

		free(table_directory);

}

char *create_table_path(char* table_name) {

	char *table_path = calloc(200, sizeof(char));
	strcpy(table_path, g_config.mount_point);
	strcat(table_path, "Tables/");
	strcat(table_path, table_name);
	return table_path;

}



