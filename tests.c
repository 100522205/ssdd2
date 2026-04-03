#include "tests.h"
#include "claves.h"
#include "unistd.h"
#include <string.h>
#include <stdio.h>

struct Paquete myPack = {1,0,0};

char mypid[32];

/* Tests individuales */
int test_set(){
    float v[3] = {1.0, 2.0, 3.0};
    return(set_value(mypid, "Hello", 3, v, myPack)==0);
}

int test_get(){
    char value1[50];
    int n=0;
    float v[10];
    struct Paquete p;

    return(get_value(mypid, value1, &n, v, &p)==0);
}

int test_modify(){
    float v[2] = {0.3,0.2};
    return(modify_value(mypid, "Rehello", 2, v, myPack)==0);
}

int test_delete(){
    return(delete_key(mypid)==0);
}

int test_exists(){
    return(exist(mypid)==1);
}

int test_destroy(){
    return(destroy()==0);
}

// Test para edge cases

int test_duplicate(){
    // probar asegurando duplicado!
    float v[1] = {1.0};
    return(set_value(mypid, "MAL", 1, v, myPack)==-1);
}

int test_N_inval(){
    // Test para probar al pasarse de tamaño de tamaño
    char v1[99];
    int n;
    float v[5];
    struct Paquete p;

    return(get_value("999", v1, &n, v, &p)==-1);
}

int test_nonexist(){
    // Test para probar obtener algo de un nodo inexistente
    float v[33] = {1.0};
    return(set_value("9999999999", "MAL", 33, v, myPack)==-1);
}

// Conjunto de tests a probar

int (*lista_tests[])(void) = {
// tests de camino feliz
test_set,
test_duplicate,
test_N_inval,
test_get,
test_nonexist,
test_modify,
test_exists,
test_delete,
//test_destroy,

};

const uint8_t num_tests=8;

// Funcion para probar todo, de orden superior porque quise probarlo con punteros
int test(){
    printf("\n\n<---------------------->\nREALIZANDO TESTS\n");
    uint8_t res=1;

    sprintf(mypid, "K_%d", getpid());

    for(int i=0; i<num_tests; ++i){
        printf("[%d]Realizando el test %d\n", getpid(),1+i);
        res= lista_tests[i]()*res;
        if(res!=1) printf("[%d]Ya falló por el test %d\n",getpid(), i);
    }
    printf("\n\n<---------------------->\n");
    if(res ==1) printf("[%d]LOS TEST TRIUNFARON\n", getpid()); else printf("[%d]LOS TEST FALLARON", getpid());
    return res;
    }