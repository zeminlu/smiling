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
	int i, qtyFileCom, **ipcChannels, status, allocSize,  *maxLevel, *levels, flag = TRUE, aux;
	pid_t *childPids;
	circuitTable **table = NULL;
	curGateProcess curCircuit;
	void *master = NULL, *set = NULL;
	
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
	maxLevel = malloc( sizeof(int) * qtyFileCom);
	for( i = 0 ; i < qtyFileCom ; ++i )
		maxLevel[i] = table[i][0].totalLevels;
		
	if( ( levels = (int*)malloc( sizeof(int) * qtyFileCom) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de niveles\n");
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	initLevels( &levels, qtyFileCom );
	
	while( (aux = checkAvailableFiles( levels, qtyFileCom, maxLevel )) <= qtyFileCom && flag == TRUE)
	{
		flag = aux == 3 ? FALSE : TRUE;
		if( (status = startCircuitsPipeline( table, &childPids, &ipcChannels, qtyFileCom, levels, maxLevel )) != 0 ||
				(master = prepareIPC(&childPids, qtyFileCom, &allocSize)) == NULL 
				|| (set = malloc(sizeof(char) * allocSize)) == NULL)
		{
			free(childPids);
			freeCircuits(table,qtyFileCom);
			for( i = 0 ; i < qtyFileCom ; ++i )
				free( ipcChannels[i]);	
			return status;
		}

		if( (status = listenToMyChildren( set, master, allocSize, table, ipcChannels, childPids, qtyFileCom, &levels, maxLevel)) != 0 )
		{
			free(childPids);
			freeCircuits(table,qtyFileCom);
			for( i = 0 ; i < qtyFileCom ; ++i )
				free( ipcChannels[i]);	
			return status;
		}
	}
	
	for( i = 0 ; i < qtyFileCom ; ++i )
		printCircuitTable(table[i]);
	free(childPids);
	for( i = 0 ; i < qtyFileCom ; ++i )
		free( ipcChannels[i]);
	free(ipcChannels);
	freeCircuits(table, qtyFileCom);
	closeIPC(_stdin_);
	closeIPC(_stdout_);
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

int startCircuitsPipeline( circuitTable **table, pid_t **childPids, int ***ipcChannels, int qtyFiles, int *levels, int *maxLevel)
{
	int i, j, aux, first = 0, notFirst = 1;
	pid_t pid;
	curCircuit cur;
	
	/*printLevel(levels,qtyFiles);*/

	for( i = 0 ; i < qtyFiles ; ++i )
	{	
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{	
			/*fprintf(stderr, "I = %d level: %d MaxLevel: %d\n",i, levels[i], maxLevel[i] );*/
			setupIPC( _FULL_DUPLEX_, (*ipcChannels)[i], "./levels.bin", &pid );
			(*childPids)[i] = pid;
			cur.curFile = i;
			cur.curLevel = levels[i];
			
			writeIPC( (*ipcChannels)[i][1], &cur, sizeof(curCircuit) );
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
	return 0;
}


/*
 *	Funcion encargada de recibir la tabla por IPC y armarla.
 */

int listenToMyChildren( void *set, void *master, int allocSize, circuitTable **table, int **ipcChannels, int *childPids, int qtyFiles, int **levels, int * maxLevel)
{
	int i, j, readBytes, aux, flag = TRUE;
	gate auxGate;
	curCircuit cur;
	
	while( memcpy(set, master, allocSize), selectIPC(set,10) > 0 && flag == TRUE )
	{
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			/*fprintf(stderr, "listenToMyChildren I: %d\n", i);*/
			if( !(aux = getIPCStatus(ipcChannels[i][0], set)) &&	(*levels)[i] >= 0 && (*levels)[i] < maxLevel[i] )
			{
				readIPC( ipcChannels[i][0], &cur, sizeof(curCircuit));
				for( j = 0 ; j < ((table[i][(*levels)[i]]).eachLevel)->qtyGates ; ++j )
				{
					readBytes = readIPC(ipcChannels[i][0], &auxGate, sizeof(gate) );
					fprintf(stderr, "Reading -- Gate: %s Output: %d\n", auxGate.name, auxGate.output );
					if( readBytes < 0 )
						return readBytes;
					memcpy( &(((table[cur.curFile][cur.curLevel]).eachLevel)->gates)[j], &auxGate, sizeof(gate));
				}
			}
			fprintf(stderr, "getIPCStatus Aux: %d\n", aux);
		}
		if( i == qtyFiles )
			flag = FALSE;
	}
	incLevels(levels, qtyFiles);
	return 0;
}

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