/**
 * \file main.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#include "../inc/main.h"

pid_t pid1, pid2;

void handler(int sig){
	kill(pid1, SIGINT);
	kill(pid2, SIGINT);
}

int main (void){
	int p1, p2;
	
	signal(SIGINT, handler);
	
	switch (pid1 = fork()){
		case 0:
			execv("./parallel.bin", NULL);
			break;
		default:
			switch (pid2 = fork()){
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
