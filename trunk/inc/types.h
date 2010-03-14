#ifndef TYPES_H_
#define TYPES_H_

typedef struct{
	int countriesAmm;
	int *country;
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
	int used;
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
	char name[30];
	int type;
	char *fathers[2];
	int output;
	int input[2];
	} gate;

typedef struct{
	int countriesAmm;
	country **countries;
	}subFixture;

#endif
