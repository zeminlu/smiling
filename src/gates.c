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
	
	loadIPC();
	return gateInitializer();
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateInitializer( void )
{
	int i, qtyFileCom, status, *maxLevel, *levels, flag = TRUE, aux;
	pid_t *childPids;
	circuitTable **table = NULL;
	curGateProcess curCircuit;
	
	readIPC(getppid(), &curCircuit, sizeof(curGateProcess));
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
	closeIPC(getppid());
	
	/* COMIENZA EL PIPELINE */
	
	if( ( childPids = (int*)malloc( sizeof(pid_t) * curCircuit.qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de pids\n");
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	memset(childPids, 0, qtyFileCom);
	
	maxLevel = malloc( sizeof(int) * qtyFileCom);
	for( i = 0 ; i < qtyFileCom ; ++i )
		maxLevel[i] = table[i][0].totalLevels;
		
	if( ( levels = (int*)malloc( sizeof(int) * qtyFileCom) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de niveles\n");
		free(childPids);
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	initLevels( &levels, qtyFileCom );
	
	while( (aux = checkAvailableFiles( levels, qtyFileCom, maxLevel )) <= qtyFileCom && flag == TRUE)
	{
		fprintf(stderr, "checkAvailableFiles: %d\n", aux);
		flag = aux == qtyFileCom ? FALSE : TRUE;
		if( (status = startCircuitsPipeline( table, &childPids, qtyFileCom, levels, maxLevel )) != 0)
		{
			perror("Error en el startCircuitsPipeline:");
			free(childPids);
			freeCircuits(table,qtyFileCom);
			return status;
		}

		if( (status = listenToMyChildren( table, childPids, qtyFileCom, &levels, maxLevel)) != 0 )
		{
			perror("Error en el listenToMyChildren:");
			free(childPids);
			freeCircuits(table,qtyFileCom);
			return status;
		}
		for (i = 0 ; i < qtyFileCom ; ++i)
		{
			if( levels[i] < maxLevel[i] && levels[i] >= 0 )
			{
				/*finalizeIPC(childPids[i]);*/
				wait(&(childPids[i]));
			}
		}
		incLevels(&levels, qtyFileCom);
	}
	
	for( i = 0 ; i < qtyFileCom ; ++i )
		printCircuitTable(table[i]);
	free(childPids);
	freeCircuits(table, qtyFileCom);
	
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

int startCircuitsPipeline( circuitTable **table, pid_t **childPids, int qtyFiles, int *levels, int *maxLevel)
{
	int i, j, aux, first = 0, notFirst = 1;
	curCircuit cur;
	
	printLevel(levels,qtyFiles);

	for (i = 0, j = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{
			++j;
		}
	}
	
	setupIPC(j);

	for (i = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{	
			switch( ((*childPids)[i] = fork()) )
			{
				case -1:
					perror("Error de fork");
					return errno;
					break;
				case 0:
					addClient();
					execv("./levels.bin", NULL);
					break;
				default:					
					break;
			}
		}else{
			(*childPids)[i] = -1;
		}
	}
	
	synchronize();
	
	for (i = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{
			cur.curFile = i;
			cur.curLevel = levels[i];

			writeIPC( (*childPids)[i], &cur, sizeof(curCircuit) );
			aux = writeIPC( (*childPids)[i], (void *)&(((table[i][levels[i]]).eachLevel)->qtyGates) , sizeof(int) );
			/*fprintf(stderr, "Aux:%d I: %d\n", aux, i);*/
			for( j = 0 ; j < ((table[i][ levels[i] ]).eachLevel)->qtyGates ; ++j )
			{
				writeIPC( (*childPids)[i], &(((table[i][levels[i]]).eachLevel)->gates[j]), sizeof(gate) );
			}
			if( levels[i] == 0 )
			{
				writeIPC( (*childPids)[i], (void*)(&first), sizeof(int) );
			}else
			{
				writeIPC( (*childPids)[i], (void*)&(notFirst), sizeof(int) );

				writeIPC( (*childPids)[i], (void*)&(((table[i][ levels[i]-1 ]).eachLevel)->qtyGates) , sizeof(int) );
				for( j = 0 ; j < ((table[i][levels[i]-1 ]).eachLevel)->qtyGates ; ++j )
				{
					writeIPC( (*childPids)[i], &(((table[i][levels[i]-1 ]).eachLevel)->gates[j]), sizeof(gate) );
				}
			}
		}
	}
	return 0;
}


/*
 *	Funcion encargada de recibir la tabla por IPC y armarla.
 */

int listenToMyChildren( circuitTable **table, int *childPids, int qtyFiles, int **levels, int * maxLevel)
{
	int i, j, aux, flag = TRUE, levelsAmm, *finished, auxLevel;
	gate auxGate;
	curCircuit cur;
	
	auxLevel = levelsAmm = getCurrentPipeFiles(*levels, qtyFiles, maxLevel);
	/*printf("levelsAmm: %d\n", levelsAmm);*/
	if( (finished = calloc(1, sizeof(int) * levelsAmm)) == NULL )
	{
		perror("Error de memoria listenToMyChildren");
		return errno;
	}
	
	while( (aux = selectIPC(2)) > 0 && flag == TRUE )
	{
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			/*printf( "I: %d -- levelsAmm: %d childPids: %d\n",i, auxLevel, childPids[i]);*/
			fprintf(stderr, "Antes del if getIPCStatus selectIPC: %d childPids: %d\n", aux);
			if( childPids[i] != -1 && (aux = getIPCStatus(childPids[i])) && !(finished[i]))
			{
				readIPC( childPids[i], &cur, sizeof(curCircuit));
				fprintf(stderr, "listenToMyChildren File: %d Level: %d\n", cur.curFile, cur.curLevel);
				for( j = 0 ; j < ((table[i][(*levels)[i]]).eachLevel)->qtyGates ; ++j )
				{
					readIPC(childPids[i], &auxGate, sizeof(gate) );
					fprintf(stderr, "Reading -- Gate: %s Output: %d\n", auxGate.name, auxGate.output );
					memcpy( &(((table[cur.curFile][cur.curLevel]).eachLevel)->gates)[j], &auxGate, sizeof(gate));
				}
				finished[i] = TRUE;
				if (--levelsAmm == 0){
					flag = FALSE;
					break;
				}
			}
			fprintf(stderr, "Despues del if getIPCStatus selectIPC: %d\n", aux);
		}
	}

	free(finished);
	return 0;
}

int buildCircuitsTable( circuitTable *** circuits, int qtyFileCom )
{
	int i,j,k, qtyGatesCom;
	circuitTable ** table = *circuits;
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		readIPC(getppid(), &(table[i][0].totalLevels), sizeof(int));
		
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			if( ((table[i][j]).eachLevel = malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			readIPC(getppid(), &qtyGatesCom, sizeof(int));
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				free( (table[i][j].eachLevel) );
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				readIPC(getppid(), &((((table[i][j]).eachLevel)->gates)[k]), sizeof(gate) );
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


/*
 *	Funcion que se encarga de verificar la cantidad de archivos en el pipeline
 */

int getCurrentPipeFiles( int * levels, int qtyFiles, int *maxLevel )
{
	int i, flag = 0;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		/*fprintf(stderr, "levels[%d]: %d maxLevel: %d\n",i, levels[i], maxLevel[i] );*/
		if( levels[i] >= 0 && levels[i] < maxLevel[i])
			flag++;
	}
	return flag;
}