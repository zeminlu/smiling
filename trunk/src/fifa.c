#include "../inc/fifa.h"

int main (void){
	int i, j, status, countriesTableEntriesAmm;
	pid_t *pids;
	country ***fixture = NULL, **countriesTable = NULL;
	
	if (loadIPC() != 0){
		return -1;
	}
	
	printf("Pre loadcountriestable\n");
	if ((countriesTableEntriesAmm = loadCountriesTable(&countriesTable)) < 0){
		fprintf(stderr, "Error en loadCountriesTable\n");
		if (writeIPC(getppid(), &countriesTableEntriesAmm, sizeof(int)) == -1){
			fprintf(stderr, "Error mandando señal de error a parallel\n");
		}
		closeIPC(getpid());
		return countriesTableEntriesAmm;
	}
	
	if (writeIPC(getppid(), &countriesTableEntriesAmm, sizeof(int)) == -1){
		fprintf(stderr, "Error mandando señal de OK a parallel\n");
		free(countriesTable);
		closeIPC(getpid());
		return -1;
	}
	
	/*printf("<-----------------------------------------LLEGO LO SIGUIENTE A FIFA-----------------------------------------\n");
	for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
		printf("Pais: %s - Continente: %d - Campeon: %d - Peso: %d - Same: %d - Death: %d - ChampG: %d - Weak: %d - Cabeza de Serie: %d\n", 
		countriesTable[j]->name, countriesTable[j]->continent, countriesTable[j]->champ, countriesTable[j]->weight, countriesTable[j]->sameContinent, countriesTable[j]->deathGroup, countriesTable[j]->champGroup, countriesTable[j]->weakGroup, countriesTable[j]->isHead);
	}
	printf("<-----------------------------------------------------------------------------------------------------------\n>");
	*/
	closeIPC(getpid());
	
	if ((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL || (fixture = malloc(sizeof(void *) * countriesTableEntriesAmm / 4)) == NULL){
		perror("Error de memoria");
		/*for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}*/
		varFree(2, pids, countriesTable);
		
		return errno;
	}
	printf("Pre startChildProcesses\n");
	if ((status = startChildProcesses(countriesTable, countriesTableEntriesAmm, &fixture, &pids)) != 0){
		fprintf(stderr, "Error en startChildprocesses\n");
		/*for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}*/
		varFree(3, countriesTable, pids, fixture);
		
		return status;
	}
	printf("Pre childsListener\n");	
	if ((status = childsListener(pids, countriesTable, countriesTableEntriesAmm, fixture)) != 0){
		fprintf(stderr, "Error en childslistener\n");
		if (status == -2){
			printf("No se encontro una solucion al problema planteado, ver salida de error\n");
		}
		else{
			printf("GroupH retorno con error\n");
		}
		/*for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free(countriesTable[j]);
		}*/
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			free(fixture[j]);
		}
		varFree(3, countriesTable, fixture, pids);
		
		return status;
	}
	finalizeIPC();
	/*
	Guardar a archivo la solucion
	*/
	
	printf("\n");
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		printf("Grupo %d:\n", j + 1);
		for(i = 0; i < 4; ++i){
			printf("%s\n", (fixture[j][i])->name);
			/*free(fixture[j][i]);*/
		}
		printf("\n");
		/*free(fixture[j]);*/
	}
	
	/*for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
		free(countriesTable[j]);
	}*/
	varFree(3, countriesTable, fixture, pids);
	
	return 0;
}

int loadCountriesTable(country ***countriesTable){
	int countriesTableEntriesAmm, i, j, bufferSize;
	void *buffer;
	
	if (readIPC(getppid(), &countriesTableEntriesAmm, sizeof(int)) != 0){
		return -1;
	}
	if (((*countriesTable) = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		if (readIPC(getppid(), &bufferSize, sizeof(int)) != 0){
			return -1;
		}
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*countriesTable)[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			for(j = 0 ; j < i ; ++j){
				free((*countriesTable)[j]);
			}
			varFree(2, (*countriesTable), buffer);
			
			return errno;
		}
		
		if (readIPC(getppid(), buffer, bufferSize) != 0){
			free(buffer);
			return -1;
		}
		unserializeCountryStruct(buffer, bufferSize, (*countriesTable)[i]);
		free(buffer);		
	}
	return countriesTableEntriesAmm;	
}

