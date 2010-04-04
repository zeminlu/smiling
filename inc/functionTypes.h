/**
 * \file functionTypes.h
 *
 *  \date Created on: 07-nov-2008
 *  \author Luciano R. Zemin
 */

#ifndef FUNCTIONTYPES_H_
#define FUNCTIONTYPES_H_

/**
 * \var compFuncT
 *
 * 		\brief Typedef of the prototype of a function that compares two generic homogeneous elements.
 */
typedef int (*compFuncT)(void *, void *);

/**
 * \var copyFuncT
 *
 * 		\brief Typedef of the prototype of a function that copies a generic element.
 */
typedef void * (*copyFuncT)(void *);

/**
 * \var freeFuncT
 *
 * 		\brief Typedef of the prototype of a function that frees a generic element.
 */
typedef void (*freeFuncT)(void *);

/**
 * \var convTreeFuncT
 *
 * 		\brief Typedef of the prototype of a function that converts a string to a flightBasicT or vice versa.
 */
typedef void * (*convTreeFuncT)( char * string, void * elem );

typedef void * (*convGraphFuncT)( char * string, void * airport, char ** key );



#endif /* FUNCTIONTYPES_H_ */
