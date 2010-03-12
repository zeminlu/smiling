/*
 *		pipe.c
 *		
 *			Programa encargado de verificar cuando se agrega un archivo en el directorio
 *			pipeDir.
 */
#include "../inc/pipe.h"

int main(){
	
	DIR *dp;
	struct dirent *d;
	int i,gate = 0, qtyFiles = 0, pos = 0;
	FILE *dataFile = NULL;
	circuitTable **table = NULL;
	char *dir = "../bin/pipeDir/", *procDir = "../bin/processed/", *dirFile = NULL, *procCopyDir;
	

	if ((dp = opendir(dir)) == NULL){
		perror("No se puede abrir el directorio\n");
		return errno;
	}
	while (getFilesAmm(dp) <= 3 ){
		sleep(1);
		rewinddir(dp);
	}
	qtyFiles = getFilesAmm(dp);
	printf("qtyFiles: %d\n", qtyFiles);
	
	if( (table = (circuitTable**)malloc( sizeof(circuitTable*) * qtyFiles)) == NULL )
	{
		closedir(dp);
		perror("Error en la alocacion de memoria de table\n");
		return errno;	
	}
	for( i = 0 ; i < qtyFiles ; ++i )
	{
		if( (table[i] = (circuitTable*)malloc( sizeof(circuitTable))) == NULL )
		{
			closedir(dp);
			perror("Error en la alocacion de memoria de table[i]\n");
			return errno;	
		}
	}
	rewinddir(dp);
	while( getFilesAmm(dp) > 3 && (d = readdir(dp)) )
	{
		printf("%s\n", d->d_name );
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
				perror("Error en la alocacion de memoria\n");
				return errno;
			}
			strcpy(dirFile, dir);
			strcat(dirFile, d->d_name);
			if( (dataFile = fopen(dirFile, "r")) ==  NULL )
			{
				closedir(dp);
				perror("No se pudo abrir el archivo de las compuertas\n");
				return errno;
			}
			table[pos++] = parseXMLGate( dirFile);
			
			/* Aca hay que cargar el archivo de las compuertas */
		}
	}
	if( ( procCopyDir = (char*)malloc(sizeof(char) + strlen(procDir) + strlen(d->d_name) + 1 )) == NULL )
	{
		closedir(dp);
		perror("Error en la alocacion de memoria\n");
		return errno;
	}
	strcpy(procCopyDir,procDir);
	strcat(procCopyDir,d->d_name);
	fclose( dataFile );
	link(dirFile,procCopyDir);
	unlink(dirFile);
	
	switch( fork() ){
		case 0:
			execv("./gates.bin", NULL);
			break;
		case -1:
			perror("Error en el fork del pipeline\n");
			break;
		default:
			wait(&gate);
			break;
	}
	free(dirFile);
	free(procCopyDir);
	return 0;
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
	/* ---------- For debugging -------------*/
	/*printf("Estoy en la funcion parseXMLGate y voy a entrar a parseCircuit\n");
	printf("ParseXMLGate, Cur: %s\n", cur->name);
	cur = cur->xmlChildrenNode;
	while( cur != NULL )
	{
		if( !xmlIsBlankNode(cur) )
		{
			printf("ParseXMLGate, Children, Cur: %s\n", cur->name);
		}
		cur = cur->next;
	}*/
	/*--------- Finish Debugging ------*/
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
	int j,i,curLevel = 0;
	
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
		circuit[i].level = i;
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
			(circuit[i].eachLevel)->gates[j].fathers[0] = malloc( sizeof(char) * 30);
			(circuit[i].eachLevel)->gates[j].fathers[1] = malloc( sizeof(char) * 30);
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
	printCircuitTable(circuit);
	return circuit;
}

/*
 *	Funcion que parsea las compuertas y arma la lista por niveles.
 *	No cuenta los niveles, sino que arma la matriz unicamente.
 */

