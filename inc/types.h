#ifndef TYPES_H_
#define TYPES_H_

typedef struct{
	int countriesAmm;
	int * country;
	}set;

typedef struct{
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

typedef struct{
	country **countries;
	country *head;
	int *index;
	set **sets;
	}condPack;
	
typedef struct{
	char *name;
	int type;
	struct gates fathers[2];
	struct gates *sons;
	int output;
} gates;

#endif
