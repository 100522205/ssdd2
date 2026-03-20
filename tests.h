#ifndef TESTS_H
#define TESTS_H

#include <stdint.h>

/* Tests individuales */
int test_set();
int test_get();
int test_modify();
int test_delete();
int test_exists();
int test_destroy();
int test_duplicate();
int test_N_inval();
int test_nonexist();


// Funcion para probar todo, de orden superior porque quise probarlo con punteros
int test();

#endif