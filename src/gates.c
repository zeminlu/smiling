/*
 *		gates.c
 *
 *			Archivo encargado de llevar a cabo todos los procesos de resolucion
 *			de un circuito de compuertas.
 */

#include "../inc/gates.h"

void printLevel( int *level, int qty )
{
	int i;
	for( i = 0 ; i < qty ; ++i )
		fprintf(stderr, "Level[%d]: %d\n", i, level[i]);
}

void printCircuitTable( circuitTable * circuit)
{
	int i,j;

	for( i = 0; i < circuit[0].totalLevels ; ++i)
	{
		for( j = 0; j < (circuit[i].eachLevel)->qtyGates ; ++j)
		{
			fprintf(stderr, "LEVEL: %d Name: %s, Father[0]: %s, Father[1]: %s, Type: %d, Input[0]: %d, Input[1]: %d, Output: %d\n", 
						i,
						((circuit[i].eachLevel)->gates[j]).name,
						(circuit[i].eachLevel)->gates[j].fathers[0],
						(circuit[i].eachLevel)->gates[j].fathers[1],
						(circuit[i].eachLevel)->gates[j].type,
						(circuit[i].eachLevel)->gates[j].input[0],
						(circuit[i].eachLevel)->gates[j].input[1],
						(circuit[i].eachLevel)->gates[j].output);
		}
	}
}

