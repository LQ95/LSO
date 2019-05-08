#include "lib_server.h"
//these subroutines handle the game itself
//this subroutine is used to create a data structure that is used to know whether a player is in a certain spot or not.
int **create_position_map(int dim){
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

int check_free(int x,int y,int **position,int dim){
	if((x>=0 && y>=0) && (x<dim && y<dim))
			{
				if(position[x][y]==0) return 1;    //check the position matrix,returns 1 if the position checked holds any value different than 0
			}
	else return 0;
}

int check_bomb(int coord[2],int **map){

	if(map[coord[0]][coord[1]]==0)return 1;  //returns 1 if thee is no bomb 0 if there is
	else return 0;
}

int check_win(player_list L,int dim){
	if(L->P.position[0]>=dim-1) return 1;   //returns 1 if the player is in the last column
	else return 0;
}

int **init_positions(int **board,int **positions,int dim,player_list P,int connfd){
	//assign a starting position to one player at a time so that he/she can start the race
	//write said starting position ,that is always on a random point in the first column,on the the player's socket
	srand(time(NULL));	
	int nwrite;
	char buf[SIGSIZE];
	int x,y;
	x=0;
	y=rand()%dim;
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
	if(x<dim-1) board[x+1][y]=0;   //if those cells aren not out of  bounds free them of bombs
	if(y<dim-1)board[x][y+1]=0;
	return positions;
}

char *display(player_list L,int flag,player_list deaths,char *data){
	data=malloc(sizeof(char)*BUFDIM);
	char entry[40];
	int i=1;
	player_list tmp=L;
	strcpy(data,"");
	if (flag==0){
		sprintf(entry,"User List \n");
		strcat(data,entry);
		//user list
		while(tmp!=NULL){
				sprintf(entry,"Giocatore %d Id:%d \n",i,tmp->P.ID);
				strcat(data,entry);
				tmp=tmp->next;
				i++;
			}
		return data;
		}
	else if (flag==1){
		//user positions
		sprintf(entry,"User positions \n");
		strcat(data,entry);
		while(tmp!=NULL){
				sprintf(entry,"Player %d position:%d,%d \n",i,tmp->P.position[0],tmp->P.position[1]);
				strcat(data,entry);
				tmp=tmp->next;
				i++;
			}
		return data;
		}
	else if (flag==2){
		//user deaths
		sprintf(entry,"User deaths \n");
		strcat(data,entry);
		tmp=deaths;
		while(tmp!=NULL){
				sprintf(entry,"Player %d, dead at position:%d,%d \n",i,tmp->P.position[0],tmp->P.position[1]);
				strcat(data,entry);
				tmp=tmp->next;
				i++;
			}
		else if (flag==3){
		//remaining time,incomplete
		sprintf(entry,"remaining time \n");
		strcat(data,entry);
		return data;
		}
	else sprintf(data," Error:invalid display function flag");
	return data;
}
//WIP
//it basically takes a list of players,and communicates with them for ever move that they make
//the width and height parameters are meant to be same height and width measurements for the board
//player_list needs to be compiled and linked to this file for this function to work properly
//L is the player_list shared among ALL threads
//P is the player_list representing the player executing an instance of this subroutine in one of the threads
void server_game(int **board,int **positions,player_list L,int dim,player_list P,player_list Dead,int *GameTime){
	int session_status,eliminated=0,nread;
	player_list tmp=L;
	time_t *EndTimestamp=malloc(sizeof(time_t));
	int nextmove;
	char buf[BUFDIM],displaysize[DISPLAYSIGSIZE],LogData[230];
	char *displaybuf;
	char PlayerPos[12],PlayerTime[60];
	session_status=LOGIN_OK;
	while(session_status!=SESSION_END){
		nread=read(P->P.socket_desc,buf,SignalSize); //read a code from the client 
		if(nread==0){
					strcpy(PlayerTime,ctime(EndTimestamp));
					sprintf(LogData,"giocatore %d disconnesso %s\n",P->P.ID,PlayerTime);
					server_log(LogData);
            	    close(P->P.socket_desc);
            	    return;
					}
		nextmove=atoi(buf); //convert the code into an int
		switch(nextmove){
            case NULL_MOVE: //no movement means you can continue on 
                sprintf(buf, "%d", LOGIN_OK);
                write(P->P.socket_desc,buf,SignalSize);
                break;
            case MOVE_LEFT: //one of the four codes for actual movement 
                if(check_free(P->P.position[0]-1,P->P.position[1],positions,dim)!=0  && check_bomb(P->P.position,board)!=0){ //if the position you want to move to is free and there are no bombs there,you move there
                        positions[P->P.position[0]][P->P.position[1]]=0; //free previously occupied position
                        P->P.position[0]--;//change position data in the list
                        positions[P->P.position[0]][P->P.position[1]]=P->P.ID; //occupy te new position in the position matrix
                        if (check_win(P,dim)==1){ //we check for a win right after movement that we know to be legitimate,if it is we log the win ,tell the player,and put the global game time to 0
                                sprintf(buf, "%d", WIN);
								strcpy(PlayerTime,ctime(EndTimestamp));
								sprintf(LogData,"giocatore %d ha vinto! x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime); 
								server_log(LogData);
                                write(P->P.socket_desc,buf,SignalSize);
                                eliminated=1;
                                *GameTime=0;
                        }
                        else{
                                sprintf(buf, "%d", MOVE_OK); //else,we jut tell the player that they actually moved
                                write(P->P.socket_desc,buf,SignalSize);
                            }
                        break;
                }
                else if(check_bomb(P->P.position,board)==0){ //if there was a bomb,we eliminate the player
                    sprintf(buf, "%d", ELIMINATED);
					strcpy(PlayerTime,ctime(EndTimestamp)); //log his/her death
					sprintf(LogData,"giocatore %d morto a x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
					server_log(LogData);
                    write(P->P.socket_desc,buf,SignalSize);//tell the player about his death
                    Dead=insert(Dead,P->P.socket_desc);//insert him into a list of deaths and eliminate him from the main list
                    positions[P->P.position[0]][P->P.position[1]]=0;//eliminate him from the position he occupied
                    tmp=eliminate(P->P.ID,L);
                    eliminated=1;
                    break;
                }
                else{
                    sprintf(buf, "%d", SQUARE_OCCUPIED); //else, the square is occupied
                    write(P->P.socket_desc,buf,SignalSize);
                    break;
                }           //the rest of the movement codes have the exact same logic as this but they obviously modify the position in a different way 
                break;
            case MOVE_RIGHT:
                if(check_free(P->P.position[0]+1,P->P.position[1],positions,dim)!=0 && check_bomb(P->P.position,board)!=0){
                    positions[P->P.position[0]][P->P.position[1]]=0;
                    P->P.position[0]++;
                    positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
                    if (check_win(P,dim)==1){
                        sprintf(buf, "%d", WIN);
						strcpy(PlayerTime,ctime(EndTimestamp));
						sprintf(LogData,"giocatore %d ha vinto! x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
						server_log(LogData);
                        write(P->P.socket_desc,buf,SignalSize);
                        eliminated=1;
                        *GameTime=0;
                    }
                    else{
                        sprintf(buf, "%d", MOVE_OK);
                        write(P->P.socket_desc,buf,SignalSize);
                    }
                break;
            }
            else if(check_bomb(P->P.position,board)==0){
                sprintf(buf, "%d", ELIMINATED);
				strcpy(PlayerTime,ctime(EndTimestamp));
				sprintf(LogData,"giocatore %d morto a x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
				server_log(LogData);
                write(P->P.socket_desc,buf,SignalSize);
                Dead=insert(Dead,P->P.socket_desc);
                positions[P->P.position[0]][P->P.position[1]]=0;
                tmp=eliminate(P->P.ID,L);
                eliminated=1;
                break;
            }
            else{
                sprintf(buf, "%d", SQUARE_OCCUPIED);
                write(P->P.socket_desc,buf,SignalSize);
                break;
            }
            break;
            case MOVE_UP:
                if(check_free(P->P.position[0],P->P.position[1]+1,positions,dim)!=0 && check_bomb(P->P.position,board)!=0){
                    positions[P->P.position[0]][P->P.position[1]]=0;
                    P->P.position[1]++;
                    positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
                    if (check_win(P,dim)==1){
                        sprintf(buf, "%d", WIN);
						strcpy(PlayerTime,ctime(EndTimestamp));
						sprintf(LogData,"giocatore %d ha vinto! x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
						server_log(LogData);
                        write(P->P.socket_desc,buf,SignalSize);
                        eliminated=1;
                        *GameTime=0;
                    }
                    else{
                        sprintf(buf, "%d", MOVE_OK);
                        write(P->P.socket_desc,buf,SignalSize);
                    }
                    break;
                }
                else if(check_bomb(P->P.position,board)==0){
                    sprintf(buf, "%d", ELIMINATED);
					strcpy(PlayerTime,ctime(EndTimestamp));
					sprintf(LogData,"giocatore %d morto a x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
					server_log(LogData);
                    write(P->P.socket_desc,buf,SignalSize);
                    Dead=insert(Dead,P->P.socket_desc);
                    positions[P->P.position[0]][P->P.position[1]]=0;
                    tmp=eliminate(P->P.ID,L);
                    eliminated=1;
                    break;
                }
                else{
                    sprintf(buf, "%d", SQUARE_OCCUPIED);
                    write(P->P.socket_desc,buf,SignalSize);
                    break;
                }
                break;
            case MOVE_DOWN:
                if(check_free(P->P.position[0],P->P.position[1]-1,positions,dim)!=0 && check_bomb(P->P.position,board)!=0){
                    positions[P->P.position[0]][P->P.position[1]]=0;
                    P->P.position[1]--;
                    positions[P->P.position[0]][P->P.position[1]]=P->P.ID;
                    if (check_win(P,dim)==1){
                        sprintf(buf, "%d", WIN);
						strcpy(PlayerTime,ctime(EndTimestamp));
						sprintf(LogData,"giocatore %d ha vinto! x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
						server_log(LogData);
                        write(P->P.socket_desc,buf,SignalSize);
                        eliminated=1;
                        *GameTime=0;
                    }
                    else{
                        sprintf(buf, "%d", MOVE_OK);
                        write(P->P.socket_desc,buf,SignalSize);
                    }
                    break;
                }
                else if(check_bomb(P->P.position,board)==0){
                    sprintf(buf, "%d", ELIMINATED);
					strcpy(PlayerTime,ctime(EndTimestamp));
					sprintf(LogData,"giocatore %d morto a x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
					server_log(LogData);
                    write(P->P.socket_desc,buf,SignalSize);
                    Dead=insert(Dead,P->P.socket_desc);
                    positions[P->P.position[0]][P->P.position[1]]=0;
                    tmp=eliminate(P->P.ID,L);
                    eliminated=1;
                    break;
                }
                else{
                    sprintf(buf, "%d", SQUARE_OCCUPIED);
                    write(P->P.socket_desc,buf,SignalSize);
                    break;
                }
                break;
            case QUIT:
				strcpy(PlayerTime,ctime(EndTimestamp));
				sprintf(LogData,"giocatore %d ha abbandonato la partita %s\n",P->P.ID,PlayerTime);
				server_log(LogData);
                tmp=eliminate_disconnect(P->P.ID,L);
                eliminated=1;
                break;
	    default:
                sprintf(buf, "%d", LOGIN_OK);
                write(P->P.socket_desc,buf,SignalSize);
                break;
        }
    if(eliminated==0){
        nread=read(P->P.socket_desc,buf,SignalSize);
        if(nread==0){
			strcpy(PlayerTime,ctime(EndTimestamp));
			sprintf(LogData,"giocatore %d disconnesso %s\n",P->P.ID,PlayerTime);
			server_log(LogData);
            close(P->P.socket_desc);
            return;
        }
        nextmove=atoi(buf);
        switch(nextmove){
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
			        displaybuf=display(L,3,Dead,displaybuf);
                    sprintf(displaysize,"%lu",strlen(displaybuf));
                    write(P->P.socket_desc,displaysize,DisplaySignalSize);
                    write(P->P.socket_desc,displaybuf,strlen(displaybuf));
                    break;
            case NULL_MOVE:
                    break;
        }
        *GameTime=(*GameTime)-1;
        //TODO writes that send an array of positions
	strcpy(buf, "");
	while(tmp!=NULL)
		{
			sprintf(PlayerPos,"%d:%d|",tmp->P.position[0],tmp->P.position[1]);
			strcat(buf,PlayerPos);
			tmp=tmp->next;
		}
	tmp=L;
	sprintf(displaysize,"%lu",strlen(buf));
	write(P->P.socket_desc,displaysize,DisplaySignalSize);
	write(P->P.socket_desc,buf,strlen(buf));
        if (*GameTime<=0) session_status=SESSION_END;
        }
        else session_status=SESSION_END;
	}
strcpy(PlayerTime,ctime(EndTimestamp));
sprintf(LogData,"giocatore %d posizione a fine sessione: x:%d y:%d %s\n",P->P.ID,P->P.position[0],P->P.position[1],PlayerTime);
server_log(LogData);
}
