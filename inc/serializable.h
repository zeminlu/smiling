/*
 * \file serializable.h
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
 * \fn int serializeCountryStruct(void **buffer, int *bufferSize, country *country)
 *
 * 		\brief This function serializes (using the TPL library) the given country into the given buffer, allocating the needed memory in it, and setting that ammount of bytes in bufferSize
 *
 * 		\param buffer The buffer where to store the serialization.
 *		\param bufferSize The int * where to store the serialization size.
 *		\param country A pointer to the country struct to be serialized.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 * 			country data;
 *			int bufferSize;
 *			void *buffer;
 *
 *					...
 *			data inizialization
 * 					...
 *
 *			serializeCountryStruct(&buffer, &bufferSize, &data);
 *		\endcode
 *
 * 		\sa unserializeCountryStruct()
 *
 */
int serializeCountryStruct(void **buffer, int *bufferSize, country *country);

/**
 * \fn int unserializeCountryStruct(void *buffer, int bufferSize, country *country)
 *
 * 		\brief This function unserializes (using the TPL library) a previously serialized country struct on the buffer parameter, and stores it in the given country struct. 
 *
 * 		\param buffer The buffer where the serialized country struct is stored.
 *		\param bufferSize The size of the serializated country struct.
 *		\param country The address of the country struct where to store the unserialization.
 * 		
 * 		\return 0 on success, < 0 on error.
 *
 * 		Use:
 * 		\code
 * 		country data;
 *		
 *		unserializeCountryStruct(buffer, bufferSize, &country);
 *		\endcode
 *
 * 		\sa serializeCountryStruct()
 *
 */
int unserializeCountryStruct(void *buffer, int bufferSize, country *country);

#endif
