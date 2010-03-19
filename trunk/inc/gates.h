/*
 *		gates.h
 *		
 */
#ifndef _GATES_
#define _GATES_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/serializable.h"
#include "../inc/pipe.h"
#include "../inc/ipcAPI.h"

int gateInitializer( void );
int buildCircuitsTable( circuitTable *** circuits, int qtyFileCom );
void freeCircuits( circuitTable **table, int qtyFile );
int startCircuitsPipeline( circuitTable **table, pid_t **childPids, int ***ipcChannels, int qtyFiles,
 						int *levels, int *maxLevel);
int listenToMyChildren( void *set, void *master, int allocSize, circuitTable **table, int **ipcChannels, 
						int *childPids, int qtyFiles, int *levels, int *maxLevel);

void initLevels( int **levels, int qtyFiles );
void incLevels( int **levels, int qtyFiles );
int checkAvailableFiles( int * levels, int qtyFiles, int *maxLevel );

#endif