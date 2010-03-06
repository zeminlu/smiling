/*
 *	luGen.c
 *			
 *		Programa generador del archivo para el problema de pipeline.
 *		Se van a crear dos archivos:
 *			1 - inicio.lu -- Aca se van a guardar las condiciones iniciales
 *			2 - ecuaciones.lu -- Aca se va a almacenar una matriz de float
 *								 con los valores de cada variable.
 */

#include <stdio.h>
#include <stdlib.h>

#include "../inc/luGen.h"
#include "../inc/pipe.h"

int main (int argc, char const *argv[])
{
	int i, j;
	int cantVar = 0;
	float **varValues;
	float *initCond;
	/*FILE *ecuFile, *initFile;*/
	
	printf("Ingrese la cantidad de variables que desea que tenga el sistema\n");
	
	if( scanf("%d", & cantVar) <= 0 )
	{
		printf("La cantidad de variables tiene que ser un numero natural positivo\n");
	}
	++cantVar;
	
	if( (varValues = (float**)malloc(sizeof(float*) * cantVar - 1)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	for(i = 0 ; i < cantVar ; ++i)
	{
		if( (varValues[i] = (float*)malloc(sizeof(float) * cantVar)) == NULL )
		{
			printf("Error en la alocacion de memoria\n");
		}
	}
	
	printf("A continuacion debera ingresar los coeficientes de cada variable\n");
	for( i = 0 ; i < cantVar - 1 ; ++i )
	{
		for( j = 0 ; j < cantVar ; ++j )
		{
			if( j == cantVar - 1 )
			{
				printf("Ingrese el termino independiente de la ecuacion %d\n", i+1);
				scanf("%f", &varValues[i][j]);
			}else
			{
				printf("Ingrese el valor de la variable numero %d y de la ecuacion %d \n", i+1, j+1);
				scanf("%f", &varValues[i][j]);
			}
		}
	}
	
	if( (initCond = malloc(sizeof(float) * cantVar - 1)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	printf("A continuacion debera ingresar las condiciones iniciales\n");
	for( i = 0; i < cantVar - 1; ++i)
	{
		printf("Ingrese las condiciones iniciales de la variable %d\n", i+1);
		scanf("%f", &initCond[i]);
	}
	
	createInitCondFile( cantVar - 1, initCond );
	createLUFile( varValues );
	
	freeAllPtr(varValues, initCond);
	
	/*if ((ecuFile = fopen("./testFiles/ecuaciones.lu", "r")) == NULL){
		return -1;
	}
	if ((initFile = fopen("./testFiles/inicial.lu", "r")) == NULL){
		return -1;
	}
	
	initCond = loadInitCondition(initFile);
	varValues = loadEquationFile( ecuFile);
	
	printInitCond(initCond);
	printMatrix(varValues);
	
	freeAllPtr(varValues, initCond);
	*/
	return 0;
}

/*
 *	Funcion que crea el archivo inicial.lu
 *	Recibe la cantidad de variables que hay y el vector de las condiciones iniciales.
 */

void createInitCondFile( int cantVar, float *cond )
{
	int i;
	char * filename = "./testFiles/inicial.lu";
	FILE * init;
	
	if((init = fopen(filename,"w+")) == NULL )
	{
		printf("No se puedo crear el archivo\n");
		exit(1);
	}
	if( fwrite(&cantVar, sizeof(int), 1, init) == 0 )
	{
		printf("Error no se pudo escribir en el archivo inicial.lu\n");
		fclose(init);
		exit(1);
	}
	for( i = 0; i < cantVar - 1; ++i)
	{
		if( fwrite(&cond[i], sizeof(float), 1, init) == 0 )
		{
			printf("Error no se pudo escribir en el archivo inicial.lu\n");
			fclose(init);
			exit(1);
		}	
	}
	printf("Se creo exitosamente el archivo inicial.lu\n");
	fclose(init);
}

/*
 *	Funcion que crea el archivo de ecuaciones.lu
 *	Recibe una matriz de float con todos los coeficientes de cada variables.
 */

void createLUFile( float ** values )
{
	int i,j, cantVar;
	char * filename = "./testFiles/ecuaciones.lu";
	FILE * ecu;
	
	if((ecu = fopen(filename,"w+")) == NULL )
	{
		printf("No se puedo crear el archivo\n");
		exit(1);
	}
	
	/*printMatrix(values);*/
	cantVar = sizeof(values)/sizeof(float);
	if( fwrite(&cantVar, sizeof(int), 1, ecu) == 0 )
	{
		printf("Error no se pudo escribir en el archivo inicial.lu\n");
		fclose(ecu);
		exit(1);
	}
	for( i = 0; i < sizeof(values)/sizeof(float) ; ++i)
	{
		for( j = 0; j <= sizeof(values[0])/sizeof(float); ++j)
		{
			if( fwrite(&values[i][j], sizeof(float), 1, ecu) == 0 )
			{
				printf("Error no se pudo escribir en el archivo inicial.lu\n");
				fclose(ecu);
				exit(1);
			}
		}
	}
	printf("Se creo exitosamente el archivo ecuaciones.lu\n");
	fclose(ecu);
	
}

/*
 *	Libera todos los punteros utilizados
 */

void freeAllPtr( float ** values, float *cond)
{
	
	int i;
	
	for( i = 0; i < sizeof(values)/sizeof(float); ++i)
	{
		free(values[i]);
	}
	free(values);
	free(cond);
}


void printMatrix( float ** values )
{
	
	int i,j;
	
	printf("La matriz es la siguientes:\n");
	for( i = 0 ; i < sizeof(values)/sizeof(float) ; ++i )
	{
		for( j = 0 ; j <= sizeof(values[0])/sizeof(float) ; ++j )
		{
			printf("%f  ",values[i][j] );
		}
		printf("\n");
	}
}

void printInitCond( float *init )
{
	int i;
	
	printf("Las condiciones iniciales son las siguientes:\n");
	for(i = 0; i < sizeof(init)/sizeof(float); ++i)
	{
		printf("%f ",init[i] );
	}
	printf("\n");
}

