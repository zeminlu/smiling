#include "../inc/fifaGen.h"

int main (void){
	int i, amm, head, headsAmm = 0, death, c1, c2, c3;
	pais *countries;
	FILE *countriesFile;
	
	if ((countriesFile = fopen("./testFiles/countries.fifa", "w")) == NULL){
			perror("Error al crear archivo");
			return errno;
	}
		
	if ((countries = malloc(sizeof(pais))) == NULL){
			perror("Error de memoria");
			fclose(countriesFile);
			return errno;
	}
	
	printf("Ingrese cantidad de countries\n");
	while (scanf("%d", &amm) != 1){
		printf("Ingrese un numero ENTERO\n");
	}	
	
	for (i = 0 ; i < amm ; ++i){
		printf("Ingrese el nombre del pais\n");
		while (scanf("%s", countries->name) != 1){
			printf("Ingrese SOLO el nombre del pais\n");
		}
		printf("Ingrese el ID del continente\n");
		while (scanf("%d", &(countries->continent)) != 1){
			printf("Ingrese SOLO el ID del continente\n");
		}
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		while (scanf("%d", &(countries->champ)) != 1){
			printf("Ingrese SOLO 1 o 0\n");
		}
		printf("Ingrese el peso del pais\n");
		while (scanf("%d", &(countries->weight)) != 1){
			printf("Ingrese un numero ENTERO\n");
		}
		if (headsAmm < amm / 4){
			printf("Debe ser Cabeza de Serie? Restan %d\n", amm/4 - headsAmm);
			while (scanf("%d", &head) != 1){
				printf("Infrese SOLO 1 o 0");
			}
			if (head){
				++headsAmm;
				countries->isHead = TRUE;
				printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
				
				if (death == FALSE){
					printf("Debe ser el grupo de la muerte?\n");
					while (scanf("%d", &death) != 1){
						printf("Ingrese SOLO 1 o 0\n");
					}
					if (death){
						countries->deathGroup = TRUE;
					}
				}
				
				printf("Puede aceptar countries del mismo continente?\n");

				while (scanf("%d", &c1) != 1){
					printf("Ingrese SOLO 1 o 0\n");
				}
				if(c1 == FALSE){
					countries->sameContinent = TRUE;
				}

				if (!countries->deathGroup){
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
							countries->champGroup = TRUE;
						}
					}
					else{
						countries->weakGroup = TRUE;
					}
				}
			}
		}
		printf("OK\n");
		
		if (fwrite(countries, sizeof(pais), 1, countriesFile) != 1){
			perror("Error de escritura");
			free(countries);
			fclose(countriesFile);
			return errno;
		}
	}
	
	free(countries);
	fclose(countriesFile);
	
	return 0;
}
