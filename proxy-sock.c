/*Archivo del proxy para el cliente. 
 * TIene las funciones falsas de claves, que llama a las verdaderas del servidor*/

#include "claves.h"
#include "sock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

int error(char * msg){
    // manejo de errores
    printf("Error de servidor en proxy-sock : %s\n", msg);
    return -1;
}

char*ip, *port;
int fdConexion=0;
int obtenido=0;
int conectado=0;

int getGlobals(){
    // funcion para poder obtener las variables de entorno
    // es decir, obtenemos IP y Puerto del servidor
    if(obtenido) return 0;

    ip=getenv("IP_TUPLAS");
    if(ip==NULL) return error(" Error obteniendo env de IP: IP_TUPLAS");
    port=getenv("PORT_TUPLAS");
    if(port==NULL) return error(" Error obteniendo env de puertp: PORT_TUPLAS");

    obtenido=1;

    return 0;
}

int connect_to(int * fd){
    /*Funcion para establecer la conexión con el servidor*/
    struct sockaddr_in socketRemoto;
    bzero(&socketRemoto, sizeof(struct sockaddr_in));

    // necesitamos el nombre entero para el fd
    struct hostent * info_host=gethostbyname(ip);

    // ahora podemos ya configurar el sockaddr
    socketRemoto.sin_family=AF_INET;
    socketRemoto.sin_port=htons(atoi(port));
    if(memcpy(&socketRemoto.sin_addr, info_host-> h_addr_list[0], info_host->h_length)== NULL)return error("Error en memcpy de connect_to");

    // ahora paso de mensajes tcp
    int o = 1;
    *fd=socket(AF_INET, SOCK_STREAM, 0);
    if(*fd<0) return error("Error creando socket para el fd de connect_to");
    if(setsockopt(*fd, IPPROTO_TCP, TCP_NODELAY, &o, sizeof(o))<0) return error("Error en setsockopt de connect_to");
    if(connect(*fd, (struct sockaddr *)&socketRemoto, sizeof(socketRemoto))<0) return error("Error en connect de connect_to");

    conectado=1;
    return 0;

}

int destroy(void){
    // Funcion del proxi para llamar a destroy
    // preparación previa
    if(!obtenido) if(getGlobals()<0) return error("Error en destroy: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en destroy: Error en conexión");

    // envío de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));
    char buff[MAX_LENG]={0};
    pet.cod_op=0;
    if(pet_to_string(&pet, buff)<0) return error("Error en destroy: Error conversion a string en función destroy");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en destroy: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en destroy: Error al recibir");
    struct Respuesta res;
    if(string_to_res(buffRec, &res)<0) return error("Error en destroy: Error al convertir respuesta a objeto");

    int result = res.cod_err==0? 0 : -1;
    return (result);
}


int set_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    // Funcion del proxy para llamar a set_value
    // preparación previa
    if(N_value2<1||N_value2>32)return -1; // BUG DE OVERFLOW SI NO CONTROLAMOS
    if(!obtenido) if(getGlobals()<0) return error("Error en set_value: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en set_value: Error en conexión");

    // preparación de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));

    char buff[MAX_LENG]={0};
    pet.cod_op=1;
    strcpy(pet.key, key);
    strcpy(pet.value1, value1);
    pet.N_value2=N_value2;
    for(int i =0; i<N_value2; ++i) pet.V_value2[i]=V_value2[i];
    pet.value3.x=value3.x;
    pet.value3.y=value3.y;
    pet.value3.z=value3.z;
    
    // envio de peticion
    if(pet_to_string(&pet, buff)<0) return error("Error en set_value: Error conversion a string en función set_value");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en set_value: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en set_value: Error al recibir");
    struct Respuesta res;
    if(string_to_res(buffRec, &res)<0) return error("Error en set_value: Error al convertir respuesta a objeto");

    int result = res.cod_err==0? 0 : -1;
    return (result);
}

