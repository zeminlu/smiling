#include "../inc/fifaGen.h"

int main (void){
	int i, amm, head, headsAmm = 0, death, c1, c2, c3;
	pais *paises;
	FILE *archivoP;
	
	if ((archivoP = fopen("./testFiles/paises.fifa", "w")) == NULL){
			perror("Error al crear archivo");
			return errno;
	}
		
	if ((paises = malloc(sizeof(pais))) == NULL){
			perror("Error de memoria");
			fclose(archivoP);
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
		if (headsAmm < amm / 4){
			printf("Debe ser Cabeza de Serie? Restan %d\n", amm/4 - headsAmm);
			while (scanf("%d", &head) != 1){
				printf("Infrese SOLO 1 o 0");
			}
			if (head){
				++headsAmm;
				paises->isHead = TRUE;
				printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
				
				if (death == FALSE){
					printf("Debe ser el grupo de la muerte?\n");
					while (scanf("%d", &death) != 1){
						printf("Ingrese SOLO 1 o 0\n");
					}
					if (death){
						paises->deathGroup = TRUE;
					}
				}
				
				printf("Puede aceptar paises del mismo continente?\n");

				while (scanf("%d", &c1) != 1){
					printf("Ingrese SOLO 1 o 0\n");
				}
				if(c1 == FALSE){
					paises->sameContinent = TRUE;
				}

				if (!paises->deathGroup){
					printf("Debe ser grupo debil?\n");
					while (scanf("%d", &c2) != 1){
						printf("Ingrese SOLO 1 o 0\n");
					}

					if(c2 == FALSE){
						printf("Debe intentar incluir campeones?\n");
						while (scanf("%d", &c3) != 1){
							printf("Ingrese SOLO 1 o 0\n");
						}
						if (c3 == !FALSE){
							paises->champGroup = TRUE;
						}
					}
					else{
						paises->weakGroup = TRUE;
					}
				}
			}
		}
		printf("OK\n");
		
		if (fwrite(paises, sizeof(pais), 1, archivoP) != 1){
			perror("Error de escritura");
			free(paises);
			fclose(archivoP);
			return errno;
		}
	}
	
	free(paises);
	fclose(archivoP);
	
	return 0;
}
