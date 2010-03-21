#ifndef PIPEIPC_H_
#define PIPEIPC_H_

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
#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/linearHashADT.h"
#include "../inc/ipcAPI.h"

int compareIPCIDs(void *elem1, void *elem2);

void * copyIPCID(void *elem);

void freeIPCID(void *elem);

void itoa(int n, char s[]);

void reverse(char s[]);

#endif