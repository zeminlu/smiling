#include "../inc/parallel.h"

int main(){
	DIR *dp;
	struct dirent *d;
	FILE *countryFile;
	country **countries = NULL;
	int i = 0, j = 0, fifa = 0;
	char *parDir = "./parallelDir/", *procDir = "./processed/", fileDir[60], procFileDir[60];
	
	if ((dp = opendir("./parallelDir")) == NULL){
		perror("Error al abrir el directiorio parallelDir");
		return errno;
	}
	while (getFilesAmm(dp) < 4){
		sleep(1);
		rewinddir(dp);
	}
	sleep(5);
	printf("Nuevo archivo en parallel\n");
	while (getFilesAmm(dp) > 3){
		rewinddir(dp);
		while ((d = readdir(dp))){
			printf("%s\n", d->d_name);
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
					if ((countries = realloc(countries, sizeof(void *) * i)) == NULL ||
						(countries[i - 1] = malloc(sizeof(country))) == NULL || 
					((fread(countries[i - 1], sizeof(country), 1, countryFile) != 1) && !feof(countryFile))){
						for(j = 0 ; j < i - 1 ; ++j){
							free(countries[i - 1]);
						}
						free(countries);
						fclose(countryFile);
						closedir(dp);
						perror("Error en la carga de un Pais");
						return errno;
					}
					else {
						if (feof(countryFile)){
							--i;
							free(countries[i]);
							if ((countries = realloc(countries, sizeof(void *) * i)) == NULL){
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
		switch (fork()){
			case 0:
				execv("./fifa.bin", NULL);
				/*Se le pasa a fifa la estructuras countries*/
				break;
			default:
				wait(&fifa);
				return 0;
				break;
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