/*
 *		gates.c
 *
 *			Archivo encargado de llevar a cabo todos los procesos de resolucion
 *			de un circuito de compuertas.
 */

#include "../inc/gates.h"

circuitTable **table = NULL;
int qtyFil = 0;

/*
 *	Verifica la cantidad de archivo que hay dentro del directorio
 */

int getFilesAmm (DIR *dp){
	
	struct dirent *d;
	int ret = 0;
	
	while ((d = readdir(dp))){
		 if (d->d_ino != 0){
			++ret;
		}
	}
	return ret;
}

/*
 *	Funcion encargada de parsear el archivo XML con todas las compuertas.
 *	Retorna la tabla correspondiente a cada circuito, en caso de error
 *	retorn NULL.
 *	La funcion que lo llama debe realizar free al finalizar su uso.
 */

circuitTable * parseXMLGate( char * docName)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	circuitTable * circuit= NULL;
	
	doc = xmlParseFile(docName);
	if( doc == NULL )
	{
		perror("El documento no se puede parsear\n");
		return NULL;
	}
	cur = xmlDocGetRootElement( doc );
	if( cur == NULL )
	{
		perror("El archivo esta vacio\n");
		xmlFreeDoc(doc);
		return NULL;
	}
	if( xmlStrcmp(cur->name, (const xmlChar *) "circuit") )
	{
		perror("El documento es invalido, ya que no tiene la raiz <circuit>\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	circuit = parseCircuit( doc, cur );
	xmlFreeDoc(doc);
	return circuit;
}

/*
 *	Funcion que parsea las compuertas, los input de cada comnpuerta
 *	se parsean en otra funcion y se modifica la tabla ya existente.
 *	El proceso al finalizar debe liberar "CIRCUIT"
 */

circuitTable * parseCircuit( xmlDocPtr doc, xmlNodePtr cur )
{
	circuitTable * circuit = NULL;
	xmlNodePtr input;
	int i, j, curLevel = 0;
	
	if( (circuit = (circuitTable*)realloc(NULL, sizeof(circuitTable) * _MAX_GATES_LEVELS_ )) == NULL )
	{
		perror("parseCircuit: Error en la alocacion de memoria\n");
		return NULL;
	}
	for( i = 0; i < _MAX_GATES_LEVELS_ ; ++i)
	{
		if( (circuit[i].eachLevel = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel)) ) == NULL )
		{
			perror("Error en la alocacion de memoria de los indices de circuit\n");
			return NULL;
		}
	}
	for( i = 0; i < _MAX_GATES_LEVELS_ ; ++i)
	{
		if( ( (circuit[i].eachLevel)->gates = (gate*)malloc( sizeof(gate) * _MAX_GATES_LEVELS_) ) == NULL )
		{
			perror("Error en la alocacion de memoria de los indices de circuit\n");
			return NULL;
		}
		
		(circuit[i].eachLevel)->qtyGates = 0;
		for( j = 0; j < _MAX_GATES_LEVELS_; ++j)
		{
			((circuit[i].eachLevel)->gates[j]).fathers[0][0] = '\0';
			((circuit[i].eachLevel)->gates[j]).fathers[1][0] = '\0';
		}
	}
	
	
	cur = (cur->xmlChildrenNode);
	while( cur != NULL )
	{	
		if( !xmlIsBlankNode(cur) )
		{
			input = cur;
			if( !xmlStrcmp(cur->name, (const xmlChar *)"gates") )
			{	
				parseGatesTags(NULL, cur->xmlChildrenNode, circuit, curLevel);
				circuit[0].totalLevels = countLevels(circuit);
			}
		}
		cur = cur->next;
	}
	/*printCircuitTable(circuit);*/
	for( i = 0 ; i < circuit[0].totalLevels ; ++i )
	{
		(circuit[i].eachLevel)->gates = realloc((circuit[i].eachLevel)->gates, sizeof(gate) * (circuit[i].eachLevel)->qtyGates);
	}
	/*circuit = realloc( circuit, sizeof(circuitTable) * circuit[0].totalLevels);*/
	return circuit;
}

