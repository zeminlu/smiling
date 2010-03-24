#include "../inc/pipeIPC.h"

int ***ipcIDs = NULL;
int clientsAmm = 0, flag = FALSE;
int info;
hashTableADT hashTable = NULL;
fd_set *master = NULL, *slave = NULL;

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int setupIPC(int channels){
	int i;
	char pid[20], fileName[20], *nameStart = "./";
	int data;
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((master = malloc(sizeof(fd_set))) == NULL || (slave = malloc(sizeof(fd_set))) == NULL){
		perror("Error de memoria\n");
		free(master);
		return errno;
	}
	
	FD_ZERO(master);
	
	data = open(fileName, O_WRONLY | O_CREAT, 0644);
		
	ipcIDs = malloc (sizeof(void *) * channels);
	for (i = 0 ; i < channels ; ++i){
		ipcIDs[i] = malloc(sizeof(void *) * 2);
		ipcIDs[i][0] = malloc(sizeof(int) * 2);
		ipcIDs[i][1] = malloc(sizeof(int) * 2);
		if (pipe(ipcIDs[i][0]) != 0){
			perror("IPCAPI: Error en pipe");
			return -1;
		}
		if (write(data, &ipcIDs[i][0][0], sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Error en primitiva write");
			return -1;
		}
		if (pipe(ipcIDs[i][1]) != 0){
			perror("IPCAPI: Error en pipe");
			return -1;	
		}
		if (write(data, &ipcIDs[i][1][1], sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Error en primitiva write");
			return -1;
		}
		FD_SET(ipcIDs[i][1][0], master);
	}
	clientsAmm = channels;
	close(data);
	info = open(fileName, O_RDONLY);
	
	return 0;
}

int addClient(){
	return dup2(info, 0);
}

int synchronize(){
	int i, *pid, ids[2];
	char pidString[20], fileName[20], *nameStart = "./";
	
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	pid = malloc(sizeof(int) * clientsAmm);
	
	for (i = 0 ; i < clientsAmm ; ++i){
		ids[0] = ipcIDs[i][1][0];
		ids[1] = ipcIDs[i][0][1];
		if (read(ipcIDs[i][1][0], &(pid[i]), sizeof(pid_t)) != sizeof(pid_t)){
			perror("IPCAPI: Error en primitiva read");
			return -1;
		}
		itoa(pid[i], pidString);
		hashInsert(&hashTable, ids, pidString, 0);
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	close(info);
	
	itoa(getpid(), pidString);
	
	strcpy(fileName, nameStart);
	
	strcat(fileName, pidString);
	
	unlink(fileName);
	
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	int ownID[2];
	pid_t pid;
	char pidString[10];
	
	pid = getpid();
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	if (read(_stdin_, &(ownID[0]), sizeof(int)) != sizeof(int)){
		perror("IPCAPI: Error en primitiva write");
		return -1;
	}
	if (read(_stdin_, &(ownID[1]), sizeof(int)) != sizeof(int)){
		perror("IPCAPI: Error en primitiva write");
		return -1;
	}
	if (write(ownID[1], &pid, sizeof(pid_t)) != sizeof(pid_t)){
		perror("IPCAPI: Error en primitiva write");
		return -1;	
	}
	
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
    while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	close(_stdin_);
	
	hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID);
	itoa(getppid(), pidString);
	hashInsert(&hashTable, ownID, pidString, 0);
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *ipcID;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	if (read(ipcID[0], buffer, bufferSize) != bufferSize){
		perror("IPCAPI: Error en primitiva read");
		return -1;	
	}
	free(ipcID);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int *ipcID;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	if (write(ipcID[1], buffer, bufferSize) != bufferSize){
		perror("IPCAPI: Error en primitiva write");
		return -1;
	}
	free(ipcID);
	
	return 0;
}

int closeIPC(int pid){
	/*int *ipcID;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = (int *) hashDelete(&hashTable, pidString);
	close(ipcID[0]);
	close(ipcID[1]);
	*/
	return 0;
}

int selectIPC(int seconds){
	int ret;
	struct timeval timeout;
	
	*slave = *master;
	
	timeout.tv_sec = seconds;
	timeout.tv_usec = 0;
	ret = select(FD_SETSIZE, slave, NULL, NULL, &timeout);
	return ret;
}

int getIPCStatus(pid_t pid){
	int * ipcID = NULL, ret;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	ret = FD_ISSET(ipcID[0], slave);
	free(ipcID);
	
	return ret;
}

int finalizeIPC(){
	int i;
	
	for (i = 0 ; i < clientsAmm ; ++i){
		varFree(3, ipcIDs[i][0], ipcIDs[i][1], ipcIDs[i]);
	}
	free(ipcIDs);
	if (master != NULL){
		free(master);
	}
	if (slave != NULL){
		free(slave);
	}
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	return 0;
}

int compareIPCIDs(void *elem1, void *elem2){
	return (((int *)elem1)[0] == ((int *)elem1)[0] && ((int *)elem1)[1] == ((int *)elem2)[1]);
}

void * copyIPCID(void *elem){
	int *id;
	
	id = malloc(sizeof(int) * 2);
	id[0] = ((int *)elem)[0];
	id[1] = ((int *)elem)[1];
	
	return id;
}
void freeIPCID(void *elem){
	close(((int *)elem)[0]);
	close(((int *)elem)[1]);
	free(elem);
	return;
}
