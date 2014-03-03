/* file : Queue.c
 * 
 * Queue to buffer received messages
 * Receiver threads insert messages received into Queue to have more response time for sender
 * Deliver thread process these messeges
 *  
 */

#include<malloc.h>
#include<string.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#include "common.h"

// structure definition for message Node 
typedef struct RecMsgQueue
{
	char recMsg[1024];
	struct RecMsgQueue *nextMsg;
} RecMsgQ;

// Received Messages Queue
RecMsgQ *RecMsgQHead = NULL;
RecMsgQ *RecMsgQTail = NULL;

// Method for message insertion
// returns 0 on success
// non-zero integer on failure/error
int insertRecMsgQ(char *RecMessage)
{
	RecMsgQ *ptrNewNode = NULL;
	int retval = 0;
	
	pthread_mutex_lock(&lockMsgQueue);
	ptrNewNode = (RecMsgQ *) malloc( sizeof(RecMsgQ));
	strcpy(ptrNewNode->recMsg,RecMessage);
	ptrNewNode->nextMsg = NULL;
	
	//printf("Inserting %s\n",ptrNewNode->recMsg);
	
	// If Queue is empty
	if(RecMsgQHead == NULL)
	{
		RecMsgQHead = ptrNewNode;
		RecMsgQTail = ptrNewNode;
		retval = 0;
	}
	else
	{
		RecMsgQTail->nextMsg = ptrNewNode;
		RecMsgQTail = ptrNewNode;
		retval = 0;
	}
	pthread_mutex_unlock(&lockMsgQueue);
	return retval;
}

// Method for reading message at the head of the Queue
// returns 0 on success
// non-zero integer on failure/error
int getHeadRecMsgQ(char *h_Msg)
{
	int retval = 0;
	pthread_mutex_lock(&lockMsgQueue);
	
	// If Queue is empty
	if(RecMsgQHead == NULL)
	{
		retval = 1;
		
	}else if(RecMsgQHead == RecMsgQTail)
	{
		// Only 1 node is present in the Queue
		strcpy(h_Msg,RecMsgQHead->recMsg);
		RecMsgQHead = NULL;
		free(RecMsgQTail);
		RecMsgQTail = NULL;
		retval = 0;
	}else
	{
		RecMsgQ *tempRecMsgNode = RecMsgQHead;
		strcpy(h_Msg,RecMsgQHead->recMsg);
		RecMsgQHead = RecMsgQHead->nextMsg;
		free(tempRecMsgNode);
		retval = 0;
	}
	pthread_mutex_unlock(&lockMsgQueue);
	
	return retval;
}