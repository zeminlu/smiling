#ifndef _SERIALIZABLE
#define _SERIALIZABLE_


#include <stdlib.h>
#include <stdio.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"


int serializeSubfixture(void **buffer, int *bufferSize, country **subFixture);

int serializeSubfixture(void **buffer, int *bufferSize, subFixture *group);

int unserializeSubfixture(void *buffer, int bufferSize, country **subFixture);

int serializeCountryStruct(void **buffer, int *bufferSize, country *country);

int unserializeCountryStruct(void *buffer, int bufferSize, country *country);

int serializeInteger(void **buffer, int *bufferSize, int number);

int unserializeInteger(void *buffer, int bufferSize);

#endif