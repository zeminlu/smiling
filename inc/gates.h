/**
 * \file gates.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
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

/**
 *	\fn int gateInitializer (void )
 *
 *	\brief 	This functions has the task of creating the pipeline.
 *			It uses a vector to control levels that have to run.
 *			Example:
 *				If there are 5 files, it will create a vector with 5 entries,
 *				and it save 5 int, in this order
 *					Position 0 = 0,
 *					Position 1 = -1,
 *						
 *						...
 *						
 *					Position 4 = -4.
 *				In every round this vector will be incremented in one. When all
 *				of levels were proccessed, it will finished and print results in
 *				standard output.
 *			If the table doesn't be created, It will return -1,
 *			otherwise return 0.
 *	
 *	\code
 *		
 *		if( addMoreFiles() != 0 )
 *		...
 *		return gateInitializer();
 *	
 *	\endcode					
 *
 */
int gateInitializer( void );

/**
 *	\fn int addMoreFiles (void )
 *
 *	\brief 	This functions has the task of loading the main
 *			table of circuits. It will receive the amount of files
 *			where pipe has parsed and then it calls the function 
 *			who will load data. It creates all of auxiliary vectors,
 *			childPids, levels and maxLevel, and initialize each one.
 *			It returns -1 if an error has occurred or
 *			0 if everything is fine.
 *	
 *	\code
 *		
 *		if( addMoreFiles() != 0 )
 *	
 *	\endcode
 *
 *	\sa	buildCircuitTable
 *
 */
int addMoreFiles( void );

/**
 * 	\fn	int buildCircuitsTable( void )
 *
 * 	\brief	This function has the task of loading the data of circuits.
 * 			It must be called by addMoreFiles. Every neccesary information
 * 			to build the table is declared as global. So the amount of files
 * 			and the table was declared behind.
 *
 * 	\code
 *
 * 		if( buildCircuitsTable() != 0 )
 *
 * 		...
 *
 * 	\endcode
 * 			
 */
int buildCircuitsTable( void );

/**
 * 	\fn	void freeCircuitsGates( void )
 *
 * 	\brief 	This function is the responsable of freeing the whole
 * 			table.
 *
 * 	\code
 *
 * 		freeCircuitsGates();
 *
 * 	\endcode
 */
void freeCircuitsGates( void );

/**
 * \fn int startCircuitsPipeline( void )
 *
 * \brief 	This function has the task of creating each level proccess.
 * 			It verifies is this file didn't finished and if it is in the
 * 			pipeline round.
 * 			It will return -1 if and error had occurred, 0 if not.
 * 	
 * 	\code
 * 		
 * 		while( !allFilesWasProccessed() )
 * 		{
 *			if( (status = startCircuitsPipeline()) != 0 )
 *			{
 *				...
 *			}
 *
 *			listenerToMyChildren()
 * 		}
 *
 * 	\endcode
 *
 */
int startCircuitsPipeline( void);

/**
 *	\fn	int listenToMyChildren( void )
 *
 * 	\brief 	This function has the task of load the information that
 * 			was proccessed by Level proccess. It will update level by
 * 			level.
 *
 * 	\code
 * 		if( (status = listenToMyChildren()) != 0 )
 *
 * 		...
 *
 * 		printResults
 * 	\endcode
 *
 */
int listenToMyChildren( void );

/**
 *	\fn	void incLevels ( void )
 *
 *	\brief	This function will increment in one each position
 *			of levels vector.
 *
 *	\code
 *		
 *		while( !allFileWasProccess() )
 *		{
 *			if( startCircuitsPipeline )
 *				...
 *			if( listenToMyChildren )
 *				...
 *			incLevels();
 *		}
 *	\endcode
 */
void incLevels( void );

/**
 *	\fn	void initLevels( void )
 *
 *	\brief	This function create the levels vector. This function must
 *			be create after buildCircuirtsTable functions.
 *			The structure of the vector if the following. If there are 3 files, 
 *			the vector contains
 *				Pos[0] = 0,
 *				Pos[1] = -1,
 *				Pos[2] = -2
 *	\code
 *		
 *		if( buildCircuitsTable() )
 *		...
 *
 *		initLevels();
 *	\endcode
 *
 */
void initLevels( void );

/**
 *	\fn int getCurrentPipeFiles( void )
 *
 *	\brief	This function return the amount of proccess that are running
 *			actually. This function is used in listenToMyChildren, to know
 *			how many entried of the table it has to update.
 *
 *	\code
 *		
 *		int amount = getCurrentPipeFiles();
 *
 *		while( amount > 0 )
 *		{
 *			 readInformation
 *		}
 *
 *	\endcode
 *
 */
int getCurrentPipeFiles( void );

/**
 *	\fn	int saveProccessFile( circuitTable *table, int pos )
 *
 *	\brief	This function stores the results of each circtuis in a file.
 *		It must be called after finish all of levels proccess.
 *	
 *	\param table The circuits table
 *	\param pos Index of the table
 *
 *	\code
 *		
 *		while( !allFilesWasProccessed() )
 *		{
 *			...
 *		}
 *
 *		for( i = 0 ; i < qtyFiles ; ++i )
 *			saveProccessFile( table[i], i );
 *	\endcode
 *
 */
int saveProccessFile( circuitTable *table, int pos );

/**
 *	\fn	int allFilesWasProccessed( void )
 *
 *	\brief	This function check if there is still some level of processing.
 *
 *	\code
 *		
 *		while( !allFileWasProccessed() )
 *
 *	\endcode
 *
 */
int allFilesWasProccessed( void );

#endif
