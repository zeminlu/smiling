/*
 * \file linearHashADT.h
 *
 *  \date Created on: 29-oct-2008
 *  \author Luciano R. Zemin
 */

#ifndef LINEARHASHADT_H_
#define LINEARHASHADT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../inc/definitions.h"
#include "../inc/functionTypes.h"

/**
 * \var hashTableADT
 *
 * 		\brief Typedef of the hashTableCDT pointer.
 */
typedef struct hashTableCDT *hashTableADT;

/**
 * \var hashTableElement
 *
 * 		\brief Typedef of the hashTableElement referencing to a void *
 */
typedef void *hashTableElement;

/**
 * \var compFuncT
 *
 * 		\brief Typedef of the prototype of a function that converts inner table elements.
 */
typedef hashTableElement (*hashConvFuncT)(char *aux, hashTableElement, char **key);

/**
 * \fn void hashSetIterator(hashTableADT table)
 *
 * 		\brief This function initializes the iterator in a hash table.
 *
 * 		\param table An instance of hashTableADT.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 *
 * 					...
 * 			initialization of table
 * 					...
 *
 * 			hashSetIterator(table);
 * 		\endcode
 *
 * 		\sa hashGetNext()
 */
void hashSetIterator(hashTableADT table);

/**
 * \fn hashTableElement hashGetNext(hashTableADT table)
 *
 * 		\brief This function gets the next element from a hash table trough it`s iteration.
 *
 * 		\param table An instance of hashTableADT.
 *
 * 		\return The iteration's next hash table element
 *
 * 		Use:
 * 		\code
 * 		hashTableADT table;
 * 		hashTableElement elem;
 *
 * 				...
 * 		initialization of table
 * 				...
 *
 * 		hashSetIterator(table);
 * 		elem = hashGetNext(table);
 * 		\endcode
 *
 * 		\sa hashSetIterator
 */
hashTableElement hashGetNext(hashTableADT table);

/**
 * \fn unsigned int hash(const char *key, int nbuckets)
 *
 * 		\brief This function hashes the given string.
 *
 * 		\param key The string to hash.
 * 		\param nbuckets The amount of buckets to operate.
 *
 * 		\return The hashed key.
 *
 * 		Use:
 * 		\code
 * 			char *string = "HI";
 * 			unsigned int key;
 *
 * 			key = hash(string);
 * 		\endcode
 *
 */
unsigned int hash(const char *key, int nbuckets);

/**
 * \fn hashTableADT hashCreateTable(int nbuckets, freeFuncT freeElement, compFuncT compElement, copyFuncT copyElement)
 *
 * 		\brief This function creates a new instance of hashTableADT.
 *
 * 		\param nbuckets Initial size of table.
 * 		\param freeElement Function to free allocated memory of a hashTableElement.
 * 		\param compElement Function to compare two instances hashTableElement.
 * 		\param copyElement Function to copy a hashTableElement.
 *
 * 		\return An instance of hashTableADT, NULL if there was an error.
 *
 * 		Use:
 * 		\code
 * 			int n = 10;
 * 			hashTableADT table;
 *
 * 			return table = hashCreateTable(n));
 * 		\endcode
 *
 */
hashTableADT hashCreateTable(int nbuckets, freeFuncT freeElement, compFuncT compElement, copyFuncT copyElement);

/**
 * \fn void hashFreeTable(hashTableADT table)
 *
 * 		\brief This function frees up the memory allocated for the given table.
 *
 * 		\param table The table to be freed.
 *
 * 		Use:
 * 		\code
 * 			hashFreeTable(table);
 * 		\endcode
 *
 */
void hashFreeTable(hashTableADT table);

/**
 * \fn int hashCompareKey(hashTableElement elem1, hashTableElement elem2)
 *
 * 		\brief This function compares two table elements by their keys.
 *
 * 		\param elem1 One of the elements to compare.
 * 		\param elem2 Idem elem1.
 *
 * 		\return 0 if equal, < 0 if key1 is lower than key2, > 0 otherwise.
 *
 * 		Use:
 * 		\code
 * 			if (hashCompareKey(elem1, elem2) == 0)
 * 				printf("The elements are equal");
 * 			else
 * 				printf("The elements are different");
 * 		\endcode
 *
 */
int hashCompareKey(hashTableElement key1, hashTableElement key2);

