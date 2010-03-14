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