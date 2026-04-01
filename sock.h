#ifndef _MQ_H
#define _MQ_H

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
#include "claves.h"

#define MAX_LENG 2048 // lo subimos por seguridad

/*
Programa para envío y recepción usando sockets
Implementamos este archivo para abstraer las funciones siguientes:

sock_send       : Envío a un socket
sock_recieve    : Recepción dado un socket
into_to_string  : Transformación de un entero a string
float_to_string : Transformación de un float a string
paq_to_string   : Trasnformación de una estructura Paquete a string
pet_to_string   : Transformación de una estructura Peticion a string
res_to_string   : Transformación de una estructura Respuesta a string

*/

/*

ops.                arg. in                         arg. out

destroy      (0)    cod op      int                 cod err     int
                    colaR       char[]

set_value    (1)    cod op      int                 cod err     int
                    colaR       char[]
                    key         char[]
                    value1      char[]
                    N_value2    int
                    V_value2    float[]
                    value3      struct Paquete

get_value    (2)    cod op      int                 cod err     int
                    colaR       char[]              value1      char[]
                    key         char[]              N_value2    int
                                                    V_value2    float[]
                                                    value3      struct Paquete

modify_value (3)    cod op      int                 cod err     int
                    colaR       char[]
                    key         char[]
                    value1      char[]
                    N_value2    int
                    V_value2    float[]
                    value3      struct Paquete

delete_key   (4)    cod op      int                 cod err     int
                    colaR       char[]
                    key         char[]

exist        (5)    cod op      int                 cod err     int
                    colaR       char[]
                    key         char[]

*/



// Estructuras para colas de mensajes

struct Peticion {
    int             cod_op;
    char            key[MAX_LENG];
    char            value1[MAX_LENG];
    int             N_value2;
    float           V_value2[32];
    struct Paquete  value3;
};

struct Respuesta {
    int             cod_err;
    char            value1[MAX_LENG];
    int             N_value2;
    float           V_value2[32];
    struct Paquete  value3;
};


int sock_send(int fd_socket, char * buff, int size);
int sock_receive(int fd_socket, char*buff, int size);
int int_to_string(int integer, char*buff);
int float_to_string(float f, char*buff);
int paquete_to_string(struct Paquete * p, char*buff);
int pet_to_string(struct Peticion * pet, char*buff);
int res_to_string(struct Respuesta* res, char*buff);
int string_to_pet(char * buff, struct Peticion * pet);
int string_to_res(char* buff, struct Respuesta * res);


#endif
