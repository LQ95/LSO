#include "lib.h"
#include "scan_int/scan_int.h"
void genrcv(int sockfd)
{
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
int login2(int connfd){
    int succ[1];
    char username[10];
    char pass[10];
    printf("nome utente: ");
    scanf("%s",username);
    printf("password": );
    scanf("%s",pass);
    write(connfd,username,sizeof(username));
    write(connfd,pass1,sizeof(pass));
    read(connfd,succ,sizeof(succ));
    return succ[0];
}
int login(int connfd){
    int choice[1];
    printf("benvenuto!\n1.Effettuare il login\n2.Crea un nuono utente\n");
    choice[0]=scan_int(1,2);
    write(connfd,choice,sizeof(choice));
    if(choice[0]==2)
        sign_up(connfd);
    else if(choice[0]==1){
        int succ_login=0;
        while(!succ_login){
            succ_login=login2(connfd);
        }
    }
    return 1;

}

int main()
{
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
        login(sockfd);
    //genrcv(sockfd);
    while(1){

    }
    close(sockfd);
}
//the sd variable is the Client's own socket descriptor once it's been connected to the server,and the position and bomb matrices are passed by the server
//the moveflag variable is used to signal to the client how is has mmoved if it received the MOVE_OK signal
//the ID variable is the client's Player ID in the current game(has to be sent from the server)
void ClientGame(int sd,int **board,int **positions)
{
	int game_status,displaysize,moveflag,ID;
	game_status=LOGIN_OK;
	char input;
	char buf[BUFDIM];
	char *answer=calloc(6,sizeof(char));
	while(game_status!=SESSION_END)
	{
		scanf(" %c",&input);
		while ((getchar()) != '\n'); //this is used to clean stdin
		switch(input)
		{
			case 'w':
			sprintf(buf, "%d", MOVE_UP);
			moveflag=0;
			write(sd,buf,SignalSize);
			break;

			case 's':
			sprintf(buf, "%d", MOVE_DOWN);
			moveflag=1;
			write(sd,buf,SignalSize);
			break;

			case 'a':
			sprintf(buf, "%d", MOVE_LEFT);
			moveflag=2;
			write(sd,buf,SignalSize);
			break;

			case 'd':
			sprintf(buf, "%d", MOVE_RIGHT);
			moveflag=3;
			write(sd,buf,SignalSize);
			break;

			case 'm':
			sprintf(buf, "%d", QUIT);
			write(sd,buf,SignalSize);
			break;

			case '0':
			sprintf(buf, "%d", NULL_MOVE);
			write(sd,buf,SignalSize);
			break;

			default:
			sprintf(buf, "%d", NULL_MOVE);
			write(sd,buf,SignalSize);
			break;
		}


			}
		if(strcmp(answer,"3000")!=0)
			{
				read(sd,buf,DisplaySignalSize);
				displaysize=atoi(buf);
				read(sd,buf,displaysize) ;
				printf("%s",buf);
			}
		}
	//sends and receives signals from the server,prints the map after every move as long as it participates in the game
}
