#ifndef _PIPE_
#define _PIPE_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"

typedef int (*handler)(int in1,int in2);

/*
 *	\struct
 *		This struct contains the array of gates and the quantity.
 */

typedef struct{
	gate *gates;
	int qtyGates;
	} gatesOfEachLevel;

/*
 *	\struct
 *		This struct show each level os the circuit.
 */

typedef struct{
	gatesOfEachLevel *eachLevel;
	int	totalLevels;
}circuitTable;

/*
 *	\fn	int createsGates ( void )
 *
 *	\brief	This function creates the Gates proccess. It read the amount of files
 *			that is in the pipeDir folder and then create the gates proccess. It works
 *			in blocks, in other words if there are 5 files, it load all of the files and
 *			create the proccess. Then delete de table and wait for more files.
 *			When appear more files in the folder, it does the same step, that mentioned before.
 *	
 *	\code
 *		
 *			int main ( void )
 *			{
 *				...
 *				return createGates();
 *			}
 *	\endcode
 */

int createsGates(void);

/*
 *	\fn		int createTable( void )
 *
 *	\brief	This function has the task of creating the circuit table.
 *
 *	\code
 *		
 *			int main (void)
 *			{
 *				...
 *				if( createTable() != 0 )
 *					error
 *				...
 *			}
 *	\endcode
 */

int createTable( void );

/*
 * 	\fn	int fileListener( void )
 *
 *	\brief
 *		This function has the task of listening pipeDir folder, when a file
 *		appear, it call to the parser, and creates the circuit entry of the table.
 *	
 *	\code
 *		
 *		while( signal )
 *		{
 *			...
 *			if( fileListener() )
 *				...
 *		}
 *	\endcode
 *
 */

int fileListener( void );

/*
 *	\fn	int sendTableToGates( int pid )
 *	
 *	\brief	This function send to gates the whole table, to proccess.
 *	
 *	\param	The pid of gates proccess
 *
 *	\code
 *			
 *		while( signal )
 *		{
 *			if( fileListener() )
 *				...
 *			...
 *			if( sendTableToGates() )
 *				...
 *			...
 *		}
 *
 *	\endcode
 *		
 */

int sendTableToGates( int pid );

/*
 *	\fn		void freeCircuits( circuitTable **table, int qtyFile )
 *
 *	\brief	This function frees the whole table.
 *
 *	\param	The table of circuits
 *	\param	The amount of files
 */

void freeCircuits( circuitTable **table, int qtyFile );

/*
 *	\fn		int getFilesAmm( DIR *dp )
 *
 *	\brief	This function return the amount of files that are in the pipeDir folder
 *
 *	\param	The directory of pipeDir
 */

int getFilesAmm (DIR *dp);

/*
 *	\fn		circuitTable * parseXMLGate( char * docName )
 *
 *	\brief	This function starting the parser of the file and return the
 *			pointer to the structure of the circuit.
 *
 *	\param The name of the file to be proccessed
 */

circuitTable * parseXMLGate( char * docName );

/*
 *	\fn		circuitTable * parseCircuit( xmlDocPtr doc, xmlNodePtr cur )
 *
 *	\brief	This function parse an specific circuit.
 *
 *	\param	The pointer to the XML document
 *	\param	The current Node of the tree.
 */

circuitTable * parseCircuit( xmlDocPtr doc, xmlNodePtr cur );

/*
 *	\fn	void parseGatesTags( char *father, xmlNodePtr cur, circuitTable *circuit, int curLevel )
 *
 *	\brief	This function parses the tag of each gates. This function is recursive.
 *
 *	\param	The name of the father of the gates.
 *	\param	The current node of the tree.
 *	\param	The remaining circuit
 *	\param	The current level that it is proccessing
 */

void parseGatesTags( char *father, xmlNodePtr cur, circuitTable * circuit, int curLevel );

/*
 *	\fn int getType ( int typeInt )
 *
 *	\brief	This function return the type of the gates.
 *
 *	\param	The integer of the type.
 */

int getType( int typeInt);

/*
 *	\fn int countLevels( circuitTable *circuits )
 *
 *	\brief	This function return the amount of levels that this circuist has.
 *
 *	\param	The circuits
 */

int countLevels( circuitTable * circuit);

/*
 * 	\fn int checGateIsLoaded( circuitTable *circuit, char *name, int curLevel )
 *
 * 	\brief This function check if the gates was loaded before in the same level.
 *
 * 	\param 	The remaining circuit
 * 	\param	The name of the gate
 * 	\param	The current level that it is proccessing
 */

int checkGateIsLoaded( circuitTable *circuit, char *name, int curLevel);

/*
 *	\fn	void printCircuitTable( circuitTable *circuit )
 *
 *	\brief This function print the while circuit
 *
 *	\param	The circuit you want to print
 */

void printCircuitTable( circuitTable * circuit);

#endif
