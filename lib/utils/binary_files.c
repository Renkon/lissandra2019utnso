#include "binary_files.h"

t_bitarray *read_bitmap(char* bitmap_directory) {
	t_bitarray *bitmap = malloc(sizeof(t_bitarray));

	FILE* arch2 = fopen(bitmap_directory, "rb");
	fread(&bitmap->mode, 1, sizeof(bitmap->mode), arch2);
	fread(&bitmap->size, 1, sizeof(bitmap->size), arch2);
	bitmap->bitarray = malloc(sizeof(char) * bitmap->size);
	fread(bitmap->bitarray, 1, sizeof(bitmap->size), arch2);

	return bitmap;
}

void write_bitmap(t_bitarray *bitmap, char* bitmap_directory) {
	FILE* arch = fopen(bitmap_directory, "wb");
	fwrite(&bitmap->mode, 1, sizeof(bitmap->mode), arch);
	fwrite(&bitmap->size, 1, sizeof(bitmap->size), arch);
	fwrite(bitmap->bitarray, 1, sizeof(bitmap->size), arch);

	fclose(arch);

}
