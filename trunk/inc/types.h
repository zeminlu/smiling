typedef struct{
	int cantPaises;
	int * paises;
	}conjunto;

typedef struct {
	char nombre[45];
	int continente;
	int campeon;
	int peso; 
	}pais;

typedef struct {
	char nombre[45];
	int sameContinent;
	int deathGroup;
	int champGroup;
	int weakGroup;
	}cabeza;

typedef struct{
	void **estructura;
	int amm;
	}package;

typedef struct{
	pais **paises;
	cabeza *head;
	}condPack;