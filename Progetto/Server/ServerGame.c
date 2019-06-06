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
//send the current player's name and position to the full list to players
void send_to_players(struct P *in,struct P *players,int mode){
    if(in!=NULL && players!=NULL){
	    int size[1];
	    int length[1];
            size[0]=list_size(in);
	    write(players->socket_desc,size,sizeof(size));//tell the other side what size the list is
	    printf("dimensione:%d\n",size[0]);
	    struct P *temp=in;
            int tmp[1];
            tmp[0]=mode;
            if(write(players->socket_desc,tmp,sizeof(tmp))==0){
                return;
            }
	    while(temp!=NULL){//iteration on the list starts from here
            write(players->socket_desc,temp->name,sizeof(temp->name));
	    printf("nome:%s\n",temp->name);
            tmp[0]=temp->socket_desc;
            write(players->socket_desc,tmp,sizeof(tmp));//socket descriptor,used as id on the other side
            write(players->socket_desc,temp->position,sizeof (temp->position));
	    printf("posizione:%d %d\n",temp->position[0],temp->position[1]);
            tmp[0]=temp->score;
            write(players->socket_desc,tmp,sizeof(tmp));//score
            //send_to_players(players->next,in,mode); //recursion happening here
	    temp=temp->next;
	}
    }
    return;
}
//main server game thread
void server_game(char name[10],int sockfd,int time,struct player_list *players,struct player_list *deaths,int dim,int seed){
        //invio seed
        int send[1];
        int tmp[1];
	char LogEntry[100];
        tmp[0]=0;//status?
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
	    sprintf(LogEntry,"giocatore:%s e' stato disconnesso",current->name);
	    server_log(LogEntry);
            return;
           //pthread_exit(NULL);
        }
        if(read(sockfd,current->position,sizeof(current->position))==0){//position is read here
            players->first=disconnect(players->first,current->socket_desc);
	    sprintf(LogEntry,"giocatore:%s e' stato disconnesso",current->name);
	    server_log(LogEntry);
            return;
            //pthread_exit(NULL);
        }
       send_to_players(players->first,current,tmp[0]);
        if(tmp[0]==2){
            //player is dead
	    sprintf(LogEntry,"giocatore:%s e' morto",current->name);
            deaths->first=add_player(current->socket_desc,deaths->first,current->name,dim);
            deaths->first->position[0]=current->position[0];
            deaths->first->position[1]=current->position[1];
            players->first=disconnect(players->first,current->socket_desc);
        }
        //print_list(*deaths);
	time--;
    }
    sprintf(LogEntry,"La sessione e' finita");
    server_log(LogEntry);
}
