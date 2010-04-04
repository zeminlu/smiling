/**
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
 * \fn int filesListener()
 *
 * 		\brief This function stays in a loop listening for incoming files in the parallel directory, starts a fifa process for each one and passes the file data to it.
 * 		
 * 		\return 0 if everything went ok, < 0 if not.
 *
 * 		\sa processFile() getFilesAmm()
 *
 */
int filesListener();

/**
 * \fn int processFile(DIR *dp, country ***countriesTable)
 *
 * 		\brief This function processes the first valid .paises file in the DIR directory and builds the needed struct with the file data.
 *
 * 		\param dp The directory where to search for a file.
 *		\param countriesTable A pointer to a country **countriesTable to be filled with the file data. 	
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		\sa filesListener()
 *
 */
int processFile(DIR *dp, country ***countriesTable);

/**
 * \fn int getFilesAmm (DIR *dp)
 *
 * 		\brief This function checks the ammount of files in the given DIR directory.
 *
 * 		\param dp A pointer to a DIR directory
 * 		
 * 		\return The ammount of files in the given directory.
 *
 * 		Use:
 * 		\code
 * 			DIR *dp;
 *			int filesAmm;
 *			
 *					...
 *			dp initialization
 *					...
 *
 *			filesAmm = getFilesAmm(dp);
 *
 *		\endcode
 *
 * 		\sa filesListener()
 *
 */
int getFilesAmm (DIR *dp);

#endif
