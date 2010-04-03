/*
 * \file
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef _SERIALIZABLE
#define _SERIALIZABLE_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/definitions.h"
#include "../inc/types.h"
#include "../inc/tpl.h"

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
int serializeCountryStruct(void **buffer, int *bufferSize, country *country);

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
int unserializeCountryStruct(void *buffer, int bufferSize, country *country);

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
int serializeInteger(void **buffer, int *bufferSize, int number);

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
int unserializeInteger(void *buffer, int bufferSize);

#endif
