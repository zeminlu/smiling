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
#include <sys/wait.h>

#include "../inc/pipe.h"

int main(){
	
	DIR *dp;
	struct dirent *d;
	int i = 0, gate = 0;
	FILE *dataFile;
	char *dir = "./pipeDir/", *procDir = "./processed/", *dirFile, *procCopyDir;
	
	if ((dp = opendir(dir)) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) <= 3 ){
		sleep(1);
		rewinddir(dp);
	};
	while( (d = readdir(dp)) && getFilesAmm(dp) > 3 )
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
				perror("Error en la alocacion de memoria\n");
				return errno;
			}
			strcpy(dirFile, dir);
			strcat(dirFile, d->d_name);
			if( (dataFile = fopen(dir, "r")) ==  NULL )
			{
				closedir(dp);
				perror("No se pudo abrir el archivo de las compuertas\n");
				return errno;
			}
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
	fclose( dirFile );
	link(dirFile,procCopyDir);
	unlink(dirFile);
	
	switch( fork() ){
		case 0:
			execv("./gates.bin");
			break;
		case -1:
			perror("Error en el fork del pipeline\n");
			break;
		default:
			wait(&gate);
			return 0;
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
