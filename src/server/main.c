#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h> // sockaadr_in pertence a esta libreria
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CRED_LENGTH 20
#define NUM_USERS 2
#define BUFFER_SIZE 1000
#define PORT_NUMBER 5520
#define UDP_PORT 5521
#define RETRY 3
#define FIRMWARE_FILE "./firmware_cliente"
#define FILE_BUFFER_SIZE 2000


struct Auth{
    char usr[CRED_LENGTH];
    char psw[CRED_LENGTH];
};

int autenticar(char *user, char *password);
int getTelemetria(char *ipaddr);
//int getScan(int sockfd);
int sendUpdate(int sockfd);

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
    //autenticacion
    /*
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
                printf("\n");
                printf("    Seleccione una opcion: \n");
                printf("1 - Update Satellite Firmware \n");
                printf("2 - Start Scanning \n");
                printf("3 - Get Telemetry \n\n");
                printf("Opcion  " );
                scanf("%d", &opt);  //scanf ( tipo_de_dato_a_leer, puntero al dato)
                //fgets(opt, strlen(opt), stdin);
                if ( opt==1 | opt==2 | opt==3 ){
                    opt_valid=true;
                }
            }
            printf("Ha elegido %d \n", opt);

            //switch por opciones
            switch(opt){
                case 1:
                    //printf("todavia no se implemento esta funcionalidad\n");
                    memset(send_buffer, 0, sizeof(send_buffer));
                    printf("sending satellite firmware version...  \n");
                    strcpy(send_buffer, "1");
                    if ( send(newsockfd, send_buffer, sizeof(send_buffer), 0 ) < 0 ){
                        perror("error al enviar solicitud de firmaware update");
                    }
                    memset(send_buffer, 0, sizeof(send_buffer));
                    sendUpdate(newsockfd);

                    break;

                case 2:
                    //printf("todavia no se implemento esta funcionalidad\n");
                    memset(send_buffer, 0, sizeof(send_buffer));
                    strcpy(send_buffer, "2");
                    printf("sending scannig request... \n");
                    if ( send(newsockfd, send_buffer, sizeof(send_buffer), 0 ) < 0 ){
                        perror("error al enviar solicitud de Start scaning");
                    }
                    memset(send_buffer, 0, sizeof(send_buffer));
                    break;

                case 3:
                    //get telemetria
                    printf("getting telemetry \n");
                    strcpy(send_buffer, "3");
                    if ( send(newsockfd, send_buffer, sizeof(send_buffer), 0 ) < 0 ){
                        perror("error al enviar solicitud de get telemetry");
                    }
                    memset(send_buffer, 0, sizeof(send_buffer));
                    getTelemetria(inet_ntoa( st_cli.sin_addr ));
                    //printf("%s \n", inet_ntoa( st_cli.sin_addr )); // se le pasa ip en formato ascii la ip del cliente
                    break;

                default:
                    break;
            }

            //codigo viejo. Anda pero hay que usarlo para algo o sacarlo a la mier
            /*
                if( recv(newsockfd, send_buffer, sizeof(send_buffer), 0) < 0){
                    perror("error en recv()" );
                    continue; //ojo aca, que hace en realidad el contnue?
                } //end if

                else{

                    printf( "PROCESO %d \n", getpid() );
                    printf( "Recibí: %s \n", send_buffer );
                    //muestra el tamano del send_buffer en bytes. El send_buffer del servidor y cliente deberian ser iguales en tamano para evitar errores, por ejemplo, si el buff_cli es 1000 y el buff_srv es 100, el server se bloqueara para esperar una recepcion despues de leer el send_buffer 10 veces. Me paso (David)
                    printf("bytes recibidos: %d  \n", ret_recv);

                    if (send( newsockfd, send_buffer, sizeof(send_buffer), 0 ) < 0 ){
                        perror("error al enviar desde el server");
                        continue;
                    }
                }//end else*/

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

