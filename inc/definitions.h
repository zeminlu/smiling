/**
 * \file definitions.h
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
 * \def TRUE
 *		\brief TRUE define
 */
#define		TRUE		1

/**
 * \def FALSE
 *		\brief FALSE define
 */
#define		FALSE		!TRUE

/**
 * \def _stdout_
 *		\brief Define of the standard output descriptor
 */
#define		_stdout_	1

/**
 * \def _stdin_
 *		\brief Define of the standard input descriptor
 */
#define		_stdin_		0

/**
 * \def _FORK_ERROR_
 *		\brief Fork error return define
 */
#define		_FORK_ERROR_	-1

/**
 * \def _FORK_SON_
 *		\brief Fork child return define
 */
#define		_FORK_SON_		0

/* Gates */

/**
 * \enum logicGates
 *		\brief Description here
 */
enum 	logicGates { AND = 0, OR, XOR, NAND, NOR, XNOR };

/**
 * \def _INVALID_OUTPUT_
 *		\brief Description here
 */
#define		_INVALID_OUTPUT_		-1

/**
 * \def _INVALID_INPUT_
 *		\brief Description here
 */
#define		_INVALID_INPUT_			-1
	
/**
 * \def _WEAK_GROUP_
 *		\brief Constant that defines the maximum weight of a weak country.
 */
#define 	_WEAK_GROUP_			4

/**
 * \def _MAX_COUNTRIES_
 *		\brief Max countries ammount define
 */
#define 	_MAX_COUNTRIES_			32

/**
 * \def _START_HASH_
 *		\brief Define of the multiplier for the initial buckets in the hashTable.
 */
#define		_START_HASH_			50

/**
 * \def _MAX_GATES_LEVELS_
 *		\brief Description here
 */
#define		_MAX_GATES_LEVELS_		30

/**
 * \def _MAX_CIRCUITS_
 *		\brief Description here
 */
#define		_MAX_CIRCUITS_			5

/**
 * \def _MAX_LEVELS_
 *		\brief Description here
 */
#define 	_MAX_LEVELS_			50

/*	Types of Connection */
	
/**
 * \enum typeOfConnection
 *		\brief Description here
 */	
enum	typeOfConnection	{ FIFO, SH_MEM, MSG_QUEUE, SOCKET };

/**
 * \fn void varFree(int amm, ...)
 *
 * 		\brief This function frees up the variable pointers given as arguments
 *
 * 		\param amm The ammount of pointers being passed to this function
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
