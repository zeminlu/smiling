#ifndef FIFA_H_
#define FIFA_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
/*#include <wait.h>*/
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"

int loadCountriesTable(country ***countriesTable);

int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids);

int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture);

#endif