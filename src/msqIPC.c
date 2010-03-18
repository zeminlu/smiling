#include "../inc/ipcAPI.h"
#include "../inc/msqIPC.h"

clients **clientsL = NULL;
servers **serversL = NULL;
int * freeClient = NULL; 
int freeClientAmm = 0;
int clientAmm =0;

int initIPC(int serverPid){
	int queue_ID =0;
	if((queue_ID = msgget(QKEY, IPCCREAT | QPERM)) == -1){
		perror("Fallo la creacion de msgget");
		return errno;
	}
	serversL = malloc(sizeof(void *));
	serversL->clientList = NULL;
	serversL[0]->pid = serverPid;
	serversL[0]->clientAmm = 0;
	serversL[0]->queue = queue_ID;
	return 0;
}

int readIPC(int listenID, int sourceID, void *buffer, int bufferSize){
	msgQ msg;
	msg.mText = buffer;
	
	((serversL[sourceID])->clientList)[listenID].talk = FALSE;
	if(msgrcv(serversL[sourceID]->queue_ID, &msg,bufferSize,listenID, MSG_NOERROR )== -1){
		perror("El mensaje no se pudo recibir");
		return errno;
	}
	return TRUE;
	
}


int writeIPC(int sendID,int rcvId ,void *buffer, int bufferSize){
	
	msgQ msg;
	
	msg.mType = (long)rcvId;
	msg.mText = buffer;
	
	((serversL[rcvID])->clientList)[sendID].talk = TRUE;
	
	if(msgsnd(serversL[rcvID]->queue_ID, &msg,bufferSize, 0 )== -1){
		perror("El mensaje no se pudo enviar");
		return -1;
	}
	return TRUE;
}

int closeIPC(int clientID){
	if(clientID < clientAmm && clientID > 0){
		freeClientAmm--;
		freeClient = realloc(freeClient, sizeof(int)* freeClientAmm);
		return TRUE;
	}else{
		return FALSE;
	}
}

int selectIPC(int serverID, int seconds){
/* devuelve True si hay que escuchar algun cliente*/
	int i = 0;
	sleep(seconds);
	for(i = 0; i < clientAmm; ++i){
		if(((serversL[serverID])->clientList)[i] == TRUE){
			return TRUE;
		}
	}
	return FALSE;
}

int getIPCStatus(int clientID, int serverID){
	/*devuelve true si el cliente consulto a ese servidor*/
	return ((serversL[serverID])->clientList)[clientID];
}

int addCliente(int clientPid, int serverID){
	int index;
	
	if(freeClient == NULL){
		clientsL = realloc(clientsL ,sizeof(void*)*(clientAmm+1));
		index = clientAmm;
	}else{
		index = freeClient[--freeClientAmm];
		if(freeClientAmm == 0){
			free(freeClient);
			freeClient = NULL;
		}else{
			freeClient = realloc(freeClient, sizeof(int)* freeClienAmm);
		}
		
	}
	clientsL[index]= malloc(sizeof(clients));
	clientsL[index]->writeID = serverID;
	clientsL[index]->pid = clientPid;
	clientsL[index]->queue_ID = serversL[0]->queue_ID;
	clientsL[index]->talk = FALSE;
	clientAmm ++;
	return index;
	
	/*almaceno el cliente en mi talba interna , y devuelvo el Id asociado al cliente creado*/
}