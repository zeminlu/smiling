#include <stdlib.h>
#include "../inc/types.h"

#define WEAK_GROUP 4

/*Nombre: sameContinent
*
* 	Busca de la talba de paises, cual pertenece al mismo continente
* que el pais de cabezera, y almacena el int asociado , ala entrada
* en la tabla de paises. De no encontrar ninguno devuelve NULL.
* 
*/

size_t * sameContinent(pais **paises,pais * cabezera){
	size_t * paisesInt;
	size_t i, j;
	conjunto * rta;
	
	rta = malloc(sizeof(conjunto));
	paisesInt = malloc(sizeof(int)*31);
	rta->paises = paisesInt;
	
	if(rta == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < 31; i++){
		if(cabezera->continente	== paises[i]->continente){
			paisesInt[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	paisesInt = realloc(paisesInt, sizeof(int)*j);
	return rta;
}

/*Nombre: deathGroup
*
* 	Busca de la talba de paises, cuales tiene mayor peso que WEAK_GROUP,
* y almacena el int asociado , ala entrada en la tabla de paises. 
* De no encontrar ninguno devuelve NULL.
* 
*/
size_t * deathGroup(pais **paises,pais * cabezera){
	size_t * paisesInt;
	size_t i, j;
	conjunto * rta;
	
	rta = malloc(sizeof(conjunto));
	paises = malloc(sizeof(int)*31);
	rta->paises = paisesInt;
	
	if(rta == NULL){
		/*	error memoria, insuficiente*/	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->peso > WEAK_GROUP){
			paisesInt[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	paisesInt = realloc(paisesInt, sizeof(int)*j);
	return rta;
}

/*Nombre: champGroup
*
* 	Busca de la talba de paises, los que hallan salido campeon,
* y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve NULL.
* 
*/
size_t * champGroup(pais **paises,pais * cabezera){
	size_t * paisesInt;
	size_t i, j;
	conjunto * rta;
	
	rta = malloc(sizeof(conjunto));
	paises = malloc(sizeof(int)*31);
	rta->paises = paisesInt;
	
	if(rta == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->campeon > 0){
			paisesInt[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	paisesInt = realloc(paisesInt, sizeof(int)*j);
	return rta;
}

/*Nombre: weakGroup
*
* 	Busca de la talba de paises, los que tenga peso menos o igual que,
* WEAK_GROUP, y almacena el int asociado , ala entrada en la tabla de paises.
* De no encontrar ninguno devuelve NULL.
* 
*/

size_t * weakGroup(pais **paises,pais * cabezera){
	size_t * paisesInt;
	size_t i, j;
	conjunto * rta;
	
	rta = malloc(sizeof(conjunto));
	paises = malloc(sizeof(int)*31);
	rta->paises = paisesInt;
	
	if(rta == NULL){
		/*	error memoria, insuficiente*/
	}
	for(i = 0, j= 0; i < 31; i++){
		if(paises[i]->peso <= WEAK_GROUP){
			paisesInt[j] = i;
			j++;
		}
	}
	if(j == 0){
		return NULL;
	}
	paisesInt = realloc(paisesInt, sizeof(int)*j);
	return rta;
}

