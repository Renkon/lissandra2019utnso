#include "binary_files.h"

t_bitarray* read_bitmap(char* bitmap_directory) {
	t_bitarray *bitmap = malloc(sizeof(t_bitarray));

	FILE* arch2 = fopen(bitmap_directory, "rb");
	fread(&bitmap->mode, sizeof(bitmap->mode), 1, arch2);
	fread(&bitmap->size, sizeof(bitmap->size), 1, arch2);
	bitmap->bitarray = malloc(sizeof(char) * bitmap->size);
	fread(bitmap->bitarray, sizeof(bitmap->size), 1, arch2);

	return bitmap;
}

void write_bitmap(t_bitarray *bitmap, char* bitmap_directory) {
	FILE* arch = fopen(bitmap_directory, "wb");
	fwrite(&bitmap->mode, sizeof(bitmap->mode), 1, arch);
	fwrite(&bitmap->size, sizeof(bitmap->size), 1, arch);
	fwrite(bitmap->bitarray, sizeof(bitmap->size), 1, arch);

	fclose(arch);

}







