
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
        for (i = 0, j = 1 ; i < channels ; ++i, j+=2){
                aux = write(data, &j, sizeof(int));
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
        int i,j, ids[2], mlen;
        pid_t *pid;
        char pidString[20], fileName[20], *nameStart = "./";
        msQ entry;
       
        /*entry = malloc(sizeof(msQ));*/
        if ((hashTable = hashCreateTable(clientsAmm * 4, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
                fprintf(stderr, "Error al crear la tabla de hash\n");
                return -1;
        }
        pid = malloc(sizeof(pid_t) * clientsAmm);
       
        for (i = 0, j = 1 ; i < clientsAmm ; ++i, j +=2){              
            printf("SYNCHRONIZE:antes queue_id = %d, Hijo numero = %d I = %d,\n",queue_id, j,i);
			if((mlen = msgrcv(queue_id ,&entry, sizeof(pid_t), j, MSG_NOERROR)) == -1){
				perror("msgrcv fallo");
				return errno;
			}      
			memcpy(&pid[i], entry.mtext, sizeof(pid_t));          
			printf("SYNCHRONIZE:despues ,Numero de escritura = %d,\n", pid[i]); 
			ids[1]= j+1;
			ids[0]= j;
			printf("SYNCHRONIZE ownId[0] = %d , ownID[1] = %d \n",ids[0],ids[1]);
               
			itoa(pid[i], pidString);
			if(hashInsert(&hashTable, ids, pidString, 0) == NULL){
				fprintf(stderr,"Error en insertar una entry en la tabla de hash en synchronize con pidString = %s\n", pidString);
				return -1;
			}
        }

        printf("SYNCHRONIZE:Ya inserte  en la tabla de hash\n");

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
        int ownID[2], mlen, aux;
        char pidString[10];
        msQ entry;

        signal(SIGALRM, sigHandler);
        sigemptyset (&mask);
        sigaddset (&mask, SIGALRM);
		sigprocmask (SIG_BLOCK, &mask, &oldmask);

        queue_id = msgget((key_t)getppid(), 0);

        aux = read(_stdin_, &(ownID[1]), sizeof(int));
        ownID[0] = 1 + ownID[1];
		printf("LOADIPC ownId[0] = %d , ownID[1] = %d \n",ownID[0],ownID[1]);
     if((hashTable = hashCreateTable(10, freeIPCID, compareIPCIDs, copyIPCID)) == NULL){
		fprintf(stderr,"Error en crear la tabla de hash en loadIPC\n");
		return -1;
	}
        
	itoa(getppid(), pidString);
	if((hashInsert(&hashTable, ownID, pidString, 0)) == NULL){
		fprintf(stderr,"Error en insertar una entry en la tabla de hash en loadIpc con pidString = %s\n", pidString);
		return -1;
	}
       
        entry.mtype = ownID[1];
        pid = getpid();
        memcpy(entry.mtext, &pid, sizeof(pid_t));

		printf("LOADIPC: queue_id = %d,Numero de escritura = %d, Mtext %d,\n",queue_id, ownID[1], pid);

        if((mlen = msgsnd(queue_id, &entry, sizeof(pid_t), 0)) == -1){
		perror("msgsnd fallo");        
		return errno;
        }
       	printf("LOADIPC: SAlio del send\n");

        while (!flag){
			sigsuspend (&oldmask);
        }              
		sigprocmask (SIG_UNBLOCK, &mask, NULL);

		
		close(_stdin_);
       
        return 0;
}


int readIPC(pid_t pid, void *buffer, int bufferSize){
        int mlen, *ipcID;
        unsigned int hkey;
        msQ entry;
        char pidString[10];
        itoa(pid, pidString);

        if((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
			fprintf(stderr,"Error en buscar una entry en la tabla de hash en readIPC con pidString = %s\n", pidString);
			return -1;
		}
		printf("READIPC: queue_id = %d,Numero con el que voy a leer = %d, %s, bufferSize %d\n",queue_id, ipcID[0], pidString, bufferSize);

        if(msLastRead == NULL){
                if((mlen = msgrcv(queue_id ,&entry, bufferSize, ipcID[0], MSG_NOERROR)) == -1){
                        printf("FALLOP: queue_id = %d,Numero con el que voy a leer = %d, %s, bufferSize %d\n",queue_id, ipcID[0], pidString, bufferSize);
						printf("Entry = mtype %d y mtext%s \n",entry.mtype, entry.mtext);
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
			fprintf(stderr,"Error en buscar una entry en la tabla de hash en writeIPC con pidString = %s\n", pidString);
			return -1;
	
		}
		entry.mtype = ipcID[1];
        memcpy(entry.mtext, buffer, bufferSize);
		printf("WRITEIPC: queue_id = %d,Numero con el que voy a escribir = %d, %s\n",queue_id, ipcID[1], pidString); 

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
       
        sleep(seconds);
        msgctl(queue_id, IPC_STAT, &msq_stat);
        return msq_stat.msg_qnum;
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
        if((ipcID = hashSearch(hashTable, pidString, &hkey)) == NULL){
			fprintf(stderr,"Error en hashSearch en getIPCStatus, invocado con Pid = %s\n",pidString);
			return -1;
		}
       
        if((mlen = msgrcv(queue_id ,entry, bufferSize, ipcID[0],IPC_NOWAIT)) == -1){
			free(entry);
			return FALSE;          
        }else{
			msLastRead = entry;
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
