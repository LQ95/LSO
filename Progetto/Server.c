#include "lib.h"

int Login(char *user,char *psw)
{
	//this routine looks up usernames and passwords after having them passed to it from the server
	//returns 1 if login is successful,otherwise it returns 0
	
}
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

//these subroutines handle the game itself
//this subroutine is used to create a data structure that is used to know whether a player is in a certain spot or not.
int **create_position_map(int width,int height)
{
    int x,y;
    int **positions=calloc(sizeof(int*),height);
    int i=0;
    for(int i=0;i<height;i++){
        positions[i]=calloc(sizeof(int),width);
    }
	for(x=0;x<width;x++){
		for(y=0;y<height;y++)
						positions[x][y]=0;
        }
	return positions;
}

int CheckFree(int x,int y,int **position,int width,int height)
{
	if((x>0 && y>0) && (x<width && y<height))
	if(position[x][y]==0) return 1;
	else return 0;
}

int CheckBomb(int coord[2],int **map)
{
	
	if(map[coord[0]][coord[1]]==0)return 1;
	else return 0;
}

int **initPositions(PlayerList L,int **board,int **positions,int height,int width)
{
	//assign starting positions to all players so that they can start the race 
	PlayerList tmp=L;
	int x,y; 
	for(x=0;x<width;x++)
		{
			for(y=0;y<height;y++)
				{
					if (tmp!=NULL)
					{
						//position assignment here
						tmp=tmp->next;
					}						
				}
		}
	return positions;
}

int **initBombs(int **board,int **positions,int height,int width)
{
	//repositions bombs after starting player positions have been given
	int x,y; 
	for(x=0;x<width;x++)
		{
			for(y=0;y<height;y++)
				{
					//bomb redistribution here 
				}
		}
	return board;
}
PlayerList initPlayer(PlayerList L,int **positions)
{
	//tell players their own starting postions
	int size=ListSize(L);
	while(size<0)
	{
		
		size --;
	}
	return L;
}

//WIP
//it basically takes a list of players,and communicates with them for ever move that they make
//the width and height parameters are meant to be same height and width measurements for the board
void ServerGame(int **board,PlayerList L,int width, int height)
{
	int session_status,eliminated=0;
	PlayerList tmp=L;
	PlayerList P;
	int **positions=create_position_map(width,height);
	int nextmove;
	char buf[1000];
	positions=initPositions(L,board,positions,width,height);
	board=initBombs(positions,board,width,height);
	tmp=initPlayer(L,positions);
	while(session_status!=SESSION_END)
	{
		
		while(tmp!=NULL)
			{
				P=tmp;
				read(P->P.socket_desc,buf,sizeof(int));
				nextmove=atoi(buf);
				switch(nextmove)
				{
					case NULL_MOVE:
						break;
					                                                               
					case MOVE_LEFT:
						if(CheckFree(P->P.position[0]-1,P->P.position[1],positions,width,height))
							{
								P->P.position[0]--;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,sizeof(int));
							}
			
						if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,sizeof(int));
								tmp=eliminate(P->P.ID,L);			
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						break;
					
					case MOVE_RIGHT:
						if(CheckFree(P->P.position[0]+1,P->P.position[1],positions,width,height))
							{
								P->P.position[0]++;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,sizeof(int));
								tmp=eliminate(P->P.ID,L);			
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						break;
						
					case MOVE_UP:
						if(CheckFree(P->P.position[0],P->P.position[1]+1,positions,width,height))
							{
								P->P.position[1]++;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,sizeof(int));
								tmp=eliminate(P->P.ID,L);			
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						break;
					
					case MOVE_DOWN:
						if(CheckFree(P->P.position[0],P->P.position[1]-1,positions,width,height))
							{
								P->P.position[1]--;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,sizeof(int));
							}
						if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,sizeof(int));
								tmp=eliminate(P->P.ID,L);			
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						break;						
				}
				if(eliminated==0)tmp=tmp->next;
				else eliminated=0;            //the reason for the use of this variable is that when a player gets eliminated the list will automatically point to the next one,so we don't need to refer to the next one 
			}
		tmp=L;
		if(L==NULL) session_status=SESSION_END;
	}
	
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
