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
		perror("Memoria insuficiente, para crear el conjunto del mismo continente");
		return errno;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		return errno;
	}
	for(i = 0, j= 0;i < _MAX_COUNTRIES_ ; ++i){
		if((head->continent != countries[i]->continent) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}

	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[*(cond->index)++] = ans;
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
		perror("Memoria insuficiente, para crear el conjunto de la muerte");
		return errno;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		return errno;
	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if((countries[i]->weight > _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[*(cond->index)++] = ans;
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
		perror("Memoria insuficiente, para crear el conjunto de paises de campeones");
		return errno;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		return errno;
	}
	for(i = 0, j= 0; i < _MAX_COUNTRIES_; ++i){
		if((countries[i]->champ > 0) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[*(cond->index)++] = ans;
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
		perror("Memoria insuficiente, para crear el conjunto de paises del weak Grpup");
		return errno;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		return errno;
	}
	for(i = 0, j= 0; (i < _MAX_COUNTRIES_); ++i){
		if((countries[i]->weight <= _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[*(cond->index)++] = ans;
	return TRUE;
}
/*Nombre: countryFree
*
* 	Busca todos los paises que estan sin usar, si no encuentra ninguno
* devuelve NULL. Necesita liberar el puntero que se devuelve.
* 
*/
int * countryFree( condPack * cond, int * amm){
	int * countryInt;
	int i , j;	
	country **countries;
		
	countries = cond->countries;
	
	countryInt = malloc(sizeof(int)*_MAX_COUNTRIES_);
	
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises libres");
	}
	for(i = 0, j= 0; (i < _MAX_COUNTRIES_); ++i){
		if((cond->countries[i])->used == FALSE){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return NULL;
	}else{
		*amm = j;
		return realloc(countryInt, sizeof(int)*(j));;
	}

}
	

int noCondition(condPack * cond){
	
	int * countryInt; 
	int *amm = NULL; 
	int *countryAns = NULL;
	set * ans; 
	
	srand(time(NULL));
	
	countryInt = countryFree(cond, amm);
	
	if(countryInt == NULL){
		return FALSE;
	}else if(*amm > 1){
		*countryAns = countryInt[rand() % (*amm)]; 
	}else{
		*countryAns = countryInt[0];
	}
	
	ans = malloc(sizeof(set));
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises libres");
		return errno;
	}
	
	ans->countriesAmm = 1;
	ans->country = countryAns;
	cond->sets[*(cond->index)++] = ans;
	return TRUE;

	
}