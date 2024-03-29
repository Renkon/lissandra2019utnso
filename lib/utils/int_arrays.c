#include "int_arrays.h"


void arrayReverse(int arr[], int size)
{
    for (int i = 0; i < size/2; ++i)
    {
        int temp = arr[i];
        arr[i] = arr[size - 1 - i];
        arr[size - 1 - i] = temp;
    }
}

void array_remove (int* arr, size_t size, size_t rem_size){
	arrayReverse(arr,size);
	arr =realloc(arr,sizeof(int)*(size-rem_size));
	arrayReverse(arr,size-rem_size);
}

//Este take toma los primeros n elementos y devuelve un nuevo array con ellos.
//El array de donde saca los elementos pierde estos n primeros elementos
int* array_take (int* arr, int size,int take_size){
	if(size==take_size){
		return arr;
	}else{
	int* array = malloc(sizeof(int)*take_size);
	array = memmove(array,arr,sizeof(int)*take_size);
	array_remove(arr,size,take_size);
	return array;}
}

int* int_array_concat(int* array1,int size1,int* array2,int size2){
	int* total = malloc(sizeof(int)*(size1+size2));
	memcpy(total,array1,sizeof(int)*size1);
	memcpy(total+size1,array2,sizeof(int)*size2);
	free(array1);
	return total;
}
