#include "directorys.h"


//No se porque no me deja ponerla como variable global en directorys.h asi que la hice funcion y fue.
char* get_tmpc_name() { return "A1.tmpc";};

char* create_new_directory(char* old_directory, char* directory_end) {
//Con esto creo nuevos directorios, mas que nada para crear de forma mas comoda los path a las nuevas tablas
	char* new_path = malloc(strlen(old_directory) + strlen(directory_end) + 1);
	strcpy(new_path, old_directory);
	strcat(new_path, directory_end);
	return new_path;
}

char* get_table_directory() {
	//devuelve el directorio actual de las tablas
	return create_new_directory(g_config.mount_point,"Tables/");

}
char* get_block_directory() {
	//devuelve el directorio actual de los bloques
	return create_new_directory(g_config.mount_point,"Bloques/");
}

char* get_bitmap_directory() {
	//devuelve el directorio actual de los bloques
	return create_new_directory(g_config.mount_point,"Metadata/Bitmap.bin");
}

char* get_tmpc_directory(char* table_directory) {
	char* table_directory_with_slash = create_new_directory(table_directory, "/");
	return create_new_directory(table_directory_with_slash, get_tmpc_name());
}

char* get_tmp_name(int tmp_number) {
	char* tmp_name = malloc(digits_in_a_number(tmp_number) + strlen("A.tmp") + 1);
	sprintf(tmp_name, "A%d.tmp", tmp_number);
	return tmp_name;
}

char* get_tmp_directory (char*  table_directory,int tmp_number) {
	char* table_directory_with_slash = create_new_directory(table_directory, "/");
	return create_new_directory(table_directory_with_slash,get_tmp_name(tmp_number));
}


char* create_metadata_directory(char* table_directory) {
	return  create_new_directory(table_directory, "/metadata.bin");
}

char* create_partition_directory(char* table_directory, int partition_number){

	return  create_new_directory(table_directory, create_partition_name(partition_number));

}

char* create_partition_name(int partition_number) {
	char* partition = malloc(digits_in_a_number(partition_number) + strlen("/.bin") + 1);
	sprintf(partition, "/%d.bin", partition_number);
	return partition;

}

char* create_block_directory (int block_number){

	char* block_name= malloc(digits_in_a_number(block_number) + strlen(".bin") + 1);
	sprintf(block_name, "%d.bin", block_number);
	return create_new_directory(get_block_directory(),block_name);
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
				closedir(d);
				return true;
			}
		}

		closedir(d);
	}
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
			if(strcmp(dir->d_name,".") != 0 && strcmp(dir->d_name,"..")!=0){
			char* upper_read = to_uppercase(dir->d_name);
			list_add(table_list, upper_read);
			}
		}

		closedir(d);
	}
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
