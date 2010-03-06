typedef struct{
	int cantPaises;
	int * paises;
	}conjunto;

typedef struct {
	char *nombre;
	int continente;
	int campeon;
	int peso; 
	}pais;

typedef struct {
	char *nombre;
	int condAmm;
	conjunto * (**conditions)(pais **paises, pais *cabecera);
	}cabeza;
	
