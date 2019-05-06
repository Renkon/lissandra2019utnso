#ifndef UTILS_STRING_H_
#define UTILS_STRING_H_

#include "commons/string.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

char** string_split_ignore_quotes(char* input, char* delimiter, int* size);
uint16_t string_to_uint16(char* str);
int string_to_int(char* str);
long string_to_long(char* str);
long long string_to_long_long(char* str);

#endif /* UTILS_STRING_H_ */
