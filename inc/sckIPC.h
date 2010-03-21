#ifndef SCKIPC_H_
#define SCKIPC_H_

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
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <sys/un.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/ipcAPI.h"
#include "../inc/linearHashADT.h"
#include "../inc/cutils.h"

int compareIPCIDs(void *elem1, void *elem2);

void * copyIPCID(void *elem);

void freeIPCID(void *elem);


#endif