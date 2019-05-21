#include "lib_server.h"

struct player_list *create_player(int sockfd,char name[10],int dim){
    srand(time(NULL));
    struct player_list *out=malloc(sizeof(struct player_list));
    out->socket_desc=sockfd;
    out->position[0]=0;
    out->position[1]=(rand()%dim);
    out->score=0;
    strcpy(out->name,name);
    out->next=NULL;
}

struct player_list *add_player(int sockfd,struct player_list *in,char name[10],int dim){
    struct player_list *out=create_player(sockfd,name,dim);
    out->next=in;
    return out;
}

struct player_list *disconnect(struct player_list *in){
    struct player_list *tmp=in->next;
    free(in);
    return tmp;
}
void print_list(struct player_list *in){
    if(in!=NULL){
        printf("%d->",in->socket_desc);
        print_list(in->next);
    }
    else printf("\n");
    return;
}
