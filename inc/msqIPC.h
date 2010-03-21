
#ifndef MSQIPC_H_
#define MSQIPC_H_


#include <sys/ipc.h>
#include <sys/msg.h>
#include "../inc/ipcAPI.h"

#define QKEY 			(key_t)0105
#define QPERM			0660

typedef struct q_entry{
	long mtype;
	char * msg;
	}msQ;

int init_queue(int newKey);

#endif