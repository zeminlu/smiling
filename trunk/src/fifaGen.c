/*
 * \file fifaGen.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#include "../inc/fifaGen.h"

int main (void){
	int i, amm, head, headsAmm = 0, death, c1, c2, c3;
	country *countries;
	FILE *countriesFile;
	time_t	now;
	struct tm *ts;
	char fileName[80], buf[80];
	
    now = time(NULL);
	ts = localtime(&now);
	strftime(buf, sizeof(buf), "%H-%M-%S", ts);
	
	strcpy(fileName, "./testFiles/paises_");
	strcat(fileName, buf);
	strcat(fileName, ".fifa");
	
	if ((countriesFile = fopen(fileName, "w")) == NULL){
			perror("Error al crear archivo");
			return errno;
	}
		
	if ((countries = malloc(sizeof(country))) == NULL){
			perror("Error de memoria");
			fclose(countriesFile);
			return errno;
	}
	
	printf("Ingrese cantidad de paises\n");
	while (scanf("%d", &amm) != 1 || amm % 8 != 0 || amm <= 0){
		printf("Ingrese un numero ENTERO multiplo de 8 y mayor a 0\n");
		while(getchar() != '\n');
	}	
	
	for (i = 0 ; i < amm ; ++i){
		if ((countries = calloc(1, sizeof(country))) == NULL){
				perror("Error de memoria");
				fclose(countriesFile);
				return errno;
		}
		
		printf("Ingrese el nombre del pais\n");
		while (scanf("%s", countries->name) != 1){
			printf("Ingrese SOLO el nombre del pais\n");
			while(getchar() != '\n');
		}
		printf("Ingrese el ID del continente\n");
		while (scanf("%d", &(countries->continent)) != 1){
			printf("Ingrese SOLO el ID del continente (ENTERO)\n");
			while(getchar() != '\n');
		}
		printf("Ingrese 1 si el pais fue campeón, 0 de lo contrario\n");
		while (scanf("%d", &(countries->champ)) != 1 || (countries->champ != 0 && countries->champ != 1)){
			printf("Ingrese SOLO 1 o 0\n");
			while(getchar() != '\n');
		}
		printf("Ingrese el peso del pais\n");
		while (scanf("%d", &(countries->weight)) != 1 && countries->weight <= 0){
			printf("Ingrese un numero ENTERO mayor a 0\n");
			while(getchar() != '\n');
		}
		if (headsAmm < amm / 4){
			printf("Debe ser Cabeza de Serie? Restan %d\n", amm/4 - headsAmm);
			while (scanf("%d", &head) != 1 || (head != 0 && head != 1)){
				printf("Ingrese SOLO 1 o 0");
				while(getchar() != '\n');
			}
			if (head){
				++headsAmm;
				countries->isHead = TRUE;
				printf("A continuacion se le preguntara acerca de la inclusion de ciertas restricciones. Conteste con 1 o 0.\n\n");
				
				if (death == FALSE){
					printf("Debe ser el grupo de la muerte?\n");
					while (scanf("%d", &death) != 1 || (death != 0 && death != 1)){
						printf("Ingrese SOLO 1 o 0\n");
						while(getchar() != '\n');
					}
					if (death){
						countries->deathGroup = TRUE;
					}
				}
				
				printf("Puede aceptar paises del mismo continente?\n");

				while (scanf("%d", &c1) != 1 || (c1 != 0 && c1 != 1)){
					printf("Ingrese SOLO 1 o 0\n");
					while(getchar() != '\n');
				}
				if(c1 == FALSE){
					countries->sameContinent = TRUE;
				}

				if (!countries->deathGroup){
					printf("Debe ser grupo debil?\n");
					while (scanf("%d", &c2) != 1 || (c2 != 0 && c2 != 1)){
						printf("Ingrese SOLO 1 o 0\n");
						while(getchar() != '\n');
					}

					if(c2 == FALSE){
						printf("Debe intentar incluir campeones?\n");
						while (scanf("%d", &c3) != 1 || (c3 != 0 && c3 != 1)){
							printf("Ingrese SOLO 1 o 0\n");
							while(getchar() != '\n');
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
			else{
				countries->isHead = FALSE;
			}
		}
		else{
			countries->isHead = FALSE;
		}
		printf("\n");
		if (fwrite(countries, sizeof(country), 1, countriesFile) != 1){
			perror("Error de escritura");
			free(countries);
			fclose(countriesFile);
			return errno;	
		}
		free(countries);
	}
	printf("Archivo generado correctamente en carpeta testFiles\n");
	
	fclose(countriesFile);
	
	return 0;
}
