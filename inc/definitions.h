/*
 *	definitions.h
 *
 *		En este archivo se van a poner todos los define que se utilicen
 *		durante la programacion del tp.
 */

#ifndef _DECLARATIONS_
#define _DECLARATIONS_

#include "../inc/types.h"

typedef	char * String;

#define		TRUE		1
#define		FALSE		!TRUE
#define		_stdout_	1
#define		_stdin_		0


/* Gates */

enum 	logicGates { AND = 0, OR, XOR, NAND, NOR, XNOR };
#define		_INVALID_OUTPUT_		-1
#define		_INVALID_INPUT_			-1

#define 	_WEAK_GROUP_			4
#define 	_MAX_COUNTRIES_			32

#define		_MAX_GATES_LEVELS_		10

#endif

