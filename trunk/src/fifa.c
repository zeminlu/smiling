#include "../inc/fifa.h"

int main (void){
	int i, j, status, **ipcIDs = NULL, countriesTableEntriesAmm, allocSize;
	pid_t *pids;
	void *master = NULL, *set = NULL;
	country ***fixture = NULL, **countriesTable = NULL;
	
	if ((countriesTableEntriesAmm = loadCountriesTable(&countriesTable)) < 0){
		return countriesTableEntriesAmm;
	}
	
	if ((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL || (ipcIDs = malloc(sizeof(int *) * (countriesTableEntriesAmm / 4))) == NULL || (fixture = malloc(sizeof(void *) * countriesTableEntriesAmm / 4)) == NULL){
		perror("Error de memoria");
		free(pids);
		free(ipcIDs);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}
		free(countriesTable);
		return errno;
	}
	
	if ((status = startChildProcesses(&ipcIDs, countriesTable, countriesTableEntriesAmm, &fixture, &pids)) != 0 || (master = prepareIPC(ipcIDs, countriesTableEntriesAmm / 4, &allocSize)) == NULL || (set = malloc(sizeof(char) * allocSize)) == NULL){
		free(pids);
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			free(ipcIDs[j]);
		}
		free(ipcIDs);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(fixture[j]);
		}
		free(fixture);
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}
		free(countriesTable);
		free(master);
		return status;
	}
		
	if ((status = childsListener(set, master, allocSize, ipcIDs, countriesTable, countriesTableEntriesAmm, fixture)) != 0){
		free(pids);
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			free(ipcIDs[j]);
		}
		free(ipcIDs);
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
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		free(ipcIDs[j]);
	}
	free(ipcIDs);
	free(pids);
	free(set);
	free(master);
	for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
		free(countriesTable[j]);
	}
	free(countriesTable);
	
	closeIPC(_stdin_);
	closeIPC(_stdout_);
	
	return 0;
}

int loadCountriesTable(country ***countriesTable){
	int countriesTableEntriesAmm, i, j, bufferSize;
	void *buffer;
	
	readIPC(_stdin_, &countriesTableEntriesAmm, sizeof(int));
		
	if (((*countriesTable) = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		
		readIPC(_stdin_, &bufferSize, sizeof(int));
		
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*countriesTable)[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			free(buffer);
			for(j = 0 ; j < i ; ++j){
				free((*countriesTable)[j]);
			}
			free((*countriesTable));
			return errno;
		}
		
		readIPC(_stdin_, buffer, bufferSize);
		unserializeCountryStruct(buffer, bufferSize, (*countriesTable)[i]);
		free(buffer);		
	}
	
	return countriesTableEntriesAmm;	
}

int startChildProcesses(int ***ipcIDs, country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids){
	int i, j, x, actPid, headsAmm = 0, bufferSize;
	void *buffer;
	
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
		}
		++j;
	}
	
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			if(((*ipcIDs)[j] = malloc(sizeof(int) * 2)) == NULL || ((*fixture)[j] = malloc(sizeof(void *) * 4)) == NULL){
				perror("Error de memoria");
				for (j -= 1; j >= 0 ; --j){
					free((*ipcIDs)[j]);
					free((*fixture)[j]);
				}
				return errno;
			}
			
			setupIPC(_FULL_DUPLEX_, (*ipcIDs)[j], "./grouph.bin", &actPid);
			(*pids)[headsAmm++] = actPid;

			writeIPC((*ipcIDs)[j][1], &countriesTableEntriesAmm, sizeof(int));
			for (x = 0 ; x < countriesTableEntriesAmm ; ++x){
				serializeCountryStruct(&buffer, &bufferSize, countriesTable[x]);
				writeIPC((*ipcIDs)[j][1], &bufferSize, sizeof(int));
				writeIPC((*ipcIDs)[j][1], buffer, bufferSize);
				free(buffer);
			}
			serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
			writeIPC((*ipcIDs)[j][1], &bufferSize, sizeof(int));
			writeIPC((*ipcIDs)[j][1], buffer, bufferSize);
			free(buffer);
			++j;
		}
	}
	return 0;
}

int childsListener(void *set, void *master, int allocSize, int **ipcIDs, country **countriesTable, int countriesTableEntriesAmm, country ***fixture){
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
	
	while(memcpy(set, master, allocSize), selectIPC(set, 10) > 0 && flag == FALSE){
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			if (getIPCStatus(ipcIDs[j][0], set) && finished[j] == FALSE){
				readIPC(ipcIDs[j][0], &bufferSize, sizeof(int));
				buffer = malloc(sizeof(char) * bufferSize);
				readIPC(ipcIDs[j][0], buffer, bufferSize);
				reqCountry = unserializeInteger(buffer, bufferSize);
				free(buffer);
				
				printf("IPC : %d, reqCountry desserializado = %d \n", j, reqCountry);
				
				if (reqCountry < 0){
					for (x = 0 ; x < 4 ; ++x){
						readIPC(ipcIDs[j][0], &bufferSize, sizeof(int));
						buffer = malloc(sizeof(char) * bufferSize);
						readIPC(ipcIDs[j][0], buffer, bufferSize);
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
					writeIPC(ipcIDs[j][1], &bufferSize, sizeof(int));
					writeIPC(ipcIDs[j][1], buffer, bufferSize);
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