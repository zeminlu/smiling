/*
 *	levels.h
 */
#ifndef _LEVELS_
#define _LEVELS_ 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/ipcAPI.h"

int proccessLevel( void );
void evaluateLevel( gate *prev, gate **cur, int qtyPrev, int qtyCur, int first );

int getGateHandler( int type, int in1, int in2 );
int getInputFromFather( gate *gates , int qtyGates, char fa[]);

int gateAnd( int in1, int in2 );
int gateOr( int in1, int in2 );
int gateXor( int in1, int in2 );
int gateNand( int in1, int in2 );
int gateNor( int in1, int in2 );
int gateXnor( int in1, int in2 );

#endif