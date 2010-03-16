#ifndef IPCAPI_H_
#define IPCAPI_H_

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
#include "../inc/definitions.h"
#include "../inc/types.h"

int setupIPC(int mode, int *ipcIDs, char *childName, int *childPID);

int readIPC(int ipcID, void *buffer, int bufferSize);

int writeIPC(int ipcID, void *buffer, int bufferSize);

int closeIPC(int ipcID);

void * prepareIPC(int **ipcIDs, int amm, int *allocSize);

int selectIPC(void *data, int seconds);

int getIPCStatus(int ipcID, void *data);

#endif