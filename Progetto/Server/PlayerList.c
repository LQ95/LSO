#include "lib_server.h"

struct P *create_player(int sockfd,char name[10],int dim){
    srand(time(NULL));
    struct P *out=malloc(sizeof(struct P));
    out->socket_desc=sockfd;
    out->position[0]=(rand()%dim);
    out->position[1]=0;
    out->score=0;
    strcpy(out->name,name);
    out->next=NULL;
    return out;
}

struct P *add_player(int sockfd,struct P *in,char name[10],int dim){
    struct P *out=create_player(sockfd,name,dim);
    out->next=in;
    return out;
}

struct P *disconnect(struct P *Players,int ID){
    pthread_mutex_lock(&sem);
    if(Players==NULL){
        pthread_mutex_unlock(&sem);
        return NULL;
    }
    else if(Players->next==NULL){
        free(Players);
        Players=NULL;
        pthread_mutex_unlock(&sem);
        return NULL;
    }
    else{
        if(Players->next->socket_desc==ID){
            struct P *tmp=Players->next->next;
            free(Players->next);
            Players->next=tmp;
            pthread_mutex_unlock(&sem);
            return Players;
        }
        pthread_mutex_unlock(&sem);
        Players->next=disconnect(Players->next,ID);
    }
    pthread_mutex_unlock(&sem);
    return Players;
}

struct P *search(int sockfd,struct P *list) //search function to make logging more accurate
{
	struct P *tmp=list;
	int found=0;
	while(tmp!=NULL &&found==0){
		if(tmp->socket_desc==sockfd) found=1;
		else tmp=tmp->next;
	}
	if(found==1) return tmp;
	else return NULL;
}


struct P *freeList(struct P *list)
{
if(list==NULL)
	return NULL;
list->next=freeList(list->next);
free(list);
list=NULL;
return list;
}

int list_size(struct P *list){
		int s=0;
		struct P *tmp=list;
		while(tmp!=NULL){
			tmp=tmp->next;
			s++;
		}
	return s;
}
void print_list(struct P *in){
    if(in!=NULL){
        printf("%d->",in->socket_desc);
        print_list(in->next);
    }
    else printf("\n");
    return;
}
