/*
 *		pipe.c
 *		
 *			Programa encargado de verificar cuando se agrega un archivo en el directorio
 *			pipeDir.
 */

#include "../inc/pipe.h"

int main(){
	
	DIR *dp;
	struct dirent *d;
	int gate = 0;
	FILE *dataFile = NULL;
	char *dir = "./pipeDir/", *procDir = "./processed/", *dirFile = NULL, *procCopyDir;
	
	if ((dp = opendir(dir)) == NULL){
		return -1;
	}
	while (getFilesAmm(dp) <= 3 ){
		sleep(1);
		rewinddir(dp);
	};
	while( (d = readdir(dp)) && getFilesAmm(dp) > 3 )
	{
		if(d->d_ino == 0 )
		{
			continue;
		}
		if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0 || strcmp(d->d_name, ".svn") == 0){
			continue;
		}
		else{
			if( ( dirFile = (char*)malloc(sizeof(char) + strlen(dir) + strlen(d->d_name) + 1 )) == NULL )
			{
				closedir(dp);
				perror("Error en la alocacion de memoria\n");
				return errno;
			}
			strcpy(dirFile, dir);
			strcat(dirFile, d->d_name);
			if( (dataFile = fopen(dirFile, "r")) ==  NULL )
			{
				closedir(dp);
				perror("No se pudo abrir el archivo de las compuertas\n");
				return errno;
			}
			/* Aca hay que cargar el archivo de las compuertas */
		}
	}
	if( ( procCopyDir = (char*)malloc(sizeof(char) + strlen(procDir) + strlen(d->d_name) + 1 )) == NULL )
	{
		closedir(dp);
		perror("Error en la alocacion de memoria\n");
		return errno;
	}
	strcpy(procCopyDir,procDir);
	strcat(procCopyDir,d->d_name);
	fclose( dataFile );
	link(dirFile,procCopyDir);
	unlink(dirFile);
	
	switch( fork() ){
		case 0:
			execv("./gates.bin", NULL);
			break;
		case -1:
			perror("Error en el fork del pipeline\n");
			break;
		default:
			wait(&gate);
			break;
	}
	free(dirFile);
	free(procCopyDir);
	return 0;
}

/*
 *	Verifica la cantidad de archivo que hay dentro del directorio
 */

int getFilesAmm (DIR *dp){
	struct dirent *d;
	int ret = 0;
	while ((d = readdir(dp))){
		 if (d->d_ino != 0){
			++ret;
		}
	}
	return ret;
}

/* Compuerta AND */

int gateAnd( int in1, int in2 )
{
	return in1 & in2;
}

/* Compuerta OR */

int gateOr( int in1, int in2 )
{
	return in1 | in2;
}

/* Compuerta XOR */

int gateXor( int in1, int in2 )
{
	return in1 ^ in2; 
}

/* Compuerta NAND */

int gateNand( int in1, int in2 )
{
	return !gateAnd(in1,in2);
}

/* Compuerta NOR */

int gateNor( int in1, int in2 )
{
	return !gateOr(in1,in2);
}

/* Compuerta XNOR */

int gateXnor( int in1, int in2 )
{
	return !gateXor(in1,in2);
}