#include "../inc/shmIPC.h"

int shmemId= 0;
void *shmemStart;
int clientsAmm = 0, flag = FALSE;
int info;
hashTableADT hashTable = NULL;


void sigHandler (int signum){
	flag = TRUE;
	return;
}

sem_t * initmutex(char *semName)
{
	sem_t *sd;
	
	if ((sd = sem_open(semName, O_RDWR|O_CREAT, 0666, 1)) == SEM_FAILED ){
		perror("IPCAPI: Error en llamada a sem_open");
		return NULL;
	}
	return sd;
}

int initShMem(int newKey){	
	shmemId = shmget((key_t)(newKey), clientsAmm * (_SHM_SEG_SIZE_ + sizeof(shmHeader)), IPC_CREAT | 0666);
	if(shmemId == -1){
		perror("IPCAPI: Error en llamada a shmget");
		return errno;
	} 
	return shmemId;
}

int initSem(int pid, shmElem *aux){
	char semString[20], pidString[20];
	
	strcpy(semString, "SEMA-");
	itoa(pid, pidString);
	strcat(semString, pidString);
	if ((aux->semA = initmutex(semString)) == NULL){
		return -1;
	}
	strcpy(semString, "SEMB-");
	itoa(pid, pidString);
	strcat(semString, pidString);
	if ((aux->semB = initmutex(semString)) == NULL){
		return -1;
	}
	
	return 0;
}

int initHeaders(){
	int i;
	shmHeader aux;
	
	for (i = 0 ; i < clientsAmm ; ++i){
		aux.startPos = ((char *)shmemStart) + (sizeof(shmHeader) * clientsAmm + i * _SHM_SEG_SIZE_);
		aux.cReadOff = aux.cWriteOff = aux.sWriteOff = aux.sReadOff = 0;
		aux.pid = -1;
		memcpy(((char *)shmemStart) + (i * sizeof(shmHeader)), &aux, sizeof(shmHeader));
	}
	
	return 0;
}

int setupIPC(int channels){
	int i, data, key, status;
	char pid[20], fileName[20], *nameStart = "./";
		
	key = getpid();
	clientsAmm = channels;
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if ((status = initShMem(key)) < 0){
		return status;
	}
	
	if ((shmemStart = shmat(shmemId, NULL, 0)) == ERR){
		perror("IPCAPI: Error en llamada a shmat en setupIPC");
		return errno;
	}
	
	data = open(fileName, O_WRONLY | O_CREAT, 0644);
	
	if ((status = initHeaders()) < 0){
		return status;
	}
	
	for (i = 0 ; i < channels ; ++i){
		if (write(data, &shmemId, sizeof(key_t)) != sizeof(key_t) || write(data, &i, sizeof(int)) != sizeof(int)){
			perror("IPCAPI: Error en primitiva write en setupIPC");
			return errno;
		}
	}

	close(data);
	info = open(fileName, O_RDONLY);
	
	fprintf(stderr, "Salio de setupIPC\n");
	
	return 0;
}


int addClient(){
	return dup2(info, 0);
}

