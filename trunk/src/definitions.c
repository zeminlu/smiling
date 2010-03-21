/*
 *	definitions.c
 */

#include "definitions.h"

void varFree(int amm, ...){
	va_list ap;
	int i;

	va_start(ap, amm);
	for (i = 0 ; i < amm ; ++i)
	{
		free(va_arg(ap, void *));
	}
	va_end(ap);
}