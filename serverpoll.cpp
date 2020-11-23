/*
	Задание: заменить отслеживание событий на сокетах с помощью select() на использование poll().
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

using namespace std;

int main()
{
	int listener;
	struct sockaddr_in addr;
	char buf[1024];
	int bytes_read;

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0)
	{
		perror("socket");
		exit(1);
	}

	fcntl(listener, F_SETFL, O_NONBLOCK);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(3425);
	addr.sin_addr.s_addr = INADDR_ANY;
	if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("bind");
		exit(2);
    }

	listen(listener, 2);
    
	// Заполняем множество сокетов
	struct pollfd readset[1024];
	readset[0].fd = listener;
	readset[0].events = POLLIN;
    
	int count = 1;
    
	while(1)
	{
        
		int ret = poll(readset, count, 15000);
       
		if(ret <= 0)
		{
			perror("error in poll");
			exit(3);
		}
		if ( readset[0].revents & POLLIN ){
	        	readset[0].revents = 0;      	
			// Поступил новый запрос на соединение, используем accept	
			int sock = accept(listener, NULL, NULL);
			if(sock < 0)
			{
				perror("accept");
				exit(3);
			}

			fcntl(sock, F_SETFL, O_NONBLOCK);

			readset[count].fd = sock;
			readset[count].events = POLLIN;
			count++;
		}

		for(int i = 1; i < count; i++)
		{
			if( readset[i].revents & POLLIN )
			{
				readset[i].revents = 0;
				// Поступили данные от клиента, читаем их
				bytes_read = recv(readset[i].fd, buf, 1024, 0);
				buf[bytes_read] = '\0';
				printf("Read %s\n", buf);
                
				if(bytes_read <= 0)
				{
					// Соединение разорвано, удаляем сокет из множества
					close(readset[i].fd);
					for (int j = i; j < count; j++){
						readset[j] = readset[j+1];
					}
                    
					count--;
					i--;
					break;
				}

				// Отправляем данные обратно клиенту
				send(readset[i].fd, buf, bytes_read, 0);
			}
		}
	}
	return 0;
}
