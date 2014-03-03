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

extern int handleEvent(enum eventType type,struct RoutingInfo senderRtngInfo); 
/**
 * Stores the Node and degree information that needs to be sent
 * via UDP to the user querying
 */
struct NodeDegree
{
	int Node;
	int Degree;
} tableNodDeg[RT_MAX_SIZE];

/**
 * Used as a part of sorting mechanism (quick sort)
 */
int compFunc(const void * elem1, const void * elem2)
{
	struct NodeDegree nodEnt1 = *((struct NodeDegree *)elem1);
	struct NodeDegree nodEnt2 = *((struct NodeDegree *)elem2);

	if(nodEnt1.Node > nodEnt2.Node)
		return 1;
	else if(nodEnt1.Node < nodEnt2.Node)
		return -1;
	else
		return 0;
}

void *queryProc_handler();
/**
 * This is the handler used by the referring m0 Node when a non m0 node
 * asks for the degree information. The node computes the best node to
 * connect as a part of Barabasi Albert model
 *
 */
void *udp_handler() {

	int sockfd, n, i, totalDegree = 0,j,cur_limit = 0,k =0;
	int rand_num;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char recvbuff[1000], sendbuff[1000];
	char connNode[50];
	int selectedNodes[RT_MAX_SIZE];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = nodes[myNodeNum].sin_port + 1; // If TCP port is 5000 then UDP port will be 5001

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	while (1) {
		totalDegree = 0;
		len = sizeof(cliaddr);
		n = recvfrom(sockfd, recvbuff, sizeof(recvbuff), 0, (struct sockaddr *) &cliaddr, &len);
		printf("\nReceived UDP Join Request message: %s\n", recvbuff);
		if (rt_num_entries >= totalNodes ) {
			//If the number of Nodes is more than the number of entries in the routing table then reject
			sprintf(sendbuff, "%s", "REJECT_JOIN");
		} else 
		{
			sprintf(sendbuff, "%s", "JOIN");

			printf("Before sorting\n");
			for(i=0 ;i<rt_num_entries; i++)
			{
				tableNodDeg[i].Node = RoutingTable[i].dest;
				tableNodDeg[i].Degree = RoutingTable[i].degree;
				totalDegree += tableNodDeg[i].Degree;
				printf("%d %d\n",tableNodDeg[i].Node,tableNodDeg[i].Degree);
			}

			printf("After sorting\n");
			qsort(tableNodDeg,rt_num_entries,sizeof(struct NodeDegree),compFunc);

			for(i=0 ;i<rt_num_entries; i++)
			{
				printf("%d %d\n",tableNodDeg[i].Node,tableNodDeg[i].Degree);
			}

			srand(time(NULL));
			// Probability computation
			// Calculating node to be connected based on degree.
			int numSelected = 0,repFlag = 0;
			for(i=0; i< edgeCount ; i++)
			{
				repFlag = 0;
				cur_limit = 0;
				rand_num = (rand() % (totalDegree-1)) +1;
				/**
				 * Get the random value and check for the range where the probability
				 * lands. That particular node is selected and sent. If the Same Node m
				 * is selected as a part of connecting edge then another value is chosen
				 */
				printf("\nRandom no. %d totalDegree %d\n",rand_num,totalDegree);
				for(j=0; j<rt_num_entries; j++)
				{
					cur_limit += tableNodDeg[j].Degree;
					if( rand_num <= cur_limit)
					{
						for(k=0; k<numSelected; k++)
						{
							if(tableNodDeg[j].Node == selectedNodes[k])
							{
								// Node already selected
								i--;
								repFlag = 1;
								break;
							}
						}
						if(repFlag == 1)
						{
							break;
						}

						selectedNodes[numSelected++] = tableNodDeg[j].Node;
						sprintf(connNode," %d",tableNodDeg[j].Node);
						strcat(sendbuff,connNode);
						break;
					}
				}
			}		

		}

		printf("Sending Node JOIN reply :%s",sendbuff);
		fflush(stdout);
		sendto(sockfd, sendbuff, strlen(sendbuff)+1, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
		usleep(1000);
		//recvbuff[n] = 0;
		//sendbuff[n] = 0;
	}

	return 0;
}
/**
 * Processes the queries sent by the query node
 * if 1: then Degree is sent
 * if 2: then Farthest node relative to myNode is sent
 * if 3: then the entire Routing table is sent
 */
void *queryProc_handler() {

	int i,max_dist;
	char sendData[1024],tempStr[100];
	char max_dist_info[1024], degInfo[1024];
	printf("\nEnter query proc handler\n");
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t len;
	char recvbuff[1000], sendbuff[1000];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = nodes[myNodeNum].sin_port + 2;

	bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("\nListening to queries...\n");

	len = sizeof(cliaddr);
	n = recvfrom(sockfd, recvbuff, sizeof(recvbuff), 0,
			(struct sockaddr *) &cliaddr, &len);
	printf("\nReceived query connection request: %s\n", recvbuff);
	if (n < 0) {
		printf("\nError in receiving data @ queryProc_handler()\n");
		//Not exiting the program for error in getting the query
	}
	usleep(100);
	sendto(sockfd, "QUERY_RECV_ACK", sizeof("QUERY_RECV_ACK"), 0,
			(struct sockaddr *) &cliaddr, sizeof(cliaddr));


	while (1) 
	{
		recvbuff[n] = 0;
		n = recvfrom(sockfd, recvbuff, sizeof(recvbuff), 0,
				(struct sockaddr *) &cliaddr, &len);
		printf("\nReceived query: %s\n", recvbuff);
		if (n < 0) {
			printf("\nError in receiving data @ queryProc_handler()\n");
			//Not exiting the program for error in getting the query
		}
		usleep(100);
		switch (atoi(recvbuff)) {
		case GET_DEGREE: {
			for(i=0; i<rt_num_entries; i++)
			{
				if(RoutingTable[i].dest == myNodeNum)
				{
					sprintf(degInfo,"%d",RoutingTable[i].degree);
					break;
				}
			}
			printf("Sending %s",degInfo);
			sendto(sockfd, degInfo, strlen(degInfo)+1, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			printf("\nRetrieving degree information\n");
			break;
		}
		case GET_FARTHEST: {
			max_dist = 0;
			for(i=0; i<rt_num_entries; i++)
			{
				if(RoutingTable[i].cost > max_dist)
				{
					max_dist = RoutingTable[i].cost;
					sprintf(max_dist_info,"%d %d",RoutingTable[i].dest,RoutingTable[i].cost);
				}
			}
			printf("Sending %s",max_dist_info);
			sendto(sockfd, max_dist_info, strlen(max_dist_info)+1, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			printf("\nRetrieving farthest node information\n");
			break;
		}
		case GET_RTABLE: {
			sprintf(sendData,"%d ",rt_num_entries);
			for(i = 0; i< rt_num_entries; i++)
			{
				sprintf(tempStr," %d", RoutingTable[i].dest);
				strcat(sendData,tempStr);

				sprintf(tempStr," %d", RoutingTable[i].cost);
				strcat(sendData,tempStr);

				sprintf(tempStr," %d", RoutingTable[i].nextHop);
				strcat(sendData,tempStr);

				sprintf(tempStr, " %d", RoutingTable[i].degree);
				strcat(sendData, tempStr);
			}
			printf("Sending %s",sendData);
			sendto(sockfd, sendData, strlen(sendData)+1, 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			printf("\nRetrieving routing table information\n");
			break;
		}
		default:
			// provide RT
			printf("\nRetrieving routing table information\n");
			break;
		}
	}
	return 0;
}
