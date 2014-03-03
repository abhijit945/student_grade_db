// Header file for Routing functions

#ifndef ROUTINGFNS_H_
#define ROUTINGFNS_H_

#define RT_MAX_SIZE 100

struct RoutingLink
{
	int dest;
	int cost;
	int nextHop;
	int degree;
};

struct RoutingInfo
{
	int sender;
	int originator;
	int counter;
	int init;
	int entries;
	struct RoutingLink *rt_sender;
};

extern struct RoutingLink RoutingTable[RT_MAX_SIZE];
extern struct RoutingInfo senderRtngInfo;

extern int rt_num_entries;

extern int rt_add_entry(int new_dest,int new_cost,int new_nextHop, int new_degree);

extern int rt_init_routing();

extern int sendRoutingInfo(int src, struct RoutingInfo *senderRtngInfo);

extern int rt_increment_degree();

extern int rt_print();

#endif  // 