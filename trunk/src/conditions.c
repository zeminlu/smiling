#include "../inc/conditions.h"

int *threadsRet = NULL;

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
	condPack * cond = condi;
	country **countries;
	country * head;
	set * ans;
		
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto del mismo continente");
		threadsRet[0] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[0] = errno;
		return NULL;
	}
	for(i = 0, j = 0 ; i < (cond->maxCountries) ; ++i){
		if((head->continent != countries[i]->continent) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		threadsRet[0] = -1;
		return NULL;
	}
	
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[0] = ans;
	threadsRet[0] = 1;
	
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
	condPack * cond = condi;
	set * ans;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto de la muerte");
		threadsRet[1] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[1] = errno;
		return NULL;
	}
	for(i = 0, j= 0; i < (cond->maxCountries); ++i){
		if((countries[i]->weight > _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		threadsRet[1] = -1;
		return NULL;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[1] = ans;
	threadsRet[1] = 1;
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
	set * ans;
	condPack * cond = condi;
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		perror("Memoria insuficiente, para crear el conjunto de paises de campeones");
		threadsRet[2] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[2] = errno;
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
	if(j == 0){
		threadsRet[2] = -1;
		return NULL;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[2] = ans;
	threadsRet[2] = 1;
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
	condPack * cond = condi;
	set * ans;	
	country **countries;
	country * head;
	
	head = cond->head;
	countries = cond->countries;
	
	ans = malloc(sizeof(set));
	countryInt = malloc(sizeof(int)*(cond->maxCountries));
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises del weak Grpup");
		threadsRet[3] = errno;
		return NULL;
	}
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto countryInt");
		threadsRet[3] = errno;
		return NULL;
	}
	for(i = 0, j= 0; (i < (cond->maxCountries)); ++i){
		if((countries[i]->weight <= _WEAK_GROUP_) && ((cond->countries[i])->used == FALSE)){
			countryInt[j] = i;
			++j;
		}
	}
	if(j == 0){
		threadsRet[3] = -1;
		return NULL;
	}
	ans->countriesAmm = j;
	ans->country = realloc(countryInt, sizeof(int)*(j));
	cond->sets[3] = ans;
	threadsRet[3] = 1;
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
	int i , j, k = 0;	
	country **countries;
	set * ans;
		
	fprintf(stderr, "Parte %d\n", k++);
		
	countries = cond->countries;
	
	fprintf(stderr, "Parte %d\n", k++);
	
	ans = malloc(sizeof(set));
	
	fprintf(stderr, "Parte %d\n", k++);
	
	countryInt = malloc(sizeof(int) * cond->maxCountries);
	fprintf(stderr, "Parte %d\n", k++);
	
	if(ans == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises del weak Grpup");
		return errno;
	}	
	fprintf(stderr, "Parte %d\n", k++);
	
	if(countryInt == NULL){
		/*	error memoria, insuficiente*/
		perror("Memoria insuficiente, para crear el conjunto de paises libres");
		return errno;
	}
	fprintf(stderr, "Parte %d\n", k++);
	
	for(i = 0, j= 0; i < cond->maxCountries ; ++i){
		if(((cond->countries)[i])->used == FALSE){
			countryInt[j] = i;
			++j;
		}
	}
	fprintf(stderr, "Parte %d\n", k++);
	
	fprintf(stderr, "j =  %d\n", j);
	
	
	if(j == 0){
		free(countryInt);
		free(ans);
		return FALSE;
	}else{
		fprintf(stderr, "Parte %d\n", k++);
		
		ans->countriesAmm = j;
		fprintf(stderr, "Parte %d\n", k++);
		
		countryInt = realloc(countryInt, sizeof(int) * j);
		ans->country = countryInt;
		fprintf(stderr, "Parte %d\n", k++);
		
		cond->sets[0] = ans;
		fprintf(stderr, "Parte %d\n", k++);
		
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

void * noCondition(void * condi){
	
	int countryAns = 0;
	int * countryAux;
	int	amm, status, i = 0;
	condPack * cond = condi;
	
	fprintf(stderr, "Parte %d\n", i++);
	status = countryFree(cond);
	fprintf(stderr, "Parte %d\n", i++);
	countryAux = (cond->sets[0])->country;
	amm = (cond->sets[0])->countriesAmm;
	srand(time(NULL));
	if(status != TRUE){	
		return NULL;
	}else if(amm > 1){
		countryAns = countryAux[rand() % (amm)];
		countryAux[0] = countryAns;
	}else{
		countryAns = countryAux[0];
	}
	countryAux = realloc(countryAux, sizeof(int));
	(cond->sets[0])->countriesAmm = 1;
	(cond->sets[0])->country = countryAux;
	return cond;
}   