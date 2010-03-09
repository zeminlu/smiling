#include <stdlib.h>
#include "../inc/group.h"

int main (void){
	country *data;
	country **countries;
	condPack *condArgs;
	conjunto *(**conditions)(pais **p, pais *c) = NULL;
	pthread_t *threads;
	int i = 0, j = 0, index = 0;
	
	condArgs = malloc(sizeof(condPack));
	condArgs->countries = countries;
	condArgs->head = data;
	condArgs->index = &index;
	
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
	
	condArgs->sets = malloc (sizeof(void *) * i);
	
	threads = malloc(sizeof(pthread_t) * i);
	
	for (j = 0 ; j < i ; ++j){
		pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));
		
	}
}