/*
 * \file parallel.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef PARALLEL_H_
#define PARALLEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../inc/types.h"
#include "../inc/definitions.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"
#include "../inc/linearHashADT.h"

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
int filesListener();

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
int processFile(DIR *dp, country ***countriesTable);

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
int getFilesAmm (DIR *dp);

#endif
