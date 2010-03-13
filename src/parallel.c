#include "../inc/parallel.h"

int main(){
	DIR *dp;
	struct dirent *d;
	FILE *countryFile;
	int i = 0, j = 0, fifa = 0, auxP[2], bufferSize;
	void *buffer;
	country **countriesTable = NULL;
	int countriesTableEntriesAmm;
	char *parDir = "./parallelDir/", *procDir = "./processed/", fileDir[60], procFileDir[60];
	
	if ((dp = opendir("./parallelDir")) == NULL){
		perror("Error al abrir el directiorio parallelDir");
		return errno;
	}
	while (getFilesAmm(dp) < 4){
		sleep(1);
		rewinddir(dp);
	}
	sleep(2);
	rewinddir(dp);
	printf("Nuevo archivo en parallel\n");
	while (getFilesAmm(dp) > 3){
		rewinddir(dp);
		while ((d = readdir(dp))){
			if (d->d_ino == 0){
				continue;
			}	
			if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".svn") == 0){
				continue;
			}
			else{
				i = 0;
				strcpy(fileDir, parDir);
				strcat(fileDir, d->d_name);
				if ((countryFile = fopen(fileDir, "r")) == NULL){
					closedir(dp);
					perror("Error al abrir el archivo de countries");
					return errno;
				}
				while (feof(countryFile) == 0){
					++i;
					if ((countriesTable = realloc(countriesTable, sizeof(void *) * i)) == NULL ||
						(countriesTable[i - 1] = malloc(sizeof(country))) == NULL || 
					((fread(countriesTable[i - 1], sizeof(country), 1, countryFile) != 1) && !feof(countryFile))){
						for(j = 0 ; j < i - 1 ; ++j){
							free(countriesTable[i - 1]);
						}
						free(countriesTable);
						fclose(countryFile);
						closedir(dp);
						perror("Error en la carga de un Pais");
						return errno;
					}
					else {
						if (feof(countryFile)){
							--i;
							free(countriesTable[i]);
							if ((countriesTable = realloc(countriesTable, sizeof(void *) * i)) == NULL){
								fclose(countryFile);
								closedir(dp);
								perror("Error de memoria");
								return errno;
							}
						}
					}				
				}
				strcpy(procFileDir, procDir);
				strcat(procFileDir, d->d_name);
				fclose(countryFile);
				link(fileDir, procFileDir);
				unlink(fileDir);
			}
		}
		countriesTableEntriesAmm = i;
		
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			printf("ANTES Nombre: %s, Head: %d\n", countriesTable[j]->name, countriesTable[j]->isHead);
		}
		pipe(auxP);
		switch (fork()){
			case 0:
				close(auxP[1]);
				dup2(auxP[0], 0);
				execv("./fifa.bin", NULL);
				break;
			default:
				close(auxP[0]);
				write(auxP[1], &countriesTableEntriesAmm, sizeof(int));
				for (j = 0 ; j < i ; ++j){
					serializeStruct(countriesTable[j], &buffer, &bufferSize);
					write(auxP[1], &bufferSize, sizeof(int));
					write(auxP[1], buffer, bufferSize);
					free(buffer);	
				}
				
				wait(&fifa);
				break;
		}
	}
	return 0;
}

int serializeStruct(country *str, void **buffer, int *bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", str, 45);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
}

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