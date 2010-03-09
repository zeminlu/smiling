#ifndef TYPES_H_
#define TYPES_H_

typedef struct{
	int countriesAmm;
	int * country;
	}set;

typedef struct {
	char name[45];
	int continent;
	int champ;
	int weight;
	int isHead;
	int sameContinent;
	int deathGroup;
	int champGroup;
	int weakGroup;
	}country;

typedef struct{
	void **genStruct;
    int amm;
	}package;

#endif
