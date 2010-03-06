#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "../inc/pipe.h"

int main(){
	DIR *dp;
	
	if ((dp = opendir("./pipeDir")) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) < 4);
	printf("Nuevo archivo en pipe\n");
	return 0;
}

int getFilesAmm (DIR *dp){
	struct dirent *d;
	int ret = 0;
	while ((d = readdir(dp))){
		 if (d->d_ino != 0){
			++ret;
		}
	}
	return ret;
}