/**
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
 * \fn int loadCountriesTable(country ***countriesTable)
 *
 * 		\brief This function loads the countriesTable being passed from the calling process' parent.
 *
 * 		\param countriesTable A pointer to a country ** where to store the table.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 */
int loadCountriesTable(country ***countriesTable);

/**
 * \fn int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids)
 *
 * 		\brief This function starts the grouph processes in order to build as many subfixture as needed, from countriesTable into fixture, storing the grouph's pids in pid numbers.
 *
 * 		\param countriesTable The countries table.
 *		\param countriesTableEntriesAmm The ammount of countries in the table
 *		\param fixture A pointer to a country *** variable where the grouph processes' subfixtures will later be stored.
 *		\param pids A pid_t ** to store the child processes' pid numbers.
 * 		
 * 		\return 0 on succes, < 0 on error.
 *
 * 		\sa childsListener()
 *
 */
int startChildProcesses(country **countriesTable, int countriesTableEntriesAmm, country ****fixture, pid_t **pids);

/**
 * \fn int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture)
 *
 * 		\brief This function acts as a server for the grouph processes' sterted by startChildProcesses function.
 *
 * 		\param pids The pid_t * where the grouph processes' pid numbers are stored.
 *		\param countriesTable The countries table.
 *		\param countriesTableEntriesAmm The ammount of countries in the table.
 *		\param fixture a country *** where the subfixtures built by the grouph processes will be stored.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		\sa startChildProcesses()
 *
 */
int childsListener(pid_t *pids, country **countriesTable, int countriesTableEntriesAmm, country ***fixture);

#endif
