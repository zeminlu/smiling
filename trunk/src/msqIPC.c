
#include "../inc/msqIPC.h"

int queue_idR = 0, queue_idW = 0;
int clientsAmm = 0, flag = FALSE;
int info;

void sigHandler (int signum){
	flag = TRUE;
	return;
}

int init_queue(int newKey){
	int queue_id;
	int key;
	
	key = getpid();
	queue_id = msgget((key_t)(key % newKey), IPC_CREAT | QPERM);
	if(queue_id == -1){
		perror("msgget Fallo");
		return errno;
	} 
	return queue_id;
}

int setupIPC(int channels){
	int i;
	int data;
	
	queue_idR = init_queue(1);
	queue_idW = init_queue(2);
	data = open("./Luchano", O_WRONLY | O_CREAT, 0644);
		
	for (i = 0 ; i < channels ; ++i){
		write(data, &queue_idR, sizeof(int));
		write(data, &queue_idW, sizeof(int));
	}
	
	clientsAmm = channels;
	close(data);
	info = open("./Luchano", O_RDONLY);
	return 0;
}

int addClient(){
	return dup2(info, 0);
}

int synchronize(){
	int i = 0;
	pid_t aux= 0;
	pid_t * pid = NULL;
	
	pid = malloc(sizeof(pid_t)* clientsAmm);
	printf("MSQAPI: antes del for que lee\n");
	for (i = 0 ; i < clientsAmm ; ++i){
		printf("MSQAPI: %d\n", i);
		readIPC(0, &aux, sizeof(pid_t));
		printf("MSQAPI: %d\n", aux);
		pid[i]= aux;
	}
	printf("MSQAPI: antes del for que lee\n");	
	for (i = 0 ; i < clientsAmm ; ++i){
		kill (pid[i], SIGALRM);
	}
	close(info);
	unlink("./Luchano");
	
	return 0;
}

int loadIPC(){
	sigset_t mask, oldmask;
	int queue_id;
	pid_t pid;
	
	pid = getpid();
	signal(SIGALRM, sigHandler);
	sigemptyset (&mask);
	sigaddset (&mask, SIGALRM);

	
	read(_stdin_, &(queue_id), sizeof(int));
	queue_idW = queue_id;
	printf("LOADIPC: %d\n", queue_id);
	
	read(_stdin_, &(queue_id), sizeof(int));
	queue_idR = queue_id;
	printf("LOADIPC: %d\n", queue_id);
	
	writeIPC(getppid(), &pid, sizeof(pid_t));
		
    while (!flag){
    	sigsuspend (&oldmask);
	}
    sigprocmask (SIG_UNBLOCK, &mask, NULL);
	
	return 0;
}

int readIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen;
	msQ entry;
	entry.msg = buffer;
	if((mlen = msgrcv(queue_idR ,&entry, bufferSize, pid, MSG_NOERROR)) == -1){
		perror("msgrcv fallo");
		return errno;
	}
	if(mlen == -1){
		buffer = NULL;
		return -2;
	}
return 0;
}

int writeIPC(pid_t pid, void *buffer, int bufferSize){
	int mlen;
	msQ entry;
	entry.msg = malloc(sizeof(char)* bufferSize);
	entry.mtype = (long)pid;
	memcpy(entry.msg, buffer, bufferSize);
	if((mlen = msgsnd(queue_idW,&entry, bufferSize, 0)) == -1){
		perror("msgsnd fallo");
		return errno;
	}
	
return 0;
}

int closeIPC(int pid){
	
	return 0;
}

int finalizeIPC(){
 	msgctl(queue_idR, IPC_RMID, NULL);
	msgctl(queue_idW, IPC_RMID, NULL);
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
