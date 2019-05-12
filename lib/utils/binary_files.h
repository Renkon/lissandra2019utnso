#ifndef UTILS_BINARY_FILES_H_
#define UTILS_BINARY_FILES_H_
#include <string.h>
#include<stdio.h>
#include<commons/bitarray.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

t_bitarray *read_bitmap(char* binary_directory);
void write_bitmap(t_bitarray *bitmap, char* bitmap_directory);

#endif /* UTILS_BINARY_FILES_H_ */
