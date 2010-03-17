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
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/serializable.h"

typedef int (*handler)(int in1,int in2);

typedef struct{
	gate *gates;
	int qtyGates;
	} gatesOfEachLevel;

typedef struct{
	gatesOfEachLevel *eachLevel;
	int level;
	int	totalLevels;
	int curLevel;
}circuitTable;

int initializeGates();
void freeCircuits( circuitTable **table, int qtyFile );
int getFilesAmm (DIR *dp);

circuitTable * parseXMLGate( char * docName );
circuitTable * parseCircuit( xmlDocPtr doc, xmlNodePtr cur );
void parseGatesTags( char *father, xmlNodePtr cur, circuitTable * circuit, int curLevel );
int getType( int typeInt);
int countLevels( circuitTable * circuit);
int checkGateIsLoaded( circuitTable *circuit, char *name, int curLevel);

void printCircuitTable( circuitTable * circuit);

#endif