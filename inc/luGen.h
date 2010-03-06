/*
 *		luGen.h
 */


void createInitCondFile( int cantVar, float *initCond );

void createLUFile( float ** values );

void freeAllPtr( float ** values, float* initCond );

float ** loadEquationFile( FILE * ecuFile );

float * loadInitCondition ( FILE *initFile );

void printMatrix( float ** values );

void printInitCond( float * init);

