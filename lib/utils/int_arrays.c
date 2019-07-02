#include "int_arrays.h"

void array_remove (int* arr, size_t size, size_t index, size_t rem_size){
  int* begin = arr+index;
  int* end   = arr+index+rem_size;
  size_t trail_size = size-index-rem_size;
  memcpy(begin,end,trail_size*sizeof(int));
  memset(begin+trail_size,0,rem_size*sizeof(int));
}

//Este take toma los primeros n elementos y devuelve un nuevo array con ellos.
//El array de donde saca los elementos pierde estos n primeros elementos
int* array_take (int* arr, size_t size,size_t take_size){
	int* array = malloc(sizeof(int)*take_size);
	memcpy (array,arr,sizeof(int)*take_size);
	array_remove(arr,size,0,take_size);
	return array;
}


