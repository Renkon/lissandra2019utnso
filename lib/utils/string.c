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