/*
 *	Funcion que parsea las compuertas y arma la lista por niveles.
 *	No cuenta los niveles, sino que arma la matriz unicamente.
 */

void parseGatesTags( char *father, xmlNodePtr cur, circuitTable * circuit, int curLevel )
{
	
	int pos = 0, posExist = 0;
	xmlChar * type=NULL, *input1=NULL, *input2=NULL;
	
	if( cur == NULL )
	{
		return;
	}else
	{
		while( xmlIsBlankNode(cur) )
		{
			cur=cur->next;
		}
		if( curLevel % _MAX_GATES_LEVELS_ == 0 && curLevel != 0)
		{
			circuit = (circuitTable*)realloc(circuit, sizeof(circuitTable) * (curLevel + _MAX_GATES_LEVELS_) );
		}
		while( cur != NULL )
		{
			parseGatesTags( (char*)cur->name, cur->xmlChildrenNode, circuit, curLevel + 1 );
			if( !xmlIsBlankNode(cur) )
			{
				pos = (circuit[curLevel].eachLevel)->qtyGates;
				if( pos % _MAX_GATES_LEVELS_ == 0 && pos != 0 )
				{
					
					(circuit[curLevel].eachLevel)->gates = (gate*)realloc((circuit[curLevel].eachLevel)->gates, 
															sizeof(gate) * (pos + _MAX_GATES_LEVELS_) );
				}
				posExist = checkGateIsLoaded( circuit, (char*)cur->name, curLevel);
				if( posExist != -1 )
				{
					strcpy( ((circuit[curLevel].eachLevel)->gates[posExist]).fathers[1], father );
				}else
				{
					strcpy( ((circuit[curLevel].eachLevel)->gates[pos]).name, (char*)cur->name);
					if( father != NULL )
					{
						strcpy( ((circuit[curLevel].eachLevel)->gates[pos]).fathers[0], father );
					}

					input1 = (xmlChar*)xmlGetProp(cur,(xmlChar*)"input1");
					input2 = (xmlChar*)xmlGetProp(cur,(xmlChar*)"input2");		
					if( input1 == NULL )
						(circuit[curLevel].eachLevel)->gates[pos].input[0] = _INVALID_INPUT_;
					else
						(circuit[curLevel].eachLevel)->gates[pos].input[0] = atoi((char*)input1);

					if( input2 == NULL )
						(circuit[curLevel].eachLevel)->gates[pos].input[1] = _INVALID_INPUT_;
					else
						(circuit[curLevel].eachLevel)->gates[pos].input[1] = atoi((char*)input2);

					(circuit[curLevel].eachLevel)->gates[pos].output = _INVALID_OUTPUT_;

					type = (xmlChar*)xmlGetProp(cur,(xmlChar*)"type");
					(circuit[curLevel].eachLevel)->gates[pos].type = getType(atoi((char*)type));
					xmlFree(type);
					xmlFree(input1);
					xmlFree(input2);
					(circuit[curLevel].eachLevel)->qtyGates++;
					
				}
			}
			cur = cur->next;
		}
	}
}

/*
 *	Devuelve el numero correspondiente al tipo de compuerta que es.
 *	En caso de error devuelve -1.
 */

int getType( int type)
{
	switch( type )
	{
		case AND:
			return AND;
		case OR:
			return OR;
		case XOR:
			return XOR;
		case NAND:
			return NAND;
		case NOR:
			return NOR;
		case XNOR:
			return XNOR;
	}
	return -1;
}

/* 
 *	Cuenta la cantidad de niveles que tiene el circuito
 */

int countLevels( circuitTable* circuit)
{
	int level = 0;
	
	while( (circuit[level].eachLevel)->qtyGates > 0 )
	{
		++level;
	}
	return level;
}

/*
 *	Verifica si la compuerta ya esta agregada a la tabla. Devuelve TRUE si lo esta
 *	FALSE en caso contratio
 */

