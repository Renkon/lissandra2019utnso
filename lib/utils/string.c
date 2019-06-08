#include "string.h"

char** string_split_ignore_quotes(char* input, char* delimiter, int* size) {
	char** return_tokens = NULL;
	int token_size = 0;
	bool inside_quotes = false;
	char* cursor = input;
	char* initial = input;
	char* token = NULL;
	*size = 0;

	do {
		if ((cursor[0] == '\0' || cursor[0] == ' ') && !inside_quotes) {
			token = string_substring_until(initial, token_size);
			if (strlen(token) < 1)
				free(token);
			else {
				(*size)++;
				return_tokens = realloc(return_tokens, sizeof(char*) * (*size));
				return_tokens[*size - 1] = token;
			}
			token_size = 0;
			initial = cursor + 1;
		} else if (cursor[0] == '"') {
			if (!inside_quotes) {
				token_size = 0;
				initial = cursor + 1;
			}
			inside_quotes = !inside_quotes;
		} else {
			token_size++;
		}
	} while (cursor++[0] != '\0');

	return return_tokens;
}

uint16_t string_to_uint16(char* str) {
	return (uint16_t) string_to_long(str);
}

int string_to_int(char* str) {
	return (int) string_to_long(str);
}

long string_to_long(char* str) {
	return strtol(str, NULL, 10);
}

long long string_to_long_long(char* str) {
	return strtoll(str, NULL, 10);
}

char* to_uppercase(char* lower_string) {
	int i = 0;
	char* str_up = strdup(lower_string);

	while (str_up[i]) {
		if (str_up[i] >= 97 && str_up[i] <= 122)
			str_up[i] -= 32;
		i++;
	}

	return str_up;
}


