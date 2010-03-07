#ifndef _PIPE_
#define _PIPE_

#include <stdio.h>
#include <dirent.h>

int getFilesAmm (DIR *dp);

float ** loadEquationFile( FILE * ecuFile );

float * loadInitCondition ( FILE *initFile );

#endif