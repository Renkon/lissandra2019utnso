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
	int i = 0;
	page_t* page;

	for(; i < page_list->elements_count; i++){
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

  char* copia = malloc(strlen(table_name)+1);
  strcpy(copia,table_name);
  string_to_upper(copia);

  segment->page = list_create();
  segment->name = malloc(strlen(copia)+1);
  strcpy(segment->name,copia);

  free(copia);
  return segment;
}

t_list* get_pages_by_modified(bool modified){
	t_list* page_list;
	t_list* modified_pages_list = list_create();

	for(int i = 0; i < g_segment_list->elements_count; i++){
		segment_t* segmento = list_get(g_segment_list,i);
		page_list = segmento->page;

		for(int j = 0; j < page_list->elements_count; j++){
			page_t* page = list_get(page_list,j);

			if(page->modified == modified){
				list_add(modified_pages_list,page);
			}
		}
	}

	return modified_pages_list;
}

void destroy_page(page_t* page){
	free(page);
}

void destroy_segment(segment_t* segment){
	free(segment->name);
	list_destroy_and_destroy_elements(segment->page,(void*)destroy_page);
	free(segment);
}

int get_segment_position_by_name(char* segment_name){
	int i=0;
	segment_t* segment;

	for(;i < list_size(g_segment_list);i++){
		segment = list_get(g_segment_list,i);
		if(strcmp(segment_name,segment->name)==0){
			break;
		}
	}

	return i < list_size(g_segment_list)?i:-1;
}

void remove_segment(segment_t* segment){

	t_list* indexes = list_map(segment->page,(void*) page_get_index);
	int index;
	int position;

	for(int i = 0; i < list_size(indexes); i++){
		index = list_get(indexes,i);
		strcpy(g_main_memory+index,"null");
	}

	list_destroy(indexes);

	position = get_segment_position_by_name(segment->name);
	list_remove_and_destroy_element(g_segment_list,position,(void*)destroy_segment);
}

bool segment_has_page_by_index(segment_t* segment, int index){
	int i=0;
	t_list* page_list = segment->page;
	page_t* comp_page;

	for(;i < list_size(page_list);i++){
		comp_page = list_get(page_list,i);
		if(comp_page->index == index){
			break;
		}
	}

	return i<list_size(page_list)?true:false;
}

char* get_table_name_by_index(int index_in_memory){
	segment_t* segment;
	int i = 0;
	for(;i < list_size(g_segment_list); i++){
		segment = list_get(g_segment_list,i);
		if(segment_has_page_by_index(segment,index_in_memory)){
			break;
		}
	}

	if( i < list_size(g_segment_list)){
		return segment->name;
	}else{
		return NULL;
	}
}

bool page_modified(page_t* page){
	return page->modified == true;
}

void remove_pages_modified(){
	int i = 0;
	segment_t* segment;

	for(;i < list_size(g_segment_list);i++){
		segment = list_get(g_segment_list,i);

		list_remove_and_destroy_by_condition(segment->page,(bool*)page_modified,(void*)destroy_page);
	}
}

segment_t* get_segment_by_index_global(int index){
	for (int i = 0; i < list_size(g_segment_list); i++){
		segment_t* segment = list_get(g_segment_list, i);

		for (int j = 0; j < list_size(segment->page); j++){
			page_t* page = list_get(segment->page, j);

			if (page->index == index){
				return segment;
			}
		}
	}
	return NULL;
}
