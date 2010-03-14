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

int serializeGate( gate *gate, void **buffer, int *bufferSize)
{
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#ic##ii#)", gate, 30, 2, 30, 2);
	tpl_pack(tn,0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize );
	tpl_free(tn);
	
	return ret;
}

int serializeSubfixture(void **buffer, int *bufferSize, subFixture *group){
	tpl_node *tn;
	int ret, i;
	country countArr[4];
	
	for (i = 0 ; i < 4 ; ++i){
		countArr[i] = *(group->countries[i]);
	}
	
	tn = tpl_map("S(c#iiiiiiiii)#", countArr, 45, 4);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
	return ret;	
}

int unserializeSubfixture(void *buffer, int bufferSize, country **subFixture){
	tpl_node *tn;
	int ret;
	
	tn = tpl_map("S(c#iiiiiiiii)#", *subFixture, 45, 4);
	ret = tpl_load(tn, TPL_MEM, buffer, bufferSize);
	tpl_unpack(tn, 0);
	tpl_free(tn);
	
	return ret;
}

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
	
	tn = tpl_map("i#", &number, 1);
	tpl_pack(tn, 0);
	ret = tpl_dump(tn, TPL_MEM, buffer, bufferSize);
	tpl_free(tn);
	
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
}