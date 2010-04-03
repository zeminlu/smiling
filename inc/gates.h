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
#include <pthread.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/serializable.h"
#include "../inc/pipe.h"
#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"

int gateInitializer( void );

int addMoreFiles( void );

int buildCircuitsTable( void );

void freeCircuitsGates( void );

int startCircuitsPipeline( void);

int listenToMyChildren( void );

void initLevels( void );

void incLevels( void );

int checkAvailableFiles( void );

int getCurrentPipeFiles( void );

int allFilesWasProccessed( void );

void itoa(int n, char s[]);

#endif