#ifndef _LISTA_H
#define _LISTA_H       
// Archivo .h
// Original de aula global
// Este es una derivacion de dicho archivo
#define MAX_KEY_LENGTH	256
#include "vector.h"
#include "claves.h"



struct Node{ 
	char 	key[MAX_KEY_LENGTH];
	char 	value1[MAX_KEY_LENGTH];
	struct vector 	value2;
	struct Paquete value3;
	struct 	Node *next; 
};


typedef struct Node * List;

int setNode(List *l, char *key,  char * v1, int n2, float*v2, struct Paquete v3);

int get(List l, char *key,  char *value1, float* value2, struct Paquete * value3, int * n2);
int printList(List l);
int delete(List *l, char *key);
int destroy_list(List *l);

int toFile(List * l, char * name);
int fromFile(List * l, char * name);

int exists_in(List* l, char * key);
int modify(List* l, char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3);
#endif

