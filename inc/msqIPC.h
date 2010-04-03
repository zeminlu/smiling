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
 * \def
 * 		Description here
 */
#define QKEY	(key_t)0105

/**
 * \def
 * 		Description here
 */
#define QPERM	0666

/**
 * \struct
 * 		Description here
 */
typedef struct q_entry{
	long mtype;
	char mtext[200];
	}msQ;

/**
 * \fn
 *
 * 		\brief
 *
 * 		\param
 * 		
 * 		\return
 *
 * 		Use:
 * 		\code
 * 		
 *		\endcode
 *
 * 		\sa
 *
 */
int init_queue(int newKey);

#endif
