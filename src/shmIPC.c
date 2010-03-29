#include "../inc/msqIPC.h"

int shmemId= 0;
void *shmemStart;
int clientsAmm = 0, flag = FALSE;
int info;
hashTableADT hashTable = NULL;


void sigHandler (int signum){
	flag = TRUE;
	return;
}


int initmutex(char *semName)
{
	int sd;
	
	if ( !(sd = sem_open(semName, O_RDWR|O_CREAT, 0666, 1)) ){
		perror("Error en llamada a sem_open");
		return errno;
	}
	return sd;
}

int initShMem(int newKey){	
	shmemId = shmget((key_t)(newKey), clientsAmm * (_SHMEM_SEG_SIZE_ + sizeof(shmHeader)), IPC_CREAT | 0644);
	if(shmemId == -1){
		perror("Error en llamada a shmget");
		return errno;
	} 
	return shmemId;
}

int initSem(int pid, shmHeader *aux){
	strcpy(semString, "SEMA-");
	itoa(pid, pidString);
	strcat(semString, pidString);
	if ((aux->semA = initmutex(semString)) < 0){
		return aux->semA;
	}
	strcpy(semString, "SEMB-");
	itoa(pid, pidString);
	strcat(semString, pidString);
	if ((aux->semB = initmutex(semString)) < 0){
		return aux->semB;
	}
	
	return 0;
}

int initHeaders(pid_t *pids){
	int i;
	shmHeader aux;
	char semString[20], pidString[20];
	
	for (i = 0 ; i < clientsAmm ; ++i){
		aux.startPos = shmemStart + sizeof(shmHeader) * clientsAmm + i * _SHM_SEG_SIZE_;
		aux.cReadOff = aux.cWriteOff = aux.sWriteOff = aux.sReadOff = 0;
		aux.semA = aux.semB = aux.pid = -1;
		memcpy(shmemStart + i * sizeof(shmHeader), &aux, sizeof(shmHeader));
	}
}

int setupIPC(int channels){
	int i,j, data, aux, key, status;
	char pid[20], fileName[20], *nameStart = "./";
	
	key = getpid();
	clientsAmm = channels;
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((status = initdShMem(key)) < 0){
		return status;
	}
	
	if ((shmemStart = shmat(shmemId, NULL, 0)) == ERR){
		perror("Error en llamada a shmat");
		return errno;
	}
	
	data = open(fileName, O_WRONLY | O_CREAT, 0644);
	
	if ((status = initHeaders()) < 0){
		return status;
	}
	
	for (i = 0 ; i < channels ; ++i){
		aux = write(data, &shmId, sizeof(key_t));
		aux = write(data, &i, sizeof(int));
	}

	close(data);
	info = open(fileName, O_RDONLY);
	
	return 0;
}


int addClient(){
	return dup2(info, 0);
}

