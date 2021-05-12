#pragma once
#include "process.h"

#define PRIORITY_ALGORITHM 1

//Node
typedef struct element
{
	struct element * prev;
	struct element * next;
	Process * data;
	int priority;
}Node;

//알고리즘 별 우선순위 값을 찾아줌
int getPriority( Process * proc,int type )
{
	if( proc == NULL )
			return 0;

 	switch( type )
	{
	case 0:	//도착시간 순 
		return proc->arrivaltime;
	case 1:	//데드라인 적은 순(RM)
		return proc->deadline;
	}

	return 0;
}

//새 노드
Node * newNode(Process * proc, int pp)
{
	Node * newnode		= (Node*)malloc(sizeof(Node));
	newnode->prev 		= NULL;
	newnode->next 		= NULL;
	newnode->data 		= proc;
	newnode->priority	= getPriority(proc, pp); // RM scheduling	
	return newnode;
}


//Queue
typedef struct runqueue
{
	int size;
	Node * head;
	Node * tail;
	int pp; // priority policy
	Node * endnode; //더미노드
}Queue;

//새 큐
Queue * newDefaultQueue()
{
	Queue * newq = (Queue*)malloc(sizeof(Queue));
	newq->endnode = newNode(NULL, 0); //더미노드
	
	newq->size = 0;
	newq->head = newq->endnode;
	newq->tail = newq->endnode;
	newq->pp = 0;

	return newq;
}
Queue * newQueue(int pp)
{
	Queue * newq = newDefaultQueue();
	newq->pp = pp;

	return newq;
}

//큐에 삽입
void insertNode( Queue * queue, Node * node )
{
	//큐가 비었으면 
	if( queue->head == queue->endnode )
	{
		queue->head = node;
		queue->head->next = queue->tail;
		queue->tail->prev = queue->head;
	}
	//새로 들어온 노드가 제일 우선 순위가 높으면
	else if( node->priority <= queue->head->priority )
	{
		node->next = queue->head;
		queue->head->prev = node;
		queue->head = node;
	}
	//큐가 비어있지 않으면
	else
	{
		//순차 탐색
		Node * it = queue->head;
		for( it = queue->head ; it != queue->endnode ; it = it->next )
		{
			//순차 탐색 중 새 노드보다 우선순위가 낮은 기존의 노드 발견시
			if( node->priority < it->priority )
			{
				//기존 노드 앞에 삽입 함
				node -> prev = it->prev;
				node -> next = it;
				it -> prev->next = node;
				it -> prev = node;
				break;
			}
		}
		//우선순위가 더 낮은 기존 노드를 찾지 못했으면 맨뒤(더미노드 앞)에 삽입
		if( it == queue->endnode )
		{
			it = it->prev;
			it->next = node;
			node->prev = it;
			node->next = queue->tail;
			queue->tail->prev = node;
		}
			
	}

	queue->size++;
}
void insertNewNode( Queue * queue, Process * proc )
{
	insertNode(queue, newNode(proc, queue->pp));
}

//큐에서 맨 앞 노드 추출(삭제)
Node * popQueue( Queue * queue)
{
	//큐가 비었으면 
	if( queue->head <= queue->endnode )
			return NULL;

	Node * tempnode = queue->head;
	queue->head = queue->head->next;
	queue->head->prev = NULL;
	tempnode->prev = NULL;
	tempnode->next = NULL;

	queue->size--;

	return tempnode;
}
//모든 노드 출력 
void printQueue( Queue * queue )
{
	printf("size:%d\n", queue->size);
	printf("head:%p, tail:%p, end:%p\n", queue->head, queue->tail, queue->endnode);
	int index = 0;
	for( Node * it = queue->head ; it != queue->endnode ; it = it->next )
	{
		printf("%03d. this:%p, prev:%p, next:%p\n", index++,it ,it->prev, it->next);
	}
}
//모든 노드 내용 출력
void printNodeData( Queue * queue )
{
	Process * data;
	for( Node * it = queue->head ; it != queue->endnode ; it = it->next )
	{
		data = it -> data;
		printf("[%s]arr:%d, burst:%d, deadline:%d, remain:%d, state:%d\n",
			data->name, data->arrivaltime, data->bursttime, data->deadline, data->remaintime, data->state);
	}
}
