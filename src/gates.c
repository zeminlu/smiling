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
	int i,j,k, qtyFileCom, qtyLevelsCom, qtyGatesCom, readBytes;
	circuitTable **table = NULL;
	
	readBytes = read(_stdin_, &qtyFileCom, sizeof(int));
	printf("Gate -- qtyFileCom: %d\n", qtyFileCom);
	
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
		readBytes = read(_stdin_, &qtyLevelsCom, sizeof(int));\
		printf("Gate -- NEW CIRCUIT n: %d -- qtyLevelsCom: %d\n", i, qtyLevelsCom );
		table[i][0].totalLevels = qtyLevelsCom;
		for( j = 0 ; j < qtyLevelsCom ; ++j )
		{
			if( ((table[i][j].eachLevel) = (gatesOfEachLevel*)malloc( sizeof(gatesOfEachLevel))) == NULL )
			{
				perror("Error en la alocacion de cada nivel\n");
				return errno;
			}
			readBytes = read(_stdin_, &qtyGatesCom, sizeof(int));
			printf("Gate -- qtyGatesCom: %d\n", qtyGatesCom);
			(table[i][j].eachLevel)->qtyGates = qtyGatesCom;
			
			if( ((table[i][j].eachLevel)->gates = (gate*)malloc( sizeof(gate) * qtyGatesCom)) == NULL )
			{
				perror("Error en la alocacion del arreglo de compuertas\n");
				return errno;
			}
			for( k = 0 ; k < qtyGatesCom ; ++k )
			{
				readBytes = read( _stdin_, &(((table[i][j].eachLevel)->gates)[k]), sizeof(gate) );
				printf("K: %d --- Gate: %s\n", k, (table[i][j].eachLevel)->gates[k].name);
			}
		}
		printf("Gate --------------------------------\n");
		printCircuitTable(table[i]);
		printf("Gate --------------------------------\n");
	}
	return 0;
}
