#include "lib.h"
pthread_mutex_t sem=PTHREAD_MUTEX_INITIALIZER;
struct thread_data{
    int seed;
    int connfd;
    FILE *db;
};
int Login(char *user,char *psw)
{
	//this routine looks up usernames and passwords after having them passed to it from the server
	//returns 1 if login is successful,otherwise it returns 0

}
int genseed()
{
    //Crea un seed per la tavola di gioco
    srand(time(NULL));
    int seed[1];
    int n;
    seed[0]=rand();
    printf("seed: %d\n",seed[0]);
    return seed[0];

}
int login(int connfd){
    char username[10];
    char password[10];
    write(connfd,username,sizeof(username));
    write(connfd,password,sizeof(password));
    FILE *fp=fopen("login_credentials.db","r");
    fscanf(fp,"%s",username);
    fscanf(fp,"%s",password);
    fclose(fp);
    printf("%s\n%s",username,password);
}
int sendseed(void *arg){
    struct thread_data *tmp=arg;
    int connfd=tmp->connfd;
    int seed[1];
    seed[0]=tmp->seed;
    printf("%d %d\n",connfd,seed[0]);
    int choice[1];
    read(connfd,choice,sizeof(choice));
    //1=login 2=sign up
    if(choice[0]==2){
        char username[10];
        char password[10];
        read(connfd,username,sizeof(username));
        read(connfd,password,sizeof(password));
        pthread_mutex_lock(&sem);
        FILE *fp=fopen("login_credentials.db","a");
        fprintf(fp,"%s %s\n",username,password);
        printf("new user created\n");
        pthread_mutex_unlock(&sem);
        fclose(fp);
    }
    int login_successful=0;
    while(!login_successful){
        login_successful=login(connfd);
    }
    write(connfd, seed, sizeof(seed));
    return 0;
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
	int size=ListSize(L);
	PlayerList tmp;
	tmp=L;
	int x,y;
	x=y=0;
	while(size<0)
	{
		if(x<width && y<height)
			{
				positions[x][y]=tmp->P.ID;
				x=x+2;
				size--;
			}
		else if(y<height)
			{
				y++;
				if (x=width) x=1;
				else x=0;
			}
		else size=-1;
	}
	return positions;
}

int **initBombs(int **board,int **positions,int height,int width)
{
	//repositions bombs after starting player positions have been given,by eliminating any bombs that might have ended up in
	int x,y;
	for(x=0;x<width;x++)
		{
			for(y=0;y<height;y++)
				{
					if (positions[x][y]!=0) board[x][y]=0;
				}
		}
	return board;
}
PlayerList initPlayer(PlayerList L,int **positions,int height,int width)
{
	//tell players their own starting postions
	int size=ListSize(L);
	PlayerList P;
	int x,y;
	x=y=0;
	while(size<0)
	{
		if(x<width && y<height)
			{
				if((P=search(positions[x][y],L))!=NULL)
				{
				P->P.position[0]=x;
				P->P.position[1]=y;
				size--;
				}
				x++;
			}
		else if(y<height)
			{
				y++;
			}
		else size=-1;
	}
	return L;
}

