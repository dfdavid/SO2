#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaadr_in pertence a esta libreria
#include <stdlib.h>

#define CRED_LENGTH 20
#define NUM_USERS 2
#define BUFFER_SIZE 100
#define PORT_NUMBER 10000

struct Auth{
    char usr[CRED_LENGTH];
    char psw[CRED_LENGTH];
};

int autenticar(char *user, char *password);
//int getTelemetria(char *ipaddr);
//int getScan(int sockfd);
//int sendUpdate(int sockfd);

int main() {

    int sockfd, pid;
    char buffer[BUFFER_SIZE], buffer_aux[BUFFER_SIZE];
    struct sockaddr_in st_serv, st_cli;


    //Este bloque de codigo testea la funcion autenticar(char user, char pass)
    /*int res_login;
    res_login=autenticar("david","d'andrea747");
    if (res_login==1){
        printf("Login successful");
    }
    else{
        printf("Usr or Psw incorrect");
    }*/

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