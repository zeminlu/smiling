#ifndef _PIPE_
#define _PIPE_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>

#include "../inc/definitions.h"

int getFilesAmm (DIR *dp);

int gateAnd( int in1, int in2 );
int gateOr( int in1, int in2 );
int gateXor( int in1, int in2 );
int gateNand( int in1, int in2 );
int gateNor( int in1, int in2 );
int gateXnor( int in1, int in2 );

#endif