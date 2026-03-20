#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "list.h"
#include "vector.h"

// Archivo .c de lista
// Original de aula global
// Este es una derivacion de dicho archivo

int setNode(List *l, char *key,  char * v1, int n2, float*v2, struct Paquete v3){
	// una funcion para actualizqr o crear un nodo
	struct Node * newNode = (struct Node *)malloc(sizeof(struct Node));
	if(newNode==NULL) return -1;

	strncpy(newNode->key, key, MAX_KEY_LENGTH);
	strncpy(newNode->value1, v1, MAX_KEY_LENGTH);

	newNode->value2 = init((uint8_t)n2);

	// Si queriamos añadir algo al vector pero no se ha creado:
	if(newNode->value2.mem== NULL && n2>0){
		free(newNode);
		return(-1);
	}
	// Copiar el vector
	for(int i = 0; i<n2; ++i) newNode->value2.mem[i] = v2[i];

	// copiar el resto
	newNode->value3=v3;
	newNode->next = *l;
	*l = newNode; // nueva cabeza
	return(0);
}


int get(List l, char *key,  char *value1, float* value2, struct Paquete * value3, int * n2){
	// funcion para copiar un nodo de la lista (obtenerlo o get)
	// según una key
	List aux;

	aux = l;	

	while (aux!=NULL) {
		if (strcmp(aux->key, key)==0) {
			// el get en si
			strcpy(value1, aux->value1);
			*n2 = aux->value2.size;
			for(int i=0;i<*n2; ++i) value2[i] = aux->value2.mem[i];
			*value3 = aux->value3;
			return(0);		// found
		}
		else
			aux = aux->next;
	}

	return -1;  // not found
}	

int printList(List l){
	// funcion para imprimir las claves guardadas correctamente
	List aux;

	aux = l;

	while(aux != NULL){
		printf("Key=%s    REGISTRADA CORRECTAMENTE\n", aux->key);
		aux = aux->next;
	}
	return 0;
}	

int delete(List *l, char *key){
	// Funcion para eliminar de la lista un nodo que tenga una key
	List aux, back;

	if (*l == NULL)  // lista vacia
		return -1;

	// primer elemento de la lista
	if (strcmp(key, (*l)->key) == 0){
		aux = *l;
		*l = (*l)->next;
		free(aux->value2.mem);
		free(aux);
		return 0;
	}
	
	aux = *l;
	back = *l;
	while (aux!=NULL) {
		if (strcmp(aux->key, key)==0) {
			back->next = aux->next;
			removeVector(&aux->value2);
			free(aux);
			return 0;		// found
		}
		else {
			back = aux;
			aux = aux->next;
		}
	}

	return -1;
}	

int destroy_list(List *l){
	// elimina la lista de manera segura
	List aux; 

	while (*l != NULL){
		aux = *l;
		*l = aux->next;
		free(aux->value2.mem);
		free(aux);
	}

	return 0;
}	

int toFile(List * l, char * name){
	// Queremos pasar la lista a un fichero
	
	FILE * fichero = fopen(name, "wb"); // los escribimos en binario
	if(fichero == NULL) return -1;

	List aux = *l; // una copia
	while(aux != NULL){
		// vamos serializandolo nodo a nodo al fichero, los punteros eran problemáticos
		fwrite(aux->key, sizeof(char), MAX_KEY_LENGTH, fichero);
		fwrite(aux->value1, sizeof(char), MAX_KEY_LENGTH, fichero);
		fwrite(&(aux->value2.size), sizeof(uint8_t), 1, fichero);

		if(aux->value2.size>0){
			fwrite(aux->value2.mem, sizeof(float), aux->value2.size, fichero);
		}

		fwrite(&(aux->value3), sizeof(struct Paquete), 1, fichero);

		aux = aux->next;
	}

	fclose(fichero);
	return 0;

}

int fromFile(List * l, char * name){
	// Obtenemos lista de fichero binario generado con tofile

	FILE * fichero = fopen(name, "rb");
	if(fichero==NULL) return -1;
	// PUEDE DAR ERROR si dejamos lista sin borrar
	destroy_list(l);

	char k[MAX_KEY_LENGTH];
	char v1[MAX_KEY_LENGTH];
	uint8_t n2;
	float v2[32];
	struct Paquete v3;

	while(fread(k, sizeof(char), MAX_KEY_LENGTH, fichero)==MAX_KEY_LENGTH){
		fread(v1, sizeof(char), MAX_KEY_LENGTH, fichero);
		fread(&n2, sizeof(uint8_t), 1, fichero);
		if(n2>0){
			fread(v2, sizeof(float), n2, fichero);
		}
		fread(&v3, sizeof(struct Paquete), 1, fichero);

		setNode(l,k,v1,(int)n2,v2,v3);
	}

	fclose(fichero);
	return 0;

}

int exists_in(List* l, char * key){
	/* Funcion para ver si existe un elemento en la lista dada una key*/

	if (*l == NULL)  // lista vacia
		return 0;
	
	List aux = *l;
	while (aux!=NULL) {
		if (strcmp(aux->key, key)==0) {
			return 1;		// found
		}
		aux = aux->next;
	}

	return 0;
}

int modify(List* l, char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
	/*Modificar el elemento que coincide el key*/

	if (*l == NULL)  // lista vacia
		return -1;
	
	List aux = *l;
	while (aux!=NULL) {
		if (strcmp(aux->key, key)==0) {
			strncpy(aux->value1, value1, MAX_KEY_LENGTH);

			// el vector lo mas sencillo es borrarlo y guardar luego
			removeVector(&(aux->value2));
			aux->value2 = init((uint8_t)N_value2);
			if(aux->value2.mem == NULL && N_value2 >0) return -1; // SOLO si no se allocatea 

			for(int i = 0; i<N_value2; ++i)
				aux->value2.mem[i] = V_value2[i];
			
			aux->value3 = value3;

			return 0;		// found
		}
		aux = aux->next;
	}
	return -1; // solo llega si no se encuentra
}
