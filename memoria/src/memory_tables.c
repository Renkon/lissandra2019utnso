/*
 * memory_tables.c
 *
 *  Created on: 9 jun. 2019
 *      Author: utnso
 */

#include "memory_tables.h"

segment_t* get_segment_by_name(t_list* list, char* table_name) {
	 int i = 0;
	 segment_t* segment_found;

	 for(;i < list_size(list); i++){
	 	segment_found = list_get(list, i);
	 	if (strcmp(segment_found->name, table_name) == 0) {
	 		break;
	 	}
	 }

	 return i < list_size(list) ? segment_found : NULL;
}

page_t* get_page_by_key(segment_t* segment, t_list* index_list, int key) {
	page_t* page_found;
	int index;
	char* str_key = string_itoa(key);

	int i = 0;
	for(; i < list_size(index_list); i++){
	 	index = list_get(index_list, i);
	 	char* comparator = main_memory_values(index,KEY);
	 	if (strcmp(comparator, str_key) == 0){
		 	free(comparator);
	 		break;
	 	}

	 	free(comparator);
	}

	free(str_key);

	if (i < list_size(index_list)) {
		page_found = get_page_by_index(segment, index);
		return page_found;
	} else {
		return NULL;
	}
}

int page_get_index(page_t* page){
	return page->index;
}

page_t* get_page_by_index(segment_t* segment, int index) {
	page_t* page_found;
	t_list* pages = segment->page;

	for(int i = 0; i < list_size(pages); i++) {
		page_found = list_get(pages, i);

		if (page_found->index == index) {
			return i < list_size(pages) ? page_found : NULL;
	 	}
	}

	return NULL;
}

page_t* create_page(int index, bool modified ) {
	page_t* page = malloc(sizeof(page_t));

	page->index = index;
	page->modified = modified;

	return page;
}

segment_t* create_segment(char* table_name) {
  segment_t* segment = malloc(sizeof(segment_t));

  segment->page = list_create();
  segment->name = malloc(strlen(table_name)+1);
  strcpy(segment->name,table_name);

  return segment;
}
