#include "lib.h"

void genrcv(int sockfd)
{
    //riceve il seed dal server per generare la board
    int buff[MAX];
    int n;
    int seed=0;
    read(sockfd, buff, sizeof(buff));
    printf("seed: %d\n",buff[0]);

}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Creazione del socket falli9ta...\n");
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
    genrcv(sockfd);
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
		read(sd,buf,SignalSize);
		game_status=atoi(buf);
		if(game_status!=ELIMINATED)
		{
			scanf(" %c",&input);
			while ((getchar()) != '\n'); 
			switch(input)
			{
				case '1':
				sprintf(buf, "%d", DISPLAY_USERS);
				write(sd,buf,SignalSize);
				break;
			
				case '2':
				sprintf(buf, "%d", DISPLAY_USER_LOCATIONS);
				write(sd,buf,SignalSize);
				break;
			
				case '3':
				sprintf(buf, "%d", DISPLAY_USER_DEATHS);
				write(sd,buf,SignalSize);
				break;
			
			
				default:
				sprintf(buf, "%d", NULL_MOVE);
				sprintf(answer, "%d", NULL_MOVE);
				write(sd,buf,SignalSize);
				break;

			}
		if(strcmp(answer,"3000")!=0)
			{
				read(sd,buf,DisplaySignalSize);
				displaysize=atoi(buf);
				read(sd,buf,displaysize) ;
				printf("%s",buf);
			}
		}
	}
	//sends and receives signals from the server,prints the map after every move as long as it participates in the game
}
