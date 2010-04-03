/*
 * \file ipcAPI.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef IPCAPI_H_
#define IPCAPI_H_

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

#include "../inc/definitions.h"
#include "../inc/types.h"

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
int setupIPC(int channels);

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
int addClient(int index);

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
int synchronize();

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
int loadIPC();

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
int readIPC(pid_t pid, void *buffer, int bufferSize);

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
int writeIPC(pid_t pid, void *buffer, int bufferSize);

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
int selectIPC(int seconds);

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
int getIPCStatus(pid_t pid);

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
int closeIPC();

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
int finalizeIPC(void);

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
int compareIPCIDs(void *elem1, void *elem2);

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
void * copyIPCID(void *elem);

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
void freeIPCID(void *elem);

#endif
