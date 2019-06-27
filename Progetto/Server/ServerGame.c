#include "lib_server.h"
void send_players(struct P *in,int connfd){
    //questo segnale indica quando i player son finiti, quando è 0 la recezione deve finire
    int other_players[1];
    other_players[0]=0;
    if(in!=NULL){
        other_players[0]=1;
        write(connfd,other_players,sizeof(other_players));
        write(connfd,in->name,sizeof (in->name));
        write(connfd,in->position,sizeof (in->position));
        int tmp[1];
        tmp[0]=in->score;
        write(connfd,tmp,sizeof(tmp));
        tmp[0]=in->socket_desc;
        write(connfd,tmp,sizeof(tmp));
        send_players(in->next,connfd);
    }
    else write(connfd,other_players,sizeof(other_players));
    return;
}
//Manda la lista completa ad un giocatore
void send_to_players(struct P *in,struct P *players,int mode){
    if(players!=NULL){
	    int size[1];
	    int length[1];
            size[0]=list_size(in);
	    write(players->socket_desc,size,sizeof(size));//manda grandezza lista
	    //printw("dimensione:%d\n",size[0]);
	    struct P *temp=in;
            int tmp[1];
            tmp[0]=mode;//manda lo stato del giocatore a cui e' asssociata questa istanza di servergame
            if(write(players->socket_desc,tmp,sizeof(tmp))==0){
                return;
            }
	    while(temp!=NULL){//inizia ad iterare sulla lista
            write(players->socket_desc,temp->name,sizeof(temp->name));
	    //printw("nome:%s\n",temp->name);
            tmp[0]=temp->socket_desc;
            write(players->socket_desc,tmp,sizeof(tmp));//socket descriptor,usato come id dall'altra parte
            write(players->socket_desc,temp->position,sizeof (temp->position));
	    //printw("posizione:%d %d\n",temp->position[0],temp->position[1]);
            tmp[0]=temp->score;
            write(players->socket_desc,tmp,sizeof(tmp));//score
	    temp=temp->next;
	}
    }
    return;
}
//subroutine principale servergame
int server_game(char name[10],int sockfd,int *time,struct player_list *players,struct player_list *deaths,int dim,int seed){
        int send[1];
        int tmp[1];
	int timebuf[1];
	char LogEntry[100];
        tmp[0]=0;//status
	//invio seed
        send[0]=seed;
        pthread_mutex_lock(&sem);
        write(sockfd,send,sizeof(send));
	//aggiungo il giocatore alla lista dei giocatori
        struct P *current;
        players->first=add_player(sockfd,players->first,name,dim);
        current=players->first;
        //invio posizione iniziale
        write(sockfd,current->position,sizeof(current->position));
        //invio lista giocatori
        send_players(players->first,sockfd);
        send_players(deaths->first,sockfd);
        pthread_mutex_unlock(&sem);
	timebuf[0]=*time;
    while(timebuf[0]>0){
        if(read(sockfd,tmp,sizeof(tmp))==0){//leggi status e gestisci la eventuale disconnessione
            players->first=disconnect(players->first,current->socket_desc);
	    sprintf(LogEntry,"giocatore:%s e' stato disconnesso",current->name);
	    server_log(LogEntry);
            return 4;
           //pthread_exit(NULL);
        }
	else if(tmp[0]==2){
            //il giocatore e' morto
	    sprintf(LogEntry,"giocatore:%s e' morto",current->name);
            deaths->first=add_player(current->socket_desc,deaths->first,current->name,dim);
            deaths->first->position[0]=current->position[0];
            deaths->first->position[1]=current->position[1];
            players->first=disconnect(players->first,current->socket_desc);
	    if(players->first==NULL) *time=1;
	    return tmp[0];
        }
        else if(tmp[0]==3){
            //il giocatore ha vinto
	    sprintf(LogEntry,"giocatore:%s ha vinto",current->name);
	    players->first=disconnect(players->first,current->socket_desc);
            *time=0;
	    return tmp[0];
        }
        if(read(sockfd,current->position,sizeof(current->position))==0){//leggi posizione 
            players->first=disconnect(players->first,current->socket_desc);
	    sprintf(LogEntry,"giocatore:%s e' stato disconnesso",current->name);
	    server_log(LogEntry);
            return 4;
            //pthread_exit(NULL);
        }
       send_to_players(players->first,current,tmp[0]);//mando lista giocatori
       send_to_players(deaths->first,current,tmp[0]);//mando lista morti
       timebuf[0]=*time;
       write(sockfd,timebuf,sizeof(timebuf)); //send remaining time
        //print_list(*deaths);
    }
    sprintf(LogEntry,"La sessione e' finita");
    server_log(LogEntry);
    return tmp[0];
}
