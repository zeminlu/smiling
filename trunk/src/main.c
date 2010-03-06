#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main (void){
	int p1, p2;
	switch (fork()){
		case 0:
			execv("./pipe", NULL);
			break;
		default:
			switch (fork()){
				case 0:
					execv("./parallel", NULL);
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