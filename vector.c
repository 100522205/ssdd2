#include "vector.h"
#include <stdlib.h>
#include <malloc.h>

struct vector init( uint8_t size){
    // inicializar vector de 1 a 32 elementos
    // devuelve v de size 0 si hay error
    struct vector v;
    v.size = 0; //
    if(!((size>0)&&(size<=32)))
        return(v);
    
    v.size=size;
    v.mem = (float*)malloc(sizeof(float)*v.size);

    return(v);
}   


int addElement(struct vector * v, uint16_t pos, float element){
    // añadir un elemento
    if(v->size < pos) return(-1);

    float * mem = v->mem;

    mem[pos]= element;

    return 0;
}

int removeVector(struct vector * v){
    if(v-> mem != NULL){
        free(v->mem);
        v->mem = NULL;
        v->size= 0;
    }
    return 0;
}
