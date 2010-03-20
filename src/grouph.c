#include "../inc/grouph.h"

int main (void){
	
	country *data = NULL, **countriesTable = NULL;
	condPack *condArgs;
	subFixture *group = NULL;
	void *(**conditions)(void *condArgs) = NULL;
	int i = 0, index = 0, countriesTableEntriesAmm, status, condAmm;
	
	loadIPC();
	
	if ((countriesTableEntriesAmm = loadHeadAndCountriesTable(&countriesTable, &data)) < 0){
		return countriesTableEntriesAmm;
	}

	if ((condAmm = checkConditions(data, &conditions)) < 0){
		for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		free(countriesTable);
		return condAmm;
	}
	
	if ((status = buildCondArgs(&condArgs, countriesTable, countriesTableEntriesAmm, data, condAmm, &index)) != 0){
		for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		free(countriesTable);
		free (conditions);
		return status;
	} 
	
	if ((status = prepareGroup(&group, data)) != 0){
		for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		free(countriesTable);
		free (conditions);
		free(condArgs->sets);
		free(condArgs);
		return status;
	}
	if ((status = buildSubfixture(&group, condAmm, condArgs, data, countriesTable, conditions)) != 0){
		for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
			free(countriesTable[i]);
		}
		free(countriesTable);
		free (conditions);
		for (i = 0 ; i < condAmm ; ++i){
			free(condArgs->sets[i]);
		}
		free(condArgs->sets);
		free(condArgs);
		return status;
	}
	
	status = sendSubfixture(group);
	
/*	for(i = 0 ; i < countriesTableEntriesAmm ; ++i){
		free(countriesTable[i]);
	}
	free(data);
	free (conditions);
	for (i = 0 ; i < condAmm ; ++i){
		free(condArgs->sets[i]);
	}
	free(condArgs->sets);
	free(condArgs);
	*for (i = 0 ; i < 4; ++i){
		free (group->countries[i]);
	}
	free(group->countries);
	free(group);	
	close(_stdin_);
*/	
	closeIPC(getppid());
	
	return status;
}

int loadHeadAndCountriesTable(country ***countriesTable, country **head){
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
	
	readIPC(getppid(), &bufferSize, sizeof(int));
	
	if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL || ((*head) = malloc(sizeof(country))) == NULL){
		perror("Error de memoria");
		for(j = 0 ; j < countriesTableEntriesAmm ; ++j){
			free((*countriesTable)[j]);
		}
		free((*countriesTable));
		free(buffer);
		return errno;
	}
	readIPC(getppid(), buffer, bufferSize);
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
	if (((*condArgs) = malloc(sizeof(condPack))) == NULL || ((*condArgs)->sets = malloc(sizeof(void *) * condAmm)) == NULL){
		perror("Error de memoria");
		free(*condArgs);
		return errno;
	}
	
	(*condArgs)->countries = countriesTable;
	(*condArgs)->head = data;
	(*condArgs)->index = index;
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
	int i, j = 0, reqCountry, bufferSize;
	pthread_t *threads = NULL;
	void *buffer;
	set *intersection = NULL;
	
	if ((threads = malloc(sizeof(pthread_t) * condAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
		
	i = condAmm;
	
	while ((*group)->countriesAmm < 4){
		if (i == 0){
			noCondition(condArgs);
			reqCountry = condArgs->sets[0]->country[0];
		}
		else if (i == 1){
			conditions[0](condArgs);
			reqCountry = condArgs->sets[0]->country[rand() % condArgs->sets[0]->countriesAmm];
		}
		else{
			for (j = 0 ; j < i ; ++j){	
				pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));
			}
			for (j = 0 ; j < i ; ++j){
				pthread_join(threads[j], NULL);
			}
			
			intersect(condAmm, condArgs, &intersection);
						
			reqCountry = intersection->country[rand() % intersection->countriesAmm];
			free(intersection->country);
			free(intersection);
		}

		for (j = 0 ; j < i ; ++j){
			free(condArgs->sets[j]->country);
		}
		
		serializeInteger(&buffer, &bufferSize, reqCountry);
		writeIPC(getppid(), &bufferSize, sizeof(int));
		writeIPC(getppid(), buffer, bufferSize);
		free(buffer);
		readIPC(getppid(), &bufferSize, sizeof(int));
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL){
			perror("Error de memoria");
			free(threads);
			return errno;
		}
		readIPC(getppid(), buffer, bufferSize);
		if (unserializeInteger(buffer, bufferSize) == FALSE){
			countriesTable[reqCountry]->used = TRUE;
			free(buffer);
			continue;
		}
		free(buffer);
		countriesTable[reqCountry]->used = TRUE;
		(*group)->countries[(*group)->countriesAmm] = countriesTable[reqCountry];
		++((*group)->countriesAmm);
	}
	
	free(threads);
	
	return 0;
}

int intersect(int condAmm, condPack *condArgs, set **intersection){
	set **aux = NULL;
	int i, j = 0, k = 0, x = 0, y = 0;
	
	if ((aux = malloc(sizeof(void *) * condAmm)) == NULL ||
		(aux[0] = malloc(sizeof(set))) == NULL ||			
		(aux[0]->country = malloc(sizeof(int *) * ((condArgs->sets)[0])->countriesAmm)) == NULL){
			perror("Error de memoria");
			free(aux[0]);
			free(aux);
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
					free(aux[i]->country);
					free(aux[i]);
				}
				free(aux[j]);
				free(aux);
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
			else if (((aux[j])->country)[x] < (((condArgs->sets)[j + 1])->country)[k]) {  
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
		free(aux[i]->country);
		free(aux[i]);
	}
	free(aux);
	
	return 0;
}

int sendSubfixture(subFixture *group){
	int i, bufferSize;
	void *buffer;
	
	serializeInteger(&buffer, &bufferSize, -1);
	writeIPC(getppid(), &bufferSize, sizeof(int));
	writeIPC(getppid(), buffer, bufferSize);
	free(buffer);
	
	for (i = 0 ; i < 4 ; ++i){
		serializeCountryStruct(&buffer, &bufferSize, group->countries[i]);
		writeIPC(getppid(), &bufferSize, sizeof(int));
		writeIPC(getppid(), buffer, bufferSize);
		free(buffer);
	}
	
	return 0;
}