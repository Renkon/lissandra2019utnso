#ifndef UTILS_STRING_H_
#define UTILS_STRING_H_

#include <stdbool.h>
#include <stddef.h>

char** string_split_ignore_quotes(char* input, char* delimiter, int* size);

#endif /* UTILS_STRING_H_ */
