/*
 * \file grouph.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef GROUPH_H_
#define GROUPH_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#include "../inc/definitions.h"
#include "../inc/conditions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"
#include "../inc/serializable.h"
#include "../inc/ipcAPI.h"

/**
 * \fn void sendErrorToParent(int error)
 *
 * 		\brief This function sends the given error to the calling proccess' parent through the ipcAPI
 *
 * 		\param error The error number to be sent.
 *
 */
void sendErrorToParent(int error);

/**
 * \fn int loadHeadAndCountriesTable(country ***countriesTable, country **head)
 *
 * 		\brief This function loads the countries table and the head country struct being recieved by the calling process' parent.
 *
 * 		\param countriesTable A pointer to the country ** where the table will be stored.
 *		\param head A pointer to a country * where the head country struct will be stored.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 */
int loadHeadAndCountriesTable(country ***countriesTable, country **head);

/**
 * \fn int checkConditions(country *data, void *(***conditions)(void *condArgs))
 *
 * 		\brief This function checks the conditions set in the given data country struct and assigns the corresponding function pointers in the given conditions argument.
 *
 * 		\param data The address of the country struct to be checked.
 *		\param conditions The address of a condition functions array to be filled.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 */
int checkConditions(country *data, void *(***conditions)(void *condArgs));

/**
 * \fn int buildCondArgs(condPack **condArgs, country **countriesTable, int countriesTableEntriesAmm, country *data)
 *
 * 		\brief This function builds the given condPack parameter from the remaining parameters. 
 *
 * 		\param condArgs The address of the condPack's array.
 *		\param countriesTable The address countries table.
 *		\param countriesTableEntriesAmm The ammount of countries in the countries table.
 * 		\param data The address of the country head struct.
 *
 * 		\return 0 on success, < 0 on error.
 *
 */
int buildCondArgs(condPack **condArgs, country **countriesTable, int countriesTableEntriesAmm, country *data);

/**
 * \fn int prepareGroup(subFixture **group, country *data)
 *
 * 		\brief This function prepares a subFixture array where the data country head and the 3 remaining countries will be stored.
 *
 * 		\param group The address of the subFixture array.
 *		\param data The address of the country head struct.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 */
int prepareGroup(subFixture **group, country *data);

/**
 * \fn int buildSubfixture(subFixture **group, int condAmm, condPack *condArgs, country *data, country **countriesTable, void *(**conditions)(void *condArgs))
 *
 * 		\brief This function builds the subFixture that will be sent later to the calling process' parent.
 *
 * 		\param group The address of the subFixture array that this function will build.
 *		\param condAmm The ammount of conditions to be processed for data.
 *		\param condArgs The address of the condPack that the condition functions will recieve.
 *		\param data The country head struct.
 *		\param countriesTable The countries table.
 *		\param conditions The address of the condition functions array.
 *
 * 		\return 0 on success, < 0 on error.
 *
 * 		\sa intersect() sortPointers()
 *
 */
int buildSubfixture(subFixture **group, int condAmm, condPack *condArgs, country *data, country **countriesTable, void *(**conditions)(void *condArgs));

/**
 * \fn int intersect(int condAmm, condPack *condArgs, set **intersection)
 *
 * 		\brief This function intersects the different country structs stored in the condPack * parameter and stores in the set ** parameter.
 *
 * 		\param condAmm The ammount of conditions that where processed.
 *		\param condArgs The address of the condPack struct where the different country structs are stored.
 *		\param intersection The address of the set array where the intersection will be stored.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		\sa buildSubfixture()
 *
 */
int intersect(int condAmm, condPack *condArgs, set **intersection);

/**
 * \fn int sendSubfixture(subFixture *group)
 *
 * 		\brief This function sends the given subFixture to the calling process' parent.
 *
 * 		\param group The address of the subFixture struct to be sent.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 */
int sendSubfixture(subFixture *group);

/**
 * \fn void sortPointers(set **sets)
 *
 * 		\brief This function sorts the set pointers stored in sets no to have NULL pointers between them.
 *
 * 		\param sets The adrees of the set array.
 * 		
 * 		\sa buildSubfixture()
 *
 */
void sortPointers(set **sets);

#endif
