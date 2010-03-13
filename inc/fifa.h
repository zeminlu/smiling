#ifndef FIFA_H_
#define FIFA_H_

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
#include "../inc/tpl.h"

int fifoServer();

int serializeHead(country *head, void **buffer, int *bufferSize);

int unserializeSubfixture(void *buffer, country **subFixture);

int unserializeStruct(void *buffer, country *subFixture);

int serializeAnswer(int status, void **buffer, int *bufferSize);

int unserializeCountry(void *buffer);

#endif