void parseGatesTags( char *father, xmlNodePtr cur, circuitTable * circuit, int curLevel )
{
	
	int pos = 0, posExist = 0;
	xmlChar * type, *input1, *input2;
	
	if( cur == NULL )
	{
		return;
	}else
	{
		while( xmlIsBlankNode(cur) )
		{
			cur=cur->next;
		}
		if( curLevel == _MAX_GATES_LEVELS_ - 1 )
		{
			circuit = (circuitTable*)realloc(circuit, sizeof(circuitTable) * curLevel * _MAX_GATES_LEVELS_ );
		}
		while( cur != NULL )
		{
			parseGatesTags( (char*)cur->name, cur->xmlChildrenNode, circuit, curLevel + 1 );
			if( !xmlIsBlankNode(cur) )
			{
				pos = (circuit[curLevel].eachLevel)->qtyGates;
				if( (circuit[curLevel].eachLevel)->qtyGates == _MAX_GATES_LEVELS_ - 1 )
				{
					
					(circuit[curLevel].eachLevel)->gates = (gate*)realloc((circuit[curLevel].eachLevel)->gates, 
															sizeof(gate) * pos * _MAX_GATES_LEVELS_);
				}
				posExist = checkGateIsLoaded( circuit, (char*)cur->name, curLevel);
				if( posExist != -1 )
				{
					strcpy( (circuit[curLevel].eachLevel)->gates[posExist].fathers[1], father );
				}else
				{
					strcpy((circuit[curLevel].eachLevel)->gates[pos].name, (char*)cur->name);
					if( father != NULL )
					{
						strcpy( (circuit[curLevel].eachLevel)->gates[pos].fathers[0], father );
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
					(circuit[curLevel].eachLevel)->gates[pos].fnGate = getHandler((char*)type, &((circuit[curLevel].eachLevel)->gates[pos].type));
					xmlFree(type);
					(circuit[curLevel].eachLevel)->qtyGates++;
					
				}
			}
			cur = cur->next;
		}
	}
}

/*
 *	Devuelve la funcion encargada de resolver la compuerta y se le pasa un puntero
 *	al tipo de compuerta que es, correspondiente a esa compuerta, y se almacena ahi
 *	el valor indicado.
 */

handler getHandler( char * typeH, int *typeInt)
{
	int type = 0;
	
	type = atoi(typeH);
	switch( type )
	{
		case AND:
			*typeInt = AND;
			return gateAnd;			
		case OR:
			*typeInt = OR;
			return gateOr;			
		case XOR:
			*typeInt = XOR;
			return gateXor;
		case NAND:
			*typeInt = NAND;
			return gateNand;
		case NOR:
			*typeInt = NOR;
			return gateNor;
		case XNOR:
			*typeInt = XNOR;
			return gateXnor;
	}
	return NULL;
}

/* 
 *	Cuenta la cantidad de niveles que tiene el circuito
 */

int countLevels( circuitTable* circuit)
{
	int level = 0;
	
	while( (circuit[level].eachLevel)->qtyGates != 0 )
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

/*
 *	Funcion para debagueo de la tabla levantada por el XML
 */

void printCircuitTable( circuitTable * circuit)
{
	int i,j;
	
	for( i = 0; i < circuit[0].totalLevels ; ++i)
	{
		for( j = 0; j < (circuit[i].eachLevel)->qtyGates ; ++j)
		{
			printf("LEVEL: %d Name: %s, Father[0]: %s, Father[1]: %s, Type: %d, Input[0]: %d, Input[1]: %d\n", 
						i,
						(circuit[i].eachLevel)->gates[j].name,
						(circuit[i].eachLevel)->gates[j].fathers[0],
						(circuit[i].eachLevel)->gates[j].fathers[1],
						(circuit[i].eachLevel)->gates[j].type,
						(circuit[i].eachLevel)->gates[j].input[0],
						(circuit[i].eachLevel)->gates[j].input[1]);
		}
	}
}