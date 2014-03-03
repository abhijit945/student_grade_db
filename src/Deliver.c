/*
 * file: Deliver.c
 * 
 * Thread to deliver messages from Queue to State machine. 
 */

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<malloc.h>
#include<stdlib.h>
#include<string.h>

#include "common.h"
#include "RoutingFns.h"

extern int handleEvent(enum eventType type,struct RoutingInfo senderRtngInfo,int connTo);

// Read messages from Queue and call handleEvent to process these messages.
void* Deliver(void * arg)
{
	char h_Msg[1024],msgSenderMsgType[20];
	int msgSender,msgOriginator,msgCounter,msgInit,msgRtEntries;
	struct RoutingInfo senderRtngInfo;
	char curMsg[1024],*tknPtr;
	int entDest,entCost,entnextHop, entDegree;
	int i;

	printf("Start Delivery of messages to State Machine\n");

	while(1)
	{
		// Read message from head of the Queue
		if(!getHeadRecMsgQ(h_Msg))
		{
			printf(">>>>>>>Delivery of msg %s\n",h_Msg);

			// If the message type is Routing then create senderRtngInfo structure to 
			// pass the information to handleEvent
			if(!strncmp(h_Msg,"ROUTING",7))
			{

				sscanf(h_Msg,"%s %d %d %d %d %d",msgSenderMsgType,&msgSender,&msgOriginator,&msgCounter,&msgInit,&msgRtEntries);

				// extract different parameters from message
				senderRtngInfo.sender = msgSender;
				senderRtngInfo.originator = msgOriginator;
				senderRtngInfo.counter = msgCounter;
				senderRtngInfo.init = msgInit;
				senderRtngInfo.entries = msgRtEntries;

				senderRtngInfo.rt_sender = (struct RoutingLink*) malloc(sizeof(struct RoutingLink)*msgRtEntries);

				strcpy(curMsg,h_Msg);
				tknPtr = strtok(curMsg," ");
				for( i=0; i<5 ;i++ )
				{
					tknPtr = strtok(NULL," ");
				}

				// Read the routing table from message
				for(i=0; i<msgRtEntries; i++)
				{
					tknPtr = strtok(NULL," ");
					sscanf(tknPtr,"%d",&entDest);
					tknPtr = strtok(NULL," ");
					sscanf(tknPtr,"%d",&entCost);
					tknPtr = strtok(NULL," ");
					sscanf(tknPtr,"%d",&entnextHop);
					tknPtr = strtok(NULL," ");
					sscanf(tknPtr,"%d",&entDegree);
					senderRtngInfo.rt_sender[i].dest = entDest;
					senderRtngInfo.rt_sender[i].cost = entCost;
					senderRtngInfo.rt_sender[i].nextHop = entnextHop;
					senderRtngInfo.rt_sender[i].degree = entDegree;
				}
				printf("calling handleEvent\n");
				handleEvent(ROUTING,senderRtngInfo,0);
			}else if(!strncmp(h_Msg,"CONNECT",7))
			{
				// Message type is connect, non m0 node is connecting.
				sscanf(h_Msg,"%s %d",msgSenderMsgType,&msgSender);
				handleEvent(CONNECT,senderRtngInfo,msgSender);
			}
		}
		usleep(1000);
	}
	return NULL;
}