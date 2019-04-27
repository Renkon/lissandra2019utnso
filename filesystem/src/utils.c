#include "utils.h"

char *get_table_directory() {
	//devuelve el directorio actual de las tablas
	char *table_path = malloc(
			strlen(g_config.mount_point) + strlen("Tables/") + 1);
	strcpy(table_path, g_config.mount_point);
	strcat(table_path, "Tables/");
	return table_path;

}
char *get_block_directory() {
	//devuelve el directorio actual de los bloques
	char *table_path = malloc(
			strlen(g_config.mount_point) + strlen("Bloques/") + 1);
	strcpy(table_path, g_config.mount_point);
	strcat(table_path, "Bloques/");
	return table_path;

}

char *get_bitmap_directory() {
	//devuelve el directorio actual de los bloques
	char *table_path = malloc(strlen(g_config.mount_point) + strlen("Metadata/Bitmap.bin") + 1);
	strcpy(table_path, g_config.mount_point);
	strcat(table_path, "Metadata/Bitmap.bin");
	return table_path;

}

char* create_new_directory(char* old_directory, char* directory_end) {
//Con esto creo nuevos directorios, mas que nada para crear de forma mas comoda los path a las nuevas tablas
	char *new_path = malloc(strlen(old_directory) + strlen(directory_end) + 1);
	strcpy(new_path, old_directory);
	strcat(new_path, directory_end);

	return new_path;
}

char* to_uppercase(char* lower_string) {

	{
		int i = 0;
		char *str_up = strdup(lower_string);

		while (str_up[i]) {
			if (str_up[i] >= 97 && str_up[i] <= 122)
				str_up[i] -= 32;
			i++;
		}
		free(lower_string);
		return (str_up);
	}
}


int exist_in_directory(char* archive, char* directory) {
	char* upper_archive = to_uppercase(archive);
	DIR *d;
	struct dirent *dir;
	d = opendir(directory);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			char* upper_read = to_uppercase(dir->d_name);
			if (strcmp(upper_read, upper_archive) == 0) {

				return 0;
			}
		}

		closedir(d);
	}
	return 1;
}

int digits_in_a_number(int number) {

	int totalDigits = 0;

	while (number != 0) {

		number = number / 10;
		totalDigits++;
	}

	return totalDigits;
}
