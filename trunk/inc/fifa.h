/*
 * \file fifa.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef FIFA_H_
#define FIFA_H_

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
#include <sys/wait.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"
#include "../inc/cutils.h"

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
int loadCountriesTable(country ***countriesTable);

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
int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids);

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
int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture);

#endif