int getTelemetria(char *ipaddr){
    printf("ud invoco la funcion get telemetria \n");

    int sockudp;
    struct sockaddr_in dest_addr;
    socklen_t dest_size= sizeof(dest_addr);
    socklen_t *dest_addr_size_p=&dest_size;
    char bufferudp[BUFFER_SIZE], bufferudp2[BUFFER_SIZE];
    //char *word = null;


    //creacion del socket UDP
    sockudp= socket(AF_INET, SOCK_DGRAM, 0);
    if (sockudp < 0){
        perror("error creando socket UDP");
        exit(-2);
    }

    //limpieza y llendo de la estructura
    /* Limpieza de la estructura */
    memset(&dest_addr, 0, sizeof(dest_addr));
    /* Carga de la familia de direccioens */
    dest_addr.sin_family=AF_INET;
    /* Carga del número de puerto. Se usa 'htons para darle el orden correcto a los bytes' */
    dest_addr.sin_port=htons(UDP_PORT);
    //Carga de dirección IPv4 del socket
    /*
     composicion de la estructura sockadd_in:

     struct sockaddr_in {
            short int sin_family;
            unsigned short int sin_port;
            struct in_addr sin_addr;
    };

    //para mas detalles sobre inet_aton consultar:
    //https://linux.die.net/man/3/inet_aton
    //int inet_aton(const char *cp, struct in_addr *inp);
    */
    inet_aton(ipaddr, &dest_addr.sin_addr);
    printf("se cargo en la estructura la direccion:\n");
    printf("%s\n", inet_ntoa(dest_addr.sin_addr));

    //se configuran las opciones del socket con setsocketopt(). Solo se hace bind del lado del srevidor. El servidor UDP es el satelite
    int socksize;
    unsigned int m= sizeof(socksize);
    //int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
    setsockopt(sockudp, SOL_SOCKET, SO_REUSEADDR, &m, sizeof(m)); // defino que la direccion del socket puede ser reutilizada por el SO

    memset(bufferudp, 0, sizeof(bufferudp));
    strcpy(bufferudp, "get_tel");
    sleep(2);
    sendto(sockudp, bufferudp, sizeof(bufferudp), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    //ssize_t sendto(int socket, const void *message, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len);

    while( strcmp(bufferudp, "udp_complete") != 0 ){
        memset(bufferudp, 0, sizeof(bufferudp));
        recvfrom(sockudp, bufferudp, sizeof(bufferudp), 0, (struct sockaddr *)&dest_addr, dest_addr_size_p );
        //ssize_t recvfrom(int socket, void *buffer, size_t length, int flags, struct sockaddr *address, socklen_t *address_len);
        char telemetria[BUFFER_SIZE]="";
        char *token=&telemetria[0];
        /*while(token!=NULL){
            token = strtok(bufferudp, "|");
            strcat(telemetria, token);
        }*/
        strcpy(telemetria, bufferudp);
        printf("Telemetria: %s\n", telemetria);

    }
    shutdown(sockudp, SHUT_WR);  //originalmente torce le puso 2 y no la macro
    close(sockudp);
}

int sendUpdate(int sockfd_arg){
    //replicate example function
    int firmware_fd;
    struct stat wtf;
    char *filename=FIRMWARE_FILE;

    //intento abrir el archivo de firmaware en este caso:
    if ( ( firmware_fd = open(filename, O_RDONLY) ) < 0 ){
        perror("error al abrir el archivo de firmware");
        return -1; //
    }

    int count;
    char buffer_envio[FILE_BUFFER_SIZE];

    fstat(firmware_fd, &wtf);
    off_t filesize = wtf.st_size;
    printf("DEBUG: tamaño del archivo a tranmitir: %ld\n", filesize);
    uint32_t bytes = htonl(filesize);

    char *s_bytes = (char *)&bytes;
    printf("DEBUG: n° de bytes a enviar: %i\n", ntohl(bytes));

    //intento enviar de a bytes lo que esta apuntando s_bytes, como esta compuesto s_bytes ya tiene que ver con el manejo de archivos en libc
    if ( send(sockfd_arg, s_bytes, sizeof(bytes), 0) < 0 ){
        perror("error al enviar al archivo");
    }

    //voy leyendo el archivo de a partes del tamano del buffer de envio y voy enviando
    while ( ( count=(int) read(firmware_fd, buffer_envio, FILE_BUFFER_SIZE) ) > 0 ){
        //ssize_t send(int socket, const void *buffer, size_t length, int flags);
        // voy enviando al socket el buffer de envio que es lo que lei recien
        if (send( sockfd_arg, buffer_envio, count, 0 ) < 0 ){
            perror("error enviando el archivo, (dentro del while)");
        }
        memset(buffer_envio, 0, BUFFER_SIZE);
    }
    close(firmware_fd); //cierro el archivo
    return 1; //return int 1: recepcion exitosa

}