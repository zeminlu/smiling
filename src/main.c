/*
 * \file main.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#include "../inc/main.h"

int main (void){
	int p1, p2;
	switch (fork()){
		case 0:
			execv("./parallel.bin", NULL);
			break;
		default:
			switch (fork()){
				case 0:
					execv("./pipe.bin", NULL);
					break;
				default:
					wait(&p1);
					wait(&p2);
					break;	
			}
			break;
	}
	return 0;
}
