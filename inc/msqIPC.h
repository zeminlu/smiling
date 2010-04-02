
#ifndef MSQIPC_H_
#define MSQIPC_H_


#include <sys/ipc.h>
#include <mqueue.h>

#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"
#include "../inc/linearHashADT.h"


#define QKEY 			(key_t)0105
#define QPERM			0666

typedef struct q_entry{
	long mtype;
	char mtext[200];
	}msQ;

int init_queue(int newKey);
int compareIPCIDs(void *elem1, void *elem2);

void * copyIPCID(void *elem);

void freeIPCID(void *elem);

#endif