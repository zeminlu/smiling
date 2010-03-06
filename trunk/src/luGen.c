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

int main (int argc, char const *argv[])
{
	size_t i, j, count = 0;
	int cantVar = 0;
	float **varValues;
	float initCond[];
	
	printf("Ingrese la cantidad de variables que desea que tenga el sistema\n");
	
	if( (cantVar = scanf("%d")) <= 0 )
	{
		printf("La cantidad de variables tiene que ser un numero natural positivo\n");
	}
	++cantVar;
	
	if( (varValues = malloc(sizeof(float) * cantVar)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	for(i = 0 ; i < cantVar ; ++i)
	{
		if( (varValues[i] = malloc(sizeof(float) * cantVar)) == NULL )
		{
			printf("Error en la alocacion de memoria\n");
		}
	}
	
	printf("A continuacion debera ingresar los coeficientes de cada variable\n");
	for( size_t i = 0 ; i < cantVar ; ++i )
	{
		for( size_t j = 0 ; j < cantVar ; ++j )
		{
			if( j == cantVar - 1 )
			{
				printf("Ingrese el termino independiente de la ecuacion %d\n", i+1);
				varValues[i][j] = scanf("%f");
			}else
			{
				printf("Ingrese el valor de la variable numero %d y de la ecuacion %d \n", i+1, j+1);
				varValues[i][j] = scanf("%f");
			}
		}
	}
	
	if( (initCond = malloc(sizeof(float) * cantVar - 1)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	printf("A continuacion debera ingresar las condiciones iniciales\n");
	for(size_t i = 0; i < cantVar - 1; ++i)
	{
		printf("Ingrese las condiciones iniciales de la variable %d\n", i+1);
		initCond[i] = scanf("%f");
	}
	
	createInitCondFile( cantVar - 1, initCond );
	createLUFile( varValues );
	
	return 0;
}

/*
 *	Funcion que crea el archivo inicial.lu
 *	Recibe la cantidad de variables que hay y el vector de las condiciones iniciales.
 */

void createInitCondFile( int cantVar, float *init )
{
	char * filename = "inicial.lu";
	FILE * init;
	
	if((init = fopen(filename,"a+")) == NULL )
	{
		printf("No se puedo crear el archivo\n");
	}
}

/*
 *	Funcion que crea el archivo de ecuaciones.lu
 *	Recibe una matriz de float con todos los coeficientes de cada variables.
 */

void createLUFile( float ** values )
{
	
	
}
