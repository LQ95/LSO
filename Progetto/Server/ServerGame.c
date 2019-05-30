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

void send_to_players(struct P *in,struct P *players,int mode){
    if(in!=NULL && players!=NULL){
            int tmp[1];
            tmp[0]=mode;
            if(write(players->socket_desc,tmp,sizeof(tmp))==0){
                return;
            }
            write(players->socket_desc,players->name,sizeof(players->name));
            tmp[0]=in->socket_desc;
            write(players->socket_desc,tmp,sizeof(tmp));
            write(players->socket_desc,in->position,sizeof (in->position));
            tmp[0]=in->score;
            write(players->socket_desc,tmp,sizeof(tmp));
            send_to_players(players->next,in,mode); //recursion happening here
    }
    return;
}

void server_game(char name[10],int sockfd,int time,struct player_list *players,struct player_list *deaths,int dim,int seed){
        //invio seed
        int send[1];
        int tmp[1];
	char LogEntry[100];
        tmp[0]=0;
        printf("seed: %d\n",seed);
        send[0]=seed;
        pthread_mutex_lock(&sem);
        write(sockfd,send,sizeof(send));
        struct P *current;
        players->first=add_player(sockfd,players->first,name,dim);
        print_list(players->first);
        current=players->first;
        //invio posizione iniziale
        write(sockfd,current->position,sizeof current->position);
        //invio altri giocatori
        send_players(players->first,sockfd);
        send_players(deaths->first,sockfd);
        pthread_mutex_unlock(&sem);
    while(time>0){
        if(read(sockfd,tmp,sizeof(tmp))==0){
            players->first=disconnect(players->first,current->socket_desc);
	    sprintf(LogEntry,"Un giocatore e' stato disconnesso ");
	    server_log(LogEntry);
            return;
           //pthread_exit(NULL);
        }
        if(read(sockfd,current->position,sizeof(current->position))==0){
            players->first=disconnect(players->first,current->socket_desc);
	    sprintf(LogEntry,"Un giocatore e' stato disconnesso ");
	    server_log(LogEntry);
            return;
            //pthread_exit(NULL);
        }
       send_to_players(players->first,current,tmp[0]);
        if(tmp[0]==2){
            //player is dead
            deaths->first=add_player(current->socket_desc,deaths->first,current->name,dim);
            deaths->first->position[0]=current->position[0];
            deaths->first->position[1]=current->position[1];
            players->first=disconnect(players->first,current->socket_desc);
        }
        //print_list(*players);
        //print_list(*deaths);
    }

}
