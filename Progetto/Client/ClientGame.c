#include "lib_client.h"
//receive the list of players
struct player *receive_players(int connfd){
    int other_players[1];
    struct player *out=NULL;
    read(connfd,other_players,sizeof(other_players));
    if(other_players[0]){
        out=malloc(sizeof(struct player));
        read(connfd,out->name,sizeof(out->name));
        read(connfd,out->position,sizeof(out->position));
        int tmp[1];
        read(connfd,tmp,sizeof(tmp));
        out->score=tmp[0];
        read(connfd,tmp,sizeof(tmp));
        out->ID=tmp[0];
        out->next=receive_players(connfd);
    }
    return out;
}
//print the list of players
void print_players(struct player *in){
    if(in!=NULL){
        printf("%s: %d %d->",in->name,in->position[0],in->position[1]);

        print_players(in->next);
    }
    else printf("fine\n");
    return;
}
//print the game map
void print_board(int **board,int dim){
    clear();
    int x=dim-1;
    int y=0;
    for(x=dim-1;x>-1;x--){
        for(y=0;y<dim;y++){
            if(board[x][y]==1){
                 printw("|");
                 addch(ACS_BLOCK);
            }
            else if(board[x][y]==2)
                printw("|X");
            else printw("| ");
        }
        printw("|\n");
        refresh();
    }
}
//fill the map with player information
int **fill_board(struct player *in,struct player *deaths,int **board){
    if(in!=NULL){
        board[in->position[0]][in->position[1]]=1;
        board=fill_board(in->next,deaths,board);
    }
    else if(deaths!=NULL){
        board[deaths->position[1]][deaths->position[0]]=2;
        board=fill_board(in,deaths->next,board);
    }
    return board;
}
//create the map
int **create_board(struct player *in,struct player *deaths,int dim){
    int **out;
    out=malloc(sizeof(int*)*dim);
    int x;
    for(x=0;x<dim;x++){
        out[x]=calloc(dim,sizeof(int));
    }
    out=fill_board(in,deaths,out);
    return out;
}
//send the position to the server
void send_position(int new_x,int new_y,int connfd){
        int positions_send[2];
        positions_send[0]=new_x;
        positions_send[1]=new_y;
        write(connfd,positions_send,sizeof(positions_send));
}

int check_playercollisions(int new_x,int new_y,int **board)
{
if(board[new_x][new_y]=1)
return 1;
else return 0;
}

//check for collisions with bombs
int check_position(int connfd,int new_x,int new_y,int dim,int seed){
    int tmp[1];
    tmp[0]=0;
    if(new_y==dim-1){
        //vittoria
        clear();
        printw("HAI VINTO!");
        tmp[0]=3;
    }
    else if((seed*new_x*new_y)%5==2){
        //morto
        clear();
        printw("GAME OVER");
        tmp[0]=2;
    }
    else{
        tmp[0]=1;
    }
    write(connfd,tmp,sizeof(tmp));
    send_position(new_x,new_y,connfd);
    return tmp[0];
}
//respond to user input and send it to server?
void *client_game_send(void *arg){
    struct data *tmp=arg;
    int *positions=tmp->positions;
    int seed=tmp->seed;
    int dim=tmp->dimension;
    int connfd=tmp->connfd;
    int **board=tmp->board;
    printw("dimension: %d\n",dim);
    int c;
    int status=0;
    int tmpx;
    int tmpy;
    while(1){
        c=getch();
        switch(c){
            case KEY_UP:
                positions[0]++;
                if(positions[0]==dim){
                        positions[0]--;
                    }
                else status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_DOWN:
                positions[0]--;
                if(positions[0]==-1){
                        positions[0]++;
                    }
                else status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_LEFT:
                positions[1]--;
                if(positions[1]==-1){
                        positions[1]++;
                    }
                else status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_RIGHT:
                positions[1]++;
                if(positions[1]==dim){
                        positions[1]--;
                    }
                else status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
        }
        if(status==2||status==3){
            endwin();
            pthread_exit(NULL);
        }
    }
}
//insert players into a list
struct player *insert(struct player *in, struct player *new_player,int **board){
    if(in!=NULL){
        if(in->ID==new_player->ID){//if we pass a playe that we already know we just update him/her
            board[in->position[0]][in->position[1]]=0;
            board[new_player->position[0]][new_player->position[1]]=1;
            in->score=new_player->score;
            in->position[0]=new_player->position[0];
            in->position[1]=new_player->position[1];
            }
            else {
		in->next=insert(in->next,new_player,board);
		return in;		
		}
    }
    else
	{ //otherwise we add him to the list
	in=malloc(sizeof(struct player));
	board[new_player->position[0]][new_player->position[1]]=1;
	in->score=new_player->score;
        in->position[0]=new_player->position[0];
        in->position[1]=new_player->position[1];
	in->ID=new_player->ID;
	in->next=NULL;
	return in;
	}
return in;
}
//receive info about other users' movements(but the list is not passed correctly)
int receive_movement(struct player *in,int connfd,int **board){
    int id[1];
    int score[1];
    int status[1];
    int position[2];
    int size[1];
    int namelength[1];
    read(connfd,size,sizeof(size));//read list size from the other side and iterate on it
    struct player *out;
    out=malloc(sizeof(struct player));
    pthread_mutex_lock(&sem);
    read(connfd,status,sizeof(status));
    while(size[0]>0){
    read(connfd,out->name,sizeof(out->name));
    printf("nome:%s\n",out->name);
    read(connfd,id,sizeof(id));
    read(connfd,position,sizeof(position));
    read(connfd,score,sizeof(score));
    out->ID=id[0];
    out->score=score[0];
    out->position[0]=position[0];
    out->position[1]=position[1];
    in=insert(in,out,board);
    size[0]--;
    }
    pthread_mutex_unlock(&sem);
    return status[0];
}
//receiving thread
void *client_game_recv(void *arg){
    initscr();
    pthread_mutex_lock(&sem);
    struct data *tmp=arg;
    int connfd=tmp->connfd;
    int dim=tmp->dimension;
    //riceve seed
    int seed[1];
    read(connfd,seed,sizeof(seed));
    //riceve posizione iniziale
    int positions_temporary[2];
    read(connfd,positions_temporary,sizeof(positions_temporary));
    int *positions=tmp->positions;
    positions[0]=positions_temporary[0];
    positions[1]=positions_temporary[1];
    printf("x=%d y=%d\n",positions[0],positions[1]);
    struct player *other_players=receive_players(connfd);
    struct player *deaths=receive_players(connfd);
    print_players(deaths);
    //create board
    int **board=create_board(other_players,deaths,dim);
    tmp->seed=seed[0];
    tmp->positions=positions;
    pthread_mutex_unlock(&sem);
    print_board(board,dim);
    //receive data of the movements
    int status=0;
    while(status!=2||status!=3){
        status=receive_movement(other_players,connfd,board);
	print_players(other_players);
        print_board(board,dim);
    }

    endwin();
    //printf("seed: %d\n",seed[0]);
    //riceve dati degli altri giocatori
}
