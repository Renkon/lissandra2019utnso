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
		return -2;
	}

	// Paso 3: inicializo la conexion
	if (connect(connection_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		log_w("No se pudo realizar la conexion. Se cancela la solicitud");
		return -3;
	}

	packet = malloc(sizeof(packet_t));
	build_packet(packet, process, HANDSHAKE_IN, true, 0, NULL);

	// Paso 4: hacemos el handshake
	log_t("Solicitando handshake a %s:%i", ip, port);
	send2(connection_socket, packet);

	// Paso 5: recibimos el handshake y procedemos con la solicitud
	recv2(connection_socket, packet);

	if (packet->header.operation != HANDSHAKE_OUT) {
		log_w("El servidor no respondio el handshake. Se cancela la solicitud");
		free_packet(packet);
		return -4;
	}

	log_t("Se realizo el handshake satisfactoriamente");

	free_packet(packet);

	return connection_socket;
}

void kill_connection(int socket) {
	close(socket);
}

void do_simple_request(process_t process, char* ip, int port, socket_operation_t operation, void* content, int content_length, void (*callback)(void*)) {
	int socket;
	packet_t* packet;

	if ((socket = setup_connection(process, ip, port)) < 0) {
		return;
	}

	packet = malloc(sizeof(packet_t));
	build_packet(packet, process, operation, false, content_length, content);

	send2(socket, packet);

	// Recibimos el paquete de respuesta del servidor
	if (recv2(socket, packet) <= 0) { // Si me devuelve 0 o menos, fallo el recv.
		free_packet(packet);
		log_w("El servidor cerro la conexion. Se cancela la request");
		return;
	}

	callback(packet->content);

	kill_connection(socket);
	free_packet(packet);
}

