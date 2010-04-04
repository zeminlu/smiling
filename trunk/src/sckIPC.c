/**
 * \file sckIPC.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

 #include "../inc/sckIPC.h"

int clientsAmm = 0, flag = FALSE, sockfd, ownPort = 5000;
hashTableADT hashTable = NULL;
fd_set *master = NULL, *slave = NULL;
struct sockaddr_in address;
char socketFileName[40];

void sigHandler (int signum){
	flag = TRUE;
	return;
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

int setupIPC(int channels){
	char pid[20], *socketNameStart = "./socket-";
	
	if ((master = malloc(sizeof(fd_set))) == NULL || (slave = malloc(sizeof(fd_set))) == NULL){
		perror("IPCAPI: Error de memoria en setupIPC\n");
		varFree(2, master, slave);
		return errno;
	}
	
	itoa (getpid(), pid);
	strcpy(socketFileName, socketNameStart);
	strcat(socketFileName, pid);
	
	address.sin_family = AF_INET;
	address.sin_port = ownPort;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	FD_ZERO(master);
		
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("IPCAPI: Error en llamada a socket");
		return errno;
	}
	
	while (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) == -1){
		if (errno == EADDRINUSE){
			++ownPort;
			address.sin_port = ownPort;
			continue;
		}
		perror("IPCAPI: Error en llamada a bind");
		return errno;
	}
	clientsAmm = channels;
	
	return 0;
}

int addClient(int index){
	int data;
	char pid[20], fileName[20], *nameStart = "./";
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((data = open(fileName, O_WRONLY | O_CREAT, 0644)) < 0){
		perror("IPCAPI: Error abriendo archivo en addClient");
		return -1;
	}
	
	if (write(data, &ownPort, sizeof(int)) != sizeof(int)){
		perror("IPCAPI: addClient - Error en primitiva write");
		varFree(2, master, slave);
		return errno;
	}
	
	close(data);
	if ((data = open(fileName, O_RDONLY)) < 0){
		return data;
	}
	
	return dup2(data, 0);
}

int synchronize(){
	int i = 0, *pid, newsockfd, status;
	char pidString[20];
	
	if ((hashTable = hashCreateTable(clientsAmm * _START_HASH_, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error al crear la tabla de hash\n");
		return -1;
	}
	
	if (listen(sockfd, clientsAmm + 5) == -1){
		perror("IPCAPI: Error en llamada a listen");
		return errno;
	}
	
	pid = malloc(sizeof(int) * clientsAmm);
	while (i < clientsAmm){
		if ((newsockfd = accept(sockfd, NULL, NULL)) == -1){
			if (errno == EAGAIN || errno == ENETDOWN || errno == EPROTO || errno == ENOPROTOOPT || errno == EHOSTDOWN || errno == ENONET || errno == EHOSTUNREACH || errno == EOPNOTSUPP || errno == ENETUNREACH){
				continue;
			}
			else if (newsockfd < 0){
				perror("IPCAPI: Error en llamada a accept");
				return -1;
			}
		}
		if ((status = recv(newsockfd, &(pid[i]), sizeof(pid_t), 0)) != sizeof(pid_t)){
			if (status == 0){
				perror("IPCAPI: Peer hizo shutdown del socket");
			}
			else if( status == -1){
				perror("IPCAPI: Error -1 en recv");
			}
			else{
				perror("IPCAPI: Error desconocido en recv");
			}
			return -1;
		}
		itoa(pid[i], pidString);
		if (hashInsert(&hashTable, &newsockfd, pidString, 0) == NULL){
			fprintf(stderr, "IPCAPI: Error en hashInsert de synchronize invocado con pidString = %s a partir de pid[%d] = %d\n", pidString, i, pid[i]);
			return -1;
		}
		FD_SET(newsockfd, master);
		++i;
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	int status;
	char pidString[20], fileName[20], *nameStart = "./", *socketNameStart = "./socket-";
	
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);

	
	if (read(_stdin_, &ownPort, sizeof(int)) != sizeof(int)){
		perror("IPCAPI: loadIPC - Error en primitiva write");
		return -1;
	}
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("IPCAPI: Error en llamada a socket");
		return errno;
	}
	
	itoa (getppid(), pidString);
	strcpy(socketFileName, socketNameStart);
	strcat(socketFileName, pidString);
	address.sin_family = AF_INET;
	address.sin_port = ownPort;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	pid = getpid();
	
	while (usleep(10000), connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) == -1){
		if (errno != ECONNREFUSED ){
			perror("IPCAPI: Error en llamada a connect");
			return errno;
		}
	}
	
	if ((status = send(sockfd, &pid, sizeof(pid_t), 0)) == -1){
		perror("IPCAPI: Error en send de loadIPC");
		return errno;
	}
	
	if ((hashTable = hashCreateTable(10, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error creando tabla de hash en loadIPC\n");
		return -1;
	}
	
	itoa(getppid(), pidString);
	
	if (hashInsert(&hashTable, &sockfd, pidString, 0) == NULL){
		fprintf(stderr, "IPCAPI: Error insertando en tabla de hash en loadIPC\n");
		return -1;
	}
	
	while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	close(_stdin_);
	itoa(getpid(), pidString);
	strcpy(fileName, nameStart);
	strcat(fileName, pidString);
	unlink(fileName);

	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd, status;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	if ((actsockfd = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch invocado en readIPC con pidString = %s\n", pidString);
		return -1;
	}
	if ((status = recv(actsockfd[0], buffer, bufferSize, 0)) != bufferSize){
		if (status == 0){
			fprintf(stderr, "IPCAPI: Peer de pid = %d hizo shutdown del socket\n", pid);
		}
		else if( status == -1){
			fprintf(stderr, "IPCAPI: Error -1 en recv\n");
		}
		else{
			fprintf(stderr, "IPCAPI: Error desconocido en recv = %d", status);
		}
		return -1;
	}
	free(actsockfd);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int *actsockfd, status;
	unsigned int hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	if ((actsockfd = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch de writeIPC invocado con pidString = %s\n", pidString);
		return -1;
	}
	if((status = send(actsockfd[0], buffer, bufferSize, 0)) == -1){
		perror("IPCAPI: Error en el send de writeIPC");
		return errno;
	}
	free(actsockfd);
	
	return 0;
}

int closeIPC(int pid){
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
		fprintf(stderr, "IPCAPI: Error en hashSearch de getIPCStatus invocado con pidString = %s\n", pidString);
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
