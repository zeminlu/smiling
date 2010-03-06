#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include "../inc/types.h"
#include "../inc/conditions.h"

int main (void){
	int i, amm, continent, champ, weight, death, c1, c2, c3;
	char nombre[45];
	pais **paises;
	cabeza **cabezas;
	FILE *archivoP, *archivoC;
	
	if ((archivoP = fopen("./testFiles/paises.fifa", "w")) == NULL || (archivoC = fopen("./testFiles/cabezas.fifa", "w")) == NULL){
		return -1;
	}
	
	printf("Ingrese cantidad de paises\n");
	scanf("%d", &amm);
	if((paises = malloc(sizeof(void *) * amm)) == NULL ||
	(cabezas = malloc(sizeof(void *) * (amm/4))) == NULL){
		return -1;
	}
	
	for (i = 0 ; i < amm ; ++i){
		paises[i] = malloc(sizeof(pais));
		printf("Ingrese el Nombre del Pais\n");
		scanf("%s", nombre);
		printf("Ingrese ID de Continente\n");
		scanf("%d", &continent);
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		scanf("%d", &champ);
		printf("Ingrese el peso del pais\n");
		scanf("%d", &weight);
		printf("OK\n");
		
		strcpy((paises[i])->nombre, nombre);
		(paises[i])->continente = continent;
		(paises[i])->campeon = champ;
		(paises[i])->peso = weight;
		fwrite(paises[i], sizeof(pais), 1, archivoP);
	}
	
	printf("Cual sera el grupo de la muerte? De no existir, ingrese 0.\n");
	scanf("%d", &death);
	
	for (i = 0 ; i < amm/4 ; ++i){
		cabezas[i] = malloc(sizeof(cabeza));
		
		printf("Ingrese el Nombre del Pais cabeza de serie\n");
		scanf("%s", nombre);
		printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
		printf("Puede aceptar paises del mismo continente?\n");
		
		scanf("%d", &c1);
		if(c1 == 0){
			cabezas[i]->sameContinent = 1;
		}
		
		if (death == i + 1){
			cabezas[i]->deathGroup = 1;
		}
		else{
			printf("Debe ser grupo debil?\n");
			scanf("%d", &c2);
		
			if(c2 == 0){
				printf("Debe intentar incluir campeones?\n");
				scanf("%d", &c3);
				if (c3 == 1){
					cabezas[i]->champGroup = 1;
				}
			}
			else{
				cabezas[i]->weakGroup = 1;
			}
		}
		fwrite(cabezas[i], sizeof(cabeza), 1, archivoC);
	}
	
	return 0;
}