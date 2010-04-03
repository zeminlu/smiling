/*
 * \file serializable.c
 *
 *  \date Created on: 3-apr-2010
 *  \author Luciano R. Zemin & Nicolás Magni & Nicolás Purita
 */

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