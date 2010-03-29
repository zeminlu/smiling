#include "../inc/grouph.h"

extern pthread_mutex_t mutexIndex;
extern int *threadsRet;
extern int threadsIndex;

int main (void){
	
	country *data = NULL, **countriesTable = NULL;
	condPack *condArgs;
	subFixture *group = NULL;
	void *(**conditions)(void *condArgs) = NULL;
	int i = 0, index = 0, countriesTableEntriesAmm, status, condAmm;
	
	loadIPC();
		
	if ((countriesTableEntriesAmm = loadHeadAndCountriesTable(&countriesTable, &data)) < 0){
		fprintf(stderr, "Error en loadHeadandCountriesTable\n");
		sendErrorToParent();
		return countriesTableEntriesAmm;
	}
	
	if ((condAmm = checkConditions(data, &conditions)) < 0){
		fprintf(stderr, "Error en checkConditions\n");
		/*for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}*/
		varFree(2, data, countriesTable);
		sendErrorToParent();
		
		return condAmm;
	}
	
	if ((status = buildCondArgs(&condArgs, countriesTable, countriesTableEntriesAmm, data, condAmm, &index)) != 0){
		fprintf(stderr, "Error en buildcondargs\n");
		/*for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		*/
		varFree(3, data, countriesTable, conditions);
		sendErrorToParent();

		return status;
	} 
	
	if ((status = prepareGroup(&group, data)) != 0){
		fprintf(stderr, "Error en prepareGroup\n");
		/*for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		*/
		varFree(5, data, countriesTable, conditions, condArgs->sets, condArgs);
		sendErrorToParent();
		return status;
	}
		
	if ((status = buildSubfixture(&group, condAmm, condArgs, data, countriesTable, conditions)) != 0){
		fprintf(stderr, "Error en buildSubfixture\n");
		/*for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}*/
		for (i = 0 ; i < condAmm ; ++i){
			free(condArgs->sets[i]);
		}
		varFree(6, countriesTable, group->countries, group, conditions, condArgs->sets, condArgs);
		sendErrorToParent();
		return status;
	}
		
	status = sendSubfixture(group);
	
	/*for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
		free(countriesTable[i]);
	}*/
	
	for (i = 0 ; i < condAmm ; ++i){
		free(condArgs->sets[i]);
	}
	for (i = 0 ; i < 4; ++i){
		free (group->countries[i]);
	}
	varFree(6, countriesTable, conditions, condArgs->sets, condArgs, group->countries, group);
	
	return status;
}

void sendErrorToParent(){
	void *buffer;
	int bufferSize;
	int msg = -2, size = sizeof(int);
	
	serializeInteger(&buffer, &bufferSize, -2);
	if(writeIPC(getppid(), &size, sizeof(int)) == -1){
		fprintf(stderr,"Error de escritura");
		return;	
	}
	if(writeIPC(getppid(), &msg, sizeof(int)) == -1){
		fprintf(stderr,"Error de escritura");
		return;
	}
/*	free(buffer);*/
	
	return;
}

int loadHeadAndCountriesTable(country ***countriesTable, country **head){
	int countriesTableEntriesAmm, i, j, bufferSize;
	void *buffer;
	
	if(readIPC(getppid(), &countriesTableEntriesAmm, sizeof(int)) == -1){
		fprintf(stderr,"Error en la lectura");
		return -1;
	}
		
	if (((*countriesTable) = malloc(sizeof(void *) * countriesTableEntriesAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	for (i = 0 ; i < countriesTableEntriesAmm ; ++i){
		
		if(readIPC(getppid(), &bufferSize, sizeof(int))==-1){
			fprintf(stderr,"Error en la lectura");
			return -1;
		}
		
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*countriesTable)[i] = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			for(j = 0 ; j < i ; ++j){
				free((*countriesTable)[j]);
			}
			varFree(2, buffer, (*countriesTable));
			return errno;
		}
		
		if(readIPC(getppid(), buffer, bufferSize)==-1){
			fprintf(stderr,"Error en la lectura");
			return -1;
		}
		unserializeCountryStruct(buffer, bufferSize, (*countriesTable)[i]);
		free(buffer);		
	}
	
	if(readIPC(getppid(), &bufferSize, sizeof(int))==-1){
		fprintf(stderr,"Error en la lectura");
		return -1;
	}
	
	if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*head) = malloc(sizeof(country))) == NULL){
		perror("Error de memoria");
		for(j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free((*countriesTable)[j]);
		}
		varFree(2, (*countriesTable), buffer);
		return errno;
	}
	if(readIPC(getppid(), buffer, bufferSize)==-1){
		fprintf(stderr,"Error en la lectura");
		return -1;
	}
	unserializeCountryStruct(buffer, bufferSize, *head);
	free(buffer);
	
	return countriesTableEntriesAmm;
}

