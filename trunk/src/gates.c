/*
 *		gates.c
 *
 *			Archivo encargado de llevar a cabo todos los procesos de resolucion
 *			de un circuito de compuertas.
 */

#include "../inc/gates.h"

static int qtyFiles = 0, qtyFileCom;
static int initTable = 0;
int flagFirst = TRUE;
static circuitTable **table = NULL;
int finishedAmm = 0, *maxLevel, *levels;
pid_t *childPids;
pthread_mutex_t mutexIndex;

void printLevel( int *level, int qty )
{
	int i;
	for( i = 0 ; i < qty ; ++i )
		fprintf(stderr, "Level[%d]: %d\n", i, level[i]);
}

void printCircuitTable( circuitTable * circuit)
{
	int i,j;
	
	fprintf(stderr, "Cantidad de niveles: %d\n", circuit[0].totalLevels);
	fprintf(stderr, "--------------------------------------------------\n");
	for( i = 0; i < circuit[0].totalLevels ; ++i)
	{
		fprintf(stderr, "Cantidad de compuertas: %d\n", (circuit[i].eachLevel)->qtyGates);
		for( j = 0; j < (circuit[i].eachLevel)->qtyGates ; ++j)
		{
			fprintf(stderr, "LEVEL: %d Name: %s, Father[0]: %s, Father[1]: %s, Type: %d, Input[0]: %d, Input[1]: %d, Output: %d\n", 
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
	fprintf(stderr, "--------------------------------------------------\n");
}

int main(void)
{
	pthread_t thread;
	int *aux = 0;
		
	loadIPC();
	pthread_create(&thread, NULL, addMoreFiles, (void*)aux );
	return gateInitializer();
}
/*
 * 	Thread encargado de leer y cuando llega algun archivo lo
 *	agrega a la lista.
 */

void * addMoreFiles( void * ret)
{
	int i, cont = 0;
		
	pthread_mutex_init(&mutexIndex, NULL);
	
	fprintf(stderr, "GATE -- READ -- ParentPID: %d myPid: %d \n", getppid(), getpid() );
	if( readIPC(getppid(), &qtyFileCom, sizeof(int)) == -1 )
	{
		perror("Error en la lectura de GATES a PIPE en la cantidad de archivo\n");
		*(int*)ret = errno;
		return (void*)ret;
	}
	qtyFiles += qtyFileCom;
	if( (table = buildCircuitsTable()) == NULL )
	{
		perror("Error al construir la tabla de los circuitos\n");
		freeCircuitsGates();
		*(int*)ret = errno;
		return (void*)ret;
	}
	if( flagFirst )
	{
		if( ( childPids = (int*)malloc( sizeof(pid_t) * qtyFiles) ) == NULL )
		{
			perror("Error en la alocacion del arreglo de pids\n");
			freeCircuitsGates();
			*(int*)ret = errno;
			return (void*)ret;
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
			*(int*)ret = errno;
			return (void*)ret;
		}
		initLevels();
	}else
	{
		if( ( childPids = (int*)realloc( childPids, sizeof(pid_t) * qtyFiles) ) == NULL )
		{
			perror("Error en la alocacion del arreglo de pids\n");
			freeCircuitsGates();
			*(int*)ret = errno;
			return (void*)ret;
		}
		memset(childPids, 0, qtyFiles);
		maxLevel = realloc( maxLevel, sizeof(int) * qtyFiles);
		for( i = 0 ; i < qtyFileCom ; ++i )
			maxLevel[i] = table[i][0].totalLevels;
		if( ( levels = (int*)realloc( levels, sizeof(int) * qtyFiles) ) == NULL )
		{
			perror("Error en la alocacion del arreglo de niveles\n");
			free(childPids);
			free(maxLevel);
			freeCircuitsGates();
			*(int*)ret = errno;
			return (void*)ret;
		}
		if( levels[qtyFiles - initTable + 1] > 0 )
		{
			for( i = 0 ; i < qtyFiles - initTable + 1 ; ++i )
				levels[i] = cont++;
		}else if( levels[qtyFiles - initTable + 1] == 0 )
		{
			cont = -1;
			for( i = 0 ; i < qtyFiles - initTable + 1 ; ++i )
				levels[i] = cont--; 
		}else
		{
			cont =  levels[qtyFiles - initTable];
			for( i = 0 ; i < qtyFiles - initTable + 1 ; ++i )
				levels[i] = cont--;
		}
	}
	pthread_mutex_destroy(&mutexIndex);
	
	return ret;
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateInitializer( void )
{
	int i, status;
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		printCircuitTable(table[i]);
	}
	
	while( !allFilesWasProccessed() && status == 0 )
	{
		pthread_mutex_init(&mutexIndex, NULL);
		
		if( finishedAmm >= 5 )
		{
			reallocAllOfInfo();
		}
		finishedAmm = 0;
		if( (status = startCircuitsPipeline()) != 0)
		{
			perror("Error en el startCircuitsPipeline:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuitsGates();
			return status;
		}
		fprintf(stderr, "Acabo de salir del startCircuitsPipeline con status: %d Voy a entrar a listenToMyChildren\n", status);
		if( (status = listenToMyChildren()) != 0 )
		{
			perror("Error en el listenToMyChildren:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuitsGates();
			return status;
		}
		fprintf(stderr, "Termine el listenToMyChildren status: %d\n", status);
		for (i = 0 ; i < qtyFileCom ; ++i)
		{
			if( levels[i] < maxLevel[i] && levels[i] >= 0 )
			{
				fprintf(stderr, "Haciendo el wait -- i = %d childPids: %d\n", i, childPids[i]);
				wait(&(childPids[i]));
			}
		}
		memset(childPids, -1, qtyFileCom * sizeof(pid_t) );
		incLevels();
		finalizeIPC();
		pthread_mutex_destroy(&mutexIndex);
	}
	
	if( status == 0 )
	{
		perror("Se suspendio el programa por error de startCircuitsPipeline o listenToMyChildren");
		return errno;
	}
	
	for( i = 0 ; i < qtyFileCom ; ++i )
		saveProccessFile(table[i], i);
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		fprintf(stderr, "-- COMIENZO DEL CIRCUITO %d\n", i);
		printCircuitTable(table[i]);
		fprintf(stderr, "-- FIN DEL CIRCUITO %d\n", i);
	}
	
	freeCircuitsGates();
	free(childPids);
	free(maxLevel);
	free(levels);
	fprintf(stderr, "Termine de correr el gateInitializer\n");
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

int startCircuitsPipeline()
{
	int i, j, first = 0, notFirst = 1;
	curCircuit cur;
	
	printLevel(levels,qtyFiles);
	
	for (i = 0, j = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 )
		{
			++j;
		}
	}
	
	fprintf(stderr, "Creando %d procesos\n", j );
	if( setupIPC(j) == -1 )
	{
		perror("Error en la creacion del setupIPC en gates\n");
		return errno;
	}
	
	for (i = 0 ; i < qtyFiles ; ++i)
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
					addClient();
					execv("./levels.bin", NULL);
					break;
				default:					
					break;
			}
		}else
		{
			childPids[i] = -1;
		}
	}
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		fprintf(stderr, "%d startCircuitsPipeline -- pid creados: %d\n", i, childPids[i]);
	}
	
	if( synchronize() == -1 )
	{
		perror("Error en el synchronize de Gates");
		return errno;
	}
	fprintf(stderr, "Acabo de sincronizar con el hijo\n");
	for (i = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 && childPids[i] != -1 )
		{
			cur.curFile = i;
			cur.curLevel = levels[i];
			fprintf(stderr, "cur.curFile: %d cur.curLevel: %d\n", cur.curFile, cur.curLevel);
			
			fprintf(stderr, "Sending Cur -- MyPID: %d childPID: %d\n", getpid(), childPids[i]);
			if( writeIPC( childPids[i], &cur, sizeof(curCircuit) ) == -1 )
			{
				perror("Error en la escritura de cur hacia levels\n");
				return errno;	
			}
			printf("Envie el Sending cur pid: %d\n", getpid());
			fprintf(stderr, "Sending the amount of currently gates -- MyPID: %d childPID: %d\n", getpid(), childPids[i]);
			if( writeIPC( childPids[i], (void *)&(((table[i][levels[i]]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
			{
				perror("Error en la escritura de la cantidad de gates de Currently\n");
				return errno;
			}
			
			for( j = 0 ; j < ((table[i][levels[i]]).eachLevel)->qtyGates ; ++j )
			{
				fprintf(stderr, "Sending each gate of currently J:%d -- MyPID: %d childPID: %d\n", j, getpid(), childPids[i]);
				if( writeIPC( childPids[i], (void*)(&(((table[i][levels[i]]).eachLevel)->gates[j])), sizeof(gate) ) == -1)
				{
					perror("Error en la escritura de compuertas\n");
					return errno;
				}
			}
			
			if( levels[i] == 0 )
			{
				fprintf(stderr, "Sending if it is the first level -- MyPID: %d childPID: %d\n", getpid(), childPids[i]);
				if( writeIPC( childPids[i], (void*)(&first), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
			}else
			{
				fprintf(stderr, "Sending if it is not the first level -- MyPID: %d childPID: %d\n", getpid(), childPids[i]);
				if( writeIPC( childPids[i], (void*)(&notFirst), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
				fprintf(stderr, "Sending the amount of previously gates -- MyPID: %d childPID: %d\n", getpid(), childPids[i]);
				if( writeIPC( childPids[i], (void *)&(((table[i][levels[i]-1]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
				{
					perror("Error en la escritura de la cantidad de gates de Previously\n");
					return errno;
				}
				for( j = 0 ; j < ((table[i][ (levels[i]-1) ]).eachLevel)->qtyGates ; ++j )
				{
					fprintf(stderr, "Sending previously each gates -- j:%d MyPID: %d childPID: %d\n", j, getpid(), childPids[i]);
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
	
	fprintf(stderr, "levelsAmm: %d \n", levelsAmm);
	while( (auxSelect = selectIPC(2)) > 0 )
	{
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			if( childPids[i] != -1 && (auxGet = getIPCStatus(childPids[i])) )
			{
				fprintf(stderr, "listenToMyChildren -- READING -- MyPID: %d childPID:%d\n", getpid(), (childPids)[i]);
				if( readIPC( childPids[i], &cur, sizeof(curCircuit)) == -1 )
				{
					perror("Error en la lectura de LEVELS a GATE, cur\n");
					return errno;
				}
				
				fprintf(stderr, "childPids: %d File: %d Level: %d\n", childPids[i], cur.curFile, cur.curLevel);
				
				for( j = 0 ; j < ((table[cur.curFile][cur.curLevel]).eachLevel)->qtyGates ; ++j )
				{
					fprintf(stderr, "listenToMyChildren -- READING -- MyPID: %d childPID:%d\n", getpid(), (childPids)[i]);
					if( readIPC(childPids[i], &auxGate, sizeof(gate)) == -1)
					{
						perror("Error en la lectura de las compuertas desde Levels\n");
						return errno;
					}
				
					printf( "Reading -- pid: %d Circuit: %d Level: %d Gate: %s Output: %d\n", 	childPids[i], 
																								cur.curFile, 
																								cur.curLevel, 
																								auxGate.name, 
																								auxGate.output );
					memcpy( &(((table[cur.curFile][cur.curLevel]).eachLevel)->gates)[j], &auxGate, sizeof(gate));
				}
				if (--levelsAmm == 0){
					break;
				}
				fprintf(stderr, "listenToMyChildren -- Estoy adentro del IF de getIPCStatus, auxGet: %d\n", auxGet);
			}
			fprintf(stderr, "Return del getIPCStatus: %d\n", auxGet );
		}
	}
	fprintf(stderr, "Devolucion del selectIPC: %d\n", auxSelect);
	printf("------ Fin de la lectura --------\n");
	return 0;
}

/*
 *	
 */

circuitTable ** buildCircuitsTable( void )
{
	int i,j,k, qtyGatesCom, qtyLevels;
	
	if( flagFirst )
	{
		if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * qtyFiles) ) == NULL )
		{
			perror("Error en la alocacion de memoria\n");
			return NULL;
		}
		for( i = 0 ; i < qtyFiles ; ++i )
		{
			if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable))) == NULL )
			{
				perror("Error en la alocacion de memoria de table[i]\n");
				free(table);
				return NULL;	
			}
		}
		initTable = i;
	}else
	{
		if( (table = (circuitTable**)realloc( table, sizeof(circuitTable*) * qtyFiles) ) == NULL )
		{
			perror("Error en la alocacion de memoria\n");
			return NULL;
		}
		/*for( i = initTable ; i < qtyFiles ; ++i )
		{
			if( (table[i] = (circuitTable*)realloc( table[i], sizeof(circuitTable))) == NULL )
			{
				perror("Error en la alocacion de memoria de table[i]\n");
				free(table);
				return NULL;	
			}
		}*/
	}
	
	
	for( i = initTable ; i < qtyFiles ; ++i )
	{
		if( readIPC(getppid(), &qtyLevels, sizeof(int)) == -1 )
		{
			perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
			return NULL;
		}
		fprintf(stderr, "File: %d Cantidad de niveles: %d \n", i, qtyLevels);
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable) * qtyLevels)) == NULL )
		{
			perror("Error en la alocacion de memoria de table[i]\n");
			free(table);
			return NULL;	
		}
		(table[i][0].totalLevels) = qtyLevels;
	
		for( j = 0 ; j < qtyLevels ; ++j )
		{
			if( ((table[i][j]).eachLevel = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return NULL;
			}
			if( readIPC(getppid(), &qtyGatesCom, sizeof(int)) == -1 )
			{
				perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
				return NULL;
			}
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			fprintf(stderr, "File: %d Level: %d Cantidad de compuertas: %d \n", i, j, qtyGatesCom);
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				for( i = 0 ; i < qtyFiles ; ++i )
					free(table[i]);
				free(table);
				return NULL;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				if( readIPC(getppid(), &((((table[i][j]).eachLevel)->gates)[k]), sizeof(gate) ) == -1 )
				{
					perror("Error en la lectura de la cantidad de niveles de buildCircuitsTable");
					return NULL;
				}
			}
		}
	}
	return table;
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
			++finishedAmm;
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
	char *nameStart = "./results/finalCircuit", *endFile = ".txt", fileName[30], posi[10];
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

/*
 *	Funcion encargda de reallocar todos los arreglos cuando se llena 
 *	a un maximo de archivo finalizados
 */

void reallocAllOfInfo( void )
{
	int i, k, *indexJustFinished, *auxLevel, *auxMaxLevel;
	circuitTable **auxTable;
	pid_t *auxChildPids;
	
	auxTable = (circuitTable**)malloc( sizeof(circuitTable*) * (qtyFiles - finishedAmm + 1));
	auxLevel = (int*)malloc(sizeof(int) * (qtyFiles - finishedAmm + 1));
	auxMaxLevel = (int*)malloc(sizeof(int) * (qtyFiles - finishedAmm + 1));
	auxChildPids = (int*)malloc(sizeof(pid_t) * (qtyFiles - finishedAmm + 1));
	k = 0;
	indexJustFinished = getFinalizeFiles();
	
	for( i = 0 ; i < finishedAmm ; ++i )
	{
		auxTable[k] = table[i];
		auxLevel[k] = levels[i];
		auxMaxLevel[k] = maxLevel[i];
		auxChildPids[k] = childPids[i];
		k++;
	}
	freeCircuitsGates();
	free(levels);
	free(maxLevel);
	free(childPids);
	table = auxTable;
	levels = auxLevel;
	maxLevel = auxMaxLevel;
	childPids = auxChildPids;
}

/*
 *	Funcion que me dice los indices de los archivos
 *	que no finalizaron aun.
 */

int * getFinalizeFiles( void )
{
	int k = 0, i, * aux = malloc( sizeof(int) * qtyFiles );
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( levels[i] >= 0 && levels[i] < maxLevel[i] )
		{
			aux[k++] = i;
		}
	}
	
	return aux;
}