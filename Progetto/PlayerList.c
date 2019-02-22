#include "lib.h"

PlayerList CreateList()
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
	L->next=NULL;
}

PlayerList insert(Player P,PlayerList L)
{
	
}

PlayerList eliminate(int ID,PlayerList L)
{
	
}

Player search(int IDnumber,PlayerList L)
{
	PlayerList tmp=L
	int found=0;
	while(tmp!=NULL ù&&found==0)
	{
		if(tmp->ID ==IDnumber) found=1;
		tmp=tmp->next;
	}
	if(found) return tmp;
	else return NULL;
}

PlayerList FreeList(PlayerList L)
{
	
}