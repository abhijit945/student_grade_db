/*
 * file: EventHandler.c
 * 
 * State machine to process received messages
 * If a ROUTING message is received then Routing table is updated
 * and messages are forwarded with updated routing table information.
 * If a CONNECT message is received then a TCP connection is established with that node.
 *  
 */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#include "common.h"
#include "RoutingFns.h"


// Modify routing table with the new information
// Returns 0 on success.
int handleRouting(struct RoutingInfo senderRtngInfo)
{
	int i,j;

	/* To print message to be processed
	printf("sender %d originator %d counter %d init %d entries %d\n",senderRtngInfo.sender,senderRtngInfo.originator,senderRtngInfo.counter,senderRtngInfo.init,senderRtngInfo.entries);

	for(i=0;i<senderRtngInfo.entries;i++)
	{
		printf("%d %d %d\n",senderRtngInfo.rt_sender[i].dest,senderRtngInfo.rt_sender[i].cost,senderRtngInfo.rt_sender[i].nextHop);
	}
	*/

	// If I receive my own packet discard it
	if( myNodeNum == senderRtngInfo.originator )
	{
		free(senderRtngInfo.rt_sender);
		return 0;
	}

	// Check if a packet is received from some other direction which has lesser cost
	if( senderRtngInfo.counter <= NodeRtngMsgCounter[senderRtngInfo.originator] )
	{
		for(i=0; i < rt_num_entries; i++)
		{
			if( (RoutingTable[i].dest == senderRtngInfo.originator) )
			{
				for(j=0; j< senderRtngInfo.entries; j++)
				{
					if(senderRtngInfo.rt_sender[j].dest == senderRtngInfo.originator)
					{
						if( RoutingTable[i].cost <= ((senderRtngInfo.rt_sender[j].cost) +1) )
						{
							free(senderRtngInfo.rt_sender);
							// No need to modify the routing table
							return 0;
						}else
							break;
					}
				}
			}
		}

	}


	printf("Modifying routing table\n");

	for(j=0; j< senderRtngInfo.entries; j++)
	{
		for(i=0; i<rt_num_entries; i++)
		{
			if(senderRtngInfo.rt_sender[j].dest == RoutingTable[i].dest)
			{
				// Modify route if the cost is lesser
				if(RoutingTable[i].cost > (senderRtngInfo.rt_sender[j].cost +1))
				{
					RoutingTable[i].cost = (senderRtngInfo.rt_sender[j].cost +1);
					RoutingTable[i].nextHop = senderRtngInfo.sender;
				}
				if(RoutingTable[i].degree < senderRtngInfo.rt_sender[j].degree)
				{
					RoutingTable[i].degree = senderRtngInfo.rt_sender[j].degree;
				}
				break;
			}
		}

		// If the node is not present in routing table
		if(i == rt_num_entries)
		{
			// add destination node to routing table
			RoutingTable[rt_num_entries].dest = senderRtngInfo.rt_sender[j].dest;
			RoutingTable[rt_num_entries].cost = senderRtngInfo.rt_sender[j].cost + 1;
			RoutingTable[rt_num_entries].nextHop = senderRtngInfo.sender;
			RoutingTable[rt_num_entries].degree = senderRtngInfo.rt_sender[j].degree;

			rt_num_entries++;
		}
	}

	//rt_print();

	// if routing is initialized then create own packet as well
	if( (NodeRtngMsgCounter[senderRtngInfo.originator] < senderRtngInfo.counter) && (senderRtngInfo.init == 1) )
	{
		printf("This is init so create a packet\n");
		sendRoutingInfo(1,&senderRtngInfo);
	}

	printf("Forwarding the packet\n");
	sendRoutingInfo(0,&senderRtngInfo);

	// Update the counter value for the originator
	if(NodeRtngMsgCounter[senderRtngInfo.originator] < senderRtngInfo.counter)
		NodeRtngMsgCounter[senderRtngInfo.originator] = senderRtngInfo.counter;

	free(senderRtngInfo.rt_sender);

	return 0;
}

// Connnect to a new non-m0 node
int handleConnect(int connTo) {

	int sockfd, n;
	struct sockaddr_in serv_addr;
	char recvline[1000];
	int remote_node_no = connTo;

	printf("\nConneting to %d -> IP:PORT %s:%d\n", remote_node_no,
			inet_ntoa(nodes[remote_node_no].sin_addr),
			(int) ntohs(nodes[remote_node_no].sin_port));

	// Establish TCP connection to new node
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = nodes[remote_node_no].sin_addr.s_addr;
	serv_addr.sin_port = nodes[remote_node_no].sin_port;

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
		printf(
				"\n Error : Connect Failed, please check if the server at \"%s\" is up\n",
				inet_ntoa(nodes[remote_node_no].sin_addr));
		return 0;
	}

	printf("\nConnected\n");

	// Store socket fd for future communications
	nodes_sockfd[remote_node_no] = sockfd;
	rt_add_entry(remote_node_no,1,remote_node_no,0);
	rt_increment_degree();

	// make this node a new neighbour
	neighbour[num_nbrs++] = remote_node_no;
	
	//Send sample info
	sendto(sockfd, "Hello", 1000, 0,(struct sockaddr *) &serv_addr, sizeof(serv_addr));
	printf("\nSent\n");
	n = recvfrom(sockfd, recvline, sizeof(recvline), 0, NULL, NULL);
	recvline[n] = 0;
	printf("\nReceiving...\n");
	fputs(recvline, stdout);
	return 0;

}

// State machine to process ROUTING and CONNECT messages
int handleEvent(enum eventType type,struct RoutingInfo senderRtngInfo,int connTo)
{
	int retval;

	printf("Entered handleEvent type:%d \n",type);

	// Only a single thread can execute switch
	// which ensures synchronized modification of Routing Table .
	pthread_mutex_lock(&lockHndlEvnt);
	retval = 0;

	switch(type)
	{
	case ROUTING:
		retval=handleRouting(senderRtngInfo);
		break;

	case CONNECT:
		retval=handleConnect(connTo);
		break;

	default:
		printf("Unknown type in handleEvent %d\n",type);
		retval = -2;
		break;
	}

	pthread_mutex_unlock(&lockHndlEvnt);
	printf("Processed message in handleEvent \n");
	return retval;
}