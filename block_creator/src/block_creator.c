#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commons/collections/list.h"
#include "commons/bitarray.h"
#include "commons/string.h"

typedef struct {
	int block_size;
	int blocks;
	char* magic_number;
} metadata_t;

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Se enviaron %i args. Esperados: [block_size] [blocks] [magic_number] [mount_point]\n", argc);
        return 1;
    }

    // Parametros
    // 0: block_size
    // 1: blocks
    // 2: magic_number
    // 3: mount_point
	metadata_t* metadata = malloc(sizeof(metadata_t));

	metadata->block_size = atoi(argv[1]);
	metadata->blocks = atoi(argv[2]);
	metadata->magic_number = strdup(argv[3]);
	char* punto_montaje = strdup(argv[4]);

	char* directorio_metadata = string_new();
	string_append(&directorio_metadata, punto_montaje);
	string_append(&directorio_metadata,"Metadata/Metadata.bin");
	char* directorio_bloques = string_new();
	char* directorio_bitarray = string_new();
	string_append(&directorio_bloques, punto_montaje);
	string_append(&directorio_bloques, "Bloques/");
	string_append(&directorio_bitarray, punto_montaje);
	string_append(&directorio_bitarray, "Metadata/Bitmap.bin");
	char* directorio_bloques2 = strdup(directorio_bloques);

	FILE* arch = fopen(directorio_metadata, "wb");
	fwrite(&metadata->block_size, 1, sizeof(metadata->block_size), arch);
	fwrite(&metadata->blocks, 1, sizeof(metadata->blocks), arch);
	fwrite(metadata->magic_number, 1, strlen("LISSANDRAAAA"), arch);

	fclose(arch);
	char* bloque = malloc(6);

	for (int i = 0; i < metadata->blocks; i++) {
		sprintf(bloque, "%d.bin", i+1);
		string_append(&directorio_metadata, punto_montaje);
		string_append(&directorio_bloques, bloque);
		FILE* arch = fopen(directorio_bloques, "wb");
		fclose(arch);
		strcpy(directorio_bloques, directorio_bloques2);
	}

	char* bitmap = malloc(metadata->blocks);
	for(int i = 0; i < metadata->blocks / 8; i++) {
		bitmap[i] = 0;
	}

	size_t size = metadata->blocks / 8;
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap, size, LSB_FIRST);
	FILE* arch2 = fopen(directorio_bitarray, "wb");
	fwrite(&bitarray->mode, 1, sizeof(bit_numbering_t), arch2);
	fwrite(&bitarray->size, 1, sizeof(size_t), arch2);
	fwrite(bitarray->bitarray, bitarray->size, 1, arch2);

	fclose(arch2);

	return EXIT_SUCCESS;
}

