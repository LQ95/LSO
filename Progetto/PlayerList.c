#include "lib.h"

PlayerList CreateList(int sd,int ID)
{
	srand(time(NULL));
	PlayerList L;
	int IDnumber;
	L=malloc(sizeof(PlayerNode*));
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
PlayerList insert(PlayerList L,int sd)
{
	srand(time(NULL));
	int IDnum=rand();
	if(L!=NULL)
	{	
		PlayerList tmp=L;
		
		while(tmp->next!=NULL )
		{
			tmp=tmp->next;
		}
		while(search(IDnum,L)!=NULL)
		{
			IDnum=rand();
		}
		tmp->next=CreateList(sd,IDnum);
	}
	else L=CreateList(sd,IDnum);
	return L;
}

PlayerList insertHead(PlayerList L,int sd)
{
	srand(time(NULL));
	int IDnum=rand();
	while(search(IDnum,L)!=NULL)
	{
		IDnum=rand();
	}
	PlayerList tmp=CreateList(sd,IDnum);
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
			if(L->next->P.ID==ID)
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

PlayerList eliminate_disconnect(int ID,PlayerList L)
{
	if(L!=NULL)
	{
		if(L->next!=NULL)
		{
			PlayerList tmp;
			if(L->next->P.ID==ID)
				{
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
PlayerList search(int IDnumber,PlayerList L)
{
	PlayerList tmp=L;
	int found=0;
	while(tmp!=NULL &&found==0)
	{
		if(tmp->P.ID ==IDnumber) found=1;
		tmp=tmp->next;
	}
	if(found) return tmp;
	else return NULL;
}

int ListSize(PlayerList L)
{
		int s=0;
		PlayerList tmp=L;
		while(L!=NULL)
		{
			tmp=tmp->next;
			s++;
		}
	return s;
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
