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
void ClientGame(int sd,int **board,int **positions)
{
	int game_status;
	game_status=LOGIN_OK;
	char input;
	char buf[BUFDIM];
	char *answer;
	while(game_status!=SESSION_END)
	{
		switch(input)
		{
			case 'w':
			sprintf(buf, "%d", MOVE_UP);
			write(sd,buf,BUFDIM);
			break;

			case 's':
			sprintf(buf, "%d", MOVE_DOWN);
			write(sd,buf,BUFDIM);
			break;

			case 'a':
			sprintf(buf, "%d", MOVE_LEFT);
			write(sd,buf,BUFDIM);
			break;

			case 'd':
			sprintf(buf, "%d", MOVE_RIGHT);
			write(sd,buf,BUFDIM);
			break;

		}

	}
	//sends and receives signals from the server,prints the map after every move as long as it participates in the game
}
