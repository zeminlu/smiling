#include "../inc/grouph.h"

int main (void){
	
	country *data = NULL;
	condPack *condArgs;
	set **aux = NULL;
	subFixture *group = NULL;
	void *buffer = NULL;
	void *(**conditions)(void *condArgs) = NULL;
	pthread_t *threads = NULL;
	int i = 0, j = 0, k = 0, x = 0, y = 0, index = 0, reqCountry, freePt = 0, bufferSize = 0;
	country **countriesTable = NULL;
	
	
	read(_stdin_, &bufferSize, sizeof(int));
	if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL ||
		(data = malloc(sizeof(country))) == NULL){
		perror("Error de memoria");
		free(buffer);
		return errno;
	}
	read(_stdin_, buffer, bufferSize);
	
	unserializeHead(data, buffer, bufferSize);
	
	free(buffer);
	
	if (data->sameContinent){
		if ((conditions = realloc(conditions, sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(data);
			return errno;
		}	
		conditions[i - 1] = sameContinent;
	}
	if (data->weakGroup){
		if ((conditions = realloc(conditions, sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			free(data);
			return errno;
		}
		conditions[i - 1] = weakGroup;
	}
	if (data->champGroup){
		if ((conditions = realloc(conditions, sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			free(data);
			return errno;
		}
		conditions[i - 1] = champGroup;
	}
	if (data->deathGroup){
		if ((conditions = realloc(conditions, sizeof(void *) * (++i))) == NULL){
			perror("Error de memoria");
			free(conditions);
			free(data);
			return errno;
		}
		conditions[i - 1] = deathGroup;
	}
	
	if ((condArgs = malloc(sizeof(condPack))) == NULL){
		perror("Error de memoria");
		free(conditions);
		free(data);
		return errno;
	}
	
	condArgs->countries = countriesTable;
	condArgs->head = data;
	condArgs->index = &index;
	if ((condArgs->sets = malloc(sizeof(void *) * i)) == NULL ||
		(threads = malloc(sizeof(pthread_t) * i)) == NULL ||
		(group = malloc(sizeof(subFixture))) == NULL ||
		(group->countries = malloc(sizeof(void *) * 4)) == NULL){
			free(condArgs->sets);
			free(threads);
			free(group);
			free(conditions);
			free(data);
			perror("Error de memoria");
			return errno;
	}
	group->countries[0] = data;
	++(group->countriesAmm);
	
	while (group->countriesAmm < 4){
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

			if ((aux = malloc(sizeof(void *))) == NULL ||
				(aux[0] = malloc(sizeof(subFixture))) == NULL ||			
				(aux[0]->country = malloc(sizeof(int *) * ((condArgs->sets)[0])->countriesAmm)) == NULL){
					free(aux[0]);
					free(aux);
					free(conditions);
					free(threads);
					free(group->countries);
					free(group);
					for(j = 0 ; j < i ; ++j){
						free(condArgs->sets[j]);
					}
					free(condArgs->sets);
					free(condArgs);
					free(data);
					perror("Error de memoria");
					return errno;
				}

			for (j = 0 ; j < ((condArgs->sets)[0])->countriesAmm ; ++j){
				((aux[0])->country)[j] = (((condArgs->sets)[0])->country)[j]; 
			}
			(aux[0])->countriesAmm = j;

			for (j = 1 ; j < i ; ++j){	
				if ((aux = realloc(aux, sizeof(void *) * (j + 1))) == NULL||
					(aux[j] = malloc(sizeof(subFixture))) == NULL ||
					(aux[j]->country = malloc(sizeof(int) * aux[0]->countriesAmm)) == NULL){
						for(freePt = 0 ; freePt <= j ; ++freePt){
							free(aux[freePt]->country);
							free(aux[freePt]);
						}
						free(aux);
						free(data);
						free(conditions);
						free(threads);
						free(group->countries);
						free(group);
						for(j = 0 ; j < i ; ++j){
							free(condArgs->sets[j]);
						}
						free(condArgs->sets);
						free(condArgs);
						perror("Error de memoria");
						return errno;
					}
				y = 0; 
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
			}
			reqCountry = aux[j]->country[rand() % aux[j]->countriesAmm];
		}
		
		serializeCountry(reqCountry, &buffer, &bufferSize);
		
		write(_stdout_, buffer, bufferSize);
		read(_stdin_, &bufferSize, sizeof(int));
		free(buffer);
		if ((buffer = malloc(sizeof(char) * bufferSize)) == NULL){
			perror("Error de memoria");
			for (j = 0 ; j < i ; ++j){
				free(condArgs->sets[j]->country);
				free(condArgs->sets[j]);
			}
			for (x = 0 ; x < i ; ++ x){
				free(aux[x]->country);
				free(aux[x]);
			}
			free(condArgs->sets);
			free(condArgs);
			free(conditions);
			free(threads);
			free(data);
			return errno;
		}
		read(_stdin_, buffer, bufferSize);
		if (unserializeAnswer(buffer, bufferSize) == FALSE){
			continue;
		}
		
		group->countries[(group->countriesAmm)++] = countriesTable[reqCountry];
		
		free(buffer);
		
		for (j = 0 ; j < i ; ++j){
			free(condArgs->sets[j]->country);
			free(condArgs->sets[j]);
		}
		for (x = 0 ; x < i ; ++ x){
			free(aux[x]->country);
			free(aux[x]);
		}
	}
	
	serializeCountry(-1, &buffer, &bufferSize);
	write(_stdout_, buffer, bufferSize);
	free(buffer);
	serializeStruct(group, &buffer, &bufferSize);
	write(_stdout_, buffer, bufferSize);
	
	free(buffer);
	free(condArgs->sets);
	free(condArgs);
	free(conditions);
	free(threads);
	return 0;
}

int unserializeHead(country *head, void *buffer, int bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", head, 45);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int unserializeAnswer(void *buffer, int bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("i", &ret);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int serializeCountry(int country, void **buffer, int *bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("i", &country);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
}

int serializeStruct(subFixture *group, void **buffer, int *bufferSize){
	tpl_node *tn;
	int ret, i;
	country countArr[4];
	
	for (i = 0 ; i < 4 ; ++i){
		countArr[i] = *(group->countries[i]);
	}
	
	tn = tpl_map("S(c#iiiiiiiii)#", countArr, 45, 4);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
	
}