/**
 * \fn hashTableElement hashInsert(hashTableADT *table, hashTableElement elem, const char *key, int update)
 *
 * 		\brief This function inserts an element into a hash table.
 *
 * 		\param table An instance of hashTableADT.
 * 		\param elem	The element to be inserted.
 * 		\param key The key to be hashed.
 * 		\param update TRUE if the element has to be updated in the table, FALSE if not.
 *
 * 		\return NULL if there was an error, the element if not.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 * 			hashTableElement elem;
 *
 * 			if (hashInsert(table, elem, FALSE) == NULL)
 * 				printf("The element could not be inserted");
 * 		\endcode
 *
 * 		\sa hashDelete() hashSearch()
 *
 */
hashTableElement hashInsert(hashTableADT *table, hashTableElement elem, const char *key, int update);

/**
 * \fn hashTableElement hashSearch(hashTableADT table, const char *key, unsigned int *ptr)
 *
 * 		\brief This function searches an element in a hash table by the given key.
 *
 * 		\param table An instance of hashTableADT.
 * 		\param key The key to search the element.
 * 		\param ptr Adress of unsigned int where to save the hashed key of the element searched.
 *
 * 		\return The element or NULL if it's not in the table.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 * 			char *key;
 *			unsigned int hkey;
 * 						...
 * 			initialization of table & key
 * 						...
 *
 * 			if (hashSearch(&table, key, &hkey) == NULL)
 * 				printf("The element is not in the table");
 * 		\endcode
 *
 *		\sa hashInsert() hashDelete()
 */
hashTableElement hashSearch(hashTableADT table, const char *key, unsigned int *ptr);

/**
 * \fn hashTableElement hashDelete(hashTableADT *table, const char *key)
 *
 * 		\brief This function erases an element from the table by the given key.
 *
 * 		\param table An instance of hashTableADT different from NULL.
 * 		\param key The key of the element to be deleted.
 *
 * 		\return The element if there were no errors, NULL instead.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 * 			char *key;
 *
 * 						...
 * 			initialization of table & key
 * 						...
 *
 * 			if (hashDelete(&table, key) == NULL)
 * 				printf("The element could not be deleted");
 * 		\endcode
 */
const char * hashDelete(hashTableADT *table, const char *key);

/**
 * \fn hashTableADT * hashReadFromFile(hashTableADT *table, char *filename, freeFuncT freeElement, compFuncT compareElement, copyFuncT copyElement, hashConvFuncT hashConvElement)
 *
 * 		\brief This function reads a hash table stored on a given file and builds it on volatile memory.
 *
 * 		\param table The hash table where to store the read information.
 * 		\param filename The name of the file to be read.
 * 		\param freeElement The function to free an element of the table.
 * 		\param compareElement The function to compare two table elements.
 * 		\param copyElement The function to make a copy of a table element.
 * 		\param hashConvElement The function that converts each line of the file into a hash element.
 *
 * 		\return The hash built hashTableADT instance, NULL if there were any errors.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 *
 * 			if (hashReadFromFile(&table, filename, freeElement, compareElement, copyElement, hashConvElement) == NULL)
 * 				printf("Cold not read the file and build the table\n");
 * 		\endcode
 *
 * 		\sa hashWriteInFile()
 */
hashTableADT * hashReadFromFile(hashTableADT *table, char *filename, freeFuncT freeElement, compFuncT compareElement, copyFuncT copyElement, hashConvFuncT hashConvElement);

/**
 * \fn hashTableADT * hashWriteInFile(hashTableADT *table, char *filename, hashConvFuncT hashConvElement)
 *
 * 		\brief This function writes an instance of hashTableADT into a given file.
 *
 * 		\param table The hash table to be written.
 * 		\param filename The name of the file where to write the table.
 * 		\param hashConvElement The function that converts each element of the table into a string.
 *
 * 		\return The table if everything went OK, NULL if there were any errors.
 *
 * 		Use:
 * 		\code
 * 			hashTableADT table;
 *
 * 					...
 * 			initialization of table
 * 					...
 *
 * 			if (hashWriteInFile(&table, filename, hashConvElement) == NULL)
 * 				printf("Could not write the table in the file\n");
 * 		\endcode
 *
 * 		\sa hashReadFromFile()
 */
hashTableADT * hashWriteInFile(hashTableADT *table, char *filename, hashConvFuncT hashConvElement);

#endif /* LINEARHASHADT_H_ */
