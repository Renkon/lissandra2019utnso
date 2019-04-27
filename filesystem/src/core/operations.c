#include "operations.h"

void process_select(select_input_t* input) {
	log_i("fs select args: %s %u", input->table_name,(unsigned int) input->key);
}

void process_insert(insert_input_t* input) {
	log_i("fs insert args: %s %u \"%s\" %ld", input->table_name,(unsigned int) input->key, input->value, input->timestamp);
}

void process_create(create_input_t* input) {
	log_i("fs create args: %s %i %i %ld", input->table_name, input->consistency,input->partitions, input->compaction_time);
	char* table_name_upper = to_uppercase(input->table_name);
	t_bitarray *bitmap = read_bitmap(get_bitmap_directory());
	int blocks[(input->partitions)];

	//Quiero saber si hay tantos bloques libres como particiones asi que busco cuantos bloques libres hay


	//Insertar una forma que haga que se rompa todo si el partitions > la cantidad de bloques. Intentemos no usar otro if
	int free_blocks_amount = assign_free_blocks(bitmap, blocks,input->partitions);



	if (free_blocks_amount == input->partitions) {
		//Si habia n bloques libres me toca saber si ya existe la tabla a crear o no
		if (create_table_folder(table_name_upper) == 0) {


			//SI no existe la tabla entonces procedo con el CREATE normlamente
			log_i("fs> Se creo la tabla %s ", table_name_upper);

			create_table_metadata(input->consistency, input->partitions,input->compaction_time, table_name_upper);
			log_i("fs> Se creo la metadata de la tabla %s ", table_name_upper);

			create_partitions(input->partitions, table_name_upper,blocks);
			write_bitmap(bitmap,get_bitmap_directory());
			log_i("fs> Se crearon %d particiones para la tabla %s ",input->partitions, table_name_upper);

			free(table_name_upper);
			free(bitmap);

		} else {

			log_i("fs> La carpeta %s ya esta en el sistema. Operacion CREATE cancelada.",table_name_upper);

			free(table_name_upper);
			free(bitmap);
		}

	} else {
		//SI no habia tantos bloques libres como particiones quiero tener, entonces cancelo el CREATE
		log_i("fs> No hay bloques suficientes como para crear la tabla %s con %d particiones. Operacion CREATE cancelada",input->table_name,input->partitions);

		free(table_name_upper);
		free(bitmap);
	}

}

void process_describe(describe_input_t* input) {
	log_i("fs describe args: %s", input->table_name);
}

void process_drop(drop_input_t* input) {
	log_i("fs drop args: %s", input->table_name);
}