char *display(PlayerList L,int flag,PlayerList deaths,char *data)
{
	data=malloc(sizeof(char)*BUFDIM);
	char entry[40];
	int i=1;
	PlayerList tmp=L;
	if (flag==0)
		{
		sprintf(entry,"User List \n");
		strcat(data,entry);
		//user list
		while(tmp!=NULL)
			{

			sprintf(entry,"Giocatore %d Id:%d \n",i,tmp->P.ID);
			strcat(data,entry);
			tmp=tmp->next;
			i++;
			}
		}
	else if (flag==1)
		{
		//user positions
		sprintf(entry,"User positions \n");
		strcat(data,entry);
		while(tmp!=NULL)
			{

			sprintf(entry,"Player %d position:%d,&d \n",i,tmp->P.position[0],tmp->P.position[1]);
			strcat(data,entry);
			tmp=tmp->next;
			i++;
			}
		return data;
		}
	else if (flag==2)
		{
		//user deaths
		}
	else sprintf(data," Error:invalid display function flag");
	return data;
}
//WIP
//it basically takes a list of players,and communicates with them for ever move that they make
//the width and height parameters are meant to be same height and width measurements for the board
//PlayerList needs to be compiled and liked to this file for this function to work properly
void ServerGame(int **board,PlayerList L,int width, int height)
{
	int session_status,eliminated=0;
	PlayerList tmp=L;
	PlayerList P,Dead=NULL;
	int **positions=create_position_map(width,height);
	int nextmove;
	char buf[BUFDIM],displaysize[DISPLAYSIGSIZE];
	char *displaybuf;
	positions=initPositions(L,board,positions,width,height);
	board=initBombs(positions,board,width,height);
	tmp=initPlayer(L,positions,height,width);
	while(session_status!=SESSION_END)
	{

		while(tmp!=NULL)
			{
				P=tmp;
				read(P->P.socket_desc,buf,SignalSize);
				nextmove=atoi(buf);
				switch(nextmove)
				{
					case NULL_MOVE:
						break;

					case MOVE_LEFT:
						if(CheckFree(P->P.position[0]-1,P->P.position[1],positions,width,height)  && CheckBomb(P->P.position,board)!=0)
							{
								P->P.position[0]--;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,SignalSize);
							}
						else if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,SignalSize);
								Dead=insert(Dead,P->P.socket_desc);
								tmp=eliminate(P->P.ID,L);
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,SignalSize);
								break;
							}

						break;

					case MOVE_RIGHT:
						if(CheckFree(P->P.position[0]+1,P->P.position[1],positions,width,height)&& CheckBomb(P->P.position,board)!=0)
							{
								P->P.position[0]++;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,SignalSize);
							}
						else if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,SignalSize);
								Dead=insert(Dead,P->P.socket_desc);
								tmp=eliminate(P->P.ID,L);
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,SignalSize);
								break;
							}
						break;

					case MOVE_UP:
						if(CheckFree(P->P.position[0],P->P.position[1]+1,positions,width,height)&& CheckBomb(P->P.position,board)!=0)
							{
								P->P.position[1]++;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,SignalSize);
							}
						else if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,SignalSize);
								Dead=insert(Dead,P->P.socket_desc);
								tmp=eliminate(P->P.ID,L);
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,SignalSize);
								break;
							}

						break;

					case MOVE_DOWN:
						if(CheckFree(P->P.position[0],P->P.position[1]-1,positions,width,height)&& CheckBomb(P->P.position,board)!=0)
							{
								P->P.position[1]--;
								positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
								sprintf(buf, "%d", MOVE_OK);
								write(P->P.socket_desc,buf,SignalSize);
							}
						else if(CheckBomb(P->P.position,board))
							{
								sprintf(buf, "%d", ELIMINATED);
								write(P->P.socket_desc,buf,SignalSize);
								Dead=insert(Dead,P->P.socket_desc);
								tmp=eliminate(P->P.ID,L);
								positions[P->P.position[0]][P->P.position[1]]=0;
								eliminated=1;
							}
						else
							{
								sprintf(buf, "%d", SQUARE_OCCUPIED);
								write(P->P.socket_desc,buf,SignalSize);
								break;
							}
						break;

					case QUIT:
						tmp=eliminate_disconnect(P->P.ID,L);
						eliminated=1;
						break;
				}
				if(eliminated==0)
				{
					read(P->P.socket_desc,buf,sizeof(int));
					nextmove=atoi(buf);
					switch(nextmove)
					{
						case DISPLAY_USERS:
							//we get a string with a variating size from this subroutine
							displaybuf=display(L,0,NULL,displaybuf);
							sprintf(displaysize,"%lu",strlen(displaybuf));
						// so we calculate it's size and send it back to the client along with the string itself
							write(P->P.socket_desc,displaysize,DisplaySignalSize);
							write(P->P.socket_desc,displaybuf,strlen(displaybuf));
							break;

						case DISPLAY_USER_LOCATIONS:
							displaybuf=display(L,0,NULL,displaybuf);
							sprintf(displaysize,"%lu",strlen(displaybuf));
							write(P->P.socket_desc,displaysize,DisplaySignalSize);
							write(P->P.socket_desc,displaybuf,strlen(displaybuf));
							break;

						case DISPLAY_USER_DEATHS:
							displaybuf=display(L,2,Dead,displaybuf);
							sprintf(displaysize,"%lu",strlen(displaybuf));
							write(P->P.socket_desc,displaysize,DisplaySignalSize);
							write(P->P.socket_desc,displaybuf,strlen(displaybuf));
							break;

						case NULL_MOVE:
							break;
					}

					tmp=tmp->next;
				}
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
    int pid;
    pthread_t tid;
    struct sockaddr_in servaddr, cli;
    FILE *db;
	db=fopen("login_credentials.db","r+");
	if(!db){
        printf("nessun utente registrato, creazione database\n");
        db=fopen("login_credentials.db","w+");
	}
	fclose(db);
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
    int seed[1];
    seed[0]=genseed();
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen fallito...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    while (1){

    len = sizeof(cli);
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server acccept fallito..\n");
        exit(0);
    }
    else{
        printf("server acccept avvenuto con sucesso...\n");
        struct thread_data thread_sd;
        thread_sd.connfd=connfd;
        thread_sd.seed=seed[0];
        //printf("%d %d\n",thread_sd[0],thread_sd[1]);
        pthread_create(&tid,NULL,sendseed,&thread_sd);
        }
    }
    int **board=create_board(seed[0]);
    close(sockfd);
    return 0;
}
