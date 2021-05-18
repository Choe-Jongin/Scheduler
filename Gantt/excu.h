#pragma once

#include <stdlib.h>

typedef struct excution
{
    int start;
    int finish;
}Exc;

typedef struct excutionNode
{
    struct excutionNode * prev;
    struct excutionNode * next;
    Exc * data;
}ExcNode;

//새 노드
ExcNode * newExcNode(Exc * data)
{
	ExcNode * newnode	= (ExcNode*)malloc(sizeof(ExcNode));
	newnode->prev 		= NULL;
	newnode->next 		= NULL;
	newnode->data 		= data;
	return newnode;
}

typedef struct exlist
{
    int size;
    struct excutionNode * head;
    struct excutionNode * tail;
	struct excutionNode * endnode; //더미노드(항상 맨 뒤에 있음)
}Exclist;

//새 리스트
exlist * newExclist()
{
    exlist * newlist = (exlist*)malloc(sizeof(exlist));
    	newlist->endnode = newExcNode(NULL, 0); //더미노드 초기화 
	
	newlist->size = 0;
	newlist->head = newlist->endnode;
	newlist->tail = newlist->endnode;

	return newlist;
}
