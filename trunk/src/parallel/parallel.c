#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

int main(){
	struct dirent *d;
	DIR *dp;
	
	if ((dp = opendir("./parallelDir")) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) < 3){
		sleep(1);
	};
	printf("Nuevo archivo en parallel\n");
	return 0;
}

int getFilesAmm (DIR *dp){
	struct dirent *d;
	int ret;
	while (d = readdir(dp)){
		 if (d->d_ino != 0){
			++ret;
		}
	}
	return ret;
}