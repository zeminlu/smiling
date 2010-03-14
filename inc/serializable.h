#ifndef _SERIALIZABLE
#define _SERIALIZABLE_

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"

int serializeGate( gate * gate, void **buffer, int *bufferSize);

#endif