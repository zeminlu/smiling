#include "../inc/fifa.h"

int main (void){
	package c, p;
	pais **paises;
	cabeza **cabezas;
	int i, pamm, camm;
	
	paises = (paises **)(p->estructura);
	cabezas = (cabezas **)(c->estructura);
	pamm = p->amm;
	camm = c->amm;
	
	for (i = 0 ; i < camm ; ++i){
		switch(fork){
			case 0:
				execv("./grouph.bin", NULL);
				/*Se le pasa a grouph la estructura cabezas[i]*/
				break;
			default:
				break;
		}
	}
	/*
	* Aca viene el listener que administra las peticiones de paises de los grouph hijos.
	* A su vez, se debe al final, invocar los waits correspondientes y evaluarlos.
	*
	*/
}