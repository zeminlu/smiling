#include "../inc/fifa.h"

int main (void){
	
	return fifoServer();
}

int fifoServer (){
	
	int i, j, bufferSize, status, **p = NULL, auxP1[2], auxP2[2], reqCountry = 0, headsAmm = 0, flag = FALSE, countriesTableEntriesAmm;
	pid_t *pids, actPid;
	void *buffer = NULL;
	fd_set master, set;
	country subFixture[4], **fixture = NULL;
	struct timeval timeout = {10, 0};
	country **countriesTable = NULL;
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
		unserializeStruct(buffer, bufferSize, countriesTable[i]);
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
					serializeHead(countriesTable[i], &buffer, &bufferSize);
					write(auxP2[1], &bufferSize, sizeof(int));
					write(auxP2[1], buffer, bufferSize);
					break;
			}
			++j;
			printf("Cabeza %d procesada\n", j);
		}
	}
	i = j;
	FD_ZERO(&master);
	for (j = 0 ; j < i ; ++j){
		FD_SET(p[j][0], &master);
	}
	printf("Entro al while del server\n");
	
	while(set = master, select(FD_SETSIZE, &set, NULL, NULL, &timeout) > 0 && flag == FALSE){
		for (j = 0 ; j < i ; ++j){
			printf("Por chequear pipe %d\n", j);
			if (FD_ISSET(p[j][0], &set)){
				read(p[j][0], &bufferSize, sizeof(int));
				read(p[j][0], buffer, bufferSize);
				printf("Por desserializar reqCountry\n");
				reqCountry = unserializeCountry(buffer, bufferSize);
				printf("reqCountry desserializado\n");
				if (reqCountry < 0){
						read(p[j][0], &bufferSize, sizeof(int));
						read(p[j][0], buffer, bufferSize);
						unserializeSubfixture(buffer, bufferSize, (country **)&subFixture);
						fixture[j] = subFixture;
						if (--headsAmm == 0){
							flag = TRUE;
							break;
						}
				}
				else{
					if (countriesTable[reqCountry]->used){
						serializeAnswer(FALSE, &buffer, &bufferSize);
					}
					else{
						countriesTable[reqCountry]->used = TRUE;
						serializeAnswer(TRUE, &buffer, &bufferSize);	
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
			printf("%s\n", fixture[i]->name);
		}
		free(fixture[j]);
	}
	free(fixture);
	
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
					serializeHead(countriesTable[i], &buffer, &bufferSize);
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
					serializeHead(countriesTable[i], &buffer, &bufferSize);
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
					serializeHead(countriesTable[i], &buffer, &bufferSize);
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

int unserializeTable(country **countriesTable, int amm, void *buffer, int bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)#", countriesTable, 45, 4);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int unserializeSubfixture(void *buffer, int bufferSize, country **subFixture){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)#", *subFixture, 45, 4);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int unserializeStruct(void *buffer, int bufferSize, country *subFixture){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", subFixture, 45);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int serializeAnswer(int status, void **buffer, int *bufferSize){
	tpl_node *tn;
	int ret = 0;
	
	tn = tpl_map("i", status);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
}

int unserializeCountry(void *buffer, int bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("i", &ret);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int serializeHead(country *head, void **buffer, int *bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", head, 45);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
}