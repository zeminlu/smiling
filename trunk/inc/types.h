#ifndef TYPES_H_
#define TYPES_H_

typedef struct{
	int cantPaises;
	int * paises;
	}conjunto;

typedef struct {
	char nombre[45];
	int continente;
	int campeon;
	int peso;
	int isHead;
	int sameContinent;
	int deathGroup;
	int champGroup;
	int weakGroup;
	}pais;

typedef struct{
	void **estructura;
	int amm;
	}package;

#endif