int checkGateIsLoaded( circuitTable *circuit, char *name, int curLevel)
{
	int i;
	
	for( i = 0; i < (circuit[curLevel].eachLevel)->qtyGates ; ++i)
	{
		if( !strcmp((circuit[curLevel].eachLevel)->gates[i].name, (char*)name) )
			return i;
	}
	return -1;
}

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

int main(void){
	
	/*loadIPC();*/
	DIR *dp;
	struct dirent *d = NULL;
	int i, qtyFiles = 0, pos = 0;
	FILE *dataFile = NULL;
	circuitTable *auxTable;
	char *dir = "../bin/pipeDir/", *procDir = "../bin/processed/", *dirFile = NULL, *procCopyDir = NULL;	
	
	if ((dp = opendir(dir)) == NULL){
		perror("No se puede abrir el directorio\n");
		return errno;
	}
	while (getFilesAmm(dp) <= 3 ){
		sleep(1);
		rewinddir(dp);
	}
	sleep(2);
	rewinddir(dp);
	
	qtyFiles = getFilesAmm(dp) - 3;
	
	if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * (qtyFiles))) == NULL )
	{
		closedir(dp);
		perror("Error en la alocacion de memoria de table\n");
		return errno;	
	}
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable) * _MAX_GATES_LEVELS_)) == NULL )
		{
			closedir(dp);
			free(table);
			perror("Error en la alocacion de memoria de table[i]\n");
			return errno;	
		}
	}
	
	rewinddir(dp);
	while( getFilesAmm(dp) > 3 )
	{
		rewinddir(dp);
		while( (d = readdir(dp)) )
		{
			if(d->d_ino == 0 )
			{
				continue;
			}
			if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".svn") == 0){
				continue;
			}
			else{
				if( ( dirFile = (char*)malloc(sizeof(char) + strlen(dir) + strlen(d->d_name) + 1 )) == NULL )
				{
					closedir(dp);
					for( i = 0 ; i < qtyFiles ; ++i )
						free(table[i]);
					free(table);
					perror("Error en la alocacion de memoria\n");
					return errno;
				}
				strcpy(dirFile, dir);
				strcat(dirFile, d->d_name);
				if( (dataFile = fopen(dirFile, "r")) ==  NULL )
				{
					closedir(dp);
					free(dirFile);
					for( i = 0 ; i < qtyFiles ; ++i )
						free(table[i]);
					free(table);
					perror("No se pudo abrir el archivo de las compuertas\n");
					return errno;
				}
				
				auxTable = parseXMLGate( dirFile);
				if( auxTable[0].totalLevels > _MAX_GATES_LEVELS_ )
				{
					table[pos] = realloc( table[pos], sizeof(circuitTable*) * auxTable[0].totalLevels);
				}
				table[pos++] = auxTable;
				/*freeCircuits(&auxTable,1);*/
			}
			
			if( ( procCopyDir = (char*)realloc(procCopyDir, sizeof(char) + strlen(procDir) + strlen(d->d_name) + 1 )) == NULL )
			{
				closedir(dp);
				free(dirFile);
				for( i = 0 ; i < qtyFiles ; ++i )
					free(table[i]);
				free(table);
				perror("Error en la alocacion de memoria\n");
				return errno;
			}
			strcpy(procCopyDir,procDir);
			strcat(procCopyDir,d->d_name);
			fclose( dataFile );
			if( link(dirFile,procCopyDir) == -1)
			{
				perror("Error en el link de pipe\n");
				return errno;
			}
			unlink(dirFile);
		}
	}
	qtyFil = pos;
	free(dirFile);
	free(procCopyDir);
	return gateInitializer();
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateInitializer( void )
{
	int i, qtyFileCom, status, *maxLevel, *levels/*, flag = TRUE, aux*/;
	pid_t *childPids;
	/*circuitTable **table = NULL;*/
	
	fprintf(stderr, "GATE -- READ -- ParentPID: %d myPid: %d \n", getppid(), getpid() );
	/*
	if( readIPC(getppid(), &qtyFileCom, sizeof(int)) == -1 )
	{
		perror("Error en la lectura de GATES a PIPE en la cantidad de archivo\n");
		return errno;
	}
	
	if( (table = buildCircuitsTable(qtyFileCom)) == NULL )
	{
		perror("Error al construir la tabla de los circuitos\n");
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	finalizeIPC();*/
	
	/* COMIENZA EL PIPELINE */
	
	qtyFileCom = qtyFil;
	if( ( childPids = (int*)malloc( sizeof(pid_t) * qtyFileCom) ) == NULL )
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
		free(maxLevel);
		freeCircuits(table, qtyFileCom);
		return errno;
	}
	initLevels( &levels, qtyFileCom );
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		printCircuitTable(table[i]);
	}
	
	while( !allFilesWasProccessed(levels, qtyFileCom, maxLevel) )
	{
		if( (status = startCircuitsPipeline( table, &childPids, qtyFileCom, levels, maxLevel )) != 0)
		{
			perror("Error en el startCircuitsPipeline:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuits(table,qtyFileCom);
			return status;
		}
		fprintf(stderr, "Acabo de salir del startCircuitsPipeline con status: %d Voy a entrar a listenToMyChildren\n", status);
		if( (status = listenToMyChildren( table, childPids, qtyFileCom, &levels, maxLevel)) != 0 )
		{
			perror("Error en el listenToMyChildren:");
			free(childPids);
			free(maxLevel);
			free(levels);
			freeCircuits(table,qtyFileCom);
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
		incLevels(&levels, qtyFileCom);
		finalizeIPC();
	}
	for( i = 0 ; i < qtyFileCom ; ++i )
		saveProccessFile(table[i], i);
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		fprintf(stderr, "-- COMIENZO DEL CIRCUITO %d\n", i);
		printCircuitTable(table[i]);
		fprintf(stderr, "-- FIN DEL CIRCUITO %d\n", i);
	}
	
	/*freeCircuits(table, qtyFileCom);*/
	free(childPids);
	free(maxLevel);
	free(levels);
	fprintf(stderr, "Termine de correr el gateInitializer\n");
	return 0;
}

