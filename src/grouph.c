#include <stdlib.h>
#include "../inc/group.h"

int main (void){
	country *data;
	country **countries;
	condPack *condArgs, **aux;
	subFixture *group;
	set *intersection;
	int (**conditions)(country **countries, country *head) = NULL;
	pthread_t *threads;
	int i = 0, j = 0, k = 0, x = 0, y = 0, index = 0, reqCountry;
	
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
	group = malloc(sizeof(subFixture));
	group->countries = malloc(sizeof(void *));
	group->countries[0] = data;
	++(group->countriesAmm);
	
	while (group->countriesAmm < 4){
		if (i == 0){
			noCondition(condArgs);
			reqCountry = condArgs->sets[0]->countries[0];
		}
		else if (i == 1){
			conditions[0](condArgs);
			reqCountry = condArgs->sets[0]->countries[rand() % condArgs->sets[0]->countriesAmm];
		}
		else{
			for (j = 0 ; j < i ; ++j){
				pthread_create(&threads[j], NULL, conditions[j], (void *)(condArgs));
			}
			for (j = 0 ; j < i ; ++j){
				pthread_join(threads[j], NULL);
			}

			aux = malloc(sizeof(void *));
			aux[0] = malloc(sizeof(subFixture));			
			aux[0]->countries = malloc(sizeof(int *) * ((condArgs->sets)[0])->countriesAmm);

			for (j = 0 ; j < ((condArgs->sets)[0])->countriesAmm ; ++j){
				aux[0]->countries[j] = condArgs->sets[0]->countries[j]; 
			}
			aux[0]->countriesAmm = j;

			for (j = 1 ; j < i ; ++j){	
				aux = realloc(aux, sizeof(void *) * (j + 1));
				aux[j] = malloc(sizeof(subFixture));
				aux[j]->countries = malloc(sizeof(int) * aux[0]->countriesAmm);
				y = 0; 
				while (x < aux[j - 1]->countriesAmm && k < ((condArgs->sets)[j])->countriesAmm) {  
					if (((aux[j - 1])->countries)[x] == (((condArgs->sets)[j])->countries)[k]) {  
						aux[j]->countries[y] = ((aux[j - 1])->countries)[x];
						x++;  
						k++;
						y++;  
					}  
					else if (((aux[j])->countries)[x] < (((condArgs->sets)[j + 1])->countries)[k]) {  
						x++;  
					}  
					else {  
						k++;  
					}  
				}
			}
			reqCountry = aux[j]->countries[rand() % aux[j]->countriesAmm];
		}
		
		/*
		Se pide a fifa reqCountry
		Si lo concede, se agrega a group y se incrementa la cantidad de paises en el mismo
		*/
	}
	/*
	Se envia a fifa el subfixture terminado, o error de no haberse logrado
	*/
}