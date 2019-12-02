#include <stdio.h>
#include <string.h>

#define CRED_LENGTH 20
#define NUM_USERS 2

struct Auth{
    char usr[CRED_LENGTH];
    char psw[CRED_LENGTH];
};

int verificar(char *user, char *password);
//int getTelemetria(char *ipaddr);
//int getScan(int sockfd);
//int sendUpdate(int sockfd);

int main() {

    int res_login;
    res_login=verificar("david","d'andrea747");
    if (res_login==1){
        printf("Login successful");
    }
    else{
        printf("Usr or Psw incorrect");
    }

    return 0;
}


int verificar(char *user, char *password){

    struct Auth users[NUM_USERS]= {  {"admin", "admin"} , {"david","d'andrea747"}  };
    struct Auth check;

    strcpy(check.usr, user);
    strcpy(check.psw, password);
    for (int i=0; i< NUM_USERS; i++ ) {
        if (strcmp(check.usr, users[i].usr )==0 && strcmp(check.psw, users[i].psw)==0) return 1;
    }//end for
    return 0;

}//end verificar