int get_value(char *key, char *value1, int *N_value2, float *V_value2, struct Paquete *value3){
    // Funcion del proxy para llamar a get_value
    // preparación previa
    if(!obtenido) if(getGlobals()<0) return error("Error en get_value: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en get_value: Error en conexión");

    // preparación de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));
    char buff[MAX_LENG]={0};
    pet.cod_op=2;
    strcpy(pet.key, key);
    
    // envio de peticion
    if(pet_to_string(&pet, buff)<0) return error("Error en get_value: Error conversion a string en función get_value");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en get_value: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en get_value: Error al recibir");
    struct Respuesta res;
    if(string_to_res(buffRec, &res)<0) return error("Error en get_value: Error al convertir respuesta a objeto");

    // preparar argumentos de vuelta
    if(strcpy(value1, res.value1)==NULL)return error("Error en get_value: Error al copiar el value1 desde la respuesta");
    *N_value2=res.N_value2;
    for(int i=0; i<*N_value2; ++i) V_value2[i] = res.V_value2[i];
    value3->x=res.value3.x;
    value3->y=res.value3.y;
    value3->z=res.value3.z;

    int result = res.cod_err==0? 0 : -1;
    return (result);
}


int modify_value(char *key, char *value1, int N_value2, float *V_value2, struct Paquete value3){
    // Funcion del proxy para llamar a modify_value
    // preparación previa
    if(N_value2<1||N_value2>32)return -1; // BUG DE OVERFLOW SI NO CONTROLAMOS

    if(!obtenido) if(getGlobals()<0) return error("Error en modify_value: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en modify_value: Error en conexión");

    // preparación de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));
    if(N_value2<1||N_value2>32)return -1; // BUG DE OVERFLOW SI NO CONTROLAMOS

    char buff[MAX_LENG]={0};
    pet.cod_op=3;
    strcpy(pet.key, key);
    strcpy(pet.value1, value1);
    pet.N_value2=N_value2;
    for(int i =0; i<N_value2; ++i) pet.V_value2[i]=V_value2[i];
    pet.value3.x=value3.x;
    pet.value3.y=value3.y;
    pet.value3.z=value3.z;
    
    // envio de peticion
    if(pet_to_string(&pet, buff)<0) return error("Error en modify_value: Error conversion a string en función modify_value");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en modify_value: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en modify_value: Error al recibir");
    struct Respuesta res;
    if(string_to_res(buffRec, &res)<0) return error("Error en modify_value: Error al convertir respuesta a objeto");

    int result = res.cod_err==0? 0 : -1;
    return (result);
}

int delete_key(char *key){
    // Funcion del proxy para llamar a delete_key
    // preparación previa

    if(!obtenido) if(getGlobals()<0) return error("Error en delete_key: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en delete_key: Error en conexión");

    // preparación de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));

    char buff[MAX_LENG]={0};
    pet.cod_op=4;
    strcpy(pet.key, key);
    
    // envio de peticion
    if(pet_to_string(&pet, buff)<0) return error("Error en delete_key: Error conversion a string en función delete_key");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en delete_key: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en delete_key: Error al recibir");
    struct Respuesta res;
    if(string_to_res(buffRec, &res)<0) return error("Error en delete_key: Error al convertir respuesta a objeto");

    int result = res.cod_err==0? 0 : -1;
    return (result);
}

int exist(char *key){
    // Funcion del proxy para llamar a exists
    // preparación previa
    if(!obtenido) if(getGlobals()<0) return error("Error en exists: Error en obtencion de var de entorno");
    if(!conectado) if(connect_to(&fdConexion)<0) return error("Error en exists: Error en conexión");

    // preparación de petición
    struct Peticion pet;
    bzero(&pet, sizeof(struct Peticion));
    
    char buff[MAX_LENG]={0};
    pet.cod_op=5;
    strcpy(pet.key, key);
    
    // envio de peticion
    if(pet_to_string(&pet, buff)<0) return error("Error en exists: Error conversion a string en función exists");
    if(sock_send(fdConexion, buff, MAX_LENG)<0) return error("Error en exists: Error al enviar");
    
    // recibir la respuesta
    char buffRec[MAX_LENG]={0};
    if(sock_receive(fdConexion, buffRec, MAX_LENG)<0) return error("Error en exists: Error al recibir");
    struct Respuesta res;
    bzero(&res, sizeof(struct Respuesta));

    if(string_to_res(buffRec, &res)<0) return error("Error en exists: Error al convertir respuesta a objeto");

    int result = res.cod_err;
    return (result);

}