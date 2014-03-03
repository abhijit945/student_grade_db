#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#include "common.h"

void *est_tcp_conn(void *);
/**
 * For each TCP connection a thread is spawned and 
 * TCP connection is established by calling the est_tcp_conn
 * 
 */
void *tcp_reciever() {

	printf("\nListening on Port: %d\n", (int) ntohs(nodes[myNodeNum].sin_port));
	sleep(1);
	int listenfd = 0, connfd = 0, *new_sock;
	struct sockaddr_in serv_addr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = nodes[myNodeNum].sin_port;

	bind(listenfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	listen(listenfd, 150);

	while (1) {
		connfd = accept(listenfd, (struct sockaddr*) NULL, NULL);
		pthread_t spawner_thread;
		new_sock = malloc(sizeof(int));
		*new_sock = connfd;
		if (pthread_create(&spawner_thread, NULL, est_tcp_conn, (void*) new_sock) < 0) {
			perror("\n Error: Thread could not be created\n");
			return 0;
		}

		// pthread_join(spawner_thread, NULL);
	}
	return 0;
}

/**
 * New thread handler that samples the data 
 * and inserts the data into a queue 
 * As each entry into the queue is mutex based 
 * the thread makes safe insertions.
 */
void *est_tcp_conn(void *socket_desc) {
	printf("New thread spawned\n");

	int sock_d = *(int*) socket_desc;
	int n, i = 0;
	time_t ticks;
	char sendBuff[1000], recvBuff[1024];

	free(socket_desc);
	while(1)
	{
		if ((n = recv(sock_d, recvBuff, sizeof(recvBuff), 0)) > 0)
		{
			recvBuff[n] = 0;
		}

		if (n < 0) {
			printf("\n Error: Could not receive properly \n");
			continue;
		}

		if(n == 0)
		{
			printf("connection is closed\n");
			gracefulExit();
			break;
		}

		if(!strncmp(recvBuff,"Hello",5))
		{
			write(sock_d, (const void*) recvBuff, strlen(recvBuff));
			printf("Data sent: %s\n",recvBuff);
			continue;
		}
		printf("Inserting msg %s\n",recvBuff);
		insertRecMsgQ(recvBuff);
		//usleep(1000);

	}

	return 0;
}