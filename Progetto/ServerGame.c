#include "lib.h"
//these subroutines handle the game itself
//this subroutine is used to create a data structure that is used to know whether a player is in a certain spot or not.
int **create_position_map(int dim)
{
    int x,y;
    int **positions=calloc(sizeof(int*),dim);
    int i=0;
    for(int i=0;i<dim;i++){
        positions[i]=calloc(sizeof(int),dim);
    }
	for(x=0;x<dim;x++){
		for(y=0;y<dim;y++)
						positions[x][y]=0;
        }
	return positions;
}

int CheckFree(int x,int y,int **position,int width,int height)
{
	if((x>=0 && y>=0) && (x<width && y<height))
			{
				if(position[x][y]==0) return 1;
			}
	else return 0;
}

int CheckBomb(int coord[2],int **map)
{

	if(map[coord[0]][coord[1]]==0)return 1;
	else return 0;
}
int CheckWin(PlayerList L,int height,int width)
{
	if(L->P.position[0]>=width-1) return 1;
	else return 0;
}
int **initPositions(int **board,int **positions,int dim,PlayerList P,int connfd)
{
	//assign starting positions to all players so that they can start the race
	//tell players their own starting postions
	int n_players=ListSize(P);
	if(n_players<dim--){
	srand(time(NULL));
	int nwrite;
	char buf[SIGSIZE];
	int x,y;
	x=0;
	y=n_players;
	positions[x][y]=P->P.ID;
	P->P.position[0]=x;
	P->P.position[1]=y;
	sprintf(buf, "%d", x);
	if ((nwrite=write(P->P.socket_desc,buf,SignalSize))<0)
		{
			perror("write 1:");
			exit(-1);
		}
	sprintf(buf, "%d", y);
	if((nwrite=write(P->P.socket_desc,buf,SignalSize))<0)
			{
				perror("write 2:");
				exit(-1);
			}
	board[x][y]=0;
	if(x<dim-1) board[x+1][y]=0;
	if(y<dim-1)board[x][y+1]=0;
	}
	else{
        close(connfd);
	}
	return positions;
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
		return data;
		}
	else if (flag==1)
		{
		//user positions
		sprintf(entry,"User positions \n");
		strcat(data,entry);
		while(tmp!=NULL)
			{
				sprintf(entry,"Player %d position:%d,%d \n",i,tmp->P.position[0],tmp->P.position[1]);
				strcat(data,entry);
				tmp=tmp->next;
				i++;
			}
		return data;
		}
	else if (flag==2)
		{
		sprintf(entry,"User deaths \n");
		strcat(data,entry);
		tmp=deaths;
		while(tmp!=NULL)
			{
				sprintf(entry,"Player %d, dead at position:%d,%d \n",i,tmp->P.position[0],tmp->P.position[1]);
				strcat(data,entry);
				tmp=tmp->next;
				i++;
			}
		return data;
		}
	else sprintf(data," Error:invalid display function flag");
	return data;
}
//WIP
//it basically takes a list of players,and communicates with them for ever move that they make
//the width and height parameters are meant to be same height and width measurements for the board
//PlayerList needs to be compiled and linked to this file for this function to work properly
//L is the PlayerList shared among ALL threads
//P is the PlayerList representing the player executing an instance of this subroutine in one of the threads
void ServerGame(int **board,int **positions,PlayerList L,int width,int height,PlayerList P,PlayerList Dead,int *GameTime)
{
	int session_status,eliminated=0,nread;
	PlayerList tmp=L;
	int nextmove;
	char buf[BUFDIM],displaysize[DISPLAYSIGSIZE];
	char *displaybuf;
	session_status=LOGIN_OK;
	while(session_status!=SESSION_END)
	{
		nread=read(P->P.socket_desc,buf,SignalSize);
		nextmove=atoi(buf);
		switch(nextmove)
			{
				case NULL_MOVE:
					sprintf(buf, "%d", LOGIN_OK);
					write(P->P.socket_desc,buf,SignalSize);
					break;

				case MOVE_LEFT:
					if(CheckFree(P->P.position[0]-1,P->P.position[1],positions,width,height)!=0  && CheckBomb(P->P.position,board)!=0)
						{
							positions[P->P.position[0]][P->P.position[1]]=0;
							P->P.position[0]--;
							positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
							if (CheckWin(P,height,width)==1)
								{
									sprintf(buf, "%d", WIN);
									write(P->P.socket_desc,buf,SignalSize);
									eliminated=1;
									*GameTime=0;
								}
							else
								{
									sprintf(buf, "%d", MOVE_OK);
									write(P->P.socket_desc,buf,SignalSize);
								}
							break;
						}
					else if(CheckBomb(P->P.position,board)==0)
						{
							sprintf(buf, "%d", ELIMINATED);
							write(P->P.socket_desc,buf,SignalSize);
							Dead=insert(Dead,P->P.socket_desc);
							positions[P->P.position[0]][P->P.position[1]]=0;
							tmp=eliminate(P->P.ID,L);
							eliminated=1;
							break;
						}
					else
						{
							sprintf(buf, "%d", SQUARE_OCCUPIED);
							write(P->P.socket_desc,buf,SignalSize);
							break;
						}
					break;

				case MOVE_RIGHT:
					if(CheckFree(P->P.position[0]+1,P->P.position[1],positions,width,height)!=0 && CheckBomb(P->P.position,board)!=0)
						{
							positions[P->P.position[0]][P->P.position[1]]=0;
							P->P.position[0]++;
							positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
							if (CheckWin(P,height,width)==1)
								{
									sprintf(buf, "%d", WIN);
									write(P->P.socket_desc,buf,SignalSize);
									eliminated=1;
									*GameTime=0;
								}
							else
								{
									sprintf(buf, "%d", MOVE_OK);
									write(P->P.socket_desc,buf,SignalSize);
								}
							break;
						}
					else if(CheckBomb(P->P.position,board)==0)
						{
							sprintf(buf, "%d", ELIMINATED);
							write(P->P.socket_desc,buf,SignalSize);
							Dead=insert(Dead,P->P.socket_desc);
							positions[P->P.position[0]][P->P.position[1]]=0;
							tmp=eliminate(P->P.ID,L);
							eliminated=1;
							break;
						}
					else
						{
							sprintf(buf, "%d", SQUARE_OCCUPIED);
							write(P->P.socket_desc,buf,SignalSize);
							break;
						}
					break;
				case MOVE_UP:
					if(CheckFree(P->P.position[0],P->P.position[1]+1,positions,width,height)!=0 && CheckBomb(P->P.position,board)!=0)
						{
							positions[P->P.position[0]][P->P.position[1]]=0;
							P->P.position[1]++;
							positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
							if (CheckWin(P,height,width)==1)
								{
									sprintf(buf, "%d", WIN);
									write(P->P.socket_desc,buf,SignalSize);
									eliminated=1;
									*GameTime=0;
								}
							else
								{
									sprintf(buf, "%d", MOVE_OK);
									write(P->P.socket_desc,buf,SignalSize);
								}
							break;
						}
					else if(CheckBomb(P->P.position,board)==0)
						{
							sprintf(buf, "%d", ELIMINATED);
							write(P->P.socket_desc,buf,SignalSize);
							Dead=insert(Dead,P->P.socket_desc);
							positions[P->P.position[0]][P->P.position[1]]=0;
							tmp=eliminate(P->P.ID,L);
							eliminated=1;
							break;
						}
					else
						{
							sprintf(buf, "%d", SQUARE_OCCUPIED);
							write(P->P.socket_desc,buf,SignalSize);
							break;
						}
					break;

				case MOVE_DOWN:
					if(CheckFree(P->P.position[0],P->P.position[1]-1,positions,width,height)!=0 && CheckBomb(P->P.position,board)!=0)
						{
							positions[P->P.position[0]][P->P.position[1]]=0;
							P->P.position[1]--;
							positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
							if (CheckWin(P,height,width)==1)
								{
									sprintf(buf, "%d", WIN);
									write(P->P.socket_desc,buf,SignalSize);
									eliminated=1;
									*GameTime=0;
								}
							else
								{
									sprintf(buf, "%d", MOVE_OK);
									write(P->P.socket_desc,buf,SignalSize);
								}
							break;
						}
					else if(CheckBomb(P->P.position,board)==0)
						{
							sprintf(buf, "%d", ELIMINATED);
							write(P->P.socket_desc,buf,SignalSize);
							Dead=insert(Dead,P->P.socket_desc);
							positions[P->P.position[0]][P->P.position[1]]=0;
							tmp=eliminate(P->P.ID,L);
							eliminated=1;
							break;
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
				default:
					sprintf(buf, "%d", LOGIN_OK);
					write(P->P.socket_desc,buf,SignalSize);
					break;
			}
		if(eliminated==0)
			{
				nread=read(P->P.socket_desc,buf,SignalSize);
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
						displaybuf=display(L,1,NULL,displaybuf);
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

					case DISPLAY_REMAINING_TIME:
						break;

					case NULL_MOVE:
						break;
				}
				*GameTime=(*GameTime)-1;
				//TODO writes that send an array of positions
				if (*GameTime<=0) session_status=SESSION_END;
			}
		else session_status=SESSION_END;
	}
}
