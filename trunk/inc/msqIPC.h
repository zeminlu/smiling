/*
 * \file msqIPC.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef MSQIPC_H_
#define MSQIPC_H_

#include <sys/ipc.h>
#include <sys/msg.h>

#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"
#include "../inc/linearHashADT.h"

/**
 * \def QPERM
 * 		\brief Definition of the permissions of the message queue.
 */
#define QPERM	0666

/**
 * \struct msQ
 * 		\brief A struct representing the package being sent and recieved through the message queue.
 */
typedef struct{
	long mtype;
	char mtext[200];
	}msQ;

/**
 * \fn int init_queue(int newKey)
 *
 * 		\brief Creates a message queue based on the given key.
 *
 * 		\param newKey The key to initialize the message queue.
 * 		
 * 		\return 0 on succes, < 0 on error.
 *
 */
int init_queue(int newKey);

#endif