int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids){
	int i, j, headsAmm = 0, bufferSize, status;
	void *buffer;
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm && j < countriesTableEntriesAmm / 4 ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
		}
		++j;
	}
	
	if ((status = setupIPC(countriesTableEntriesAmm / 4)) < 0){
		return status;
	}
	
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
	if ((status = synchronize()) < 0){
		return status;
	}
	for (i = 0 ; i < countriesTableEntriesAmm / 4 ; ++i){
		if (writeIPC((*pids)[i], &countriesTableEntriesAmm, sizeof(int)) != 0){
			return -1;
		}
		for (j = 0 ; j < countriesTableEntriesAmm ; ++j){
			serializeCountryStruct(&buffer, &bufferSize, countriesTable[j]);
			if (writeIPC((*pids)[i], &bufferSize, sizeof(int)) != 0 || writeIPC((*pids)[i], buffer, bufferSize) != 0){
				free(buffer);
				return -1;
			}
			free(buffer);
		}
		serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
		if (writeIPC((*pids)[i], &bufferSize, sizeof(int)) != 0 || writeIPC((*pids)[i], buffer, bufferSize) != 0){
			free(buffer);
			return -1;
		}
		
		free(buffer);
	}
	return 0;
}

int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture){
	void *buffer;
	int bufferSize, status, i, j, x, k, reqCountry, flag = FALSE, headsAmm, *finished, finishedAmm = 0, errorStat = FALSE, response;
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
	
	while(flag == FALSE && selectIPC(2) > 0 ){
		for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
			if (finished[j] == FALSE && getIPCStatus(pids[j])){

				if (readIPC(pids[j], &reqCountry, sizeof(int)) != 0){
					return -1;
				}
				
				printf("IPC : %d, reqCountry desserializado = %d \n", j, reqCountry);
				
				if (reqCountry == -1){
					for (x = 0 ; x < 4 ; ++x){
						if (readIPC(pids[j], &bufferSize, sizeof(int)) != 0){
							return -1;
						}
						if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || (fixture[j][x] = malloc(sizeof(country))) == NULL){
							perror("Error de memoria en childsListener allocando buffer y fixture[][]");
							for (i = 0 ; i < j - 1 ; ++i){
								for (k = 0 ; k < 4 ; ++k){
									free(fixture[i][k]);
								}
							}
							for (i = 0 ; i < x ; ++i){
								free(fixture[j][i]);
							}
							for (i = 0 ; i < 4 ; ++i){
								free(subFixture[i]);
							}
							varFree(3, buffer, finished, subFixture);
							return errno;
						}
						if (readIPC(pids[j], buffer, bufferSize) != 0){
							return -1;
						}
						unserializeCountryStruct(buffer, bufferSize, subFixture[x]);
						free(buffer);
						memcpy(fixture[j][x], subFixture[x], sizeof(country));
					}
					finished[j] = TRUE;
					finishedAmm++;
					if (--headsAmm == 0){
						flag = TRUE;
						break;
					}
				}
				else if (reqCountry == -2){
					finished[j] = TRUE;
					++finishedAmm;
					flag = TRUE;
					break;
				}
				else{
					if (countriesTable[reqCountry]->used){
						response = FALSE;
					}
					else{
						countriesTable[reqCountry]->used = TRUE;
						response = TRUE;	
					}
					if (writeIPC(pids[j], &response, sizeof(int)) != 0){
						return -1;
					}
				}
			}
		}
	}
	
	free(finished);
	for (x = 0 ; x < 4 ; ++x){
		free(subFixture[x]);
	}
	free(subFixture);
	
	for (i = 0 ; i < finishedAmm ; ++i){
		wait(&status);
		if (status < 0 && status != -2){
			errorStat = TRUE;
			fprintf(stderr, "Error en un groupH: %d\n", status);
			break;
		}
	}
	
	if (finishedAmm < headsAmm){
		fprintf(stderr, "Error en un groupH: %d\n", status);
		return -1;
	}
	
	if (errorStat){
		return status;
	}
	
	if (headsAmm != 0){
		return -2;
	}
	
	return 0;
}