/*
	Задание: организовать двустороннюю поочередную связь процесса-родителя и процесса-ребенка через pipe, используя для синхронизации семафоры.	
	Для организации двусторонней поочередной связи процесса-родителя и процесса-ребенка достаточно одного pipe и двух семафоров: один из семафоров используется для управления состоянием родителя (чтение/запись или ожидание), второй - для управления состоянием ребенка. Родительский и дочерний процессы поочередно читают сообщение из общего pipe и пишут в него ответ.
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(){
int fd[2], result;
size_t size;
char resstring[14];

if(pipe(fd) < 0){
	printf("Can\'t create pipe\n");
	exit(-1);
}

int semid; 
char pathname[] = "08-1a.c"; 
key_t key; 
struct sembuf mybuf[2];
if((key = ftok(pathname,0)) < 0){
	printf("Can\'t generate key\n");
	exit(-1);
}
if((semid = semget(key, 2, 0666 | IPC_CREAT)) < 0){
	printf("Can\'t get semid\n");
	exit(-1);
}

result = fork();
if(result < 0){
	printf("Can\'t fork child\n");
	exit(-1);

	} else if (result > 0) {
	
		/* PARENT */	
						
		for (int i = 0; i < 3; i++){
			printf("Parent writing\n");		
			size = write(fd[1], "Hello, child!", 14);
			if(size != 14){
				printf("Can\'t write all string\n");
				exit(-1);
			}
			
			mybuf[0].sem_op = 1;
			mybuf[0].sem_flg = 0;
			mybuf[0].sem_num = 0;
			if(semop(semid, &mybuf[0], 1) < 0){
				printf("Can\'t wait for condition\n");
				exit(-1);
			}			
			
			mybuf[1].sem_op = -1;
			mybuf[1].sem_flg = 0;
			mybuf[1].sem_num = 1;
			if(semop(semid, &mybuf[1], 1) < 0){
				printf("Can\'t wait for condition\n");
				exit(-1);
			}
			printf("Parent reading\n");		
			size = read(fd[0], resstring, 14);
			if(size < 0){
				printf("Can\'t read string\n");
				exit(-1);
			}
			printf("%d%s\n",1,resstring);			
		}		
		close(fd[0]);
		close(fd[1]);
		printf("Parent exit\n");
	} else {
	
		/* CHILD */
		
		for (int i = 0; i < 3; i++){	
			mybuf[0].sem_op = -1;
			mybuf[0].sem_flg = 0;
			mybuf[0].sem_num = 0;
			if(semop(semid, &mybuf[0], 1) < 0){
				printf("Can\'t wait for condition\n");
				exit(-1);
			}
		
			printf("Child reading\n");							
			size = read(fd[0], resstring, 14);
			if(size < 0){
				printf("Can\'t read string\n");
				exit(-1);
			}
			printf("%d%s\n",2,resstring);
			printf("Child writing\n");
			size = write(fd[1], "Hello, mommy!", 14);
			if(size != 14){
				printf("Can\'t write all string\n");
				exit(-1);
			}		
			mybuf[1].sem_op = 1; 
			mybuf[1].sem_flg = 0;
			mybuf[1].sem_num = 1;
			if(semop(semid, &mybuf[1], 1) < 0){
				printf("Can\'t wait for condition\n");
				exit(-1);
			}
		}		
		close(fd[0]);	
		close(fd[1]);
		printf("Child exit\n");
	}

	return 0;
} 
