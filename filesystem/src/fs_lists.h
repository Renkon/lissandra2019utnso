#ifndef FS_LISTS_H_
#define FS_LISTS_H_

#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <commons/collections/list.h>

typedef struct {
	char*	name;
	t_list*	tkvs;

}table_t;

extern char* possible_table_name;
bool table_not_exist_in_list(t_list* list, char* table_name);
bool table_exist(table_t* table);
void assing_name(char* new_name);
#endif /* FS_LISTS_H_ */



