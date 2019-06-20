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

int get_position_by_index(t_list* page_list, int index){
	int i;
	page_t* page;

	for(; i < list_size(page_list); i++){
		page = list_get(page_list,i);
		if(page->index == index){
			break;
		}
	}
	return i < list_size(page_list)?i:-1;
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

t_list* get_pages_by_modified(bool modified){
	segment_t* segment;
	t_list* page_list;
	page_t* page;
	t_list* modified_pages_list = list_create();

	for(int i = 0; i < list_size(g_segment_list); i++){
		segment = list_get(g_segment_list,i);
		page_list = segment->page;

		for(int j = 0; j < list_size(page_list); j++){
			page = list_get(page_list,j);

			if(page->modified == modified){
				list_add(modified_pages_list,page);
			}
		}
	}

	free(segment);
	free(page_list);

	return modified_pages_list;
}

void destroy_page(page_t* page){
	free(page);
}

void remove_segment(segment_t* segment){

	t_list* indexes = list_map(segment->page,(void*) page_get_index);
	int index;

	for(int i = 0; i < list_size(indexes); i++){
		index = list_get(indexes,i);
		strcpy(main_memory[index],"null");
	}

	list_destroy_and_destroy_elements(segment->page,(void*)destroy_page);

	free(indexes);

	free(segment);
}

