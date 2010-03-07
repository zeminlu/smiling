/*
 *		luGen.h
 */

#ifndef _LUGEN_
#define _LUGEN_

#include <stdio.h>
#include <stdlib.h>

#include "../inc/pipe.h"

void createInitCondFile( int cantVar, float *cond );

void createLUFile( float ** values, int cantVar );

void freeAllPtr( float ** values, float* initCond, int cantVar );

void printMatrix( float ** values, int row, int col );

void printInitCond( float * init, int row);

#endif