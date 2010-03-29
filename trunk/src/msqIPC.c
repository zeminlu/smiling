
#include "../inc/msqIPC.h"

int queue_id= 0;
int clientsAmm = 0, flag = FALSE;
int info;
hashTableADT hashTable = NULL;
msQ * msLastRead = NULL;

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int init_queue(int newKey){
	int queue_id;
		
	return msgget((key_t)(newKey), IPC_CREAT | QPERM);
}

int setupIPC(int channels){
	int i,j, data, aux, key;
	char pid[20], fileName[20], *nameStart = "./";
	key = getpid();
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	if((queue_id = init_queue(key)) == -1){
	  	perror("msgget Fallo");
		return errno;
	}
	if((data = open(fileName, O_WRONLY | O_CREAT, 0644)) < 0){
		perror("Error abriendo archivo en setupIPC");	
		return -1;
	}
	
	for (i = 0, j = 1 ; i < channels ; ++i, j+=2){
		aux = write(data, &j, sizeof(int));
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
	int i,j, ids[2], mlen;
	pid_t *pid;
	char pidString[20], fileName[20], *nameStart = "./";
	msQ  * entry;
	
	if((entry = malloc(sizeof(msQ))) == NULL){
	    perror("Error en crear la entry para la msQ");
	    return errno;
	}
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	if((pid = malloc(sizeof(pid_t) * clientsAmm)) == NULL){
		perror("Error en crear el vector de pids");
		free(entry);
		return errno;
	}
	
	for (i = 0, j = 1 ; i < clientsAmm ; ++i, j +=2){		
		if((mlen = msgrcv(queue_id ,entry, sizeof(pid_t), j, MSG_NOERROR)) == -1){
			perror("msgrcv fallo");
			free(entry);
			free(pid);
			return errno;
		}		
		memcpy(&pid[i], entry->mtext, sizeof(pid_t));		
		ids[1]= j+1;
		ids[0]= j;

		
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
	
	free(pid);
	free(entry);
	
	return 0;
}
int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	int ownID[2], mlen, aux;
	char pidString[10];
	msQ entry;

	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	
	if((queue_id = msgget((key_t)getppid(), 0)) == -1){
		perror("msgget fallo");
		return -1;
	}

	if((aux = read(_stdin_, &(ownID[1]), sizeof(int))) == -1){
		perror("Error en el la primitiva read");
		return errno;
	}
	
	ownID[0] = 1 + ownID[1];
	
	if((hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	itoa(getppid(), pidString);
	
	if((hashInsert(&hashTable, ownID, pidString, 0)) == NULL){
		fprintf(stderr, "Error al insertar en la tabla de hash\n");
		return -1;
	}
	
	entry.mtype = ownID[1];
	pid = getpid();	
	memcpy(entry.mtext, &pid, sizeof(pid_t));

	if((mlen = msgsnd(queue_id, &entry, sizeof(pid_t), IPC_NOWAIT)) == -1){
		perror("msgsnd fallo");		
		return errno;
	}
	
	while (!flag){
    	sigsuspend (&oldmask);
	}		
	close(_stdin_);
	
	return 0;
}


int readIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen, *ipcID;
	unsigned int hkey;
	msQ entry;
	char pidString[20];
	
	itoa(pid, pidString);
	if((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error al buscar en la tabla de hash en readIPC\n");
		return -1;
	}
	
	if(msLastRead == NULL){
		if((mlen = msgrcv(queue_id ,&entry, bufferSize, ipcID[0], MSG_NOERROR)) == -1){
			perror("msgrcv fallo");
			return errno;		
		}	
		memcpy(buffer,entry.mtext, bufferSize);	

		
	}else if(msLastRead->mtype == ipcID[0]){
		memcpy(buffer,msLastRead->mtext, bufferSize);
		free(msLastRead);
		msLastRead = NULL;			
	}

	
	return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen, * ipcID;
	msQ entry;
	unsigned hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	if((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
		fprintf(stderr, "Error al buscar en la  tabla de hash en writeIPC\n");
		return -1;
	}
	entry.mtype = ipcID[1];	
	memcpy(entry.mtext, buffer, bufferSize);
		

	if((mlen = msgsnd(queue_id, &entry, bufferSize, 0)) == -1){
		perror("msgsnd fallo");		
		return errno;
	}

	return 0;
}

int closeIPC(int pid){
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	return 0;
}

int finalizeIPC(){
 	msgctl(queue_id, IPC_RMID, NULL);
	if (hashTable != NULL){
		hashFreeTable(hashTable);
	}
	return 0;
}

int selectIPC(int seconds){
	struct msqid_ds msq_stat;
	int acc = 0;
	
	while (usleep(60), (acc += 60) < seconds * 1000){
		msgctl(queue_id, IPC_STAT, &msq_stat);
		if(msq_stat.msg_qnum > 0){
		  return msq_stat.msg_qnum;
		}
	}
	return FALSE;
}

int getIPCStatus(pid_t pid){
	int mlen, *ipcID;
	int bufferSize = sizeof(char)*200;
	unsigned int hkey;
	msQ * entry;
	char pidString[10];
	
	if((entry = malloc(sizeof(msQ))) == NULL){
		perror("GETIPCSTATUS: Errora al crear la memoria para el entry de IPC status");
		return errno;
	}
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	if((mlen = msgrcv(queue_id ,entry, bufferSize, ipcID[0],IPC_NOWAIT)) == -1){
		free(entry);
		return FALSE;		
	}else{
		msLastRead = entry;
		free(entry);
		return TRUE;
	}	
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
	free(elem);
	return;
}