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
	while (getFilesAmm(dp) < 4){
		rewinddir(dp);
	};
	printf("Nuevo archivo en parallel\n");
	rewinddir(dp);
	while ((d = readdir(dp))){
		printf("%s\n", d->d_name);
		if (d->d_ino == 0){
			continue;
		}	
		if (strcmp(d->d_name, "paises.fifa") == 0){
			i = 0;
			if ((paisesF = fopen("./parallelDir/paises.fifa", "r")) == NULL){
				return -1;
			}
			while (feof(paisesF) == 0){
				++i;
				if ((paises = realloc(paises, sizeof(void *) * i)) == NULL ||
					(paises[i - 1] = malloc(sizeof(pais))) == NULL || 
					((fread(paises[i - 1], sizeof(pais), 1, paisesF) != 1) && !feof(paisesF))){
					return -1;
				}
				else {
					if (feof(paisesF)){
						--i;
						free(paises[i]);
						paises = realloc(paises, sizeof(void *) * i);
					}
				}				
			}
		}
		else if (strcmp(d->d_name, "cabezas.fifa") == 0){
			i = 0;
			if ((cabezasF = fopen("./parallelDir/cabezas.fifa", "r")) == NULL){
				return -1;
			}
			while (feof(cabezasF) == 0){
				++i;
				if ((cabezas = realloc(cabezas, sizeof(void *) * i)) == NULL ||
					(cabezas[i - 1] = malloc(sizeof(cabeza))) == NULL ||
					((fread(cabezas[i - 1], sizeof(cabeza), 1, cabezasF) != 1) && !feof(cabezasF))){
						return -1;
				}
				else {
					if (feof(cabezasF)){
						--i;
						free(cabezas[i]);
						cabezas = realloc(cabezas, sizeof(void *) * i);
					}
				}
			}
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