/*
 *	Funcion encargada de iniciar el proceso pipeline con todos los circuitos.
 */

int startCircuitsPipeline( circuitTable **table, pid_t **childPids, int qtyFiles, int *levels, int *maxLevel)
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
		}else
		{
			(*childPids)[i] = -1;
		}
	}
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		fprintf(stderr, "startCircuitsPipeline -- pid creados: %d\n", (*childPids)[i]);
	}
	
	synchronize();
	fprintf(stderr, "Acabo de sincronizar\n");
	for (i = 0 ; i < qtyFiles ; ++i)
	{
		if( levels[i] < maxLevel[i] && levels[i] >= 0 && (*childPids)[i] != -1 )
		{
			cur.curFile = i;
			cur.curLevel = levels[i];
			fprintf(stderr, "cur.curFile: %d cur.curLevel: %d\n", cur.curFile, cur.curLevel);
			
			fprintf(stderr, "Sending Cur -- MyPID: %d childPID: %d\n", getpid(), (*childPids)[i]);
			if( writeIPC( (*childPids)[i], &cur, sizeof(curCircuit) ) == -1 )
			{
				perror("Error en la escritura de cur hacia levels\n");
				return errno;	
			}
			printf("Envie el Sending cur\n");
			fprintf(stderr, "Sending the amount of currently gates -- MyPID: %d childPID: %d\n", getpid(), (*childPids)[i]);
			if( writeIPC( (*childPids)[i], (void *)&(((table[i][levels[i]]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
			{
				perror("Error en la escritura de la cantidad de gates de Currently\n");
				return errno;
			}
			
			for( j = 0 ; j < ((table[i][levels[i]]).eachLevel)->qtyGates ; ++j )
			{
				fprintf(stderr, "Sending each gate of currently J:%d -- MyPID: %d childPID: %d\n", j, getpid(), (*childPids)[i]);
				if( writeIPC( (*childPids)[i], (void*)(&(((table[i][levels[i]]).eachLevel)->gates[j])), sizeof(gate) ) == -1)
				{
					perror("Error en la escritura de compuertas\n");
					return errno;
				}
			}
			
			if( levels[i] == 0 )
			{
				fprintf(stderr, "Sending if it is the first level -- MyPID: %d childPID: %d\n", getpid(), (*childPids)[i]);
				if( writeIPC( (*childPids)[i], (void*)(&first), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
			}else
			{
				fprintf(stderr, "Sending if it is not the first level -- MyPID: %d childPID: %d\n", getpid(), (*childPids)[i]);
				if( writeIPC( (*childPids)[i], (void*)(&notFirst), sizeof(int) ) == -1 )
				{
					perror("Error en la escritura, indicando si es el primer nivel\n");
					return errno;
				}
				fprintf(stderr, "Sending the amount of previously gates -- MyPID: %d childPID: %d\n", getpid(), (*childPids)[i]);
				if( writeIPC( (*childPids)[i], (void *)&(((table[i][levels[i]-1]).eachLevel)->qtyGates) , sizeof(int) ) == -1)
				{
					perror("Error en la escritura de la cantidad de gates de Previously\n");
					return errno;
				}
				for( j = 0 ; j < ((table[i][ (levels[i]-1) ]).eachLevel)->qtyGates ; ++j )
				{
					fprintf(stderr, "Sending previously each gates -- j:%d MyPID: %d childPID: %d\n", j, getpid(), (*childPids)[i]);
					if( writeIPC( (*childPids)[i], (void*)(&(((table[i][levels[i]-1]).eachLevel)->gates[j])), sizeof(gate) ) == -1)
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

int listenToMyChildren( circuitTable **table, int *childPids, int qtyFiles, int **levels, int * maxLevel)
{
	int i, j, auxGet=-1, auxSelect, levelsAmm;
	gate auxGate;
	curCircuit cur;
	
	levelsAmm = getCurrentPipeFiles(*levels, qtyFiles, maxLevel);
	
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

circuitTable ** buildCircuitsTable( int qtyFileCom )
{
	int i,j,k, qtyGatesCom, qtyLevels;
	circuitTable **table;
	
	if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * qtyFileCom) ) == NULL )
	{
		perror("Error en la alocacion de memoria\n");
		return NULL;
	}
	/*for( i = 0 ; i < qtyFileCom ; ++i )
	{
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable))) == NULL )
		{
			perror("Error en la alocacion de memoria de table[i]\n");
			free(table);
			return NULL;	
		}
	}*/
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		readIPC(getppid(), &qtyLevels, sizeof(int));
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
			readIPC(getppid(), &qtyGatesCom, sizeof(int));
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			fprintf(stderr, "File: %d Level: %d Cantidad de compuertas: %d \n", i, j, qtyGatesCom);
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				free( (table[i][j].eachLevel) );
				for( i = 0 ; i < qtyFileCom ; ++i )
					free(table[i]);
				free(table);
				return NULL;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				readIPC(getppid(), &((((table[i][j]).eachLevel)->gates)[k]), sizeof(gate) );
			}
		}
	}
	return table;
}


/*
 *	Libera toda la lista de circuitos levantados
 */

void freeCircuits( circuitTable **table, int qtyFile )
{
	int i,j;
	
	for( i = 0 ; i < qtyFile ; ++i )
	{
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			free( ((table[i][j]).eachLevel)->gates );
			free( (table[i]) );
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
 *	Funcion que se encarga de verificar si queda algun archivo por procesar
 */

int allFilesWasProccessed( int * levels, int qtyFiles, int *maxLevel )
{
	int i;
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( levels[i] < 0 || (levels[i] >= 0 && levels[i] < maxLevel[i]) )
			return FALSE;
	}
	return TRUE;
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