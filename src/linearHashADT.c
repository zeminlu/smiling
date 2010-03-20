/*
 * \file linearHashADT.c
 *
 *  \date Created on: 29-oct-2008
 *  \author: Luciano R. Zemin & Maria Eugenia Cura
 */
#include "linearHashADT.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 1000

typedef enum
{
	PHYSICAL,
	LOGICAL,
	USED
}
bucketStates;

typedef struct tBucket
{
	hashTableElement item;
	bucketStates state;
	char *key;
}tBucket;

typedef struct hashTableCDT
{
	tBucket *buckets;
	int nbuckets;
	int iterator;
	int used;
	freeFuncT freeElement;
	compFuncT compElement;
	copyFuncT copyElement;
}hashTableCDT;

hashTableADT hashCreateTable(int nbuckets, freeFuncT freeElement, compFuncT compElement, copyFuncT copyElement)
{
	hashTableADT table;

	if ((table = calloc(1, sizeof(hashTableCDT))) == NULL || (table->buckets = calloc(1, (sizeof(tBucket) * nbuckets))) == NULL)
		return table;

	table->nbuckets = nbuckets;
	table->freeElement = freeElement;
	table->compElement = compElement;
	table->copyElement = copyElement;
	return table;
}

void hashFreeTable(hashTableADT table)
{
	unsigned int i;

	for (i = 0 ; i < table->nbuckets ; ++i)
		if (table->buckets[i].state == USED)
		{
			table->freeElement(table->buckets[i].item);
			free(table->buckets[i].key);
		}
	free(table->buckets);
	free(table);
}

static hashTableADT * reHash(hashTableADT *table, int expand)
{
	hashTableADT tmp;
	int newBuckets;
	unsigned int i;

	tmp = *table;
	newBuckets = (expand ? tmp->nbuckets * 2 : tmp->nbuckets / 2);

	if (((*table = hashCreateTable(newBuckets, tmp->freeElement, tmp->compElement, tmp->copyElement)) == NULL))
		return NULL;

	for (i = 0 ; i < tmp->nbuckets ; ++i)
		if (tmp->buckets[i].state == USED)
			hashInsert(table, tmp->buckets[i].item, tmp->buckets[i].key, FALSE);

	hashFreeTable(tmp);

	return table;
}

unsigned int hash(const char *key, int nbuckets)
{
	unsigned int hash_val = 1;
	while( *key != '\0')
		hash_val = (hash_val << 5) + *key++;

	return (hash_val % nbuckets);
}

void hashSetIterator(hashTableADT table)
{
	table->iterator = 0;
}

hashTableElement hashGetNext(hashTableADT table)
{
	if (table->iterator == table->nbuckets)
	{
		table->iterator = 0;
		return NULL;
	}
	if (table->buckets[table->iterator].state != USED)
	{
		++table->iterator;
		return hashGetNext(table);
	}
	return table->copyElement(table->buckets[table->iterator++].item);
}

hashTableElement hashInsert(hashTableADT *table, hashTableElement elem, const char *key, int update)
{
	unsigned int hkey, fkey, aux = 0;
	int state = 0, itState = 0;

	fkey = hkey = hash(key, (*table)->nbuckets);

	if ((*table)->buckets[hkey].state != PHYSICAL)
	{
		while ((*table)->buckets[hkey].state != PHYSICAL && (hkey != fkey || itState == 0))
		{
			if ((*table)->buckets[hkey].state == LOGICAL && state == 0)
			{
				aux = hkey;
				state = 1;
			}
			else if ((*table)->buckets[hkey].state == USED && !((*table)->compElement(elem, (*table)->buckets[hkey].item)))
			{
				if (update)
				{
					(*table)->freeElement((*table)->buckets[hkey].item);
					(*table)->buckets[hkey].item = (*table)->copyElement(elem);
					return elem;
				}
				return NULL;
			}
			hkey = (hkey == (*table)->nbuckets - 1 ? 0 : hkey + 1);
			itState = 1;
		}
		if (!(hkey != fkey || state))
		{
			if ((reHash(table, TRUE)) == NULL)
				return NULL;
			else
				return hashInsert(table, elem, key, update);
		}
		if (state)
			hkey = aux;
	}
	(*table)->buckets[hkey].item = (*table)->copyElement(elem);
	(*table)->buckets[hkey].state = USED;
	if (((*table)->buckets[hkey].key = calloc(1, strlen(key) + 1)) == NULL)
		return NULL;
	strcpy((*table)->buckets[hkey].key, key);
	(*table)->used++;
	return elem;
}

