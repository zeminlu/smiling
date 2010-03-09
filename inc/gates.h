/*
 *		lu.h
 *		
 */
#ifndef _LU_
#define _LU_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../inc/definitions.h"
#include "../inc/luGen.h"

float ** productTwoMatrix( float ** matrix1, float **matrix2, int qtyF1, int qtyC1, int qtyF2, int qtyC2 );

#endif