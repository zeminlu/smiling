#include "../inc/conditions.h"

pthread_mutex_t mutexIndex;
int *threadsRet;
int threadsIndex = 0;

/*Nombre: sameContinent
*
* 	Busca de la talba de paises, cual pertenece al mismo continente
* que el pais de cabezera, y almacena el int asociado , ala entrada
* en la tabla de paises. De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
*
*/

void * sameContinent(void * condi){
	int * countryInt;
	int i, j;
	int myIndex = 0;
	condPack * cond = condi;
	country **countries;
	country * head;
	set * ans;

	pthread_mutex_lock (&mutexIndex);
	myIndex = threadsIndex;
	++threadsIndex;
	pthread_mutex_unlock (&mutexIndex);
		
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto del mismo continente");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	for(i = 0, j = 0 ; i < (cond->maxCountries) ; ++i){
		if((head->continent != countries[i]->continent) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	
	ans->countriesAmm = j;
	if (j != 0){
		ans->country = realloc(countryInt, sizeof(int)*(j));
	}
	else{
		free(countryInt);
	}
	cond->sets[0] = ans;
	threadsRet[myIndex] = 1;
	
	return cond;
}

/*Nombre: deathGroup
*
* 	Busca de la talba de paises, cuales tiene mayor peso que WEAK_GROUP,
* y almacena el int asociado , ala entrada en la tabla de paises. 
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
*
*/
void * deathGroup(void * condi){
	int * countryInt;
	int i, j;
	int myIndex = 0;
	condPack * cond = condi;
	set * ans;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	pthread_mutex_lock (&mutexIndex);
	myIndex = threadsIndex;
	++threadsIndex;
	pthread_mutex_unlock (&mutexIndex);
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto de la muerte");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	for(i = 0, j= 0; i < (cond->maxCountries); ++i){
		if((countries[i]->weight > _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	
	ans->countriesAmm = j;
	if (j != 0){
		ans->country = realloc(countryInt, sizeof(int)*(j));
	}
	else{
		free(countryInt);
	}
	cond->sets[1] = ans;
	threadsRet[myIndex] = 1;
	return cond;
}

/*Nombre: champGroup
*
* 	Busca de la talba de paises, los que hallan salido campeon,
* y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve. 
*
*/
void * champGroup(void * condi){
	int * countryInt;
	int i, j;
	int myIndex = 0;
	set * ans;
	condPack * cond = condi;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	pthread_mutex_lock (&mutexIndex);
	myIndex = threadsIndex;
	++threadsIndex;
	pthread_mutex_unlock (&mutexIndex);
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto de paises de campeones");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	for(i = 0, j= 0; i < (cond->maxCountries) ; ++i){
		if((countries[i]->champ == 1) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if (j == 0){
		for (i = 0, j = 0 ; i < (cond->maxCountries); ++i){
			if((cond->countries[i])->used == FALSE){
				countryInt[j] = i;
				++j;
			}
		}	
	}

	ans->countriesAmm = j;
	if (j != 0){
		ans->country = realloc(countryInt, sizeof(int)*(j));
	}
	else{
		free(countryInt);
	}
	cond->sets[2] = ans;
	threadsRet[myIndex] = 1;
	return cond;
}

/*Nombre: weakGroup
*
* 	Busca de la talba de paises, los que tenga peso menos o igual que,
* WEAK_GROUP, y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve FALSE.
* Necesita liberar el puntero que se devuelve.
*
*/

void * weakGroup(void * condi){
	int * countryInt;
	int i, j;
	int myIndex = 0;
	condPack * cond = condi;
	set * ans;	
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	pthread_mutex_lock (&mutexIndex);
	myIndex = threadsIndex;
	++threadsIndex;
	pthread_mutex_unlock (&mutexIndex);
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises del weak Grpup");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[myIndex] = errno;
		return NULL;
	}
	for(i = 0, j= 0; (i < (cond->maxCountries)); ++i){
		if((countries[i]->weight <= _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}

	ans->countriesAmm = j;
	if (j != 0){
		ans->country = realloc(countryInt, sizeof(int)*(j));
	}
	else{
		free(countryInt);
	}
	cond->sets[3] = ans;
	threadsRet[myIndex] = 1;
	return cond;
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
	
	countryInt = malloc(sizeof(int) * cond->maxCountries);
	
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
	
	for(i = 0, j= 0; i < cond->maxCountries ; ++i){
		if(((cond->countries)[i])->used == FALSE){
			countryInt[j] = i;
			++j;
		}
	}

	ans->countriesAmm = j;

	if (j != 0){
		ans->country = realloc(countryInt, sizeof(int)*(j));
		ans->country = countryInt;
	}
	else{
		free(countryInt);
	}
	cond->sets[0] = ans;

	return TRUE;
}
/*Nombre: noCondition
*
* 	De los paises libres busca uno al azar, si no hay paises
* devuelve FALSE. Necesita liberar el puntero que se devuelve.
* en caso de error retorna errno.
*
*/	

void * noCondition(void * condi){
	
	int countryAns = 0;
	int * countryAux;
	int	amm, status;
	condPack * cond = condi;
	
	status = countryFree(cond);
	countryAux = (cond->sets[0])->country;
	amm = (cond->sets[0])->countriesAmm;
	srand(time(NULL));
	if(status != TRUE){	
		return NULL;
	}else if(amm > 1){
		countryAns = countryAux[((rand() % amm) + getpid()) % amm];
		countryAux[0] = countryAns;
	}else{
		countryAns = countryAux[0];
	}
	countryAux = realloc(countryAux, sizeof(int));
	(cond->sets[0])->countriesAmm = 1;
	(cond->sets[0])->country = countryAux;
	return cond;
}   