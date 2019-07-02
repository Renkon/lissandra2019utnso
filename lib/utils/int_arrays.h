#ifndef UTILS_INT_ARRAYS_H_
#define UTILS_INT_ARRAYS_H_

#include <string.h>
#include <stdlib.h>

void array_remove (int* arr, size_t size, size_t index, size_t rem_size);
int* array_take (int* arr, size_t size,size_t take_size);
int* int_array_concat(int* array1,int size1,int* array2,int size2);

#endif /* UTILS_INT_ARRAYS_H_ */
