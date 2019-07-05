#include "generic_client.h"

int setup_connection(process_t process, char* ip, int port) {
	int connection_socket;
	struct sockaddr_in serv_addr;
	packet_t* packet;

	// Paso 1: creo el socket
	if ((connection_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_w("No se pudo crear el socket para la solicitud.");
		return -1;
	}

	// Paso 2: asigno las propiedades de la conexion
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	// Esto tambien es parte del paso 2
	if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
		log_w("No se pudo convertir la IP a binario. Se cancela la solicitud");
		kill_connection(connection_socket);
		return -2;
	}

	struct timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	setsockopt(connection_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

	// Paso 3: inicializo la conexion
	if (connect(connection_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		log_w("No se pudo realizar la conexion. Se cancela la solicitud");
		kill_connection(connection_socket);
		return -3;
	}

	packet = malloc(sizeof(packet_t));
	build_packet(packet, process, HANDSHAKE_IN, true, 0, NULL, NULL, true);

	// Paso 4: hacemos el handshake
	log_t("Solicitando handshake a %s:%i", ip, port);
	send2(connection_socket, packet);

	// Paso 5: recibimos el handshake y procedemos con la solicitud
	recv2(connection_socket, packet);

	if (packet->header.operation != HANDSHAKE_OUT) {
		log_w("El servidor no respondio el handshake. Se cancela la solicitud");
		kill_connection(connection_socket);
		free_packet_content(packet);
		free(packet);
		return -4;
	}

	log_t("Se realizo el handshake satisfactoriamente");

	free_packet_content(packet);
	free(packet);

	return connection_socket;
}

void kill_connection(int socket) {
	shutdown(socket, SHUT_RDWR);
	close(socket);
}

void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int elements, int* elements_length,
		void (*callback)(void*, response_t*), bool success, void (*free_content)(void*), response_t* response) {
	pthread_t thread;
	client_conn_args_t* args = malloc(sizeof(client_conn_args_t));
	args->process = process;
	args->ip = ip;
	args->port = port;
	args->operation = operation;
	args->content = content;
	args->elements = elements;
	args->elements_length = elements_length;
	args->callback = callback;
	args->success = success;
	args->free_content = free_content;
	args->response = response;

	if (pthread_create(&thread, NULL, (void*) do_request, (void*) args)) {
		log_e("No se pudo inicializar el hilo para la solicitud");
	}
}

void do_request(void* arguments) {
	client_conn_args_t* args = (client_conn_args_t*) arguments;
	int socket;
	packet_t* packet;
	void* deserialized_content;
	bool successful = false;

	pthread_detach(pthread_self());
	if ((socket = setup_connection(args->process, args->ip, args->port)) < 0) {
		if (args->free_content != NULL)
			args->free_content(args->content);
		if (args->elements > 0)
			free(args->elements_length);

		args->callback(NULL, args->response);

		free(arguments);
		return;
	}

	packet = malloc(sizeof(packet_t));
	build_packet(packet, args->process, args->operation, false, args->elements, args->elements_length, args->content, args->success);

	send2(socket, packet);

	if (args->free_content != NULL)
		args->free_content(args->content);

	// Recibimos el paquete de respuesta del servidor
	if (recv2(socket, packet) <= 0) { // Si me devuelve 0 o menos, fallo el recv.
		log_w("El servidor cerro la conexion. Se cancela la request");
		args->callback(NULL, args->response);
	} else {
		successful = packet->header.success;
		deserialized_content = deserialize_content(packet->content, packet->header.operation, packet->header.elements, packet->header.elements_size);
		args->callback(deserialized_content, args->response);

		free_deserialized_content(deserialized_content, packet->header.operation);
	}

	kill_connection(socket);
	free_packet_content(packet);
	free(packet);
	free(arguments);
}

