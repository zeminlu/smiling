/*
 *	luGen.c
 *			
 *		Programa generador del archivo para el problema de pipeline.
 *		Se van a crear dos archivos:
 *			1 - inicio.lu -- Aca se van a guardar las condiciones iniciales
 *			2 - ecuaciones.lu -- Aca se va a almacenar una matriz de float
 *								con los valores de cada variable.
 */

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char const *argv[])
{
	size_t i, j, count = 0;
	int cantVar = 0;
	float **varValues;
	
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
	
	for(i = 0; i < cantVar; ++i)
	{
		for(j = 0; j < count; ++j)
		{
			printf("Ingrese el valor de la \n");
		}
	}
	
	return 0;
}