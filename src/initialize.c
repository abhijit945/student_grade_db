/* file : initialize.c
 * 
 * Initializes Project configuration parameters
 * Reads nodes address from config file
 * connects nodes:
 *    m0 nodes connect using initial adjacency matrix(initialConnect.txt).
 *    non m0 nodes connect by making JOIN request.
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

int nodeID;
pthread_t udp_thread, tcp_thread, tcp_conn_thread, queryProc_thread;

void *connect_nodes();

// Read Nodes addresses from CONF_FILE
// returns 0 on success; negetive integer value to indicate failure/error
int getNodesAddr() {
	FILE *conf_file;
	int cur_node, temp_node_no, temp_port;
	char temp_node_addr[20];

	conf_entries = 0;
	// Read from configuration file
	conf_file = fopen(CONF_FILE, "r");
	if (conf_file == NULL) {
		printf("Error in opening file :%s\n", CONF_FILE);
		return -2;
	}

	// Store the information into array nodes.
	cur_node = -1;
	while (fscanf(conf_file, "%d %s %d", &temp_node_no, temp_node_addr,
			&temp_port) != EOF) {
		cur_node++;
		nodes[cur_node].sin_addr.s_addr = inet_addr(temp_node_addr);
		nodes[cur_node].sin_family = AF_INET;
		nodes[cur_node].sin_port = htons(temp_port);

	};

	conf_entries = cur_node + 1;

	printf("\nConfiguration stored for %d nodes\n", cur_node + 1);
	fclose(conf_file);
	return 0;
}

//Read the inital input parameters for the total number of nodes and the inital nodes
// returns 0 on success
// returns negetive integer on error
int getInitialNodeParams() {
	FILE *prj_conf_file;

	char temp_str[15];
	int temp_var;

	prj_conf_file = fopen(PRJ_CONF_FILE, "r");

	if (prj_conf_file == NULL) {
		printf("\nUnable to open file :%s\n", PRJ_CONF_FILE);
		return -2;
	}

	while (fscanf(prj_conf_file, "%s %d", temp_str, &temp_var) != EOF) {
		if (strcmp(temp_str, "initial") == 0) {
			initialNodeCount = temp_var;
		} else if (strcmp(temp_str, "edges") == 0) {
			edgeCount = temp_var;
		} else if (strcmp(temp_str, "nodes") == 0) {
			totalNodes = temp_var;
		}
	}

	fclose(prj_conf_file);

	printf("\nThe Total number of allowed nodes in the Network: %d\n", totalNodes);
	printf("\nThere are %d initial nodes in the network\n", initialNodeCount);
	printf("\nNumber of edges are: %d\n", edgeCount);
	return 0;
}

// connect to Nodes
// returns 0 on success
// On error it returns negetive integer.
int initialNodeConnect(int myNodeNum) {

	nodeID = myNodeNum;
	printf("\nCollecting information for initial connection setup\n");
	FILE *init_connect_file;
	int array[50][50] = { { 0 } }, i = 0, j = 0;
	int sockfd_udp = 0, n;
	char recvline[1000];
	char recvdData[1024];
	struct sockaddr_in serv_addr;

	init_connect_file = fopen(INIT_CONNECT_FILE, "r");

	if (init_connect_file == NULL) {
		printf("\nUnable to open file :%s\n", INIT_CONNECT_FILE);
		return -2;
	}

	// Read adjacency matrix from initial connect file
	for (i = 0; i < initialNodeCount; i++) {
		for (j = 0; j < initialNodeCount; j++) {
			fscanf(init_connect_file, "%d", &array[i][j]);
		}
	}

	fclose(init_connect_file);
	printf("\nConnecting to the nodes\n");
	for (i = 0; i < initialNodeCount; i++) {
		printf("\n");
		for (j = 0; j < initialNodeCount; j++) {
			printf("%d ", array[i][j]);
		}
	}

	printf("\nRetrieved the nodes information\n");

	printf("\nConnecting to following Nodes: \n");

	//To accept connections from other nodes
	pthread_create(&tcp_thread, NULL, tcp_reciever, NULL);

	//To accept connections from non m0 node
	pthread_create(&udp_thread, NULL, udp_handler, NULL);

	//Start the query thread for UDP based queries
	pthread_create(&queryProc_thread, NULL, queryProc_handler, NULL);

	printf("\nSleeping for 7 secs, please start your m0 Nodes...\n\n");
	sleep(7);
	rt_add_entry(nodeID,0,nodeID,0);

	if (nodeID < initialNodeCount) {

		//For the m0 nodes we can directly provide a TCP connection
		for (i = 0; i < initialNodeCount; i++)
		{
			int temp = 0;
			if( array[nodeID][i] == 0 || (nodeID == i) )
				continue;
			temp = i;
			printf("\n%d -> IP:PORT %s:%d\n", temp, inet_ntoa(nodes[temp].sin_addr),
					(int) ntohs(nodes[temp].sin_port));

			//Connect to other m0 nodes
			int *tempNode = (int*)malloc(sizeof(int));
			*tempNode = temp;
			pthread_create(&tcp_conn_thread, NULL, connect_nodes,
					(void *) tempNode);
			sleep(5);
		}
	} else {

		// connecting non m0 nodes

		int connectToNodes[RT_MAX_SIZE];
		char *tempTkPtr;
		printf("Waiting for %d seconds",myNodeNum * 15);
		sleep(40+(myNodeNum-initialNodeCount) * 15);

		//Get Random m0 Node and select the IP to connect
		//For non m0 nodes we need a UDP connection setup
		srand(time(NULL));
		int rndm = rand() % initialNodeCount;
		printf("\nThis is a Non m0 Node, Selecting a random Node for request\n");
		printf("\n%d selected\n", rndm);
		printf("\n%d -> IP:PORT %s:%d\n", rndm, inet_ntoa(nodes[rndm].sin_addr),
				(int) ntohs(nodes[rndm].sin_port));

		//Initiate a UDP connection for handshake
		sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = nodes[rndm].sin_addr.s_addr;
		serv_addr.sin_port = nodes[rndm].sin_port + 1; // Default UDP Port used

		printf("\nSending JOIN_REQ\n");
		sendto(sockfd_udp, "UDP_JOIN_REQ", 1000, 0,
				(struct sockaddr *) &serv_addr, sizeof(serv_addr));

		n = recvfrom(sockfd_udp, recvline, sizeof(recvline), 0, NULL, NULL);
		recvline[n] = 0;

		// Check if maximum nodes are already connected
		if(strcmp(recvline, "REJECT_JOIN") == 0){
			fputs(recvline, stdout);
			printf("\nMaximum number of nodes in the network reached...\n Exiting\n");
			gracefulExit();
		}

		// We have received nodes to be connected.
		printf("Recieved msg %s",recvline);
		strcpy(recvdData,recvline);
		tempTkPtr = strtok(recvdData," ");

		for(i=0; i<edgeCount; i++)
		{
			tempTkPtr = strtok(NULL," ");
			connectToNodes[i] = atoi(tempTkPtr);
		}

		// connect to designated nodes.
		for(i=0; i<edgeCount; i++)
		{
			int *tempNode = (int*)malloc(sizeof(int));
			char sendData[1024],tempStr[20];

			*tempNode = connectToNodes[i];
			pthread_create(&tcp_conn_thread, NULL, connect_nodes,(void *) tempNode);

			pthread_join(tcp_conn_thread,NULL);

			sprintf(sendData,"%s","CONNECT");
			sprintf(tempStr," %d",myNodeNum);
			strcat(sendData,tempStr);
			send(nodes_sockfd[connectToNodes[i]], sendData, strlen(sendData)+1,0);
			usleep(1000);

		}

		// wait for connection establishment.
		sleep(5);

		// start routing
		rt_init_routing();
	}

	return 0;
}

// This function establishes connection for a given node.
// returns pointer to an integer containing result value
void *connect_nodes(void* temp) {

	int sockfd, n;
	struct sockaddr_in serv_addr;
	char recvline[1000];
	int remote_node_no = *(int *) temp;
	int *ret_val = (int*)malloc(sizeof(int));

	*ret_val = 0;
	int *tempPtr = (int*) temp;
	free(tempPtr);
	printf("\nThread Created\n");
	printf("\nConneting to %d -> IP:PORT %s:%d\n", remote_node_no,
			inet_ntoa(nodes[remote_node_no].sin_addr),
			(int) ntohs(nodes[remote_node_no].sin_port));

	// Make a TCP connection
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = nodes[remote_node_no].sin_addr.s_addr;
	serv_addr.sin_port = nodes[remote_node_no].sin_port;

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) {
		printf("\n Error : Connect Failed, please check if the server at \"%s\" is up\n",
				inet_ntoa(nodes[remote_node_no].sin_addr));
		*ret_val = -1;
		return ret_val;
	}

	printf("\nConnected\n");

	// Store the socket file descriptor for future communications
	nodes_sockfd[remote_node_no] = sockfd;
	rt_add_entry(remote_node_no,1,remote_node_no,0);
	rt_increment_degree();

	// Make newly connected node neighbour
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