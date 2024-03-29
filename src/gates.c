/**
 * \file gates.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#include "../inc/gates.h"

static int qtyFiles = 0, qtyFileCom;
static int initTable = 0;
int flagSignal = TRUE, flagFirst = TRUE, *maxLevel, *levels;
static circuitTable **table = NULL;
pid_t *childPids;

void ctrlC( int sig )
{
	int i;
	
	flagSignal = FALSE;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( childPids[i] != -1 )
			kill(childPids[i], SIGINT);
	}
}

void printLevel( int *level, int qty )
{
	int i;
	for( i = 0 ; i < qty ; ++i )
		fprintf(stderr, "Level[%d]: %d    MaxLevel[%d] = %d\n", i, level[i], i, maxLevel[i]);
}

void printCircuitTable( circuitTable * circuit)
{
	int i,j;
	
	for( i = 0; i < circuit[0].totalLevels ; ++i)
	{
		for( j = 0; j < (circuit[i].eachLevel)->qtyGates ; ++j)
		{
			printf( "LEVEL: %d Name: %s, Father[0]: %s, Father[1]: %s, Type: %d, Input[0]: %d, Input[1]: %d, Output: %d\n", 
						i,
						(circuit[i].eachLevel)->gates[j].name,
						(circuit[i].eachLevel)->gates[j].fathers[0],
						(circuit[i].eachLevel)->gates[j].fathers[1],
						(circuit[i].eachLevel)->gates[j].type,
						(circuit[i].eachLevel)->gates[j].input[0],
						(circuit[i].eachLevel)->gates[j].input[1],
						(circuit[i].eachLevel)->gates[j].output);
		}
	}
	printf("\n\n\n");
}

int main(void)
{

	int status;
		
	/*signal(SIGINT, ctrlC );*/	
	
	if( (status = loadIPC()) < 0 )
	{
		perror("Error en el LOAD de Gates");
		return status;
	}
	addMoreFiles();
	closeIPC();
	return gateInitializer();
}
/*
 * 	Thread encargado de leer y cuando llega algun archivo lo
 *	agrega a la lista.
 */

