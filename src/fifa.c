#include "../inc/fifa.h"

int main (void){
	
	return fifoServer();
}

int fifoServer (){
	int i, j, x, bufferSize, status, **p = NULL, auxP1[2], auxP2[2], reqCountry = 0, headsAmm = 0, flag = FALSE, countriesTableEntriesAmm;
	pid_t *pids, actPid;
	void *buffer = NULL;
	fd_set master, set;
	country **subFixture, ***fixture = NULL, **countriesTable = NULL;	
	struct timeval timeout = {10, 0};
	
	read(_stdin_, &countriesTableEntriesAmm, sizeof(int));
	if ((countriesTable = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		
		read(_stdin_, &bufferSize, sizeof(int));
		
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL ||
			(countriesTable[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			free(buffer);
			for(j = 0 ; j < i ; ++j){
				free(countriesTable[j]);
			}
			free(countriesTable);
			return errno;
		}
		
		read(_stdin_, buffer, bufferSize);
		unserializeCountryStruct(buffer, bufferSize, countriesTable[i]);
		free(buffer);		
	}
	
	if ((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL ||
		(p = malloc(sizeof(int *) * (countriesTableEntriesAmm / 4))) == NULL ||
		(fixture = malloc(sizeof(void *) * (countriesTableEntriesAmm / 4))) == NULL){
		perror("Error de memoria");
		free(pids);
		free(p);
		return errno;
	}
	for (j = 0, i = 0 ; i < countriesTableEntriesAmm ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
			
			if((p[j] = malloc(sizeof(int) * 2)) == NULL){
				perror("Error de memoria");
				for (j -= 1; j >= 0 ; --j){
					free(p[j]);
				}
				free(p);
				free(pids);
				free(fixture);
				return errno;
			}
			
			pipe(auxP1);
			pipe(auxP2);
			p[j][0] = auxP1[0];
			p[j][1] = auxP2[1];
			switch((actPid = fork())){
				case -1:
					perror("Error de fork");
					for ( ; j >= 0 ; --j){
						free(p[j]);
					}
					free(p);
					free(pids);
					free(fixture);
					return errno;
					break;
				case 0:
					close(auxP1[0]);
					close(auxP2[1]);
					dup2(auxP1[1], 1);
					dup2(auxP2[0], 0);
					execv("./grouph.bin", NULL);
					break;
				default:					
					pids[headsAmm++] = actPid;
					close(auxP1[1]);
					close(auxP2[0]);
					
					write(auxP2[1], &countriesTableEntriesAmm, sizeof(int));
					for (x = 0 ; x < countriesTableEntriesAmm ; ++x){
						serializeCountryStruct(&buffer, &bufferSize, countriesTable[x]);
						write(auxP2[1], &bufferSize, sizeof(int));
						write(auxP2[1], buffer, bufferSize);
						free(buffer);	
					}
					serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
					write(auxP2[1], &bufferSize, sizeof(int));
					write(auxP2[1], buffer, bufferSize);
					break;
			}
			++j;
		}
	}
	i = j;
	FD_ZERO(&master);
	for (j = 0 ; j < i ; ++j){
		FD_SET(p[j][0], &master);
	}
	printf("Entro al while del server\n");
	
	
	if ((subFixture = malloc(sizeof(void *) * countriesTableEntriesAmm / 4)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		if ((subFixture[j] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			for (x = 0 ; x < j ; ++x){
				free(subFixture[x]);
			}
			free(subFixture);
			return errno;
		}
	}
	
	while(set = master, select(FD_SETSIZE, &set, NULL, NULL, &timeout) > 0 && flag == FALSE){
		for (j = 0 ; j < i ; ++j){
			if (FD_ISSET(p[j][0], &set)){
				read(p[j][0], &bufferSize, sizeof(int));
				read(p[j][0], buffer, bufferSize);
				
				reqCountry = unserializeInteger(buffer, bufferSize);
				printf("reqCountry desserializado = %d \n", reqCountry);

				if (reqCountry < 0){
					/*for (x = 0 ; x < 4 ; ++x){
						read(p[j][0], &bufferSize, sizeof(int));
						read(p[j][0], buffer, bufferSize);
						fprintf(stderr, "FIFA: Por desserializar subfixture\n");
						unserializeCountryStruct(buffer, bufferSize, subFixture[x]);
						fprintf(stderr, "FIFA: desserializo el subfixture\n");
					}
					fixture[j] = subFixture;
					*/if (--headsAmm == 0){
						flag = TRUE;
						break;
					}
				}
				else{
					if (countriesTable[reqCountry]->used){
						serializeInteger(&buffer, &bufferSize, FALSE);
					}
					else{
						countriesTable[reqCountry]->used = TRUE;
						serializeInteger(&buffer, &bufferSize, TRUE);	
					}
					write(p[j][1], &bufferSize, sizeof(int));
					write(p[j][1], buffer, bufferSize);
				}
			}
		}
	}
	
	printf("Salio del while del server\n");
	
	
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
	
		printf("TODO BIEN!!\n");
	
	for (j = 0 ; j < i - 1 ; ++j){
		free(p[j]);
	}
	free(p);
	free(pids);
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		for(i = 0; i < 4; ++i){
			/*printf("%s\n", (fixture[j][i])->name);*/
			free(fixture[j][i]);
		}
		free(fixture[j]);
	}
	for (j = 0 ; j < countriesTableEntriesAmm / 4 ; ++j){
		free(subFixture[i]);
	}
	free(subFixture);
	free(fixture);
	close(_stdin_);
	close(_stdout_);
	return 0;
}

int msqServer(){
	int i, headsAmm = 0, bufferSize, status, countriesTableEntriesAmm = 0;
	pid_t *pids, actPid;
	void *buffer = NULL;
	country **countriesTable = NULL;
	
	if((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
			switch((actPid = fork())){
				case -1:
					perror("Error de fork");
					return errno;
					break;
				case 0:
					execv("./grouph.bin", NULL);
					serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
					/*Se le pasa a grouph el buffer que contiene al head actual*/
					break;
				default:
					pids[headsAmm++] = actPid;
					break;
			}
		}
	}
	
	/*
	* Aca viene el listener que administra las peticiones de countries de los grouph hijos.
	* A su vez, se debe al final, invocar los waits correspondientes y evaluarlos.
	*
	*/
	
	for (i = 0 ; i < headsAmm ; ++i){
		wait(&status);
		/*Verificar status*/
	}
	
	return 0;
}

int shmServer(){
	int i, headsAmm = 0, bufferSize, status, countriesTableEntriesAmm = 0;
	pid_t *pids, actPid;
	void *buffer = NULL;
	country **countriesTable = NULL;
	
	if((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
			switch((actPid = fork())){
				case -1:
					perror("Error de fork");
					return errno;
					break;
				case 0:
					execv("./grouph.bin", NULL);
					serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
					/*Se le pasa a grouph el buffer que contiene al head actual*/
					break;
				default:
					pids[headsAmm++] = actPid;
					break;
			}
		}
	}
	
	/*
	* Aca viene el listener que administra las peticiones de countries de los grouph hijos.
	* A su vez, se debe al final, invocar los waits correspondientes y evaluarlos.
	*
	*/
	
	for (i = 0 ; i < headsAmm ; ++i){
		wait(&status);
		/*Verificar status*/
	}
	
	return 0;
}

int sckServer(){
	int i, headsAmm = 0, bufferSize, status, countriesTableEntriesAmm = 0;
	pid_t *pids, actPid;
	void *buffer = NULL;
	country **countriesTable = NULL;
	
	if((pids = malloc(sizeof(pid_t) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		if ((countriesTable[i])->isHead){
			countriesTable[i]->used = TRUE;
			switch((actPid = fork())){
				case -1:
					perror("Error de fork");
					return errno;
					break;
				case 0:
					execv("./grouph.bin", NULL);
					serializeCountryStruct(&buffer, &bufferSize, countriesTable[i]);
					/*Se le pasa a grouph el buffer que contiene al head actual*/
					break;
				default:
					pids[headsAmm++] = actPid;
					break;
			}
		}
	}
	
	/*
	* Aca viene el listener que administra las peticiones de countries de los grouph hijos.
	* A su vez, se debe al final, invocar los waits correspondientes y evaluarlos.
	*
	*/
	
	for (i = 0 ; i < headsAmm ; ++i){
		wait(&status);
		/*Verificar status*/
	}
	
	return 0;
}