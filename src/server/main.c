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
#define RETRY 3

struct Auth{
    char usr[CRED_LENGTH];
    char psw[CRED_LENGTH];
};

int autenticar(char *user, char *password);
//int getTelemetria(char *ipaddr);
//int getScan(int sockfd);
//int sendUpdate(int sockfd);

int main() {

    struct Auth current_user;
    int sockfd, newsockfd, pid, ret_recv;
    socklen_t cli_length;
    char send_buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];
    memset(send_buffer, 0, sizeof(send_buffer));
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


    //Antes del bind, se configuran las opciones del socket
    const int valor=1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &valor, sizeof(valor)); // defino que la direccion del socket puede ser reutilizada por el SO
    int socksize;
    unsigned int m= sizeof(socksize);
    getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&socksize, &m); // obtengo el tamaño del buffer del socket por default y lo guardo en socksize
    printf("CONTROL: Tamaño del socket TCP %i\n", socksize);


    //hacer el bind
    //Upon successful completion, bind() returns 0. Otherwise, -1 is returned and errno is set to indicate the error.
    int bind_result;
    bind_result=  bind(sockfd, (struct sockaddr *)&st_serv, sizeof(st_serv) );
    if (bind_result < 0 ){
        perror("binding error");
        exit(2);
    }


    //invocar a listen(int sockfd, int maxConections) Esta funcion Convierte al Socket en un socket servidor
    //Upon successful completions, listen() returns 0. Otherwise, -1 is returned and errno is set to indicate the error.
    int listening=listen(sockfd, 3);
    if( listening < 0){
        perror("listening error");
    }

    //prompt 1
    /*//autenticacion
    bool auth = false;
    int intentos=RETRY;
    memset(&current_user, (char)NULL, sizeof(current_user) );
    while(auth == false) {
        printf("Autenticacion necesaria \n");
        printf("Usuario: ");
        fgets(current_user.usr, sizeof(current_user.usr)-1, stdin);
        current_user.usr[strlen(current_user.usr)-1]='\0';

        printf("Password: ");
        fgets(current_user.psw, sizeof(current_user.psw)-1, stdin);
        current_user.psw[strlen(current_user.psw)-1]='\0';

        //si al llamar a autenticar obtengo 0 informo el error y descuento un intento
        if (autenticar(current_user.usr, current_user.psw) == 0) {
            intentos--;
            if(intentos==0){
                printf("Limite de intentos fallidos alncanzado \n");
                exit(3);
            }
            printf("Usuario o Password incorecto, queda/n %i intento/s \n", intentos);
        }

            // si el llamado a autenticar devuelve 1, el login es exitoso y escapo del bucle
        else {
            auth = true;
        }
    }//fin de la autenticacion*/
    printf("Login successfull...\n \n");

    cli_length = sizeof(st_cli);

    //prompt 2 (already logged)
    while (1){
        printf("%s\\> Esperando una conexion... \n", current_user.usr);

        //Upon successful completion, accept() returns the nonnegative file descriptor of the accepted socket. Otherwise, -1 is returned and errno is set to indicate the error.
        newsockfd= accept(sockfd, (struct sockaddr *)&st_cli, &cli_length);

        if (  newsockfd < 0 ){
            perror("accept(): no se acepto la conexion");
            exit(1);
        }
        printf("%s\\> Conexion entrante... \n\n\n", current_user.usr);
        sleep(1);

        bool terminar=false;
        while( terminar==false) {

            bool opt_valid= false;
            int opt=0;
            while(opt_valid != true){ //menu de operaciones
                printf("    Seleccione una opcion: \n");
                printf("1 - Update Satellite Firmware \n");
                printf("2 - Start Scanning \n");
                printf("3 - Get Telemetry \n\n");
                printf("Opcion  " );
                scanf("%d", &opt);
                //fgets(opt, strlen(opt), stdin);
                if ( opt==1 | opt==2 | opt==3 ){
                    opt_valid=true;
                }
            }
            printf("Ha elegido %d \n", opt);

            ret_recv= recv(newsockfd, send_buffer, sizeof(send_buffer), MSG_WAITALL);
            if( ret_recv < 0){
                perror("error en recv()" );
                continue; //ojo aca, que hace en realidad del contnue?
            }

            else{

                printf( "PROCESO %d \n", getpid() );
                printf( "Recibí: %s \n", send_buffer );
                //muestra el tamano del send_buffer en bytes. El send_buffer del servidor y cliente deberian ser iguales en tamano para evitar errores, por ejemplo, si el buff_cli es 1000 y el buff_srv es 100, el server se bloqueara para esperar una recepcion despues de leer el send_buffer 10 veces. Me paso (David)
                printf("bytes recibidos: %d  \n", ret_recv);

                if (send( newsockfd, send_buffer, sizeof(send_buffer), 0 ) < 0 ){
                    perror("error al enviar desde el server");
                    continue;
                }
            }//end

            if(strcmp(send_buffer, "fin\n") == 0  ){
                terminar=true;
            }

        }//end while
        //}//end if "proceso hijo"

//        else{//proceso padre
//            // https://linux.die.net/man/2/close
//            close(newsockfd);
        printf("El cliente cerro la conexion con el comando 'fin'  \n");
//            continue;
    }//end while "big server loop"




    return 0;
}

//
int autenticar(char *user, char *password){

    struct Auth users[NUM_USERS]= {  {"admin", "admin"} , {"david","dandrea747"}  };
    struct Auth check;

    strcpy(check.usr, user);
    strcpy(check.psw, password);
    for (int i=0; i< NUM_USERS; i++ ) {
        if (strcmp(check.usr, users[i].usr )==0 && strcmp(check.psw, users[i].psw)==0) return 1;
    }//end for
    return 0;

}//end autenticar