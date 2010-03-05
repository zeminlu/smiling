typedef struct {
	char nombre[45];
	int continente;
	int campeon;
	int peso; 
	} pais;

typedef struct {
	char *nombre;
	int condAmm;
	char ** (**conditions)(pais **paises);
	} cabeza;