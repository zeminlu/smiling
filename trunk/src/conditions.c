#include <stdlib.h>
#include "../inc/types.h"

#define WEAK_GROUP 4

//------------------------sameContinent----------------------------
int * sameContinent(pais **paises,pais * cabezera){
	int * rta;
	int i, j;
	
	rta = malloc(sizeof(int)*31);
	if(rta == NULL){
		//	error memoria, insuficiente
	}
	for(i = 0, j= 0; i < 31; i++){
		if(cabezera->continente	== paises[i]->continente){
			rta[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	rta = realloc(rta, sizeof(int)*j);
	return rta;
}

//------------------------deathGroup----------------------------
int * deathGroup(pais **paises,pais * cabezera){
	int * rta;
	int i, j;
	
	rta = malloc(sizeof(int)*31);
	if(rta == NULL){
		//	error memoria, insuficiente
	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->peso > WEAK_GROUP){
			rta[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	rta = realloc(rta, sizeof(int)*j);
	return rta;
}

//------------------------champGroup----------------------------
int * champGroup(pais **paises,pais * cabezera){
	int * rta;
	int i, j;
	
	rta = malloc(sizeof(int)*31);
	if(rta == NULL){
		//	error memoria, insuficiente
	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->campeon > 0){
			rta[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	rta = realloc(rta, sizeof(int)*j);
	return rta;
}

//------------------------weakGroup----------------------------
int * weakGroup(pais **paises,pais * cabezera){
	int * rta;
	int i, j;
	
	rta = malloc(sizeof(int)*31);
	if(rta == NULL){
		//	error memoria, insuficiente
	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->peso <= WEAK_GROUP){
			rta[j] = i;
			j++;
		}
	}
	if(j == 0){
		return = NULL;
	}
	rta = realloc(rta, sizeof(int)*j);
	return rta;
}

