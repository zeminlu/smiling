#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../inc/types.h"
#include "../inc/definitions.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"

int getFilesAmm (DIR *dp);

#endif
