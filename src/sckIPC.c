 #include "../inc/sckIPC.h"

int clientsAmm = 0, flag = FALSE, sockfd, info, ownPort = 5000;
hashTableADT hashTable = NULL;
fd_set *master = NULL, *slave = NULL;
struct sockaddr_in address;
char socketFileName[40];

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int setupIPC(int channels){
	char pid[20], fileName[20], *socketNameStart = "./socket-", *nameStart = "./";
	int data, i;
	
	itoa (getpid(), pid);
	strcpy(socketFileName, socketNameStart);
	strcat(socketFileName, pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((master = malloc(sizeof(fd_set))) == NULL || (slave = malloc(sizeof(fd_set))) == NULL){
		perror("Error de memoria\n");
		free(master);
		return errno;
	}
	
	if ((data = open(fileName, O_WRONLY | O_CREAT, 0644)) < 0){
		perror("Error abriendo archivo en setupIPC");
		varFree(2, master, slave);
		return -1;
	}
	
	address.sin_family = AF_INET;
	address.sin_port = ownPort;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	FD_ZERO(master);
		
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Error en llamada a socket");
		return errno;
	}
	
	while (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1){
		if (errno == EADDRINUSE){
			++ownPort;
			address.sin_port = ownPort;
			continue;
		}
		perror("Error en llamada a bind");
		return errno;
	}
	
	for (i = 0 ; i < channels ; ++i){
		if (write(data, &ownPort, sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Setup - Error en primitiva write");
			varFree(2, master, slave);
			return -1;
		}
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
	printf("me fui de synchronize\n");
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	int acc = 0;
	char pidString[20], *socketNameStart = "./socket-";
	if (read(_stdin_, &ownPort, sizeof(int)) != sizeof(int)){
		perror("IPCAPI: loadIPC 1 - Error en primitiva write");
		return -1;
	}
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("Error en llamada a socket");
		return errno;
	}
	
	itoa (getppid(), pidString);
	strcpy(socketFileName, socketNameStart);
	strcat(socketFileName, pidString);
	address.sin_family = AF_INET;
	address.sin_port = ownPort;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	pid = getpid();
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	
	fprintf(stderr, "Por hacer connect con pid = %d\n", pid);
	while (usleep(60), (acc += 60) < 1000 && connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) == -1){
		if (errno != ECONNREFUSED ){
			perror("Error en llamada a connect");
			return errno;
		}
	}
	fprintf(stderr, "Hizo connect con pid = %d\n", pid);
	
	send(sockfd, &pid, sizeof(pid_t), 0);
	
	if ((hashTable = hashCreateTable(10, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error creando tabla de hash en loadIPC\n");
		return -1;
	}
	
	itoa(getppid(), pidString);
	
	if (hashInsert(&hashTable, &sockfd, pidString, 0) == NULL){
		fprintf(stderr, "Error insertando en tabla de hash en loadIPC\n");
		return -1;
	}
	
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	close(_stdin_);

	printf("Sali de loadIPC\n");
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	if ((actsockfd = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error en hashSearch invocado en readIPC con pidString = %s\n", pidString);
		return -1;
	}
	recv(actsockfd[0], buffer, bufferSize, 0);
	free(actsockfd);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	if ((actsockfd = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error en hashSearch de writeIPC invocado con pidString = %s\n", pidString);
		return -1;
	}
	send(actsockfd[0], buffer, bufferSize, 0);
	free(actsockfd);
	
	return 0;
}

int closeIPC(int pid){
	fprintf(stderr, "Terminando pid = %d\n", pid);
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	hashTable = NULL;
	
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
	if ((actsockfd = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error en hashSearch de getIPCStatus invocado con pidString = %s\n", pidString);
		return -1;
	}
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
	
	unlink(socketFileName);
	
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