#include "../inc/fifa.h"

int main (void){
	package *p = NULL;
	country **countries;
	int i, pamm;
	
	countries = (country **)(p->genStruct);
	pamm = p->amm;
	/*VER BIEN PAMM PORQUE NO VA ESE...*/
	for (i = 0 ; i < pamm ; ++i){
		switch(fork()){
			case 0:
				execv("./grouph.bin", NULL);
				/*Se le pasa a grouph la estructura cabezas[i]*/
				break;
			default:
				break;
		}
	}
	/*
	* Aca viene el listener que administra las peticiones de countries de los grouph hijos.
	* A su vez, se debe al final, invocar los waits correspondientes y evaluarlos.
	*
	*/
	
	return 0;
}