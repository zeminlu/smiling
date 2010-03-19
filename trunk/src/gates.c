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
	int i, qtyFileCom, **ipcChannels;
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
			free(table);
			return errno;	
		}
	}
	
	if( buildCircuitsTable(&table, qtyFileCom) != 0 )
	{
		perror("Error al construir la tabla de los circuitos\n");
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	
	if( ( childPids = (int*)malloc( sizeof(pid_t) * curCircuit.qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de pids\n");
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	memset(childPids, 0, qtyFileCom);
	if( (ipcChannels = malloc( sizeof(int*) * qtyFileCom)) == NULL )
	{
		perror("Error en la alocacion del arreglo de ipcs\n");
		freeCircuits(table, qtyFileCom);
		free(childPids);
		return errno;
	}
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		if( (ipcChannels[i] = malloc( sizeof(int) * 2)) == NULL )
		{
			perror("Error en la alocacion del arreglo de ipcs[i]\n");
			free(ipcChannels);
			return errno;
		}	
	}
	
	startCircuitsPipeline( table, &childPids, &ipcChannels, qtyFileCom );
	
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

int startCircuitsPipeline( circuitTable **table, pid_t **childPids, int ***ipcChannels, int qtyFiles)
{
	int i, j, aux, first = 0, notFirst = 1, status, *maxLevel, *levels, flag = TRUE;
	pid_t pid;

	maxLevel = malloc( sizeof(int) * qtyFiles);
	for( i = 0 ; i < qtyFiles ; ++i )
		maxLevel[i] = table[i][0].totalLevels;
		
	if( ( levels = (int*)malloc( sizeof(int) * qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de niveles\n");
		freeCircuits(table, qtyFiles);
		return errno;
	}
	initLevels( &levels, qtyFiles );

	while( (aux = checkAvailableFiles( levels, qtyFiles, maxLevel )) <= qtyFiles && flag )
	{
		flag = aux == 3 ? FALSE : TRUE;
	
		fprintf(stderr, "qtyFiles: %d checkAvailableFiles: %d \n", qtyFiles, aux);
		fprintf(stderr, "------------------------\n");
		printLevel(levels,qtyFiles);
	
		for( i = 0 ; i < qtyFiles ; ++i )
		{	
			if( levels[i] < maxLevel[i] && levels[i] >= 0 )
			{	
				fprintf(stderr, "I = %d level: %d MaxLevel: %d\n",i, levels[i], maxLevel[i] );
				setupIPC( _HALF_DUPLEX_, (*ipcChannels)[i], "./levels", &pid );
				(*childPids)[i] = pid;

				aux = writeIPC( (*ipcChannels)[i][1], (void *)&(((table[i][levels[i]]).eachLevel)->qtyGates) , sizeof(int) );
				/*fprintf(stderr, "Aux:%d I: %d\n", aux, i);*/
				for( j = 0 ; j < ((table[i][ levels[i] ]).eachLevel)->qtyGates ; ++j )
				{
					writeIPC( (*ipcChannels)[i][1], &(((table[i][levels[i]]).eachLevel)->gates[j]), sizeof(gate) );
				}
				if( levels[i] == 0 )
				{
					writeIPC( (*ipcChannels)[i][1], (void*)(&first), sizeof(int) );
				}else
				{
					writeIPC( (*ipcChannels)[i][1], (void*)&(notFirst), sizeof(int) );
					writeIPC( (*ipcChannels)[i][1], (void*)&(((table[i][ levels[i]-1 ]).eachLevel)->qtyGates) , sizeof(int) );
					for( j = 0 ; j < ((table[i][levels[i]-1 ]).eachLevel)->qtyGates ; ++j )
					{
						writeIPC( (*ipcChannels)[i][1], &(((table[i][levels[i]-1 ]).eachLevel)->gates[j]), sizeof(gate) );
					}
				}
			}
		}
		incLevels( &levels, qtyFiles);
		/*for( i = 0 ; i < qtyFiles ; ++i )
		{
			for( j = 0 ; j < ((table[i][ ((*levels)[i]) ]).eachLevel)->qtyGates ; ++j )
			{
				if( (*childPids)[i] != 0 )
				{
		*/			/* TENGO QUE LEER LO QUE ME MANDA EL PROCESO CON ID PID[I] */
		/*			readIPC( _stdout_, &(((table[i][ ((*levels)[i])]).eachLevel)->gates[j]), sizeof(gate) );
					while( waitpid( (*childPids)[i]	, &status , WNOHANG) == 0 )
						;
				}
			}
		}*/
		closeIPC(_stdin_);
		closeIPC(_stdout_);
	}
	free(levels);
	return 0;
}

/*
 *	Funcion encargada de recibir la tabla por IPC y armarla.
 */

int buildCircuitsTable( circuitTable *** circuits, int qtyFileCom )
{
	int i,j,k, qtyGatesCom, readBytes;
	circuitTable ** table = *circuits;
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		readBytes = readIPC(_stdin_, &(table[i][0].totalLevels), sizeof(int));
		/*fprintf(stderr, "Gate -- totalLevels -- readBytes: %d\n", readBytes );*/
		
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			if( ((table[i][j]).eachLevel = malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			readBytes = readIPC(_stdin_, &qtyGatesCom, sizeof(int));
			/*fprintf(stderr, "Gate -- archivo:%d nivel: %d -- qtyGates -- readBytes: %d\n", i,j, readBytes );*/
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				free( (table[i][j].eachLevel) );
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				readBytes = readIPC( _stdin_, &((((table[i][j]).eachLevel)->gates)[k]), sizeof(gate) );
				/*fprintf(stderr, "Gate -- archivo: %d nivel: %d gate: %d readBytes: %d\n", i,j,k,readBytes );*/
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
		fprintf(stderr, "Estoy incrementando\n");
		(*levels)[i] += 1;
	}
}

/*
 *	Funcion que se encarga de verificar que se hayan completado todos los circuitos
 */

int checkAvailableFiles( int * levels, int qtyFiles, int *maxLevel )
{
	int i, flag = 0;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		/*fprintf(stderr, "levels[%d]: %d maxLevel: %d\n",i, levels[i], maxLevel[i] );*/
		if( levels[i] >= maxLevel[i] - 1 )
			flag++;
	}
	return flag;
} 