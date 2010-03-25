
#include "../inc/msqIPC.h"

int queue_id= 0;
int clientsAmm = 0, flag = FALSE;
int info;
hashTableADT hashTable = NULL;

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int init_queue(int newKey){
	int queue_id;
		
	queue_id = msgget((key_t)(newKey), IPC_CREAT | QPERM);
	if(queue_id == -1){
		perror("msgget Fallo");
		return errno;
	} 
	return queue_id;
}

int setupIPC(int channels){
	int i,j, data, aux, key;
	char pid[20], fileName[20], *nameStart = "./";
	key = getpid();
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	queue_id = init_queue(key);
	data = open(fileName, O_WRONLY | O_CREAT, 0644);
	printf("SETUPIPC: queue_id = %d \n", queue_id);
	
	for (i = 0, j = 1 ; i < channels ; ++i, j+=2){
		aux = write(data, &j, sizeof(int));
	}
	printf("SETUPIPC: ya mande todos los key \n");	
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
	
	entry = malloc(sizeof(msQ));
	printf("SYNCHRONIZE: antes de crear todo \n");
	
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	printf("SYNCHRONIZE: se crea la tabla de hash \n");
	pid = malloc(sizeof(pid_t) * clientsAmm);
	
	for (i = 0, j = 1 ; i < clientsAmm ; ++i, j +=2){		
		if((mlen = msgrcv(queue_id ,entry, sizeof(pid_t), j, MSG_NOERROR)) == -1){
			perror("msgrcv fallo");
			free(entry);
			return errno;
		}		
		printf("SYNCHRONIZE: se crea la tabla de hash %d \n", entry->mtext);
		memcpy(&pid[i], entry->mtext, sizeof(pid_t));		
		ids[1]= j+1;
		ids[0]= j;
		itoa(pid[i], pidString);
		hashInsert(&hashTable, ids, pidString, 0);
	}
	printf("SYNCHRONIZE: ya tengo a todos mis hijos \n");
	
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	printf("SYNCHRONIZE: Mande la señal para que arranquen \n");
	close(info);
	
	itoa(getpid(), pidString);
	
	strcpy(fileName, nameStart);
	
	strcat(fileName, pidString);
	
	unlink(fileName);
	free(entry);
	printf("SYNCHRONIZE: termino bien \n");
	
	return 0;
}
int loadIPC(){
	sigset_t mask, oldmask;
	pid_t pid;
	int ownID[2], mlen, aux;
	char pidString[10];
	msQ entry;
	printf("LOADIPC: ENTRO AL LOAD %d \n", getpid());
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	
	queue_id = msgget((key_t)getppid(), 0);
	printf("LOADIPC: queue_id = %d \n", queue_id);
	aux = read(_stdin_, &(ownID[1]), sizeof(int));
	ownID[0] = 1 + ownID[1];
	
	printf("LOADIPC: clave de lectura = %d \n", ownID[0]);
	hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID);
	itoa(getppid(), pidString);
	hashInsert(&hashTable, ownID, pidString, 0);
	
	entry.mtype = ownID[1];
	pid = getpid();	
	memcpy(entry.mtext, &pid, sizeof(pid_t));
	printf("LOADIPC: antes de mandar %d \n", entry.mtext);
	
	if((mlen = msgsnd(queue_id, &entry, sizeof(pid_t), IPC_NOWAIT)) == -1){
	perror("msgsnd fallo");		
	return errno;
	}
	printf("LOADIPC: salio del write\n");
	
	while (!flag){
    	sigsuspend (&oldmask);
	}
	printf("LOADIPC: LLego la señal\n");
		
	close(_stdin_);
	
	return 0;
}


int readIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen, *ipcID;
	unsigned int hkey;
	msQ entry;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	if((mlen = msgrcv(queue_id ,&entry, bufferSize, ipcID[0], MSG_NOERROR)) == -1){
		perror("msgrcv fallo");
		return errno;		
	}	
	memcpy(buffer,entry.mtext, bufferSize);

	if(mlen == -1){
		buffer = NULL;
		return -2;
	}
	
return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen, * ipcID;
	msQ entry;
	unsigned hkey;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	entry.mtype = ipcID[1];	
	memcpy(entry.mtext, buffer, bufferSize);

	if((mlen = msgsnd(queue_id, &entry, bufferSize, IPC_NOWAIT)) == -1){
		perror("msgsnd fallo");		
		return errno;
	}

return 0;
}

int closeIPC(int pid){
	
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
	
	/*sleep(seconds);*/
	msgctl(queue_id, IPC_STAT, &msq_stat);
	return msq_stat.msg_qnum;
}

int getIPCStatus(pid_t pid){
	return TRUE;
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