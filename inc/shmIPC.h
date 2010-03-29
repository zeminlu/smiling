#ifndef SHMIPC_H_
#define SHMIPC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/linearHashADT.h"
#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"

#define ERR ((struct databuf *) -1)

typedef struct{
	pid_t pid;
	void *startPos;
	int sReadOff;
	int sWriteOff;
	int cReadOff;
	int cWriteOff;
	int semA;
	int semB;
} shmHeader;

typedef struct{
	int index;
	int *read;
	int *write;
	int *otherRead;
	int *otherWrite;
	int semA;
	int semB;
	} shmElem;

int compareIPCIDs(void *elem1, void *elem2);

void * copyIPCID(void *elem);

void freeIPCID(void *elem);

#endif