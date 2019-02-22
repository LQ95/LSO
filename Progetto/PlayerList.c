#include "lib.h"

PlayerList CreateList(int sd)
{
	srand(time(NULL));
	PlayerList L;
	int IDnumber;
	L=malloc(sizeof(Player*));
	IDnumber=rand();
	while(search(IDnumber)!=NULL)
	{
		IDnumber=rand();
	}
	L->ID=IDnumber;
	L->speed=0;
	L->score=0;
	L->position[0]=0;
	L->position[1]=0;
	L->socket_desc=sd;
	L->next=NULL;
	return L;
}

//a player is inserted into the list right after connection the the server, so all we know and need is his/her socket descriptor
PlayerList insert(PlayerList L,int sd)
{
	PlayerList tmp=L;
	while(tmp->next!=NULL )
	{
		tmp=tmp->next;
	}
	tmp->next=CreateList(sd);
	return L;
}

PlayerList insertHead(PlayerList L,int sd)
{
	PlayerList tmp=CreateList(int sd);
	tmp->next=L;
	return tmp;
	
}

PlayerList eliminate(int ID,PlayerList L)
{
	if(L!=NULL)
	{
		if(L->next!=NULL)
		{
			PlayerList tmp;
			if(L->next->ID==ID)
				{
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

PlayerList search(int IDnumber,PlayerList L)
{
	PlayerList tmp=L;
	int found=0;
	while(tmp!=NULL ù&&found==0)
	{
		if(tmp->ID ==IDnumber) found=1;
		tmp=tmp->next;
	}
	if(found) return tmp;
	else return NULL;
}

void FreeList(PlayerList L)
{
	PlayerList tmp=L;
	while(tmp->next!=NULL )
	{
		L=tmp;
		tmp=tmp->next;
		free(L);
	}
}