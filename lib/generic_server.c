#include "generic_server.h"

bool init_server(int listen_port, process_t process) {
	pthread_t connection_thread;
	conn_thread_args_t* conn_args = malloc(sizeof(conn_thread_args_t));
	conn_args->port = listen_port;
	conn_args->process = process;

	if (pthread_create(&connection_thread, NULL, (void*) setup_server, (void*) conn_args)) {
		log_e("No se pudo inicializar el hilo principal de conexiones");
		return false;
	}

	return true;
}

bool setup_server(void* args) {
	struct sockaddr_in address;
	int listen_port = ((conn_thread_args_t*) args)->port;
	int opt = 1;
	int address_length = sizeof(address);
	int server_fd, accepted_socket;

	// Me detacheo porque me considero independiente :3
	pthread_detach(pthread_self());

	// Paso 1: inicializamos el socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		log_e("No se pudo inicializar el socket. No se pudo inicializar el servidor");
		return false;
	}

	// Paso 2: asignarle configuraciones al socket
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		log_e("No se pudo configurar el socket. No se pudo inicializar el servidor");
		return false;
	}

	// Paso 2.5: seteamos unos valores a la estructura del address
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(listen_port);

	// Paso 3: bindeamos el socket al port
	if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
		log_e("No se pudo bindear el socket al puerto definido. No se pudo inicializar el servidor");
		return false;
	}

	// Paso 4: le decimos que vamos a escuchar conexiones
	if (listen(server_fd, LISTEN_QUEUE) < 0) {
		log_e("No se pudo habilitar el puerto. No se pudo inicializar el servidor");
		return false;
	}

	// Paso 5, entro en un bucle infinito escuchando conexiones
	while (true) {
		pthread_t pthread_id;
		conn_args_t* connection_args;

		if ((accepted_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &address_length)) < 0) {
			log_e("No se pudo aceptar una solicitud realizada al servidor. Ignorando solicitud...");
			continue;
		}

		connection_args = malloc(sizeof(conn_args_t));
		connection_args->process = ((conn_thread_args_t*) args)->process;
		connection_args->socket = accepted_socket;

		if (pthread_create(&pthread_id, NULL, (void*) handle_request, (void*) connection_args)) {
			log_e("No se pudo iniciar un hilo para la conexion con el socket %i", accepted_socket);
			free(connection_args);
		}
	}

	// Paso 6: se cierra el socket, aunque esto no deberia pasar nunca
	close(server_fd);

	return true;
}

void handle_request(void* args) {
	int received_bytes = 1;
	conn_args_t* connection_args = (conn_args_t*) args;
	packet_t* packet = malloc(sizeof(packet_t));
	void* response;

	pthread_detach(pthread_self());

	// Seteamos para que entre al principio
	packet->header.keep_alive = true;

	while (packet->header.keep_alive && received_bytes > 0) {
		// Paso 1: leemos el paquete
		received_bytes = recv2(connection_args->socket, packet);

		if (!valid_source(connection_args->process, packet->header.process)) {
			log_w("El proceso %s intento comunicarse conmigo. Rechazo solicitud", get_process_name(packet->header.process));
			break;
		}

		// Paso 2: revisamos si es handshake o no
		if (packet->header.operation == HANDSHAKE_IN) {
			log_t("Recibi solicitud de handshake de %s", get_process_name(packet->header.process));
			build_packet(packet, connection_args->process, HANDSHAKE_OUT, true, 0, NULL);
			send2(connection_args->socket, packet);
		} else { // Esto seria una peticion normal
			char* dummy_response = " y he sido respondida";

			log_t("Recibi request de %s. Body: %s", get_process_name(packet->header.process), (char*) packet->content);

			// TODO: agregar logica de recepcion del buffer
			// aca se procesa todo... y se obtiene un body
			response = malloc(packet->header.content_length + strlen(dummy_response));
			memcpy(response, packet->content, packet->header.content_length);
			strcat(response, dummy_response);

			build_packet(packet, connection_args->process, packet->header.operation + 1, packet->header.keep_alive,
					packet->header.content_length + strlen(dummy_response), response);

			send2(connection_args->socket, packet);

			free(response);
		}
	}

	close(connection_args->socket);
	free_packet(packet);
	free(args);
}

bool valid_source(process_t me, process_t client) {
	if (me == KERNEL)
		return false;
	if (me == MEMORY)
		return client == KERNEL;
	if (me == FILESYSTEM)
		return client == MEMORY;
	return false;
}
