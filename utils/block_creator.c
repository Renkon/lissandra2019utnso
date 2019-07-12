#include "block_creator.h"

typedef struct  {
	int block_size;
	int blocks;
	char* magic_number;
} metadata_t;

int main(void) {
	metadata_t* metadata = malloc(sizeof(metadata_t));

	metadata->block_size = 64; //parametrizar de aca
	metadata->blocks = 5192;
	metadata->magic_number = "LISSANDRA";
	//ESTO CAMBIA DEPENDIENDO LA PERSONA
	char* config_file ="/home/utnso/git/tp-2019-1c-Papito-code-en-Assembler/filesystem/filesystem.cfg";//hasta aca
	t_config* config = config_create(config_file);
	char* punto_montaje = config_get_string_value(config, "PUNTO_MONTAJE");
	char* directorio_metadata =string_new();
	string_append(&directorio_metadata, punto_montaje);
	string_append(&directorio_metadata,"Metadata/Metadata.bin");
	char* directorio_bloques = string_new();
	char* directorio_bloques2 = string_new();
	char* directorio_bitarray = string_new();
	string_append(&directorio_bloques, punto_montaje);
	string_append(&directorio_bloques, "Bloques/");
	strcpy(directorio_bloques2,directorio_bloques);
	string_append(&directorio_bitarray, punto_montaje);
	string_append(&directorio_bitarray, "Metadata/Bitmap.bin");

	FILE* arch = fopen(directorio_metadata, "wb");
	fwrite(&metadata->block_size, 1, sizeof(metadata->block_size), arch);
	fwrite(&metadata->blocks, 1, sizeof(metadata->blocks), arch);
	fwrite(metadata->magic_number, 1, strlen("LISSANDRAAAA"), arch);

	/*FILE* arch = fopen(directorio_metadata, "rb");
	fread(&metadata->block_size, 1, sizeof(metadata->block_size), arch);
	fread(&metadata->blocks, 1, sizeof(metadata->blocks), arch);
	fread(metadata->magic_number, 1, strlen("LISSANDRA"), arch);*/
	fclose(arch);
	char* bloque = malloc(6);

	for(int i =0; i<metadata->blocks;i++){
		sprintf(bloque, "%d.bin", i+1);
		string_append(&directorio_metadata, punto_montaje);
		string_append(&directorio_bloques,bloque);
		FILE* arch = fopen(directorio_bloques, "wb");
		fclose(arch);
		strcpy(directorio_bloques,directorio_bloques2);
	}

	char* bitmap = malloc(metadata->blocks);
	for(int i =0; i<(metadata->blocks)/8;i++){
		bitmap[i] = 0;
	}
	size_t size = (metadata->blocks)/8;
	t_bitarray* bitarray = bitarray_create_with_mode(bitmap,size,LSB_FIRST);
	FILE* arch2 = fopen(directorio_bitarray, "wb");
	fwrite(&bitarray->mode, 1, sizeof(bit_numbering_t), arch2);
	fwrite(&bitarray->size, 1, sizeof(size_t), arch2);
	fwrite(bitarray->bitarray,bitarray->size,1, arch2);

	fclose(arch2);

	return EXIT_SUCCESS;
}



