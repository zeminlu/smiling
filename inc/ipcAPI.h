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
 * \fn int setupIPC(int channels)
 *
 * 		\brief This function prepares channels comunication channels to be used between the calling process and channels direct child processes.
 *
 * 		\param channels The ammount of comunication channels needed.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int j, status
 *
 *		if ((status = setupIPC(childProcessesAmmount)) < 0){
 *			return status;
 *		}
 *
 * 		for (j = 0; j < childProcessesAmmount ; ++j){
 *			switch((fork())){
 *				case -1:
 *					perror("Fork erro");
 *					return errno;
 *					break;
 *				case 0:
 *					addClient(j);
 *					execv("./childProcess", NULL);
 *					break;
 *				default:					
 *					break;
 *			}
 *		}
 *		
 *		if ((status = synchronize()) < 0){
 * 			return status;
 * 		}
 *		\endcode
 *
 * 		\sa addClient() synchronize() loadIPC()
 *
 */
int setupIPC(int channels);

/**
 * \fn int addClient(int index)
 *
 * 		\brief This function sets up needed stuff so that the calling process can later comunicate with it's parent. Must be called as the first instruction after the return of a fork in the child portion of the switch and before the execv() call.
 *
 * 		\param index The index, from 0 to the channels ammount passed to setupIPC. Only one call to addClient for a given index must be done. 
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int j, status
 *
 *		if ((status = setupIPC(childProcessesAmmount)) < 0){
 *			return status;
 *		}
 *
 * 		for (j = 0; j < childProcessesAmmount ; ++j){
 *			switch((fork())){
 *				case -1:
 *					perror("Fork erro");
 *					return errno;
 *					break;
 *				case 0:
 *					addClient(j);
 *					execv("./childProcess", NULL);
 *					break;
 *				default:					
 *					break;
 *			}
 *		}
 *		
 *		if ((status = synchronize()) < 0){
 * 			return status;
 * 		}
 *		\endcode
 *
 * 		\sa setupIPC() synchronize() loadIPC()
 *
 */
int addClient(int index);

/**
 * \fn int synchronize()
 *
 * 		\brief This function completes the communication setup between the calling process and it's child processes. Must be called reight after the fork return of the parent.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int j, status
 *
 *		if ((status = setupIPC(childProcessesAmmount)) < 0){
 *			return status;
 *		}
 *
 * 		for (j = 0; j < childProcessesAmmount ; ++j){
 *			switch((fork())){
 *				case -1:
 *					perror("Fork erro");
 *					return errno;
 *					break;
 *				case 0:
 *					addClient(j);
 *					execv("./childProcess", NULL);
 *					break;
 *				default:					
 *					break;
 *			}
 *		}
 *		
 *		if ((status = synchronize()) < 0){
 * 			return status;
 * 		}
 *		\endcode
 *
 * 		\sa setupIPC() addClient() loadIPC()
 *
 */
int synchronize();

/**
 * \fn int loadIPC()
 *
 * 		\brief This function completes the communication setup between the calling process and it's parent process. Must be called as the first instruction of the process invocated with execv().
 *
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int j, status
 *
 *		if ((status = setupIPC(childProcessesAmmount)) < 0){
 *			return status;
 *		}
 *
 * 		for (j = 0; j < childProcessesAmmount ; ++j){
 *			switch((fork())){
 *				case -1:
 *					perror("Fork erro");
 *					return errno;
 *					break;
 *				case 0:
 *					addClient(j);
 *					execv("./childProcess", NULL);
 *					break;
 *				default:					
 *					break;
 *			}
 *		}
 *		
 *		if ((status = synchronize()) < 0){
 * 			return status;
 * 		}
 *
 *      //As first instruction of "childProcess", loadIPC() must be called.//
 *
 *		\endcode
 *
 * 		\sa setupIPC() addClient() synchronize()
 *
 */
int loadIPC();

/**
 * \fn int readIPC(pid_t pid, void *buffer, int bufferSize)
 *
 * 		\brief This function reads bufferSize bytes from the communication channel between the calling process and the pid process, and stores it in buffer, which has to have bufferSize memory allocated.
 *
 * 		\param pid The pid number of the process from who the calling process wants read data.
 * 		\param buffer The buffer where to store the read bytes.
 *		\param bufferSize The ammount of bytes to read.		
 *
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 * 		void *buffer;
 * 		pid_t pids[childProcessAmm];
 * 		int j;
 *	
 *		if ((buffer = malloc(sizeof(int))) == NULL){
 * 			perror("Malloc error");
 *			return errno;
 *		}
 *
 *		while(selectIPC(2) > 0 ){
 *			for (j = 0 ; j < childProcessAmm ; ++j){
 *				if (getIPCStatus(pids[j])){
 *					if (readIPC(pids[j], &buffer, sizeof(int)) != 0){
 *						return -1;
 *					}
 *					//Do something with buffer
 *				}
 *			}
 *		}
 *
 *		free(buffer);
 *		\endcode
 *
 * 		\sa writeIPC() selectIPC() getIPCStatus()
 *
 */
