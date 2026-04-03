// $>export IP_TUPLAS=localhost
// $>export PORT_TUPLAS=0777

// ----------- COMPROBAR IP/PUERTO EXISTEN ----------- //

// includes genéricos (ver luego si sobran)
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "claves.h"
#include "sock.h"
#include <pthread.h>

int error(char * message){
    printf("\n Error en el código del servidor: %s\n", message);
    return -1;
}

struct argumento{
    int fd;
};

void * worker(void * argum){
    /*Funcion del thread trabajador*/
    // detatch para tener muchos

    // obtener los argumentos y evitar condicion de carrera
    int fd=*((int*)argum);

    pthread_detach(pthread_self());

    char buff[MAX_LENG]={0};

    struct Peticion pet;
    struct Respuesta res;

    while(sock_receive(fd, buff, MAX_LENG)>0){

        // obtener de string
        bzero(&pet, sizeof(struct Peticion));
        if(string_to_pet(buff, &pet)<0) {
            printf("Error en la creación de Struct Peticion");
            if(close(fd)<0){
                printf("Error cerrando fd de conexión");
                break;}
            break;}

        // operar
        int err = 0;

        bzero(&res, sizeof(struct Respuesta));

        switch(pet.cod_op){
        case 0:
            err=destroy();
            break;
        case 1:
            err = set_value(pet.key, pet.value1, pet.N_value2, pet.V_value2, pet.value3);
            break;
        
        case 2:
            err = get_value(pet.key, res.value1, &res.N_value2, res.V_value2, &res.value3);
            break;
        case 3:
            err = modify_value(pet.key, pet.value1, pet.N_value2, pet.V_value2, pet.value3);
            break;
        case 4:
            err = delete_key(pet.key);
            break;
        case 5:
            err = exist(pet.key);
            break;
        }
        res.cod_err=err;

        // ahora, enviar
        bzero(buff, MAX_LENG);
        if(res_to_string(&res, buff)<0){
            printf("Error en creación de string de respuesta");
            if(close(fd)<0){
                printf("Error cerrando fd de conexión");
                break;}       
            break;}
        if(sock_send(fd, buff, MAX_LENG)<0) {
            printf("Error en envío de string de respuesta");
            if(close(fd)<0){
                printf("Error cerrando fd de conexión");
                break;}
            break;}
        }

    if(close(fd)<0){
        printf("Error cerrando fd de conexión");
        }
    pthread_exit(NULL);
}

int main(int argc, char **argv){
    /*Código principal del hilo principal
     * Será el que: 1o Comrpuebe que el argumento pasado es el puerto existente para recibir peticiones. 
     * luego crea otro puerto para la comunicación con el cliente.
     * finalmente asigna la tarea a un thread, que se encarga de devolver el resultado*/
    // checkeamos el argumento 
    if(argc!=2) return error("Introduzca como parámetro el puerto para recepción de requests");
    
    // preparar la direccion propia y hacer bind
    struct sockaddr_in mySocket;
    int myAddress=atoi(argv[1]);

    bzero(&mySocket, sizeof(struct sockaddr_in));
    mySocket.sin_addr.s_addr=INADDR_ANY;
    mySocket.sin_family=AF_INET;
    mySocket.sin_port = htons(myAddress);

    int o=1;
    int server_fd=socket(AF_INET, SOCK_STREAM, 0);
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &o, sizeof(o))<0) return error("En setsockopt");

    if(bind(server_fd, (struct sockaddr *)&mySocket, sizeof(mySocket))<0) return error("En bind");

    // Conexión TCP
    // Escuchar y aceptar

    if(listen(server_fd, SOMAXCONN)<0) return error("Error en listen");

    // loop principal
    while(1){
        struct sockaddr_in socketRemote;
        socklen_t s = sizeof(socketRemote);
        // accept es bloqueante, se espera en la sig linea hasta que llegue request

        int fd_client = accept(server_fd,(struct sockaddr *)&socketRemote, &s);
        if(fd_client<0) {
            printf("Error en accept");
            continue;}
        // Ahora queda recibir, operar y enviar: se encarga el thread dado el fd
        int *fd_client_ptr=malloc(sizeof(int));
        *fd_client_ptr=fd_client;
        pthread_t id;
        pthread_create(&id, NULL, (void *)worker, fd_client_ptr);
        // evitar condicion de carrera por el arg
        
    }
    return -1;
 }