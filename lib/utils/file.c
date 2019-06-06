#include "file.h"

t_list* get_file_lines(char* file) {
	FILE* stream;
	char* line;
	size_t len = 0;
	ssize_t read;
	t_list* lines;

	stream = fopen(file, "r");
	if (stream == NULL) {
		return NULL;
	}

	lines = list_create();

	while ((read = getline(&line, &len, stream)) != -1) {
		char* new_line = malloc(read);
		memcpy(new_line, line, read);
		memset(new_line + read - 1, '\0', 1);
		list_add(lines, new_line);
	}

	free(line);
	fclose(stream);
	return lines;
}
