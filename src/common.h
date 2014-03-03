// Common header file

#ifndef COMMON_H_
#define COMMON_H_

#define CONF_FILE "config.txt"
#define PRJ_CONF_FILE "prj_config.txt"
#define INIT_CONNECT_FILE "initialConnect.txt"

#define MAX_NODES 100

enum eventType {ROUTING,CONNECT};
enum queryType {
	GET_DEGREE = 1, GET_FARTHEST = 2, GET_RTABLE = 3
};

extern struct sockaddr_in nodes[MAX_NODES];
extern int nodes_sockfd[MAX_NODES];
extern int myNodeNum;
extern int conf_entries;
extern int initialNodeCount;
extern int edgeCount;
extern int totalNodes;
extern int afterInitLoad;
extern pthread_t udp_thread, tcp_thread, tcp_call_thread, queryProc_thread;

extern int neighbour[MAX_NODES];
extern int num_nbrs;

extern int NodeRtngMsgCounter[MAX_NODES];

extern pthread_mutex_t lockMsgQueue;
extern pthread_mutex_t lockHndlEvnt;

extern void gracefulExit();

extern int getNodesAddr();
extern int getInitialNodeParams();
extern int initialNodeConnect(int myNodeNum);

extern void * tcp_reciever();
extern void * udp_handler();
extern void * queryProc_handler();
extern void * query_handler();
extern void * Deliver(void *);

extern int insertRecMsgQ(char *);
extern int getHeadRecMsgQ(char *);

#endif /*COMMON_H_*/
