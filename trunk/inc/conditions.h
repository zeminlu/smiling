#ifndef CONDITIONS_H_
#define CONDITIONS_H_

#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "../inc/types.h"
#include "../inc/definitions.h"

void * sameContinent(void * cond);

void * deathGroup(void * cond);

void * champGroup(void * cond);

void * weakGroup(void * cond);

void * noCondition(void * cond);
#endif
