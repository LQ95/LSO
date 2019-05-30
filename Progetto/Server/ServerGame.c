#include "lib_server.h"

void server_game(char name[10],int sockfd,int time,struct player_list **players,struct player_list **deaths,int dim,int seed){
        int send[1];
        write(sockfd,send,sizeof(send));
        *players=add_player(sockfd,*players,name,dim);
        print_list(*players);
    while(time>0){

    }
}
