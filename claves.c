#include "claves.h"
#include "list.h"
#include <string.h>
// Archivo .c de lista
// Original de aula global
// Este es una derivacion de dicho archivo

List myList;
const char * path = "./backup/bck";

int destroy(void){
/* Destruir todas las tuplas previas */
    int res = destroy_list(&myList);
    //if(res==0) toFile(&myList, (char *)path);
    return res;
}


int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    /*Insertar un elemento*/
    if(N_value2<1 || N_value2>32 || strlen(value1)>255 || strlen(key)>255) return(-1);

    if(exists_in(&myList, key)==1) return -1;

    int res = setNode(&myList, key, value1, N_value2, V_value2, value3);
    //toFile(&myList, (char *)path);
    return res;
}


int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){
    /* Obtener los elementos de 1 dada la key */
    if(exists_in(&myList, key)==0||strlen(key)>255) return -1;

    int res = get(myList, key, value1, V_value2, value3, N_value2);
    return res;
}


int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    /* Modificar los valores dada una key */
    if(N_value2<1 || N_value2>32 || strlen(value1)>255|| strlen(key)>255) return(-1);

    if(exist(key)==0) return(-1);

    int res = modify(&myList, key, value1, N_value2, V_value2, value3);

    //if(res==0) toFile(&myList, (char *)path); // plasmamos al backup

    return res;

}


int delete_key(char *key){
    /* Borrar 1 elemento dado una key*/
    int res = delete(&myList, key);
    //if(res==0) toFile(&myList, (char*)path);
    return res;
}


int exist(char *key){
    /* Determinar si existe un elemento dada una key*/
    return exists_in(&myList, key);

}