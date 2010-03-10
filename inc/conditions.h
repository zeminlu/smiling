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

int sameContinent(condPack * cond);

int deathGroup(condPack * cond);

int champGroup(condPack * cond);

int weakGroup(condPack * cond);

int noCondition(condPack * cond);
#endif
