#include "../inc/fifaGen.h"

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
	while (scanf("%d", &amm) != 1){
		printf("Ingrese un numero ENTERO\n");
	}	
	
	for (i = 0 ; i < amm ; ++i){
		printf("Ingrese el nombre del pais\n");
		while (scanf("%s", paises->nombre) != 1){
			printf("Ingrese SOLO el nombre del pais\n");
		}
		printf("Ingrese el ID del continente\n");
		while (scanf("%d", &(paises->continente)) != 1){
			printf("Ingrese SOLO el ID del continente\n");
		}
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		while (scanf("%d", &(paises->campeon)) != 1){
			printf("Ingrese SOLO 1 o 0\n");
		}
		printf("Ingrese el peso del pais\n");
		while (scanf("%d", &(paises->peso)) != 1){
			printf("Ingrese un numero ENTERO\n");
		}
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
	while (scanf("%d", &death) != 1){
		printf("Ingrese SOLO un numero ENTERO\n");
	}

	for (i = 0 ; i < amm/4 ; ++i){
		
		printf("Ingrese el Nombre del Pais cabeza de serie\n");
		while (scanf("%s", cabezas->nombre) != 1){
			printf("Ingrese SOLO el NOMBRE del pais\n");
		}
		printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
		printf("Puede aceptar paises del mismo continente?\n");
		
		while (scanf("%d", &c1) != 1){
			printf("Ingrese SOLO 1 o 0\n");
		}
		if(c1 == 0){
			cabezas->sameContinent = 1;
		}
		
		if (death == i + 1){
			cabezas->deathGroup = 1;
		}
		else{
			printf("Debe ser grupo debil?\n");
			while (scanf("%d", &c2) != 1){
				printf("Ingrese SOLO 1 o 0\n");
			}
		
			if(c2 == 0){
				printf("Debe intentar incluir campeones?\n");
				while (scanf("%d", &c3) != 1){
					printf("Ingrese SOLO 1 o 0\n");
				}
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
