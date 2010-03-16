#include "../inc/ipcAPI.h"

int setupIPC(int mode, int *ipcIDs, char *childName, int *childPID){
	int auxP1[2], auxP2[2];
	
	if (mode == _FULL_DUPLEX_){
		pipe(auxP1);
		pipe(auxP2);
		
		switch((*childPID = fork())){
			case -1:
				perror("Error de fork");
				return errno;
				break;
			case 0:
				close(auxP1[0]);
				close(auxP2[1]);
				dup2(auxP1[1], 1);
				dup2(auxP2[0], 0);
				execv(childName, NULL);
				break;
			default:					
				close(auxP1[1]);
				close(auxP2[0]);		
				break;
		}
		ipcIDs[0] = auxP1[0];
		ipcIDs[1] = auxP2[1];
	}
	else{
		pipe(auxP1);
		
		switch((*childPID = fork())){
			case -1:
				perror("Error de fork");
				return errno;
				break;
			case 0:
				close(auxP1[1]);
				dup2(auxP1[0], 0);
				execv(childName, NULL);
				break;
			default:					
				close(auxP1[0]);
				break;
		}
		ipcIDs[1] = auxP1[1];
	}
	return 0;
}

int readIPC(int ipcID, void *buffer, int bufferSize){
	return read(ipcID, buffer, bufferSize);
}


int writeIPC(int ipcID, void *buffer, int bufferSize){
	return write(ipcID, buffer, bufferSize);
}

int closeIPC(int ipcID){
	return close(ipcID);
}

void * prepareIPC(int **ipcIDs, int amm, int *allocSize){
	int j;
	fd_set *tmp;
	
	*allocSize = sizeof(fd_set);
	
	if ((tmp = malloc(sizeof(fd_set))) == NULL){
		perror("Error de memoria\n");
		return NULL;
	}
	
	FD_ZERO(tmp);
	
	for (j = 0 ; j < amm ; ++j){
		FD_SET(ipcIDs[j][0], tmp);
	} 
	
	return tmp;
}

int selectIPC(void *set, int seconds){
	int ret;
	struct timeval timeout;
	
	timeout.tv_sec = seconds;
	timeout.tv_usec = 0;
	
	ret = select(FD_SETSIZE, set, NULL, NULL, &timeout);
	
	fprintf(stderr, "Ret en select = %d", ret);
	
	return ret;
}

int getIPCStatus(int ipcID, void *data){
	
	return FD_ISSET(ipcID, data);
}
