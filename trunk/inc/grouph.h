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
#include <time.h>

#include "../inc/definitions.h"
#include "../inc/conditions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"

void sendErrorToParent(int error);

int loadHeadAndCountriesTable(country ***countriesTable, country **head);

int checkConditions(country *data, void *(***conditions)(void *condArgs));

int buildCondArgs(condPack **condArgs, country **countriesTable, int countriesTableEntriesAmm, country *data, int condAmm, int *index);

int prepareGroup(subFixture **group, country *data);

int buildSubfixture(subFixture **group, int condAmm, condPack *condArgs, country *data, country **countriesTable, void *(**conditions)(void *condArgs));

int intersect(int condAmm, condPack *condArgs, set **intersection);

int sendSubfixture(subFixture *group);

void sortPointers(set **sets);

#endif
