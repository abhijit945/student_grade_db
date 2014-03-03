#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>

#include "common.h"
#include "RoutingFns.h"

/**
 * Handles the query that is asked by the user from the console
 * also is used for initiating the UDP connection to the said Node
 */
void *query_handler() {
	int sockfd_udp = 0, n;
	char recvline[1000];
	char qNode[3], queryId[3];
	int node, qid;
	struct sockaddr_in serv_addr;
	printf("\nThis is a query node\n");
	printf("Please type the node to communicate to\n");
	if (fgets(qNode, 10, stdin) == NULL) {
		printf("\nNo input was provided exiting...\n");
		gracefulExit();
	}
	node = atoi(qNode);
	//Get all the information of the nodes from the config files
	getNodesAddr();
	getInitialNodeParams();
	if (node > totalNodes) {
		printf("\nNo such node is currently active for me to query!\n");
		gracefulExit();
	}
	printf("\n%d selected\n", node);
	printf("\n%d -> IP:PORT %s:%d\n", node, inet_ntoa(nodes[node].sin_addr),
			(int) ntohs(nodes[node].sin_port));

	//Initiate a UDP connection for handshake
	sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = nodes[node].sin_addr.s_addr;
	serv_addr.sin_port = nodes[node].sin_port + 2; // or Default UDP Port can be used
	printf("\nSending connect request\n");
	sendto(sockfd_udp, "UDP_QUERY_HELLO", 1000, 0,
			(struct sockaddr *) &serv_addr, sizeof(serv_addr));

	n = recvfrom(sockfd_udp, recvline, sizeof(recvline), 0, NULL, NULL);
	usleep(1000);
	if (strcmp(recvline, "QUERY_RECV_ACK") == 0) {
		printf("\nConnection successful\n");
	}

	fputs(recvline, stdout);
	recvline[n] = 0;
	//Choose one of the options from the menu
	while(1)
	{
		printf("\nChoose the query\n");
		printf("\n1. What is your degree?\n");
		printf("\n2. What is your farthest node?\n");
		printf("\n3. What are your Routing table entries?\n");

		if (fgets(queryId, 10, stdin) == NULL) {
			printf("\nNo input was provided exiting...\n");
			gracefulExit();
		}
		qid = atoi(queryId);

		if (qid > 3 && qid <= 0) {
			printf("\nPlease enter a valid query number\n");
			gracefulExit();
		}
		//Send the queries to the Node chosen via UDP
		switch (qid) {
		case 1: {
			//"GET_DEGREE"
			sendto(sockfd_udp, "1", 2, 0, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr));
			recvfrom(sockfd_udp, recvline, sizeof(recvline), 0, NULL, NULL);
			printf("\nDegree of the node is: %d\n", atoi(recvline));

			break;
		}
		case 2: {
			//GET_FARTHEST
			int farthest_dest, farthest_cost;
			sendto(sockfd_udp, "2", 2, 0, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr));
			recvfrom(sockfd_udp, recvline, sizeof(recvline), 0, NULL, NULL);
			printf("\nStirng received: %s\n", recvline);
			sscanf(recvline, "%d %d", &farthest_dest, &farthest_cost);
			printf("\nFarthest destination is %d and cost for this node is: %d \n",
					farthest_dest, farthest_cost);
			break;
		}
		case 3: {
			//GET_RTABLE
			int i, nodeCount, dest, cost, nexthop, degree;
			char *ptr;
			sendto(sockfd_udp, "3", 2, 0, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr));
			recvfrom(sockfd_udp, recvline, sizeof(recvline), 0, NULL, NULL);
			printf("\nStirng received: %s\n", recvline);
			nodeCount = atoi(strtok(recvline, " "));
			for (i=0; i<nodeCount; i++) {
				ptr = strtok(NULL," ");
				sscanf(ptr, "%d", &dest);
				ptr = strtok(NULL," ");
				sscanf(ptr, "%d", &cost);
				ptr = strtok(NULL," ");
				sscanf(ptr, "%d", &nexthop);
				ptr = strtok(NULL," ");
				sscanf(ptr, "%d", &degree);
				printf("Dest :%d  Cost :%d  NextHop :%d Degree:%d\n", dest, cost, nexthop, degree);
			}
			break;
		}
		default:
			// provide RT - GET_RTABLE
			sendto(sockfd_udp, "3", 2, 0, (struct sockaddr *) &serv_addr,
					sizeof(serv_addr));

			break;
		}
	}
	return 0;
}
