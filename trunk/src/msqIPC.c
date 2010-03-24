
#include "../inc/msqIPC.h"

int queue_id = 0;
int clientsAmm = 0, flag = FALSE;
int info;

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
	int i,j;
	int data, key;
	char pid[20], fileName[20], *nameStart = "./";
	
	key = getpid();
	
	itoa (getpid(), pid);
	strcpy(fileName, nameStart);
	strcat(fileName, pid);
	
	queue_id = init_queue(key);
	data = open(filename, O_WRONLY | O_CREAT, 0644);
	printf("SETUPIPC: queue_id = %d \n", queue_id);
	
	for (i = 0, j = 1 ; i < channels ; ++i, j+=2){
		write(data, &j, sizeof(int));
	}
	
	clientsAmm = channels;
	close(data);
	info = open(filename, O_RDONLY);
	
	return 0;
}


int addClient(){
	return dup2(info, 0);
}
int synchronize(){
	int i,j, *pid, ids[2], mlen;
	char pidString[20], fileName[20], *nameStart = "./";
	msQ  * entry;
	entry = malloc(sizeof(msQ));
	if ((hashTable = hashCreateTable(clientsAmm, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr, "Error al crear la tabla de hash\n");
		return -1;
	}
	
	pid = malloc(sizeof(int) * clientsAmm);
	
	for (i = 0, j = 1 ; i < clientsAmm ; ++i, ++j){
		
		if((mlen = msgrcv(queue_id ,entry, sizeof(pid_t), j, MSG_NOERROR)) == -1){
			perror("msgrcv fallo");
			return errno;
			printf("READ IPC DESPUES de leer \n");

		}		
		ids[1]= j;
		ids[0]= j+1;
		pid[i]= entry->mtext;
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
	int ownID[2], mlen;

	char pidString[10];
	msQ entry;
	
	entry.mtype = 	
	memcpy(entry.mtext, getpid(), sizeof(int));
	
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);
	
	queue_id = msgget((key_t)getppid(), 0);

	read(_stdin_, &(ownID[0]), sizeof(int));
	ownID[1] = 1+ownID[0];
	
	hashTable = hashCreateTable(1, freeIPCID, compareIPCIDs, copyIPCID);
	itoa(getppid(), pidString);
	hashInsert(&hashTable, ownID, pidString, 0);
	
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
	msQ entry;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	printf("READ IPC antes de LEER \n");
	if((mlen = msgrcv(queue_id ,&entry, bufferSize, ipcID[0], MSG_NOERROR)) == -1){
		perror("msgrcv fallo");
		return errno;
		printf("READ IPC DESPUES de leer \n");
		
	}
	printf("READ IPC DESPUES de leer \n");
	
	memcpy(buffer,entry.mtext, bufferSize);

	
	printf("READ IPC DESPUES de hacer el copymem \n");
	if(mlen == -1){
		buffer = NULL;
		return -2;
	}
	
return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen, * ipcID;
	msQ entry;
	char pidString[10];
	
	itoa(pid, pidString);
	ipcID = hashSearch(hashTable, pidString, &hkey);
	
	entry.mtype = (long)ipcID[1];	
	memcpy(entry.mtext, buffer, bufferSize);
	printf("writeIPC: PID en el quevoy a escibir % d\n");

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

int selectIPC(int seconds){
	struct msqid_ds msq_stat;
	
	sleep(seconds);
	msgctl(queue_idR, IPC_STAT, &msq_stat);
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
