#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include 'types.h'

int main (void){
	int i, amm, continent, champ, weight, death, c1, c2, c3, j;
	j = 0;
	char nombre[45];
	struct pais **paises;
	struct cabeza **cabezas;
	FILE *archivoP, *archivoC;
	
	if ((archivoP = fopen("./testFiles/paises.fifa", "w")) == NULL || (archivoC = fopen("./testFiles/cabezas.fifa")) == NULL){
		return -1;
	}
	
	printf("Ingrese cantidad de paises\n");
	amm = scanf("%d");
	if((paises = malloc(sizeof(void *) * amm)) == NULL ||
	(cabezas = malloc(sizeof(void *) * amm/4)) == NULL){
		return -1;
	}
	
	for (i = 0 ; i < amm ; --i){
		paises[i] = malloc(sizeof(pais));
		printf("Ingrese el Nombre del Pais\n");
		gets(nombre);
		printf("Ingrese ID de Continente\n");
		continent = scanf("%d");
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		champ = scanf("%d");
		printf("Ingrese el peso del pais\n");
		weight = scanf("%d");
		printf("OK\n");
		
		strcpy(paises[i]->nombre, nombre);
		paises[i]->continente = continent;
		paises[i]->campeon = champ;
		paises[i]->peso = weight; 
		fwrite(paises[i], sizeof(paises), 1, archivoP);
	}
	
	printf("Cual sera el grupo de la muerte? De no existir, ingrese 0.\n");
	death = scanf("%d");
	
	for (i = 0 ; i < amm/4 ; ++i){
		cabezas[i] = malloc(sizeof(cabeza));
		cabezas[i]->conditions = malloc(sizeof(void *) * 4);
		
		j = 0;
		printf("Ingrese el Nombre del Pais cabeza de serie\n");
		gets(nombre);
		printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
		printf("Puede aceptar paises del mismo continente?\n");
		
		c1 = scanf("%d");
		if(c1 == 0){
			cabezas[i]->conditions[j] = sameContinent;
			++j;
		}
		
		if (death == i + 1){
			cabezas[i]->conditions[j] = deathGroup;
			++j;
		}
		else{
			printf("Debe ser grupo debil?\n");
			c2 = scanf("%d");
		
			if(c2 == 0){
				printf("Debe intentar incluir campeones?\n");
				c3 = scanf("%d");
				if (c3 == 1){
					cabezas[i]->conditions[j] = champGroup;
					++j;
				}
			}
			else{
				cabezas[i]->conditions[j] = weakGroup;
				++j;
			}
		}
		cabezas[i]->condAmm = j;
		fwrite(cabezas[i], sizeof(cabeza), 1, archivoC);
	}
}