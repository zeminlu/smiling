#ifndef GROUPH_H_
#define GROUPH_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include "../inc/definitions.h"
#include "../inc/conditions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"

int serializeCountry(int country, void **buffer, int *bufferSize);

int serializeStruct(subFixture *group, void **buffer, int *bufferSize);

int unserializeAnswer(void *buffer, int bufferSize);

int unserializeHead(country *head, void *buffer, int bufferSize);

#endif
