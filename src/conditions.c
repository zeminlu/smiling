#include "../inc/conditions.h"


/*Nombre: sameContinent
*
* 	Busca de la talba de paises, cual pertenece al mismo continente
* que el pais de cabezera, y almacena el int asociado , ala entrada
* en la tabla de paises. De no encontrar ninguno devuelve NULL.
* 
*/

int sameContinent(condPack * cond){
	int * countryInt;
	int i, j;
	country **countries;
	country * head;
	set * ans;
	
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*_MAX_COUNTRIES_);
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if(head->continent != countries[i]->continent){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}

	ans->countriesAmm = j;
	ans->countries = realloc(countryInt, sizeof(int)*(j));
	condPack->sets[*(condPack->index)] = ans;
	return TRUE;
}

/*Nombre: deathGroup
*
* 	Busca de la talba de paises, cuales tiene mayor peso que WEAK_GROUP,
* y almacena el int asociado , ala entrada en la tabla de paises. 
* De no encontrar ninguno devuelve NULL.
* 
*/
int deathGroup(condPack * cond){
	int * countryInt;
	int i, j;
	set * ans;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*_MAX_COUNTRIES_);
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if(countries[i]->weight > WEAK_GROUP){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	condPack->sets[*(condPack->index)] = ans;
	return TRUE;
}

/*Nombre: champGroup
*
* 	Busca de la talba de paises, los que hallan salido campeon,
* y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve NULL.
* 
*/
int champGroup(condPack * cond){
	int * countryInt;
	int i, j;
	set * ans;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*_MAX_COUNTRIES_);
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if(countries[i]->champ > 0){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	condPack->sets[*(condPack->index)] = ans;
	return TRUE;
}

/*Nombre: weakGroup
*
* 	Busca de la talba de paises, los que tenga peso menos o igual que,
* WEAK_GROUP, y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve NULL.
* 
*/

int weakGroup(condPack * cond){
	int * countryInt;
	int i, j;
	set * ans;	
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*_MAX_COUNTRIES_);
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if(countries[i]->weight <= WEAK_GROUP){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	condPack->sets[*(condPack->index)] = ans;
	return TRUE;
}

