/*
	Задание: переделать функции чата так, чтобы для каждого клиента создавалась собственная нить исполнения.
	После каждого вызова accept создается новая нить исполнения с помощью функции pthread_create. Каждая новая нить выполняет функцию client_connection(), которой в качестве параметра передается идентификатор сокета, взаимодействующего с клиентом.	
	Клиентская часть програмы представлена в файле client.cpp.
*/

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#define ERROR_S "Server ERROR:"
#define DEFAULT_PORT 1601
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SIMBOL '#'

bool is_client_connection_close(const char* msg);
void * client_connection(void * dummy);

int main(int argc, char const* argv[]){
	int client;
	int server;
	struct sockaddr_in server_adress;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if(client < 0){
		std::cout << ERROR_S <<" establishing socket error.\n";
		exit(0);
	}
	std::cout << "SERVER: Socket for server was successfully created.\n";
	server_adress.sin_port = htons(DEFAULT_PORT);
	server_adress.sin_family = AF_INET;
	server_adress.sin_addr.s_addr = htons(INADDR_ANY);

	int ret = bind(client, reinterpret_cast<struct sockaddr*>(&server_adress), sizeof(server_adress));

	if (ret < 0){
		std::cout << ERROR_S << " binding connection error." << '\n';
		return -1;
	}
	socklen_t size = sizeof(server_adress);
	std::cout << "SERVER:" << " Listening client...\n";
	listen(client,1);

	while (1){
		server = accept (client, reinterpret_cast<struct sockaddr*>(&server_adress), &size);
		if (server < 0){
			std::cout <<  ERROR_S << "Can't accepting client" << '\n';
		}
		pthread_t thid;
		int result = pthread_create(&thid, (pthread_attr_t *)NULL, client_connection, (void*)(&server));

		if(result != 0){
			printf ("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
	}
	return 0;
}

void * client_connection(void * dummy){
	int server_socket = *((int*) dummy);
	char buffer[BUFFER_SIZE];
	bool isExit = false;
	while (server_socket > 0){
		strcpy(buffer, "-> Server connected!\n");
		send(server_socket, buffer, BUFFER_SIZE,0);
		std::cout << "-> Connected to client #1" << std::endl
		<< "Enter " << CLIENT_CLOSE_CONNECTION_SIMBOL << "to end the connection\n";

		std::cout << "Client " << server_socket << ":";
		recv(server_socket, buffer, BUFFER_SIZE,0);
		std::cout << buffer << std::endl;
		if (is_client_connection_close(buffer)){
			isExit = true;
		}
		while (!isExit){
			std::cout << "Server: ";
			std::cin.getline(buffer, BUFFER_SIZE);
			send(server_socket, buffer, BUFFER_SIZE,0);
			if (is_client_connection_close(buffer)){
				break;
			}
			std::cout << "Client " << server_socket << ":";
			recv(server_socket, buffer, BUFFER_SIZE,0);
			std::cout << buffer << std::endl;
			if (is_client_connection_close(buffer)){
				break;
			}
		}
		std::cout << "\nGoodbye..." << std::endl;
		isExit = false;
		exit(1);
	}
	return 0;
}

bool is_client_connection_close(const char* msg){
	for(int i =0; i < strlen(msg); ++i){
		if(msg[i] == CLIENT_CLOSE_CONNECTION_SIMBOL){
			return true;
		}
	}
	return false;
}
