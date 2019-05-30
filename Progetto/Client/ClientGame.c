#include "lib_client.h"

void *client_game_send(void *arg){

}

void *client_game_recv(void *arg){
    printf("ciao");
    struct data *tmp=arg;
    int connfd=tmp->connfd;
    int seed[1];
    read(connfd,seed,sizeof(seed));
    printf("seed: %d",seed);
}
