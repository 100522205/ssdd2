#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include "claves.h"
#include "malloc.h"
#include "sock.h"


int sock_send(int fd_socket, char * buff, int size){
    /*Función para el envío de un string a un socket*/

    int remaining = size;
    while((remaining>0)&&(write(fd_socket, buff, size)>0)) ;
    return 0;

}


int sock_recieve(int fd_socket, char*buff, int size){
    /* Función para la recepción de un string por socket*/

    int remaining = size;
    while((remaining>0)&&(read(fd_socket, buff, size)>0)) ;
    return 0;
}


int int_to_string(int integer, char*buff){
    /* Función para añadir un entero a un buffer */
    char*aux[MAX_LENG];
    sprintf(aux, "%d", integer);
    if(strcat(buff, aux)<1) return -1;
    return 0;
}


int float_to_string(float f, char*buff){
    char*aux[MAX_LENG];
    sprintf(aux, "%f", f);
    if (strcat(buff, aux)<1) return -1;
    return 0;
}

/**
 * 
 * struct Paquete {
 *  int x ;
 *  int y ;
 *  int z ;
 * } ; 
 * 
 */
int paquete_to_string(struct Paquete * p, char*buff){
    if(int_to_string(p->x, buff)<0) return -1;
    if(int_to_string(p->y, buff)<0) return -1;
    if(int_to_string(p->z, buff)<0) return -1;
    return 0;
}


/**
 * 
 * struct Peticion {
 *   int             cod_op;
 *   char            key[MAX_LENG];
 *   char            value1[MAX_LENG];
 *   int             N_value2;
 *   float           V_value2[32];
 *   struct Paquete  value3;
 * };
 *
 */
int pet_to_string(struct Peticion, char*buff){
    /* Función para pasar una petición a un buffer como string */
    
    char * buff;
    
}


/**
 * 
 * struct Respuesta {
 *   int             cod_err;
 *   char            value1[MAX_LENG];
 *   int             N_value2;
 *   float           V_value2[32];
 *   struct Paquete  value3;
 * };
 * 
 */
int res_to_string(struct Respuesta, char*buff){
    /* Función para pasar una respuesta a un buffer como string */


}
