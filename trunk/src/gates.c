/*
 *		lu.c
 *
 *			Archivo encargado de llevar a cabo todos los procesos de resolucion
 *			de un sistema de ecuaciones lineales por el metodo LUX = B.
 */

#include "../inc/gates.h"

/*void fillMatrix( float **mtx1, float **mtx2, int row1, int col1, int row2, int col2)
{
	int i,j;
		
	for( i = 0 ; i < row1 ; ++i )
	{
		for( j = 0; j < col1 ; ++j)
		{
			mtx1[i][j] = 1;
		}
	}

	for( i = 0 ; i < row2 ; ++i )
	{
		for( j = 0; j < col2 ; ++j)
		{
			mtx2[i][j] = 2;
		}
	}
}*/

/*int main (int argc, char const *argv[])
{
	float **mtx1 = NULL, **mtx2 = NULL ,**rta = NULL;
	int i,j;
	
	if( (mtx1 = (float**)malloc(sizeof(float*) * 3)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	for(i = 0 ; i < 3 ; ++i)
	{
		if( (mtx1[i] = (float*)malloc(sizeof(float) * 2)) == NULL )
		{
			printf("Error en la alocacion de memoria\n");
		}
	}
	
	if( (mtx2 = (float**)malloc(sizeof(float*) * 2)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
	}
	
	for( i = 0 ; i < 2  ; ++i)
	{
		if( (mtx2[i] = (float*)malloc(sizeof(float) * 3)) == NULL )
		{
			printf("Error en la alocacion de memoria\n");
		}
	}
	
	fillMatrix(mtx1,mtx2, 3, 2, 2, 3);
	printf("Matriz 1:\n");
	printMatrix(mtx1, 3, 2);
	printf("Matrix 2:\n");
	printMatrix(mtx2, 2, 3);
	printf("Matriz resultado: \n");
	if( (rta = productTwoMatrix(mtx1, mtx2, 3, 2, 2, 3)) != NULL )
	{
		printMatrix(rta,3,3);
	}else
	{
		printf("Error en el producto de la matriz\n");
	}	
	return 0;
}*/

/*
 *		Esta funcion realiza el producto de dos matrices. Obviamente para que se
 * 		pueda realizar el producto la matriz 1 tiene que tener dimension M x S
 *		y la matriz 2 de S X N.
 *		Params: La funcion recibe como parametros la matriz 1, matriz 2 y un float **rta.
 *				Rta tiene que ser un puntero con valor NULL, dentro de la funcion se crea
 *				la matriz nueva y se retorna su valor por ahi.
 *		Retorno: retorna la matriz resultante, en caso de error devuelve NULL
 */


/*float ** productTwoMatrix( float ** matrix1, float **matrix2, int qtyF1, int qtyC1, int qtyF2, int qtyC2 )
{
	
	int i,j,k;
	float **rta;
	
	if( qtyC1 != qtyF2 )
	{
		printf("No es posible multiplicar las matrices\n");
		return NULL;
	}
	if( (rta = (float**)malloc(sizeof(float*)*qtyF1)) == NULL )
	{
		printf("Error al alocar memoria\n");
		return NULL; 
	}
	for( i = 0 ; i < qtyC2 ; ++i )
	{
		if( (rta[i] = (float*)malloc(sizeof(float) * qtyC2 )) == NULL )
		{
			printf("Error al alocar memoria\n");
			return NULL; 
		}
	}
	
	for( i = 0 ; i < qtyF1 ; ++i )
	{
		for( j = 0 ; j < qtyC2 ; ++j )
		{
			rta[i][j] = 0;
			for( k = 0 ; k < qtyC1 ; ++k )
			{
				rta[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	return rta;
}*/