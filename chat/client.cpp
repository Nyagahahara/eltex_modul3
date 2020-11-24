/*
	Задание: переделать функции чата так, чтобы для каждого клиента создавалась собственная нить исполнения.
	После каждого вызова accept создается новая нить исполнения с помощью функции pthread_create. Каждая новая нить выполняет функцию client_connection(), которой в качестве параметра передается идентификатор сокета, взаимодействующего с клиентом.	
	Функции клиента не изменены. Серверная часть приложения находится в файле server2.cpp.
*/

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define ERROR_S "CLIENT ERROR:"
#define DEFAULT_PORT 1601
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define CLIENT_CLOSE_CONNECTION_SIMBOL '#'

bool is_client_connection_close(const char* msg);

int main(int argc, char const* argv[]){
	int client;
	struct sockaddr_in server_adress;
	client = socket(AF_INET, SOCK_STREAM, 0);
	if(client < 0){
		std::cout << ERROR_S <<"establishing soccket error\n";
		exit(0);
	}

	server_adress.sin_port = htons(DEFAULT_PORT);
	server_adress.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_IP, &server_adress.sin_addr);
	std::cout << "Client  Socket for server was successfully created\n";

	int ret = connect(client, reinterpret_cast<struct sockaddr*>(&server_adress), sizeof(server_adress));

	if (ret <0){
		std::cout << ERROR_S << " connection error" << '\n';
		return -1;
	}
	if (ret = 0){
		std::cout << ERROR_S << " connection to server\n";
	}

	char buffer[BUFFER_SIZE];
	std::cout  << "Waiting to server confirmation\n";
	recv(client, buffer, BUFFER_SIZE,0);
	std::cout  << "server confirmation\n";
	while (true){
		std::cout << "Client: ";
		std::cin.getline(buffer, BUFFER_SIZE);
		send(client, buffer, BUFFER_SIZE,0);
		if (is_client_connection_close(buffer)){
			break;
		}
		std::cout << "Server: ";
		recv(client, buffer, BUFFER_SIZE,0);
		std::cout << buffer << std::endl;
		if (is_client_connection_close(buffer)){
			break;
		}
		std::cout << std::endl;
	}
	close(client);
	std::cout << "Goodbye..." << std::endl;
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