int readIPC(pid_t pid, void *buffer, int bufferSize);

/**
 * \fn int writeIPC(pid_t pid, void *buffer, int bufferSize)
 *
 * 		\brief This function writes bufferSize bytes to the communication channel between the calling process and the pid process, taken from the given buffer.
 *
 * 		\param pid The pid number of the process to who the calling process wants to write data.
 *		\param buffer The buffer where from where the data will be taken to write.
 * 		\param bufferSize The ammount of bytes to be written.
 *
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 * 		void *buffer;
 * 		pid_t pids[childProcessAmm];
 * 		int j;
 *	
 *		if ((buffer = malloc(sizeof(int))) == NULL){
 * 			perror("Malloc error");
 *			return errno;
 *		}
 *
 *		while(selectIPC(2) > 0 ){
 *			for (j = 0 ; j < childProcessAmm ; ++j){
 *				if (getIPCStatus(pids[j])){
 *					if (readIPC(pids[j], &buffer, sizeof(int)) != 0){
 *						return -1;
 *					}
 *					//Do something with buffer
 *				}
 *			}
 *		}
 *
 *		free(buffer);
 *		\endcode
 *
 * 		\sa readIPC() selectIPC() getIPCStatus()
 *
 */
int writeIPC(pid_t pid, void *buffer, int bufferSize);

/**
 * \fn int selectIPC(int seconds)
 *
 * 		\brief This blocking function checks if at least one child process of the calling process has written something to to him that hasn't been read yet.
 *
 * 		\param seconds The timeout for the selectIPC().
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 * 		void *buffer;
 * 		pid_t pids[childProcessAmm];
 * 		int j;
 *	
 *		if ((buffer = malloc(sizeof(int))) == NULL){
 * 			perror("Malloc error");
 *			return errno;
 *		}
 *
 *		while(selectIPC(2) > 0 ){
 *			for (j = 0 ; j < childProcessAmm ; ++j){
 *				if (getIPCStatus(pids[j])){
 *					if (readIPC(pids[j], &buffer, sizeof(int)) != 0){
 *						return -1;
 *					}
 *					//Do something with buffer
 *				}
 *			}
 *		}
 *
 *		free(buffer);
 *		\endcode
 *
 * 		\sa readIPC() writeIPC() getIPCStatus()
 *
 */
int selectIPC(int seconds);

/**
 * \fn int getIPCStatus(pid_t pid)
 *
 * 		\brief This functions checks wether the given pid child process has written something to the calling process that hasn't been read yet.
 *
 * 		\param pid The pid number from the child process to be checked.
 * 		
 * 		\return TRUE if there's something to be read, FALSE if not.
 *
 * 		Use:
 * 		\code
 * 		void *buffer;
 * 		pid_t pids[childProcessAmm];
 * 		int j;
 *	
 *		if ((buffer = malloc(sizeof(int))) == NULL){
 * 			perror("Malloc error");
 *			return errno;
 *		}
 *
 *		while(selectIPC(2) > 0 ){
 *			for (j = 0 ; j < childProcessAmm ; ++j){
 *				if (getIPCStatus(pids[j])){
 *					if (readIPC(pids[j], &buffer, sizeof(int)) != 0){
 *						return -1;
 *					}
 *					//Do something with buffer
 *				}
 *			}
 *		}
 *
 *		free(buffer);
 *		\endcode
 *
 * 		\sa readIPC() writeIPC() selectIPC()
 *
 */
int getIPCStatus(pid_t pid);

/**
 * \fn int closeIPC()
 *
 * 		\brief This function closes the communication between the calling process and it's parent process.
 *
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int status;
 *
 *		if ((status = loadIPC()) < 0){
 *			return status;
 *		}
 *
 *		//Communicate with parent// 		
 *
 *		closeIPC();
 *
 *		\endcode
 *
 * 		\sa loadIPC()
 *
 */
int closeIPC();

/**
 * \fn int finalizeIPC(void)
 *
 * 		\brief This function finalizes all the communication channels between the calling process and it's child processes that were set up to communicate with him.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 *		int j, status
 *
 *		if ((status = setupIPC(childProcessesAmmount)) < 0){
 *			return status;
 *		}
 *
 * 		for (j = 0; j < childProcessesAmmount ; ++j){
 *			switch((fork())){
 *				case -1:
 *					perror("Fork erro");
 *					return errno;
 *					break;
 *				case 0:
 *					addClient(j);
 *					execv("./childProcess", NULL);
 *					break;
 *				default:					
 *					break;
 *			}
 *		}
 *		
 *		if ((status = synchronize()) < 0){
 * 			return status;
 * 		}
 *
 *		//Comunicate with child processes//
 *
 *		if ((status = finalizeIPC()) < 0){
 *			return status;
 *		}
 *		\endcode
 *
 * 		\sa setupIPC() synchronize() addClient()
 *
 */
int finalizeIPC(void);

#endif