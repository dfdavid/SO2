#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaadr_in pertence a esta libreria
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#define CRED_LENGTH 20
#define NUM_USERS 2
#define BUFFER_SIZE 1000
#define PORT_NUMBER 5520

struct Auth{
    char usr[CRED_LENGTH];
    char psw[CRED_LENGTH];
};

int autenticar(char *user, char *password);
//int getTelemetria(char *ipaddr);
//int getScan(int sockfd);
//int sendUpdate(int sockfd);

int main() {

    int sockfd, newsockfd, pid, ret_recv;
    socklen_t cli_length;
    char buffer[BUFFER_SIZE], buffer_aux[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    struct sockaddr_in st_serv, st_cli;


    //creacion del socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //aca maneja el error si eventualmente el socket no se abre
    if(sockfd <0){
        perror("error al invocar 'socket' ");
        exit(1);
    }

    /* Limpieza de la estructura */
    memset( &st_serv, 0, sizeof(st_serv ));
    /* Carga de la familia de direccioens */
    st_serv.sin_family=AF_INET;
    /* Carga del número de puerto. Se usa 'htons para darle el orden correcto a los bytes' */
    st_serv.sin_port=htons(PORT_NUMBER);
    /* Carga de dirección IPv4 del socket, */
    st_serv.sin_addr.s_addr= INADDR_ANY; //INADDR_ANY es una macro que contiene/obtiene la ip local del servidor
    //https://es.stackoverflow.com/questions/168128/duda-sobre-inaddr-any


    //hacer el bind
    //Upon successful completion, bind() returns 0. Otherwise, -1 is returned and errno is set to indicate the error.
    int bind_result;
    bind_result=  bind(sockfd, (struct sockaddr *)&st_serv, sizeof(st_serv) );
    if (bind_result < 0 ){
        perror("binding error");
        exit(EXIT_FAILURE);
    }


    //invocar a listen(int sockfd, int maxConections) Esta funcion Convierte al Socket en un socket servidor
    //Upon successful completions, listen() returns 0. Otherwise, -1 is returned and errno is set to indicate the error.
    int listening=listen(sockfd, 3);
    if( listening < 0){
        perror("listening error");
    }

    //prompt


    cli_length = sizeof(st_cli);

    while (1){
        printf("Esperando una conexion... \n");

        //Upon successful completion, accept() returns the nonnegative file descriptor of the accepted socket. Otherwise, -1 is returned and errno is set to indicate the error.
        newsockfd= accept(sockfd, (struct sockaddr *)&st_cli, &cli_length);

        if (  newsockfd < 0 ){
            perror("accept(): no se acepto la conexion");
            exit(1);
        }
        printf("Conexion entrante... \n");

        //Conceptos escenciales sobre el uso de la "syscall" fork()

        // #1 http://man7.org/linux/man-pages/man2/fork.2.html
        // #2 https://www.programacion.com.py/escritorio/c/creacion-y-duplicacion-de-procesos-en-c-linux
        // #3 https://es.stackoverflow.com/questions/179414/como-funciona-la-funci%C3%B3n-fork

        /*On success, the PID of the child process is returned in the parent,
        and 0 is returned in the child.  On failure, -1 is returned in the
        parent, no child process is created, and errno is set appropriately.*/
//        if ( (pid=fork()) < 0){
//            perror("fork(): error");
//            exit(1);
//        }
//
//        if (pid==0){ //proceso hijo
//            close(sockfd);

            bool terminar=false;
            while( terminar==false) {

                ret_recv= recv(newsockfd, buffer, sizeof(buffer), MSG_WAITALL);

                if( ret_recv < 0){
                    perror("error en recv()" );
                    continue; //ojo aca, que hace en realidad del contnue?
                }

                else{

                    printf( "PROCESO %d \n", getpid() );
                    printf( "Recibí: %s \n", buffer );
                    //muestra el tamano del buffer en bytes. El buffer del servidor y cliente deberian ser iguales en tamano para evitar errores, por ejemplo, si el buff_cli es 1000 y el buff_srv es 100, el server se bloqueara para esperar una recepcion despues de leer el buffer 10 veces. Me paso (David)
                    printf("bytes recibidos: %d  \n", ret_recv);

                    if (send( newsockfd, buffer, sizeof(buffer), 0 ) < 0 ){
                        perror("error al enviar desde el server");
                        continue;
                    }
                }//end

                if(strcmp(buffer, "fin\n") == 0  ){
                    terminar=true;
                }

            }//end while
        //}//end if "proceso hijo"

//        else{//proceso padre
//            // https://linux.die.net/man/2/close
//            close(newsockfd);
            printf("Se ha establecido una conexion \n");
//            continue;
        }//end while "big server loop"




    return 0;
}


int autenticar(char *user, char *password){

    struct Auth users[NUM_USERS]= {  {"admin", "admin"} , {"david","d'andrea747"}  };
    struct Auth check;

    strcpy(check.usr, user);
    strcpy(check.psw, password);
    for (int i=0; i< NUM_USERS; i++ ) {
        if (strcmp(check.usr, users[i].usr )==0 && strcmp(check.psw, users[i].psw)==0) return 1;
    }//end for
    return 0;

}//end autenticar