hashTableElement hashSearch(hashTableADT table, const char *key, unsigned int *ptr)
{
	unsigned int hkey, nbuckets = 0, state = 1;
	hkey = hash(key, table->nbuckets);

	if (table->used == 0)
		return NULL;

	while (state != 0 && nbuckets++ < table->nbuckets)
	{
		if (table->buckets[hkey].state == USED)
			state = (strcmp(table->buckets[hkey++].key, key));
	}
	if (!state)
		return (table->copyElement(table->buckets[*ptr = --hkey].item));
	return NULL;
}

const char * hashDelete(hashTableADT *table, const char *key)
{
	unsigned int aux;
	hashTableElement temp;

	if ((temp = hashSearch(*table, key, &aux)) == NULL)
		return NULL;
	(*table)->freeElement((*table)->buckets[aux].item);
	free((*table)->buckets[aux].key);
	if((*table)->buckets[aux + 1].state != PHYSICAL)
		(*table)->buckets[aux].state = LOGICAL;
	else
		(*table)->buckets[aux].state = PHYSICAL;

	if (--((*table)->used) < ((*table)->nbuckets / 2))
		reHash(table, FALSE);
	(*table)->freeElement(temp);
	return key;
}

hashTableADT * hashReadFromFile(hashTableADT *table, char *filename, freeFuncT freeElement, compFuncT compareElement, copyFuncT copyElement, hashConvFuncT hashConvElement)
{
	FILE *hashFile;
	hashTableElement air = NULL;
	char aux[MAX_LINE], *key;

	if (filename == NULL || freeElement == NULL || compareElement == NULL || hashConvElement == NULL)
		return NULL;

	if ((hashFile = fopen(filename, "rb+")) == NULL)
	{
		if ((hashFile = fopen(filename, "wb+")) == NULL)
			return NULL;
		fputs("2\n", hashFile);
		fseek(hashFile, 0, SEEK_SET);
	}
	if (fgets(aux, MAX_LINE, hashFile) == NULL
			|| (*table = hashCreateTable(atoi(aux), freeElement, compareElement, copyElement)) == NULL)
	{
		fclose(hashFile);
		return NULL;
	}
	while (fgets(aux, MAX_LINE, hashFile) != NULL)
	{
		air = hashConvElement(aux, NULL, &key);

		if (hashInsert(table, air, key, TRUE) == NULL)
		{
			hashFreeTable(*table);
			(*table)->freeElement(air);
			fclose(hashFile);
			free(key);
			return NULL;
		}
		(*table)->freeElement(air);
		free(key);
	}
	fclose(hashFile);
	return table;
}

hashTableADT * hashWriteInFile(hashTableADT *table, char *filename, hashConvFuncT hashConvElement)
{
	FILE *hashFile = NULL;
	char *aux = NULL;
	hashTableElement elem;

	if (table == NULL || filename == NULL || (hashFile = fopen(filename, "wb+")) == NULL || fprintf(hashFile, "%d\n", (*table)->nbuckets) == EOF)
	{
		fclose(hashFile);
		return NULL;
	}

	hashSetIterator(*table);
	while ((elem = hashGetNext(*table)) != NULL)
	{
		if ((aux = hashConvElement(NULL, elem, NULL)) == NULL
				|| fputs(aux, hashFile) == EOF)
		{
			fclose(hashFile);
			free(aux);
			(*table)->freeElement(elem);
			return NULL;
		}
		(*table)->freeElement(elem);
		free(aux);
	}
	fclose(hashFile);
	return table;
}
