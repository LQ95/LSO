#ifndef lib_int_list
#define lib_int_list
#include <stdio.h>
#include <stdlib.h>
typedef struct Node{
	struct Node *next;
	int info[2];
} ListNode;
typedef ListNode *IntList;
IntList add(IntList L,int *el);
IntList addHead(IntList L,int *el);
void Print(IntList L);
int IntSearch(IntList L,int *el);
#endif
