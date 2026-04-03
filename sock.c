#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include "claves.h"
#include <malloc.h>
#include <stdlib.h>
#include "sock.h"


int sock_send(int fd_socket, char * buff, int size){
    /*Función para el envío de un string a un socket*/

    int remaining = size, sent;
    int to_return = 0;
    while((remaining>0)&&((sent =write(fd_socket, buff, remaining))>0)){
        remaining-=sent;
        to_return+=sent;
        buff+=sent;
    } 
    return to_return;

}


int sock_receive(int fd_socket, char*buff, int size){
    /* Función para la recepción de un string por socket*/

    int remaining = size, read_v, to_return=0;
    while((remaining>0)&&((read_v = read(fd_socket, buff, remaining))>0)){
        remaining-=read_v;
        to_return+=read_v;
        buff+=read_v;
    }
    return to_return;
}


int int_to_string(int integer, char*buff){
    /* Función para añadir un entero a un buffer */
    return sprintf(buff, "%d ", integer);
}


int float_to_string(float f, char*buff){
    return sprintf(buff, "%f ", f);
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
    int move_this, total=0;

    move_this = int_to_string(p->x, buff);
    if(move_this<0) return -1;
    else {
        buff+=move_this;
        total+=move_this;
    }

    move_this = int_to_string(p->y, buff);
    if(move_this<0) return -1;
    else {
        buff+=move_this;
        total+=move_this;
    }

    move_this = int_to_string(p->z, buff);
    if(move_this<0) return -1;
    else {
        buff+=move_this;
        total+=move_this;
    }
    return total;
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
int pet_to_string(struct Peticion * pet, char*buff){
    /* Función para pasar una petición a un buffer como string */
    int move_this, total=0;
    
    move_this=int_to_string(pet->cod_op, buff);
    buff+=move_this;
    total+=move_this;

    move_this= sprintf(buff, "%s ", pet->key[0]== '\0' ? "-" : pet->key);
    buff+=move_this;
    total+=move_this;

    move_this= sprintf(buff, "%s ", pet->value1[0] == '\0' ? "-" : pet->value1); 
    buff+=move_this;
    total+=move_this;

    move_this=int_to_string(pet->N_value2, buff);
    buff+=move_this;
    total+=move_this;

    for(int i=0; i<pet->N_value2; ++i){
        move_this=float_to_string(pet->V_value2[i], buff);
        buff+=move_this;
        total+=move_this;
    }
    
    if((move_this=paquete_to_string(&(pet->value3), buff))<1) return (-1);
    total+=move_this;

    return total;
    
}

int string_to_pet(char* buff, struct Peticion * pet){
     /* Función para pasar una respuesta a un buffer como string 
     * Usamos strtok_r, que nos devuelve strings de uno mayor
     * luego usamos strncpy para strings, atoi para integers y atof para floats*/
    char* segment, *rest = buff;

    if(!(segment = strtok_r(rest, " ", &rest))) return -1;
    pet->cod_op=atoi(segment);

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    strncpy(pet->key,segment, MAX_LENG);
    pet->key[MAX_LENG-1]='\0';

    
    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    strncpy( pet->value1, segment, MAX_LENG); 
    pet->value1[MAX_LENG-1]='\0';

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    pet->N_value2=atoi(segment);

    for(int i=0; i< pet->N_value2;++i){
        if(!(segment=strtok_r(NULL, " ", &rest))) return -1;
        pet->V_value2[i]=atof(segment);
    }

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    pet->value3.x=atoi(segment);

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    pet->value3.y=atoi(segment);
   
    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    pet->value3.z=atoi(segment);

    return 0;
    
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
int res_to_string(struct Respuesta* res, char*buff){
    int move_this, total=0;
    
    move_this=int_to_string(res->cod_err, buff);
    buff+=move_this;
    total+=move_this;

    move_this= sprintf(buff, "%s ", res->value1[0] == '\0' ? "-" : res->value1);
    buff+=move_this;
    total+=move_this;

    move_this=int_to_string(res->N_value2, buff);
    buff+=move_this;
    total+=move_this;

    for(int i=0; i<res->N_value2; ++i){
        move_this=float_to_string(res->V_value2[i], buff);
        buff+=move_this;
        total+=move_this;
    }
    
    if((move_this=paquete_to_string(&(res->value3), buff))<1) return (-1);
    total+=move_this;

    return total;
}

int string_to_res(char* buff, struct Respuesta * res){
    /*Funcion para pasar un string a estructura respuesta*/
    char* segment, *rest = buff;

    if(!(segment = strtok_r(rest, " ", &rest))) return -1;
    res->cod_err=atoi(segment);

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    strncpy(res->value1,segment, MAX_LENG);
    res->value1[MAX_LENG-1]='\0';

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    res->N_value2=atoi(segment);

    for(int i=0; i< res->N_value2;++i){
        if(!(segment=strtok_r(NULL, " ", &rest))) return -1;
        res->V_value2[i]=atof(segment);
    }

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    res->value3.x=atoi(segment);

    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    res->value3.y=atoi(segment);
   
    if(!(segment = strtok_r(NULL, " ", &rest))) return -1;
    res->value3.z=atoi(segment);

    return 0;
    
}

