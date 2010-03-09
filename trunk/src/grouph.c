#include <stdlib.h>
#include "../inc/group.h"

int main (void){
	country *data;
	country **countries;
	condPack *condArgs;
	set *group;
	int (**conditions)(country **countries, country *head) = NULL;
	pthread_t *threads;
	int i = 0, j = 0, index = 0;
	
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
	
	condArgs = malloc(sizeof(condPack));
	condArgs->countries = countries;
	condArgs->head = data;
	condArgs->index = &index;
	condArgs->sets = malloc(sizeof(void *) * i);
	threads = malloc(sizeof(pthread_t) * i);
	group = malloc(sizeof(set));
	group->countries = malloc(sizeof(void *));
	group->countries[0] = data;
	++(group->countriesAmm);
	
	while (group->countriesAmm < 4){
		for (j = 0 ; j < i ; ++j){
			pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));

			/*
				Se verifica que hayan terminado todos los threads
				Se realiza la interseccion de los resultados de cada thread
				Se pide un pais de la interseccion a fifa
				Si fifa lo concede, se lo agrega a group->countries
			*/
		}
	}
}