int addMoreFiles( void)
{
	int i;
		
	if( readIPC(getppid(), &qtyFileCom, sizeof(int)) == -1 )
	{
		perror("Error en la lectura de GATES a PIPE en la cantidad de archivo\n");
		return errno;
	}
	qtyFiles = qtyFileCom;
	
	if( (buildCircuitsTable()) != 0 )
	{
		perror("Error al construir la tabla de los circuitos\n");
		freeCircuitsGates();
		return errno;
	}
	if( ( childPids = (int*)malloc( sizeof(pid_t) * qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de pids\n");
		freeCircuitsGates();
		return errno;
	}
	memset(childPids, 0, qtyFiles);
	maxLevel = malloc( sizeof(int) * qtyFiles);
	
	for( i = 0 ; i < qtyFileCom ; ++i )
		maxLevel[i] = table[i][0].totalLevels;
	
	if( ( levels = (int*)malloc( sizeof(int) * qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion del arreglo de niveles\n");
		free(childPids);
		free(maxLevel);
		freeCircuitsGates();
		return errno;
	}
	initLevels();
	/*for( i = 0 ; i < qtyFiles ; ++i )
	{
		printCircuitTable(table[i]);
	}*/
	return 0;
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateInitializer( void )
{
	int i,status;
	
	while( !allFilesWasProccessed() && flagSignal )
	{
		if( (status = startCircuitsPipeline()) != 0)
		{
			perror("Error en el startCircuitsPipeline:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuitsGates();
			return status;
		}
		if( (status = listenToMyChildren()) != 0 )
		{
			perror("Error en el listenToMyChildren:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuitsGates();
			return status;
		}
		for (i = 0 ; i < qtyFiles ; ++i)
		{
			if( levels[i] == (maxLevel[i] - 1) )
			{
				saveProccessFile(table[i], childPids[i]);
				printf("Se resolvio el archivo de pipeline, el resultado se encuentra en el archivo finalizado con %d\n\n", childPids[i]);
			}
			if( levels[i] < maxLevel[i] && levels[i] >= 0 )
			{
				wait(&(childPids[i]));
			}
		}
		memset( childPids, -1, sizeof(pid_t) * qtyFiles);
		incLevels();
		finalizeIPC();
	}
	
	/*if( allFilesWasProccessed() )
	{
		for( i = 0 ; i < qtyFileCom ; ++i )
			saveProccessFile(table[i], i);

		for( i = 0 ; i < qtyFileCom ; ++i )
		{
			printCircuitTable(table[i]);
		}
	}*/
	
	freeCircuitsGates();
	free(childPids);
	free(maxLevel);
	free(levels);
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

int startCircuitsPipeline()
{
	int i, j, first = 0, notFirst = 1;
	curCircuit cur;
	
	for (i = 0, j = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{
			++j;
		}
	}
	
	if( setupIPC(j) == -1 )
	{
		perror("Error en la creacion del setupIPC en gates\n");
		return errno;
	}
	
	for (i = 0, j = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{	
			switch( (childPids[i] = fork()) )
			{
				case -1:
					perror("Error de fork");
					return errno;
					break;
				case 0:
					addClient(j);
					execv("./levels.bin", NULL);
					break;
				default:
					j++;
					break;
			}
		}else
		{
			childPids[i] = -1;
		}
	}
	
	if( synchronize() == -1 )
	{
		perror("Error en el synchronize de Gates");
		return errno;
	}
	for (i = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 && childPids[i] != -1 )
		{
			cur.curFile = i;
			cur.curLevel = levels[i];
			
			if( writeIPC( childPids[i], &cur, sizeof(curCircuit) ) == -1 )
			{
				perror("Error en la escritura de cur hacia levels\n");
				return errno;	
			}
			
			if( writeIPC( childPids[i], (void *)&(((table[i][levels[i]]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
			{
				perror("Error en la escritura de la cantidad de gates de Currently\n");
				return errno;
			}
			
			for( j = 0 ; j < ((table[i][levels[i]]).eachLevel)->qtyGates ; ++j )
			{
				if( writeIPC( childPids[i], (void*)(&(((table[i][levels[i]]).eachLevel)->gates[j])), sizeof(gate) ) == -1)
				{
					perror("Error en la escritura de compuertas\n");
					return errno;
				}
			}
			
			if( levels[i] == 0 )
			{
				if( writeIPC( childPids[i], (void*)(&first), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
			}else
			{
				if( writeIPC( childPids[i], (void*)(&notFirst), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
				if( writeIPC( childPids[i], (void *)&(((table[i][levels[i]-1]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
				{
					perror("Error en la escritura de la cantidad de gates de Previously\n");
					return errno;
				}
				for( j = 0 ; j < ((table[i][ (levels[i]-1) ]).eachLevel)->qtyGates ; ++j )
				{
					if( writeIPC( childPids[i], (void*)(&(((table[i][levels[i]-1]).eachLevel)->gates[j])), sizeof(gate) ) == -1)
					{
						perror("Error en la escritura de compuertas de previously\n");
						return errno;
					}
				}
			}
		}
	}
	return 0;
}


/*
 *	Funcion encargada de recibir la tabla por IPC y armarla.
 */

int listenToMyChildren( void )
{
	int i, j, auxGet=-1, auxSelect, levelsAmm;
	gate auxGate;
	curCircuit cur;
	
	levelsAmm = getCurrentPipeFiles();
		
	while( levelsAmm > 0 && (auxSelect = selectIPC(2)) > 0 )
	{
		if( levelsAmm == 0 )
			break;
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			if( childPids[i] > 0 && (auxGet = getIPCStatus(childPids[i])) )
			{
				if( readIPC( childPids[i], &cur, sizeof(curCircuit)) == -1 )
				{
					perror("Error en la lectura de LEVELS a GATE, cur\n");
					return errno;
				}
				
				for( j = 0 ; j < ((table[cur.curFile][cur.curLevel]).eachLevel)->qtyGates ; ++j )
				{
					if( readIPC(childPids[i], &auxGate, sizeof(gate)) == -1)
					{
						perror("Error en la lectura de las compuertas desde Levels\n");
						return errno;
					}
					memcpy( &(((table[cur.curFile][cur.curLevel]).eachLevel)->gates)[j], &auxGate, sizeof(gate));
				}
				if (--levelsAmm == 0 )
				{
					break;
				}
				childPids[i] = childPids[i] * -1;
			}
		}
	}
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( childPids[i] < -1 )
		{
			childPids[i] = childPids[i] * -1;
		}
	}
	return 0;
}

/*
 *	
 */

int buildCircuitsTable( void )
{
	int i,j,k, qtyGatesCom, qtyLevels;
	
	if( (table = (circuitTable**)realloc( NULL, sizeof(circuitTable*) * qtyFiles) ) == NULL )
	{
		perror("Error en la alocacion de memoria\n");
		return errno;
	}
	
	for( i = initTable ; i < qtyFiles ; ++i )
	{
		if( readIPC(getppid(), &qtyLevels, sizeof(int)) == -1 )
		{
			perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
			return errno;
		}
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable) * qtyLevels)) == NULL )
		{
			perror("Error en la alocacion de memoria de table[i]\n");
			free(table);
			return errno;	
		}
		(table[i][0].totalLevels) = qtyLevels;
	
		for( j = 0 ; j < qtyLevels ; ++j )
		{
			if( ((table[i][j]).eachLevel = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			if( readIPC(getppid(), &qtyGatesCom, sizeof(int)) == -1 )
			{
				perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
				return errno;
			}
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				for( i = 0 ; i < qtyFiles ; ++i )
					free(table[i]);
				free(table);
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				if( readIPC(getppid(), &((((table[i][j]).eachLevel)->gates)[k]), sizeof(gate) ) == -1 )
				{
					perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
					return errno;
				}
			}
		}
	}
	return 0;
}


/*
 *	Libera toda la lista de circuitos levantados
 */

void freeCircuitsGates( void )
{
	int i,j;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			free( ((table[i][j]).eachLevel)->gates );
		}
	}
	free(table);
}

/*
 *	Funcion encargada de iniciar el arreglo de niveles.
 *	El arreglo es de la siguiente forma, de acuerda a la cantidad
 *	de archivos se inicia en 0 la posicion 0 y se coloca el valor
 *	de la posicion negativo en esa posicion.
 */

void initLevels( void )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		levels[i] = (i * -1);
	}
}

/*
 *	Funcion encargada de incrementar todas las posiciones del arreglo de niveles
 */

void incLevels( void )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		levels[i] += 1;
	}
}

/*
 *	Funcion que se encarga de verificar si queda algun archivo por procesar
 */

int allFilesWasProccessed( void )
{
	int i, flag = TRUE;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( levels[i] < 0 || (levels[i] >= 0 && levels[i] < maxLevel[i]) )
		{
			flag = FALSE;
		}
	}
	return flag;
} 


/*
 *	Funcion que se encarga de verificar la cantidad de archivos en el pipeline
 */

int getCurrentPipeFiles( void )
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

/*
 *	Carga en el directorio los archivos ya procesados. EL nombre va a estar conformado por
 *	circuit + pos en la tabla.txt
 */

int saveProccessFile( circuitTable *table, int pos )
{
	int i,j;
	char *nameStart = "./results/finalCircuit", *endFile = ".txt", fileName[50], posi[10];
	FILE *proFile;
	
	itoa(pos, posi);
	strcpy( fileName, nameStart);
	strcat(fileName, posi);
	strcat(fileName, endFile);
	
	if( (proFile = fopen(fileName, "w")) == NULL )
	{
		perror("Error al abrir el archivo procesado");
		return errno;
	}
	
	for( i = 0 ; i < table[0].totalLevels ; ++i )
	{
		for( j = 0 ; j < (table[i].eachLevel)->qtyGates ; ++j )
		{
			fprintf(proFile, "Level: %d -- Name of the Gate: %s, Type of gate: %d, Input: (1)->%d (2)->%d, Output: %d\n\n",
			 										i,
													(table[i].eachLevel)->gates[j].name,
			 										(table[i].eachLevel)->gates[j].type,
													(table[i].eachLevel)->gates[j].input[0],
													(table[i].eachLevel)->gates[j].input[1],
													(table[i].eachLevel)->gates[j].output);
		}
	}
	return 0;
}