int synchronize(){
	int i, flag;
	pid_t *pid;
	char pidString[20], fileName[20], *nameStart = "./";
	shmHeader *auxHead;
	shmElem entry;
	
	fprintf(stderr, "Entre a synchronize\n");
	
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error al crear la tabla de hash en synchronize\n");
		return -1;
	}
	
	if ((pid = malloc(sizeof(pid_t) * clientsAmm)) == NULL){
		perror("IPCAPI: Error de memoria allocando pid en synchronize");
		return errno;
	}
	
	flag = FALSE;
	
	while (!flag){
		flag = TRUE;
		for (i = 0 ; i < clientsAmm ; ++i){
			auxHead = (shmHeader *)(((char *)shmemStart) + (i * sizeof(shmHeader)));
			if (auxHead->pid == -1){
				flag = FALSE;
			}
		}
	}
	
	for (i = 0 ; i < clientsAmm ; ++i){		
		fprintf(stderr, "Entre al for de synchronize con i = %d, clientsAmm = %d\n", i, clientsAmm);
		auxHead = (shmHeader *)(((char *)shmemStart) + (i * sizeof(shmHeader)));
		
		pid[i] = auxHead->pid;
		if (initSem(pid[i], &entry) == -1){
			return -1;
		}
		
		itoa(pid[i], pidString);
		
		entry.index = i;
		
		entry.read = &(auxHead->sReadOff);
		
		entry.write = &(auxHead->sWriteOff);
		
		entry.otherRead = &(auxHead->cReadOff);
		
		entry.otherWrite = &(auxHead->cWriteOff);
		
		if (hashInsert(&hashTable, &entry, pidString, 0) == NULL){
			fprintf(stderr, "IPCAPI: Error en hashInsert invocado en synchronize con pidString = %s", pidString);
		}
		fprintf(stderr, "Sali del for de synchronize con i = %d\n", i);
	}

	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	
	close(info);
	
	itoa(getpid(), pidString);
	strcpy(fileName, nameStart);
	strcat(fileName, pidString);
	unlink(fileName);
	
	fprintf(stderr, "Salio de synchronize\n");
	
	return 0;
}
int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	void *data;
	int aux, index;
	key_t key;
	char pidString[20];
	shmElem entry;
	shmHeader *auxHead;
	sem_t *tmp;
	
	fprintf(stderr, "Entre a loadIPC\n");
	
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	sigprocmask (SIG_BLOCK, &mask, &oldmask);
	
	if ((data = malloc(sizeof(int) + sizeof(key_t))) == NULL){
		perror("IPCAPI: Error de memoria alocando data en loadIPC");
		return errno;
	}
	
	if ((aux = read(_stdin_, data, sizeof(int) + sizeof(key_t)) != sizeof(int) + sizeof(key_t))){
		perror("IPCAPI: Error en llamada a read en loadIPC");
		return errno;
	}
	
	memcpy(&key, data, sizeof(key_t));
	memcpy(&index, ((char *)data) + sizeof(key_t), sizeof(int));
	
	if ((shmemStart = shmat(key, NULL, 0)) == ERR){
		perror("IPCAPI: Error en llamada a shmat en loadIPC");
		return errno;
	}
	
	if ((hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "IPCAPI: Error en la inicializacion de la tabla de hash en loadIPC\n");
		return -1;
	}
	
	auxHead = (shmHeader *)(((char *)shmemStart) + (index * sizeof(shmHeader)));

	pid = getpid();

	initSem(pid, &entry);
	
	itoa(getppid(), pidString);
	entry.index = index;
	entry.read = &(auxHead->cReadOff);
	entry.write = &(auxHead->cWriteOff);
	entry.otherRead = &(auxHead->sReadOff);
	entry.otherWrite = &(auxHead->sWriteOff);
	tmp = entry.semA;
	entry.semA = entry.semB;
	entry.semB = tmp;
	if (hashInsert(&hashTable, &entry, pidString, 0) == NULL){
		fprintf(stderr, "IPCAPI: Error en el insert de hashTable de loadIPC invocado con pidString = %s\n", pidString);
		return -1;
	}	
	
	auxHead->pid = pid;
	
	while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
			
	close(_stdin_);
	free(data);
	
	fprintf(stderr, "Sali del load\n");	
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int module, remaining, readAlready;
	unsigned int hkey;
	shmElem *entry;
	shmHeader *auxHead = NULL;
	char pidString[20];
	
	if (bufferSize > _SHM_SEG_SIZE_ / 2){
		fprintf(stderr, "IPCAPI: Lectura de tamaño superior a _SHM_SEG_SIZE\n");
		return -1;
	}
	
	remaining = bufferSize;
	readAlready = 0;
	
	itoa(pid, pidString);
	if ((entry = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch dentro de readIPC invocado con pidString = %s\n", pidString);
	}
	auxHead = (shmHeader *)(((char *)shmemStart) + (entry->index * sizeof(shmHeader)));

	while(TRUE){
		sem_wait(entry->semA);
		if (*(entry->read) < *(entry->otherWrite)){
			if (bufferSize > (*(entry->otherWrite) - *(entry->read))){
				sem_post(entry->semA);
				if (usleep(60) == -1){
					fprintf(stderr, "IPCAPI: Error en el usleep de readIPC\n");
					return -1;
				}
				continue;
			}
			break;
		}
		else if (*(entry->read) > *(entry->otherWrite)){
			module = (_SHM_SEG_SIZE_ / 2) - *(entry->read);
			if (module + *(entry->otherWrite) < bufferSize){
				sem_post(entry->semA);
				if (usleep(60) == -1){
					fprintf(stderr, "IPCAPI: Error en el usleep de readIPC\n");
					return -1;
				}
				continue;
			}
			break;
		}
		else{
			sem_post(entry->semA);
			if (usleep(60) == -1){
				fprintf(stderr, "IPCAPI: Error en el usleep de readIPC\n");
				return -1;
			}
			continue;
		}
	}
	
	if (*(entry->read) < *(entry->otherWrite)){
		memcpy(buffer, ((char *)auxHead->startPos) + (*(entry->read)), bufferSize);
		*(entry->read) += bufferSize;
	}
	else if (*(entry->read) > *(entry->otherWrite)){
		if (bufferSize > _SHM_SEG_SIZE_ / 2 - *(entry->read)){
			memcpy(buffer, ((char *)auxHead->startPos) + (*(entry->read)), _SHM_SEG_SIZE_ / 2 - *(entry->read));
			remaining -= _SHM_SEG_SIZE_ / 2 - *(entry->read);
			memcpy(((char *)buffer) + (_SHM_SEG_SIZE_ / 2 - *(entry->read)), auxHead->startPos, remaining);
			*(entry->read) = remaining;
		}
		else{
			memcpy(buffer, ((char *)auxHead->startPos) + *(entry->read), bufferSize);
			*(entry->read) += bufferSize;
		}
	}

	sem_post(entry->semA);
	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int i, module, remaining, wroteAlready;
	unsigned int hkey;
	shmElem *entry;
	shmHeader *auxHead = NULL;
	char pidString[20];
	
	remaining = bufferSize;
	wroteAlready = 0;
	
	if (bufferSize > _SHM_SEG_SIZE_ / 2){
		fprintf(stderr, "IPCAPI: Escritura de tamaño superior a _SHM_SEG_SIZE\n");
	}
	
	itoa(pid, pidString);
	if ((entry = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch dentro de writeIPC invocado con pidString = %s\n", pidString);
	}
	auxHead = (shmHeader *)(((char *)shmemStart) + entry->index * sizeof(shmHeader));
	
	while(TRUE){
		sem_wait(entry->semB);
		if (*(entry->write) < *(entry->otherRead)){
			if (bufferSize > (*(entry->otherRead) - *(entry->write))){
				sem_post(entry->semB);
				if (usleep(60) == -1){
					fprintf(stderr, "IPCAPI: Error en el usleep de writeIPC\n");
					return -1;
				}
				continue;
			}
			break;
		}
		else if (*(entry->write) > *(entry->otherRead)){
			module = (_SHM_SEG_SIZE_ / 2) - *(entry->write);
			if (module + *(entry->otherRead) < bufferSize){
				sem_post(entry->semB);
				if (usleep(60) == -1){
					fprintf(stderr, "IPCAPI: Error en el usleep de writeIPC\n");
					return -1;
				}
				continue;
			}
			break;
		}
		break;
	}
	
	for (i = 0 ; i < bufferSize ; ++i){
		if (*(entry->write) > _SHM_SEG_SIZE_ / 2){
			*(entry->write) = 0;
		}
		memcpy(((char *)auxHead->startPos) + *(entry->write), ((char *)buffer) + i, sizeof(char));
		(*(entry->write))++;
	}

	sem_post(entry->semB);
	
	return 0;
}

int closeIPC(int pid){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	hashTable = NULL;
	if (shmdt(shmemStart) == -1){
		perror("IPCAPI: Error desatachandome de la shmem");
	}
	return 0;
}

int finalizeIPC(){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	hashTable = NULL;
	if (shmctl(shmemId, IPC_RMID, NULL) == -1){
		perror("IPCAPI: Error liberando la shmem");
	}
	return 0;
}

int selectIPC(int seconds){
	shmElem *aux;
	int acc = 0;

	while (usleep(60), (acc += 60) < seconds * 1000){
		hashSetIterator(hashTable);
		while((aux = (shmElem *) hashGetNext(hashTable))){
			if (*(aux->read) != *(aux->otherWrite)){
				return TRUE;
			}
		}
	}
	return FALSE;
}

int getIPCStatus(pid_t pid){
	shmElem *aux;
	unsigned int hkey;
	char pidString[20];
	
	itoa(pid, pidString);
	if ((aux = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "IPCAPI: Error en hashSearch dentro de getIPCStatus, invocado con pidString = %s\n", pidString);
		return -1;
	}
	if (*(aux->read) != *(aux->otherWrite)){
		return TRUE;
	}
	
	return FALSE;
}

int compareIPCIDs(void *elem1, void *elem2){
	return (((shmElem *)elem1)->index == ((shmElem *)elem2)->index);
}

void * copyIPCID(void *elem){
	shmElem *aux;
	
	aux = malloc(sizeof(shmElem));
	*aux = *((shmElem *) elem);
	
	return aux;
}
void freeIPCID(void *elem){
	shmElem *aux;
	
	aux = (shmElem *)elem;
	
	if (sem_close(aux->semA) == -1 || sem_close(aux->semB) == -1){
		perror("IPCAPI: Error liberando semaforo");
	}
	free(aux);
	return;
}