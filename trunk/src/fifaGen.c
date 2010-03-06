#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include "../inc/types.h"
#include "../inc/conditions.h"

int main (void){
	int i, amm, death, c1, c2, c3;
	pais *paises;
	cabeza *cabezas;
	FILE *archivoP, *archivoC;
	
	if ((archivoP = fopen("./testFiles/paises.fifa", "w")) == NULL || 
		(archivoC = fopen("./testFiles/cabezas.fifa", "w")) == NULL){
			perror("Error al crear archivos");
			fclose(archivoP);
			return errno;
	}
	
	if ((paises = malloc(sizeof(pais))) == NULL || 
		(cabezas = malloc(sizeof(cabeza))) == NULL){
			perror("Error de memoria");
			free(paises);
			fclose(archivoP);
			fclose(archivoC);
			return errno;
	}
	
	printf("Ingrese cantidad de paises\n");
	scanf("%d", &amm);
	
	for (i = 0 ; i < amm ; ++i){
		printf("Ingrese el Nombre del Pais\n");
		scanf("%s", paises->nombre);
		printf("Ingrese ID de Continente\n");
		scanf("%d", &(paises->continente));
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		scanf("%d", &(paises->campeon));
		printf("Ingrese el peso del pais\n");
		scanf("%d", &(paises->peso));
		printf("OK\n");
		
		if (fwrite(paises, sizeof(pais), 1, archivoP) != 1){
			perror("Error de escritura");
			free(paises);
			free(cabezas);
			fclose(archivoP);
			fclose(archivoC);
			return errno;
		}
	}
	
	printf("Cual sera el grupo de la muerte? De no existir, ingrese 0.\n");
	scanf("%d", &death);
	
	for (i = 0 ; i < amm/4 ; ++i){
		
		printf("Ingrese el Nombre del Pais cabeza de serie\n");
		scanf("%s", cabezas->nombre);
		printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
		printf("Puede aceptar paises del mismo continente?\n");
		
		scanf("%d", &c1);
		if(c1 == 0){
			cabezas->sameContinent = 1;
		}
		
		if (death == i + 1){
			cabezas->deathGroup = 1;
		}
		else{
			printf("Debe ser grupo debil?\n");
			scanf("%d", &c2);
		
			if(c2 == 0){
				printf("Debe intentar incluir campeones?\n");
				scanf("%d", &c3);
				if (c3 == 1){
					cabezas->champGroup = 1;
				}
			}
			else{
				cabezas->weakGroup = 1;
			}
		}
		if (fwrite(cabezas, sizeof(cabeza), 1, archivoC) != 1){
			perror("Error de escritura");
			free(paises);
			free(cabezas);
			fclose(archivoP);
			fclose(archivoC);
			return errno;
		}
	}
	
	free(paises);
	free(cabezas);
	fclose(archivoC);
	fclose(archivoP);
	
	return 0;
}