int checkConditions(country *data, void *(***conditions)(void *condArgs)){
	int i = 0;
	if (data->sameContinent){
		if (((*conditions) = realloc((*conditions), sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			return errno;
		}	
		(*conditions)[i - 1] = sameContinent;
	}
	if (data->weakGroup){
		if (((*conditions) = realloc((*conditions), sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			return errno;
		}
		(*conditions)[i - 1] = weakGroup;
	}
	if (data->champGroup){
		if (((*conditions) = realloc((*conditions), sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			return errno;
		}
		(*conditions)[i - 1] = champGroup;
	}
	if (data->deathGroup){
		if (((*conditions) = realloc((*conditions), sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			return errno;
		}
		(*conditions)[i - 1] = deathGroup;
	}
	return i;
}

int buildCondArgs(condPack **condArgs, country **countriesTable, int countriesTableEntriesAmm, country *data, int condAmm, int *index){
	if (((*condArgs) = malloc(sizeof(condPack))) == NULL || ((*condArgs)->sets = malloc(sizeof(void *) * 4)) == NULL){
		perror("Error de memoria");
		free(*condArgs);
		return errno;
	}
	
	(*condArgs)->countries = countriesTable;
	(*condArgs)->head = data;
	(*condArgs)->maxCountries = countriesTableEntriesAmm;
	
	return 0;
}

int prepareGroup(subFixture **group, country *data){
	if (((*group) = malloc(sizeof(subFixture))) == NULL || ((*group)->countries = malloc(sizeof(void *) * 4)) == NULL){
			perror("Error de memoria");
			free((*group));
			return errno;
	}
	(*group)->countries[0] = data;
	(*group)->countriesAmm = 1;
	
	return 0;
}

int buildSubfixture(subFixture **group, int condAmm, condPack *condArgs, country *data, country **countriesTable, void *(**conditions)(void *condArgs)){
	int i, j = 0, k, reqCountry, bufferSize, threadsFlag;
	pthread_t *threads = NULL;
	void *buffer, *ret;
	set *intersection = NULL;
		
	pthread_mutex_init(&mutexIndex, NULL);
	
	if ((threads = malloc(sizeof(pthread_t) * condAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
		
	srand(time(NULL));	
	i = condAmm;
	
	while ((*group)->countriesAmm < 4){
		for (k = 0 ; k < 4 ; ++k){
			condArgs->sets[k] = NULL;
		}
		threadsIndex = 0;
		threadsRet = calloc(1, sizeof(int) * 4);
		if (i == 0){
			noCondition(condArgs);

			if (condArgs->sets[0]->countriesAmm == 0){
				return -1;
			}
			reqCountry = condArgs->sets[0]->country[0];
		}
		else if (i == 1){
			ret = conditions[0](condArgs);
			if (ret == NULL || threadsRet[0] < 0 || condArgs->sets[0]->countriesAmm == 0){
				return -1;
			}
			reqCountry = condArgs->sets[0]->country[((rand() % condArgs->sets[0]->countriesAmm) + getpid()) % condArgs->sets[0]->countriesAmm];
		}
		else{
			for (j = 0 ; j < i ; ++j){
				pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));
			}
			threadsFlag = FALSE;
			while (!threadsFlag){
				threadsFlag = TRUE;
				for (j = 0 ; j < i ; ++j){
					if (threadsRet[j] == 0){
						threadsFlag = FALSE;
					}
					else if (threadsRet[j] < 0){
						free(threads);
						return threadsRet[j]; 
					}
				}
			}
			
			sortPointers(condArgs->sets);
			
			if (intersect(condAmm, condArgs, &intersection) == -1){
				return -1;
			}
						
			reqCountry = intersection->country[((rand() % intersection->countriesAmm) + getpid()) % intersection->countriesAmm];
			
			varFree(2, intersection->country, intersection);
		}

		for (j = 1 ; j < i ; ++j){
			free(condArgs->sets[j]->country);
		}
		free(condArgs->sets[0]->country);
		
		serializeInteger(&buffer, &bufferSize, reqCountry);
		
		if(writeIPC(getppid(), &bufferSize, sizeof(int))==-1){
			fprintf(stderr, "Error en la escritura");
			return -1;
		}
		if(writeIPC(getppid(), buffer, bufferSize) ==-1){
			fprintf(stderr, "Error en la escritura");
			return -1;
		}
		
		free(buffer);
		if(readIPC(getppid(), &bufferSize, sizeof(int)) == -1){
			fprintf(stderr, "Erro en la lectura");
			return -1;
		}
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL){
			perror("Error de memoria");
			free(threads);
			return errno;
		}
		fprintf(stderr, "Esperando respuesta en grouph = %s\n", data->name);
		if(readIPC(getppid(), buffer, bufferSize) == -1){
			fprintf(stderr, "Error en la lectura");
			return -1;
		}
		if (unserializeInteger(buffer, bufferSize) == FALSE){
			fprintf(stderr, "Respuesta en grouph = %s fue FALSE\n", data->name);
			countriesTable[reqCountry]->used = TRUE;
			
			free(buffer);
			continue;
		}
		fprintf(stderr, "Respuesta en grouph = %s fue TRUE\n", data->name);
		free(buffer);
		free(threadsRet);
		countriesTable[reqCountry]->used = TRUE;
		(*group)->countries[(*group)->countriesAmm] = countriesTable[reqCountry];
		++((*group)->countriesAmm);
	}
	pthread_mutex_destroy(&mutexIndex);
	free(threads);
	
	fprintf(stderr, "TERMINO GROUPH DE %s\n", data->name);
	
	return 0;
}

int intersect(int condAmm, condPack *condArgs, set **intersection){
	set **aux = NULL;
	int i, j = 0, k = 0, x = 0, y = 0;
	if ((aux = malloc(sizeof(void *) * condAmm)) == NULL ||
		(aux[0] = malloc(sizeof(set))) == NULL ||			
		(aux[0]->country = malloc(sizeof(int *) * ((condArgs->sets)[0])->countriesAmm)) == NULL){
			perror("Error de memoria");
			varFree(2, aux[0], aux);
			return errno;
		}
	
	for (j = 0 ; j < ((condArgs->sets)[0])->countriesAmm ; ++j){
		((aux[0])->country)[j] = (((condArgs->sets)[0])->country)[j]; 
	}
	(aux[0])->countriesAmm = j;

	for (j = 1 ; j < condAmm ; ++j){			
		if ((aux[j] = malloc(sizeof(set))) == NULL || ((aux[j])->country = malloc(sizeof(int) * (aux[j - 1])->countriesAmm)) == NULL){
				perror("Error de memoria");
				for(i = 0 ; i < j - 1 ; ++i){
					varFree(2, aux[i]->country, aux[i]);
				}
				varFree(2, aux[j], aux);
				return errno;
		}
		
		x = k = y = 0;
		while (x < aux[j - 1]->countriesAmm && k < ((condArgs->sets)[j])->countriesAmm) {  
			if (((aux[j - 1])->country)[x] == (((condArgs->sets)[j])->country)[k]) {  
				aux[j]->country[y] = ((aux[j - 1])->country)[x];
				x++;  
				k++;
				y++;  
			}  
			else if (((aux[j - 1])->country)[x] < (((condArgs->sets)[j])->country)[k]) {  
				x++;  
			}  
			else {  
				k++;  
			}  
		}
		aux[j]->countriesAmm = y;
	}

	*intersection = aux[j - 1];	
	for(i = 0 ; i < j - 2 ; ++i){
		varFree(2, aux[i]->country, aux[i]);
	}
	free(aux);
	
	return 0;
}

int sendSubfixture(subFixture *group){
	int i, bufferSize;
	void *buffer;
	
	serializeInteger(&buffer, &bufferSize, -1);
	if(writeIPC(getppid(), &bufferSize, sizeof(int)) == -1){
		fprintf(stderr, "Error en la escritura");
		return -1;
	}
	if(writeIPC(getppid(), buffer, bufferSize) == -1){
		fprintf(stderr, "Error en la escritura");
		return -1;
	}
	free(buffer);
	
	for (i = 0 ; i < 4 ; ++i){
		printf("Por mandar pais %d...\n", i);
		serializeCountryStruct(&buffer, &bufferSize, group->countries[i]);
		if(writeIPC(getppid(), &bufferSize, sizeof(int)) == -1){
			fprintf(stderr,"Error en la escritura");
			return -1;
		}
		if(writeIPC(getppid(), buffer, bufferSize) == -1){
			fprintf(stderr, "Erro en la escritura");
			return -1;
		}
		free(buffer);
		printf("...Mando pais %d\n", i);
	}
	
	return 0;
}

void sortPointers(set **sets){
	int i,j;
	
	for(j = 0; j < 3; j++){
		if(sets[j] == NULL){
			for(i = j + 1; i < 4; i++){
				if(sets[i] != NULL){
					sets[j] = sets[i];
					sets[i] = NULL;
				}
			}
		}
	}
	return;
}
