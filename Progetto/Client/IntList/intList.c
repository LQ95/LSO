#include "intlist.h"
IntList create(int *el)
{
	IntList L;
	L=malloc(sizeof(ListNode));
	L->info[0]=el[0]; 
	L->info[1]=el[1];
	L->next=NULL;
	return L;
}

IntList add(IntList L,int *el)
{
	if(L!=NULL && IntSearch(L,el)==0)
	{
		IntList tmp;
		tmp=L;
		while(tmp->next!=NULL)
			{
				tmp=tmp->next;
			}
	
		tmp->next=malloc(sizeof(ListNode));
		tmp->info[0]=el[0]; 
		tmp->info[1]=el[1];
		tmp->next->next=NULL;
	}
	else if(L==NULL) L=create(el);
	return L;
}

IntList addHead(IntList L,int *el)
{
	if(L!=NULL && IntSearch(L,el)==0)
	{
		IntList H;
		H=malloc(sizeof(ListNode));
		H->info[0]=el[0]; 
		H->info[1]=el[1];
		H->next=L;
		return H;
	}
	else return create(el);
}

int IntSearch(IntList L,int *el)
{
int found=0;
IntList tmp=L;
while(tmp!=NULL)
	{
		if(el[0]==tmp->info[0] && el[1]==tmp->info[1])
			 found=1;
		tmp=tmp->next;
	}
return found;
}

void PrintList(IntList L)
{
	IntList tmp;
	tmp=L;
	while(tmp!=NULL)
		{
			printf("x:%d y:%d ",tmp->info[0],tmp->info[1]);
			tmp=tmp->next;
		}
}
void EliminateElement(int *el,IntList L)
{
	IntList tmp,next;
	tmp=L;
while(tmp->next!=NULL)
	{
	if (el[0]==tmp->info[0] && el[1]==tmp->info[1])
		{
			next=tmp->next;
			tmp->next=tmp->next->next;
			free(el);
			free(next);
			return;
		}
	else tmp=tmp->next;
	}

}
void FreeList(IntList L)
{
	IntList tmp;
	tmp=L;
	while(tmp!=NULL)
		{
			EliminateElement(tmp->info,L);
			tmp=tmp->next;
		}
}
