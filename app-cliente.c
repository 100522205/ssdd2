/*Cliente simulado, es decir, tests*/
#include <stdio.h>
#include "tests.h"
int main(){
    printf("\n--------------------------\n\n--------------------------\nCLIENTE:");
    return (test()+1)%2;
}

