/*
 *		pipe.c
 *		
 *			Programa encargado de verificar cuando se agrega un archivo en el directorio
 *			pipeDir.
 */

#include "../inc/pipe.h"

int signalFlag = TRUE;
static int qtyFiles = 0;
static circuitTable **table = NULL;
static int pos = 0;

void handlerCtrlC(int sig){
	signalFlag = FALSE;
}

int main (int argc, char const *argv[])
{

	if( createsGates() == -1 )
	{
		perror("Error en la creacion de la tablas");
		return errno;
	}
	return createsGates();
}

/*
 *	Funcion encargada de crear la tabla de circuitos.
 *	En caso de error retorna -1, sino 0.
 */

int createTable( void )
{
	int i;
	
	if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * _MAX_GATES_LEVELS_ )) == NULL )
	{
		perror("Error en la alocacion de memoria de table\n");
		return errno;	
	}
	
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable) * _MAX_GATES_LEVELS_)) == NULL )
		{
			perror("Error en la alocacion de memoria de table[i]\n");
			return errno;	
		}
	}
	return 0;
}

/*
 *	Funcion encargada de crear al hijo Gates y luego desde el padre va a comenzar a mandar las tablas.
 */

int createsGates(void)
{
	int pid, procStatus = 0;
	
	signal(SIGINT, handlerCtrlC);
	
	setupIPC(1);

	switch((pid = fork()))
	{
		case -1:
			perror("Error de fork");
			return errno;
			break;
		case 0:
			addClient();
			execv("./gates.bin", NULL);
			break;
		default:
			break;
	}	
	
	synchronize();
	while( signalFlag )
	{
		if( (procStatus = fileListener()) == -1)
		{
			perror("Error no se pudo escuchar el archivo");
			return errno;
		}
		fprintf(stderr, "Pipe pid: %d\n", pid);
		sendTableToGates(pid);
		freeCircuits(table,pos);
		if( createTable() == -1 )
		{
			perror("Error en la creacion de la tabla");
			return errno;
		}
		pos = 0;
		qtyFiles = 0;
	}
	wait(&pid);
	finalizeIPC();	
	return procStatus;
}

/*
 *	Funcion que escucha los archivos constantemente y los va procesando.
 *	En caso de que ocurra un error retorna -1, sino 0.
 */

int fileListener( void)
{
	DIR *dp;
	struct dirent *d = NULL;
	FILE *dataFile = NULL;
	circuitTable *auxTable;
	char *dir = "../bin/pipeDir/", *procDir = "../bin/processed/", *dirFile = NULL, *procCopyDir = NULL;	
	int i;
	
	if ((dp = opendir(dir)) == NULL){
		perror("No se puede abrir el directorio\n");
		return errno;
	}
	while (getFilesAmm(dp) <= 3 ){
		sleep(1);
		rewinddir(dp);
	}
	rewinddir(dp);
	
	qtyFiles = getFilesAmm(dp) - 3;
	
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
	free(dirFile);
	free(procCopyDir);
	return 0;
}

/*
 *	Funcion encargada de enviar todos los archivos que se procesaron. Retorna
 *	-1 en caso de error sino 0.
 */

int sendTableToGates( int pid )
{
	int i, j, k;
	
	writeIPC(pid, &pos, sizeof(int) );
	for( i = 0 ; i < pos ; ++i )
	{	
		fprintf(stderr, "Pipe -- WRITE -- MyPID: %d childPid: %d \n", getpid(), pid);
		if( writeIPC(pid, &(table[i][0].totalLevels), sizeof(int)) == -1 )
		{
			perror("Error en el write de totalLevels\n");
			return -1;
		}
		for( j = 0 ; j < table[i][0].totalLevels ; ++j )
		{
			fprintf(stderr, "Pipe -- WRITE -- MyPID: %d childPid: %d \n", getpid(), pid);
			if( writeIPC(pid, &((table[i][j].eachLevel)->qtyGates), sizeof(int)) == -1 )
			{
				perror("Error en el write de cantidad de compuertas\n");
				return -1;
			}
			
			for( k = 0 ; k < (table[i][j].eachLevel)->qtyGates ; ++k )
			{
				fprintf(stderr, "Pipe -- WRITE -- MyPID: %d childPid: %d \n", getpid(), pid);
				if( writeIPC(pid, &((table[i][j].eachLevel)->gates[k]), sizeof(gate) ) == -1 )
				{
					perror("Error en el write de la compuerta: a Gates\n");
					return -1;
				}
			}
		}
	}
	return 0;
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
		}
	}
	free(table);
}

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

/*
 *	Funcion para debug de la tabla levantada por el XML
 */

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
