#include "lib_client.h"
//ricevi lista di giocatori
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

//stampa lista di giocatori
void print_players(struct player *in){
    if(in!=NULL){
        printf("%s: x:%d y:%d->",in->name,in->position[0],in->position[1]);

        print_players(in->next);
    }
    else printf("fine\n");
    return;
}

//stampa la mappa
void print_board(int **board,int dim,int time,int status){
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
if(time>=0)
	{
	printw("\n %d secondi mancanti per la fine della gara",time);
	if (status==2) printw("\n sei morto \n");
	else printw("\n\n");
	refresh();
	}
}

//refresh the map with player information
int **refresh_board(struct player *in,struct player *deaths,int **board,int dim){
    int x,y;
     struct player *tmp=in;
     for(x=0;x<dim;x++)
     {
     	for(y=0;y<dim;y++)
     	{
        	board[x][y]=0;
	}
    }
    while(tmp!=NULL){ //itera attraverso i giocatori
        board[tmp->position[0]][tmp->position[1]]=1;
        tmp=tmp->next;
    }
    tmp=deaths;
    while(tmp!=NULL){//itera attraverso i morti
        board[tmp->position[0]][tmp->position[1]]=2;
        tmp=tmp->next;
    }
    return board;
}

//riempie la mappa con le posizioni dei giocatori
int **fill_board(struct player *in,struct player *deaths,int **board){
    if(in!=NULL){
        board[in->position[0]][in->position[1]]=1;
        board=fill_board(in->next,deaths,board);
    }
    else if(deaths!=NULL){
        board[deaths->position[0]][deaths->position[1]]=2;
        board=fill_board(in,deaths->next,board);
    }
    return board;
}

//crea la mappa
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
//Manda posizione al server
void send_position(int new_x,int new_y,int connfd){
        int positions_send[2];
        positions_send[0]=new_x;
        positions_send[1]=new_y;
        write(connfd,positions_send,sizeof(positions_send));
}

//controlla collisioni con altri giocatori
int check_playercollisions(int new_x,int new_y,int **board,int dim)
{
int pos;
if( (new_x!=dim && new_x>-1) && (new_y!=dim && new_y>-1) )
	{
		pos=board[new_x][new_y];
		if(pos!=0)
		return 1;
		else return 0;
	}
else return 0;
}

//controlla collisioni con le mine o se il giocatore ha vinto
int check_position(int connfd,int new_x,int new_y,int dim,int seed){
    int tmp[1];
    tmp[0]=0;
    if(new_y==dim-1){
        //vittoria
        clear();
        printw("HAI VINTO!");
	refresh();
	sleep(2);
        tmp[0]=3;
    }
    else if((seed*new_x*new_y)%5==2){
        //morto
        clear();
        printw("GAME OVER");
	refresh();
	sleep(2);
        tmp[0]=2;
    }
    else{
        tmp[0]=1;
    }
    return tmp[0];
}
//gestione input utene
void *client_game_send(void *arg){
    struct data *tmp=arg;
    int *positions=tmp->positions;
    int seed=tmp->seed;
    int dim=tmp->dimension;
    int connfd=tmp->connfd;
    int **board=tmp->board;
    //printw("dimension: %d seed: %d\n",dim,seed);
    int c;
    int *status=tmp->status;
    int tmpx;
    int tmpy;
    while(1){
        c=getch();
        switch(c){
            case KEY_UP:
		if(check_playercollisions((positions[0])+1,positions[1],board,dim)==0)
                positions[0]++;
                if(positions[0]==dim){
                        positions[0]--;
                    }
                else *status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_DOWN:
		if(check_playercollisions((positions[0])-1,positions[1],board,dim)==0)
                positions[0]--;
                if(positions[0]==-1){
                        positions[0]++;
                    }
                else *status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_LEFT:
		if(check_playercollisions(positions[0],(positions[1])-1,board,dim)==0)
                positions[1]--;
                if(positions[1]==-1){
                        positions[1]++;
                    }
                else *status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
            case KEY_RIGHT:
		if(check_playercollisions(positions[0],(positions[1])+1,board,dim)==0)
                positions[1]++;
                if(positions[1]==dim){
                        positions[1]--;
                    }
                else *status=check_position(connfd,positions[0],positions[1],dim,seed);
                break;
        }
        if(*status==2||*status==3){
            //endwin();
            pthread_exit(status);
        }
    }
}
//insert players into a list
struct player *insert(struct player *in, struct player *new_player,int **board){
    if(in!=NULL){
        if(in->ID==new_player->ID){//Se riceviamo un giocatore gia' conosciuto lo aggiorniamo
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
	{ //Altrimenti lo aggiungiamo
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
//aggiorna lista utenti e posizioni degli stessi
int receive_movement(struct player *in,int connfd,int **board){
    int id[1];
    int score[1];
    int status[1];
    int position[2];
    int size[1];
    int namelength[1];
    read(connfd,size,sizeof(size));//leggi quanto e' grande la lista
    struct player *out;
    out=malloc(sizeof(struct player));
    pthread_mutex_lock(&sem);
    read(connfd,status,sizeof(status));
    while(size[0]>0){
    read(connfd,out->name,sizeof(out->name));
    //printf("nome:%s\n",out->name);
    read(connfd,id,sizeof(id));
    //printf("id:%d\n",id[0]);
    read(connfd,position,sizeof(position));
    //printf("posizione:%d %d\n",position[0],position[1]);
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
    int time[1];
    time[0]=1;
    int *status=tmp->status;
    *status=1;
    int statusBuf[1];
    int positions_temporary[2];
    printw("read posizioni\n");
    refresh();
    read(connfd,positions_temporary,sizeof(positions_temporary));
    int *positions=tmp->positions;
    positions[0]=positions_temporary[0];
    positions[1]=positions_temporary[1];
    printw("read giocatori\n");
    refresh();
    struct player *other_players=receive_players(connfd);
    struct player *deaths=receive_players(connfd);
    //crea mappa
    int **board=tmp->board;
    printw("riempio mappa\n");
    refresh();
    fill_board(other_players,deaths,board);
    tmp->positions=positions;
    pthread_mutex_unlock(&sem);
    print_board(board,dim,-1,1);
    //ricevi e manda dati finche' sei in gioco
    while(status[0]!=2 && status[0]!=3 && time[0]>0){
	statusBuf[0]=status[0];
	write(connfd,statusBuf,sizeof(statusBuf));
	send_position(tmp->positions[0],tmp->positions[1],connfd);
	statusBuf[0]=receive_movement(other_players,connfd,board);
	statusBuf[0]=receive_movement(deaths,connfd,board);
	read(connfd,time,sizeof(time));
	print_players(other_players);
	board=refresh_board(other_players,deaths,board,dim);
	print_board(board,dim,time[0],status[0]);
	
    }
    statusBuf[0]=status[0];
    sleep(2);
    write(connfd,statusBuf,sizeof(statusBuf));
    clear();
    endwin();
    printf("Fine sessione\n");
    if(status[0]==2)
    printf("Sei morto.Aspetta la prossima sessione se sei interessato a continuare\n");
    else if(status[0]==3)
    printf("Hai vinto.adesso verrai portato alla prossima sessione\n");
    else
	{ 
		printf("tempo scaduto,adesso verrai portato alla prossima sessione");
		status[0]=4;	
	}
    pthread_mutex_lock(&sem);
    pthread_cond_broadcast(&c); //segnala la fine della sessione al thread principale
    pthread_mutex_unlock(&sem);
    pthread_exit(NULL);
}
