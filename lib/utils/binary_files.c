#include "binary_files.h"

t_bitarray* read_bitmap(char* bitmap_directory) {
	bit_numbering_t mode;
	size_t size;
	FILE* arch2 = fopen(bitmap_directory, "rb");
	fread(&mode, 1, sizeof(bit_numbering_t), arch2);
	fread(&size, 1, sizeof(size_t), arch2);
	char* bitarray = malloc(size*8);
	fread(bitarray,1,size*8, arch2);
	fclose(arch2);
	t_bitarray* bitmap = bitarray_create_with_mode(bitarray,size,LSB_FIRST);
	return bitmap;
}

void write_bitmap(t_bitarray *bitmap, char* bitmap_directory) {
	FILE* arch = fopen(bitmap_directory, "wb");
	fwrite(&bitmap->mode, 1, sizeof(bitmap->mode), arch);
	fwrite(&bitmap->size, 1, sizeof(bitmap->size), arch);
	fwrite(bitmap->bitarray, 1,bitmap->size, arch);
	fclose(arch);
}










