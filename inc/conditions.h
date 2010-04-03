/*
 * \file conditions.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef CONDITIONS_H_
#define CONDITIONS_H_

#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#include "../inc/types.h"
#include "../inc/definitions.h"

/**
 * \fn void * sameContinent(void * condi)
 *
 * 		\brief This function builds an array of indexes referencing the position of the avilable countries in the countries table that satisfy the same continent condition and stores it in the appropiate global variable. It runs as a thread using mutexes.
 *
 * 		\param condi The address to the condPack struct where the array will be stored.
 * 		
 */
void * sameContinent(void * condi);

/**
 * \fn void * deathGroup(void * condi)
 *
 * 		\brief This function builds an array of indexes referencing the position of the available countries in the countries table that satisfy the death group condition and stores it in the appropiate global variable. It runs as a thread using mutexes.
 *
 * 		\param condi The address to the condPack struct where the array will be stored.
 * 		
 */
void * deathGroup(void * condi);

/**
 * \fn void * champGroup(void * condi)
 *
 * 		\brief This function builds an array of indexes referencing the position of the available countries in the countries table that satisfy the champion group condition and stores it in the appropiate global variable. It runs as a thread using mutexes.
 *
 * 		\param condi The address to the condPack struct where the array will be stored.
 * 		
 */
void * champGroup(void * condi);

/**
 * \fn void * weakGroup(void * condi)
 *
 * 		\brief This function builds an array of indexes referencing the position of the available countries in the countries table that satisfy the weak group condition and stores it in the appropiate global variable. It runs as a thread using mutexes.
 *
 * 		\param condi The address to the condPack struct where the array will be stored.
 * 		
 */
void * weakGroup(void * condi);

/**
 * \fn void * noCondition(void * condi)
 *
 * 		\brief This function builds an array of indexes referencing the position of all the countries in the countries table that are avaiable and stores it in the appropiate global variable. It runs as a thread using mutexes.
 *
 * 		\param condi The address to the condPack struct where the array will be stored.
 * 		
 */
void * noCondition(void * condi);

#endif