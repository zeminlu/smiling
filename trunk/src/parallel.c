#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include "../inc/parallel.h"
#include "../inc/types.h"

int main(){
	DIR *dp;
	struct dirent *d;
	FILE *paisesF, *cabezasF;
	pais **paises = NULL;
	cabeza **cabezas = NULL;
	size_t i = 0;
	
	if ((dp = opendir("./parallelDir")) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) < 4);
	printf("Nuevo archivo en parallel\n");
	rewinddir(dp);
	readdir(dp);
	readdir(dp);
	while ((d = readdir(dp))){
		if (d->d_ino == 0){
			continue;
		}
		if (strcmp(d->d_name, "paises.fifa") == 0){
			i = 0;
			if ((paisesF = fopen(d->d_name, "r")) == NULL){
				return -1;
			}
			while (feof(paisesF) == 0){
				++i;
				if((paises = realloc(paises, sizeof(void *) * i)) == NULL ||
					(paises[i - 1] = malloc(sizeof(pais))) == NULL){
					if (fread(paises[i - 1], sizeof(pais), 1, paisesF) != 1){
						return -1;
					}
				}
			}
		}
		else if (strcmp(d->d_name, "cabezas.fifa") == 0){
			i = 0;
			if ((cabezasF = fopen(d->d_name, "r")) == NULL){
				return -1;
			}
			while (feof(cabezasF) == 0){
				++i;
				if ((cabezas = realloc(cabezas, sizeof(void *) * i)) == NULL ||
					(cabezas[i - 1] = malloc(sizeof(cabeza))) == NULL){
					if (fread(cabezas[i - 1], sizeof(cabeza), 1, cabezasF) != 1){
						return -1;
					}
				}
			}
		}
		else if (strcmp(d->d_name, ".svn") == 0){
			
		}
		else{
			printf("Se inserto un arhivo incorrecto\n");
		}
	}
	for(; i >=0 ;i--){
		printf("Pais %s Continente %d Campeon %d Peso %d", (paises[i])->nombre, (paises[i])->continente, (paises[i])->campeon, (paises[i])->peso);
	}
	
	return 0;
}

int getFilesAmm (DIR *dp){
	struct dirent *d;
	size_t ret = 0;
	while ((d = readdir(dp))){
		 if (d->d_ino != 0){
			++ret;
		}
	}
	return ret;
}