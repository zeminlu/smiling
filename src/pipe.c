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
#include "../inc/luGen.h"


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
			initCond = loadInitCond(initFile);
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