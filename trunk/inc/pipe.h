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

typedef int (*handler)(int in1,int in2);

typedef struct{
	gate *gates;
	int qtyGates;
	} gatesOfEachLevel;

typedef struct{
	gatesOfEachLevel *eachLevel;
	int level;
	int	totalLevels;
}circuitTable;

int getFilesAmm (DIR *dp);

circuitTable * parseXMLGate( char * docName );
circuitTable * parseCircuit( xmlDocPtr doc, xmlNodePtr cur );
void parseGatesTags( char *father, xmlNodePtr cur, circuitTable * circuit, int curLevel );
handler getHandler( char * typeH, int *typeInt);
int countLevels( circuitTable * circuit);
int checkGateIsLoaded( circuitTable *circuit, char *name, int curLevel);

int gateAnd( int in1, int in2 );
int gateOr( int in1, int in2 );
int gateXor( int in1, int in2 );
int gateNand( int in1, int in2 );
int gateNor( int in1, int in2 );
int gateXnor( int in1, int in2 );

void printCircuitTable( circuitTable * circuit);

#endif