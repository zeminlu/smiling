 #include "../inc/sckIPC.h"

int clientsAmm = 0, flag = FALSE, sockfd, info;
hashTableADT hashTable = NULL;
fd_set *master = NULL, *slave = NULL;
struct sockaddr_un address;
char socketFileName[40];

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int setupIPC(int channels){
	char pid[20], *socketNameStart = "./socket-";
	
	itoa (getpid(), pid);
	/*strcpy(fileName, nameStart);*/
	strcpy(socketFileName, socketNameStart);
	/*strcat(fileName, pid);*/
	strcat(socketFileName, pid);
	
	if ((master = malloc(sizeof(fd_set))) == NULL || (slave = malloc(sizeof(fd_set))) == NULL){
		perror("Error de memoria\n");
		free(master);
		return errno;
	}
	
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socketFileName);
	
	FD_ZERO(master);
	
	/*data = open(fileName, O_WRONLY | O_CREAT, 0644);*/
		
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("Error en llamada a socket");
		return errno;
	}
	
	if (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_un)) == -1){
		perror("Error en llamada a bind");
		return errno;
	}
	
	/*for (i = 0 ; i < channels ; ++i){
		write(data, &sockfd, sizeof(int));
	}*/
	clientsAmm = channels;
	/*close(data);
	info = open(fileName, O_RDONLY);*/
	
	return 0;
}

int addClient(){
	/*return dup2(info, 0);*/
	return 0;
}

int synchronize(){
	int i = 0, *pid, newsockfd;
	char pidString[20], fileName[20], *nameStart = "./";
	
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	if (listen(sockfd, clientsAmm) == -1){
		perror("Error en llamada a listen");
		return errno;
	}
	
	pid = malloc(sizeof(int) * clientsAmm);
	
	while (i < clientsAmm){
		if ((newsockfd = accept(sockfd, NULL, NULL)) == -1){
			perror("Error en llamada a accept");
			continue;
		}
		recv(newsockfd, &(pid[i]), sizeof(pid_t), 0);
		itoa(pid[i], pidString);
		hashInsert(&hashTable, &newsockfd, pidString, 0);
		FD_SET(newsockfd, master);
		++i;
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
	pid_t pid;
	int acc = 0;
	char pidString[20], *socketNameStart = "./socket-";
	
	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("Error en llamada a socket");
		return errno;
	}
	
	itoa (getppid(), pidString);
	strcpy(socketFileName, socketNameStart);
	strcat(socketFileName, pidString);
	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, socketFileName);
	
	pid = getpid();
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	/*read(_stdin_, &ownID, sizeof(int));*/
	
	fprintf(stderr, "Por hacer connect con pid = %d\n", pid);
	while (usleep(60), (acc += 60) < 1000 && connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_un)) == -1){
		if (errno != ECONNREFUSED ){
			perror("Error en llamada a connect");
			return errno;
		}
	}
	fprintf(stderr, "Hizo connect con pid = %d\n", pid);
	
	send(sockfd, &pid, sizeof(pid_t), 0);
	
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
    while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	close(_stdin_);
	
	hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID);
	itoa(getppid(), pidString);
	hashInsert(&hashTable, &sockfd, pidString, 0);
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	actsockfd = hashSearch(hashTable, pidString, &hkey);
	recv(actsockfd[0], buffer, bufferSize, 0);
	free(actsockfd);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	actsockfd = hashSearch(hashTable, pidString, &hkey);
	send(actsockfd[0], buffer, bufferSize, 0);
	free(actsockfd);
	
	return 0;
}

int closeIPC(int pid){

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
	int *actsockfd, ret;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	actsockfd = hashSearch(hashTable, pidString, &hkey);
	ret = FD_ISSET(actsockfd[0], slave);
	free(actsockfd);
	
	return ret;
}

int finalizeIPC(){
	if (master != NULL){
		free(master);
	}
	if (slave != NULL){
		free(slave);
	}
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	
	/*unlink(socketFileName);*/
	
	return 0;
}

int compareIPCIDs(void *elem1, void *elem2){
	return (((int *)elem1)[0] == ((int *)elem2)[0]);
}

void * copyIPCID(void *elem){
	int *id;
	
	id = malloc(sizeof(int));
	id[0] = ((int *)elem)[0];
	
	return id;
}
void freeIPCID(void *elem){
	close(((int *)elem)[0]);	
	free(elem);
	
	return;
}