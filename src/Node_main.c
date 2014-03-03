/*
 *  Node_main.c
 *
 *  Created on: Nov 17, 2013
 *  Author: Premkumar .
 * 
 *  Invokes Initialization functions
 *  Initializes mutex variables and Deliver thread
 *  Starts routing through rt_init_routing()
 * 
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

// Local header files
#include "common.h"
#include "RoutingFns.h"

// Global variables required for storing socket file descriptors, node addresses,
// initial node(m0) count, Maximum nodes, neighbours 
struct sockaddr_in nodes[MAX_NODES];

int nodes_sockfd[MAX_NODES];

int neighbour[MAX_NODES];
int num_nbrs = 0;

int initialNodeCount, edgeCount, totalNodes;

int myNodeNum = -1;
int conf_entries = -1;
int afterInitLoad = 0;

int NodeRtngMsgCounter[MAX_NODES];

pthread_mutex_t lockHndlEvnt;
pthread_mutex_t lockMsgQueue;

// signal handler function for SIGINT, SIGTERM
void cleanup() {
    printf("\nKeyboard Interrupt - Cleanup performed!\n");
    exit(0);
}

// Proper exit
void gracefulExit() {
	exit(0);
}

// Main function for all nodes
// returns 0 on success; negetive integer value to indicate failure/error
int main(int argc, char *argv[]) {
	
	int i;
	pthread_t tidDel, query_thread;

	// Initialize signal handlers
	signal(SIGINT, cleanup);
	signal(SIGTERM, cleanup);

	if (argc != 2) {
		printf("Usage: NodeProg node_no\n");
		return -2;
	}

	// If the node is Query node then create Query handler thread 
	if (strcmp(argv[1], "query") == 0) {
		//Start the query thread for UDP based queries
		pthread_create(&query_thread, NULL, query_handler, NULL);
		while (1);
		return 0;
	}
	
	// get my node number from command line argument
	myNodeNum = atoi(argv[1]);
	
	printf("\nMy nodeID is %d\n", myNodeNum);

	if (getNodesAddr()) {
		printf("Error setting configuration \n");
		return -2;
	}
	
	// Create mutex for handleEvent state machine
	if (pthread_mutex_init(&lockHndlEvnt, NULL) != 0)
	{
		printf("\n lockHndlEvnt mutex init failed\n");
		return -1;
	}
	
	// Create mutex for Message Queue
	if (pthread_mutex_init(&lockMsgQueue, NULL) != 0)
	{
		printf("\n lockMsgQueue mutex init failed\n");
		return -1;
	}

	// Read initial parameters for project configuration file
	afterInitLoad = getInitialNodeParams();
	if (afterInitLoad) {
		printf("Error getting global configuration information\n");
		return -2;
	}
	
	// Initialize socket descriptors and local Message counter for each node
	for(i = 0; i < MAX_NODES; i++)
	{
		nodes_sockfd[i] = -1;
		NodeRtngMsgCounter[i] = -1;
	}
		
	// Connect to neighbouring edges
	if (initialNodeConnect(myNodeNum)) {
		printf("Error when attempting to make initial connections\n");
		return -2;
	}
	
	// Create Deliver thread to deliver messages to Event Handler
	if( pthread_create(&tidDel,NULL,&Deliver,NULL) != 0 )
	{
		printf("Error creating deliver thread\n");
		return -1;
	}
	
	// Node 0 initializes routing for initial m0 nodes
	if( 0 == myNodeNum)
	{
		printf("Initializing routing\n");
		rt_init_routing();
	}

	// Initial Routing Table
	//rt_print();
	
	sleep(20);
	
	// Final routing table
	rt_print();
	
	while(1);

	return 0;
}