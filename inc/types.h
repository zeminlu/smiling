typedef struct {
	char *nombre;
	int continente;
	int campeon;
	int peso; 
	}pais;

typedef struct {
	char *nombre;
	int condAmm;
	int * (**conditions)(pais **paises, pais *cabecera);
	}cabeza;