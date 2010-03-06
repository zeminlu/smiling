/*
 *		pipe.c
 *		
 *			Programa encargado de verificar cuando se agrega un archivo en el directorio
 *			pipeDir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>

#include "../inc/pipe.h"

int main(){
	
	DIR *dp;
	struct dirent *d;
	int i = 0;
	FILE *ecuFile, *initFile;
	float **values,*initCond;
	
	if ((dp = opendir("./pipeDir")) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) < 4){
		rewinddir(dp);
	};
	printf("Nuevo archivo en pipe\n");
	rewinddir(dp);
	while ((d = readdir(dp))){
		printf("%s\n", d->d_name);
		if (d->d_ino == 0){
			continue;
		}	
		if (strcmp(d->d_name, "inicial.lu") == 0){
			i = 0;
			if ((initFile = fopen("./pipelDir/inicial.lu", "r")) == NULL){
				return -1;
			}
			initCond = loadInitCondition(initFile);
		}
		else if (strcmp(d->d_name, "ecuaciones.lu") == 0){
			i = 0;
			if ((ecuFile = fopen("./pipeDir/ecuaciones.lu", "r")) == NULL){
				return -1;
			}
			values = loadEquationFile(ecuFile);
		}
		else if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".svn") == 0){
			continue;
		}
		else{
			printf("Se inserto un arhivo incorrecto\n");
		}
	}
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
 *	Carga los datos del archivo inicial.lu y retorna el vector de condiciones iniciales.
 *	El archivo que llama a esta funcion debe hacer el free de este vector cuando no se utilice mas.
 */

float * loadInitCondition ( FILE * initFile )
{
	int i = 0, cantVar;
	float *initCond;
	
	if( fread(&cantVar, sizeof(int), 1, initFile) == 0 )
	{
		printf("Error en la lectura del archivo\n");
		exit(1);
	}
	if( (initCond = (float*)malloc(sizeof(float) * cantVar)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
		exit(1);
	}
	while( feof(initFile) == 0 )
	{
		/*printf("i = %d\n",i );*/
		if( fread(&initCond[i], sizeof(float), 1, initFile) != -1 )
		{
			++i;
		}
	}
	fclose(initFile);
	return initCond;
}

/*
 *	Carga los datos del archivo ecuaciones.lu y retorna una matriz con los coeficientes
 *	de cada ecuacion y los terminos independientes de cada ecuacion. El formato de la 
 *	matriz es el siguiente:
 *
 *			a_0_0	a_0_1 	...		a_0_n	|	b_0
 *			a_1_0	a_1_1	...		a_1_n	|	b_1
 *			...
 *	El programa que lo llama debera hacer el free cuando no se utilice mas.
 */

float ** loadEquationFile( FILE * ecuFile )
{
	int i,j, cantVar;
	float ** values;
	
	i = j = 0;
	if( feof(ecuFile) == 0 )
	{
	 	if( fread(&cantVar, sizeof(int), 1, ecuFile) == 0 )
		{
			printf("Error en la lectura del archivo\n");
			exit(1);
		}	
	}
	
	++cantVar;
	if( (values = (float**)malloc(sizeof(float*) * cantVar - 1)) == NULL )
	{
		printf("Error en la alocacion de memoria\n");
		exit(1);
	}
	
	for(i = 0 ; i < cantVar ; ++i)
	{
		if( (values[i] = (float*)malloc(sizeof(float) * cantVar)) == NULL )
		{
			printf("Error en la alocacion de memoria\n");
			exit(1);
		}
	}
	
	i = j = 0;
	while( feof(ecuFile) == 0 )
	{
		/*printf("i = %d  j = %d \n", i,j);*/
		if( fread(&values[i][j], sizeof(float), 1, ecuFile) != 0 )
		{
			if( j == cantVar - 1 )
			{
				++i;
				j = 0;
			}else
			{
				++j;
			}
		}
	}
	fclose(ecuFile);
	return values;
}
