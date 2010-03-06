#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

int main (void){
	int p1, p2;
	switch (fork()){
		case 0:
			execv("./parallel.bin", NULL);
			break;
		default:
			/*switch (fork()){
				case 0:
					execv("./pipe.bin", NULL);
					break;
				default:
					wait(&p1);
			*/		wait(&p2);
					break;	
			
			break;
	}
	return 0;
}