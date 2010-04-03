/*
 * \file types.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <pthread.h>

/**
 * \def _MAX_FATHER_
 *		A gate father's name max length
 */
#define		_MAX_FATHER_		15

/**
 * \struct set
 * 		A struct used to store posible countries petitions.
 */
typedef struct{
	int countriesAmm;
	int *country;
	}set;

/**
 * \struct country
 * 		The struct of a country, containing all it's data.
 */
typedef struct{
	char name[45];
	int continent;
	int champ;
	int weight;
	int isHead;
	int sameContinent;
	int deathGroup;
	int champGroup;
	int weakGroup;
	int used;
	}country;

/**
 * \struct package
 * 		The struct of a package used to pass info between fixture builders.
 */
typedef struct{
	void **genStruct;
    int amm;
	}package;

/**
 * \struct condPack
 * 		A struct containing the data passed to the condition thread's.
 */
typedef struct{
	country **countries;
	country *head;
	set **sets;
	int maxCountries;
	}condPack;
	
/**
 * \struct gate
 * 		Description here
 */
typedef struct{
	char name[30];
	int type;
	char fathers[2][_MAX_FATHER_];
	int output;
	int input[2];
	} gate;

/**
 * \struct subFixture
 * 		A struct containing 4 countries, representing a subFixture built by a grouph process.
 */
typedef struct{
	int countriesAmm;
	country **countries;
	}subFixture;

/**
 * \struct threadsStruct
 * 		A struct containing the data of a thread
 */
typedef struct{
	pthread_t index;
	void *(**conditions)(void *condArgs);
	int type;
	}threadsStruct;
	
/**
 * \struct conditions
 * 		A struct containing the data of all the conditions for a given grouph process.
 */
typedef struct{
	threadsStruct * threadsTable;
	int conditionsAmm;
	}conditions;

/**
 * \struct curCircuit
 * 		Description here
 */
typedef struct{
	int curFile;
	int curLevel;
	} curCircuit;
	
#endif