#include "table_state_utils.h"

t_list* create_table_state_list(){
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
					table_state_t* table_state = malloc(sizeof(table_state_t));
					table_state->is_blocked_mutex = malloc(sizeof(sem_t));
					sem_init(table_state->is_blocked_mutex, 0,1);
					table_state->live_status_mutex = malloc(sizeof(sem_t));
					sem_init(table_state->live_status_mutex, 0,1);
					table_state->live_status_mutex = malloc(sizeof(sem_t));
					sem_init(table_state->select_mutex, 0,1);
					char* upper_read = to_uppercase(dir->d_name);
					table_state->name = upper_read;
					list_add(table_list, table_state);
				}
			}

			closedir(d);
		}
		free(directory);
		return table_list;
	}



