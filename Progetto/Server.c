#include "lib.h"

int genseed(int sockfd)
{
    //Crea un seed per la tavola di gioco
    srand(time(NULL));
    int seed[1];
    int n;
    seed[0]=rand();
    write(sockfd, seed, sizeof(seed));
    printf("seed: %d\n",seed[0]);
    return seed[0];

}

void print_board(int **board){
    int i=0;
    int j=0;
    for(i=0;i<10;i++){
        for(j=0;j<10;j++){
            printf("|%d|",board[i][j]);
        }
        printf("\n");
    }
}

int **create_board(int seed){
    int **board=calloc(sizeof(int*),10);
    int i=0;
    for(int i=0;i<10;i++){
        board[i]=calloc(sizeof(int),10);
    }
    //inserisco le mine a seconda del seed, 1 nella matrice rappresenta una mina
    int x=0;
    int y=0;
    for(x=0;x<10;x++){
        for(y=0;y<10;y++)
                if((seed*x*y)%5==2)
            board[x][y]=1;
        }
    //DEBUG print_board(board);
    return board;
}

void ServerLog(char *data)
{
	int fd,size;
	size=sizeof(data);
	fd=open("Log.txt",O_WRONLY|O_CREAT|O_APPEND,S_IRUSR);
	if(write(fd,data,size)<0)
	{
		perror("errore nella compilazione del log");
	}
	close(fd);
}

int main()
{
    //Creazione della connesione TCP
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Fallita creazione socket\n");
        exit(0);
    }
    else
        printf("La creazione del socket ha avuto successo..\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind fallito...\n");
        exit(0);
    }
    else
        printf("Socket bind ha avuto successo..\n");
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen fallito...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept fallito..\n");
        exit(0);
    }
    else
        printf("server acccept avvenuto con sucesso...\n");
    int seed=genseed(connfd);
    int **board=create_board(seed);
    close(sockfd);
}
