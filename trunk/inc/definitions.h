/*
 * \file
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H

#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

#include "../inc/types.h"

/**
 * \def
 *		Description here
 */
#define		_MAX_FATHER_		15

/**
 * \def
 *		Description here
 */
#define		TRUE		1

/**
 * \def
 *		Description here
 */
#define		FALSE		!TRUE

/**
 * \def
 *		Description here
 */
#define		_stdout_	1

/**
 * \def
 *		Description here
 */
#define		_stdin_		0

/**
 * \def
 *		Description here
 */
#define		_FORK_ERROR_	-1

/**
 * \def
 *		Description here
 */
#define		_FORK_SON_		0

/* Gates */

/**
 * \enum
 *		Description here
 */
enum 	logicGates { AND = 0, OR, XOR, NAND, NOR, XNOR };

/**
 * \def
 *		Description here
 */
#define		_INVALID_OUTPUT_		-1

/**
 * \def
 *		Description here
 */
#define		_INVALID_INPUT_			-1

/**
 * \def
 *		Description here
 */
#define		_MAX_FATHER_			15

/**
 * \def
 *		Description here
 */
#define		_WEAK_GROUP_TYPE_		10

/**
 * \def
 *		Description here
 */
#define		_DEATH_GROUP_TYPE_		11

/**
 * \def
 *		Description here
 */
#define		_SAME_CONTINENT_TYPE_	12

/**
 * \def
 *		Description here
 */
#define		_CHAMP_GROUP_TYPE_		13

/**
 * \def
 *		Description here
 */
#define		_NO_CONDITIONS_TYPE_	14
	
/**
 * \def
 *		Description here
 */
#define 	_WEAK_GROUP_			4

/**
 * \def
 *		Description here
 */
#define 	_MAX_COUNTRIES_			32

/**
 * \def
 *		Description here
 */
#define		_START_HASH_			50

/**
 * \def
 *		Description here
 */
#define		_HALF_DUPLEX_			0

/**
 * \def
 *		Description here
 */
#define		_FULL_DUPLEX_			1

/**
 * \def
 *		Description here
 */
#define		_MAX_GATES_LEVELS_		30

/**
 * \def
 *		Description here
 */
#define		_MAX_CIRCUITS_			5

/**
 * \def
 *		Description here
 */
#define 	_MAX_LEVELS_			50

/*	Types of Connection */
	
/**
 * \enum
 *		Description here
 */	
enum	typeOfConnection	{ FIFO, SH_MEM, MSG_QUEUE, SOCKET };

/**
 * \fn
 *
 * 		\brief
 *
 * 		\param
 * 		
 * 		\return
 *
 * 		Use:
 * 		\code
 * 		
 *		\endcode
 *
 * 		\sa
 *
 */
void varFree(int amm, ...);

#endif
