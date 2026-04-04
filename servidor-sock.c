// $>export IP_TUPLAS=localhost
// $>export PORT_TUPLAS=8080 <- cambio de puerto a uno no restringido por SO

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

pthread_mutex_t mutex_db = PTHREAD_MUTEX_INITIALIZER;

int error(char * message){
    printf("\n Error en el código del servidor: %s\n", message);
    return -1;
}

struct argumento{
    int sd;
};

void * worker(void * argum){
    /*Funcion del thread trabajador*/
    // detatch para tener muchos

    // obtener los argumentos y evitar condicion de carrera
    int sd=*((int*)argum);

    pthread_detach(pthread_self());

    char buff[MAX_LENG]={0};

    struct Peticion pet;
    struct Respuesta res;

    while(sock_receive(sd, buff, MAX_LENG)>0){

        // obtener de string
        bzero(&pet, sizeof(struct Peticion));
        if(string_to_pet(buff, &pet)<0) {
            printf("Error en la creación de Struct Peticion");
            if(close(sd)<0){
                printf("Error cerrando sd de conexión");
                break;}
            break;}

        // operar
        int err = 0;

        bzero(&res, sizeof(struct Respuesta));

        pthread_mutex_lock(&mutex_db);

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

        pthread_mutex_unlock(&mutex_db);

        // ahora, enviar
        bzero(buff, MAX_LENG);
        if(res_to_string(&res, buff)<0){
            printf("Error en creación de string de respuesta");
            if(close(sd)<0){
                printf("Error cerrando sd de conexión");
                break;}       
            break;}
        if(sock_send(sd, buff, MAX_LENG)<0) {
            printf("Error en envío de string de respuesta");
            if(close(sd)<0){
                printf("Error cerrando sd de conexión");
                break;}
            break;}
        }

    if(close(sd)<0){
        printf("Error cerrando sd de conexión");
        }
    pthread_exit(NULL);
}

int main(int argc, char **argv){
    /*Código principal del hilo principal
     * Será el que: 1o Comrpuebe que el argumento pasado es el puerto existente para recibir peticiones. 
     * luego crea otro puerto para la comunicación con el cliente.
     * finalmente asigna la tarea a un thread, que se encarga de devolver el resultado*/
    // checkeamos el argumento 
    if(argc!=2) return error("Uso: ./servidor <num_puerto>");
    
    // preparar la direccion propia y hacer bind
    struct sockaddr_in mySocket;
    int myAddress=atoi(argv[1]);

    bzero(&mySocket, sizeof(struct sockaddr_in));
    mySocket.sin_addr.s_addr=INADDR_ANY;
    mySocket.sin_family=AF_INET;
    mySocket.sin_port = htons(myAddress);

    int o=1;
    int server_sd=socket(AF_INET, SOCK_STREAM, 0);
    if(setsockopt(server_sd, SOL_SOCKET, SO_REUSEADDR, &o, sizeof(o))<0) return error("En setsockopt");

    if(bind(server_sd, (struct sockaddr *)&mySocket, sizeof(mySocket))<0) return error("En bind");

    // Conexión TCP
    // Escuchar y aceptar

    if(listen(server_sd, SOMAXCONN)<0) return error("Error en listen");

    // loop principal
    while(1){
        struct sockaddr_in socketRemote;
        socklen_t s = sizeof(socketRemote);
        // accept es bloqueante, se espera en la sig linea hasta que llegue request

        int sd_client = accept(server_sd,(struct sockaddr *)&socketRemote, &s);
        if(sd_client<0) {
            printf("Error en accept");
            continue;}
        // Ahora queda recibir, operar y enviar: se encarga el thread dado el sd
        int *sd_client_ptr=malloc(sizeof(int));
        *sd_client_ptr=sd_client;
        pthread_t id;
        pthread_create(&id, NULL, (void *)worker, sd_client_ptr);
        // evitar condicion de carrera por el arg
        
    }
    return -1;
 }