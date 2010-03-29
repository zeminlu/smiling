/*
 *	definitions.h
 *
 *		En este archivo se van a poner todos los define que se utilicen
 *		durante la programacion del tp.
 */

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#include <stdlib.h>
#include "../inc/types.h"
#include <stdarg.h>

#define		TRUE		1
#define		FALSE		!TRUE
#define		_stdout_	1
#define		_stdin_		0

#define		_FORK_ERROR_	-1
#define		_FORK_SON_		0

#define		_SHM_SEG_SIZE_ 8000


/* Gates */

enum 	logicGates { AND = 0, OR, XOR, NAND, NOR, XNOR };

#define		_INVALID_OUTPUT_		-1
#define		_INVALID_INPUT_			-1
#define		_MAX_FATHER_			15

#define		_WEAK_GROUP_TYPE_		10
#define		_DEATH_GROUP_TYPE_		11
#define		_SAME_CONTINENT_TYPE_	12
#define		_CHAMP_GROUP_TYPE_		13
#define		_NO_CONDITIONS_TYPE_	14
	
#define 	_WEAK_GROUP_			4
#define 	_MAX_COUNTRIES_			32

#define		_HALF_DUPLEX_			0
#define		_FULL_DUPLEX_			1

#define		_MAX_GATES_LEVELS_		10
#define		_MAX_CIRCUITS_			5

#define 	_MAX_LEVELS_			50

/*	Types of Connection */
	
enum	typeOfConnection	{ FIFO, SH_MEM, MSG_QUEUE, SOCKET };

void varFree(int amm, ...);

#endif