int main(void){
	return gateInitializer();
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateInitializer( void )
{
	int i, qtyFileCom, *levels, **ipcChannels;
	pid_t *childPids;
	circuitTable **table = NULL;
	curGateProcess curCircuit;
	
	readIPC(_stdin_, &curCircuit, sizeof(curGateProcess));
	qtyFileCom = curCircuit.qtyFiles;
	
	if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * qtyFileCom) ) == NULL )
	{
		perror("Error en la alocacion de memoria\n");
		return errno;
	}
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable))) == NULL )
		{
			perror("Error en la alocacion de memoria de table[i]\n");
			return errno;	
		}
	}
	
	if( buildCircuitsTable(&table, qtyFileCom) != 0 )
	{
		perror("Error al construir la tabla de los circuitos\n");
		return errno;
	}
	
	if( ( levels = (int*)malloc( sizeof(int) * curCircuit.qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de niveles\n");
		return errno;
	}
	initLevels( &levels, curCircuit.qtyFiles );
	if( ( childPids = (int*)malloc( sizeof(pid_t) * curCircuit.qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de pids\n");
		return errno;
	}
	memset(childPids, 0, qtyFileCom);
	if( (ipcChannels = malloc( sizeof(int*) * qtyFileCom)) == NULL )
	{
		perror("Error en la alocacion del arreglo de ipcs\n");
		return errno;
	}
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		if( (ipcChannels[i] = malloc( sizeof(int) * 2)) == NULL )
		{
			perror("Error en la alocacion del arreglo de ipcs[i]\n");
			return errno;
		}	
	}
	
	startCircuitsPipeline( table, &levels, &childPids, &ipcChannels, qtyFileCom );
	
	free(levels);
	free(childPids);
	for( i = 0 ; i < qtyFileCom ; ++i )
		free( ipcChannels[i]);
	free(ipcChannels);
	freeCircuits(table, qtyFileCom);
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

void startCircuitsPipeline( circuitTable **table, int **levels, pid_t **childPids, int ***ipcChannels, int qtyFiles)
{
	int i, j, aux, first = 0, notFirst = 1, status;
	pid_t pid;

	for( i = 0 ; i < qtyFiles ; ++i )
		printCircuitTable(table[i]);
	while( checkAvailableFiles( levels, qtyFiles ) )
	{
		printLevel(*levels,qtyFiles);
		for( i = 0 ; i < qtyFiles ; ++i )
		{			
			if( (*levels)[i] == table[i][0].totalLevels - 1 )
				(*levels)[i] = _MAX_LEVELS_;
			else
			{
				if( (*levels)[i] > 0 && (*levels)[i] < _MAX_LEVELS_ )
				{	
					setupIPC( _FULL_DUPLEX_, (*ipcChannels)[i], "./levels", &pid );
					(*childPids)[i] = pid;
				
					writeIPC( (*ipcChannels)[i][1], (void*)&(notFirst), sizeof(int) );
					writeIPC( (*ipcChannels)[i][1], (void*)&(((table[i][ ((*levels)[i])-1 ]).eachLevel)->qtyGates) , sizeof(int) );
					for( j = 0 ; j < ((table[i][ ((*levels)[i])-1 ]).eachLevel)->qtyGates ; ++j )
					{
						writeIPC( (*ipcChannels)[i][1], &(((table[i][ ((*levels)[i])-1 ]).eachLevel)->gates[j]), sizeof(gate) );
					}
					writeIPC( (*ipcChannels)[i][1], (void *)&(((table[i][ ((*levels)[i]) ]).eachLevel)->qtyGates) , sizeof(int) );
					for( j = 0 ; j < ((table[i][ ((*levels)[i]) ]).eachLevel)->qtyGates ; ++j )
					{
						writeIPC( (*ipcChannels)[i][1], &(((table[i][ ((*levels)[i])]).eachLevel)->gates[j]), sizeof(gate) );
					}
				}else if( (*levels)[i] == 0 )
				{
					aux = writeIPC( (*ipcChannels)[i][1], (void*)(&first), sizeof(int) );
					fprintf(stderr, "Aux:%d\n", aux);
					writeIPC( (*ipcChannels)[i][1], (void *)&(((table[i][0]).eachLevel)->qtyGates) , sizeof(int) );
					for( j = 0 ; j < ((table[i][0]).eachLevel)->qtyGates ; ++j )
					{
						writeIPC( (*ipcChannels)[i][1], &(((table[i][0]).eachLevel)->gates[j]), sizeof(gate) );
					}
				}else
					continue;
			}
		}
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			for( j = 0 ; j < ((table[i][ ((*levels)[i]) ]).eachLevel)->qtyGates ; ++j )
			{
				if( (*childPids)[i] != 0 )
				{
					/* TENGO QUE LEER LO QUE ME MANDA EL PROCESO CON ID PID[I] */
					readIPC( _stdout_, &(((table[i][ ((*levels)[i])]).eachLevel)->gates[j]), sizeof(gate) );
					while( waitpid( (*childPids)[i]	, &status , WNOHANG) == 0 )
						;
				}
			}
		}
		incLevels(levels,qtyFiles);
	}
	closeIPC(_stdin_);
	closeIPC(_stdout_);
	
	for( i = 0 ; i < qtyFiles ; ++i )
		printCircuitTable(table[i]);
}

/*
 *	Funcion encargada de recibir la tabla por IPC y armarla.
 */

int buildCircuitsTable( circuitTable *** circuits, int qtyFileCom )
{
	int i,j,k, qtyGatesCom;
	circuitTable ** table = *circuits;
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		readIPC(_stdin_, &(table[i][0].totalLevels), sizeof(int));
		fprintf(stderr, "Levels: %d\n", table[i][0].totalLevels );
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			if( ((table[i][j].eachLevel) = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			readIPC(_stdin_, &qtyGatesCom, sizeof(int));
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * (qtyGatesCom))) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				readIPC( _stdin_, &(((table[i][j].eachLevel)->gates)[k]), sizeof(gate) );
			}
			fprintf(stderr, "buildCircuitsTable -- Name Gate: %s -- %d\n", ((table[0][0].eachLevel)->gates[0]).name, j );
		}
	}
	return 0;
}


/*
 *	Libera toda la lista de circuitos levantados
 */

void freeCircuits( circuitTable **table, int qtyFile )
{
	int i;
	
	for( i = 0 ; i < qtyFile ; ++i )
	{
		free( ((table[i])->eachLevel)->gates );
		free( table[i]->eachLevel );
		free( table[i] );
	}
}

/*
 *	Funcion encargada de iniciar el arreglo de niveles.
 *	El arreglo es de la siguiente forma, de acuerda a la cantidad
 *	de archivos se inicia en 0 la posicion 0 y se coloca el valor
 *	de la posicion negativo en esa posicion.
 */

void initLevels( int **levels, int qtyFiles )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		(*levels)[i] = (i * -1);
	}
}

/*
 *	Funcion encargada de incrementar todas las posiciones del arreglo de niveles
 */

void incLevels( int **levels, int qtyFiles )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		(*levels)[i] += 1;
	}
}

/*
 *	Funcion que se encarga de verificar que se hayan completado todos los circuitos
 */

int checkAvailableFiles( int ** levels, int qtyFiles )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( (*levels)[i] == _MAX_LEVELS_ )
			return FALSE;
	}
	return TRUE;
} 