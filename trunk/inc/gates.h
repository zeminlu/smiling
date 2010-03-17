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

int gateServer( void );
void freeCircuits( circuitTable **table, int qtyFile );

int getGateHandler( int type, int in1, int in2 );
int getInputFromFather( circuitTable **table, int level, int file, char fa[]);

int gateAnd( int in1, int in2 );
int gateOr( int in1, int in2 );
int gateXor( int in1, int in2 );
int gateNand( int in1, int in2 );
int gateNor( int in1, int in2 );
int gateXnor( int in1, int in2 );

#endif