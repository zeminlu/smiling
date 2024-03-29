/**
 * \file levels.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#include "../inc/levels.h"

int main ( void )
{
	int status;
	
	if( (status = loadIPC()) < 0 )
	{
		perror("Error en el LOAD de Gates");
		return status;
	}
	return proccessLevel();
}

/*
 *	Funcion que resuelve el nivel recibido por IPC. La funcion
 *	va a leer mediante IPC el nivel que tiene que procesar y el
 *	nivel anterior.
 *	Luego va a retornar por medio del IPC seleccionado el nivel
 *	ya procesado.
 */

int proccessLevel( void )
{
	int i, first, qtyGatesPrev, qtyGatesCur;
	gate *prevLevel = NULL, *curLevelTable;
	curCircuit cur;
	
	if( readIPC( getppid(), &cur, sizeof(curCircuit)) == -1 )
	{
		perror("Error en la lectura de LEVELS a GATE, cur\n");
		return errno;
	}
	if( readIPC( getppid(), &qtyGatesCur, sizeof(int)) == -1 )
	{
		perror("Error en la lectura de GATES a LEVELS #qtyGatesCur\n");
		return errno;
	}
	
	if( (curLevelTable = malloc( sizeof(gate) * qtyGatesCur)) == NULL )
	{
		perror("Error en la alocacion de memoria de prevLevel\n");
		return errno;
	}
	
	for( i = 0 ; i < qtyGatesCur ; ++i )
	{
		if( readIPC( getppid(), &(curLevelTable[i]), sizeof(gate) ) == -1 )
		{
			perror("Error en la lectura de las compuertas de Levels\n");
			return errno;
		}
	}
	
	if( readIPC( getppid(), &first, sizeof(int)) == -1 )
	{
		perror("Error en la lectura de si es el primero o no de Levels\n");
		return errno;
	}
	if( first != 0)
	{
		if( readIPC( getppid(), &qtyGatesPrev, sizeof(int)) == -1 )
		{
			perror("Error en la lectura de cantidad de compuertas de previously\n");
			return errno;
		}
	
		if( (prevLevel = malloc( sizeof(gate) * qtyGatesPrev)) == NULL )
		{
			perror("Error en la alocacion de memoria de prevLevel\n");
			free(curLevelTable);
			return errno;
		}
		for( i = 0 ; i < qtyGatesPrev ; ++i )
		{
			if( readIPC( getppid(), &(prevLevel[i]), sizeof(gate) ) == -1 )
			{
				perror("Error en la lecturas de las compuertas de prevLevel\n");
				return errno;
			}
		}
	}
	
	evaluateLevel( prevLevel, &curLevelTable, qtyGatesPrev, qtyGatesCur, first );
	
	if( writeIPC( getppid(), &cur, sizeof(curCircuit)) == -1 )
	{
		perror("Error en la escritura de LEVELS hacia GATES, cur\n");
		return errno;
	}
	for( i = 0 ; i < qtyGatesCur ; ++i )
	{
		if( writeIPC( getppid(), &(curLevelTable[i]), sizeof(gate) ) == -1 )
		{
			perror("Error en la escritura de LEVELS a GATES, cada compuerta\n");
			return errno;
		}
	}
	if( first != 0 )
		free(prevLevel);	
	free(curLevelTable);
	closeIPC();
	
	return 0;
}

/*
 *	Funcion encargada de evaluar el nivel y buscar en el nivel anterior las entradas correspondiente.
 */

void evaluateLevel( gate *prev, gate **cur, int qtyPrev, int qtyCur, int first )
{
	int i, *input1, *input2, type;
	char fa1[_MAX_FATHER_], fa2[_MAX_FATHER_];
	
	for( i = 0 ; i < qtyCur ; ++i )
	{
		type = (*cur)[i].type;
		input1 = &((*cur)[i].input[0]);
		input2 = &((*cur)[i].input[1]);
		strcpy( fa1, (*cur)[i].fathers[0] );
		strcpy( fa2, (*cur)[i].fathers[1] );
		
		if( first != 0 )
		{
			if( fa1[0] != '\0' )
			{
				if( *input1 == -1 )
				{
					*input1 = getInputFromFather(prev, qtyPrev, fa1);
				}else
				{
					*input2 = getInputFromFather(prev, qtyPrev, fa1);
				}
			}
			if( fa2[0] != '\0' )
			{
				if( *input1 == -1 )
				{
					*input1 = getInputFromFather(prev, qtyPrev, fa2);
				}else
				{
					*input2 = getInputFromFather(prev, qtyPrev, fa2);
				}
			}
		}
		(*cur)[i].output = getGateHandler(type, *input1, *input2);
	}
}

/*
 *	Funcion encargada de retornar el output de una compuerta.
 */

int getGateHandler( int type, int in1, int in2 )
{
	switch( type )
	{
		case AND:
			return gateAnd(in1,in2);
		case OR:
			return gateOr(in1,in2);
		case XOR:
			return gateXor(in1,in2);
		case NAND:
			return gateNand(in1,in2);
		case NOR:
			return gateNor(in1,in2);
		case XNOR:
			return gateXnor(in1,in2);
	}
	return -1;
}

/*
 *	Funcion que retorna la salida de una compuerta, del nivel anterior al que se
 *	esta procesando.
 */

int getInputFromFather( gate *gates , int qtyGates, char fa[])
{
	
	int i;
	
	for( i = 0 ; i < qtyGates  ; ++ i )
	{
		if( strcmp( gates[i].name, fa ) == 0 )
		{
			return gates[i].output;
		}
	}
	return -1;
}

/* Compuerta AND */

int gateAnd( int in1, int in2 )
{
	return in1 & in2;
}

/* Compuerta OR */

int gateOr( int in1, int in2 )
{
	return in1 | in2;
}

/* Compuerta XOR */

int gateXor( int in1, int in2 )
{
	return in1 ^ in2; 
}

/* Compuerta NAND */

int gateNand( int in1, int in2 )
{
	return !gateAnd(in1,in2);
}

/* Compuerta NOR */

int gateNor( int in1, int in2 )
{
	return !gateOr(in1,in2);
}

/* Compuerta XNOR */

int gateXnor( int in1, int in2 )
{
	return !gateXor(in1,in2);
}
