#include "lib_server.h"

player_list CreateList(int sd,int ID){
	srand(time(NULL));
	player_list L;
	int IDnumber;
	L=malloc(sizeof(PlayerNode));
	if(ID!=0)IDnumber=rand();
	else IDnumber=ID;
	L->P.ID=IDnumber;
	L->P.score=0;
	L->P.position[0]=0;
	L->P.position[1]=0;
	L->P.socket_desc=sd;
	L->next=NULL;
	return L;
}

//a player is inserted into the list right after connection the the server, so all we know and need is his/her socket descriptor
player_list insert(player_list L,int sd){
	srand(time(NULL));
	int IDnum=rand()%50000;
	player_list tmp=L;
	if(L!=NULL){
        while(tmp->next!=NULL )
			tmp=tmp->next;
		while(search(IDnum,L)!=NULL)
			IDnum=IDnum+111;
		tmp->next=CreateList(sd,IDnum);
		return L;
	}
	else L=CreateList(sd,IDnum);
	return L;
}

player_list insertHead(player_list L,int sd){
	srand(time(NULL));
	int IDnum=rand();
	while(search(IDnum,L)!=NULL)
		IDnum=rand();
	player_list tmp=CreateList(sd,IDnum);
	tmp->next=L;
	return tmp;
}

player_list eliminate(int ID,player_list L){
	if(L!=NULL){
		if(L->next!=NULL){
			player_list tmp;
			if(L->next->P.ID==ID){
                tmp=L->next;
                L->next=tmp->next;
                free(tmp);
                return L;
            }
			else return eliminate(ID,L->next);
		}
	}
	return L;
}

player_list eliminate_disconnect(int ID,player_list L){
	if(L!=NULL){
		if(L->next!=NULL){
			player_list tmp;
			if(L->next->P.ID==ID){
                tmp=L->next;
                L->next=tmp->next;
                close(L->P.socket_desc);
                free(tmp);
                return L;
            }
			else return eliminate(ID,L->next);
		}
	}
	return L;
}
player_list search(int IDnumber,player_list L){
	player_list tmp=L;
	int found=0;
	while(tmp!=NULL &&found==0){
		if(tmp->P.ID ==IDnumber) found=1;
		else tmp=tmp->next;
	}
	if(found) return tmp;
	else return NULL;
}

player_list search_by_SD(int sd,player_list L){
	player_list tmp=L;
	int found=0;
	while(tmp!=NULL &&found==0){
		if(tmp->P.socket_desc==sd) found=1;
		else tmp=tmp->next;
	}
	if(found==1) return tmp;
	else return NULL;
}

int list_size(player_list L){
		int s=0;
		player_list tmp=L;
		while(tmp!=NULL){
			tmp=tmp->next;
			s++;
		}
	return s;
}
void free_list(player_list L){
	player_list tmp=L;
	while(tmp->next!=NULL ){
		L=tmp;
		tmp=tmp->next;
		free(L);
	}
	free(tmp);
}
