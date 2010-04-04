/**
 * \file levels.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef _LEVELS_
#define _LEVELS_ 

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/ipcAPI.h"

/**
 *	\fn 	int proccessLevel( void )
 *
 *	\brief	This function proccess the level. If the level is 0, it
 *			read only the level 0, if not it read the previously level
 *			and the current. It is the main function of this proccess.
 *
 *	\code
 *			int main ( void )
 *			{
 *				...
 *				return proccessLevel();
 *			}
 *	\endcode
 */
int proccessLevel( void );

/**
 *	\fn		void evaluateLevel( gate *prev, gate **cur, int qtyPrev, int qtyCur, int first )
 *
 *	\brief	This function evaluate the current level, using the previously if it
 *			neccesary. If you are proccessing the first level, you must put NULL
 *			in the first argument.
 *
 *	\param	prev The previously levels gates array
 *	\param	cur The direction of the current level gates array
 *	\param	qtyPrev The amount of previously gates
 *	\param	qtyCur The amount of current gates
 *	\param 	first TRUE if it's the first one, false if not.
 *
 * 	\code
 * 		
 * 		if( isFirst )
 * 			evaluateLevel(NULL, cur, n, m);
 * 		else
 * 			evaluateLevel(prev, cur, n, m);
 *	\endcode
 *
 */
void evaluateLevel( gate *prev, gate **cur, int qtyPrev, int qtyCur, int first );

/**
 *	\fn	int getGateHandler( int type, int in1, int in2 )
 *
 *	\brief	This function has the task of calling the appropiate
 *			gateHandler.
 *	
 *	\param	type The type of the gate
 *	\param	in1 The first input value
 *	\param 	in2 The second input value
 *
 */	
int getGateHandler( int type, int in1, int in2 );

/**
 *	\fn	int getInputFromFather( gate *gates, int qtyGates, char fa[])
 *
 *	\brief	This function has the task of searching the input of
 *			specific gate. The input was looked in the previously
 *			level.
 *	\param	gates The gates array
 *	\param	qtyGates The amount of gates
 *	\param	fa The array of previously level. It is used to search the input
 *
 */
int getInputFromFather( gate *gates , int qtyGates, char fa[]);

/**
 *	\fn int gateAnd( int in1, int in2 )
 *
 *	\brief This function evaluate the AND gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateAnd( int in1, int in2 );

/**
 *	\fn int gateOr( int in1, int in2 )
 *
 *	\brief This function evaluate the OR gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateOr( int in1, int in2 );

/**
 *	\fn int gateXor( int in1, int in2 )
 *
 *	\brief This function evaluate the XOR gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateXor( int in1, int in2 );

/**
 *	\fn int gateNand( int in1, int in2 )
 *
 *	\brief This function evaluate the NAND gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateNand( int in1, int in2 );

/**
 *	\fn int gateNor( int in1, int in2 )
 *
 *	\brief This function evaluate the NOR gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateNor( int in1, int in2 );

/**
 *	\fn int gateXnor( int in1, int in2 )
 *
 *	\brief This function evaluate the XNOR gate
 *
 *	\param	in1 first input of the gate
 *	\param	in2 second input of the gate
 */
int gateXnor( int in1, int in2 );

#endif
