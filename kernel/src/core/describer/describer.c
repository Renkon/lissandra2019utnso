#include "describer.h"

void init_describer() {
	pthread_t describer_thread;

	// Este semaforo se usa para que lo del post describe se haga una sola vez.
	sem_init(&g_mutex_user_describe, 0, 1);

	g_table_metadata = list_create();

	if (pthread_create(&describer_thread, NULL, (void*) update_describer_continuously, NULL)) {
		log_e("No se pudo inicializar el hilo de describe");
	}
}

void update_describer_continuously() {
	pthread_detach(pthread_self());

	while (true) {
		usleep(g_config.metadata_refresh * 1000);
		update_descriptions();
	}
}

void update_descriptions() {
	describe_input_t* input = malloc(sizeof(describe_input_t));
	input->table_name = NULL;

	memory_t* memory = get_random_memory(true);
	if (memory == NULL) {
		log_e("No se puede disparar describe automatizado porque no hay memorias vivas");
		free(input);
		return;
	}

	do_simple_request(KERNEL, memory->ip, memory->port, DESCRIBE_IN, input,
				0, NULL, update_descriptions_callback, true, free_auto_describe_input, NULL);
}

void update_descriptions_callback(void* result, response_t* response) {
	t_list* new_metadata = (t_list*) result;
	on_post_describe(new_metadata, false, true);
}

void on_describe_update_triggered(void* untyped_args) {
	pthread_detach(pthread_self());
	sem_wait(&g_mutex_user_describe);
	update_metadata_args_t* args = (update_metadata_args_t*) untyped_args;

	// Caso individual. Me fijo si es insert (upsert = true), o delete.
	if (args->single) {
		table_metadata_t* affected_metadata = (table_metadata_t*) list_get(args->metadata_list, 0);
		if (args->upsert) { // Es insert o update?
			// Me fijo que no exista
			int found = -1;
			for (int i = 0; i < list_size(g_table_metadata); i++) {
				table_metadata_t* existing_metadata = list_get(g_table_metadata, i);
				if (string_equals_ignore_case(affected_metadata->table_name, existing_metadata->table_name)) {
					found = i;
					break;
				}
			}

			if (found == -1) {
				list_add(g_table_metadata, affected_metadata);
			} else {
				table_metadata_t* new_metadata = list_get(g_table_metadata, found);
				free(new_metadata->table_name);
				new_metadata->table_name = affected_metadata->table_name;
				new_metadata->compaction_time = affected_metadata->compaction_time;
				new_metadata->consistency = affected_metadata->consistency;
				new_metadata->partitions = affected_metadata->partitions;
				free(affected_metadata);
			}

		} else { // Es delete!
			// Bueno, lo borro!
			for (int i = 0; i < list_size(g_table_metadata); i++) {
				table_metadata_t* existing_metadata = (table_metadata_t*) list_get(g_table_metadata, i);
				if (string_equals_ignore_case(affected_metadata->table_name, existing_metadata->table_name)) {
					// Ok. Encontramos la entrada. Borramos la metadata.
					free(existing_metadata->table_name);
					free(existing_metadata);
					list_remove(g_table_metadata, i);
					free(affected_metadata->table_name);
					free(affected_metadata);
					break;
				}
			}
		}
	} else {
		// Un caso general. Aca limpiamos al carajo lo q habia en el describe
		for (int i = list_size(g_table_metadata) -1; i >= 0; i--) {
			table_metadata_t* metadata = list_get(g_table_metadata, i);
			free(metadata->table_name);
			free(metadata);
			list_remove(g_table_metadata, i);
		}

		// Ahora agrego los nuevos.
		for (int i = 0; i < list_size(args->metadata_list); i++)
			list_add(g_table_metadata, (table_metadata_t*) list_get(args->metadata_list, i));
	}

	list_destroy(args->metadata_list);
	free(args);
	display_metadata();
	sem_post(&g_mutex_user_describe);
}

void on_post_describe(t_list* metadata_list, bool single, bool upsert) {
	if (metadata_list == NULL) {
		// Me viene una lista vacia
		// No puedo hacer el describe
		log_e("No puedo actualizar metadatos porque fallo la solicitud de DESCRIBE");
		return;
	}

	// Debo clonar la lista para mandarla
	update_metadata_args_t* args = malloc(sizeof(update_metadata_args_t));
	t_list* new_metadata_list = list_create();
	table_metadata_t* old_meta;
	table_metadata_t* new_meta;
	pthread_t updater_thread;

	for (int i = 0; i < list_size(metadata_list); i++) {
		old_meta = (table_metadata_t*) list_get(metadata_list, i);
		new_meta = malloc(sizeof(table_metadata_t));
		new_meta->compaction_time = old_meta->compaction_time;
		new_meta->consistency = old_meta->consistency;
		new_meta->partitions = old_meta->partitions;
		new_meta->table_name = strdup(old_meta->table_name);

		list_add(new_metadata_list, new_meta);
	}

	args->metadata_list = new_metadata_list;
	args->single = single;
	args->upsert = upsert;

	if (pthread_create(&updater_thread, NULL, (void*) on_describe_update_triggered, (void*) args)) {
		log_e("No se pudo inicializar el hilo de actualizacion de describe");
		for (int i = 0; i < list_size(new_metadata_list); i++) {
			table_metadata_t* metadata = (table_metadata_t*) list_get(new_metadata_list, i);
			free(metadata->table_name);
			free(metadata);
		}
		list_destroy(new_metadata_list);
		free(args);
	}
}

void on_post_create(create_input_t* input) {
	t_list* metadata_list = list_create();
	table_metadata_t* metadata = malloc(sizeof(table_metadata_t));
	metadata->compaction_time = input->compaction_time;
	metadata->consistency = input->consistency;
	metadata->partitions = input->partitions;
	metadata->table_name = strdup(input->table_name);

	list_add(metadata_list, metadata);
	on_post_describe(metadata_list, true, true);
	free(metadata->table_name);
	free(metadata);
	list_destroy(metadata_list);
}

void on_post_drop(drop_input_t* input) {
	t_list* metadata_list = list_create();
	table_metadata_t* metadata = malloc(sizeof(table_metadata_t));
	metadata->table_name = strdup(input->table_name);

	list_add(metadata_list, metadata);
	on_post_describe(metadata_list, true, false);
	free(metadata->table_name);
	free(metadata);
	list_destroy(metadata_list);
}

void display_metadata() {
	int tables = list_size(g_table_metadata);

	log_t("En la metadata actualmente hay %i tablas", tables);

	for (int i = 0; i < tables; i++) {
		table_metadata_t* metadata = list_get(g_table_metadata, i);
		log_t("--- Metadata de %s ---", metadata->table_name);
		log_t("  -> Tiempo de compactacion: %i", metadata->compaction_time);
		log_t("  -> Tipo de consistencia: %i", metadata->consistency);
		log_t("  -> Cantidad de particiones: %i", metadata->partitions);
	}
}

bool table_exists_in_metadata(char* table_name) {
	for (int i = 0; i < list_size(g_table_metadata); i++) {
		table_metadata_t* table_metadata = list_get(g_table_metadata, i);
		if (string_equals_ignore_case(table_name, table_metadata->table_name))
			return true;
	}

	return false;
}

void free_auto_describe_input(void* input) {
	free(input);
}
