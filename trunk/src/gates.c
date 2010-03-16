/*
 *		gates.c
 *
 *			Archivo encargado de llevar a cabo todos los procesos de resolucion
 *			de un circuito de compuertas.
 */

#include "../inc/gates.h"


void printCircuitTable( circuitTable * circuit)
{
	int i,j;

	for( i = 0; i < circuit[0].totalLevels ; ++i)
	{
		for( j = 0; j < (circuit[i].eachLevel)->qtyGates ; ++j)
		{
			fprintf(stderr, "LEVEL: %d Name: %s, Father[0]: %s, Father[1]: %s, Type: %d, Input[0]: %d, Input[1]: %d\n", 
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

int main(void){
	return gateServer();
}

/*
 * 	Funcion encargada de comunicarse con pipe. Va a ser la encargada
 *	de leer toda la informacion y rearmar de vuelta toda la tabla de
 *	circuitos.
 */

int gateServer( void )
{
	int i,j,k, qtyFileCom, qtyLevelsCom, qtyGatesCom, input1, input2, type;
	circuitTable **table = NULL;
	curGateProcess curCircuit;
	
	read(_stdin_, &curCircuit, sizeof(curCircuit));
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
	
	for( i = 0 ; i < qtyFileCom ; ++i )
	{
		read(_stdin_, &qtyLevelsCom, sizeof(int));\
		table[i][0].totalLevels = qtyLevelsCom;
		for( j = 0 ; j < qtyLevelsCom ; ++j )
		{
			if( ((table[i][j].eachLevel) = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			read(_stdin_, &qtyGatesCom, sizeof(int));
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				read( _stdin_, &(((table[i][j].eachLevel)->gates)[k]), sizeof(gate) );
			}
		}
	}
	
	for( i = 0 ; i < (table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->qtyGates ; ++i )
	{
		type = ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].type;
		if( curCircuit.curLevel == 0 )
		{
			input1 = ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[0];
			input2 = ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[1];
		}else{
			if( ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[0] != '\0' )
			{
				if( ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[0] == -1 )
				{
					input1 = getInputFromFather( table, curCircuit.curLevel - 1, curCircuit.curFile,
													((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[0])
				}else
				{
					input2 = getInputFromFather( table, curCircuit.curLevel - 1, curCircuit.curFile,
													((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[0])
				}
			}else
			{
				input1 = ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[0];
			}
			
			if( ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[1] != '\0' )
			{
				if( ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[1] == -1 )
				{
					input1 = getInputFromFather( table, curCircuit.curLevel - 1, curCircuit.curFile,
													((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[1])
				}else
				{
					input2 = getInputFromFather( table, curCircuit.curLevel - 1, curCircuit.curFile,
													((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].fathers[1])
				}
			}else
			{
				input1 = ((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].input[1];
			}
		}
		
		((table[curCircuit.curFile][curCircuit.curLevel]->eachLevel)->gates)[i].output = getOutput(table, input1,input2,);
	}
	
	if( curCircuit.curLevel == (table[curCircuit.curFile][0]).totalLevels )
	{
		curCircuit.curLevel = 0;
		curCircuit.curFile++;
		/* FALTA CREAR EL ARCHIVO DE SALIDA DEL CIRCUITO YA PROCESADO */
	}else
	{
		curCircuit.curLevel++;
	}
	
	/* aca hay que hacer el fork para sus hijos y volver a pasar todo */
	
	return 0;
}

/*
 *	Funcion encargada de retornar el output de una compuerta.
 */

int getOutput( int type, int in1, int in2 )
{
	return getGateHandler(type,in1,in2);
}

int getGateHandler( int type, int in1, int in2 )
{
	switch( type )
	{
		case AND:
			return gateAnd(in1,in2);
		case OR:
			return gateOr(in1,in2);
		case XOR:
			return gateXor(in1.in2);
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

int getInputFromFather( circuitTable **table, int level, int file, char[] fa)
{
	
	int i;
	
	for( i = 0 ; i < (table[file][level].eachLevel)->qtyGates  ; ++ i )
	{
		if( strcmp( ((table[file][level].eachLevel)->gates)[i].name, fa ) == 0 )
			return ((table[file][level].eachLevel)->gates)[i].output;
	}
	return -1;
}