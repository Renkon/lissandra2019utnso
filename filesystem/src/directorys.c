#include "directorys.h"

char* create_new_directory(char* old_directory, char* directory_end) {
	//Con esto creo nuevos directorios, mas que nada para crear de forma mas comoda los path a las nuevas tablas
	int size_old_directory =strlen(old_directory);
	int size_directory_end =strlen(directory_end);
	char* new_path = malloc(size_old_directory + size_directory_end + 1);
	strcpy(new_path, old_directory);
	strcat(new_path, directory_end);
	return new_path;
}

char* get_table_directory() {
	//devuelve el directorio actual de las tablas
	return create_new_directory(g_config.mount_point, "Tables/");

}
char* get_block_directory() {
	//devuelve el directorio actual de los bloques
	return create_new_directory(g_config.mount_point, "Bloques/");
}

char* get_bitmap_directory() {
	//devuelve el directorio actual de los bloques
	return create_new_directory(g_config.mount_point, "Metadata/Bitmap.bin");
}

char* get_tmpc_directory(char* table_directory) {
	char* table_directory_with_slash = create_new_directory(table_directory, "/");
	char* new_dir = create_new_directory(table_directory_with_slash, get_tmpc_name);
	free(table_directory_with_slash);
	return new_dir;
}

char* get_tmp_name(int tmp_number) {
	char* tmp_name = malloc(digits_in_a_number(tmp_number) + strlen("A.tmp") + 1);
	sprintf(tmp_name, "A%d.tmp", tmp_number);
	return tmp_name;
}

char* get_tmp_directory(char* table_directory, int tmp_number) {
	char* table_directory_with_slash = create_new_directory(table_directory, "/");
	char* tmp_name = get_tmp_name(tmp_number);
	char* new_dir = create_new_directory(table_directory_with_slash, tmp_name);
	free(table_directory_with_slash);
	free(tmp_name);
	return new_dir;
}


char* create_metadata_directory(char* table_directory) {
	return create_new_directory(table_directory, "/metadata.bin");
}

char* create_partition_directory(char* table_directory, int partition_number){
	char* partition_name = create_partition_name(partition_number);
	char* new_dir = create_new_directory(table_directory, partition_name);
	free(partition_name);
	return new_dir;
}

char* create_partition_name(int partition_number) {
	char* partition = malloc(digits_in_a_number(partition_number) + strlen("/.bin") + 1);
	sprintf(partition, "/%d.bin", partition_number);
	return partition;
}

char* create_block_directory(int block_number) {
	char* block_name = malloc(digits_in_a_number(block_number) + strlen(".bin")+1);
	sprintf(block_name, "%d.bin", block_number);
	char* block_dir = get_block_directory();
	char* new_dir = create_new_directory(block_dir, block_name);
	free(block_name);
	free(block_dir);
	return new_dir;
}

bool exist_in_directory(char* archive, char* directory) {
	char* upper_archive = to_uppercase(archive);
	DIR* d;
	struct dirent* dir;
	d = opendir(directory);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			char* upper_read = to_uppercase(dir->d_name);
			if (strcmp(upper_read, upper_archive) == 0) {
				free(upper_read);
				free(upper_archive);
				closedir(d);
				return true;
			}
			free(upper_read);
		}

		closedir(d);
	}
	free(upper_archive);
	return false;
}

t_list* get_tables_list(){
	char* directory = get_table_directory();
	DIR* d;
	struct dirent* dir;
	t_list* table_list = list_create();
	d = opendir(directory);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			//*Si lo que lei no es . o .. entonces lo meto a la lista de  tablas
			//No se porque pero me lee esos caracteres, por eso los ignoro (?
			if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
				char* upper_read = to_uppercase(dir->d_name);
				list_add(table_list, upper_read);
			}
		}

		closedir(d);
	}
	free(directory);
	return table_list;
}

//Para borrar la carpeta
int remove_directory(char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d)
   {
      struct dirent *p;

      r = 0;

      while (!r && (p=readdir(d)))
      {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
          {
             continue;
          }

          len = path_len + strlen(p->d_name) + 2;
          buf = malloc(len);

          if (buf)
          {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);

             if (!stat(buf, &statbuf))
             {
                if (S_ISDIR(statbuf.st_mode))
                {
                   r2 = remove_directory(buf);
                }
                else
                {
                   r2 = unlink(buf);
                }
             }

             free(buf);
          }

          r = r2;
      }

      closedir(d);
   }

   if (!r)
   {
      r = rmdir(path);
   }

   return r;
}

metadata_t* read_fs_metadata(){
	char* metadata_directory =create_new_directory(g_config.mount_point,"Metadata/Metadata.bin");
	metadata_t* metadata = malloc(sizeof(metadata_t));
	metadata->magic_number = malloc(strlen("LISSANDRA")+1);

	FILE* arch = fopen(metadata_directory, "rb");
	fread(&metadata->block_size, 1, sizeof(metadata->block_size), arch);
	fread(&metadata->blocks, 1, sizeof(metadata->blocks), arch);
	fread(metadata->magic_number, 1, strlen("LISSANDRA")+1, arch);
	free(metadata_directory);
	return metadata;
}


FILE* open_block(int block){
	char* block_directory = create_block_directory(block);
	FILE* the_block = fopen(block_directory, "wb");
	free(block_directory);
	return the_block;
}

void write_tkv(char* tkv,FILE* block){
	//Si me entra el tkv en el bloque lo meto asi nomas.
	fwrite(tkv,1,strlen(tkv)+1,block);
}
