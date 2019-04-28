#include "lib_client.h"
#include "scan_int/scan_int.h"

//the sd variable is the Client's own socket descriptor once it's been connected to the server,and the position and bomb matrices are passed by the server
//the moveflag variable is used to signal to the client how is has mmoved if it received the MOVE_OK signal
//the ID variable is the client's Player ID in the current game(has to be sent from the server)

void genrcv(int sockfd){
    //riceve il seed dal server per generare la board
    int buff[1];
    int seed=0;
    read(sockfd, buff, sizeof(buff));
    printf("seed: %d\n",buff[0]);

}

void sign_up(int connfd){
    char username[10];
    char pass1[10];
    char pass2[10];
    printf("Nome Utente: ");
    scanf("%s",username);
    do{
        printf("Password: ");
        scanf("%s",pass1);
        printf("Reinserire Password: ");
        scanf("%s",pass2);
    }
    while((strcmp(pass1,pass2))!=0);
    write(connfd,username,sizeof(username));
    write(connfd,pass1,sizeof(pass1));
    return;
    }

int login(int connfd){
    int succ[1];
    succ[0]=0;
    char username[10];
    char pass[10];
    printf("nome utente: ");
    scanf("%s",username);
    printf("password:" );
    scanf("%s",pass);
    write(connfd,username,sizeof(username));
    write(connfd,pass,sizeof(pass));
    read(connfd,succ,sizeof(succ));
    return succ[0];
}

int *menu(int connfd){
    int choice[1];
    int seed[1];
    int dim[1];
    int *out;
    out=malloc(sizeof(int)*2);
    clear();
    printf("benvenuto!\n1.Effettuare il login\n2.Crea un nuono utente\n");
    choice[0]=scan_int(1,2);
    write(connfd,choice,sizeof(choice));
    if(choice[0]==2){
        sign_up(connfd);
        printf("Nuovo utente Registrato!\n\n");
    }
    int succ_login=0;
    succ_login=login(connfd);
    while(!succ_login){
        printf("login errato\n");
        succ_login=login(connfd);
        }
    read(connfd, out, sizeof(out));
    return out;
}

int main(){
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Creazione del socket fallita...\n");
        exit(0);
    }
    else
        printf("Socket creato con successo..\n");
    bzero(&servaddr, sizeof(servaddr));
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connessione al server fallita...\n");
        exit(0);
    }
    else
        printf("connesso al server..\n");
        int *seeddim;
        seeddim=menu(sockfd);
    printf("seed:%d\n", seeddim[0]);
    printf("dimension:%d\n", seeddim[1]);
    client_game(sockfd,seeddim[1]);
    close(sockfd);
}
