/* file : RoutingFns.c
 * 
 * Functions to maintain routing table
 * and to carry out routing.
 */

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

// Definition of Routing Table
struct RoutingLink RoutingTable[RT_MAX_SIZE];

// Number of entries present in the routing table
int rt_num_entries = 0;

int routing_msg_cntr = 0;

// Function to add an entry into routing table
// returns 0 on success
// non-zero integer on failure/error
int rt_add_entry(int new_dest,int new_cost,int new_nextHop, int new_degree)
{
	if(rt_num_entries < RT_MAX_SIZE)
	{
		RoutingTable[rt_num_entries].dest = new_dest;
		RoutingTable[rt_num_entries].cost = new_cost;
		RoutingTable[rt_num_entries].nextHop = new_nextHop;
		RoutingTable[rt_num_entries].degree = new_degree;
		rt_num_entries++;
		return 0;
	} else {
		printf("\nRouting table limit exceeded\n");
		return -1;
	}

}

// Function to print routing table
// returns 0 on success
// non-zero integer on failure/error
int rt_print()
{
	int i;
	printf("Routing Table for %d node is\n", myNodeNum);
	for (i = 0; i < rt_num_entries; i++)
	{
		printf("Dest :%d  Cost :%d  NextHop :%d Degree:%d\n", RoutingTable[i].dest, RoutingTable[i].cost, RoutingTable[i].nextHop, RoutingTable[i].degree);
	}
	return 0;
}

// Method to initialize routing
// returns 0 on success
// non-zero integer on failure/error
int rt_init_routing()
{
	char sendData[1024],tempStr[100];
	int i;
	int cur_sockfd;

	memset(sendData,0,sizeof(sendData));

	// Create a routing packet

	strcpy(sendData,"ROUTING ");
	sprintf(tempStr,"%d ",myNodeNum);  // Sender
	strcat(sendData,tempStr);

	strcat(sendData,tempStr);  // Originator

	sprintf(tempStr,"%d ", routing_msg_cntr);  // set msg counter
	strcat(sendData,tempStr);
	routing_msg_cntr++;

	sprintf(tempStr,"%d ", 1);  // msg type init
	strcat(sendData,tempStr);

	sprintf(tempStr,"%d ", rt_num_entries);  // num of entries in routing table
	strcat(sendData,tempStr);

	// Add routing table values into message
	for(i = 0; i< rt_num_entries; i++)
	{
		sprintf(tempStr,"%d ", RoutingTable[i].dest);
		strcat(sendData,tempStr);

		sprintf(tempStr,"%d ", RoutingTable[i].cost);
		strcat(sendData,tempStr);

		sprintf(tempStr,"%d ", RoutingTable[i].nextHop);
		strcat(sendData,tempStr);
		//Send my degree details to my neighbours
		sprintf(tempStr, "%d ", RoutingTable[i].degree);
		strcat(sendData, tempStr);
	}

	printf("Sending msg %s\n",sendData);

	// Send message to all neighbours
	for( i = 0; i < num_nbrs; i++ )
	{
		cur_sockfd = nodes_sockfd[neighbour[i]];

		if(send(cur_sockfd,sendData,strlen(sendData)+1,0) < 0)
		{
			printf("Send failed\n");
			return -1;
		}
		usleep(1000);
	}
	return 0;
}

// Method to send Routing information
// returns 0 on success
// non-zero integer on failure/error
int sendRoutingInfo(int src, struct RoutingInfo *senderRtngInfo)
{
	char sendData[1024],tempStr[100];
	int i;
	int cur_sockfd;

	memset(sendData,0,sizeof(sendData));

	// Create a routing message
	strcpy(sendData,"ROUTING");
	sprintf(tempStr," %d",myNodeNum);  // Sender
	strcat(sendData,tempStr);

	// which node is intializing packet
	if(src)
	{
		// a new packet from current node
		strcat(sendData,tempStr);  // Originator

		sprintf(tempStr," %d", routing_msg_cntr);  // orig msg counter
		strcat(sendData,tempStr);
		routing_msg_cntr++;

		sprintf(tempStr," %d", 0);  // msg type init
		strcat(sendData,tempStr);
	}else
	{
		// forwarding the received packet with new routing table
		sprintf(tempStr," %d", senderRtngInfo->originator);
		strcat(sendData,tempStr);  // Originator

		sprintf(tempStr," %d", senderRtngInfo->counter);  // orig msg counter
		strcat(sendData,tempStr);

		sprintf(tempStr," %d", senderRtngInfo->init);  // msg type init
		strcat(sendData,tempStr);
	}

	sprintf(tempStr," %d", rt_num_entries);  // num of entries in routing table
	strcat(sendData,tempStr);

	// Adding routing table into message
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

	printf("sending routing Data %s\n",sendData);
	
	// Send message to all the neighbours
	for( i = 0; i < num_nbrs; i++ )
	{
		cur_sockfd = nodes_sockfd[neighbour[i]];

		if(send(cur_sockfd,sendData,strlen(sendData)+1,0) < 0)
		{
			printf("Send failed\n");
			return -1;
		}
		usleep(1000);
	}
	return 0;
}

// Function to add degree to make connections consistent
// returns 0 on success
// non-zero integer on failure/error
int rt_increment_degree()
{
	// A new node is connected so increase the degree of this node
	int i;
	for(i=0 ;i<rt_num_entries; i++)
	{
		if(RoutingTable[i].dest == myNodeNum)
		{
			RoutingTable[i].degree += 1;
			break;
		}
	}

	return 0;	
}