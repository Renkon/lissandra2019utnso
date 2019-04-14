#include "generic_client.h"

int setup_connection(process_t process, char* ip, int port, header_t* header_send, header_t* header_recv, packet_t* packet) {
	int connection_socket;
	struct sockaddr_in serv_addr;

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
		return -2;
	}

	// Paso 3: inicializo la conexion
	if (connect(connection_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		log_w("No se pudo realizar la conexion. Se cancela la solicitud");
		return -3;
	}

	// Paso 4: hacemos el handshake
	header_send->content_length = 0;
	header_send->keep_alive = true;
	header_send->operation = HANDSHAKE_IN;
	header_send->process = process;

	log_t("Solicitando handshake a %s:%i", ip, port);
	send(connection_socket, header_send, sizeof(header_t), 0);

	// Paso 5: recibimos el handshake y procedemos con la solicitud
	recv2(connection_socket, header_recv, sizeof(header_t));

	if (header_recv->operation != HANDSHAKE_OUT) {
		log_w("El servidor no respondio el handshake. Se cancela la solicitud");
		return -4;
	}

	log_t("Se realizo el handshake satisfactoriamente");
	return connection_socket;
}

void kill_connection(int socket) {
	close(socket);
}

void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int content_length, void (*callback)(void*)) {
	int socket;
	void* buffer;
	header_t* header_send = malloc(sizeof(header_t));
	header_t* header_recv = malloc(sizeof(header_t));
	packet_t* packet = malloc(sizeof(packet_t));

	if ((socket = setup_connection(process, ip, port, header_send, header_recv, packet)) < 0) {
		release_request_resources(header_send, header_recv, packet);
		return;
	}

	header_send->content_length = content_length;
	header_send->keep_alive = false;
	header_send->operation = operation;
	packet->header = *header_send;
	packet->content = content;

	send(socket, packet, sizeof(header_t) + content_length, 0);

	// Recibimos lo que necesitamos
	// Primero el header
	recv2(socket, header_recv, sizeof(header_t));

	// Configuroel buffer y leo
	buffer = malloc(header_recv->content_length);
	recv2(socket, buffer, header_recv->content_length);

	log_t("Se recibio un paquete con contenido: %s", buffer);

	release_request_resources(header_send, header_recv, packet);

	kill_connection(socket);

	callback(buffer);
	free(buffer);
}

void release_request_resources(header_t* header1, header_t* header2, packet_t* packet) {
	free(header1);
	free(header2);
	free(packet);
}

