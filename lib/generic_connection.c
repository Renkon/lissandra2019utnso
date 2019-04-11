#include "generic_connection.h"

bool init_server(int listen_port) {
	pthread_t connection_thread;

	if (pthread_create(&connection_thread, NULL, (void*) setup_server, (void*) listen_port)) {
		log_e("No se pudo inicializar el hilo principal de conexiones");
		return false;
	}

	return true;
}

bool setup_server(int listen_port) {
	struct sockaddr_in address;
	int opt = 1;
	int address_length = sizeof(address);
	int server_fd, accepted_socket;
	char* buffer[BUFFER_SIZE];

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
		if ((accepted_socket = accept(server_fd, (struct sockaddr*) &address, (socklen_t*) &address_length)) < 0)
			log_e("No se pudo aceptar una solicitud realizada al servidor. Ignorando solicitud...");


		while (recv(accepted_socket, buffer, BUFFER_SIZE, 0)) {
			log_t("%s", buffer);
			memset(buffer, 0, BUFFER_SIZE);
		}

		if (close(accepted_socket))
			log_e("No se pudo cerrar el socket %i", accepted_socket);
	}

	// Paso 6: se cierra el socket, aunque esto no deberia pasar nunca
	close(server_fd);

	return true;
}
