#ifndef UTILS_INT_ARRAYS_H_
#define UTILS_INT_ARRAYS_H_

#include <string.h>

void array_remove (int* arr, size_t size, size_t index, size_t rem_size);
int* array_take (int* arr, size_t size,size_t take_size);

#endif /* UTILS_INT_ARRAYS_H_ */
