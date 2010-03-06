#include <stdlib.h>
#include "../inc/group.h"

int main (void){
	cabeza *data;
	pais **paises;
	condPack *condArgs;
	conjunto *(**conditions)(pais **p, pais *c) = NULL;
	pthread_t *threads;
	int i = 0, j = 0;
	
	condArgs = malloc(sizeof(condPack));
	condArgs->paises = paises;
	condArgs->head = data;
	
	if (data->sameContinent){
		conditions = realloc(conditions, sizeof(void *) * (++i));
		conditions[i - 1] = sameContinent;
	}
	if (data->weakGroup){
		conditions = realloc(conditions, sizeof(void *) * (++i));
		conditions[i - 1] = weakGroup;
	}
	if (data->champGroup){
		conditions = realloc(conditions, sizeof(void *) * (++i));
		conditions[i - 1] = champGroup;
	}
	if (data->deathGroup){
		conditions = realloc(conditions, sizeof(void *) * (++i));
		conditions[i - 1] = deathGroup;
	}
	
	threads = malloc(sizeof(pthread_t) * i);
	
	for (j = 0 ; j < i ; ++j){
		pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));
		
	}
}