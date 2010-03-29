/*
 *	Este archivo es el encargado de serializar las estructuras
 *	utilizadas durante el programa para ser enviadas por medio
 *	de cada IPC.
 */

#include "../inc/serializable.h"

/*
 *	Serializa el circuito por completo.
 *	Parametros que recibe:
 *		- El puntero del circuito
 *		- La direccion del buffer donde se va serializar
 *		- El tamaño del buffer
 */

int serializeCountryStruct(void **buffer, int *bufferSize, country *country){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", country, 45);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;
}

int unserializeCountryStruct(void *buffer, int bufferSize, country *country){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)", country, 45);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

int serializeInteger(void **buffer, int *bufferSize, int number){
	tpl_node *tn;
	int ret = 0;
	/*char puto[5];
	
	puto[5] = '\0';
	
	printf("<---------SERIALIZANDO ENTERO = .oO¡¡¡<%d>!!!Oo.--------->\n", number);*/
	
	tn = tpl_map("i#", &number, 1);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	/*
	*buffer = malloc(sizeof(int));
	memcpy(*buffer, &number, 4);
	*bufferSize = sizeof(int);
	
	printf("<-----buffer = %d----->\n", **((int **)buffer));

	printf("<------------En Serialize: %x %x %x %x ------------>\n", ((char *)*buffer)[0], ((char *)*buffer)[1], ((char *)*buffer)[2], ((char *)*buffer)[3]);
	*/
	return ret;
}

int unserializeInteger(void *buffer, int bufferSize){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("i#", &ret, 1);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
	/*	
	
	printf("<------------En UNserialize: %x %x %x %x ------------>\n", ((char *)buffer)[0], ((char *)buffer)[1], ((char *)buffer)[2], ((char *)buffer)[3]);
	
	return *((int*)(buffer));*/
}