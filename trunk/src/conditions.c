#include "../inc/conditions.h"


/*Nombre: sameContinent
*
* 	Busca de la talba de paises, cual pertenece al mismo continente
* que el pais de cabezera, y almacena el int asociado , ala entrada
* en la tabla de paises. De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
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
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
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
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve. 
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
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
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
* devuelve FALSE. Necesita liberar el puntero que se devuelve.
* en caso de error devuelvo errno
*
*/
int countryFree( condPack * cond){
	int * countryInt;
	int i , j;	
	country **countries;
	set * ans;
		
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
		perror("Memoria insuficiente, para crear el conjunto de paises libres");
		return errno;
	}
	for(i = 0, j= 0; (i < _MAX_COUNTRIES_); ++i){
		if((cond->countries[i])->used == FALSE){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		return FALSE;
	}else{
		ans->countriesAmm = j;
		ans->country = realloc(countryInt, sizeof(int)*(j));
		cond->sets[*(cond->index)] = ans;
		return TRUE;
	}

}
/*Nombre: noCondition
*
* 	De los paises libres busca uno al azar, si no hay paises
* devuelve FALSE. Necesita liberar el puntero que se devuelve.
* en caso de error retorna errno.
*
*/	

int noCondition(condPack * cond){
	
	int * countryAns = NULL;
	int * countryAux;
	int	amm, status;
	
	status = countryFree(cond);
	
	countryAux = (cond->sets[*(cond->index)])->country;
	amm = (cond->sets[*(cond->index)])->countriesAmm;
	
	srand(time(NULL));
		
	if(status){
		return FALSE;
	}else if(amm > 1){
		*countryAns = countryAux[rand() % (amm)]; 
	}else{
		*countryAns = countryAux[0];
	}
	
	free(countryAux);
	Å
	(cond->sets[*(cond->index)])->countriesAmm = 1;
	(cond->sets[*(cond->index)++])->country = countryAns;
	return TRUE;

	
}