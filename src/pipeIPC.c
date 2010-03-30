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
	
	if ((data = open(fileName, O_WRONLY | O_CREAT, 0644)) < 0){
		perror("Error abriendo archivo en setupIPC");
		varFree(2, master, slave);
		return -1;
	}
		
	ipcIDs = malloc (sizeof(void *) * channels);
	for (i = 0 ; i < channels ; ++i){
		ipcIDs[i] = malloc(sizeof(void *) * 2);
		ipcIDs[i][0] = malloc(sizeof(int) * 2);
		ipcIDs[i][1] = malloc(sizeof(int) * 2);
		if (pipe(ipcIDs[i][0]) == -1){
			perror("IPCAPI: Error en pipe");
			varFree(2, master, slave);
			return -1;
		}
		if (write(data, &ipcIDs[i][0][0], sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Setup1 - Error en primitiva write");
			varFree(2, master, slave);
			return -1;
		}
		if (pipe(ipcIDs[i][1]) == -1){
			perror("IPCAPI: Error en pipe");
			varFree(2, master, slave);
			return -1;	
		}
		if (write(data, &ipcIDs[i][1][1], sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Setup2 - Error en primitiva write");
			varFree(2, master, slave);
			return -1;
		}
		FD_SET(ipcIDs[i][1][0], master);
	}
	clientsAmm = channels;
	close(data);
	if ((info = open(fileName, O_RDONLY)) < 0){
		return info;
	}
	
	return 0;
}

int addClient(){
	return dup2(info, 0);
}

int synchronize(){
	int i, *pid, ids[2];
	char pidString[20], fileName[20], *nameStart = "./";
	
	if ((hashTable = hashCreateTable(clientsAmm * 4, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	if ((pid = malloc(sizeof(int) * clientsAmm)) == NULL){
		perror("Error de memoria en synchronize");
		return errno;
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		ids[0] = ipcIDs[i][1][0];
		ids[1] = ipcIDs[i][0][1];
		if (read(ipcIDs[i][1][0], &(pid[i]), sizeof(pid_t)) != sizeof(pid_t)){
			perror("IPCAPI: synchronize - Error en primitiva read");
			return -1;
		}
		itoa(pid[i], pidString);
		if (hashInsert(&hashTable, ids, pidString, 0) == NULL){
			fprintf(stderr, "Error en el insert de la tabla de hash en synchronize, invocado con ids[0] = %d, ids[1] = %d y pidString = %s\n", ids[0], ids[1], pidString);
			return -1;
		}
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	close(info);
	itoa(getpid(), pidString);
	strcpy(fileName, nameStart);
	strcat(fileName, pidString);
	unlink(fileName);
	free(pid);
	
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	int ownID[2];
	pid_t pid;
	char pidString[20];
	
	pid = getpid();
	
	if (read(_stdin_, ownID, sizeof(int) * 2) != sizeof(int) * 2){
		perror("IPCAPI: loadIPC 1 - Error en primitiva write");
		return -1;
	}

	if (write(ownID[1], &pid, sizeof(pid_t)) != sizeof(pid_t)){
		perror("IPCAPI: loadIPC - Error en primitiva write");
		return -1;	
	}
	
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);	
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
    
	while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	close(_stdin_);
		
	if ((hashTable = hashCreateTable(10, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error creando tabla de hash en loadIPC\n");
		return -1;
	}
	itoa(getppid(), pidString);
	
	if (hashInsert(&hashTable, ownID, pidString, 0) == NULL){
		fprintf(stderr, "Error insertando en tabla de hash en loadIPC\n");
		return -1;
	}
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *ipcID;
	unsigned int hkey;
	char pidString[20];
	
	itoa(pid, pidString);
	if ((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error en hashSearch de readIPC, invocaco con pid = %s\n", pidString);
		return -1;
	}
	
	if (read(ipcID[0], buffer, bufferSize) != bufferSize){
		perror("IPCAPI: readIPC - Error en primitiva read");
		return -1;	
	}
	free(ipcID);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int *ipcID;
	unsigned int hkey;
	char pidString[20];
	
	itoa(pid, pidString);
	if ((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error en hashSearch de writeIPC, invocado con pid = %s\n", pidString);
		return -1;
	}
	
	if (write(ipcID[1], buffer, bufferSize) != bufferSize){
		perror("IPCAPI: writeIPC - Error en primitiva write");
		return -1;
	}
	
	free(ipcID);
	
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

int closeIPC(){
/*	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	hashTable = NULL;
*/
	return 0;
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
	return (((int *)elem1)[0] == ((int *)elem2)[0] && ((int *)elem1)[1] == ((int *)elem2)[1]);
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
