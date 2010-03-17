#include "../inc/fifa.h"

int main (void){
	int i, j, x, bufferSize, status, **ipcIDs = NULL, reqCountry = 0, headsAmm = 0, flag = FALSE, countriesTableEntriesAmm, ipcStatus, allocSize, finished[2];
	pid_t *pids, actPid;
	void *buffer = NULL, *master = NULL, *set = NULL;
	country **subFixture, ***fixture = NULL, **countriesTable = NULL;
	
	readIPC(_stdin_, &countriesTableEntriesAmm, sizeof(int));
		
	if ((countriesTable = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		
		readIPC(_stdin_, &bufferSize, sizeof(int));
		
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || (countriesTable[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			free(buffer);
			for(j = 0 ; j < i ; ++j){
				free(countriesTable[j]);
			}
			free(countriesTable);
			return errno;
		}
		
		readIPC(_stdin_, buffer, bufferSize);
		unserializeCountryStruct(buffer, bufferSize, countriesTable[i]);
		free(buffer);		
	}
	
	if ((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL || (ipcIDs = malloc(sizeof(int *) * (countriesTableEntriesAmm / 4))) == NULL || (fixture = malloc(sizeof(void *) * (countriesTableEntriesAmm / 4))) == NULL){
		perror("Error de memoria");
		free(pids);
		free(ipcIDs);
		return errno;
	}

	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
		}
		++j;
	}
	
	fixture = malloc(sizeof(void *) * j);
	
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			if((ipcIDs[j] = malloc(sizeof(int) * 2)) == NULL || (fixture[j] = malloc(sizeof(void *) * 4)) == NULL){
				perror("Error de memoria");
				for (j -= 1; j >= 0 ; --j){
					free(ipcIDs[j]);
				}
				free(ipcIDs);
				free(pids);
				free(fixture);
				return errno;
			}
			finished[j] = FALSE;
			ipcStatus = setupIPC(_FULL_DUPLEX_, ipcIDs[j], "./grouph.bin", &actPid);
			printf("ipcStatus de %d = %d y pid = %d\n", j, ipcStatus, actPid);
			
			pids[headsAmm++] = actPid;

			writeIPC(ipcIDs[j][1], &countriesTableEntriesAmm, sizeof(int));
			for (x = 0 ; x < countriesTableEntriesAmm ; ++x){
				serializeCountryStruct(&buffer, &bufferSize, countriesTable[x]);
				writeIPC(ipcIDs[j][1], &bufferSize, sizeof(int));
				writeIPC(ipcIDs[j][1], buffer, bufferSize);
				free(buffer);
			}
			serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
			writeIPC(ipcIDs[j][1], &bufferSize, sizeof(int));
			writeIPC(ipcIDs[j][1], buffer, bufferSize);
			free(buffer);
			++j;
		}
	}
		
	if ((subFixture = malloc(sizeof(void *) * 4)) == NULL){
		perror("Error de memoria");
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
	
	master = prepareIPC(ipcIDs, countriesTableEntriesAmm / 4, &allocSize);
	
	set = malloc(sizeof(char) * allocSize);
	
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
	
	for (i = 0 ; i < headsAmm ; ++i){
		wait(&status);
		if (status != 0){
			perror("Error en un groupH");
			return status;
		}
	}
	
	/*
	Guardar a archivo la solucion
	*/
	
	for (j = 0 ; j < i ; ++j){
		free(ipcIDs[j]);
	}
	free(ipcIDs);
	free(pids);
	
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
	
	free(set);
	free(master);
	
	closeIPC(_stdin_);
	closeIPC(_stdout_);
	
	return 0;
}