int synchronize(){
	int i,j, ids[2], mlen, flag;
	pid_t *pid;
	char pidString[20], fileName[20], *nameStart = "./";
	shmHeader *auxHead;
	shmElem entry;
	
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	if ((pid = malloc(sizeof(pid_t) * clientsAmm)) == NULL){
		perror("Error de memoria");
		return errno;
	}
	
	flag = FALSE;
	
	while (!flag){
		flag = TRUE;
		for (i = 0 ; i < clientsAmm ; ++i){
			auxHead = shmemStart + i * sizeof(shmHeader);
			if (auxHead->pid == -1){
				flag = FALSE;
			}
		}
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){
		auxHead = shmemStart + i * sizeof(shmHeader);
		if (initSem(pid[i] = auxHead->pid, auxHead) < 0){
			return -1;
		}
		itoa(pid[i], pidString);
		entry->index = i;
		entry->read = &(auxHead->sReadOff);
		entry->write = &(auxHead->sWriteOff);
		entry->otherRead = &(auxHead->cReadOff);
		entry->otherWrite = &(auxHead->cWriteOff);
		entry->semA = auxHead->semA;
		entry->semB = auxHead->semB;
		hashInsert(&hashTable, &entry, pidString, &hkey, 0);
	}

	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}

	close(info);
	
	itoa(getpid(), pidString);
	strcpy(fileName, nameStart);
	strcat(fileName, pidString);
	unlink(fileName);
	free(entry);
	
	return 0;
}
int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	int data[2], mlen, aux;
	char pidString[20];
	shmElem entry;
	shmHeader *auxHead;
	
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	
	if ((aux = read(_stdin_, data, sizeof(int) * 2) != sizeof(int) * 2){
		perror("Error en llamada a read");
		return errno;
	}
	
	if ((shmemStart = shmat(data[0], NULL, 0)) == ERR){
		perror("Error en llamada a shmat");
		return errno;
	}
	
	if ((hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error en la inicializacion de la tabla de hash\n");
		return -1;
	}
	
	auxHead = shmemStart + data[1] * sizeof(shmHeader);

	pid = getpid();

	initSem(pid, auxHead);

	itoa(getppid(), pidString);
	entry.index = data[1];
	entry.read = &(auxHead->cReadOff);
	entry.write = &(auxHead->cWriteOff);
	entry.otherRead = &(auxHead->sReadOff);
	entry.otherWrite = &(auxhead->sWriteOff);
	entry->semA = auxHead->semB;
	entry->semB = auxHead->semA;
	if (hashInsert(&hashTable, &entry, pidString, 0) == NULL){
		fprintf(stderr, "Error en el insert de hashTable\n");
		return -1;
	}	
	
	auxHead->pid = pid;
	
	while (!flag){
    	sigsuspend (&oldmask);
	}		
	close(_stdin_);
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int module, *ipcID, remaining, readAlready;
	unsigned int hkey;
	shmElem *entry;
	shmHeader auxHead = NULL;
	char pidString[10];
	void *shm;
	
	if (bufferSize > _SHM_SEG_SIZE_ / 2){
		fprintf(stderr, "Lectura de tamaño superior a _SHM_SEG_SIZE\n");
	}
	
	remaining = bufferSize;
	readAlready = 0;
	
	itoa(pid, pidString);
	entry = hashSearch(hashTable, pidString, &hkey);
	auxHead = shmStart + entry->index * sizeof(shmHeader);
	
	

	shm = shmStart + auxHead->startPos + *(entry->read);
	
	sem_wait(entry->semA);
	while(sem_post(entry->semA), uspleep(100)){
		sem_wait(entry->semA);
		if (*(entry->read) < *(entry->otherWrite)){
			if (bufferSize > (*(entry->otherWrite) - *(entry->read))){
				continue;
			}
			break;
		}
		else if (*(entry->read) > *(entry->otherWrite)){
			module = (_SHM_SEG_SIZE_ / 2) - *(entry->read);
			if (module + *(entry->otherWrite) < bufferSize){
				continue;
			}
			break;
		}
		else{
			continue;
		}
	}
	
	if (*(entry->read) < *(entry->otherWrite)){
		memcpy(buffer, auxHead->startPos + *(entry->read), bufferSize);
		*(entry->read) += bufferSize;
	}
	else if (*(entry->read) > *(entry->otherWrite)){
		if (bufferSize > _SHM_SEG_SIZE_ / 2 - *(entry->read)){
			memcpy(buffer, auxHead->startPos + *(entry->read), _SHM_SEG_SIZE_ / 2 - *(entry->read));
			remaining -= _SHM_SEG_SIZE_ / 2 - *(entry->read);
			memcpy(&(buffer[_SHM_SEG_SIZE_ / 2 - *(entry->read)]), auxHead->startPos, remaining);
			*(entry->read) = remaining;
		}
		else{
			memcpy(buffer, auxHead->startPos + *(entry->read), bufferSize);
			*(entry->read) += bufferSize;
		}
	}

	sem_post(entry->semA);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int module, *ipcID, remaining, wroteAlready;
	unsigned int hkey;
	shmElem *entry;
	shmHeader auxHead = NULL;
	char pidString[10];
	void *shm;
	
	if (bufferSize > _SHM_SEG_SIZE_ / 2){
		fprintf(stderr, "Escritura de tamaño superior a _SHM_SEG_SIZE\n");
	}
	
	remaining = bufferSize;
	wroteAlready = 0;
	
	itoa(pid, pidString);
	entry = hashSearch(hashTable, pidString, &hkey);
	auxHead = shmStart + entry->index * sizeof(shmHeader);

	shm = shmStart + auxHead->startPos + *(entry->write);
	
	sem_wait(entry->semB);
	while(sem_post(entry->semB), uspleep(100)){
		sem_wait(entry->semB);
		if (*(entry->write) < *(entry->otherRead)){
			if (bufferSize > (*(entry->otherRead) - *(entry->write))){
				continue;
			}
			break;
		}
		else if (*(entry->write) > *(entry->otherRead)){
			module = (_SHM_SEG_SIZE_ / 2) - *(entry->write);
			if (module + *(entry->otherRead) < bufferSize){
				continue;
			}
			break;
		}
		else{
			continue;
		}
	}
	
	for (i = 0 ; i < bufferSize ; ++i){
		if (*(entry->write) > _SHM_SEG_SIZE_ / 2){
			*(entry->write) = 0;
		}
		buffer[i] = auxHead->startPos + *(entry->write);
		(*(entry->write))++;
	}

	sem_post(entry->semB);
	
	return 0;
}

int closeIPC(int pid){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	if (shmctl(shmId, IPC_RMID) == -1){
		perror("Error liberando la shmem");
	}
	return 0;
}

int finalizeIPC(){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	if (shmctl(shmId, IPC_RMID) == -1){
		perror("Error liberando la shmem");
	}
	return 0;
}

int selectIPC(int seconds){
	shmHeader *aux;
	int acc = 0;

	while (usleep(60), (acc += 60) < seconds * 1000){
		hashSetIterator(hashTable);
		while(aux = (shmHeader *) hashGetNext(hashTable)){
			if (*(aux->read) == *(aux->otherWrite)){
				return TRUE;
			}
		}
	}
	return FALSE;
}

int getIPCStatus(pid_t pid){
	shmHeader *aux;
	int bufferSize = sizeof(char)*200;
	unsigned int hkey;
	char pidString[20];
	
	itoa(pid, pidString);
	aux = hashSearch(hashTable, pidString, &hkey);
	
	if (aux->read != aux->otherWrite){
		return TRUE;
	}
	
	return FALSE;
}

int compareIPCIDs(void *elem1, void *elem2){
	return (((shmHeader *)elem1)->pid == ((shmHeader *)elem2)->pid);
}

void * copyIPCID(void *elem){
	shmHeader *aux;
	
	aux = malloc(sizeof(shmHeader));
	*aux = *((shmHeader *) elem);
	
	return a;
}
void freeIPCID(void *elem){
	shmHeader *aux = (shmHeader *)elem;
	if (semctl(elem->semA, IPC_RMID) == -1 || semctl(elem->semB, IPC_RMID) == -1){
		perror("Error liberando semaforo");
		return errno;
	}
	free(elem);
	return;
}