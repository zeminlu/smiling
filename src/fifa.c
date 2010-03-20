#include "../inc/fifa.h"

int main (void){
	int i, j, status, countriesTableEntriesAmm;
	pid_t *pids;
	void *master = NULL, *set = NULL;
	country ***fixture = NULL, **countriesTable = NULL;
	
	loadIPC();
	
	if ((countriesTableEntriesAmm = loadCountriesTable(&countriesTable)) < 0){
		return countriesTableEntriesAmm;
	}
	
	if ((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL || (fixture = malloc(sizeof(void *) * countriesTableEntriesAmm / 4)) == NULL){
		perror("Error de memoria");
		free(pids);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}
		free(countriesTable);
		return errno;
	}
	
	if ((status = startChildProcesses(countriesTable, countriesTableEntriesAmm, &fixture, &pids)) != 0){
		free(pids);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(fixture[j]);
		}
		free(fixture);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}
		free(countriesTable);
		return status;
	}
		
	if ((status = childsListener(pids, countriesTable, countriesTableEntriesAmm, fixture)) != 0){
		free(pids);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}
		free(countriesTable);
		free(master);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			for(i = 0 ; i < 4 ; ++i){
				free(fixture[j][i]);
			}
			free(fixture[j]);
		}
		free(fixture);
		return status;
	}	
	
	/*
	Guardar a archivo la solucion
	*/
	
	printf("\n");
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		printf("Grupo %d:\n", j + 1);
		for(i = 0; i < 4; ++i){
			printf("%s\n", (fixture[j][i])->name);
			free(fixture[j][i]);
		}
		printf("\n");
		free(fixture[j]);
	}
	free(fixture);
	free(pids);
	free(set);
	free(master);
	for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
		free(countriesTable[j]);
	}
	free(countriesTable);
	
	return 0;
}

int loadCountriesTable(country ***countriesTable){
	int countriesTableEntriesAmm, i, j, bufferSize;
	void *buffer;
	
	readIPC(getppid(), &countriesTableEntriesAmm, sizeof(int));
		
	if (((*countriesTable) = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		readIPC(getppid(), &bufferSize, sizeof(int));
		
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*countriesTable)[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			free(buffer);
			for(j = 0 ; j < i ; ++j){
				free((*countriesTable)[j]);
			}
			free((*countriesTable));
			return errno;
		}
		
		readIPC(getppid(), buffer, bufferSize);
		unserializeCountryStruct(buffer, bufferSize, (*countriesTable)[i]);
		free(buffer);		
	}
	closeIPC(getppid());
	return countriesTableEntriesAmm;	
}

int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids){
	int i, j, headsAmm = 0, bufferSize;
	void *buffer;
	
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
		}
		++j;
	}
	
	setupIPC(countriesTableEntriesAmm / 4);
	
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			if(((*fixture)[j] = malloc(sizeof(void *) * 4)) == NULL){
				perror("Error de memoria");
				for (j -= 1; j >= 0 ; --j){
					free((*fixture)[j]);
				}
				return errno;
			}
			
				switch(((*pids)[headsAmm++] = fork())){
					case -1:
						perror("Error de fork");
						return errno;
						break;
					case 0:
						addClient();
						execv("./grouph.bin", NULL);
						break;
					default:					
						break;
				}
			++j;
		}
	}
	
	synchronize();
	
	for (i = 0 ; i < countriesTableEntriesAmm / 4 ; ++i){
		writeIPC((*pids)[i], &countriesTableEntriesAmm, sizeof(int));
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			serializeCountryStruct(&buffer, &bufferSize, countriesTable[j]);
			writeIPC((*pids)[i], &bufferSize, sizeof(int));
			writeIPC((*pids)[i], buffer, bufferSize);
			free(buffer);
		}
		serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
		writeIPC((*pids)[i], &bufferSize, sizeof(int));
		writeIPC((*pids)[i], buffer, bufferSize);
		free(buffer);
	}
	
	return 0;
}

int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture){
	void *buffer;
	int bufferSize, status, i, j, x, reqCountry, flag = FALSE, headsAmm, *finished;
	country **subFixture;
	
	headsAmm = countriesTableEntriesAmm / 4;
	
	if ((finished = calloc(1, sizeof(int) * countriesTableEntriesAmm / 4)) == NULL || (subFixture = malloc(sizeof(void *) * 4)) == NULL){
		perror("Error de memoria");
		free(finished);
		return errno;
	}
	
	for (j = 0 ; j < 4 ; ++j){
		if ((subFixture[j] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			for (x = 0 ; x < j ; ++x){
				free(subFixture[x]);
			}
			free(subFixture);
			return errno;
		}
	}
	
	
	while(selectIPC(2) > 0 && flag == FALSE){
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			if (getIPCStatus(pids[j]) && finished[j] == FALSE){
				readIPC(pids[j], &bufferSize, sizeof(int));
				buffer = malloc(sizeof(char) * bufferSize);
				readIPC(pids[j], buffer, bufferSize);
				reqCountry = unserializeInteger(buffer, bufferSize);
				free(buffer);
				
				printf("IPC : %d, reqCountry desserializado = %d \n", j, reqCountry);
				
				if (reqCountry < 0){
					for (x = 0 ; x < 4 ; ++x){
						readIPC(pids[j], &bufferSize, sizeof(int));
						buffer = malloc(sizeof(char) * bufferSize);
						readIPC(pids[j], buffer, bufferSize);
						unserializeCountryStruct(buffer, bufferSize, subFixture[x]);
						free(buffer);
						fixture[j][x] = malloc(sizeof(country));
						memcpy(fixture[j][x], subFixture[x], sizeof(country));
					}
					if (--headsAmm == 0){
						flag = TRUE;
						finished[j] = TRUE;
						break;
					}
					finished[j] = TRUE;
				}
				else{
					if (countriesTable[reqCountry]->used){
						serializeInteger(&buffer, &bufferSize, FALSE);
						printf("Repetido\n");
					}
					else{
						countriesTable[reqCountry]->used = TRUE;
						serializeInteger(&buffer, &bufferSize, TRUE);	
					}
					writeIPC(pids[j], &bufferSize, sizeof(int));
					writeIPC(pids[j], buffer, bufferSize);
					free(buffer);
				}
			}
		}
	}
	
	if (headsAmm != 0){
		perror("No solution found");
		printf("No se encontro una solucion al problema planteado");
		return -1;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm / 4 ; ++i){
		wait(&status);
		if (status != 0){
			perror("Error en un groupH");
			return status;
		}
	}
	
	return 0;
}