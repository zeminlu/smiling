#include "../inc/pipeIPC.h"

int ***ipcIDs = NULL;
int clientsAmm = 0, flag = FALSE;
hashTableADT hashTable = NULL;
fd_set *master = NULL, *slave = NULL;

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int setupIPC(int channels){
	int i;
	
	if ((master = malloc(sizeof(fd_set))) == NULL || (slave = malloc(sizeof(fd_set))) == NULL || (ipcIDs = malloc (sizeof(void *) * channels)) == NULL){
		perror("IPCAPI: Error de memoria en setupIPC\n");
		free(master);
		free(slave);
		return errno;
	}
	
	FD_ZERO(master);
	
	for (i = 0 ; i < channels ; ++i){
		ipcIDs[i] = malloc(sizeof(void *) * 2);
		ipcIDs[i][0] = malloc(sizeof(int) * 2);
		ipcIDs[i][1] = malloc(sizeof(int) * 2);
		if (pipe(ipcIDs[i][0]) == -1){
			perror("IPCAPI: Error en pipe");
			varFree(2, master, slave);
			return -1;
		}
		
		if (pipe(ipcIDs[i][1]) == -1){
			perror("IPCAPI: Error en pipe");
			varFree(2, master, slave);
			return -1;	
		}
		FD_SET(ipcIDs[i][1][0], master);
	}
	clientsAmm = channels;
	
	return 0;
}

int addClient(int index){
	char pid[20], fileName[20], *nameStart = "./";
	int info;
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((info = open(fileName, O_WRONLY | O_CREAT, 0644)) < 0){
		perror("IPCAPI: Error abriendo archivo en setupIPC");
		return -1;
	}
	if (write(info, &(ipcIDs[index][0][0]), sizeof(int)) != sizeof(int)){
		perror("IPCAPI: addClient - Error en primitiva write");
		return -1;
	}
	if (write(info, &(ipcIDs[index][1][1]), sizeof(int)) != sizeof(int)){
		perror("IPCAPI: addClient - Error en primitiva write");
		return -1;
	}
	close(ipcIDs[index][0][1]);
	close(ipcIDs[index][1][0]);
	
	close(info);
	
	if ((info = open(fileName, O_RDONLY, 0644)) < 0){
		perror("IPCAPI: Error abriendo archivo en setupIPC");
		return -1;
	}
	return dup2(info, 0);
}

int synchronize(){
	int i, *pid, ids[2];
	char pidString[20];
	
	
	if ((hashTable = hashCreateTable(clientsAmm * _START_HASH_, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error al crear la tabla de hash\n");
		return -1;
	}
	
	if ((pid = malloc(sizeof(int) * clientsAmm)) == NULL){
		perror("IPCAPI: Error de memoria en synchronize");
		return errno;
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		close(ipcIDs[i][0][0]);
		close(ipcIDs[i][1][1]);
		
		ids[0] = ipcIDs[i][1][0];
		ids[1] = ipcIDs[i][0][1];
		if (read(ipcIDs[i][1][0], &(pid[i]), sizeof(pid_t)) != sizeof(pid_t)){
			perror("IPCAPI: synchronize - Error en primitiva read");
			return -1;
		}
		itoa(pid[i], pidString);
		if (hashInsert(&hashTable, ids, pidString, 0) == NULL){
			fprintf(stderr, "IPCAPI: Error en el insert de la tabla de hash en synchronize, invocado con ids[0] = %d, ids[1] = %d y pidString = %s\n", ids[0], ids[1], pidString);
			return -1;
		}
	}
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	free(pid);
		
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	int ownID[2];
	pid_t pid;
	char pidString[20], fileName[20], *nameStart = "./";
	printf("Entro a LoadIPC\n");
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	
	pid = getpid();
	
	if (read(_stdin_, ownID, sizeof(int) * 2) != sizeof(int) * 2){
		perror("IPCAPI: loadIPC - Error en primitiva read");
		return -1;
	}
	printf("LoadIPC Recibi voy a leer= %d voy a escribir en %d \n", ownID[0], ownID[1]);
	if (write(ownID[1], &pid, sizeof(pid_t)) != sizeof(pid_t)){
		perror("IPCAPI: loadIPC - Error en primitiva write");
		return -1;
	}
	
	while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
			
	if ((hashTable = hashCreateTable(_START_HASH_, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error creando tabla de hash en loadIPC\n");
		return -1;
	}
	itoa(getppid(), pidString);
	
	if (hashInsert(&hashTable, ownID, pidString, 0) == NULL){
		fprintf(stderr, "IPCAPI: Error insertando en tabla de hash en loadIPC\n");
		return -1;
	}
	
	itoa(getpid(), pidString);
	strcpy(fileName, nameStart);
	strcat(fileName, pidString);
	unlink(fileName);
		printf("LoadIPC SAli delload \n");
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *ipcID;
	unsigned int hkey;
	char pidString[20];
	
	itoa(pid, pidString);
	if ((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch de readIPC, invocaco con pid = %s\n", pidString);
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
		fprintf(stderr, "IPCAPI: Error en hashSearch de writeIPC, invocado con pid = %s\n", pidString);
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
	if ((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch invocado en getIPCStatus con pidstring = %s", pidString);
	}
	
	ret = FD_ISSET(ipcID[0], slave);
	free(ipcID);
	
	return ret;
}

int closeIPC(){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	hashTable = NULL;

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
