/*
 * \file shmIPC.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef SHMIPC_H_
#define SHMIPC_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/shm.h>
#include <semaphore.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/linearHashADT.h"
#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"

/**
 * \def _SHM_SEG_SIZE_
 * 		\brief Description here
 */
#define	_SHM_SEG_SIZE_ 4000

/**
 * \def ERR
 * 		\brief Description here
 */
#define ERR ((struct databuf *) -1)

/**
 * \struct shmHeader
 * 		\brief Description here
 */
typedef struct{
	pid_t pid;
	int sReadOff;
	int sWriteOff;
	int cReadOff;
	int cWriteOff;
} shmHeader;

/**
 * \struct shmStruct
 * 		\brief Description here
 */
typedef struct{
	shmHeader header;
	char bufA[_SHM_SEG_SIZE_];
	char bufB[_SHM_SEG_SIZE_];
} shmStruct;

/**
 * \struct shmElem
 * 		\brief Description here
 */
typedef struct{
	int index;
	char *readBuf;
	char *writeBuf;
	int *read;
	int *write;
	int *otherRead;
	int *otherWrite;
	sem_t *semA;
	sem_t *semB;
} shmElem;

/**
 * \fn sem_t * initmutex(char *semName)
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
sem_t * initmutex(char *semName);

/**
 * \fn int initShMem(int newKey)
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
int initShMem(int newKey);

/**
 * \fn int initSem(int pid, shmElem *aux)
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
int initSem(int pid, shmElem *aux);

/**
 * \fn int initHeaders()
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
int initHeaders();

#endif