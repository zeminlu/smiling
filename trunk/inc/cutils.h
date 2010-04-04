/**
 * \file cutils.h
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

#ifndef _CUTILS_
#define _CUTILS_ 

/**
 * \fn void reverse(char s[])
 *
 * 		\brief This function reverses the content of it's string parameter.
 *
 * 		\param s The string to reverse
 *
 * 		Use:
 * 		\code
 * 			char *aux = "Reverse";
 *			reverse(aux);
 *		\endcode
 *
 * 		\sa itoa()
 *
 */
void reverse(char s[]);

/**
 * \fn void itoa(int n, char s[])
 *
 * 		\brief This function takes it's int n parameter and converts it to a char * storing it in the char s[] parameter.
 *
 * 		\param n The int to convert
 *		\param s The char[] where to store the conversion
 *
 * 		Use:
 * 		\code
 * 			int n = 10;
 *			char conv[3];
 *			
 *			itoa(n, conv);
 *		\endcode
 *
 * 		\sa reverse()
 *
 */
void itoa(int n, char s[]);

#endif
