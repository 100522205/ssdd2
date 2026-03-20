#ifndef _VECTOR_H
#define _VECTOR_H     

#include <stdint.h>

struct vector{
    uint8_t size;
    float * mem;
};

struct vector init( uint8_t size);

int addElement(struct vector * v, uint16_t pos, float element);
int removeVector(struct vector * v);

#endif