#include "../inc/parallel.h"

int signalFlag = TRUE;

void handler(int sig){
	signalFlag = FALSE;
}

int main(){
	return filesListener();
}

int filesListener(){
	DIR *dp;
	int i, j, fifa, bufferSize, pid = 0, countriesTableEntriesAmm, countriesTableEntriesAmm2, childs = 0;
	void *buffer = NULL;
	country **countriesTable = NULL;
	
	signal(SIGINT, handler);
	

	if ((dp = opendir("./parallelDir")) == NULL){
		perror("Error al abrir el directiorio parallelDir");
		return errno;
	}
	while (signalFlag){
		usleep(100000);
		rewinddir(dp);

		if ((countriesTableEntriesAmm =  processFile(dp, &countriesTable)) < 0){
			return countriesTableEntriesAmm; 
		}
		else if (countriesTableEntriesAmm == 0){
			continue;
		}
		++childs;
		setupIPC(1);

		switch((pid = fork())){
			case -1:
				perror("Error de fork");
				return errno;
				break;
			case 0:
				addClient();
				execv("./fifa.bin", NULL);
				break;
			default:					
				break;
		}
		if (synchronize() != 0){
			closedir(dp);
			return -1;
		}
		if (writeIPC(pid, &countriesTableEntriesAmm, sizeof(int)) != 0){
			closedir(dp);
			return -1;
		}
		
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			serializeCountryStruct(&buffer, &bufferSize, countriesTable[j]);
			if (writeIPC(pid, &bufferSize, sizeof(int)) != 0 || writeIPC(pid, buffer, bufferSize) != 0){
				for (i = 0 ; i < j ; ++i){
					free(countriesTable[i]);
				}
				free(buffer);
				finalizeIPC();
				closedir(dp);
				return -1;	
			}
			varFree(2, buffer, countriesTable[j]);
		}
		free(countriesTable);
		countriesTable = NULL;
		if (readIPC(pid, &countriesTableEntriesAmm2, sizeof(int)) == -1){
			fprintf(stderr, "Error leyendo señal de fifa\n");
			finalizeIPC();
			return -1;
		}
		if (countriesTableEntriesAmm2 < countriesTableEntriesAmm){
			fprintf(stderr, "Fifa no recibio bien la tabla de paises\n");
			finalizeIPC();
			return -1;
		}
		finalizeIPC();
	}

	for (i = 0 ; i < childs ; ++i){
		wait(&fifa);
		if (fifa < 0){
			fprintf(stderr, "Error en fifa del archivo procesado nro = %d\n", i);
		}
	}
	
	printf("\n%d archivos procesados.\n", childs);
	
	closedir(dp);

	return 0;
}

int processFile(DIR *dp, country ***countriesTable){
	struct dirent *d;
	int i = 0, j = 0;
	char *parDir = "./parallelDir/", *procDir = "./processed/", fileDir[60], procFileDir[60];
	FILE *countryFile = NULL;
	while ((d = readdir(dp))){
		if (d->d_ino == 0){
			continue;
		}
		
		if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".svn") == 0){
			continue;
		}else{
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
				if (((*countriesTable) = realloc((*countriesTable), sizeof(void *) * i)) == NULL ||
					((*countriesTable)[i - 1] = malloc(sizeof(country))) == NULL || 
				((fread((*countriesTable)[i - 1], sizeof(country), 1, countryFile) != 1) && !feof(countryFile))){
					for(j = 0 ; j < i - 1 ; ++j){
						free((*countriesTable)[i - 1]);
					}
					free((*countriesTable));
					fclose(countryFile);
					closedir(dp);
					perror("Error en la carga de un Pais");
					return errno;
				}
				else {
					if (feof(countryFile)){
						--i;
						free((*countriesTable)[i]);
						if (((*countriesTable) = realloc((*countriesTable), sizeof(void *) * i)) == NULL){
							fclose(countryFile);
							closedir(dp);
							perror("Error de memoria en processFile de parallel");
							return errno;
						}
					}
				}				
			}

			strcpy(procFileDir, procDir);
			strcat(procFileDir, d->d_name);
			fclose(countryFile);

			if (rename(fileDir, procFileDir) < 0){
				return -1;
			}
			
			return i;
		}
	}
	return 0;
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
