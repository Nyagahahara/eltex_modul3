/*
	Задание: написать программы сервера и клиента для мультиплексирования сообщений через одну очередь сообщений для модели клиент-сервер.
	Схема взаимодействия:
	Пусть сервер получает из очереди сообщений только сообщения с типом 1. В состав сообщений с типом 1, посылаемых серверу, процессы-клиенты включают значение своих идентификаторов процесса. Приняв сообщение с типом 1, сервер анализирует его содержание, выявляет идентификатор процесса, пославшего запрос, и отвечает клиенту, посылая сообщение с типом равным идентификатору запрашивавшего процесса. Процесс-клиент после посылки запроса ожидает ответа в виде сообщения с типом равным своему идентификатору. Поскольку идентификаторы процессов в системе различны, и ни один пользовательский процесс не может иметь PID равный 1, все сообщения могут быть прочитаны только теми процессами, которым они адресованы.
	Клиентская часть програмы представлена в файле client.c.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
	int msqid; 
	char pathname[] = "08-1a.c"; 
	key_t key; 
	struct mymsgbuf
	{
		long mtype;
		pid_t pid;
	} mybuf1, mybuf2;
	int len;

	if((key = ftok(pathname,0)) < 0){
	printf("Can\'t generate key\n");
	exit(-1);
	}

	if((msqid = msgget(key, 0666 | IPC_CREAT)) < 0){
		printf("Can\'t get msqid\n");
		exit(-1);
	}

	while(1){
		/* get message from clients */
		printf("Waiting for message\n");
		if((len = msgrcv(msqid, (struct msgbuf *) &mybuf1, len, 1, 0) < 0)){
			printf("Can\'t receive message from queue\n");
			exit(-1);
		}
		printf("message type = %ld, info = %d\n", mybuf1.mtype, (int)mybuf1.pid);
		
		/* message to client */
		mybuf2.mtype = (long)mybuf1.pid;
		mybuf2.pid = getpid();
		len = sizeof(pid_t);		
		if (msgsnd(msqid, (struct msgbuf *) &mybuf2, len, 0) < 0){
			printf("Can\'t send message to queue\n");
			msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
			exit(-1);
		}
	}

	return 0; 
} 
