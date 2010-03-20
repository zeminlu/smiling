#include "../inc/parallel.h"

int main(){
	
	return filesListener();
}

int filesListener(){
	DIR *dp;
	int j, fifa, bufferSize, pid = 0, countriesTableEntriesAmm;
	void *buffer = NULL;
	country **countriesTable = NULL;
	
	if ((dp = opendir("./parallelDir")) == NULL){
		perror("Error al abrir el directiorio parallelDir");
		return errno;
	}
	
	while (getFilesAmm(dp) < 4){
		sleep(1);
		rewinddir(dp);
	}
	
	sleep(1);
	
	rewinddir(dp);
	
	while (getFilesAmm(dp) > 3){
		rewinddir(dp);
		if ((countriesTableEntriesAmm =  processFile(dp, &countriesTable)) < 0){
			return countriesTableEntriesAmm; 
		}
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
	}
	synchronize();
	writeIPC(pid, &countriesTableEntriesAmm, sizeof(int));
	
	for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
		serializeCountryStruct(&buffer, &bufferSize, countriesTable[j]);
		writeIPC(pid, &bufferSize, sizeof(int));
		writeIPC(pid, buffer, bufferSize);
		free(buffer);	
	}
	
	wait(&fifa);
	closeIPC(pid);

	return 0;
}

int processFile(DIR *dp, country ***countriesTable){
	struct dirent *d;
	int i = 0, j;
	char *parDir = "./parallelDir/", *procDir = "./processed/", fileDir[60], procFileDir[60];
	FILE *countryFile;
	
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
	return i;
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